/*Demonstates using the Galil Linux API in dmclnx.h with a PCI controller
[root@localhost galil]# gcc -g simppci.c dmclnxlib/dmclnx.a -Idmclnxlib -osimppci -Wall
[root@localhost galil]# ./simppci
Simple pci bus communications example for Galil Motion Controllers.

Connected to DMC1886 Rev 1.0
:The serial number is  1.0000
:The value of the inputs is 1
The value of the X axis position is 4000
The value of the X axis position error is 51
The interrupt status for UI0 is f0
The interrupt status for UI1 is f1
The interrupt status for UI2 is f2
The interrupt status for UI3 is f3
The interrupt status for UI4 is f4
The interrupt status for UI5 is f5
The interrupt status for UI6 is f6
The interrupt status for UI7 is f7
The interrupt status for UI8 is f8
The interrupt status for UI9 is f9
The interrupt status for UI10 is fa
The interrupt status for UI11 is fb
The interrupt status for UI12 is fc
The interrupt status for UI13 is fd
The interrupt status for UI14 is fe
The interrupt status for UI15 is ff
Sample Number       57642
General Status =           0
Error code =           0
Axis 1 =         100
Axis 2 =         200

Done
[root@localhost galil]#
*/
#include "dmclnx.h"

void PrintError(long rc)
{
	printf("An error has occurred. Return code = %ld\n", rc);
   fflush(stdout);
}

//
//	Purpose : Sample program
//
int main(int argc, char* argv[])
{
	int            	i, temp;
	long           	ltemp,rc = 0;      // Return code
	int            	fInMotion = FALSE; // Motion complete flag
	char           	buffer[256] = "";  // Response from controller
	USHORT         	usStatus;          //the interrupt status byte retreived from the controller after a UI or EI interrupt
	HANDLEDMC      	hdmc = -1;         // Handle to controller
	CONTROLLERINFO 	controllerinfo;    // Controller information structure	

	printf("Simple pci bus communications example for Galil Motion Controllers. \n\n");
	
	memset(&controllerinfo, '\0', sizeof(controllerinfo));
	
	controllerinfo.cbSize = sizeof(controllerinfo);
	controllerinfo.usModelID = MODEL_1800; 
	controllerinfo.fControllerType = ControllerTypePCIBus;
	controllerinfo.ulTimeout = 1000;
	controllerinfo.hardwareinfo.businfo.fDataRecordAccess = DataRecordAccessFIFO;
	controllerinfo.ulSerialNumber = 0;  //use relative number
   controllerinfo.ulRelativeNumber = (argc == 2 ? atoi(argv[1]) : 0); //0 is for /dev/galilpci0, 1 for /dev/galilpci1...

	DMCInitLibrary();
	
	// Open the connection
	rc = DMCOpen( &controllerinfo, &hdmc );
	if (rc)
	{
		PrintError(rc);
	  return rc;  
	}
	
   // Get the model and serial number
   rc = DMCCommand(hdmc, "\x12\x16", buffer, sizeof(buffer));
   printf("Connected to %s", buffer);
   rc = DMCCommand(hdmc, "MG _BN", buffer, sizeof(buffer));
   printf("The serial number is %s", buffer);

	// Query the state of the inputs
	rc = DMCCommand( hdmc, "TI;", buffer, sizeof(buffer));
	if (rc)
	  PrintError(rc);
	else
	{
	  temp = atoi(buffer);
	  printf("The value of the inputs is %d\n", temp);      
	}     

	// Set the X axis position
	rc = DMCCommand( hdmc, "DP4000;", buffer, sizeof(buffer));
	if (rc)
	  PrintError(rc);
	
	// Query the X axis position
	rc = DMCCommand( hdmc, "TPX;", buffer, sizeof(buffer));
	if (rc)
		PrintError(rc);
	else
	{
		long temp;
		temp = atol(buffer);
		printf("The value of the X axis position is %ld\n", temp);      
	}     

	// Move the X axis
	rc = DMCCommand( hdmc, "PR4000;BGX;", buffer, sizeof(buffer));
	if (rc)
	  PrintError(rc);
	
	fInMotion = 1;
	
	// Wait until after motion
	while (!rc && fInMotion)
	{
	  rc = DMCCommand( hdmc, "MG_BGX;", buffer, sizeof(buffer));
	  if (rc)
			PrintError(rc);
	  else
			fInMotion = atoi(buffer);         
	}
	
	// Query the X axis position error
	rc = DMCCommand( hdmc, "TEX;", buffer, sizeof(buffer));
	if (rc)
	  PrintError(rc);
	else
	{
		ltemp = atol(buffer);
		printf("The value of the X axis position error is %ld\n", ltemp );      
	}     
	

   usStatus = 1;
   rc = DMCGetInterruptStatus(hdmc, &usStatus); //clear the interrupt queue

   for (i = 0; i < 16; i++)
   {
     char command[16];
   
     sprintf(command, "UI%d;", i);
     rc = DMCCommand(hdmc, command, buffer, sizeof(buffer));
     if (rc)
        PrintError(rc);

     usStatus = 0;
     rc = DMCGetInterruptStatus(hdmc, &usStatus); //sleep until we get an interrupt
     if (rc)
     {
      PrintError(rc);
     }
     else
      printf("The interrupt status for UI%d is %x\n", i, usStatus);
   }

	DMCCommand( hdmc, "DP 100,200", buffer, sizeof(buffer));
   DMCCommand( hdmc, "DR1;DU1", buffer, sizeof(buffer)); //turn on the data record
   sleep(1); //wait for the data record to refresh
		
	{	
		USHORT	type1;
		long		out1;
		
		rc = DMCRefreshDataRecord( hdmc, 0 );
		rc = DMCGetDataRecordByItemId( hdmc, DRIdSampleNumber, 0, &type1, &out1 );
 		printf("Sample Number %11li\n", out1 );
		
		rc = DMCGetDataRecordByItemId( hdmc, DRIdGeneralStatus, 0, &type1, &out1 );
 		printf("General Status = %11li\n", out1 );
 		
		rc = DMCGetDataRecordByItemId( hdmc, DRIdErrorCode, 0, &type1, &out1 );
 		printf("Error code = %11li\n", out1 );
 		
		rc = DMCGetDataRecordByItemId( hdmc, DRIdAxisReferencePosition, DRIdAxis1, &type1, &out1 );
 		printf("Axis 1 = %11li\n", out1 );
 		
		rc = DMCGetDataRecordByItemId( hdmc, DRIdAxisReferencePosition, DRIdAxis2, &type1, &out1 );
 		printf("Axis 2 = %11li\n", out1 );
	}
		
	// Close the connection
	rc = DMCClose(hdmc);
	if (rc)
	{
	  PrintError(rc);
	  return rc;  
	}
	
	printf("\nDone\n");
	
	return 0L;
}
