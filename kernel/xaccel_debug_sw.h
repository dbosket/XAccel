#ifndef XACCEL_SW_DEBUG_H
#define XACCEL_SW_DEBUG_H

#include <linux/kernel.h>
#include <linux/types.h>

#include "xaccel_core.h"
#include "xaccel_uapi.h"
#include "../include/xaccel_desc.h"

#define XACCEL_TEST_DESC_REGION_SIZE  0x1000
#define XACCEL_TEST_FUNC0_MMIO_OFFSET 0x1000
#define XACCEL_TEST_FUNC1_MMIO_OFFSET 0x2000
#define XACCEL_TEST_MMIO_SIZE         0x200

#define XACCEL_TEST_ONE_FUNC_REGION_SIZE \
	(XACCEL_TEST_FUNC0_MMIO_OFFSET + XACCEL_TEST_MMIO_SIZE)

#define XACCEL_TEST_TWO_FUNC_REGION_SIZE \
	(XACCEL_TEST_FUNC1_MMIO_OFFSET + XACCEL_TEST_MMIO_SIZE)


enum test_case
{
	ONE_FUNCTION,
	TWO_FUNCTION,
	EXT_BLOCK,
	INVALID_MAGIC,
	INVALID_VERSION,
	INVALID_TOTAL_SIZE,
	INVALID_FUNC_WINDOW,
	IMPOSSIBLE_NUM_FUNCS,
	BAD_EXT_BLOCK
};

/*
 * This function is intended to generate a buffer in kmemory which mimics the register mapping in hardware
 * Based on the test case specified, it will return a buffer to the start of a "MMIO MAPPED" region
 * Which will allows rapid testing of descriptor parsing and processing efforts
 */
int gen_xaccel_test_obj(enum test_case test, void** buf_out, size_t *region_size_out);


/* 
 * This function is intended to write at the memory address specified by buffer, which is the start of a
 * fake MMIO MAPPED region. It will create a descriptor header using the input parameters
 */
int gen_xaccel_desc_header(void* buf, int16_t version, __u32 total_size, __u16 num_funcs, __u16 flags, __u32 checksum, __u16 device_id);


/* 
 * This function is intended to write at the memory address specified by buffer, which is the start of a
 * fake MMIO MAPPED region. It will create a function descriptor using the input parameters.
 */

int gen_xaccel_function_desc(void* buf, __u16 id, __u16 type, __u16 version, __u16 irq_index, __u32 mmio_offset, __u32 mmio_size, __u32 capabilities, __u32 ext_offset, __u32 ext_size);


static inline void write32(void *base, __u32 offset, __u32 val)
{
    *(__u32 *)((__u8 *)base + offset) = val;
}

static inline __u32 read32(void *base, __u32 offset){
	return *(__u32 *)((__u8 *)base + offset);
}

static inline void write16(void *base, __u16 offset, __u16 val)
{
    *(__u16 *)((__u8 *)base + offset) = val;
}

static inline __u16 read16(void *base, __u16 offset){
	return *(__u16 *)((__u8 *)base + offset);
}


#endif
