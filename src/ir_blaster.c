/*
 * ir_blaster.c
 *
 * IR learn / send with the codes stored on the device.
 *
 * A code is a list of raw mark/space durations, so any protocol and any
 * length up to IR_MAX_TIMINGS works. Codes are kept in flash slots (one 4K
 * sector each) and never cross the radio: the hub only writes a slot number
 * to the "learn", "send" or "erase" attribute and gets a "result" report.
 *
 * Each slot also holds the code's name, so the hub can rebuild its list of
 * names from the device after a re-pair. Slot layout:
 *   0x000  magic, count, checksum, reserved (4 x u16)
 *   0x008  timings, u16 each, up to IR_MAX_TIMINGS
 *   0xFB8  name length (0xFF = no name), then up to IR_NAME_MAX bytes
 * Codes saved before names existed have an erased (0xFF) name area, which
 * reads as "no name", so they need no conversion.
 */
#include "tl_common.h"
#include "zb_api.h"
#include "zcl_include.h"
#include "bdb.h"
#include "app_main.h"
#include "app_ui.h"
#include "ir_blaster.h"

#if USE_IR_BLASTER

/* Storage: the upper half of a 1M flash is not used by the 512K layout.
 * IR_SLOT_COUNT sectors from 0x80000; 0xFC000..0xFFFFF (config, MAC) is off limits */
#define IR_FLASH_BASE			0x80000
#define IR_SLOT_SIZE			FLASH_SECTOR_SIZE	// 4096
#define IR_MAGIC				0x5249				// "IR"
#define IR_NAME_OFFSET			(IR_SLOT_SIZE - 72)	// 0xFB8: [len][IR_NAME_MAX bytes], padded
#define IR_MAX_TIMINGS			2000				// (IR_NAME_OFFSET - 8) / 2 = 2008

/* Timings are stored in units of 4 us: 64 ticks of the 16 MHz system timer */
#define IR_UNIT_SHIFT			6
#define IR_MIN_TIMINGS			8					// shorter captures are noise
#define IR_END_GAP_TICKS		(120 * 1000 * 16)	// 120 ms of silence ends a code
#define IR_IDLE_TICKS			(100 * 1000 * 16)	// line must be quiet before arming
#define IR_LEARN_TIMEOUT_MS		15000
#define IR_CMD_DEFER_TICKS		(300 * 1000 * 16)	// max wait for the write response

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
	IR_CMD_SEND,
	IR_CMD_ERASE
};

#define IR_CAP_OVERFLOW			1
#define IR_CAP_STUCK			2

typedef struct {
	u16 magic;
	u16 count;
	u16 csum;
	u16 rsv;
	u16 t[IR_MAX_TIMINGS];
} ir_code_t;

typedef struct {
	volatile u8  state;
	u8  cmd;
	u8  cmd_slot;
	u8  slot;				// slot being learned
	u8  storage_ok;
	volatile u8  cap_flags;
	volatile u16 cap_count;
	u32 tick;				// state / command time stamp
	u8  learn_name[1 + IR_NAME_MAX];	// name for the learn in progress
	u8  query[8];			// slots whose info is waiting to be reported
	ev_timer_event_t *timerLearnEvt;
	ev_timer_event_t *timerFastPollEvt;
} ir_ctx_t;

static ir_ctx_t ir;
static ir_code_t ir_code;

zcl_irAttr_t g_zcl_irAttrs = {
	.learn = IR_SLOT_ALL,
	.send = IR_SLOT_ALL,
	.erase = IR_SLOT_ALL,
	.slot_count = IR_SLOT_COUNT,
	.poll_idle = IR_POLL_IDLE_DEF
};

const zclAttrInfo_t ir_attrTbl[ZCL_IR_ATTR_NUM] = {
	{ ZCL_ATTRID_IR_LEARN,		ZCL_DATA_TYPE_UINT8,	ACCESS_CONTROL_READ | ACCESS_CONTROL_WRITE,	(u8*)&g_zcl_irAttrs.learn },
	{ ZCL_ATTRID_IR_SEND,		ZCL_DATA_TYPE_UINT8,	ACCESS_CONTROL_READ | ACCESS_CONTROL_WRITE,	(u8*)&g_zcl_irAttrs.send },
	{ ZCL_ATTRID_IR_ERASE,		ZCL_DATA_TYPE_UINT8,	ACCESS_CONTROL_READ | ACCESS_CONTROL_WRITE,	(u8*)&g_zcl_irAttrs.erase },
	{ ZCL_ATTRID_IR_RESULT,		ZCL_DATA_TYPE_UINT32,	ACCESS_CONTROL_READ | ACCESS_CONTROL_REPORTABLE, (u8*)&g_zcl_irAttrs.result },
	{ ZCL_ATTRID_IR_SLOTS_USED,	ZCL_DATA_TYPE_BITMAP64,	ACCESS_CONTROL_READ,						(u8*)&g_zcl_irAttrs.slots_used },
	{ ZCL_ATTRID_IR_SLOT_COUNT,	ZCL_DATA_TYPE_UINT8,	ACCESS_CONTROL_READ,						(u8*)&g_zcl_irAttrs.slot_count },
	{ ZCL_ATTRID_IR_POLL_IDLE,	ZCL_DATA_TYPE_UINT8,	ACCESS_CONTROL_READ | ACCESS_CONTROL_WRITE,	(u8*)&g_zcl_irAttrs.poll_idle },
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
 * Result report (to the coordinator, no binding needed)
 */
static void ir_result(u8 status, u8 slot, u16 count) {
	epInfo_t dstEpInfo;
	g_zcl_irAttrs.result = status | (slot << 8) | (count << 16);
	if(!zb_isDeviceJoinedNwk())
		return;
	TL_SETSTRUCTCONTENT(dstEpInfo, 0);
	dstEpInfo.profileId = HA_PROFILE_ID;
	dstEpInfo.dstAddrMode = APS_SHORT_DSTADDR_WITHEP;
	dstEpInfo.dstAddr.shortAddr = 0x0000;
	dstEpInfo.dstEp = 1;
	zcl_sendReportCmd(SENSOR_DEVICE_ENDPOINT, &dstEpInfo, TRUE, ZCL_FRAME_SERVER_CLIENT_DIR,
			ZCL_CLUSTER_IR_BLASTER, ZCL_ATTRID_IR_RESULT, ZCL_DATA_TYPE_UINT32, (u8*)&g_zcl_irAttrs.result);
}

/*******************************************************************
 * Flash slots
 */
static u16 ir_csum(void) {
	u16 s = 0;
	for(u32 i = 0; i < ir_code.count; i++)
		s += ir_code.t[i];
	return s;
}

static void ir_set_used(u8 slot, u8 used) {
	if(used)
		g_zcl_irAttrs.slots_used[slot >> 3] |= BIT(slot & 7);
	else
		g_zcl_irAttrs.slots_used[slot >> 3] &= ~BIT(slot & 7);
}

/* Load a slot into ir_code, returns the number of timings or 0 */
static u16 ir_slot_load(u8 slot) {
	u32 faddr = IR_FLASH_BASE + slot * IR_SLOT_SIZE;
	flash_read(faddr, 8, (u8*)&ir_code);
	if(ir_code.magic != IR_MAGIC || ir_code.count == 0 || ir_code.count > IR_MAX_TIMINGS)
		return 0;
	flash_read(faddr + 8, ir_code.count * 2, (u8*)&ir_code.t);
	if(ir_code.csum != ir_csum())
		return 0;
	return ir_code.count;
}

static int ir_flash_verify(u32 faddr, u8 *p, u32 len) {
	u8 buf[32];
	for(u32 i = 0; i < len; i += sizeof(buf)) {
		u32 n = len - i;
		if(n > sizeof(buf))
			n = sizeof(buf);
		flash_read(faddr + i, n, buf);
		if(memcmp(buf, p + i, n))
			return 0;
	}
	return 1;
}

/* Read a slot's name into name[1 + IR_NAME_MAX], length 0 if it has none */
static void ir_slot_name_read(u8 slot, u8 *name) {
	flash_read(IR_FLASH_BASE + slot * IR_SLOT_SIZE + IR_NAME_OFFSET, 1 + IR_NAME_MAX, name);
	if(name[0] > IR_NAME_MAX)
		name[0] = 0;
}

/* Save ir_code (count is set) and a name ([len][bytes], may be empty) into a slot, verified */
static int ir_slot_save(u8 slot, u8 *name) {
	u32 faddr = IR_FLASH_BASE + slot * IR_SLOT_SIZE;
	u8 *p = (u8*)&ir_code;
	u32 len = 8 + ir_code.count * 2;
	ir_code.magic = IR_MAGIC;
	ir_code.csum = ir_csum();
	ir_code.rsv = 0xffff;
	flash_erase(faddr);
	flash_write(faddr, len, p);
	if(!ir_flash_verify(faddr, p, len))
		return 0;
	if(name[0] && name[0] <= IR_NAME_MAX) {
		flash_write(faddr + IR_NAME_OFFSET, 1 + name[0], name);
		if(!ir_flash_verify(faddr + IR_NAME_OFFSET, name, 1 + name[0]))
			return 0;
	}
	return 1;
}

static void ir_query(u8 slot) {
	if(slot < IR_SLOT_COUNT)
		ir.query[slot >> 3] |= BIT(slot & 7);
}

/* Report one slot: [slot][timings u16][name], to the coordinator */
static void ir_slot_info_send(u8 slot) {
	epInfo_t dstEpInfo;
	u8 buf[4 + IR_NAME_MAX];
	u16 hdr[2];
	u16 count = 0;
	flash_read(IR_FLASH_BASE + slot * IR_SLOT_SIZE, sizeof(hdr), (u8*)hdr);
	if(hdr[0] == IR_MAGIC && hdr[1] != 0 && hdr[1] <= IR_MAX_TIMINGS)
		count = hdr[1];
	buf[0] = slot;
	buf[1] = count & 0xff;
	buf[2] = count >> 8;
	if(count)
		ir_slot_name_read(slot, &buf[3]);
	else
		buf[3] = 0;
	TL_SETSTRUCTCONTENT(dstEpInfo, 0);
	dstEpInfo.profileId = HA_PROFILE_ID;
	dstEpInfo.dstAddrMode = APS_SHORT_DSTADDR_WITHEP;
	dstEpInfo.dstAddr.shortAddr = 0x0000;
	dstEpInfo.dstEp = 1;
	zcl_sendCmd(SENSOR_DEVICE_ENDPOINT, &dstEpInfo, ZCL_CLUSTER_IR_BLASTER, IR_ZCL_CMD_SLOT_INFO,
			TRUE, ZCL_FRAME_SERVER_CLIENT_DIR, TRUE, MANUFACTURER_CODE_NONE, ZCL_SEQ_NUM, 4 + buf[3], buf);
}

static void ir_slot_erase(u8 slot) {
	u32 magic;
	u32 faddr = IR_FLASH_BASE + slot * IR_SLOT_SIZE;
	flash_read(faddr, sizeof(magic), (u8*)&magic);
	if(magic != 0xffffffff)
		flash_erase(faddr);
	ir_set_used(slot, 0);
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

static void ir_send(u8 slot) {
	u32 r;
	u16 count = ir_slot_load(slot);
	if(!count) {
		ir_set_used(slot, 0);
		ir_result(IR_STA_SLOT_EMPTY, slot, 0);
		return;
	}
	light_on();
	reg_clk_en0 |= FLD_CLK0_PWM_EN;
	reg_pwm_clk = 0;	// PWM clock = system clock
	pwm_set_mode(PWM0_ID, PWM_NORMAL_MODE);
	pwm_set_cycle_and_duty(PWM0_ID, IR_PWM_MAX, IR_PWM_CMP);
	gpio_set_func(GPIO_IR_TX, AS_PWM0);
	r = drv_disable_irq();
	ir_play(ir_code.t, count);
	drv_restore_irq(r);
	gpio_write(GPIO_IR_TX, 0);
	gpio_set_func(GPIO_IR_TX, AS_GPIO);
	gpio_set_output_en(GPIO_IR_TX, 1);
	reg_clk_en0 &= ~FLD_CLK0_PWM_EN;
	light_off();
	ir_result(IR_STA_SEND_OK, slot, count);
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
		ir_code.t[n++] = dt >> IR_UNIT_SHIFT;
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
	ir.learn_name[0] = 0;
	ir_result(status, ir.slot, count);
	if(status == IR_STA_LEARN_OK)
		ir_query(ir.slot);	// confirm the stored name to the hub
}

static s32 ir_learnTimeoutCb(void *arg) {
	ir.timerLearnEvt = NULL;
	if(ir.state != IR_ST_IDLE)
		ir_learn_end(IR_STA_LEARN_TIMEOUT, 0);
	return -1;
}

static void ir_learn_start(u8 slot) {
	if(ir.state != IR_ST_IDLE)
		ir_learn_end(IR_STA_CANCELLED, 0);
	ir.slot = slot;
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
	ir_result(IR_STA_LEARNING, slot, 0);
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
		ir_code.count = count;
		if(ir_slot_save(ir.slot, ir.learn_name)) {
			ir_set_used(ir.slot, 1);
			ir_learn_end(IR_STA_LEARN_OK, count);
		} else {
			ir_set_used(ir.slot, 0);
			ir_learn_end(IR_STA_FLASH_ERROR, count);
		}
	}
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

static void ir_cmd_queue(u8 cmd, u8 slot) {
	ir.cmd = cmd;
	ir.cmd_slot = slot;
	ir.tick = clock_time();
	ir_fast_poll();
}

/* Called from the ZCL write handler, the work is done later in ir_task() */
void ir_zcl_write(u16 attrID, u8 *data) {
	u8 cmd;
	switch(attrID) {
	case ZCL_ATTRID_IR_LEARN:
		cmd = IR_CMD_LEARN;
		ir.learn_name[0] = 0;	// learned without a name
		break;
	case ZCL_ATTRID_IR_SEND:
		cmd = IR_CMD_SEND;
		break;
	case ZCL_ATTRID_IR_ERASE:
		cmd = IR_CMD_ERASE;
		break;
	case ZCL_ATTRID_IR_POLL_IDLE:
		g_zcl_irAttrs.poll_idle = data[0];
		ir_cfg_save();
		return;
	default:
		return;
	}
	ir_cmd_queue(cmd, data[0]);
}

/* Change the name stored with a code: rewrites the slot */
static status_t ir_rename(u8 slot, u8 *name) {
	u8 old[1 + IR_NAME_MAX];
	if(!ir.storage_ok || ir.state != IR_ST_IDLE)	// a capture may be using ir_code
		return ZCL_STA_FAILURE;
	if(!ir_slot_load(slot))
		return ZCL_STA_NOT_FOUND;
	ir_slot_name_read(slot, old);
	if(old[0] != name[0] || memcmp(&old[1], &name[1], name[0])) {
		if(!ir_slot_save(slot, name)) {
			ir_set_used(slot, 0);
			return ZCL_STA_FAILURE;
		}
	}
	ir_query(slot);
	return ZCL_STA_SUCCESS;
}

/* Cluster specific commands. Every length is checked against the frame. */
static status_t ir_zclCmdHandler(zclIncoming_t *pInMsg) {
	u8 *p = pInMsg->pData;
	u16 n = pInMsg->dataLen;
	if(pInMsg->hdr.frmCtrl.bf.dir != ZCL_FRAME_CLIENT_SERVER_DIR)
		return ZCL_STA_UNSUP_CLUSTER_COMMAND;
	switch(pInMsg->hdr.cmd) {
	case IR_ZCL_CMD_LEARN:
	case IR_ZCL_CMD_RENAME:
		if(n < 2 || n < 2 + p[1])
			return ZCL_STA_MALFORMED_COMMAND;
		if(p[0] >= IR_SLOT_COUNT || p[1] > IR_NAME_MAX)
			return ZCL_STA_INVALID_FIELD;
		if(pInMsg->hdr.cmd == IR_ZCL_CMD_RENAME)
			return ir_rename(p[0], &p[1]);
		memcpy(ir.learn_name, &p[1], 1 + p[1]);
		ir_cmd_queue(IR_CMD_LEARN, p[0]);
		return ZCL_STA_SUCCESS;
	case IR_ZCL_CMD_QUERY:
		if(n < 1)
			return ZCL_STA_MALFORMED_COMMAND;
		if(p[0] == IR_SLOT_ALL)
			memcpy(ir.query, g_zcl_irAttrs.slots_used, sizeof(ir.query));
		else if(p[0] < IR_SLOT_COUNT)
			ir_query(p[0]);
		else
			return ZCL_STA_INVALID_FIELD;
		ir_fast_poll();
		return ZCL_STA_SUCCESS;
	default:
		return ZCL_STA_UNSUP_CLUSTER_COMMAND;
	}
}

/* Send one queued slot info, returns 1 while any are left */
static int ir_query_task(void) {
	u8 slot;
	if(!zb_isDeviceJoinedNwk()) {
		memset(ir.query, 0, sizeof(ir.query));
		return 0;
	}
	for(slot = 0; slot < IR_SLOT_COUNT; slot++)
		if(ir.query[slot >> 3] & BIT(slot & 7))
			break;
	if(slot >= IR_SLOT_COUNT)
		return 0;
	if(!tl_stackBusy() && zb_isTaskDone()) {	// one at a time, after the previous has gone
		ir.query[slot >> 3] &= ~BIT(slot & 7);
		ir_slot_info_send(slot);
	}
	return 1;
}

static void ir_cmd_exec(void) {
	u8 cmd = ir.cmd;
	u8 slot = ir.cmd_slot;
	ir.cmd = IR_CMD_NONE;
	if(cmd == IR_CMD_LEARN && slot == IR_SLOT_ALL) {
		if(ir.state != IR_ST_IDLE)
			ir_learn_end(IR_STA_CANCELLED, 0);
		return;
	}
	if(!ir.storage_ok) {
		ir_result(IR_STA_NO_STORAGE, slot, 0);
		return;
	}
	if(cmd == IR_CMD_ERASE && slot == IR_SLOT_ALL) {
		for(slot = 0; slot < IR_SLOT_COUNT; slot++)
			ir_slot_erase(slot);
		ir_result(IR_STA_ERASED, IR_SLOT_ALL, 0);
		return;
	}
	if(slot >= IR_SLOT_COUNT) {
		ir_result(IR_STA_BAD_SLOT, slot, 0);
		return;
	}
	if(cmd == IR_CMD_LEARN) {
		ir_learn_start(slot);
		return;
	}
	if(ir.state != IR_ST_IDLE)	// send or erase ends a learn in progress
		ir_learn_end(IR_STA_CANCELLED, 0);
	if(cmd == IR_CMD_SEND) {
		ir_send(slot);
	} else {
		ir_slot_erase(slot);
		ir_result(IR_STA_ERASED, slot, 0);
	}
}

/*******************************************************************
 * Task, called from app_task(). Returns != 0 while the device must stay awake.
 */
int ir_task(void) {
	int busy;
	if(ir.cmd != IR_CMD_NONE) {
		// let the stack send the write response first
		if((!tl_stackBusy() && zb_isTaskDone())
			|| clock_time_exceed(ir.tick, IR_CMD_DEFER_TICKS / 16))
			ir_cmd_exec();
		return 1;
	}
	busy = ir_query_task();
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
 * Init, cold start only
 */
void ir_init(void) {
	u32 hdr;
	u8 poll_idle;
	memset(&ir, 0, sizeof(ir));
	ir.storage_ok = ((flash_read_mid() >> 16) & 0xff) >= FLASH_SIZE_1M;
	memset(g_zcl_irAttrs.slots_used, 0, sizeof(g_zcl_irAttrs.slots_used));
	if(ir.storage_ok) {
		for(u8 slot = 0; slot < IR_SLOT_COUNT; slot++) {
			flash_read(IR_FLASH_BASE + slot * IR_SLOT_SIZE, sizeof(hdr), (u8*)&hdr);
			if((hdr & 0xffff) == IR_MAGIC && (hdr >> 16) != 0 && (hdr >> 16) <= IR_MAX_TIMINGS)
				ir_set_used(slot, 1);
		}
	}
	if(nv_flashReadNew(1, NV_MODULE_APP, NV_ITEM_APP_IR_CFG, sizeof(poll_idle), &poll_idle) == NV_SUCC
		&& poll_idle >= 1 && poll_idle <= IR_POLL_IDLE_MAX)
		g_zcl_irAttrs.poll_idle = poll_idle;
}

#endif // USE_IR_BLASTER
