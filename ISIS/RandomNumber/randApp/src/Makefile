TOP=../..

include $(TOP)/configure/CONFIG
#----------------------------------------
#  ADD MACRO DEFINITIONS AFTER THIS LINE
#=============================

#=============================
# Build the IOC application

PROD_IOC = rand
# rand.dbd will be created and installed
DBD += rand.dbd

# rand.dbd will be made up from these files:
rand_DBD += base.dbd

# Include dbd files from all support applications:
rand_DBD += randdev.dbd

# Add all the support libraries needed by this IOC
#rand_LIBS += xxx

# rand_registerRecordDeviceDriver.cpp derives from rand.dbd
rand_SRCS += rand_registerRecordDeviceDriver.cpp

rand_SRCS += devRand.c

# Build the main IOC entry point on workstation OSs.
rand_SRCS_DEFAULT += randMain.cpp
rand_SRCS_vxWorks += -nil-

# Add support from base/src/vxWorks if needed
#rand_OBJS_vxWorks += $(EPICS_BASE_BIN)/vxComLibrary

# Finally link to the EPICS Base libraries
rand_LIBS += $(EPICS_BASE_IOC_LIBS)

#===========================

include $(TOP)/configure/RULES
#----------------------------------------
#  ADD RULES AFTER THIS LINE

