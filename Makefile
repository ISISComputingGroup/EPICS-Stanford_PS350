TOP = ./extensions
include $(TOP)/configure/CONFIG

DIRS = base base_examples extensions support ISIS

DIRS := $(wildcard $(DIRS))

base_examples_DEPENDS_DIRS = base
extensions_DEPENDS_DIRS = base
support_DEPENDS_DIRS = base
ISIS_DEPENDS_DIRS = support

all : ISIS_CONFIG.$(EPICS_HOST_ARCH)

#
# check if we need to re-fun config_env
#
ifeq ($(findstring windows,$(EPICS_HOST_ARCH)),) 
ISIS_CONFIG.$(EPICS_HOST_ARCH) : config_env.sh
	$(error You need to re-run   config_env.sh   first)
else
ISIS_CONFIG.$(EPICS_HOST_ARCH) : config_env.bat
	$(error You need to re-run   config_env.bat   first)
endif

ACTIONS += uninstall kit

include $(TOP)/configure/RULES_DIRS
