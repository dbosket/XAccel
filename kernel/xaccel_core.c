#include "xaccel_core.h"


// Creates an instance of xaccel device base on mmio region
int xaccel_create_instance(void* base_addr, struct xaccel_dev* xdev, struct file_operations xaccel_fops)
{
	pr_info("Creating instance of xaccel device");

	if (!base_addr)
	{
		pr_err("ERROR: Invalid Base Addr\n");
		return -EFAULT;
	}

	// Allocate Space for the Device                                                                            
        pr_info("Allocating space for device object\n");                                                            
        xdev = kzalloc(sizeof(*xdev), GFP_KERNEL);

	if(!xdev)
	{
		pr_err("Failed to allocate memory\n");
		return -ENOMEM;
	}
	xdev->funcs = NULL; //Init to null for later checks
	
#ifndef NO_HW
	//TODO: Initialize mmio_base, mmio_size, from pdev?
#endif
	
	// Initialize Semaphore
	pr_info("Initializaing the semaphore\n");
	sema_init(&(xdev->sem), MAX_LOCK_HOLDERS);
	down(&(xdev->sem));

	// Build and Parse Descriptor Header
	struct xaccel_desc_header* desc_head;
	if (xaccel_build_header(base_addr, &desc_head) || xaccel_check_header(desc_head))
	{
		pr_info("ERROR: Failed to build descripter header...");
		xaccel_cleanup(xdev);
		return -1;
		
	}	
	xdev->hdr = desc_head;
	

	// Allocating space for function array
	xdev->funcs = kcalloc(desc_head->num_functions, sizeof(struct xaccel_function), GFP_KERNEL);
	if (!xdev->funcs)
		{
			pr_info("ERROR: Failed to allocate memory for function array\n");
			xaccel_cleanup(xdev);
			return -ENOMEM;
		}

	pr_info("Allocating device numbers for character devices\n");
	if (alloc_chrdev_region(&(xdev->base_devt), FIRST_MINOR, xdev->num_functions, XACCEL_NAME))
		{
			pr_err("ERROR: alloc_chrdev_region_failed...\n");
			xaccel_cleanup(xdev);
			return -1;
		}
	// Creating class for xaccel device
	pr_info("Creating class for xaccel devices\n");
	xdev->class = class_create(XACCEL_CLASS_NAME);


	struct xaccel_func_desc *temp_f_desc;
	struct xaccel_function  *temp_func;
	
	// Initializing function array
	pr_info("Initializing the function array for this xaccel_device\n");
	for(__u32 i=0; i<xdev->num_functions; i++){

		if (xaccel_build_function_descriptor(base_addr, &temp_f_desc))
		{
			pr_err("ERROR: Failed to allocate memory for function header array\n");
			xaccel_cleanup(xdev);
			return -ENOMEM;
		}

		if (xaccel_create_function_device(xdev, temp_f_desc, temp_func))
		{
			pr_err("ERROR: Failed to build function devices\n");
			xaccel_cleanup(xdev);
			return -1;
		}

		// Initialize character device for current function
		
		cdev_init(&(temp_func->cdev), &(xaccel_fops));
		temp_func->cdev.owner = THIS_MODULE;
		temp_func->devt = MKDEV(MAJOR(xdev->base_devt), 0);
		cdev_add(&(temp_func->cdev), temp_func->devt, 1);
		temp_func->device = device_create(xdev->class, NULL, temp_func->devt, NULL, "xaccel%d_func%d", 0, i);
		xdev->funcs[i] = *temp_func;
	}
	
	up(&(xdev->sem));
	pr_info("XACCEL_CREATE_INSTANCE() returning successfully...\n");
	return 0;
}

void xaccel_cleanup(struct xaccel_dev *xdev)
{
	pr_info("xaccel_cleanup() starting...");
	if(!xdev) return;

	// Release function objects and cdevs	
	if (xdev->funcs)
	{
		struct xaccel_function* temp_func;
		for (__u32 i=0; i<xdev->num_functions; i++)
		{
			temp_func = &(xdev->funcs[i]);
			device_destroy(xdev->class, temp_func->devt);
			pr_info("Deleting char device for func[%d]\n", i);
			cdev_del(&(temp_func->cdev));
		}
		kfree(xdev->funcs);
		xdev->funcs = NULL;
	}	
	if (xdev->class)
		class_destroy(xdev->class);
	unregister_chrdev_region(xdev->base_devt, xdev->num_functions);
	kfree(xdev);
	xdev = NULL;
	pr_info("XACCEL_CLEANUP() Returning Successfully...\n");
	return;
}

