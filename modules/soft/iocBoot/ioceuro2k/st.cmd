#!../../bin/windows-x64/euro2k

## You may have to change euro2k to something else
## everywhere it appears in this file

< envPaths

cd ${TOP}

## Register all support components
dbLoadDatabase "dbd/euro2k.dbd"
euro2k_registerRecordDeviceDriver pdbbase

## Load record instances
#dbLoadRecords("db/xxx.db","user=faa59Host")

dbLoadTemplate("db/eurotherm2k.subs")

#drvAsynIPPortConfigure("eurothermAsyn", "172.23.111.180:7001", 100, 0, 0)

# Configure StreamDevice paths
epicsEnvSet "STREAM_PROTOCOL_PATH", "$(EUROTHERM2K)/euro2kdata"

cd ${TOP}/iocBoot/${IOC}
iocInit

## Start any sequence programs
#seq sncxxx,"user=faa59Host"
