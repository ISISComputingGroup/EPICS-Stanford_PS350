#!../../bin/windows-x64/Stanford_PS350

## You may have to change Stanford_PS350 to something else
## everywhere it appears in this file

< envPaths

cd ${TOP}

## Register all support components
dbLoadDatabase "dbd/Stanford_PS350.dbd"
Stanford_PS350_registerRecordDeviceDriver pdbbase

cd ${TOP}/iocBoot/${IOC}

# Turn on asynTraceFlow and asynTraceError for global trace, i.e. no connected asynUser.
#asynSetTraceMask("", 0, 17)

## main args are:  portName, configSection, configFile, host, options (see lvDCOMConfigure() documentation in lvDCOMDriver.cpp)
##
## there are additional optional args to specify a DCOM ProgID for a compiled LabVIEW application 
## and a different username + password for remote host if that is required 
##
## the "options" argument is a combination of the following flags (as per the #lvDCOMOptions enum in lvDCOMInterface.h)
##    viWarnIfIdle=1, viStartIfIdle=2, viStopOnExitIfStarted=4, viAlwaysStopOnExit=8
lvDCOMConfigure("frontpanel", "frontpanel", "$(TOP)/Stanford_PS350App/protocol/stanfordPS350.xml", "ndxchipir", 6, "", "spudulike", "reliablebeam")
#lvDCOMConfigure("frontpanel", "frontpanel", "$(TOP)/Stanford_PS350App/protocol/stanfordPS350.xml", "", 6)

dbLoadRecords("$(TOP)/db/Stanford_PS350.db","P=INST:SE:STPS350:")
#asynSetTraceMask("frontpanel",0,0xff)
asynSetTraceIOMask("frontpanel",0,0x2)

iocInit

