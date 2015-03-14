#*************************************************************************
# Copyright (c) 2002 The University of Chicago, as Operator of Argonne
# National Laboratory.
# Copyright (c) 2002 The Regents of the University of California, as
# Operator of Los Alamos National Laboratory.
# This file is distributed subject to a Software License Agreement found
# in the file LICENSE that is included with this distribution. 
#*************************************************************************
# Makefile for CaSnooper

TOP = ../..
include $(TOP)/configure/CONFIG

# Define to make caSnooperB with a modified version of base and not
# build caSnooper
CA_SNOOPERB=NO
#CA_SNOOPERB=YES

# Use special, modified base (Will also be used for caSnooper)
ifeq ($(CA_SNOOPERB),YES)
  EPICS_BASE = /home/phoebus/EVANS/epics/baseSnoop
endif

# Use gcc
#CPLUSPLUS=G++

CXXCMPLR = STRICT

#HOST_OPT=NO

# The following is necessary to include server.h to get to casCtx
USR_CXXFLAGS += -DcaNetAddrSock
USR_INCLUDES += -I$(EPICS_BASE)/src/cas/generic
USR_INCLUDES += -I$(EPICS_BASE)/src/cas/generic/st
USR_INCLUDES += -I$(EPICS_BASE)/src/cas/io/bsdSocket

# USR_CXXFLAGS += -DDEBUG_MODE

# Use CAS diagnostics statistics
USR_CXXFLAGS += -DCAS_DIAGNOSTICS

caSnooper_LIBS := cas ca gdd Com
SYS_PROD_LIBS_WIN32 := ws2_32 advapi32 user32

SRCS += ut.cc

caSnooper_SRCS += caSnooper.cc
caSnooper_SRCS += snoopServer.cc
caSnooper_SRCS += snoopStat.cc

PROD_HOST := caSnooper

# CaSnooperB
ifeq ($(CA_SNOOPERB),YES)
  caSnooperB_LIBS := ca Com

  caSnooperB_SRCS += caSnooperB.cc
  caSnooperB_SRCS += snoopClient.cc

  PROD_HOST := caSnooperB
endif

include $(TOP)/configure/RULES

caSnooper$(OBJ) snoopServer$(OBJ) : ../snoopServer.h
caSnooperB$(OBJ) snoopClient$(OBJ) : ../snoopClient.h
snoopStat$(OBJ) : ../snoopStat.h

xxxx:
	@echo EPICS_BASE: $(EPICS_BASE)
	@echo CA_SNOOPERB: $(CA_SNOOPERB)
	@echo caSnooperB_LIBS: $(caSnooperB_LIBS)
	@echo PROD: $(PROD)
	@echo CCC: $(CCC)
	@echo CXXFLAGS: $(CXXFLAGS)

# **************************** Emacs Editing Sequences *****************
# Local Variables:
# mode: makefile
# End:
