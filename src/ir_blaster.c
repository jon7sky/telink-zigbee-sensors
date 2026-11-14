/*
 * ir_blaster.c
 *
 * IR learn / send with the codes stored on the device.
 *
 * A code is a list of raw mark/space durations, so any protocol and any
 * length up to IR_MAX_TIMINGS works. Codes are stored on the device by name
 * (ir_store.c) and never cross the radio: the
 * hub sends a name and gets a result. A list command reports every stored
 * name, so the device holds the only list of codes.
 */
#include "tl_common.h"
#include "zb_api.h"
#include "zcl_include.h"
#include "bdb.h"
#include "app_main.h"
#include "app_ui.h"
#include "ir_blaster.h"
#include "ir_store.h"

#if USE_IR_BLASTER

#define IR_MAX_TIMINGS			2000

/* Timings are stored in units of 4 us: 64 ticks of the 16 MHz system timer */
#define IR_UNIT_SHIFT			6
#define IR_MIN_TIMINGS			8					// shorter captures are noise
#define IR_END_GAP_TICKS		(120 * 1000 * 16)	// 120 ms of silence ends a code
#define IR_IDLE_TICKS			(100 * 1000 * 16)	// line must be quiet before arming
#define IR_LEARN_TIMEOUT_MS		15000
#define IR_CMD_DEFER_TICKS		(300 * 1000 * 16)	// max wait for the stack to go idle

/* Carrier 38 kHz, duty 1/2 (as the stock firmware) */
#define IR_PWM_MAX				(CLOCK_SYS_CLOCK_HZ / 38000)
#define IR_PWM_CMP				(IR_PWM_MAX / 2)

/* Polling: fast for a while after a command, slower when idle */
#define IR_POLL_ACTIVE_MS		250
#define IR_POLL_ACTIVE_WINDOW_MS (15 * 1000)
#define IR_POLL_IDLE_DEF		4					// 1 s, in 1/4 s
#define IR_POLL_IDLE_MAX		40					// 10 s

enum {
	IR_ST_IDLE = 0,
	IR_ST_LEARN_SETTLE,		// receiver powered, waiting for a quiet line
	IR_ST_LEARN_WAIT,		// GPIO interrupt armed
	IR_ST_CAPTURED			// set by the interrupt
};

enum {
	IR_CMD_NONE = 0,
	IR_CMD_LEARN,
	IR_CMD_RENAME,
	IR_CMD_LIST,
	IR_CMD_SEND,
	IR_CMD_ERASE,
	IR_CMD_ERASE_ALL,
	IR_CMD_CANCEL
};

#define IR_CAP_OVERFLOW			1
#define IR_CAP_STUCK			2

typedef struct {
	volatile u8  state;
	u8  cmd;
	u8  storage_ok;
	u8  listing;			// sending the code list
	volatile u8  cap_flags;
	volatile u16 cap_count;
	u32 tick;				// state / command time stamp
	char cmd_name[1 + IR_NAME_MAX];		// names are kept as C strings
	char cmd_name2[1 + IR_NAME_MAX];	// rename: the new name
	char learn_name[1 + IR_NAME_MAX];	// learn in progress
	ev_timer_event_t *timerLearnEvt;
	ev_timer_event_t *timerFastPollEvt;
} ir_ctx_t;

static ir_ctx_t ir;
static u16 ir_buf[IR_MAX_TIMINGS];	// capture, playback and rename buffer

zcl_irAttr_t g_zcl_irAttrs = {
	.code_count = 0,
	.poll_idle = IR_POLL_IDLE_DEF
};

const zclAttrInfo_t ir_attrTbl[ZCL_IR_ATTR_NUM] = {
	{ ZCL_ATTRID_IR_POLL_IDLE,	ZCL_DATA_TYPE_UINT8,	ACCESS_CONTROL_READ | ACCESS_CONTROL_WRITE,	(u8*)&g_zcl_irAttrs.poll_idle },
	{ ZCL_ATTRID_IR_CODE_COUNT,	ZCL_DATA_TYPE_UINT16,	ACCESS_CONTROL_READ,						(u8*)&g_zcl_irAttrs.code_count },
	{ ZCL_ATTRID_GLOBAL_CLUSTER_REVISION, ZCL_DATA_TYPE_UINT16, ACCESS_CONTROL_READ,				(u8*)&zcl_attr_global_clusterRevision },
};

static status_t ir_zclCmdHandler(zclIncoming_t *pInMsg);

_CODE_ZCL_ status_t zcl_ir_blaster_register(u8 endpoint, u16 manuCode, u8 attrNum, const zclAttrInfo_t attrTbl[], cluster_forAppCb_t cb)
{
	return zcl_registerCluster(endpoint, ZCL_CLUSTER_IR_BLASTER, manuCode, attrNum, attrTbl, ir_zclCmdHandler, cb);
}

/*******************************************************************
 * Polling
 */
static s32 ir_fastPollEndCb(void *arg) {
	ir.timerFastPollEvt = NULL;
	app_pollRateApply();
	return -1;
}

/* Poll quickly for a while: commands tend to come in groups */
static void ir_fast_poll(void) {
	if(ir.timerFastPollEvt)
		TL_ZB_TIMER_CANCEL(&ir.timerFastPollEvt);
	ir.timerFastPollEvt = TL_ZB_TIMER_SCHEDULE(ir_fastPollEndCb, NULL, IR_POLL_ACTIVE_WINDOW_MS);
	app_pollRateApply();
}

u32 ir_poll_rate_ms(void) {
	if(ir.timerFastPollEvt)
		return IR_POLL_ACTIVE_MS;
	return g_zcl_irAttrs.poll_idle * POLL_RATE_QUARTERSECONDS;
}

/*******************************************************************
 * Messages to the coordinator (no binding needed)
 */
static void ir_send_to_hub(u8 cmdId, u8 *buf, u8 len) {
	epInfo_t dstEpInfo;
	if(!zb_isDeviceJoinedNwk())
		return;
	TL_SETSTRUCTCONTENT(dstEpInfo, 0);
	dstEpInfo.profileId = HA_PROFILE_ID;
	dstEpInfo.dstAddrMode = APS_SHORT_DSTADDR_WITHEP;
	dstEpInfo.dstAddr.shortAddr = 0x0000;
	dstEpInfo.dstEp = 1;
	zcl_sendCmd(SENSOR_DEVICE_ENDPOINT, &dstEpInfo, ZCL_CLUSTER_IR_BLASTER, cmdId,
			TRUE, ZCL_FRAME_SERVER_CLIENT_DIR, TRUE, MANUFACTURER_CODE_NONE, ZCL_SEQ_NUM, len, buf);
}

/* [timings u16][name], or with a status byte in front */
static u8 ir_pack(u8 *buf, u16 count, const char *name) {
	u8 n = strlen(name);
	buf[0] = count & 0xff;
	buf[1] = count >> 8;
	buf[2] = n;
	memcpy(&buf[3], name, n);
	return 3 + n;
}

static void ir_result(u8 status, u16 count, const char *name) {
	u8 buf[4 + IR_NAME_MAX];
	buf[0] = status;
	ir_send_to_hub(IR_ZCL_CMD_RESULT, buf, 1 + ir_pack(&buf[1], count, name));
}

static void ir_code_info(u16 count, const char *name) {
	u8 buf[3 + IR_NAME_MAX];
	ir_send_to_hub(IR_ZCL_CMD_CODE_INFO, buf, ir_pack(buf, count, name));
}

/* After any change to the store */
static void ir_store_changed(void) {
	g_zcl_irAttrs.code_count = ir_store_count();
	if(ir.listing)
		ir_store_iter_start();	// the iterator may be stale: list again (repeats are harmless)
}

/*******************************************************************
 * Send
 */
