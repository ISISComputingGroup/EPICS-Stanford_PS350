TOP = ../extensions
include $(TOP)/configure/CONFIG

ACTIONS += uninstall kit

DIRS=seq ipac sscan calc asyn busy ip motor galil pcre StreamDevice eurotherm2k
ifeq ($(findstring windows,$(EPICS_HOST_ARCH)),) 
#DIRS += galil 
endif  
asyn_DEPENDS_DIRS = ipac seq
busy_DEPENDS_DIRS = asyn
calc_DEPENDS_DIRS = sscan
eurotherm2k_DEPENDS_DIRS = StreamDevice
galil_DEPENDS_DIRS = calc sscan motor seq
ip_DEPENDS_DIRS = ipac seq asyn
motor_DEPENDS_DIRS = asyn seq busy ipac
sscan_DEPENDS_DIRS = seq
StreamDevice_DEPENDS_DIRS = seq asyn pcre calc

DIRS := $(wildcard $(DIRS))

include $(TOP)/configure/RULES_DIRS
