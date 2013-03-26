TOP=../..

include $(TOP)/configure/CONFIG
#----------------------------------------
#  ADD MACRO DEFINITIONS AFTER THIS LINE
#=============================

#=============================
# Build the IOC application

PROD_IOC = agilent3631A
# agilent3631A.dbd will be created and installed
DBD += agilent3631A.dbd

# agilent3631A.dbd will be made up from these files:
agilent3631A_DBD += base.dbd

# Include dbd files from all support applications:
#agilent3631A_DBD += xxx.dbd
agilent3631A_DBD += stream.dbd
agilent3631A_DBD += asyn.dbd
agilent3631A_DBD += drvAsynSerialPort.dbd
agilent3631A_DBD += drv488Gpib.dbd

# Add all the support libraries needed by this IOC
#agilent3631A_LIBS += xxx
agilent3631A_LIBS += stream
agilent3631A_LIBS += asyn

# agilent3631A_registerRecordDeviceDriver.cpp derives from agilent3631A.dbd
agilent3631A_SRCS += agilent3631A_registerRecordDeviceDriver.cpp

# Build the main IOC entry point on workstation OSs.
agilent3631A_SRCS_DEFAULT += agilent3631AMain.cpp
agilent3631A_SRCS_vxWorks += -nil-

# Add support from base/src/vxWorks if needed
#agilent3631A_OBJS_vxWorks += $(EPICS_BASE_BIN)/vxComLibrary

# Finally link to the EPICS Base libraries
agilent3631A_LIBS += $(EPICS_BASE_IOC_LIBS)

#===========================

include $(TOP)/configure/RULES
#----------------------------------------
#  ADD RULES AFTER THIS LINE

