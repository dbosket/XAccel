#include "xaccel_debug_sw.h"


int gen_xaccel_test_obj(enum test_case test, void* buf_out)
{
	if (!test) 
	{
	    pr_err("ERROR: Specify positive integer\n");
	    return -1;
	}
	//
	__u16 func_desc_sz = sizeof(struct xaccel_func_desc);	
	__u16 desc_header_sz = sizeof(struct xaccel_desc_header);

	switch (test)	
	{

	    // TEST CASE 1: One function, no irq, no, extension, properly formatted	
	    case ONE_FUNCTION:
	        
		__u32 total_size = desc_header_sz + func_desc_sz;
                buf_out = kzalloc(total_size, GFP_KERNEL);

		if (!buf_out){
			pr_err("ERROR: Failed to allocate memeory\n");	
			return -ENOMEM;
		}
		// Generate descriptor header
		if( gen_xaccel_desc_header(buf_out, 1, total_size, 1, 0x0, 1234, 7777))
		{
		    pr_err("ERROR: Failed to crate descriptor header\n");
		    return -1;
		}
		// Generate function header
		if ( gen_xaccel_function_desc(buf_out, 1, 0, 0, 0, 0x0, 0x100, XACCEL_CAP_MMIO_RW, 0x0, 0x0))
		{
		    pr_err("ERROR: Failed to create function descriptor\n");
		    return -1;
		}

		break;

		
	    default:
		pr_err("ERROR: Specify positive integer (1-8)\n");
	    	return -1;

	}
	return 0;
}


int gen_xaccel_desc_header(void* buf, int16_t version, __u32 total_size, __u16 num_funcs, __u16 flags, __u32 checksum, __u16 device_id)
{
	
	if (!buf) return -EFAULT;

	write32(buf, XACCEL_MAGIC_OFFSET, 0xACCE1);
	write16(buf, XACCEL_VERSION_OFFSET, 7);
	write16(buf, XACCEL_HEAD_SIZE_OFFSET, sizeof(struct xaccel_desc_header));
	write32(buf, XACCEL_TOT_SIZE_OFFSET, total_size);
	write16(buf, XACCEL_NUM_FUNC_OFFSET, num_funcs);
	write16(buf, XACCEL_FLAG_OFFSET, flags);
	write32(buf, XACCEL_CHECKSUM_OFFSET, checksum);
	write32(buf, XACCEL_DEVICE_ID_OFFSET, device_id);
	return 0;
}

int gen_xaccel_function_desc(void* buf, __u16 id, __u16 type, __u16 version, __u16 irq_index, __u32 mmio_offset, __u32 mmio_size, __u32 capabilities, __u32 ext_offset, __u32 ext_size)
{
	if (!buf) return -EFAULT;

	write16(buf, XACCEL_FUNC_ID_OFFSET, id);
	write16(buf, XACCEL_FUNC_TYPE_OFFSET, type);
	write16(buf, XACCEL_FUNC_VERS_OFFSET, version);
	write16(buf, XACCEL_IRQ_INDEX, irq_index);
	write32(buf, XACCEL_MMIO_OFFSET, mmio_offset);
	write32(buf, XACCEL_MMIO_SIZE, mmio_size);
	write32(buf, XACCEL_CAPS, capabilities);
	write32(buf, XACCEL_REG_LAYOUT_VER, 1);
	write32(buf, XACCEL_EXT_OFFSET, ext_offset);
	write32(buf, XACCEL_EXT_SIZE, ext_size);
	return 0;
}

