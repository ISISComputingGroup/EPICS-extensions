/*Minimal C program to communicate to PCI and Ethernet controllers on Linux, and any controller on Windows.
./hello connects to /dev/galilpci0
./hello 1 connects to /dev/galilpci1
./hello 1.2.3.4 connects to an Ethernet controller at IP address 1.2.3.4

TO COMPILE ON LINUX WITH THE GNU COMPILER
[root@localhost galil]# gcc hello.c -DLINUX dmclnxlib/dmclnx.a -Idmclnxlib -Wall -g -ohello
[root@localhost galil]# ./hello
Connected to DMC1886 Rev 1.0
:Serial number 1.0000
:
[root@localhost galil]#           


TO COMPILE ON WINDOWS WITH THE VISUAL C COMPILER:
C:\ftproot>cl hello.c -Ic:\progra~1\galil\dmcwin\include c:\progra~1\galil\dmcwin\lib\dmc32.lib
Microsoft (R) 32-bit C/C++ Optimizing Compiler Version 13.10.3077 for 80x86
Copyright (C) Microsoft Corporation 1984-2002. All rights reserved.

hello.c
Microsoft (R) Incremental Linker Version 7.10.3077
Copyright (C) Microsoft Corporation.  All rights reserved.

/out:hello.exe
hello.obj
c:\progra~1\galil\dmcwin\lib\dmc32.lib

C:\ftproot>hello
Connected to DMC1886 Rev 1.0adev
:Serial number 125.0000
:

C:\ftproot>
*/
#ifdef LINUX
	#include "dmclnx.h"
#else
	#include <windows.h>
	#include "dmccom.h"
#endif

char acResponse[256] = "";  // Response from controller
HANDLEDMC      hdmc = -1;   // Handle to controller

int main(int argc, char *argv[]) 
{
   #ifdef LINUX
      CONTROLLERINFO controllerinfo;    // Controller information structure	
      memset(&controllerinfo, 0, sizeof(controllerinfo)); //initialize the structure
      controllerinfo.cbSize = sizeof(controllerinfo); //store the size (in bytes) of the structure
      controllerinfo.ulTimeout = 1000; //set communication timeout to 1 second
   
      if(argc == 2 && strchr(argv[1], '.'))  //two tokens on the command line and the second contains period
      {
         controllerinfo.fControllerType = ControllerTypeEthernet;
         strcpy(controllerinfo.hardwareinfo.socketinfo.szIPAddress, argv[1]);
      }
      else //PCI (one token or second token has no period)
      {
         controllerinfo.fControllerType = ControllerTypePCIBus;
         controllerinfo.ulRelativeNumber = (argc == 2 ? atoi(argv[1]) : 0); //0 is for /dev/galilpci0, 1 for /dev/galilpci1...
      }
   
      DMCInitLibrary();
      DMCOpen(&controllerinfo, &hdmc); //Open the connection to the controller (Linux)
   #else //Windows
      DMCOpen(DMCSelectController(NULL), 0, &hdmc); //Open the connection to the controller (Windows)
   #endif

   DMCCommand(hdmc, "\x12\x16", acResponse, sizeof(acResponse)); //get model number
   printf("Connected to %s", acResponse);
   
   DMCCommand(hdmc, "MG _BN", acResponse, sizeof(acResponse)); //get serial number
   printf("Serial number%s\n", acResponse);

   DMCClose(hdmc); //Close the connection
   return 0;
}
