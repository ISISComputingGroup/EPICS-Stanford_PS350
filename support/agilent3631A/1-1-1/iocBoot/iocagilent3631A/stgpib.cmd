#!../../bin/windows-x64/agilent3631Atest

## You may have to change agilent3631Atest to something else
## everywhere it appears in this file

< envPaths
epicsEnvSet "STREAM_PROTOCOL_PATH" "$(TOP)/db"

epicsEnvSet "P" "$(PREFIX)"
epicsEnvSet "R" "$(R=A3631:PSU:)"

cd ${TOP}

## Register all support components
dbLoadDatabase "dbd/agilent3631Atest.dbd"
agilent3631Atest_registerRecordDeviceDriver pdbbase

#Set up the GPIB access
GpibBoardDriverConfig("GPIB0","1","0","3","0")


asynOctetSetInputEos("GPIB0", 1, "\n")
asynOctetSetOutputEos("GPIB0", 1, "\n")



## Load record instances
dbLoadRecords("db/devagilent3631A.db","P=$(P),R=$(R),PORT=GPIB0,A=1")

cd ${TOP}/iocBoot/${IOC}
iocInit

## Start any sequence programs
#seq sncxxx,"user=kvlb23Host"
