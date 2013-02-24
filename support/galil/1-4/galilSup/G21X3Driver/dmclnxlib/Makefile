# Makefile
TOP = ../../..
include $(TOP)/configure/CONFIG

LIBRARY_IOC = dmclnx

#The following line makes epics build system compatible with // style comments

#dmcANSI= -std=gnu89

USR_CFLAGS = $(dmcANSI)

# Galil Driver
dmclnx_SRCS += dmcbin.c dmccvs.c dmcdma.c dmceth.c dmcint.c dmcisa.c dmclnx.c dmcpci.c dmcsup.c # dmcser.c

dmclnx_LIBS += $(EPICS_BASE_IOC_LIBS)
dmclnx_SYS_LIBS_WIN32 += ws2_32

include $(TOP)/configure/RULES

