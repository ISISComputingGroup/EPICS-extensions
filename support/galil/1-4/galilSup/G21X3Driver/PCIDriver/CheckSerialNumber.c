/*Repeatedly sends MG_BN to the specified PCI controller, gets the reponse,
and compares against the first serial number retreived.  If a difference is
detected, the program prints the erroneous response and stops.  This can
be used as communication test.

root@localhost PCIDriver]# gcc CheckSerialNumber.c -o CheckSerialNumber
[root@localhost PCIDriver]# ./CheckSerialNumber
Connected to DMC1886 Rev 1.0
:Serial number  1.0000
40592
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

int file_desc;  //the file descriptor returned from open()
int command(char* Command, char* Response, int ResponseSize);  //function is defined below


int main(int argc, char *argv[]) //./a.out X connects to /dev/galilpciX.  No arg connects to /dev/galilpci0
{
	char acResponse[1000] = {0};    //buffer to hold controller responses
   char acSerial[100] = {0}  ;
	int i = 0;                      //counter for time measurement
	
   file_desc = open((argc == 2 ? strcat(GALIL_FILE, argv[1]) : strcat(GALIL_FILE, "0")), O_RDWR);  //open the Galil PCI driver for read and write
   if (file_desc < 0) 
   {
      printf("Can't open device file %s\n", GALIL_FILE);
      exit(-1);
   }

   ioctl(file_desc, 0); //clear FIFOs

	//get controller model number and firmware version
	command("\x12\x16\r", acResponse, sizeof(acResponse));
	printf("Connected to %s", acResponse);

   //get serial number	
	command("MG_BN\r", acSerial, sizeof(acSerial));
	printf("Serial number %s", acSerial);

   
   for(i = 0; 1; i++)
   {
      command("MG_BN\r", acResponse, sizeof(acResponse));
      
      if(strcmp(acSerial, acResponse))
         {
            printf("\n%s\n", acResponse);
            break;
         }
      else
         printf("%i\r", i);
   } 

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

