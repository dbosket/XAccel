#include <stdio.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <stdlib.h>
#include "../kernel/xaccel_uapi.h"

int xaccel_ioc_get_info(int fd);
int xaccel_ioc_read(int fd, struct xaccel_reg_io* dest_buf);
int xaccel_ioc_write(int fd, void* src_buf);


int xaccel_ioc_get_info(int fd)
{
       struct xaccel_info f_info = {-1, -1, -1, -1, -1, -1, -1};

       if (ioctl(fd, XACCEL_IOC_GET_INFO, &f_info))
       {
	       perror("ERROR: ioctl(get_info) failed...\n");
	       return EXIT_FAILURE;
       }

       // Printing the info recived
       fprintf(stdout, "FUNC_ID            =%d\n", f_info.func_id);
       fprintf(stdout, "FUNC_TYPE          =%d\n", f_info.func_type);
       fprintf(stdout, "FUNC_VERSION       =%d\n", f_info.func_version);
       fprintf(stdout, "IRQ_INDEX          =%d\n", f_info.irq_index);
       fprintf(stdout, "MMIO_SIZE          =%d\n", f_info.mmio_size);
       fprintf(stdout, "CAPABILITIES       =%d\n", f_info.caps);
       fprintf(stdout, "REG_LAYOUT_VERSION =%d\n", f_info.reg_layout_ver);

       return EXIT_SUCCESS;

}

int xaccel_ioc_read(int fd, struct xaccel_reg_io *req)
{
    if (ioctl(fd, XACCEL_IOC_READ_REG, req))
    {
        perror("ERROR: ioctl(read) failed...\n");
	return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}


int main (int argc, char** argv)
{

    if (argc < 2)
    {
	    fprintf(stderr, "ERROR: specify the device you want to use!\n");
	    return EXIT_FAILURE;
    }
    fprintf(stdout, "First command arg is %s\n", argv[1]);
    int fd = open(argv[1], O_RDWR);
    if (fd < 0)
    {
	    fprintf(stderr, "ERROR: open() failed...\n");
	    return EXIT_FAILURE;
    }
   
    fprintf(stdout, "Open xaccel0_func0 opened successfully...\n"); 
    struct xaccel_reg_io req = {
	    .offset = 0x0,
    };

    fprintf(stdout, "Running ioctl()...\n");

    // Support command line args
    if (argc)
    {
    
        switch (atoi(argv[2])){
	    // Get call get info command 
	    case 0:
		xaccel_ioc_get_info(fd);
		break;
	    // Call read command
            case 1:
		if (!xaccel_ioc_read(fd, &req))
		    printf("val = 0x%x\n", req.value);
		break;
	    // Call write command
            case 2:
		break;
	    default:
		xaccel_ioc_get_info(fd);
		break;
        }
    close(fd);

    return 0;
    }
}
