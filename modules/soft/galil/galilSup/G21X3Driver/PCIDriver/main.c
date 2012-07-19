/*This file contains a sample program that excersises the Galil pci driver.
The function command() sends a command to the controller and retrieves the response.
main() uses command() to get the controller's model and serial numbers, times the average
time it takes to send a command and receive a response by measuring the amount
of time it takes to send 1000 commands and dividing by 1000, downloads and runs a program then uploads it,
and downloads an array and uploads it.  It also checks the ioctl functions in the driver.

[root@localhost PCIDriver]# gcc main.c -o main
[root@localhost PCIDriver]# ./main
Connected to DMC1886 Rev 1.0
:Serial number  1.0000
: 4538870.0000
:Sent 1000 commands.  Average time per command: 0.050000 ms
Uploaded program
#AUTO
ER0
WT100
ER-1
WT100
JP#AUTO
EN

▒:
:Uploaded array  0.0000, 1.0000, 2.0000, 3.0000, 4.0000
ioctl 0 should return 0:  0
Serial number after wait  1.0000
:
ioctl 0 should return 0:  0
GOOD
:
ioctl 1 should return 1: 1
ioctl 2 should return -1: -1
interrupt status should be f0: f0
[root@localhost PCIDriver]#
*/

char GALIL_FILE[100] = "/dev/galilpci\0"; //default device file is /dev/galilpci0.  The null character will be overwritten.
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>		// open 
#include <unistd.h>		// exit 
#include <string.h>
#include <time.h>
#include <sys/ioctl.h>
#include "pciioctl.h"

int file_desc;  //the file descriptor returned from open()
int command(char* Command, char* Response, int ResponseSize);  //function is defined below


int main(int argc, char *argv[]) //./a.out X connects to /dev/galilpciX.  No arg connects to /dev/galilpci0
{
	char acResponse[1000] = {0};    //buffer to hold controller responses
	int i = 0;                      //counter for time measurement
	clock_t time;                   //holds initial time for time measurement
	int NUM_COMMANDS = 1000;        //the number of commands to send to compute average command processing time
   int iBytes;
	
   file_desc = open((argc == 2 ? strcat(GALIL_FILE, argv[1]) : strcat(GALIL_FILE, "0")), O_RDWR);  //open the Galil PCI driver for read and write
   if (file_desc < 0) 
   {
      printf("Can't open device file %s\n", GALIL_FILE);
      exit(-1);
   }

   ioctl(file_desc, GALIL_PCI_CLEARFIFO); //clear FIFOs

	//get controller model number and firmware version
	command("\x12\x16\r", acResponse, sizeof(acResponse));
	printf("Connected to %s", acResponse);

   //get serial number	
	command("MG_BN\r", acResponse, sizeof(acResponse));
	printf("Serial number %s", acResponse);

   iBytes = write(file_desc, "MGTIME\r", 7); //write the command to the controller
   //printf("write %i\n", iBytes);

   while(1)
   {
      iBytes = read(file_desc, acResponse, sizeof(acResponse));
      //printf("read %i %s\n", iBytes, acResponse);

      if(iBytes <= 0)   //nothing read, keep reading until :
         continue;
      else if (acResponse[iBytes - 1] != ':' && acResponse[iBytes - 1] != '?') //Got partial response.  iBytes > 0
      {
         acResponse[iBytes] = '\0'; //null terminate
         printf("%s", acResponse); //print the partial response
         continue;      //keep checking, no colon yet
      }
      else              //got a colon or a question mark
      {
         acResponse[iBytes] = '\0'; //null terminate
         printf("%s", acResponse);
         break;
      }
   }

   //write(file_desc, "\x88\x01\x00\x01\xff", 5); //write ERX=-1 to the controller

	//Measure the amount of time it takes to send 1000 commands
	time = clock();  //store initial time

   //command("x=123\r", acResponse, sizeof(acResponse));

	for(i = 0; i < NUM_COMMANDS; i++)
	{
      //memset(acResponse, '\0', sizeof(acResponse));
		command("MG\"GOOD\"\r", acResponse, sizeof(acResponse));

//		if(strcmp(acResponse, "GOOD\r\n:"))
//      {
//         printf("ERROR\n");
//			break;  //error
//      }
	}

   printf("Sent %i commands.  Average time per command: %f ms\n", i, ((float) (clock() - time)) / CLOCKS_PER_SEC / NUM_COMMANDS * 1000);

   //download a program
   command("AB0\r", acResponse, sizeof(acResponse));  //stop motion and program
   command("DL\r#AUTO\rER0\rWT100\rER-1\rWT100\rJP#AUTO\rEN\r\\", acResponse, sizeof(acResponse));
   command("XQ\r", acResponse, sizeof(acResponse));  //run the program from the top

   //upload the program just downloaded
   command("UL\r", acResponse, sizeof(acResponse));
   printf("Uploaded program\n%s", acResponse);

   //download an array
   command("DM A[5]\r", acResponse, sizeof(acResponse)); //define the array on the controller
   command("QD A[]\r0,1,2,3,4\\", acResponse, sizeof(acResponse));

   //upload the array just downloaded
   command("QU A[],0,4,1\r", acResponse, sizeof(acResponse));
   printf("\n Uploaded array %s\n", acResponse);

   //test clearing write FIFO
   command("WT5000\r", acResponse, sizeof(acResponse));
   usleep(1000); //wait for controller to start processing WT5000
   printf("ioctl 0 should return 0:  %i\n", ioctl(file_desc, GALIL_PCI_CLEARFIFO)); //clear FIFOs.  Without this, next line would block.
   command("MG_BN\r", acResponse, sizeof(acResponse));
   printf("Serial number after wait %s\n", acResponse);

   //test clearing read FIFO
   write(file_desc, "MG\"BAD\"\r", 8); //write the command to the controller
   usleep(1000); //wait for read FIFO to contain "BAD"
   printf("ioctl 0 should return 0:  %i\n", ioctl(file_desc, GALIL_PCI_CLEARFIFO)); //clear FIFOs.  Without this, BAD shows up in the response to the next line
   command("MG\"GOOD\"\r", acResponse, sizeof(acResponse));
   printf("%s\n", acResponse);

   write(file_desc, "MG\"WAZZUP\"\r", 11); //write the command to the controller
   usleep(1000); //wait for read FIFO to contain "WAZZUP"
   printf("ioctl 1 should return 1: %i\n", ioctl(file_desc, GALIL_PCI_CHARAVAIL)); //check if characters available to read.  Should be 1.
   printf("ioctl 2 should return -1: %i\n", ioctl(file_desc, GALIL_PCI_GETFIFODR));

   //test interrupts (UI is usually generated by the application program running on the controller)
   int Status = 1;
   ioctl(file_desc, GALIL_PCI_INTSTATUS, &Status); //clear the interrupt status byte queue

   command("UI0\r", acResponse, sizeof(acResponse));
   Status = 0;
   ioctl(file_desc, GALIL_PCI_INTSTATUS, &Status); //go to sleep until the interrupt comes in
   printf("interrupt status should be f0: %x\n", Status);

   close(file_desc);  //close the Galil PCI driver
   return 0;
}


