#!./streamApp

epicsEnvSet "STREAM_PROTOCOL_PATH", ".:protocols:../protocols/"
dbLoadDatabase "O.Common/streamApp.dbd"
streamApp_registerRecordDeviceDriver

#setup the busses
drvAsynSerialPortConfigure "MYCOM", "COM5"
drvAsynSerialPortConfigure "stdout", "/dev/stdout"
#drvAsynIPPortConfigure "terminal", "localhost:40000"

#load the records
dbLoadRecords "example.db"
#dbLoadRecords "scalcout.db"

var streamDebug 1
iocInit