_attribute_ram_code_ static void ir_play(u16 *p, u32 n) {
	u32 t = clock_time();
	for(u32 i = 0; i < n; i++) {
		if(i & 1)
			pwm_stop(PWM0_ID);	// space
		else
			pwm_start(PWM0_ID);	// mark
		t += (u32)p[i] << IR_UNIT_SHIFT;
		while((u32)(clock_time() - t) > BIT(31));
	}
	pwm_stop(PWM0_ID);
}

static void ir_send(const char *name) {
	u32 r;
	u16 count = ir_store_get(name, ir_buf, IR_MAX_TIMINGS);
	if(!count) {
		ir_result(IR_STA_NOT_FOUND, 0, name);
		return;
	}
	light_on();
	reg_clk_en0 |= FLD_CLK0_PWM_EN;
	reg_pwm_clk = 0;	// PWM clock = system clock
	pwm_set_mode(PWM0_ID, PWM_NORMAL_MODE);
	pwm_set_cycle_and_duty(PWM0_ID, IR_PWM_MAX, IR_PWM_CMP);
	gpio_set_func(GPIO_IR_TX, AS_PWM0);
	r = drv_disable_irq();
	ir_play(ir_buf, count);
	drv_restore_irq(r);
	gpio_write(GPIO_IR_TX, 0);
	gpio_set_func(GPIO_IR_TX, AS_GPIO);
	gpio_set_output_en(GPIO_IR_TX, 1);
	reg_clk_en0 &= ~FLD_CLK0_PWM_EN;
	light_off();
	ir_result(IR_STA_SEND_OK, count, name);
}

/*******************************************************************
 * Learn
 */
/* GPIO interrupt on the first falling edge: records the whole code */
_attribute_ram_code_ static void ir_rx_irq(void) {
	u32 t0 = clock_time();
	u32 dt;
	u32 n = 0;
	u8 flags = 0;
	u8 mark = 1;	// the receiver output is low during a mark
	gpio_en_interrupt(GPIO_IR_RX, 0);
	if(ir.state != IR_ST_LEARN_WAIT)
		return;
	while(1) {
		do {
			dt = clock_time() - t0;
			if(dt > IR_END_GAP_TICKS)
				goto end;
		} while((gpio_read(GPIO_IR_RX) == 0) == mark);
		if(n >= IR_MAX_TIMINGS) {
			flags = IR_CAP_OVERFLOW;
			goto end;
		}
		ir_buf[n++] = dt >> IR_UNIT_SHIFT;
		t0 += dt;
		mark ^= 1;
	}
end:
	if(mark)	// the line stays low: not an IR code
		flags |= IR_CAP_STUCK;
	ir.cap_count = n;
	ir.cap_flags = flags;
	ir.state = IR_ST_CAPTURED;
}

static void ir_rx_arm(void) {
	ir.state = IR_ST_LEARN_WAIT;
	drv_gpio_irq_config(GPIO_IRQ_MODE, GPIO_IR_RX, GPIO_FALLING_EDGE, ir_rx_irq);
	drv_gpio_irq_en(GPIO_IR_RX);
}

static void ir_rx_off(void) {
	gpio_en_interrupt(GPIO_IR_RX, 0);
	gpio_set_input_en(GPIO_IR_RX, 0);
	gpio_setup_up_down_resistor(GPIO_IR_RX, PM_PIN_PULLDOWN_100K);
	gpio_write(GPIO_IR_RX_POWER, 0);
}

static void ir_learn_end(u8 status, u16 count) {
	ir.state = IR_ST_IDLE;
	ir_rx_off();
	if(ir.timerLearnEvt)
		TL_ZB_TIMER_CANCEL(&ir.timerLearnEvt);
	// Let the blink timer end by itself at its next "off" step. Cancelling it here
	// is not reliable: after a long capture (interrupts off) the timer is overdue
	// and keeps running, and with times = 0 it would then wrap to 255 more blinks.
	g_sensorAppCtx.times = 1;
	light_off();
	ir_result(status, count, ir.learn_name);
	ir.learn_name[0] = 0;
}

