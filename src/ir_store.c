/*
 * ir_store.c
 *
 * IR code store: a log of codes in flash, keyed by name, with an index in RAM.
 *
 * Flash layout: IR_STORE_BLOCKS blocks of 8 KB (two erase sectors) from
 * IR_STORE_ADDR. A code never spans two blocks.
 *
 *   block:  magic u32 | seq u32 (0xFFFFFFFF = free) | 8 bytes reserved | entries...
 *   entry:  state u8 | name_len u8 | value_len u16 | crc32 u32 | name | value
 *
 * Entries are only ever appended at the end of the newest block (the head),
 * and a byte of flash can only go from 1 to 0, so an entry's state moves
 * WRITING -> VALID -> DELETED by clearing bits. The CRC covers the lengths,
 * the name and the value.
 *
 * Power loss:
 * - while an entry is being written: it is not VALID, so it is ignored, and
 *   its block takes no more entries.
 * - between writing a new copy of a name (replace, rename, compaction) and
 *   deleting the old one: at start-up two VALID entries have the same name,
 *   and the newer one (higher block seq, or later in the same block) wins.
 * - while erasing a block: its magic is gone, so it is erased again at start.
 *
 * Space: one free block is always kept for compaction. When a code does not
 * fit, the block with the most deleted bytes has its live entries copied into
 * the free block, then is erased.
 */
#include "tl_common.h"
#include "ir_store.h"

#if USE_IR_BLASTER

#define IR_STORE_ADDR			0x80000		// upper half of a 1M flash, unused by the 512K layout
#define IR_STORE_BLOCKS			62			// 496 KB, up to 0xFBFFF (0xFC000.. holds calibration and MAC)
#define BLK_SIZE				8192
#define SECTOR_SIZE				4096
#define BLK_HDR_SIZE			16
#define ENT_HDR_SIZE			8
#define IR_STORE_MAX_CODES		256			// RAM index size
#define IR_STORE_MAX_VALUE		4000		// bytes: 2000 timings

#define BLK_MAGIC				0x31535249	// "IRS1"
#define SEQ_FREE				0xFFFFFFFF

#define ST_EMPTY				0xFF
#define ST_WRITING				0xFE
#define ST_VALID				0xFC
#define ST_DELETED				0xF8

#define SRAM_START				0x840000
#define SRAM_END				0x850000

enum { BLK_FREE = 0, BLK_USED };

typedef struct {
	u32 seq;
	u16 end;		// offset of the first byte not written; BLK_SIZE = closed
	u16 live;		// bytes of VALID entries
	u8  state;
} blk_t;

typedef struct {
	u32 addr;		// flash address of the entry
	u16 hash;
	u16 count;		// timings
} ent_t;

typedef struct {
	u8  state;
	u8  name_len;
	u16 value_len;
	u32 crc;
} ent_hdr_t;

static blk_t blk[IR_STORE_BLOCKS];
static ent_t idx[IR_STORE_MAX_CODES];
static u16 n_idx;
static u8 ready;
static s8 head = -1;
static u32 max_seq;
static u16 iter_pos;

#define BLK_ADDR(b)			(IR_STORE_ADDR + (u32)(b) * BLK_SIZE)
#define ADDR_BLK(a)			(((a) - IR_STORE_ADDR) / BLK_SIZE)
#define ENT_SIZE(nl, vl)	(ENT_HDR_SIZE + (nl) + (vl))

/*******************************************************************
 * Helpers
 */
static u16 name_hash(const char *s, u8 n) {
	u32 h = 2166136261u;
	while(n--)
		h = (h ^ (u8)*s++) * 16777619u;
	return (u16)(h ^ (h >> 16));
}

static u32 crc32_upd(u32 crc, const u8 *p, u32 n) {
	crc = ~crc;
	while(n--) {
		crc ^= *p++;
		for(int k = 0; k < 8; k++)
			crc = (crc >> 1) ^ (0xEDB88320 & -(crc & 1));
	}
	return ~crc;
}

/* The flash driver cannot program data that is itself in flash: copy through RAM */
static void flash_put(u32 addr, const void *src, u32 len) {
	const u8 *p = src;
	if((u32)p >= SRAM_START && (u32)p + len <= SRAM_END) {
		flash_write(addr, len, (u8 *)p);
		return;
	}
	while(len) {
		u8 tmp[64];
		u32 n = len > sizeof(tmp) ? sizeof(tmp) : len;
		memcpy(tmp, p, n);
		flash_write(addr, n, tmp);
		addr += n;
		p += n;
		len -= n;
	}
}

