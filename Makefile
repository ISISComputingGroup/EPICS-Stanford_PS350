SUBDIRS=base base_examples extensions support ISIS

all : ISIS_CONFIG.$(EPICS_HOST_ARCH) $(SUBDIRS) base_examples

extensions : base
support : base
ISIS : support

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

#MAKEBASEAPP=.\base\bin\${EPICS_HOST_ARCH}\makeBaseApp.pl
#aaa : base
#	cd base_examples#
#	perl $(MAKEBASEAPP) -t example example
#	perl $(MAKEBASEAPP) -i -p example -t example example
#	perl $(MAKEBASEAPP) -t caClient caClient
#	cd ..

include Makefile.rules
