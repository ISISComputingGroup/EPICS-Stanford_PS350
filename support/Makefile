SUBDIRS=seq ipac sscan calc asyn busy ip motor pcre StreamDevice # eurotherm2k
ifeq ($(findstring windows,$(EPICS_HOST_ARCH)),) 
SUBDIRS += galil 
endif  

all : $(SUBDIRS)

include ../Makefile.rules