static void set_state(u32 addr, u8 st) {
	flash_write(addr, 1, &st);
}

static void blk_erase(u8 b) {
	u32 hdr[2] = { BLK_MAGIC, SEQ_FREE };
	flash_erase(BLK_ADDR(b));
	flash_erase(BLK_ADDR(b) + SECTOR_SIZE);
	flash_write(BLK_ADDR(b), sizeof(hdr), (u8 *)hdr);
	blk[b].state = BLK_FREE;
	blk[b].seq = SEQ_FREE;
	blk[b].end = BLK_HDR_SIZE;
	blk[b].live = 0;
}

static void blk_activate(u8 b) {
	u32 seq = ++max_seq;
	flash_write(BLK_ADDR(b) + 4, 4, (u8 *)&seq);
	blk[b].state = BLK_USED;
	blk[b].seq = seq;
	head = b;
}

static int free_count(void) {
	int n = 0;
	for(int b = 0; b < IR_STORE_BLOCKS; b++)
		if(blk[b].state == BLK_FREE)
			n++;
	return n;
}

/* the free block erased longest ago is not known; take the one after the head */
static int free_block(void) {
	for(int i = 1; i <= IR_STORE_BLOCKS; i++) {
		int b = (head + i + IR_STORE_BLOCKS) % IR_STORE_BLOCKS;
		if(blk[b].state == BLK_FREE)
			return b;
	}
	return -1;
}

static int name_match(u32 addr, const char *name, u8 nl) {
	ent_hdr_t h;
	u8 buf[IR_STORE_NAME_MAX];
	flash_read(addr, sizeof(h), (u8 *)&h);
	if(h.name_len != nl)
		return 0;
	flash_read(addr + ENT_HDR_SIZE, nl, buf);
	return memcmp(buf, name, nl) == 0;
}

static int find(const char *name, u8 nl) {
	u16 h = name_hash(name, nl);
	for(int i = 0; i < n_idx; i++)
		if(idx[i].hash == h && name_match(idx[i].addr, name, nl))
			return i;
	return -1;
}

static void idx_remove(int i) {
	idx[i] = idx[--n_idx];
	if(iter_pos > i)
		iter_pos = 0;	// the list changed under the iterator: start again
}

static void entry_delete(u32 addr) {
	ent_hdr_t h;
	flash_read(addr, sizeof(h), (u8 *)&h);
	set_state(addr, ST_DELETED);
	blk[ADDR_BLK(addr)].live -= ENT_SIZE(h.name_len, h.value_len);
}

/* Append an entry at the end of block b (the caller checked the room) */
static u32 entry_write(u8 b, const char *name, u8 nl, const void *value, u16 vl) {
	u32 addr = BLK_ADDR(b) + blk[b].end;
	ent_hdr_t h;
	u16 size = ENT_SIZE(nl, vl);
	h.state = ST_WRITING;
	h.name_len = nl;
	h.value_len = vl;
	h.crc = crc32_upd(crc32_upd(crc32_upd(0, &nl, 1), (u8 *)&vl, 2), (const u8 *)name, nl);
	h.crc = crc32_upd(h.crc, value, vl);
	flash_put(addr, &h, sizeof(h));
	flash_put(addr + ENT_HDR_SIZE, name, nl);
	flash_put(addr + ENT_HDR_SIZE + nl, value, vl);
	set_state(addr, ST_VALID);
	blk[b].end += size;
	blk[b].live += size;
	return addr;
}

/* Copy a VALID entry into block b, flash to flash through RAM */
static u32 entry_copy(u8 b, u32 src) {
	ent_hdr_t h;
	u8 buf[128];
	u32 dst = BLK_ADDR(b) + blk[b].end;
	u32 len, done;
	flash_read(src, sizeof(h), (u8 *)&h);
	len = ENT_SIZE(h.name_len, h.value_len);
	h.state = ST_WRITING;
	flash_write(dst, sizeof(h), (u8 *)&h);
	for(done = ENT_HDR_SIZE; done < len; done += sizeof(buf)) {
		u32 n = len - done > sizeof(buf) ? sizeof(buf) : len - done;
		flash_read(src + done, n, buf);
		flash_write(dst + done, n, buf);
	}
	set_state(dst, ST_VALID);
	blk[b].end += len;
	blk[b].live += len;
	return dst;
}

