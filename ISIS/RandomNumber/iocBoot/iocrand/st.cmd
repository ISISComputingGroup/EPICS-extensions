#!../../bin/win32-x86/rand

## You may have to change rand to something else
## everywhere it appears in this file

< envPaths

cd ${TOP}

## Register all support components
dbLoadDatabase "dbd/rand.dbd"
rand_registerRecordDeviceDriver pdbbase

## Load record instances
dbLoadRecords("db/rand.db","P=test:rand, D=Random, S=324235")

cd ${TOP}/iocBoot/${IOC}
iocInit

## Start any sequence programs
#seq sncxxx,"user=mjc23Host"
