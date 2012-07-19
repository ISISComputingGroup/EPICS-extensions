#Dual Purpose Makefile to support kernel module builds
ifneq ($(KERNELRELEASE),)
obj-m       := galilpci.o

else
KDIR        := /lib/modules/$(shell uname -r)/build
PWD         := $(shell pwd)

default:
	$(MAKE) -C $(KDIR) SUBDIRS=$(PWD) modules

clean:
	$(MAKE) -C $(KDIR) SUBDIRS=$(PWD) clean

install:
	$(MAKE) -C $(KDIR) SUBDIRS=$(PWD) modules_install

endif
