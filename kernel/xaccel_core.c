#include "xaccel_core.h"



int xaccel_core_init(struct device *dev, void __iomem *base, size_t mmio_size)
{
    return 0;
}

void xaccel_core_cleanup(struct xaccel_device *xdev)
{
    return;
}

int xaccel_core_parse_descriptor(struct xaccel_device *xdev)
{
    return 0;
}

int xaccel_core_create_function_devices(struct xaccel_device *xdev)
{
    return 0;
}

int xaccel_core_destroy_function_device(struct xaccel_device *xdev)
{
  return 0;
}

// Is this a valid descriptor
int check_descriptor_header(xaccel_desc_header* header)
{
    if (!header) return -1;
    if (header->magic == XACCEL_DESC_MAGIC) return 0;
    
    pr_info(stderr, "ERROR: Invalid Header\n");
    return -1;
}


void build_descriptor_header(void* source_addr, xaccel_desc_header* head_out)
{
  if (source_addr){

    head_out = kmalloc(sizeof(xaccel_desc_header), GFP_KERNEL);
    if (!head_out){
      pr_err("Failed to Allocate Memory\n");
      return -ENOMEM;
    }
    
    //TODO: Potentially Read as Full 128 or 256 Bytes (1 bus access instead of ~6)
    //TODO: If read as full byte stack then do byte arithmetic in Software
    head_out->magic         = ioread32(source_addr);
    head_out->version       = ioread16(source_addr + XACCEL_VERSION_OFFSET);
    head_out->header_size   = ioread16(source_addr + XACCEL_HEAD_SIZE_OFFSET); 
    head_out->total_size    = ioread32(source_addr + XACCEL_TOT_SIZE_OFFSET);
    head_out->num_functions = ioread32(source_addr + XACCEL_NUM_FUNCTIONS_OFFSET);
    head_out->flags         = ioread16(soruce_addr + XACCEL_FLAG_OFFSET);
    head_out->checksum      = ioread32(source_addr + XACCEL_CHECKSUM_OFFSET);
    head_out->device_id     = ioread32(source_addr + XACCEL_DEVICE_ID_OFFSET);
    
    
  return head_out;
}

void build_function_header(void* source_addr, xaccel_func_header* head_out)
{
  return;
}
