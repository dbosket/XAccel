
obj-m += xaccel.o



# Objects that make up xaccel.ko
xaccel-y := kernel/xaccel_driver.o \
	kernel/xaccel_core.o \
	kernel/xaccel_debug_sw.o


# Shared Project Headers
ccflags-y += -I$(src)/include 
ccflags-y += -I$(src)/kernel



# Running kernel build tree
KDIR ?= /lib/modules/$(shell uname -r)/build


# Smoke test
SMOKE_SRC := tests/smoketest.c
SMOKE_BIN := tests/smoketest

# Current repo root
PWD := $(shell pwd)


all:
	$(MAKE) -C $(KDIR) M=$(PWD) modules

clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean
	rm -rf Module.symvers module.order

modules_install:
	$(MAKE) -C $(KDIR) M=$(PWD) modules_install


smoke:
	gcc -Wall -I$(PWD)/include -o $(SMOKE_BIN) $(SMOKE_SRC)