static s32 ir_learnTimeoutCb(void *arg) {
	ir.timerLearnEvt = NULL;
	if(ir.state != IR_ST_IDLE)
		ir_learn_end(IR_STA_LEARN_TIMEOUT, 0);
	return -1;
}

static void ir_learn_start(const char *name) {
	if(ir.state != IR_ST_IDLE)
		ir_learn_end(IR_STA_CANCELLED, 0);
	strcpy(ir.learn_name, name);
	gpio_write(GPIO_IR_RX_POWER, 1);
	gpio_set_output_en(GPIO_IR_RX_POWER, 1);
	gpio_set_func(GPIO_IR_RX, AS_GPIO);
	gpio_set_output_en(GPIO_IR_RX, 0);
	gpio_setup_up_down_resistor(GPIO_IR_RX, PM_PIN_PULLUP_1M);
	gpio_set_input_en(GPIO_IR_RX, 1);
	ir.tick = clock_time();
	ir.state = IR_ST_LEARN_SETTLE;
	ir.timerLearnEvt = TL_ZB_TIMER_SCHEDULE(ir_learnTimeoutCb, NULL, IR_LEARN_TIMEOUT_MS);
	light_blink_start(IR_LEARN_TIMEOUT_MS / 500, 250, 250);
	ir_result(IR_STA_LEARNING, 0, name);
}

static void ir_learn_captured(void) {
	u16 count = ir.cap_count;
	if(ir.cap_flags & IR_CAP_OVERFLOW) {
		ir_learn_end(IR_STA_LEARN_TOO_LONG, count);
	} else if((ir.cap_flags & IR_CAP_STUCK) || count < IR_MIN_TIMINGS) {
		// noise: wait for a quiet line and listen again
		ir.tick = clock_time();
		ir.state = IR_ST_LEARN_SETTLE;
	} else {
		// An existing code with this name is replaced only now, after a good capture
		int r = ir_store_set(ir.learn_name, ir_buf, count);
		ir_store_changed();
		if(r == IR_STORE_OK)
			ir_learn_end(IR_STA_LEARN_OK, count);
		else
			ir_learn_end(r == IR_STORE_FULL ? IR_STA_STORE_FULL : IR_STA_FLASH_ERROR, count);
	}
}

/*******************************************************************
 * Rename, erase
 */
static void ir_rename(const char *old_name, const char *new_name) {
	u16 count = ir_store_get(old_name, ir_buf, IR_MAX_TIMINGS);
	int r;
	if(!count) {
		ir_result(IR_STA_NOT_FOUND, 0, old_name);
		return;
	}
	if(strcmp(old_name, new_name) == 0) {
		ir_result(IR_STA_RENAMED, count, new_name);
		return;
	}
	if(ir_store_size(new_name)) {
		ir_result(IR_STA_NAME_EXISTS, 0, new_name);
		return;
	}
	// write the new name first: a power loss in between leaves both, never neither
	r = ir_store_set(new_name, ir_buf, count);
	if(r == IR_STORE_OK)
		r = ir_store_del(old_name);
	ir_store_changed();
	if(r == IR_STORE_OK)
		ir_result(IR_STA_RENAMED, count, new_name);
	else
		ir_result(r == IR_STORE_FULL ? IR_STA_STORE_FULL : IR_STA_FLASH_ERROR, count, new_name);
}

static void ir_erase(const char *name) {
	u16 count = ir_store_size(name);
	int r = ir_store_del(name);
	if(r == IR_STORE_OK) {
		ir_store_changed();
		ir_result(IR_STA_ERASED, count, name);
	} else {
		ir_result(r == IR_STORE_NOT_FOUND ? IR_STA_NOT_FOUND : IR_STA_FLASH_ERROR, 0, name);
	}
}

