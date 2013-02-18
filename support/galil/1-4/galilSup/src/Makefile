# Makefile
TOP = ../..
include $(TOP)/configure/CONFIG

# Both the following line, and a line in the *.dbd file,
# must be uncommented to use diagnostic debugging messages.
#!USR_CXXFLAGS += -DDEBUG

DBD += devGalil.dbd

LIBRARY_IOC = galil

SRC_DIRS += $(TOP)/galilSup/G21X3Driver/dmclnxlib
SRC_DIRS += $(MOTOR)

# Galil Driver
galil_SRCS += GalilRegister.cc devG21X3.cc devAiG21X3.cc devAoG21X3.cc devMbbiG21X3.cc devMbboG21X3.cc drvG21X3.cc


galil_LIBS += dmclnx motor
galil_LIBS += $(EPICS_BASE_IOC_LIBS)

include $(TOP)/configure/RULES

