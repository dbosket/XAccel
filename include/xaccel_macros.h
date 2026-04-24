#ifndef XACCEL_MACROS_H
#define XACCEL_MACROS_H

#define LICENSE "GPL"
#define AUTHOR "Taco"
#define DESCRIPTION "Accelerator Linux Kernel Module"

#define FIRST_MINOR 0
#define NUM_DEV_REQUESTED 1

#define MAX_LOCK_HOLDERS 1

#define XACCEL_DESC_MAGIC 0x000ACCE1
#define XACCEL_DESC_VER 1U
#define XACCEL_IRQ_NONE 0xFFFFU

#define XACCEL_NAME "xaccel"
#define XACCEL_CLASS_NAME "xaccel" 

#define NO_HW 1
#define DEBUG 1

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

// XACCEL Descriptor Header Offsets
#define MAX_VERSION_SUPPORTED   1     // The latest version of descriptor supported
#define XACCEL_MAGIC_OFFSET     0x00
#define XACCEL_VERSION_OFFSET   0x04
#define XACCEL_HEAD_SIZE_OFFSET 0x06
#define XACCEL_TOT_SIZE_OFFSET  0x08
#define XACCEL_NUM_FUNC_OFFSET  0x0C
#define XACCEL_FLAG_OFFSET      0x0E
#define XACCEL_CHECKSUM_OFFSET  0x10
#define XACCEL_DEVICE_ID_OFFSET 0x14

// XACCEL Function Descriptor Offsets
#define XACCEL_FUNC_ID_OFFSET   0x00
#define XACCEL_FUNC_TYPE_OFFSET 0x02
#define XACCEL_FUNC_VERS_OFFSET 0x04
#define XACCEL_IRQ_INDEX        0x06
#define XACCEL_MMIO_OFFSET      0x08
#define XACCEL_MMIO_SIZE        0x0C
#define XACCEL_CAPS             0x10
#define XACCEL_REG_LAYOUT_VER   0x14
#define XACCEL_EXT_OFFSET	0x18
#define XACCEL_EXT_SIZE		0x1C

#endif
