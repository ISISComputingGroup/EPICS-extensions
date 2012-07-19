#!../../bin/linux-x86/galilTest

## You may have to change galilTest to something else
## everywhere it appears in this file

< envPaths

cd ${TOP}

## Register all support components
dbLoadDatabase("dbd/galilTest.dbd",0,0)
galilTest_registerRecordDeviceDriver(pdbbase)

## Load record instances

#Choose 1 out of the 2 dbLoadTemplate statements below for your application

#dbLoadTemplate("db/galil_motor_withoutwrappers.substitutions")
dbLoadTemplate("db/galil_motor_withwrappers.substitutions")

#Load extra functionality, untop of motorRecord features for axis/motors (eg. runtime gear ratio changes between master & slaves)
dbLoadTemplate("db/galil_motor_extras.substitutions")

#Load digital IO databases
dbLoadTemplate("db/galil_digital_ports.substitutions")

#Load analog IO databases
#dbLoadTemplate("db/galil_analog_ports.substitutions")

#Load user defined functions
dbLoadTemplate("db/galil_userdef_records.substitutions")

#Number of cards in system, and scan rate
#
G21X3Setup(1,60)

#Following are parameter descriptions for G21X3Config command.

#model numbers for galil controller to be used in G21X3Config, use number in G21X3Config not name.
#
#define MODEL_UNKNOWN          0
#define MODEL_100              100
#define MODEL_200              200
#define MODEL_600              600
#define MODEL_700              700
#define MODEL_1000             1000
#define MODEL_1200             1200
#define MODEL_1410             1410
#define MODEL_1411             1411
#define MODEL_1412             1412
#define MODEL_1415             1415
#define MODEL_1417             1417
#define MODEL_1500             1500
#define MODEL_1600             1600
#define MODEL_1700             1700
#define MODEL_1800             1800
#define MODEL_1802             1802
#define MODEL_2000             2000
#define MODEL_2100             2100

# Cardnumber starting from 0, ip, totalaxis used, model_num, limit switch type (-1 normally closed, 1 NOpen), home switch type (-1 normally closed, 1 NOpen)
#Controller home direction (-1 reverse, 1 Forward)
#
G21X3Config(0,"10.22.206.207",8,2100,-1,-1,-1)

