# Makefile for gpiodemo kernel module

# Kernel build system
KDIR := /lib/modules/$(shell uname -r)/build
PWD  := $(shell pwd)


obj-m += Task2_driver.o

all:
	$(MAKE) -C $(KDIR) M=$(PWD) modules

clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean

