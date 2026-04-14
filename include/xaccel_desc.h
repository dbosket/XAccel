/*
 * Author: Taco
 * Purpose: This file defines the core format for descriptors used by
 * 	the accelerators to describe their capabilities to xaccel.
 * Date Created: 26 March 2026
 */
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

#define MAX_VERSION_SUPPORTED 1     // The latest version of descriptor supported
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
	struct xaccel_device *parent;
	struct xaccel_func_desc desc;
	void __iomem *regs;
	struct cdev cdev;
	dev_t devt;
	struct device *device;
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

