#include "xaccel_debug_sw.h"


int gen_xaccel_test_obj(enum test_case test, void** buf_out)
{
	if (!buf_out)
	{
		pr_info("ERROR: Buffer out is NULL\n");
		return -EINVAL;
	}
	*buf_out = NULL;

	pr_info("The test case is %d...\n", test);

	__u8 func_desc_sz = sizeof(struct xaccel_func_desc);	
	__u8 desc_header_sz = sizeof(struct xaccel_desc_header);

	struct xaccel_desc_header head;
	struct xaccel_func_desc fdesc[2];

	switch (test)	
	{

	    	// TEST CASE 1: One function, no irq, no, extension, properly formatted	
		case ONE_FUNCTION:
	        
			head.magic         = XACCEL_DESC_MAGIC;
			head.version       = 8;
			head.header_size   = sizeof(struct xaccel_desc_header);
			head.total_size    = sizeof(struct xaccel_desc_header) + sizeof(struct xaccel_func_desc);
			head.num_functions = 1;
			head.flags         = 0xF;
			head.checksum      = 0x11111111;
			head.device_id     = 0x1FFFFFFF;

			fdesc[0].func_id        = 0;
			fdesc[0].func_type      = 7;
			fdesc[0].func_version   = 1;
			fdesc[0].irq_index      = 0;
			fdesc[0].mmio_offset    = sizeof(struct xaccel_desc_header);
			fdesc[0].mmio_size      = sizeof(struct xaccel_func_desc);
			fdesc[0].caps           = XACCEL_CAP_MMIO_RW;
			fdesc[0].reg_layout_ver = 1;
			fdesc[0].ext_offset     = 0;
			fdesc[0].ext_size       = 0;
			break;

		// TEST CASE 2: Two Function, no irq no, extension, properly formatted 
		case TWO_FUNCTION:

			head.magic         = XACCEL_DESC_MAGIC;
			head.version       = 9;
			head.header_size   = sizeof(struct xaccel_desc_header);
			head.total_size    = sizeof(struct xaccel_desc_header) + sizeof(struct xaccel_func_desc) * 2;
			head.num_functions = 2;
			head.flags         = 0xF;
			head.checksum      = 0x11111111;
			head.device_id     = 0x1FFFFFFF;

			fdesc[0].func_id        = 0;
			fdesc[0].func_type      = 2;
			fdesc[0].func_version   = 1;
			fdesc[0].irq_index      = 0;
			fdesc[0].mmio_offset    = sizeof(struct xaccel_desc_header);
			fdesc[0].mmio_size      = sizeof(struct xaccel_func_desc);
			fdesc[0].caps           = XACCEL_CAP_MMIO_RW;
			fdesc[0].reg_layout_ver = 1;
			fdesc[0].ext_offset     = 0;
			fdesc[0].ext_size       = 0;

			fdesc[1].func_id        = 1;
			fdesc[1].func_type      = 3;
			fdesc[1].func_version   = 1;
			fdesc[1].irq_index      = 0;
			fdesc[1].mmio_offset    = sizeof(struct xaccel_desc_header);
			fdesc[1].mmio_size      = sizeof(struct xaccel_func_desc);
			fdesc[1].caps           = XACCEL_CAP_MMIO_RW;
			fdesc[1].reg_layout_ver = 1;
			fdesc[1].ext_offset     = 0;
			fdesc[1].ext_size       = 0;
			break;

	    	default:
			pr_err("ERROR: Specify positive integer (1-8)\n");
	    		return -1;
	}

	__u32 total_size = desc_header_sz + (func_desc_sz) * head.num_functions;
	pr_info("Allocating space in emulated mmap for descriptor + functions\n...");
        *buf_out = kzalloc(total_size, GFP_KERNEL);

		if (!(*buf_out)){
			pr_err("ERROR: Failed to allocate memeory\n");	
			return -ENOMEM;
		}
		// Generate descriptor header
		if( gen_xaccel_desc_header(*buf_out, head.version, head.total_size, 
					head.num_functions, head.flags, head.checksum, head.device_id))
		{
		    pr_err("ERROR: Failed to crate descriptor header\n");
		    return -1;
		}
		// Generate function header(s)
		void* func_start_region = (__u8*)(*buf_out) + sizeof(struct xaccel_desc_header);
		for (int i=0; i<head.num_functions; i++){
			if ( gen_xaccel_function_desc(func_start_region, fdesc[i].func_id, fdesc[i].func_type,
					fdesc[i].func_version, fdesc[i].irq_index, fdesc[i].mmio_offset,
					fdesc[i].mmio_size, fdesc[i].caps, fdesc[i].ext_offset, fdesc[i].ext_size))
			{
		   	pr_err("ERROR: Failed to create function descriptor\n");
			return -1;
			}
		func_start_region = (__u8*) func_start_region + func_desc_sz;
		}

	pr_info("The address of output buffer's pointer is 0x%p and the address of output buffer is 0x%p", buf_out, *buf_out);
	pr_info("Emulated MMIO successfully...\n");
	return 0;
}


int gen_xaccel_desc_header(void* buf, int16_t version, __u32 total_size, __u16 num_funcs, __u16 flags, __u32 checksum, __u16 device_id)
{
	
	if (!buf) return -EFAULT;

	pr_info("Generating Descriptor Header...\n");
	printk("The address is 0x%p, and offset is %x", buf, XACCEL_MAGIC_OFFSET);
	printk("The value written should be in hex %x", 0xACCE1);
	printk("Writing value...\n");
	xaccel_write32(buf, XACCEL_MAGIC_OFFSET, 0xACCE1);
	printk("The value read at 0x%p, offset %x, is %x...", buf, XACCEL_MAGIC_OFFSET, xaccel_read32(buf, XACCEL_MAGIC_OFFSET));

	xaccel_write16(buf, XACCEL_VERSION_OFFSET, version);
	xaccel_write16(buf, XACCEL_HEAD_SIZE_OFFSET, sizeof(struct xaccel_desc_header));
	xaccel_write32(buf, XACCEL_TOT_SIZE_OFFSET, total_size);
	xaccel_write16(buf, XACCEL_NUM_FUNC_OFFSET, num_funcs);
	xaccel_write16(buf, XACCEL_FLAG_OFFSET, flags);
	xaccel_write32(buf, XACCEL_CHECKSUM_OFFSET, checksum);
	xaccel_write32(buf, XACCEL_DEVICE_ID_OFFSET, device_id);
	return 0;
}

int gen_xaccel_function_desc(void* buf, __u16 id, __u16 type, __u16 version, __u16 irq_index, __u32 mmio_offset, __u32 mmio_size, __u32 capabilities, __u32 ext_offset, __u32 ext_size)
{
	if (!buf) return -EFAULT;

	xaccel_write16(buf, XACCEL_FUNC_ID_OFFSET, id);
	xaccel_write16(buf, XACCEL_FUNC_TYPE_OFFSET, type);
	xaccel_write16(buf, XACCEL_FUNC_VERS_OFFSET, version);
	xaccel_write16(buf, XACCEL_IRQ_INDEX, irq_index);
	xaccel_write32(buf, XACCEL_MMIO_OFFSET, mmio_offset);
	xaccel_write32(buf, XACCEL_MMIO_SIZE, mmio_size);
	xaccel_write32(buf, XACCEL_CAPS, capabilities);
	xaccel_write32(buf, XACCEL_REG_LAYOUT_VER, 1);
	xaccel_write32(buf, XACCEL_EXT_OFFSET, ext_offset);
	xaccel_write32(buf, XACCEL_EXT_SIZE, ext_size);
	return 0;
}

