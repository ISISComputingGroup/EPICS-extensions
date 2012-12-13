TOP=../..

include $(TOP)/configure/CONFIG
#----------------------------------------
#  ADD MACRO DEFINITIONS AFTER THIS LINE




#=============================
# Build the IOC support library

LIBRARY_IOC += lvexportSupport

# xxxRecord.h will be created from xxxRecord.dbd
#DBDINC += xxxRecord
DBD += lvexport.dbd

# Compile and add the code to the support library
lvexportSupport_SRCS += testAsynPortDriver.cpp


lvexportSupport_LIBS += asyn
lvexportSupport_LIBS += $(EPICS_BASE_IOC_LIBS)

#=============================
# Build the IOC application

PROD_IOC = lvexport
# lvexport.dbd will be created and installed
#DBD += lvexport.dbd

# lvexport.dbd will be made up from these files:
#lvexport_DBD += base.dbd
#lvexport_DBD += xxxSupport.dbd
#lvexport_DBD += dbSubExample.dbd
#lvexport_DBD += lvexportHello.dbd
#lvexport_DBD += initTrace.dbd

# lvexport_registerRecordDeviceDriver.cpp derives from lvexport.dbd
lvexport_SRCS_DEFAULT += lvexport_registerRecordDeviceDriver.cpp testAsynPortDriverMain.cpp

# Build the main IOC entry point on workstation OSs.
#lvexport_SRCS_DEFAULT += lvexportMain.cpp

# Add support from base/src/vxWorks if needed
#lvexport_OBJS_vxWorks += $(EPICS_BASE_BIN)/vxComLibrary

# Link in the code from the support library
lvexport_LIBS += lvexportSupport asyn

# Finally link to the EPICS Base libraries
lvexport_LIBS += $(EPICS_BASE_IOC_LIBS)



#=============================

include $(TOP)/configure/RULES
#----------------------------------------
#  ADD RULES AFTER THIS LINE

