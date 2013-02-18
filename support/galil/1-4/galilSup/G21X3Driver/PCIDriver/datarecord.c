/*This file contains a sample program that excersises the Galil pci driver.
It repeatedly prints out the 18x6 data record.

[root@localhost PCIDriver]# gcc datarecord.c -o datarecord
[root@localhost PCIDriver]# ./datarecord
Connected to DMC1886 Rev 1.0
:Serial number  1.0000
:SampleNumber 10660, DeltaSampleNumber 10660, NumberOfSamples 0
Inputs    1 255 255   0   0   0   0   0   0   0
Outputs   0   0   0   0   0   0   0   0   0   0
ErrorCode   0, GeneralStatus   1
SegmentCountS     0, CoordinatedMoveStatusS 16384, CoordinatedMoveDistanceS 0
SegmentCountT     0, CoordinatedMoveStatusT 16384, CoordinatedMoveDistanceT 0
Axis Status Switches StopCode ReferencePosition EncoderPosition PositionError AuxiliaryPosition Velocity Torque AnalogInput
   0  16424      238        1            148000          147938            62                 0        0    372       16416
   1  16424      174        1            188000          187940            60                 0        0    360        8256
   2  16424      174        1            208000          207907            93                 0        0    558        8192
   3  16424      174        1            222000          221951            49                 0        0    294        8192
   4  16384      110        1                 0               0             0                 0        0      0        8224
   5  16384      110        1                 0               0             0                 0        0      0        8208
   6  16384      110        1                 0               0             0                 0        0      0        8224
   7  16384      110        1                 0               0             0                 0        0      0        8080
*/

char GALIL_FILE[100] = "/dev/galilpci\0"; //default device file is /dev/galilpci0.  The null character will be overwritten.
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>		// open 
#include <unistd.h>		// exit 
#include <string.h>
//#include <time.h>
#include <sys/ioctl.h>
#include <math.h> //pow

int file_desc;  //the file descriptor returned from open()
int command(char* Command, char* Response, int ResponseSize);  //function is defined below

//_______________________________________________________________________________________________
//DMC-18x6

// Structure must be aligned on 1 byte boundary
#pragma pack(1)

typedef struct
{
   unsigned short   Status;
   unsigned char    Switches;          //~TS
   unsigned char    StopCode;          //SC
   long             ReferencePosition; //RP
   long             EncoderPosition;   //TP
   long             PositionError;     //TE
   long             AuxiliaryPosition; //TD
   long             Velocity;          //TV scaled
   long             Torque;            //TT scaled  18x6 new size
   short            AnalogInput;       //@AN scaled
   unsigned short   Reserved;
   long             UserVariable;      //ZA
}DMC18x6AxisData;

typedef struct
{
   unsigned short      SegmentCount; //_CS
   unsigned short      Status;
   long                Distance;     //_AV
   unsigned short      BufferSpace;  //_LM  18x6
}DMC18x6VectorData;

typedef struct
{
   unsigned short      SampleNumber;
   unsigned char       InputByte[10];
   unsigned char       OutputByte[10];
   
   short               Reserved0[8];  //18x6
   char                Reserved1[8];  //18x6
   unsigned char       ErrorCode;
   unsigned char       GeneralStatus; //Thread Status
   unsigned long       Reserved2;     //18x6 
   
   unsigned long       ContourSegmentCount; //18x6
   unsigned short      ContourBufferSpace;  //_CM  18x6
   
   DMC18x6VectorData   CoordinateSystem[2]; //0 is S, 1 is T
   DMC18x6AxisData     Axis[8];             // One DMC18x6AxisData struct for each axis
   char                Padding[146];        //bring the data record up to 512 bytes (the size of the dual port RAM)
}DMC18x6DataRecord;


//_______________________________________________________________________________________________
//DMC-1800

typedef struct
{
   unsigned short   Status;
   unsigned char    Switches;          //~TS
   unsigned char    StopCode;          //SC
   long             ReferencePosition; //RP
   long             EncoderPosition;   //TP
   long             PositionError;     //TE
   long             AuxiliaryPosition; //TD
   long             Velocity;          //TV scaled
   short            Torque;            //TT scaled
   short            AnalogInput;       //@AN scaled     
}DMC18x0AxisData;

typedef struct
{
   unsigned short      SegmentCount; //_CS
   unsigned short      Status;
   long                Distance;     //_AV
}DMC18x0VectorData;

typedef struct
{
   unsigned short      SampleNumber;
   unsigned char       InputByte[10];
   unsigned char       OutputByte[10];
   unsigned char       ErrorCode;
   unsigned char       GeneralStatus;
   DMC18x0VectorData   CoordinateSystem[2]; //0 is S, 1 is T
   DMC18x0AxisData     Axis[8];             // One DMC18x0AxisData struct for each axis
   char                Padding[248];        //bring the data record up to 512 bytes (the size of the dual port RAM)
}DMC18x0DataRecord;

