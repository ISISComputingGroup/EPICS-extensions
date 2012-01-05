TOP=../..

include $(TOP)/configure/CONFIG
#----------------------------------------
#  ADD MACRO DEFINITIONS AFTER THIS LINE
#=============================

#==================================================
# build a support library

LIBRARY_IOC += pcre

# xxxRecord.h will be created from xxxRecord.dbd
#DBDINC += xxxRecord
# install pcre.dbd into <top>/dbd
DBD += pcre.dbd

# specify all source files to be compiled and added to the library
#pcre_SRCS += xxx

pcre_LIBS += $(EPICS_BASE_IOC_LIBS)

#===========================

include $(TOP)/configure/RULES
#----------------------------------------
#  ADD RULES AFTER THIS LINE

