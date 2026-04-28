#include "xaccel_debug_sw.h"


int gen_xaccel_test_obj(enum test_case test, void** buf_out, size_t *region_size_out)
{
	if (!buf_out)
	{
		pr_info("ERROR: Buffer out is NULL\n");
		return -EINVAL;
	}
	*buf_out = NULL;

	pr_info("The test case is %d...\n", test);

	__u32 func_desc_sz = sizeof(struct xaccel_func_desc);	
	__u32 desc_header_sz = sizeof(struct xaccel_desc_header);
	__u32 alloc_size = 0;

	struct xaccel_desc_header head;
	struct xaccel_func_desc fdesc[2];
	switch (test)	
	{
	    	// TEST CASE 1: One function, no irq, no, extension, properly formatted	
		case ONE_FUNCTION:
			head.magic         = XACCEL_DESC_MAGIC;
			head.version       = 1;
			head.header_size   = sizeof(struct xaccel_desc_header);
			head.total_size    = sizeof(struct xaccel_desc_header) + sizeof(struct xaccel_func_desc);
			head.num_functions = 1;
			head.flags         = 0x0;
			head.checksum      = 0x0;
			head.device_id     = 1;

			fdesc[0].func_id        = 0;
			fdesc[0].func_type      = XACCEL_FUNC_TYPE_VECTOR_ADD;
			fdesc[0].func_version   = 1;
			fdesc[0].irq_index      = 0;
			fdesc[0].mmio_offset    = XACCEL_TEST_FUNC0_MMIO_OFFSET;
			fdesc[0].mmio_size      = XACCEL_TEST_MMIO_SIZE;
			fdesc[0].caps           = XACCEL_CAP_MMIO_RW;
			fdesc[0].reg_layout_ver = 0x1;
			fdesc[0].ext_offset     = 0x0;
			fdesc[0].ext_size       = 0x0;

			alloc_size = XACCEL_TEST_ONE_FUNC_REGION_SIZE;
			break;

		// TEST CASE 2: Two Function, no irq no, extension, properly formatted 
		case TWO_FUNCTION:
			head.magic         = XACCEL_DESC_MAGIC;
			head.version       = 1;
			head.header_size   = sizeof(struct xaccel_desc_header);
			head.total_size    = sizeof(struct xaccel_desc_header) + 
					     2 * sizeof(struct xaccel_func_desc);
			head.num_functions = 2;
			head.flags         = 0x0;
			head.checksum      = 0x0;
			head.device_id     = 9;

			fdesc[0].func_id        = 0;
			fdesc[0].func_type      = XACCEL_FUNC_TYPE_VECTOR_ADD;
			fdesc[0].func_version   = 1;
			fdesc[0].irq_index      = 0;
			fdesc[0].mmio_offset    = XACCEL_TEST_FUNC0_MMIO_OFFSET;
			fdesc[0].mmio_size      = XACCEL_TEST_MMIO_SIZE;
			fdesc[0].caps           = XACCEL_CAP_MMIO_RW;
			fdesc[0].reg_layout_ver = 0x1;
			fdesc[0].ext_offset     = 0x0;
			fdesc[0].ext_size       = 0x0;

			fdesc[1].func_id        = 1;
			fdesc[1].func_type      = XACCEL_FUNC_TYPE_SHA256_BLOCK;
			fdesc[1].func_version   = 1;
			fdesc[1].irq_index      = 0;
			fdesc[1].mmio_offset    = XACCEL_TEST_FUNC1_MMIO_OFFSET;
			fdesc[1].mmio_size      = XACCEL_TEST_MMIO_SIZE;
			fdesc[1].caps           = XACCEL_CAP_MMIO_RW;
			fdesc[1].reg_layout_ver = 0x1;
			fdesc[1].ext_offset     = 0x0;
			fdesc[1].ext_size       = 0x0;

			alloc_size = XACCEL_TEST_TWO_FUNC_REGION_SIZE;
			break;

	    	default:
			pr_err("ERROR: Specify positive integer (1-8)\n");
	    		return -1;
	}

	__u32 desc_blob_size = desc_header_sz + (func_desc_sz) * head.num_functions;
	pr_info("The allocated size for test 2 is 0x%x\n", alloc_size);

	if (head.total_size != desc_blob_size) 
	{
		pr_err("ERROR: descriptor total_size mismtach: header says %d, computed %d",
				head.total_size, desc_blob_size);
		return -EINVAL;
	}

	if (!alloc_size)
	{
		pr_err("ERROR: alloc_size was not set for this test case %d\n", test);
		return -EINVAL;
	}
	
	if (alloc_size < head.total_size)
	{
		pr_err("ERROR: alloc_size smaller than descriptor blob\n");
		return -EINVAL;
	}

	pr_info("Allocating emulated XACCEL region: desc_size=%u alloc_size=%u\n",
			head.total_size, alloc_size);

	*buf_out = kzalloc(alloc_size, GFP_KERNEL);
	*region_size_out = alloc_size;
	if (!(*buf_out))
	{
		pr_err("ERROR: Failed to allocate memeory\n");	
		return -ENOMEM;
	}

	// Generate descriptor header
	pr_info("Generating the descriptor headers");
	if( gen_xaccel_desc_header(*buf_out, head.version, head.total_size, 
					head.num_functions, head.flags, head.checksum, head.device_id))
	{
		pr_err("ERROR: Failed to crate descriptor header\n");
		return -1;
	}
	// Generate function header(s)
	pr_info("Generating the function headers");
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
	pr_info("The address is 0x%p, and offset is %x", buf, XACCEL_MAGIC_OFFSET);
	pr_info("The value written should be in hex %x", 0xACCE1);
	pr_info("Writing value...\n");
	xaccel_write32(buf, XACCEL_MAGIC_OFFSET, XACCEL_DESC_MAGIC);
	pr_info("The value read at 0x%p, offset %x, is %x...", buf, XACCEL_MAGIC_OFFSET, xaccel_read32(buf, XACCEL_MAGIC_OFFSET));

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

