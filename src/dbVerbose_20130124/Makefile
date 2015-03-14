#*************************************************************************
# Copyright (c) 2002 The University of Chicago, as Operator of Argonne
# National Laboratory.
# Copyright (c) 2002 The Regents of the University of California, as
# Operator of Los Alamos National Laboratory.
# This file is distributed subject to a Software License Agreement found
# in the file LICENSE that is included with this distribution. 
#*************************************************************************
#
# Makefile,v 1.4 2013/01/15 17:21:07 jba Exp
#
TOP = ../..
include $(TOP)/configure/CONFIG

ifdef BASE_3_15
PROD_LIBS += dbCore Com
else
PROD_LIBS += dbStaticHost Com
endif

SRCS.c = dbVerbose.c
PROD_HOST = dbVerbose

include $(TOP)/configure/RULES

