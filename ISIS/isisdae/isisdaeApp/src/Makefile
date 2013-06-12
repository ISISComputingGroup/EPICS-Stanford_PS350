TOP=../..

include $(TOP)/configure/CONFIG
#----------------------------------------
#  ADD MACRO DEFINITIONS AFTER THIS LINE
#=============================

# Build the IOC support library

LIBRARY_IOC += isisdaeSupport

isisdaeSupport_SRCS += isisdaeDriver.cpp isisdaeInterface.cpp variant_utils.cpp convertToString.cpp
isisdaeSupport_LIBS += asyn
isisdaeSupport_LIBS += $(EPICS_BASE_IOC_LIBS)
#isisdaeSupport_SYS_LIBS_WIN32 += msxml2


#=============================
# Build the IOC application

PROD_IOC = isisdae
# isisdae.dbd will be created and installed
DBD += isisdae.dbd

# Include dbd files from all support applications:
#isisdae_DBD += xxx.dbd

# Add all the support libraries needed by this IOC
#isisdae_LIBS += xxx

# isisdae_registerRecordDeviceDriver.cpp derives from isisdae.dbd
isisdae_SRCS += isisdae_registerRecordDeviceDriver.cpp

# Build the main IOC entry point on workstation OSs.
isisdae_SRCS_DEFAULT += isisdaeMain.cpp
isisdae_SRCS_vxWorks += -nil-

# Add support from base/src/vxWorks if needed
#isisdae_OBJS_vxWorks += $(EPICS_BASE_BIN)/vxComLibrary

isisdae_LIBS += isisdaeSupport asyn
#isisdae_SYS_LIBS_WIN32 += msxml2

# Finally link to the EPICS Base libraries
isisdae_LIBS += $(EPICS_BASE_IOC_LIBS)

#===========================

include $(TOP)/configure/RULES
#----------------------------------------
#  ADD RULES AFTER THIS LINE

