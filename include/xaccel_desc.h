/*
 * Author: Taco
 * Purpose: This file defines the core format for descriptors used by
 * 	the accelerators to describe their capabilities to xaccel.
 * Date Created: 26 March 2026
 */
#ifdef XACCEL_DESC_H
#define XACCEL_DESC_H

#define XACCEL_DESC_MAGIC 0x000ACCE1
#define XACCEL_DESC_VER 1U
#define XACCEL_IRQ_NONE 0xFFFFU

//Global Descriptor Flags
#define XACCEL_DESC_F_HAS_CHECKSUM    BIT(0)
#define XACCEL_DESC_F_HAS_EXTENSIONS  BIT(1)

// Bitmask to describe capabilities
#define XACCEL_CAP_MMIO_RW (1U << 0) // Can this func R/W from memory
#define XACCEL_CAP_START_STOP (1U << 1) // Does SW need to start this accelerator
#define XACCEL_XACCEL_CAP_STATUS (1U << 2) // What is the status of this accelerator

#define XACCEL_CAP_IRQ (1U << 3) // Can this function notify interrupt on completion?
#define XACCEL_CAP_RESET (1U << 4) // Can this be reset to known state
#define XACCEL_CAP_BULK_INPUT (1U << 5) // Can this function consume larger inputs versus just a few registers
#define XACCEL_CAP_BULK_OUTPUT (1U << 6) // Does this function product larage outputs

#define MAX_VERSION_SUPPORTED 1`// The latest version of descriptor supported
=======
#define XACCEL_MAGIC_OFFSET     0x0
#define XACCEL_VERSION_OFFSET   0x4
#define XACCEL_HEAD_SIZE_OFFSET 0x6
#define XACCEL_TOT_SIZE_OFFSET  0x8
#define XACCEL_NUM_FUNC_OFFSET  0x12
#define XACCEL_FLAG_OFFSET      0x14
#define XACCEL_CHECKSUM_OFFSET  0x16
#define XACCEL_DEVICE_ID_OFFSET 0x20

struct xaccel_desc_header 
{
	uint32_t magic;		// 'XACC'
	uint16_t version;	// v1.0
	uint16_t header_size;	// sizeof(header)	
	uint32_t total_size;	// full descriptor in bytes
	uint16_t num_functions; // number of functions device has
	uint16_t flags;		// global flags
	uint32_t checksum;
	uint32_t device_id;	// Image ID
} __packed;


struct xaccel_func_desc
{
	uint16_t func_id;	// ID is device specific 0,1,2...
	uint16_t func_type;	//
	uint16_t func_version;	// What version of the ADDITION func is this?
	uint16_t irq_index;	// For accelerators requiring interrupts

	uint32_t mmio_offset; 	// e.g. 0x0F000000
	uint32_t mmio_size;	// e.g. 0x1000

	uint32_t caps		// Capabilities of this fucntion R/W
	uint32_t reg_layout_ver	// 1

	uint32_t ext_offset	// Extension Block 
	uint32_t size		// Extension Block Size
} __packed;	
	

// Is this Descriptor Header Valid?
static inline int xaccel_desc_magic_valid(const struct xaccel_desc_header *head)
{
	if (head == NULL) return 0;

	return head->magic == XACCEL_DESC_MAGIC;
}

// Is this Descriptor Version Supported?
static inline int xaccel_desc_version_valid(const struct xaccel_desc_header *head)
{
	if (head == NULL) return 0;
	return ( (head->version) && (head->version <= MAX_VERSION_SUPPORTED));
		 
}

// Does this function have capabilities 
static inline bool xaccel_func_has_cap(const struct xaccel_func_desc *fd, uint32_t cap)
{
	if (head == NULL) return 0;
	return (head->caps > 0);
}

// Does this function use interrupts
static inline bool xaccel_funct_has_irq(const struct xaccel_fun_desc *fd);
{
	if (head == NULL) return 0;
	return (head->irq_index != XACCEL_IRQ_NONE);
}




