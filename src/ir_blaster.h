/*
 * ir_blaster.h
 *
 * IR learn / send with the codes stored on the device (flash slots).
 */
#ifndef _IR_BLASTER_H_
#define _IR_BLASTER_H_

#if USE_IR_BLASTER

/* Manufacturer specific cluster, used without a manufacturer code */
#define ZCL_CLUSTER_IR_BLASTER				0xFC49

/* Attributes */
#define ZCL_ATTRID_IR_LEARN					0x0000 // UINT8  RW: write a slot number to learn into it, IR_SLOT_ALL = cancel
#define ZCL_ATTRID_IR_SEND					0x0001 // UINT8  RW: write a slot number to transmit it
#define ZCL_ATTRID_IR_ERASE					0x0002 // UINT8  RW: write a slot number to erase it, IR_SLOT_ALL = erase all
#define ZCL_ATTRID_IR_RESULT				0x0003 // UINT32 R : status | (slot << 8) | (number of timings << 16), reported
#define ZCL_ATTRID_IR_SLOTS_USED			0x0004 // BITMAP64 R: bit n = slot n holds a code
#define ZCL_ATTRID_IR_SLOT_COUNT			0x0005 // UINT8  R : number of slots
#define ZCL_ATTRID_IR_POLL_IDLE				0x0006 // UINT8  RW: idle poll interval in 1/4 s (saved)
#define ZCL_IR_ATTR_NUM						8      // + cluster revision

/* Cluster specific commands, client to server. A name is [length][bytes], up to
 * IR_NAME_MAX bytes of any value (UTF-8 from the hub, spaces allowed). Names are
 * carried in commands, not attributes, so their length can be checked here. */
#define IR_ZCL_CMD_LEARN					0x00 // [slot][name]: learn into a slot, store the name with it
#define IR_ZCL_CMD_RENAME					0x01 // [slot][name]: change the name stored with a code
#define IR_ZCL_CMD_QUERY					0x02 // [slot]: report its slot info, IR_SLOT_ALL = all used slots
/* Server to client */
#define IR_ZCL_CMD_SLOT_INFO				0x00 // [slot][timings u16][name], timings 0 = empty slot

#define IR_NAME_MAX							64

/* 64 slots is a choice, not the limit: the free flash (0x80000..0xFBFFF) has
 * room for 124. More than 64 needs a wider "slots used" attribute. */
#define IR_SLOT_COUNT						64     // 0x80000..0xBFFFF, one 4K sector each
#define IR_SLOT_ALL							0x80

/* Result status (low byte of ZCL_ATTRID_IR_RESULT) */
enum {
	IR_STA_IDLE = 0,
	IR_STA_LEARNING,		// waiting for the remote
	IR_STA_LEARN_OK,
	IR_STA_LEARN_TIMEOUT,	// no signal
	IR_STA_LEARN_TOO_LONG,	// more timings than a slot holds
	IR_STA_SEND_OK,
	IR_STA_SLOT_EMPTY,
	IR_STA_BAD_SLOT,
	IR_STA_ERASED,
	IR_STA_FLASH_ERROR,
	IR_STA_NO_STORAGE,		// flash smaller than 1M
	IR_STA_CANCELLED
};

typedef struct {
	u8  learn;
	u8  send;
	u8  erase;
	u8  slot_count;
	u32 result;
	u8  slots_used[8];
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
