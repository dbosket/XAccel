/*
 * Author: Taco
 * Purpose: This file defines the core format for descriptors used by
 * 	the accelerators to describe their capabilities to xaccel.
 * Date Created: 26 March 2026
 */
#ifndef XACCEL_DESC_H
#define XACCEL_DESC_H


#include "xaccel_macros.h"
#include <linux/cdev.h>
#include <linux/errno.h>
#include <linux/mutex.h>

struct xaccel_desc_header 
{
	__u32 magic;		// 'XACC'
	__u16 version;		// v1.0
	__u16 header_size;	// sizeof(header)	
	__u32 total_size;	// full descriptor in bytes
	__u16 num_functions;    // number of functions device has
	__u16 flags;		// global flags
	__u32 checksum;
	__u32 device_id;	// Image ID
} __packed;


struct xaccel_func_desc
{
	__u16 func_id;	 	 // ID is device specific 0,1,2...
	__u16 func_type;	 // Numeric Classification of function (e.g. VECTOR_ADD, AES)
	__u16 func_version;	 // Version of function interface
	__u16 irq_index;	 // For accelerators requiring interrupts

	__u32 mmio_offset; 	 // e.g. 0x0100
	__u32 mmio_size;	 // e.g. 0x1000

	__u32 caps;		 // Capabilities of this fucntion R/W
	__u32 reg_layout_ver;    // 1

	__u32 ext_offset;	 // Extension Block 
	__u32 ext_size;		 // Extension Block Size
} __packed;	
	
struct xaccel_function
{
	struct xaccel_dev *parent;
	struct xaccel_func_desc desc;
	void __iomem *regs;
	struct cdev cdev;
	dev_t devt;
	struct device *device;
	struct mutex lock;
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
static inline bool xaccel_func_has_cap(const struct xaccel_func_desc *head, __u32 cap)
{
	if (head == NULL) return 0;
	return (head->caps & cap);
}

// Does this function use interrupts
static inline bool xaccel_funct_has_irq(const struct xaccel_func_desc *head)
{
	if (head == NULL) return 0;
	return (head->irq_index != XACCEL_IRQ_NONE);
}

#endif
