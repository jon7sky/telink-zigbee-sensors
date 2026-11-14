/*
 * ir_store.h
 *
 * Learned IR codes, stored by name in the upper half of a 1M flash (see
 * ir_store.c). A code is its list of u16 timings, at most 2000.
 */
#ifndef _IR_STORE_H_
#define _IR_STORE_H_

#define IR_STORE_NAME_MAX		64		// bytes, not counting the terminating 0

enum {
	IR_STORE_OK = 0,
	IR_STORE_NOT_FOUND,
	IR_STORE_FULL,
	IR_STORE_EXISTS,
	IR_STORE_ERROR
};

/* Open the store: reads every entry header. The first start erases the area. */
int ir_store_init(void);
int ir_store_ready(void);

/* Read a code into t[max], returns the number of timings, 0 if not found */
unsigned ir_store_get(const char *name, unsigned short *t, unsigned max);
/* Number of timings of a code, 0 if not found */
unsigned ir_store_size(const char *name);
/* Add or replace a code */
int ir_store_set(const char *name, const unsigned short *t, unsigned count);
int ir_store_del(const char *name);
/* Delete every code */
int ir_store_wipe(void);
/* Number of codes (at most 256) */
unsigned ir_store_count(void);

/* List the codes: start, then next until it returns 0.
 * name must hold IR_STORE_NAME_MAX + 1 bytes. */
void ir_store_iter_start(void);
int  ir_store_iter_next(char *name, unsigned *count);

#endif /* _IR_STORE_H_ */