/* Move the live entries of the block with the most deleted bytes into a free
 * block, then erase it. Returns 1 if space was gained. */
static int compact(void) {
	int victim = -1, best = 0, r;
	for(int b = 0; b < IR_STORE_BLOCKS; b++) {
		int dead;
		if(blk[b].state != BLK_USED)
			continue;
		// space that compaction gains: everything but the live entries, except
		// the head's unwritten tail, which can still be used as it is
		dead = (b == head ? blk[b].end : BLK_SIZE) - BLK_HDR_SIZE - blk[b].live;
		if(dead > best) {
			best = dead;
			victim = b;
		}
	}
	if(victim < 0)
		return 0;
	if(blk[victim].live == 0) {	// nothing to copy
		if(victim == head)
			head = -1;
		blk_erase(victim);
		return 1;
	}
	if((r = free_block()) < 0)
		return 0;
	blk_activate(r);	// newer than the victim: its copies win after a power loss
	for(int i = 0; i < n_idx; i++)
		if(ADDR_BLK(idx[i].addr) == (u32)victim)
			idx[i].addr = entry_copy(r, idx[i].addr);
	blk_erase(victim);
	return 1;
}

/* Find room for size bytes, returns the block or -1 */
static int alloc(u16 size) {
	for(int tries = 0; tries <= IR_STORE_BLOCKS; tries++) {
		int b;
		if(head >= 0 && blk[head].end + size <= BLK_SIZE)
			return head;
		if(free_count() > 1 && (b = free_block()) >= 0) {	// keep one free block
			blk_activate(b);
			continue;
		}
		if(!compact())
			return -1;
	}
	return -1;
}

/*******************************************************************
 * Start-up
 */
static void scan_block(u8 b) {
	u16 off = BLK_HDR_SIZE;
	blk[b].live = 0;
	while(off + ENT_HDR_SIZE <= BLK_SIZE) {
		ent_hdr_t h;
		u32 addr = BLK_ADDR(b) + off;
		u16 size;
		flash_read(addr, sizeof(h), (u8 *)&h);
		if(h.state == ST_EMPTY && h.name_len == 0xFF && h.value_len == 0xFFFF && h.crc == 0xFFFFFFFF)
			break;		// end of the log in this block
		size = ENT_SIZE(h.name_len, h.value_len);
		if((h.state != ST_VALID && h.state != ST_DELETED) || h.name_len == 0 || h.name_len > IR_STORE_NAME_MAX
			|| h.value_len > IR_STORE_MAX_VALUE || (h.value_len & 1) || off + size > BLK_SIZE) {
			off = BLK_SIZE;	// an interrupted write: take no more entries here
			break;
		}
		if(h.state == ST_VALID) {
			char name[IR_STORE_NAME_MAX];
			int i;
			flash_read(addr + ENT_HDR_SIZE, h.name_len, (u8 *)name);
			i = find(name, h.name_len);
			if(i >= 0) {
				entry_delete(idx[i].addr);	// an older copy (blocks are scanned oldest first)
				idx[i].addr = addr;
				idx[i].count = h.value_len / 2;
			} else if(n_idx < IR_STORE_MAX_CODES) {
				idx[n_idx].addr = addr;
				idx[n_idx].hash = name_hash(name, h.name_len);
				idx[n_idx].count = h.value_len / 2;
				n_idx++;
			} else {
				set_state(addr, ST_DELETED);	// cannot be indexed
				off += size;
				continue;
			}
			blk[b].live += size;
		}
		off += size;
	}
	blk[b].end = off;
}

int ir_store_init(void) {
	u8 order[IR_STORE_BLOCKS];
	int n = 0;
	if(ready)
		return IR_STORE_OK;
	n_idx = 0;
	head = -1;
	max_seq = 0;
	for(int b = 0; b < IR_STORE_BLOCKS; b++) {
		u32 hdr[2];
		flash_read(BLK_ADDR(b), sizeof(hdr), (u8 *)hdr);
		blk[b].live = 0;
		if(hdr[0] != BLK_MAGIC) {
			blk_erase(b);	// first start, or an interrupted erase
		} else if(hdr[1] == SEQ_FREE) {
			blk[b].state = BLK_FREE;
			blk[b].seq = SEQ_FREE;
			blk[b].end = BLK_HDR_SIZE;
		} else {
			blk[b].state = BLK_USED;
			blk[b].seq = hdr[1];
			if(hdr[1] > max_seq)
				max_seq = hdr[1];
			// insert into order[], oldest first
			int i = n++;
			while(i > 0 && blk[order[i - 1]].seq > hdr[1]) {
				order[i] = order[i - 1];
				i--;
			}
			order[i] = b;
		}
	}
	for(int i = 0; i < n; i++)
		scan_block(order[i]);
	if(n)
		head = order[n - 1];
	// Blocks that hold nothing live are left for compaction to erase when the
	// space is needed, which keeps the start-up short.
	ready = 1;
	return IR_STORE_OK;
}