// Is this descriptor valid
int xaccel_check_header(struct xaccel_desc_header* header)
{
	if (!header) return -EFAULT;
	if (header->magic == XACCEL_DESC_MAGIC) return 0;
    
	pr_info("ERROR: Invalid Header\n");
	return -EFAULT;
}


// Reading a descriptor from mmap address specified at soruce_addr, and saves to head_out
int xaccel_build_header(void* source_addr, struct xaccel_desc_header** head_out)
{
	if (!head_out)
		return -EFAULT;
	*head_out = NULL;

  	if (source_addr)
  	{
    		*head_out = kzalloc(sizeof(struct xaccel_desc_header), GFP_KERNEL);
		if (!head_out)
		{
			pr_err("Failed to Allocate Memory\n");
			return -ENOMEM;
		}
    
		(*head_out)->magic         = xaccel_read32(source_addr, XACCEL_MAGIC_OFFSET);
		pr_info("MAGIC Value is: %x...\n", (*head_out)->magic);
    		(*head_out)->version       = xaccel_read16(source_addr, XACCEL_VERSION_OFFSET);
    		(*head_out)->header_size   = xaccel_read16(source_addr, XACCEL_HEAD_SIZE_OFFSET); 
    		(*head_out)->total_size    = xaccel_read32(source_addr, XACCEL_TOT_SIZE_OFFSET);
    		(*head_out)->num_functions = xaccel_read32(source_addr, XACCEL_NUM_FUNC_OFFSET);
    		(*head_out)->flags         = xaccel_read16(source_addr, XACCEL_FLAG_OFFSET);
    		(*head_out)->checksum      = xaccel_read32(source_addr, XACCEL_CHECKSUM_OFFSET);
    		(*head_out)->device_id     = xaccel_read32(source_addr, XACCEL_DEVICE_ID_OFFSET);
    
  		return 0;
	}
	return -1;
}

// Reading a function descriptor from mmap address specified at source addr, and saves to header 
int xaccel_build_function_descriptor(void* source_addr, struct xaccel_func_desc** desc_out)
{
	if (source_addr)
	{
		*desc_out = kmalloc(sizeof(struct xaccel_func_desc), GFP_KERNEL);
		if (!desc_out)
		{
        		pr_err("Failed to Allocate Memory\n");
			return -ENOMEM;
		}

    		(*desc_out)->func_id        = xaccel_read16(source_addr, XACCEL_FUNC_ID_OFFSET);
    		(*desc_out)->func_type      = xaccel_read16(source_addr, XACCEL_FUNC_TYPE_OFFSET);
    		(*desc_out)->func_version   = xaccel_read16(source_addr, XACCEL_FUNC_VERS_OFFSET);
    		(*desc_out)->irq_index      = xaccel_read16(source_addr, XACCEL_IRQ_INDEX);
    		(*desc_out)->mmio_offset    = xaccel_read32(source_addr, XACCEL_MMIO_OFFSET);
    		(*desc_out)->mmio_size      = xaccel_read32(source_addr, XACCEL_MMIO_SIZE);
    		(*desc_out)->caps           = xaccel_read32(source_addr, XACCEL_CAPS);
    		(*desc_out)->reg_layout_ver = xaccel_read32(source_addr, XACCEL_REG_LAYOUT_VER);
    		(*desc_out)->ext_offset     = xaccel_read32(source_addr, XACCEL_EXT_OFFSET);
    		(*desc_out)->ext_size       = xaccel_read32(source_addr, XACCEL_EXT_SIZE);

    		return 0;
	}
	return -1;
}



int xaccel_create_function_device(struct xaccel_dev *xdev, struct xaccel_func_desc *fdesc, struct xaccel_function* func_obj)
{
    return 0;
}

int xaccel_destroy_function_device(struct xaccel_dev *xdev)
{
    return 0;
}


