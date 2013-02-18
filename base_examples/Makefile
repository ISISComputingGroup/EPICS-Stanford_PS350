include ../ISIS_CONFIG.$(EPICS_HOST_ARCH)

COMMAND=./generate_examples.sh
ifneq ($(findstring windows,$(EPICS_HOST_ARCH)),)
COMMAND=generate_examples.bat
endif

all :
	$(COMMAND) $(EPICS_BASE)
	
clean :

uninstall :