//sets the alignment to the one that was in effect when compilation started.  see also command line option -fpack-struct[=<n>]
#pragma pack()

//_______________________________________________________________________________________________

int main(int argc, char *argv[]) //./a.out X connects to /dev/galilpciX.  No arg connects to /dev/galilpci0
{
   DMC18x6DataRecord r;
   char acResponse[1000] = {0};    //buffer to hold controller responses
   int i = 0, j = 0, a = 0;
   unsigned short LastSampleNumber = 0;
   char acCommand[10];
   int n = 1;  //DR value 1 to 8
   //unsigned short DeltaSample = pow(2, n);

   //memset(&r, '\0', sizeof(DMC18x0DataRecord));

	file_desc = open((argc == 2 ? strcat(GALIL_FILE, argv[1]) : strcat(GALIL_FILE, "0")), O_RDWR);  //open the Galil PCI driver for read and write
	if (file_desc < 0) 
	{
      printf("Can't open device file %s\n", GALIL_FILE);
		exit(-1);
	}

   //ioctl(file_desc, 0); //clear FIFOs

	//get controller model number and firmware version
	command("\x12\x16\r", acResponse, sizeof(acResponse));
	printf("Connected to %s", acResponse);

   //get serial number	
	command("MG_BN\r", acResponse, sizeof(acResponse));
	printf("Serial number %s", acResponse);

   sprintf(acCommand, "DR%i\r", n); //set data record refresh rate
   command(acCommand, acResponse, sizeof(acResponse));
   command("DU1\r", acResponse, sizeof(acResponse)); //turn on dual port RAM on the 1800.  Will return ? on 18x6.


   for(i = 0; 1; i++)
   {
      usleep(100000); //100 ms
      while(0 != ioctl(file_desc, 2, &r)) {} //wait for the data record

      //for(j = 0; j < sizeof(r); j++)
         //printf("%02x ", *((char*)&r + j) & 0xff); //print the data record in hex
      //printf("\n");

      printf("SampleNumber %5hu, DeltaSampleNumber %hu, NumberOfSamples %i\n", r.SampleNumber, r.SampleNumber - LastSampleNumber, i);
      printf("Inputs  %3i %3i %3i %3i %3i %3i %3i %3i %3i %3i\n", r.InputByte[0], r.InputByte[1], r.InputByte[2], r.InputByte[3], r.InputByte[4], r.InputByte[5], r.InputByte[6], r.InputByte[7], r.InputByte[8], r.InputByte[9]);
      printf("Outputs %3i %3i %3i %3i %3i %3i %3i %3i %3i %3i\n", r.OutputByte[0], r.OutputByte[1], r.OutputByte[2], r.OutputByte[3], r.OutputByte[4], r.OutputByte[5], r.OutputByte[6], r.OutputByte[7], r.OutputByte[8], r.OutputByte[9]);
      printf("ErrorCode %3i, GeneralStatus %3i\n", r.ErrorCode, r.GeneralStatus);
      printf("SegmentCountS %5hu, CoordinatedMoveStatusS %5hu, CoordinatedMoveDistanceS %li\n", r.CoordinateSystem[0].SegmentCount, r.CoordinateSystem[0].Status, r.CoordinateSystem[0].Distance);
      printf("SegmentCountT %5hu, CoordinatedMoveStatusT %5hu, CoordinatedMoveDistanceT %li\n", r.CoordinateSystem[1].SegmentCount, r.CoordinateSystem[1].Status, r.CoordinateSystem[1].Distance);
   
      printf("Axis Status Switches StopCode ReferencePosition EncoderPosition PositionError AuxiliaryPosition Velocity Torque AnalogInput\n");
      for(a = 0; a < 8; a++)
         printf("%4i %6hu %8hu %8hu %17li %15li %13li %17li %8li %6li %11hi\n",  a, r.Axis[a].Status, r.Axis[a].Switches, r.Axis[a].StopCode, r.Axis[a].ReferencePosition, r.Axis[a].EncoderPosition, r.Axis[a].PositionError, r.Axis[a].AuxiliaryPosition, r.Axis[a].Velocity, r.Axis[a].Torque, r.Axis[a].AnalogInput);

      //check that we don't miss any samples
      //if((unsigned short)(r.SampleNumber - LastSampleNumber) != DeltaSample && (i >= 2))
         //break;

      LastSampleNumber = r.SampleNumber;

      /*for(a = 1; a < 8; a++) //JG*=22000000 test for same sample
      {
         if(r.Axis[0].ReferencePosition != r.Axis[a].ReferencePosition)
            exit(0);
      }*/
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