//command() sends an ASCII Command (e.g. "TPX") to the controller and retrieves a Response (e.g. "123\r\n:").
//The size of Response should be supplied as ResponseSize so that unallocated memory is not overwritten.
//If you statically allocate your response buffer (e.g. char buff[100]) use sizeof(buff).

int command(char* Command, char* Response, int ResponseSize) //returns the number of bytes read
{
	char acPartialResponse[512] = {0}; //buffer to contain partial responses (which will be concatenated together to form the final response)
	int iPartialBytesRead = 0; //number of bytes read each time through the loop
	int iTotalBytesRead = 0;   //the total number of bytes read.  Can't exceed ResponseSize.

	Response[0] = 0; //set response to null string 
	write(file_desc, Command, strlen(Command)); //write the command to the controller
	//write(file_desc, "\r", 1);

   //keep reading until we (a) get a colon (b) get a question mark (c) fill up the callers Response buffer
	while(1)
	{
		iPartialBytesRead = read(file_desc, acPartialResponse, sizeof(acPartialResponse)); //read some characters
		
      if(iPartialBytesRead <= 0)   //nothing read, keep reading until :
         continue;
      else if(iTotalBytesRead + iPartialBytesRead > ResponseSize) //get out of the loop if we will fill up the caller's buffer, iPartialBytesRead >= 1
         break;
      else 
      {
         strncat(Response, acPartialResponse, iPartialBytesRead); //add the partial response to the full response.  Response is null terminated
         iTotalBytesRead += iPartialBytesRead; //tally up the total number of bytes read
   //    printf("%s|%s|%i\n", Response, acPartialResponse, iPartialBytesRead); 
         if (acPartialResponse[iPartialBytesRead - 1] == ':' || acPartialResponse[iPartialBytesRead - 1] == '?') //got a colon, iPartialBytesRead >= 1
            break;
      }
	}
	
	return(iTotalBytesRead);
}

