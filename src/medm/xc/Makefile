#*************************************************************************
# Copyright (c) 2002 The University of Chicago, as Operator of Argonne
# National Laboratory.
# Copyright (c) 2002 The Regents of the University of California, as
# Operator of Los Alamos National Laboratory.
# This file is distributed subject to a Software License Agreement found
# in the file LICENSE that is included with this distribution.
#*************************************************************************
#
# Makefile,v 1.11 2007/07/03 21:14:22 jba Exp
#
TOP = ../../..
include $(TOP)/configure/CONFIG

SHARED_LIBRARIES = NO

CMPLR = STRICT

# Motif
USE_MOTIF12=NO
#USE_MOTIF12=YES

# Optimization
ifdef WIN32
#HOST_OPT = NO
else
#HOST_OPT = NO
endif

# Compiler specific
#GCC_WARN_YES = -Wall -Wtraditional -Wmissing-prototypes -Wstrict-prototypes
GCC_WARN_YES = -Wall

# Source browser options
ifeq ($(HOST_OPT),NO)
  ifeq ($(ANSI),ACC)
    ifeq ($(OS_CLASS),solaris)
# Set this for browsing all files
#      USR_CFLAGS += -xsb
# Set this to just do one file so it doesn't mix up Initialize, etc.
      USR_CFLAGS_WheelSwitch += -xsb
      USR_CFLAGS += $(USR_CFLAGS_$*)
    endif
  endif
endif

USR_INCLUDES = -I$(MOTIF_INC) -I$(X11_INC)

ifdef WIN32
USR_CFLAGS_WIN32 += /DWIN32 /D_WINDOWS
USR_CFLAGS_WIN32 += $(EXCEED_CFLAGS)

# The proper import/export declarations are not included for DLLs
SHARED_LIBRARIES = NO
endif

ifeq ($(USE_MOTIF12),YES)
USR_CFLAGS_solaris += -DMOTIF12_HEADERS
endif

USR_LIBS_DEFAULT = Xm Xt Xmu X11 Xext

LIBSRCS += BarGraph.c
LIBSRCS += Byte.c
LIBSRCS += Caption.c
LIBSRCS += Clip.c
LIBSRCS += Control.c
LIBSRCS += Indicator.c
LIBSRCS += Matrix.c
LIBSRCS += Marquee.c
LIBSRCS += Meter.c
LIBSRCS += Value.c
LIBSRCS += bae.c
LIBSRCS += utils.c
LIBSRCS += WheelSwitch.c

LIBRARY_HOST = ANLwidgets

include $(TOP)/configure/RULES

meter1: $(OBJS) $(DEPLIBS) meter1.c
	$(LINK.c) -o $@ $@.c $(LDLIBS)

indicator1 : $(OBJS) $(DEPLIBS) indicator1.c
	$(LINK.c) -o $@ $@.c $(LDLIBS)

bar1: $(OBJS) $(DEPLIBS) bar1.c
	$(LINK.c) -o $@ $@.c $(LDLIBS)

