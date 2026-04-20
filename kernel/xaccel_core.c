#include "xaccel_core.h"



int xaccel_core_init(struct device *dev, void __iomem *base, size_t mmio_size)
{
    return 0;
}

void xaccel_core_cleanup(struct xaccel_dev *xdev)
{
    if (!xdev)
    {
	pr_info("ERROR: Invalid Pointer");
        return;
    }
    
    
    

    


    
    return;
}

int xaccel_core_parse_descriptor(struct xaccel_dev *xdev)
{
    return 0;
}

int xaccel_core_create_function_devices(struct xaccel_dev *xdev)
{
    /*
    if (!xaccel_dev) return -ENODEV;
    
    struct xaccel_function* xfunc = kmalloc(sizeof(struct xaccel_function), GFP_KERNEL);
    if (!xfunc) return -EFAULT;

    xfunc->parent = xaccel_dev;
    xfunc->
    */
    return 0;
}

int xaccel_core_destroy_function_device(struct xaccel_dev *xdev)
{
    return 0;
}

// Is this a valid descriptor
int check_descriptor_header(struct xaccel_desc_header* header)
{
    if (!header) return -EFAULT;
    if (header->magic == XACCEL_DESC_MAGIC) return 0;
    
    pr_info("ERROR: Invalid Header\n");
    return -EFAULT;
}


int build_descriptor_header(void* source_addr, struct xaccel_desc_header* head_out)
{
  if (source_addr)
  {
    head_out = kmalloc(sizeof(struct xaccel_desc_header), GFP_KERNEL);
    if (!head_out)
    {
      pr_err("Failed to Allocate Memory\n");
      return -ENOMEM;
    }
    
    head_out->magic         = ioread32(source_addr + XACCEL_MAGIC_OFFSET);
    head_out->version       = ioread16(source_addr + XACCEL_VERSION_OFFSET);
    head_out->header_size   = ioread16(source_addr + XACCEL_HEAD_SIZE_OFFSET); 
    head_out->total_size    = ioread32(source_addr + XACCEL_TOT_SIZE_OFFSET);
    head_out->num_functions = ioread32(source_addr + XACCEL_NUM_FUNC_OFFSET);
    head_out->flags         = ioread16(source_addr + XACCEL_FLAG_OFFSET);
    head_out->checksum      = ioread32(source_addr + XACCEL_CHECKSUM_OFFSET);
    head_out->device_id     = ioread32(source_addr + XACCEL_DEVICE_ID_OFFSET);
    
  return 0;
  }
  return -EFAULT;
}

int build_function_header(void* source_addr, struct xaccel_func_desc* head_out)
{
    if (source_addr)
    {
	head_out = kmalloc(sizeof(struct xaccel_func_desc), GFP_KERNEL);
	if (!head_out)
	{
        pr_err("Failed to Allocate Memory\n");
	return -ENOMEM;
	}
    }

    head_out->func_id        = ioread16(source_addr + XACCEL_FUNC_ID_OFFSET);
    head_out->func_type      = ioread16(source_addr + XACCEL_FUNC_TYPE_OFFSET);
    head_out->func_version   = ioread16(source_addr + XACCEL_FUNC_VERS_OFFSET);
    head_out->irq_index      = ioread16(source_addr + XACCEL_IRQ_INDEX);
    head_out->mmio_offset    = ioread32(source_addr + XACCEL_MMIO_OFFSET);
    head_out->mmio_size      = ioread32(source_addr + XACCEL_MMIO_SIZE);
    head_out->caps           = ioread32(source_addr + XACCEL_CAPS);
    head_out->reg_layout_ver = ioread32(source_addr + XACCEL_REG_LAYOUT_VER);
    head_out->ext_offset     = ioread32(source_addr + XACCEL_EXT_OFFSET);
    head_out->ext_size       = ioread32(source_addr + XACCEL_EXT_SIZE);

    return 0;
}
