#include <stdio.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <stdlib.h>
#include "../kernel/xaccel_uapi.h"

int main (int argc, char** argv)
{
    int ret_val;
    int fd = open("/dev/xaccel0_func0", O_RDWR);
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
    ret_val = ioctl(fd, XACCEL_IOC_READ_REG, &req);

    if (!ret_val)
    {
        fprintf(stderr, "ERROR: ioctl() failed...\n");
	perror("ioctl");
	return EXIT_FAILURE;
    }
    printf("val = 0x%x\n", req.value);
    close(fd);

    return 0;
}
