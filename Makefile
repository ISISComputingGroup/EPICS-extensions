# Makefile,v 1.17 2007/06/22 21:08:32 jba Exp

TOP = .
include $(TOP)/configure/CONFIG
DIRS = $(wildcard configure src)

src_DEPEND_DIRS += configure

include $(TOP)/configure/RULES_TOP