int ir_store_ready(void) {
	return ready;
}

/*******************************************************************
 * Access
 */
static u8 name_len(const char *name) {
	u32 n = strlen(name);
	return (n == 0 || n > IR_STORE_NAME_MAX) ? 0 : n;
}

unsigned ir_store_size(const char *name) {
	u8 nl = name_len(name);
	int i;
	if(!ready || !nl || (i = find(name, nl)) < 0)
		return 0;
	return idx[i].count;
}

unsigned ir_store_get(const char *name, unsigned short *t, unsigned max) {
	u8 nl = name_len(name);
	ent_hdr_t h;
	u8 buf[IR_STORE_NAME_MAX];
	u32 crc;
	int i;
	if(!ready || !nl || (i = find(name, nl)) < 0 || idx[i].count > max)
		return 0;
	flash_read(idx[i].addr, sizeof(h), (u8 *)&h);
	flash_read(idx[i].addr + ENT_HDR_SIZE, nl, buf);
	flash_read(idx[i].addr + ENT_HDR_SIZE + nl, h.value_len, (u8 *)t);
	crc = crc32_upd(crc32_upd(crc32_upd(0, &h.name_len, 1), (u8 *)&h.value_len, 2), buf, nl);
	if(crc32_upd(crc, (u8 *)t, h.value_len) != h.crc)
		return 0;
	return h.value_len / 2;
}

int ir_store_set(const char *name, const unsigned short *t, unsigned count) {
	u8 nl = name_len(name);
	u16 vl = count * 2;
	int b, i;
	u32 addr;
	if(!ready)
		return IR_STORE_ERROR;
	if(!nl || count == 0 || vl > IR_STORE_MAX_VALUE)
		return IR_STORE_ERROR;
	if(n_idx >= IR_STORE_MAX_CODES && find(name, nl) < 0)
		return IR_STORE_FULL;
	if((b = alloc(ENT_SIZE(nl, vl))) < 0)
		return IR_STORE_FULL;
	addr = entry_write(b, name, nl, t, vl);
	i = find(name, nl);		// after alloc: compaction may have moved the old copy
	if(i >= 0 && idx[i].addr != addr) {
		entry_delete(idx[i].addr);
		idx[i].addr = addr;
		idx[i].count = count;
	} else if(i < 0) {
		idx[n_idx].addr = addr;
		idx[n_idx].hash = name_hash(name, nl);
		idx[n_idx].count = count;
		n_idx++;
	}
	return IR_STORE_OK;
}

int ir_store_del(const char *name) {
	u8 nl = name_len(name);
	int i;
	if(!ready)
		return IR_STORE_ERROR;
	if(!nl || (i = find(name, nl)) < 0)
		return IR_STORE_NOT_FOUND;
	entry_delete(idx[i].addr);
	idx_remove(i);
	return IR_STORE_OK;
}

int ir_store_wipe(void) {
	if(!ready)
		return IR_STORE_ERROR;
	while(n_idx) {
		entry_delete(idx[n_idx - 1].addr);
		n_idx--;
	}
	iter_pos = 0;
	return IR_STORE_OK;
}

unsigned ir_store_count(void) {
	return n_idx;
}

void ir_store_iter_start(void) {
	iter_pos = 0;
}

int ir_store_iter_next(char *name, unsigned *count) {
	ent_hdr_t h;
	if(!ready || iter_pos >= n_idx)
		return 0;
	flash_read(idx[iter_pos].addr, sizeof(h), (u8 *)&h);
	flash_read(idx[iter_pos].addr + ENT_HDR_SIZE, h.name_len, (u8 *)name);
	name[h.name_len] = 0;
	*count = idx[iter_pos].count;
	iter_pos++;
	return 1;
}

#endif // USE_IR_BLASTER