static void ir_erase_all(void) {
	int r = ir_store_wipe();	// marks every code deleted
	ir_store_changed();
	ir_result(r == IR_STORE_OK ? IR_STA_ERASED : IR_STA_FLASH_ERROR, 0, "");
}

/*******************************************************************
 * List: one code info per call, while the stack is idle
 */
static int ir_list_task(void) {
	char name[1 + IR_NAME_MAX];
	unsigned count;
	if(!ir.listing)
		return 0;
	if(!zb_isDeviceJoinedNwk()) {
		ir.listing = 0;
		return 0;
	}
	if(tl_stackBusy() || !zb_isTaskDone())	// one at a time, after the previous has gone
		return 1;
	if(ir.storage_ok && ir_store_iter_next(name, &count)) {
		ir_code_info(count, name);
	} else {
		ir_code_info(0, "");	// end of the list
		ir.listing = 0;
	}
	return 1;
}

/*******************************************************************
 * Commands
 */
static void ir_cfg_save(void) {
	u8 poll_idle;
	if(g_zcl_irAttrs.poll_idle < 1)
		g_zcl_irAttrs.poll_idle = 1;
	else if(g_zcl_irAttrs.poll_idle > IR_POLL_IDLE_MAX)
		g_zcl_irAttrs.poll_idle = IR_POLL_IDLE_MAX;
	if(nv_flashReadNew(1, NV_MODULE_APP, NV_ITEM_APP_IR_CFG, sizeof(poll_idle), &poll_idle) != NV_SUCC
		|| poll_idle != g_zcl_irAttrs.poll_idle) {
		nv_flashWriteNew(1, NV_MODULE_APP, NV_ITEM_APP_IR_CFG, sizeof(poll_idle), &g_zcl_irAttrs.poll_idle);
	}
	app_pollRateApply();
}

/* Called from the ZCL write handler */
void ir_zcl_write(u16 attrID, u8 *data) {
	if(attrID == ZCL_ATTRID_IR_POLL_IDLE) {
		g_zcl_irAttrs.poll_idle = data[0];
		ir_cfg_save();
	}
}

/* Take a name [len][bytes] off the frame into a C string */
static status_t ir_name_take(u8 **p, u16 *n, char *out) {
	u8 len;
	if(*n < 1 || *n < 1 + (*p)[0])
		return ZCL_STA_MALFORMED_COMMAND;
	len = (*p)[0];
	if(len == 0 || len > IR_NAME_MAX || memchr(*p + 1, 0, len))
		return ZCL_STA_INVALID_FIELD;
	memcpy(out, *p + 1, len);
	out[len] = 0;
	*p += 1 + len;
	*n -= 1 + len;
	return ZCL_STA_SUCCESS;
}

/* Cluster specific commands: checked here, done later in ir_task() */
static status_t ir_zclCmdHandler(zclIncoming_t *pInMsg) {
	u8 *p = pInMsg->pData;
	u16 n = pInMsg->dataLen;
	status_t st = ZCL_STA_SUCCESS;
	u8 cmd;
	if(pInMsg->hdr.frmCtrl.bf.dir != ZCL_FRAME_CLIENT_SERVER_DIR)
		return ZCL_STA_UNSUP_CLUSTER_COMMAND;
	switch(pInMsg->hdr.cmd) {
	case IR_ZCL_CMD_LEARN:		cmd = IR_CMD_LEARN;		break;
	case IR_ZCL_CMD_RENAME:		cmd = IR_CMD_RENAME;	break;
	case IR_ZCL_CMD_LIST:		cmd = IR_CMD_LIST;		break;
	case IR_ZCL_CMD_SEND:		cmd = IR_CMD_SEND;		break;
	case IR_ZCL_CMD_ERASE:		cmd = IR_CMD_ERASE;		break;
	case IR_ZCL_CMD_ERASE_ALL:	cmd = IR_CMD_ERASE_ALL;	break;
	case IR_ZCL_CMD_CANCEL:		cmd = IR_CMD_CANCEL;	break;
	default:
		return ZCL_STA_UNSUP_CLUSTER_COMMAND;
	}
	if(cmd == IR_CMD_LEARN || cmd == IR_CMD_RENAME || cmd == IR_CMD_SEND || cmd == IR_CMD_ERASE)
		st = ir_name_take(&p, &n, ir.cmd_name);
	if(st == ZCL_STA_SUCCESS && cmd == IR_CMD_RENAME)
		st = ir_name_take(&p, &n, ir.cmd_name2);
	if(st != ZCL_STA_SUCCESS)
		return st;
	ir.cmd = cmd;
	ir.tick = clock_time();
	ir_fast_poll();
	return ZCL_STA_SUCCESS;
}

