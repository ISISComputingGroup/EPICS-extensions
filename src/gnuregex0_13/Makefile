#*************************************************************************
# Copyright (c) 2002 The University of Chicago, as Operator of Argonne
# National Laboratory.
# This file is distributed subject to a Software License Agreement found
# in the file LICENSE that is included with this distribution. 
#*************************************************************************

# 3.14 Makefile for GNU Regex

TOP = ../..
include $(TOP)/configure/CONFIG

CMPLR = STRICT

# Optimization
ifdef WIN32
HOST_OPT = YES
else
HOST_OPT = YES
endif

USR_CFLAGS_DEFAULT += -DSTDC_HEADERS=1 -DHAVE_STRING_H=1 -DHAVE_ALLOCA_H=1
USR_CFLAGS_WIN32 += -DSTDC_HEADERS=1 -DHAVE_STRING_H=1 -DHAVE_ALLOCA_H=0 -DREGEX_MALLOC

ifdef WIN32
# The proper import/export declarations are not included for DLLs
SHARED_LIBRARIES = NO
endif

INC=regex.h

LIBSRCS += regex.c

LIBRARY_HOST = regex

include $(TOP)/configure/RULES

xxxx:
	@echo USR_CFLAGS_WIN32: $(USR_CFLAGS_WIN32)
	@echo WIN32_DLLFLAGS: $(WIN32_DLLFLAGS)
	@echo LINK.shrlib: $(LINK.shrlib)
	@echo LINK.c: $(LINK.c)
	@echo USR_LIBS: $(USR_LIBS)


# **************************** Emacs Editing Sequences *****************
# Local Variables:
# mode: makefile
# End:
