/*
 * ir_blaster.h
 *
 * IR learn / send with the codes stored on the device, by name (ir_store.c).
 */
#ifndef _IR_BLASTER_H_
#define _IR_BLASTER_H_

#if USE_IR_BLASTER

/* Manufacturer specific cluster, used without a manufacturer code */
#define ZCL_CLUSTER_IR_BLASTER				0xFC49

/* Attributes */
#define ZCL_ATTRID_IR_POLL_IDLE				0x0006 // UINT8  RW: idle poll interval in 1/4 s (saved)
#define ZCL_ATTRID_IR_CODE_COUNT			0x0007 // UINT16 R : number of stored codes
#define ZCL_IR_ATTR_NUM						3      // + cluster revision

/* Cluster specific commands. A name is [length][bytes], 1..IR_NAME_MAX bytes of
 * any value except 0 (UTF-8 from the hub, spaces allowed). */
/* Client to server */
#define IR_ZCL_CMD_LEARN					0x00 // [name]: learn a code, store it under the name
#define IR_ZCL_CMD_RENAME					0x01 // [old name][new name]
#define IR_ZCL_CMD_LIST						0x02 // -: report every code, then an end marker
#define IR_ZCL_CMD_SEND						0x03 // [name]
#define IR_ZCL_CMD_ERASE					0x04 // [name]
#define IR_ZCL_CMD_ERASE_ALL				0x05 // -
#define IR_ZCL_CMD_CANCEL					0x06 // -: cancel a learn
/* Server to client */
#define IR_ZCL_CMD_CODE_INFO				0x00 // [timings u16][name], 0 + empty name = end of list
#define IR_ZCL_CMD_RESULT					0x01 // [status][timings u16][name], after every operation

#define IR_NAME_MAX							64

/* Result status */
enum {
	IR_STA_IDLE = 0,
	IR_STA_LEARNING,		// waiting for the remote
	IR_STA_LEARN_OK,
	IR_STA_LEARN_TIMEOUT,	// no signal
	IR_STA_LEARN_TOO_LONG,	// more than IR_MAX_TIMINGS
	IR_STA_SEND_OK,
	IR_STA_NOT_FOUND,		// no code with that name
	IR_STA_RESERVED_7,		// was "bad slot"
	IR_STA_ERASED,			// empty name: all codes
	IR_STA_FLASH_ERROR,
	IR_STA_NO_STORAGE,		// flash smaller than 1M, or the store could not be opened
	IR_STA_CANCELLED,
	IR_STA_STORE_FULL,
	IR_STA_NAME_EXISTS,		// rename to a name that is taken
	IR_STA_RENAMED			// name = the new name
};

typedef struct {
	u16 code_count;
	u8  poll_idle;			// in 1/4 s
} zcl_irAttr_t;

extern zcl_irAttr_t g_zcl_irAttrs;
extern const zclAttrInfo_t ir_attrTbl[ZCL_IR_ATTR_NUM];

status_t zcl_ir_blaster_register(u8 endpoint, u16 manuCode, u8 attrNum, const zclAttrInfo_t attrTbl[], cluster_forAppCb_t cb);

void ir_init(void);
void ir_zcl_write(u16 attrID, u8 *data);
int  ir_task(void);			// != 0: busy, do not sleep
u32  ir_poll_rate_ms(void);

#endif // USE_IR_BLASTER
#endif // _IR_BLASTER_H_
