SUBDIRS=base extensions modules ISIS

all : $(SUBDIRS)

extensions : base
modules : base
ISIS : modules

#MAKEBASEAPP=.\base\bin\${EPICS_HOST_ARCH}\makeBaseApp.pl
#aaa : base
#	cd base_examples#
#	perl $(MAKEBASEAPP) -t example example
#	perl $(MAKEBASEAPP) -i -p example -t example example
#	perl $(MAKEBASEAPP) -t caClient caClient
#	cd ..

include Makefile.rules
