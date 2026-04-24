/*
 * Author: Taco
 * Purpose: This file defines the core format for descriptors used by
 * 	the accelerators to describe their capabilities to xaccel.
 * Date Created: 26 March 2026
 */
#ifndef XACCEL_DESC_H
#define XACCEL_DESC_H


#include "xaccel_macros.h"

#ifdef __KERNEL__
    #include <linux/cdev.h>
    #include <linux/errno.h>
    #include <linux/mutex.h>
#else
    #include <errno.h>
    #include <pthread.h>
    #include <stdint.h>
#endif

#ifndef __packed
    #define __packed __attribute__((packed))
#endif

struct xaccel_desc_header 
{
	uint32_t magic;		// 'XACC'
	int16_t version;		// v1.0
	int16_t header_size;	// sizeof(header)	
	uint32_t total_size;	// full descriptor in bytes
	int16_t num_functions;    // number of functions device has
	int16_t flags;		// global flags
	uint32_t checksum;
	uint32_t device_id;	// Image ID
} __packed;


struct xaccel_func_desc
{
	int16_t func_id;	 	 // ID is device specific 0,1,2...
	int16_t func_type;	 // Numeric Classification of function (e.g. VECTOR_ADD, AES)
	int16_t func_version;	 // Version of function interface
	int16_t irq_index;	 // For accelerators requiring interrupts

	uint32_t mmio_offset; 	 // e.g. 0x0100
	uint32_t mmio_size;	 // e.g. 0x1000

	uint32_t caps;		 // Capabilities of this fucntion R/W
	uint32_t reg_layout_ver;    // 1

	uint32_t ext_offset;	 // Extension Block 
	uint32_t ext_size;		 // Extension Block Size
} __packed;	
	

// Is this Descriptor Header Valid?
static inline int xaccel_desc_magic_valid(const struct xaccel_desc_header *head)
{
	if (head == NULL) return 0;
	return (head->magic == XACCEL_DESC_MAGIC);
}

// Is this Descriptor Version Supported?
static inline int xaccel_desc_version_valid(const struct xaccel_desc_header *head)
{
	if (head == NULL) return 0;
	return ( (head->version) && (head->version <= MAX_VERSION_SUPPORTED));
		 
}

// Does this function have capabilities 
static inline int xaccel_func_has_cap(const struct xaccel_func_desc *head, uint32_t cap)
{
	if (head == NULL) return 0;
	return (head->caps & cap);
}

// Does this function use interrupts
static inline int xaccel_funct_has_irq(const struct xaccel_func_desc *head)
{
	if (head == NULL) return 0;
	return (head->irq_index != XACCEL_IRQ_NONE);
}

// Write 32 bytes to either hardware or kernel memory
static inline void xaccel_write32(void* base, __u32 offset, __u32 val)
{
	#ifdef NO_HW
	*(__u32 *)((__u8 *)base + offset) = val;
	#else
	iowrite32(val, (__u8 __iomem *)base + offset);
	#endif
}

// Write 16 bytes to either hardware or kernel memory
static inline void xaccel_write16(void* base, __u32 offset, __u32 val)
{
	#ifdef NO_HW
	*(__u16 *)((__u8 *)base + offset) = val;
	#else
	iowrite16(val,(__u8 __iomem *)base + offset);
	#endif
}

// Read 32 bytes from hardware or kernel memory
static inline __u32 xaccel_read32(void* base, __u32 offset)
{
	#ifdef NO_HW
	return *(__u32 *)((__u8 *)base + offset);
	#else
	return ioread32((__u8 __iomem *)base + offset);	
	#endif
}

// Read 16 bytes from hardware or kernel memory
static inline __u16 xaccel_read16(void* base, __u32 offset)
{
	#ifdef NO_HW
	return *(__u16 *)((__u8 *)base + offset);
	#else
	return ioread16((__u8 __iomem *)base + offset);	
	#endif
}


#endif
