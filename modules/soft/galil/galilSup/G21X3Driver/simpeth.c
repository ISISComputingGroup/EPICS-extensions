/* This is a Linux application for Ethernet communications to a
   Galil motion controller

[root@localhost galil]# gcc -g -Wall simpeth.c dmclnxlib/dmclnx.a -Idmclnxlib -osimpeth
[root@localhost galil]# ./simpeth 10.0.0.67
Simple Ethernet communications example for Galil Motion Controllers

Connected to DMC2182 Rev 1.0o
:
The serial number is  5532.0000
:
The value of the inputs is 255
The value of the X axis motor position is 4000
The value of the X axis commanded position is 8000
The value of the X axis position error is 4000
The value of the X axis commanded position using QR is 8000

Done
[root@localhost galil]#           
*/
#include "dmclnx.h"

void PrintError(long rc);

int main(int argc, char* argv[])
{
   long           rc = 0;          /* Return code */
   int            fInMotion = 0;   /* Motion complete flag */
   char           buffer[32] = ""; /* Response from controller */
   HANDLEDMC      hdmc = -1;       /* Handle to controller */
   CONTROLLERINFO controllerinfo;  /* Controller information structure */

   if(argc != 2) //if there aren't two arguments.  IP address should be in argv[1]
     {
       printf("Usage:  %s ipaddress\n", argv[0]);
       return(0); //exit the program
     }

   printf("Simple Ethernet communications example for Galil Motion Controllers\n\n");

   memset(&controllerinfo, '\0', sizeof(controllerinfo));

   controllerinfo.cbSize = sizeof(controllerinfo);
   controllerinfo.usModelID = MODEL_2100;
   controllerinfo.fControllerType = ControllerTypeEthernet;
   controllerinfo.ulTimeout = 1000;
   controllerinfo.ulDelay = 5;
   strcpy(controllerinfo.hardwareinfo.socketinfo.szIPAddress, argv[1]);
   controllerinfo.hardwareinfo.socketinfo.fProtocol = EthernetProtocolTCP;
 
   DMCInitLibrary();

   /* Open the connection */   
   rc = DMCOpen(&controllerinfo, &hdmc);
   if (rc)
   {
      PrintError(rc);
      return rc;  
   }

  /* Get the model and serial number */
   rc = DMCCommand(hdmc, "\x12\x16", buffer, sizeof(buffer));
   printf("Connected to %s\n", buffer); 
   rc = DMCCommand(hdmc, "MG _BN", buffer, sizeof(buffer));
   printf("The serial number is %s\n", buffer);      

   /* Query the state of the inputs */
   rc = DMCCommand(hdmc, "TI;", buffer, sizeof(buffer));
   if (rc)
      PrintError(rc);
   else
   {
      int temp;
      temp = atoi(buffer);
      printf("The value of the inputs is %d\n", temp);      
   }     

   /* Set the X axis position */
   rc = DMCCommand(hdmc, "DP4000;", buffer, sizeof(buffer));
   if (rc)
      PrintError(rc);

   /* Query the X axis motor position */
   rc = DMCCommand(hdmc, "TPX;", buffer, sizeof(buffer));
   if (rc)
      PrintError(rc);
   else
   {
      long temp;
      temp = atol(buffer);
      printf("The value of the X axis motor position is %ld\n", temp);      
   }     
   
   /* Move the X axis */
   rc = DMCCommand(hdmc, "PR4000;", buffer, sizeof(buffer));
   if (rc)
      PrintError(rc);
   rc = DMCCommand(hdmc, "BGX;", buffer, sizeof(buffer));
   if (rc)
      PrintError(rc);

   fInMotion = 1;

   /* Wait until after motion */
   while (!rc && fInMotion)
   {
      rc = DMCCommand(hdmc, "MG_BGX;", buffer, sizeof(buffer));
      if (rc)
         PrintError(rc);
      else
         fInMotion = atoi(buffer);        
   }

   /* Query the X axis commanded position */
   rc = DMCCommand(hdmc, "RPX;", buffer, sizeof(buffer));
   if (rc)
      PrintError(rc);
   else
   {
      long temp;
      temp = atol(buffer);
      printf("The value of the X axis commanded position is %ld\n", temp);
   }

   /* Query the X axis position error */
   rc = DMCCommand(hdmc, "TEX;", buffer, sizeof(buffer));
   if (rc)
      PrintError(rc);     
   else
   {
      long temp;
      temp = atol(buffer);
      printf("The value of the X axis position error is %ld\n", temp);      
   }     

   /* If the controller supports the QR command, retrieve the data record */
   {
      DMCDATARECORDQR   dmcdatarecordqr;
      rc = DMCGetDataRecordQR(hdmc, &dmcdatarecordqr, sizeof(dmcdatarecordqr));
      if (!rc)
      {
         printf("The value of the X axis commanded position using QR is %ld\n",
            dmcdatarecordqr.DataRecord.AxisInfo[0].ReferencePosition);
      }
   }
                       
   /* Close the connection */                       
   rc = DMCClose(hdmc);
   if (rc)
   {
      PrintError(rc);
      return rc;  
   }

   printf("\nDone\n");
   
   return 0L;
}

void PrintError(long rc)
{
   printf("An error has occurred. Return code = %ld\n", rc);
   fflush(stdout);
}
