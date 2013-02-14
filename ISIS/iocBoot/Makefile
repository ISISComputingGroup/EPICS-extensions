TOP = ..
include $(TOP)/configure/CONFIG
ifneq ($(findstring windows,$(T_A)),)
DIRS += $(wildcard *ioc*)
endif
#DIRS += $(wildcard *ioc*)
DIRS += $(wildcard as*)
include $(CONFIG)/RULES_DIRS

