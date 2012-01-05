TOP=../..

include $(TOP)/configure/CONFIG

-include CONFIG_STREAM
-include ../CONFIG_STREAM

USR_CXXFLAGS_windows-x64=/DSTREAM_EXPORTS=1
USR_CFLAGS_windows-x64=/DSTREAM_EXPORTS=1

# For vxWorks, we need an OBJLIB.
# Otherwise bus and converter registry does not work.
LIBRARY_DEFAULT = stream
LIBRARY_vxWorks = -nil-
OBJLIB_vxWorks = $(LIBRARY_DEFAULT)
OBJLIB_SRCS = $(SRCS)

DBD += $(LIBRARY_DEFAULT).dbd

ifdef ASYN_VERS
LIB_LIBS += asyn

ifeq ($(ASYN_COMPAT_$(ASYN_VERS)),)
ASYN_COMPAT_$(ASYN_VERS)=$(ASYN_VERS)
endif

BUSSES += AsynDriver$(ASYN_COMPAT_$(ASYN_VERS))
endif

ifeq ($(LOADABLE_MODULE),YES)
SRCS += $(LIBRARY_DEFAULT)_registerRecordDeviceDriver.cpp
endif 
SRCS += $(STREAM_SRCS)
SRCS += $(RECORDS:%=dev%Stream.c)
SRCS += $(BUSSES:%=%Interface.cc)
SRCS += $(FORMATS:%=%Converter.c)

LIB_LIBS += Com dbIoc dbStaticIoc registryIoc 

# memguard looks for memory leaks (gcc only)
# CPPFLAGS += -include ../memguard.h
# LIB_SRCS += memguard.cc

INC += devStream.h

include $(TOP)/configure/RULES

# Update version string (contains __DATE__ and __TIME__)
# each time make runs.
StreamVersion$(OBJ): FORCE
FORCE:

# create stream.dbd from all RECORDS
$(COMMON_DIR)/$(LIBRARY_DEFAULT).dbd:
	perl -e "foreach $$r(split(/ /,\"$(RECORDS)\")) { print \"device($$r,INST_IO,dev$${r}Stream,\\\"stream\\\")\n\" }" > $@
	perl -e "print \"driver(stream)\nvariable(streamDebug, int)\nregistrar(streamRegistrar)\n\"" >> $@