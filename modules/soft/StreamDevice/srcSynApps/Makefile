TOP=../..

include $(TOP)/configure/CONFIG

-include ../src/CONFIG_STREAM
-include ../../src/CONFIG_STREAM

# For vxWorks, we need an OBJLIB.
# Otherwise bus and converter registry does not work.
LIBRARY_DEFAULT = streamSynApps
LIBRARY_vxWorks = -nil-
OBJLIB_vxWorks = $(LIBRARY_DEFAULT)
OBJLIB_SRCS = $(SRCS)

DBD += $(LIBRARY_DEFAULT).dbd

ifeq ($(LOADABLE_MODULE),YES)
SRCS += $(LIBRARY_DEFAULT)_registerRecordDeviceDriver.cpp
endif 
SRCS += $(SYNAPPS_RECORDS:%=dev%Stream.c)

LIB_LIBS += stream

include $(TOP)/configure/RULES

# create streamSynApps.dbd from all SYNAPPS_RECORDS
$(COMMON_DIR)/$(LIBRARY_DEFAULT).dbd:
	@for r in $(SYNAPPS_RECORDS); \
	do echo "device($$r,INST_IO,dev$${r}Stream,\"stream\")"; \
	done > $@
