TOP=../..

include $(TOP)/configure/CONFIG

HOST_OPT = NO

PROD = streamApp
DBD  = streamApp.dbd

streamApp_DBD += base.dbd

PROD_SRCS += streamApp_registerRecordDeviceDriver.cpp
PROD_SRCS_DEFAULT = streamAppMain.cc
PROD_SRCS_vxWorks = -nil-

# For vxWorks use the OBJLIB
PROD_LIBS_DEFAULT = stream
PROD_LIBS_vxWorks = -nil-
PROD_OBJS_vxWorks = $(PROD_LIBS_DEFAULT:%=$(INSTALL_BIN)/%Library)

ifdef ASYN
# Which types of asyn busses do you have?
streamApp_DBD += drvAsynSerialPort.dbd
streamApp_DBD += drvAsynIPPort.dbd
#streamApp_DBD += drvGsIP488.dbd
#streamApp_DBD += drvNi1014.dbd
#streamApp_DBD += drvVxi11.dbd
PROD_LIBS += asyn
endif

ifneq ($(words $(CALC) $(SYNAPPS)), 0)
# With synApps scalcout record
streamApp_DBD += calc.dbd
streamApp_DBD += streamSynApps.dbd
PROD_LIBS += calc sscan
PROD_LIBS_DEFAULT += streamSynApps
endif

streamApp_DBD += stream.dbd

PROD_LIBS += $(EPICS_BASE_IOC_LIBS)

#Write StreamDevice debug output to this file
CPPFLAGS += -DDEBUGFILE=StreamDebug.log

include $(TOP)/configure/RULES

#clean::
#	rm -f core*

#build: link

#link:
#	rm -f ../streamApp
#	ln -s O.$(EPICS_HOST_ARCH)/streamApp ../streamApp
