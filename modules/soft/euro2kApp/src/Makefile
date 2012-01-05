TOP=../..

include $(TOP)/configure/CONFIG
#----------------------------------------
#  ADD MACRO DEFINITIONS AFTER THIS LINE
#=============================

#=============================
# Build the IOC application

PROD_IOC = euro2k
# euro2k.dbd will be created and installed
DBD += euro2k.dbd

# euro2k.dbd will be made up from these files:
euro2k_DBD += base.dbd
euro2k_DBD += asyn.dbd
euro2k_DBD += drvAsynIPPort.dbd
euro2k_DBD += stream.dbd

# Include dbd files from all support applications:
#euro2k_DBD += xxx.dbd

# Add all the support libraries needed by this IOC
euro2k_LIBS += stream
euro2k_LIBS += asyn

# euro2k_registerRecordDeviceDriver.cpp derives from euro2k.dbd
euro2k_SRCS += euro2k_registerRecordDeviceDriver.cpp

# Build the main IOC entry point on workstation OSs.
euro2k_SRCS_DEFAULT += euro2kMain.cpp
euro2k_SRCS_vxWorks += -nil-

# Add support from base/src/vxWorks if needed
#euro2k_OBJS_vxWorks += $(EPICS_BASE_BIN)/vxComLibrary

# Finally link to the EPICS Base libraries
euro2k_LIBS += $(EPICS_BASE_IOC_LIBS)

#===========================

include $(TOP)/configure/RULES
#----------------------------------------
#  ADD RULES AFTER THIS LINE

