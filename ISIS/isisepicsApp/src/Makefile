TOP=../..

include $(TOP)/configure/CONFIG
#----------------------------------------
#  ADD MACRO DEFINITIONS AFTER THIS LINE

#=============================
# Build the IOC support library

LIBRARY_IOC += isisepicsSupport

# xxxRecord.h will be created from xxxRecord.dbd
DBDINC += xxxRecord
# Install devXxxSoft.dbd into <top>/dbd
DBD += xxxSupport.dbd

# Compile and add the code to the support library
isisepicsSupport_SRCS += xxxRecord.c
isisepicsSupport_SRCS += devXxxSoft.c

# Link locally-provided code into the support library,
# rather than directly into the IOC application.
isisepicsSupport_SRCS += dbSubExample.cpp
isisepicsSupport_SRCS += isisepicsHello.c
isisepicsSupport_SRCS += initTrace.c

isisepicsSupport_LIBS += $(EPICS_BASE_IOC_LIBS)

#=============================
# Build the IOC application

PROD_IOC = isisepics
# isisepics.dbd will be created and installed
DBD += isisepics.dbd

# isisepics.dbd will be made up from these files:
isisepics_DBD += base.dbd
isisepics_DBD += xxxSupport.dbd
isisepics_DBD += dbSubExample.dbd
isisepics_DBD += isisepicsHello.dbd
isisepics_DBD += initTrace.dbd

# isisepics_registerRecordDeviceDriver.cpp derives from isisepics.dbd
isisepics_SRCS += isisepics_registerRecordDeviceDriver.cpp

# Build the main IOC entry point on workstation OSs.
isisepics_SRCS_DEFAULT += isisepicsMain.cpp
isisepics_SRCS_vxWorks += -nil-

# Add support from base/src/vxWorks if needed
#isisepics_OBJS_vxWorks += $(EPICS_BASE_BIN)/vxComLibrary

# Link in the code from the support library
isisepics_LIBS += isisepicsSupport

# NOTE: To build SNL programs, SNCSEQ must be defined
# in the <top>/configure/RELEASE file

ifneq ($(SNCSEQ),)
    # Build sncExample into isisepicsSupport
    sncExample_SNCFLAGS += +r
    isisepics_DBD += sncExample.dbd
    isisepicsSupport_SRCS += sncExample.stt
    isisepicsSupport_LIBS += seq pv
    isisepics_LIBS += seq pv

    # Build sncProgram as a standalone program
    PROD_HOST += sncProgram
    sncProgram_SNCFLAGS += +m
    sncProgram_SRCS += sncProgram.st
    sncProgram_LIBS += seq pv
    sncProgram_LIBS += $(EPICS_BASE_HOST_LIBS)
endif

# Finally link to the EPICS Base libraries
isisepics_LIBS += $(EPICS_BASE_IOC_LIBS)

#=============================

include $(TOP)/configure/RULES
#----------------------------------------
#  ADD RULES AFTER THIS LINE

