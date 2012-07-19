TOP=../..

include $(TOP)/configure/CONFIG
#----------------------------------------
#  ADD MACRO DEFINITIONS AFTER THIS LINE
#=============================

#=============================
# build an ioc application

PROD_IOC = galilTest

DBD += galilTest.dbd
# galilTest.dbd will be made up from these files:
galilTest_DBD += base.dbd
#include definitions for any other support applications needed
galilTest_DBD += devGalil.dbd motorSupport.dbd calcSupport.dbd sscanSupport.dbd

# <name>_registerRecordDeviceDriver.cpp will be created from <name>.dbd
galilTest_SRCS += galilTest_registerRecordDeviceDriver.cpp
galilTest_SRCS_DEFAULT += galilTestMain.cpp
galilTest_SRCS_vxWorks += -nil-

#The following adds support from base/src/vxWorks
galilTest_OBJS_vxWorks += $(EPICS_BASE_BIN)/vxComLibrary

#add a definition for each support application used by this application
galilTest_LIBS += galil motor calc sscan

#NOTES:
#      1)It is not possible to build sncExample both as a component of galilTest
#        and standalone. You must choose only one.
#      2)To build sncExample SNCSEQ must be defined in <top>/configure/RELEASE

#The following builds sncExample as a component of galilTest
#galilTest_SRCS += sncExample.stt
#galilTest_LIBS += seq pv

galilTest_LIBS += $(EPICS_BASE_IOC_LIBS)

#The following builds sncExample as a standalone application
#PROD_HOST += sncExample
#sncExample_SNCFLAGS += +m
#sncExample_SRCS += sncExample.stt
#sncExample_LIBS += seq pv
#sncExample_LIBS += $(EPICS_BASE_HOST_LIBS)

#===========================

include $(TOP)/configure/RULES
#----------------------------------------
#  ADD RULES AFTER THIS LINE

