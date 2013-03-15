include ../ISIS_CONFIG.$(EPICS_HOST_ARCH)

COMMAND=./generate_examples.sh
ifneq ($(findstring windows,$(EPICS_HOST_ARCH)),)
COMMAND=generate_examples.bat
endif
ifneq ($(findstring win32,$(EPICS_HOST_ARCH)),)
COMMAND=generate_examples.bat
endif

install :
	$(COMMAND) $(EPICS_BASE)
	
clean :

uninstall :

kit :

zip :