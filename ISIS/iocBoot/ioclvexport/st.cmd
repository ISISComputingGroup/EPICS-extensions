#!../../bin/windows-x64/lvexport

## You may have to change lvexport to something else
## everywhere it appears in this file

< envPaths

cd ${TOP}

## Register all support components
dbLoadDatabase "dbd/lvexport.dbd"
lvexport_registerRecordDeviceDriver pdbbase

cd ${TOP}/iocBoot/${IOC}

# Turn on asynTraceFlow and asynTraceError for global trace, i.e. no connected asynUser.
#asynSetTraceMask("", 0, 17)

testAsynPortDriverConfigure("testAPD", 1000)

dbLoadRecords("../../db/lvexport.db","P=testAPD:,R=scope1:,PORT=testAPD,ADDR=0,TIMEOUT=1,NPOINTS=1000")
dbLoadRecords("$(ASYN)/db/asynRecord.db","P=testAPD:,R=asyn1,PORT=testAPD,ADDR=0,OMAX=80,IMAX=80")
#asynSetTraceMask("testAPD",0,0xff)
asynSetTraceIOMask("testAPD",0,0x2)

iocInit
