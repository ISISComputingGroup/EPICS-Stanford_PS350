#!../../bin/windows-x64/isisdae

## You may have to change isisdae to something else
## everywhere it appears in this file

< envPaths

cd ${TOP}

## Register all support components
dbLoadDatabase "dbd/isisdae.dbd"
isisdae_registerRecordDeviceDriver pdbbase

isisdaeConfigure("icp", "localhost")
#isisdaeConfigure("icp", "ndxchipir", 0, "spudulike", "reliablebeam")

## Load record instances
dbLoadRecords("$(TOP)/db/isisdae.db","P=ex1:")

cd ${TOP}/iocBoot/${IOC}
iocInit

## Start any sequence programs
#seq sncxxx,"user=faa59Host"
