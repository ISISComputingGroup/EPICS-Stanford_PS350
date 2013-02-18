SUBDIRS=seq ipac sscan calc asyn busy ip pcre StreamDevice # eurotherm2k
ifeq ($(findstring windows,$(EPICS_HOST_ARCH)),) 
SUBDIRS += motor galil 
endif  

all : $(SUBDIRS)

include ../Makefile.rules
