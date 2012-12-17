TOP=../..

include $(TOP)/configure/CONFIG
#----------------------------------------
#  ADD MACRO DEFINITIONS AFTER THIS LINE

FILE_TYPE += pdb
INSTALL_pdb = $(INSTALL_BIN)

USR_INCLUDES += -IC:/development/InstrumentControl/ICP/Third_Party/POCO/include
USR_INCLUDES += -IC:/development/InstrumentControl/ICP/tlb

USR_CXXFLAGS += /Zi
#USR_CPPFLAGS
USR_LDFLAGS += /DEBUG

pocofoundation_DIR = C:/development/InstrumentControl/ICP/Third_Party/POCO/lib_x64
pocoxml_DIR = C:/development/InstrumentControl/ICP/Third_Party/POCO/lib_x64
pocoutil_DIR = C:/development/InstrumentControl/ICP/Third_Party/POCO/lib_x64

#=============================
# Build the IOC support library

LIBRARY_IOC += lvexportSupport

pdb += lvexportSupport.pdb

# xxxRecord.h will be created from xxxRecord.dbd
#DBDINC += xxxRecord
DBD += lvexport.dbd

# Compile and add the code to the support library
lvexportSupport_SRCS += testAsynPortDriver.cpp isis_stuff.cpp variant_utils.cpp

lvexportSupport_LIBS += asyn
lvexportSupport_LIBS += pocoutil pocoxml pocofoundation
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

pdb += lvexport.pdb


#=============================

include $(TOP)/configure/RULES
#----------------------------------------
#  ADD RULES AFTER THIS LINE

