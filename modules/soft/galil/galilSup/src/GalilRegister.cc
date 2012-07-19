/*
FILENAME...	GalilRegister.cc
USAGE...	Register Galil motor device driver shell commands.

Version:	1-4
Modified By:	Clift
Last Modified:	2008/07/16
*/

/*****************************************************************
                          COPYRIGHT NOTIFICATION
*****************************************************************

(C)  COPYRIGHT 1993 UNIVERSITY OF CHICAGO

This software was developed under a United States Government license
described on the COPYRIGHT_UniversityOfChicago file included as part
of this distribution.
**********************************************************************/

#include <iocsh.h>
#include "GalilRegister.h"
#include "epicsExport.h"

extern "C"
{

// Galil Setup arguments
static const iocshArg setupArg0 = {"Max. controller count", iocshArgInt};
static const iocshArg setupArg1 = {"Polling rate", iocshArgInt};

// Galil Config arguments
static const iocshArg configArg0 = {"Card being configured", iocshArgInt};
static const iocshArg configArg1 = {"asyn port name", iocshArgString};
static const iocshArg configArg2 = {"asyn address (GPIB)", iocshArgInt};

// Galil 21X3 Config arguments
static const iocshArg G21X3configArg0 = {"Card being configured", iocshArgInt};
static const iocshArg G21X3configArg1 = {"IP", iocshArgString};
static const iocshArg G21X3configArg2 = {"Number of Axes Used", iocshArgInt};
static const iocshArg G21X3configArg3 = {"Model Number", iocshArgInt};
static const iocshArg G21X3configArg4 = {"Limit Switch Type", iocshArgInt};
static const iocshArg G21X3configArg5 = {"Home Switch Type", iocshArgInt};
static const iocshArg G21X3configArg6 = {"Controller Home Direction", iocshArgInt};

// Galil 21X3 NameConfig arguments
static const iocshArg G21X3NameconfigArg0 = {"Card being configured", iocshArgInt};
static const iocshArg G21X3NameconfigArg1 = {"Axis Name being configured", iocshArgString};
static const iocshArg G21X3NameconfigArg2 = {"Motor Type", iocshArgInt};
static const iocshArg G21X3NameconfigArg3 = {"Reverse or Forward Home", iocshArgInt};
static const iocshArg G21X3NameconfigArg4 = {"Invert Coordinates", iocshArgInt};
static const iocshArg G21X3NameconfigArg5 = {"Home Position in EGU", iocshArgDouble};
static const iocshArg G21X3NameconfigArg6 = {"Program regs when home", iocshArgInt};
static const iocshArg G21X3NameconfigArg7 = {"Main Encoder", iocshArgInt};
static const iocshArg G21X3NameconfigArg8 = {"Aux Encoder", iocshArgInt};
static const iocshArg G21X3NameconfigArg9 = {"SSI Encoder Input", iocshArgInt};
static const iocshArg G21X3NameconfigArg10 = {"SSI Total Bits", iocshArgInt};
static const iocshArg G21X3NameconfigArg11 = {"SSI Single Turn Bits", iocshArgInt};
static const iocshArg G21X3NameconfigArg12 = {"SSI Error Bits", iocshArgInt};
static const iocshArg G21X3NameconfigArg13 = {"SSI Timming Code", iocshArgInt};
static const iocshArg G21X3NameconfigArg14 = {"SSI Data Format", iocshArgInt};
static const iocshArg G21X3NameconfigArg15 = {"Limit As Home", iocshArgInt};
static const iocshArg G21X3NameconfigArg16 = {"EGU Travelled after Home/Limit Activation", iocshArgDouble};
static const iocshArg G21X3NameconfigArg17 = {"Slave Positioner Data", iocshArgString};
static const iocshArg G21X3NameconfigArg18 = {"Slave Command Source", iocshArgInt};
static const iocshArg G21X3NameconfigArg19 = {"Gantry Mode", iocshArgInt};
static const iocshArg G21X3NameconfigArg20 = {"Digital Home Input", iocshArgString};
static const iocshArg G21X3NameconfigArg21 = {"Digital Away Input", iocshArgInt};
static const iocshArg G21X3NameconfigArg22 = {"Position Maintenance", iocshArgString};

// Galil 21X3 StartCard arguments
static const iocshArg G21X3StartcardArg0 = {"Card being started", iocshArgInt};
static const iocshArg G21X3StartcardArg1 = {"Code file", iocshArgString};
static const iocshArg G21X3StartcardArg2 = {"Burn to EEPROM conditions", iocshArgInt};
static const iocshArg G21X3StartcardArg3 = {"Display Galil code", iocshArgInt};

static const iocshArg * const GalilG21X3SetupArgs[2] = {&setupArg0,
                                                          &setupArg1};

static const iocshArg * const GalilG21X3ConfigArgs[7] = {&G21X3configArg0,
                                                           &G21X3configArg1,
                                                           &G21X3configArg2,
                                                           &G21X3configArg3,
							   &G21X3configArg4,
                                                           &G21X3configArg5,
							   &G21X3configArg6};

static const iocshArg * const GalilG21X3NameArgs[23] = {&G21X3NameconfigArg0,
                                                         &G21X3NameconfigArg1,
                                                         &G21X3NameconfigArg2,
							 &G21X3NameconfigArg3,
                                                         &G21X3NameconfigArg4,
							 &G21X3NameconfigArg5,
							 &G21X3NameconfigArg6,
							 &G21X3NameconfigArg7,
							 &G21X3NameconfigArg8,
                                                         &G21X3NameconfigArg9,
							 &G21X3NameconfigArg10,
							 &G21X3NameconfigArg11,
							 &G21X3NameconfigArg12,
							 &G21X3NameconfigArg13,
						   	 &G21X3NameconfigArg14,
							 &G21X3NameconfigArg15,
							 &G21X3NameconfigArg16,
							 &G21X3NameconfigArg17,
						 	 &G21X3NameconfigArg18,
							 &G21X3NameconfigArg19,
							 &G21X3NameconfigArg20,
							 &G21X3NameconfigArg21,
							 &G21X3NameconfigArg22};
							 
static const iocshArg * const GalilG21X3StartArgs[4] = {&G21X3StartcardArg0,
                                                         &G21X3StartcardArg1,
							 &G21X3StartcardArg2,
							 &G21X3StartcardArg3};

static const iocshFuncDef setupG21X3  = {"G21X3Setup", 2, GalilG21X3SetupArgs};

static const iocshFuncDef configG21X3  = {"G21X3Config",  7, GalilG21X3ConfigArgs};
static const iocshFuncDef nameG21X3    = {"G21X3NameConfig", 23, GalilG21X3NameArgs};
static const iocshFuncDef startG21X3    = {"G21X3StartCard", 4,  GalilG21X3StartArgs};

static void setupG21X3CallFunc(const iocshArgBuf *args)
{
    G21X3Setup(args[0].ival, args[1].ival);
}


static void configG21X3CallFunc(const iocshArgBuf *args)
{
    G21X3Config(args[0].ival, args[1].sval, args[2].ival, args[3].ival, args[4].ival, args[5].ival, args[6].ival);
}

static void nameG21X3CallFunc(const  iocshArgBuf *args)
{
    G21X3NameConfig(args[0].ival, args[1].sval, args[2].ival, args[3].ival, args[4].ival, args[5].dval, args[6].ival, args[7].ival, args[8].ival,args[9].ival,args[10].ival,args[11].ival, args[12].ival, args[13].ival, args[14].ival,args[15].ival, args[16].dval,args[17].sval,args[18].ival,args[19].ival,args[20].sval,args[21].ival,args[22].sval);
}

static void startG21X3CallFunc(const iocshArgBuf *args)
{
    G21X3StartCard(args[0].ival, args[1].sval, args[2].ival, args[3].ival);
}

static void GalilRegister(void)
{

    iocshRegister(&setupG21X3, setupG21X3CallFunc);
    iocshRegister(&configG21X3, configG21X3CallFunc);
    iocshRegister(&nameG21X3, nameG21X3CallFunc);
    iocshRegister(&startG21X3, startG21X3CallFunc);
}

epicsExportRegistrar(GalilRegister);

} // extern "C"
