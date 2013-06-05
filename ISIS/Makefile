#Makefile at top of application tree
TOP = ../extensions
include $(TOP)/configure/CONFIG
#DIRS = RandomNumber SimpleIoc

# extra ones #
MODULES = RandomNumber SimpleIoc
ifeq ($(findstring linux,$(EPICS_HOST_ARCH)),)
ifeq ($(HAVE_ATL),YES)
MODULES += lvDCOM
#DIRS += lvDCOM
endif
endif
DIRS := $(MODULES) 

include $(TOP)/configure/RULES_TOP


