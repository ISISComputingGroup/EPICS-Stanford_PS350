#Makefile at top of application tree
TOP = .
include $(TOP)/configure/CONFIG
DIRS := $(DIRS) $(filter-out $(DIRS), configure)
## commented out second occurrences as on Windows they match first one and cause a multiple defined warning
DIRS := $(DIRS) $(filter-out $(DIRS), $(wildcard *Sup))
#DIRS := $(DIRS) $(filter-out $(DIRS), $(wildcard *sup))
DIRS := $(DIRS) $(filter-out $(DIRS), $(wildcard *App))
#DIRS := $(DIRS) $(filter-out $(DIRS), $(wildcard *app))
DIRS := $(DIRS) $(filter-out $(DIRS), $(wildcard iocBoot))
#DIRS := $(DIRS) $(filter-out $(DIRS), $(wildcard iocboot))
include $(TOP)/configure/RULES_TOP
