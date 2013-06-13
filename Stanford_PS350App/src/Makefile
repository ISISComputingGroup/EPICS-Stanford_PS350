TOP=../..

include $(TOP)/configure/CONFIG
#----------------------------------------
#  ADD MACRO DEFINITIONS AFTER THIS LINE
#=============================

#=============================
# Build the IOC application

PROD_IOC = Stanford_PS350
# Stanford_PS350.dbd will be created and installed
DBD += Stanford_PS350.dbd

# Stanford_PS350.dbd will be made up from these files:
Stanford_PS350_DBD += base.dbd
Stanford_PS350_DBD += asyn.dbd

# Include dbd files from all support applications:
#Stanford_PS350_DBD += xxx.dbd

# Add all the support libraries needed by this IOC
#Stanford_PS350_LIBS += xxx

# Stanford_PS350_registerRecordDeviceDriver.cpp derives from Stanford_PS350.dbd
Stanford_PS350_SRCS += Stanford_PS350_registerRecordDeviceDriver.cpp

# Build the main IOC entry point on workstation OSs.
Stanford_PS350_SRCS_DEFAULT += Stanford_PS350Main.cpp
Stanford_PS350_SRCS_vxWorks += -nil-

# Add support from base/src/vxWorks if needed
#Stanford_PS350_OBJS_vxWorks += $(EPICS_BASE_BIN)/vxComLibrary

Stanford_PS350_LIBS +=  lvDCOMSupport asyn
Stanford_PS350_SYS_LIBS_WIN32 += msxml2

# Finally link to the EPICS Base libraries
Stanford_PS350_LIBS += $(EPICS_BASE_IOC_LIBS)

#===========================

include $(TOP)/configure/RULES
#----------------------------------------
#  ADD RULES AFTER THIS LINE