static void ir_cmd_exec(void) {
	u8 cmd = ir.cmd;
	ir.cmd = IR_CMD_NONE;
	if(cmd == IR_CMD_CANCEL) {
		if(ir.state != IR_ST_IDLE)
			ir_learn_end(IR_STA_CANCELLED, 0);
		return;
	}
	if(cmd == IR_CMD_LIST) {
		if(ir.storage_ok)
			ir_store_iter_start();
		ir.listing = 1;		// without storage: just the end marker
		return;
	}
	if(!ir.storage_ok) {
		ir_result(IR_STA_NO_STORAGE, 0, cmd == IR_CMD_ERASE_ALL ? "" : ir.cmd_name);
		return;
	}
	if(cmd == IR_CMD_LEARN) {
		ir_learn_start(ir.cmd_name);
		return;
	}
	if(ir.state != IR_ST_IDLE)	// anything else ends a learn in progress (and needs ir_buf)
		ir_learn_end(IR_STA_CANCELLED, 0);
	switch(cmd) {
	case IR_CMD_SEND:		ir_send(ir.cmd_name);						break;
	case IR_CMD_RENAME:		ir_rename(ir.cmd_name, ir.cmd_name2);		break;
	case IR_CMD_ERASE:		ir_erase(ir.cmd_name);						break;
	case IR_CMD_ERASE_ALL:	ir_erase_all();								break;
	default:														break;
	}
}

/*******************************************************************
 * Task, called from app_task(). Returns != 0 while the device must stay awake.
 */
int ir_task(void) {
	int busy;
	if(ir.cmd != IR_CMD_NONE) {
		// let the stack send the default response first
		if((!tl_stackBusy() && zb_isTaskDone())
			|| clock_time_exceed(ir.tick, IR_CMD_DEFER_TICKS / 16))
			ir_cmd_exec();
		return 1;
	}
	busy = ir_list_task();
	switch(ir.state) {
	case IR_ST_LEARN_SETTLE:
		if(!gpio_read(GPIO_IR_RX))
			ir.tick = clock_time();
		else if((u32)(clock_time() - ir.tick) > IR_IDLE_TICKS)
			ir_rx_arm();
		return 1;
	case IR_ST_LEARN_WAIT:
		return 1;
	case IR_ST_CAPTURED:
		ir_learn_captured();
		return 1;
	default:
		break;
	}
	return busy;
}

/*******************************************************************
 * Init, cold start only: opening the store reads every entry header.
 * The first start after an update formats it.
 */
void ir_init(void) {
	u8 poll_idle;
	memset(&ir, 0, sizeof(ir));
	ir.storage_ok = ((flash_read_mid() >> 16) & 0xff) >= FLASH_SIZE_1M
			&& ir_store_init() == IR_STORE_OK;
	g_zcl_irAttrs.code_count = ir.storage_ok ? ir_store_count() : 0;
	if(nv_flashReadNew(1, NV_MODULE_APP, NV_ITEM_APP_IR_CFG, sizeof(poll_idle), &poll_idle) == NV_SUCC
		&& poll_idle >= 1 && poll_idle <= IR_POLL_IDLE_MAX)
		g_zcl_irAttrs.poll_idle = poll_idle;
}

#endif // USE_IR_BLASTER