# In the G21X3NameConfig command, the parameters are:
#
#    cardnumber starting from 0, 
#    axis A-H,
#    motortype, has the following values:
#
#           0 is a servo motor
#           1 is a reversed polarity servo motor
#           2 is a high active pulse stepper
#           3 is a low active pulse stepper
#           4 is a reverse high active pulse stepper
#           5 is a reverse low active pulse stepper
#
#    Axis home direction (-1 Reverse or 1 Forward),
#    Invert axis coordinates.  Makes -ve coordinate space +ve and vice versa.  Bit 0 = invert motor, Bit 1 = Invert Encoder, Bit 2 = Invert Position Maintenance, Bit 3 = Swap limits
#    home position value (User coordinate system in EGU)
#    Program position registers once home (0=No, 1=Yes)
#    mainencoder (-1 off, other see manual)),
#    auxencoder (-1 off, other see manual), 
#    ssiencoderinput (0 off, 1 main enc reg, 2 aux enc reg), 
#    ssitotalbits, 
#    ssidatabits,
#    ssierrbits, 
#    ssitimecode, 
#    ssidataform, 
#    limits_as_home (0 off 1 on), 
#    egu_after_limit (EGU travelled after limit/home trip),
#    slave axis, is of format "B=1.0000,1,uencB=usrVarB,0.00025,0,uencValidVar,C=-.0250,0,uencC=usrVarC,0.00025,0,uencValidVar"
#			      "B=1.0000,1, , , , ,C=-.0250,0" - No user encoder for slaves
#			      "" no slave
#
#           That is: axis=gearratio,homeallowed,uenc[Axis]=[expression],ueres,ueoff, uencValidVar, repeat till end of list
#									 (usr encoder resolution)
#									 	(user encoder offset)
#										      (user encoder reading valid flag 1=valid)
#		
#	    Where uenc[Axis] is the user encoder input location for [Axis] as specified by
#	    the [expression].  The uenc[Axis] string actually ends up as Galil code, on the controller.
#	    If uenc[Axis] is not specified and stepper position maintenance is on, the code generator will
#	    produce code based on your encoder type (eg. quadrature, SSI, Inverted SSI, etc).  This is an
#	    advanced setting, and it is not normally specified.  Examples are 3 IVU devices with 2 motors each
#	    with two SSI encoders, and 1 APS type wiggler with two motors, 2 SSI encoders and two digital interface
#	    "user" encoders known as gurley virtual absolute encoders.  User encoder relies on user code specified
#           in G21X3AddThreadCode to provide user encoder validation sequence (optional), to set the user encoder
#           valid flag (uencValidVar, mandatory), and to provide the encoder readback in 
#           the specified [expression] (mandatory).  The encoder units in [expression] must be raw counts
#           In user code uencv[Axis] = 1 is used to define the user encoder 
#           validation sequence.  PV's are:
#           $(KitName):$(MotorName)_UEVALIDATE_CMD. Will set uencv[Axis] = 1.  Triggers user provided validation process
#           $(KitName):$(MotorName)_UESTATE_CMD.    Requests user encoder state on/off (on may be denied, if not valid)
#           $(KitName):$(MotorName)_UESTATE_STATUS. Readback for user encoder state (on/off)
#           $(KitName):$(MotorName)_UEVALID_STATUS. User encoder reading valid status
#           $(KitName):$(MotorName)_UERAW_MONITOR.  User encoder raw counts
#           $(KitName):$(MotorName)_UEDIAL_MONITOR. User encoder dial reading (raw * ueres)
#	    $(KitName):$(MotorName)_UEUSER_MONITOR. User encoder user reading (raw * ueres) + ueoff
#
#           $(KitName):$(MotorName)_NERAW_MONITOR.  Native encoder raw counts
#           $(KitName):$(MotorName)_NEDIAL_MONITOR. Native encoder dial reading (raw * ueres)
#	    $(KitName):$(MotorName)_NEUSER_MONITOR. Native encoder user reading (raw * ueres) + ueoff
#
#
#           Always specify last axis last.
#
#           homeallowed is only for system setup purposes, where master and slave are not tightly coupled.  Home allowed is good for homming a
#           slave after the master home is completed.
#
#    slave cmd (0 master cmd, 1 master encoder),
#    gantrymode (0 off 1 on), 
#		0 Off - applications where master/slave are not tightly/mechanically coupled
#			Limit activation on slave during coupled move will result in slave reverting to independant mode
#			Limit activation on master or slave during independant move results in motor stop only
#		1 On  - applications where master/slave are tightly/mechanically coupled.
#			Limit activation on slave during coupled move will result in master stop
#			Limit activation on master or slave during independant move results in motor stop only
#
#    EPS digital home/Motor Interlock Port
#		EPS digital home (0 off 1-8 nominates port) 
#		Motor interlock digital port number if -1 to -8
#
#    EPS digital away/Interlock switch type (Function depends on EPS digital home/Motor Interlock Port value specified.. above)
#               EPS digital away (0 off 1-8 nominates port)
#		Interlock switch type -1 normally closed, all other values is normally open interlock switch type
# 
#    stepper position maintenance
#		0 off
#		1 on and uses standard encoder settings, and code generator output, or
#		"invalid string" on and uses standard encoder settings, and code generator output
#		"uenc[Axis]=[expression], ueres, ueoff, uencValidVar" on and uses user defined encoder location.  Same format as used for slave definition, see above.
#
#    min steps error before engaging pos maintenance(in EGU), 
#    max steps error before disengaging pos maintenance (in EGU)


G21X3NameConfig(0,"A",3,-1,0,0.0,1,-1,-1,0,25,13,0,13,1,1,0.000001,"",0,1,0,0,1,0.001,.110)
G21X3NameConfig(0,"B",3,-1,0,0.0,1,-1,-1,0,25,13,0,13,1,1,0.000001,"",0,1,0,0,1,0.001,.110)
G21X3NameConfig(0,"C",3,-1,0,0.0,1,-1,-1,0,25,13,0,13,1,1,0.000001,"",0,1,0,0,1,0.001,.110)
G21X3NameConfig(0,"D",3,-1,0,0.0,1,-1,-1,0,25,13,0,13,1,1,0.000001,"",0,1,0,0,1,0.001,.110)
G21X3NameConfig(0,"E",3,-1,0,0.0,1,-1,-1,0,25,13,0,13,1,1,0.000001,"",0,1,0,0,1,0.001,.110)
G21X3NameConfig(0,"F",3,-1,0,0.0,1,-1,-1,0,25,13,0,13,1,1,0.000001,"",0,1,0,0,1,0.001,.110)
G21X3NameConfig(0,"G",3,-1,0,0.0,1,-1,-1,0,25,13,0,13,1,1,0.000001,"",0,1,0,0,1,0.001,.110)
G21X3NameConfig(0,"H",3,-1,0,0.0,1,-1,-1,0,25,13,0,13,1,1,0.000001,"",0,1,0,0,1,0.001,.110)

# In the G21X3StartCard command, the parameters are:
#
# card number starting from 0
# code file to deliver to the card we are starting "" = use generated code
# Burn to EEPROM conditions
# 0 = transfer code if differs from eeprom, dont burn code to eeprom, execute/restart code.
# 1 = transfer code if differs from eeprom, burn code to eeprom, execute/restart code.
#
# display code. Bit 1 displays generated code and or the code file code, Bit 2 displays uploaded code

G21X3StartCard(0,"",1,0)

cd ${TOP}/iocBoot/${IOC}
iocInit()

## Start any sequence programs
#seq sncxxx,"user=icsHost"
