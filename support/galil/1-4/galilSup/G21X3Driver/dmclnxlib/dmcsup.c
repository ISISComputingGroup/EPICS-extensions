#include "dmclnx.h"
#include "../PCIDriver/pciioctl.h"


long WriteData(int iIndex, PCHAR pchCommand, ULONG cbCommand, PULONG pulBytesWritten)
{
   LONG  rc;
   
   switch (controller[iIndex].controllerinfo.fControllerType)
   {
      default:
         return 0;      
#ifdef DMC_ISABUS
      case ControllerTypeISABus:
         rc = ISABUSWriteData(iIndex, pchCommand, cbCommand, pulBytesWritten);
         break;
#endif
#ifdef DMC_PCIBUS
      case ControllerTypePCIBus:
         rc = PCIBUSWriteData(iIndex, pchCommand, cbCommand, pulBytesWritten);
         break;
#endif
#ifdef DMC_SERIAL
      case ControllerTypeSerial:
         rc = SERIALWriteData(iIndex, pchCommand, cbCommand, pulBytesWritten);
         break;
#endif
#ifdef DMC_ETHERNET
      case ControllerTypeEthernet:
         rc = ETHERNETWriteData(iIndex, pchCommand, cbCommand, pulBytesWritten);
         break;
#endif
   }

#ifdef DMC_DEBUG
   DMCTrace("   Bytes written <%ld>.\n", *pulBytesWritten);
   DMCTrace("Leaving WriteData.\n");
#endif   
   return rc;
}

long ReadData(int iIndex, PCHAR pchResponse, ULONG cbResponse, PULONG pulBytesRead)
{
   LONG  rc;
   
   switch (controller[iIndex].controllerinfo.fControllerType)
   {
      default:
         return 0;      
#ifdef DMC_ISABUS
      case ControllerTypeISABus:
         rc = ISABUSReadData(iIndex, pchResponse, cbResponse, pulBytesRead);
         break;
#endif
#ifdef DMC_PCIBUS
      case ControllerTypePCIBus:
         rc = PCIBUSReadData(iIndex, pchResponse, cbResponse, pulBytesRead);
         break;
#endif
#ifdef DMC_SERIAL
      case ControllerTypeSerial:
         rc = SERIALReadData(iIndex, pchResponse, cbResponse, pulBytesRead);
         break;
#endif
#ifdef DMC_ETHERNET
      case ControllerTypeEthernet:
         rc = ETHERNETReadData(iIndex, pchResponse, cbResponse, pulBytesRead);
         break;
#endif
   }

#ifdef DMC_DEBUG
   DMCTrace("   Bytes read <%ld>.\n", *pulBytesRead);
   DMCTrace("Leaving ReadData.\n");
#endif   
   return rc;
}

int CharAvailableInput(int iIndex)
{
   switch (controller[iIndex].controllerinfo.fControllerType)
   {
      default:
         return 0;      
#ifdef DMC_ISABUS
      case ControllerTypeISABus:
         return ISABUSCharAvailableInput(iIndex);
#endif
#ifdef DMC_PCIBUS
      case ControllerTypePCIBus:
         return PCIBUSCharAvailableInput(iIndex);
#endif
#ifdef DMC_SERIAL
      case ControllerTypeSerial:
         return SERIALCharAvailableInput(iIndex);
#endif
#ifdef DMC_ETHERNET
      case ControllerTypeEthernet:
         return ETHERNETCharAvailableInput(iIndex);
#endif
   }
}

void /* __cdecl*/ DMCTrace(char* pFormat, ...)
{
	va_list  pArgList;
	FILE*    fileTrace;
	
	if (!szFileTrace[0])
		return;
	
	fileTrace = fopen(szFileTrace, "a");
	if (fileTrace)
	{
		va_start(pArgList, pFormat);
		vfprintf(fileTrace, pFormat, pArgList);
		va_end(pArgList);
		fflush(fileTrace);
		fclose(fileTrace);
	}
}

//
//	Purpose	:	Set main index to controler
//	
//	Returns	:	zero based index to our device else -1 on error
//
int AddIndex(	PCONTROLLERINFO pcontrollerinfo	)
{
	int   i;
	
	for (i = 0; i < MAX_CONTROLLERS; i++)
	{
		if (!controller[i].fInUse)
	  {
	  	controller[i].fInUse = 1;
	    controller[i].fBinaryCommand = 0;
	    controller[i].controllerinfo.cbSize = pcontrollerinfo->cbSize;
	    controller[i].controllerinfo.usModelID = pcontrollerinfo->usModelID;
	    controller[i].controllerinfo.fControllerType = pcontrollerinfo->fControllerType;
	    controller[i].controllerinfo.ulTimeout = pcontrollerinfo->ulTimeout;
	    controller[i].controllerinfo.ulDelay = pcontrollerinfo->ulDelay;
	    controller[i].controllerinfo.pid = pcontrollerinfo->pid;
	     
	    switch (controller[i].controllerinfo.fControllerType)
	    {
	      case ControllerTypeISABus:
	         memcpy(&(controller[i].controllerinfo.hardwareinfo.businfo),
	            &(pcontrollerinfo->hardwareinfo.businfo), sizeof(BUSINFO));
	         break;
	      case ControllerTypePCIBus:
	         memcpy(&(controller[i].controllerinfo.hardwareinfo.businfo),
	            &(pcontrollerinfo->hardwareinfo.businfo), sizeof(BUSINFO));
	         break;
	      case ControllerTypeSerial:
	         memcpy(&(controller[i].controllerinfo.hardwareinfo.serialinfo),
	            &(pcontrollerinfo->hardwareinfo.serialinfo), sizeof(SERIALINFO));
	         break;
	      case ControllerTypeEthernet:
	         memcpy(&(controller[i].controllerinfo.hardwareinfo.socketinfo),
	            &(pcontrollerinfo->hardwareinfo.socketinfo), sizeof(SOCKETINFO));
	         break;
	    }
	     
			if (controller[i].controllerinfo.fControllerType == ControllerTypeISABus)
			{         
			  switch (controller[i].controllerinfo.usModelID)
			  {
			     default:
			        controller[i].controllerinfo.hardwareinfo.businfo.fIOStyle = DMC1000IOStyle;
			        controller[i].controllerinfo.hardwareinfo.businfo.fInterruptStyle = DMC1000InterruptStyle;
			        break;
			     case MODEL_600:
			        controller[i].controllerinfo.hardwareinfo.businfo.fIOStyle = DMC600IOStyle;
			        controller[i].controllerinfo.hardwareinfo.businfo.fInterruptStyle = DMCNoInterruptStyle;
			        break;
			     case MODEL_1000:
			        controller[i].controllerinfo.hardwareinfo.businfo.fIOStyle = DMC1000IOStyle;
			        controller[i].controllerinfo.hardwareinfo.businfo.fInterruptStyle = DMC1000InterruptStyle;
			        break;
			     case MODEL_1410:
			     case MODEL_1411:
			        controller[i].controllerinfo.hardwareinfo.businfo.fIOStyle = DMC600IOStyle;
			        controller[i].controllerinfo.hardwareinfo.businfo.fInterruptStyle = DMC1400InterruptStyle;
			        break;
			     case MODEL_1200:
			     case MODEL_1700:
			        controller[i].controllerinfo.hardwareinfo.businfo.fIOStyle = DMC1000IOStyle;
			        controller[i].controllerinfo.hardwareinfo.businfo.fInterruptStyle = DMC1700InterruptStyle;
			        break;
		  }	}
	     
			if (controller[i].controllerinfo.fControllerType == ControllerTypePCIBus)
			{         
			  switch (controller[i].controllerinfo.usModelID)
			  {
			     default:
			        controller[i].controllerinfo.hardwareinfo.businfo.fIOStyle = DMC1600IOStyle;
			        controller[i].controllerinfo.hardwareinfo.businfo.fInterruptStyle = DMC1600InterruptStyle;
			        break;
			     case MODEL_1600:
			     case MODEL_1800:
			        controller[i].controllerinfo.hardwareinfo.businfo.fIOStyle = DMC1600IOStyle;
			        controller[i].controllerinfo.hardwareinfo.businfo.fInterruptStyle = DMC1600InterruptStyle;
			        break;
			     case MODEL_1802:
			        controller[i].controllerinfo.hardwareinfo.businfo.fIOStyle = DMC1000IOStyle;
			        controller[i].controllerinfo.hardwareinfo.businfo.fInterruptStyle = DMC1700InterruptStyle;
			        break;
		  }	}
	     
	    return i;
	}	}
	
	return -1;
}

//
//	Purpose	:	remove main index to controler
//	
//	Returns	:	zero based index to our device else -1 on error
//
int DeleteIndex(int iIndex)
{
   if ((iIndex >= 0) && (iIndex < MAX_CONTROLLERS))
   {
      if (controller[iIndex].fInUse)
         memset(&(controller[iIndex]), '\0', sizeof(CONTROLLERINFO));
      return 0;
   }
   return -1;
}

//
//	Purpose	:	Retrieve index based upon handle
//	
//	Returns	:	zero based index to our device else -1 on error
//
int Handle2Index(HANDLEDMC hdmc)
{
	int i = (int)(hdmc - 1L);

  if ((i >= 0) && (i < MAX_CONTROLLERS))
  {
  	if (controller[i].fInUse)
    	return i;
  }

  return -1;
}

//
//	Purpose	:	Set main index to controler
//	
//	Returns	:	zero based index to our device else -1 on error
//
HANDLEDMC Index2Handle(int iIndex)
{
	return (HANDLEDMC)(iIndex + 1);
}

#ifdef __BORLANDC__
#define Timeb   timeb
#define BTime   biostime
#define Ftime   ftime
#else
#define Timeb   _timeb
#define BTime   _biostime
#define Ftime   _ftime
#endif

/* Return the time of the day in milliseconds */
ULONG DMCGetTime(void)
{
	unsigned long ulTemp=0;
	int nRetCode=0;
	struct timeval tv;
	struct timezone tz;
	gettimeofday(&tv, &tz);
	ulTemp=( 1000*( tv.tv_sec - (tv.tv_sec/10000*10000) ) + tv.tv_usec/1000);
	/*while (ulTemp==0)
	{
		printf("ulTemp was zero.\n");
		nRetCode=gettimeofday(&tv, &tz);
		if (nRetCode)
			nRetCode = errno;
		ulTemp=( 1000*( tv.tv_sec - (tv.tv_sec/10000*10000) ) + tv.tv_usec/1000);
	}
	printf("ulTemp <%i> tv.tv_sec<%i>.\n", ulTemp, tv.tv_sec);
	*/
	return  ulTemp; // Just take the first 10k sec of epoch.
}

#if defined(DMC_ISABUS) || defined(DMC_PCIBUS)

//
//	Purpose	:	Validates the device by SN if specified else defaults to relative indexing
//
//	Returns	:	TRUE success
//						FALSE	failure
//
static int ValidateISADeviceSN( int iIndex, PCONTROLLERINFO pcontrollerinfo )
{
  char szDevice[ 32 ];
  
	sprintf( szDevice, "/dev/galilisa%d", controller[ iIndex ].controllerinfo.hardwareinfo.businfo.usDevice );
	
  controller[iIndex].iDriver = open( szDevice, O_RDWR | O_NONBLOCK );
  
  if (controller[iIndex].iDriver == -1)
  {
		#ifdef DMC_DEBUG
		char s[ 255 ];
		sprintf( s, strerror(  errno ) );
  	DMCTrace("   Could not open device driver. %s\n",  strerror(  errno ) );
		#endif
  	return DMCERROR_DRIVER;
  }

  return 0L;
}

//
//	Purpose	:	Validates the device by SN if specified else defaults to relative indexing
//
//	Returns	:	TRUE success
//						FALSE	failure
//
static int ValidatePCIDeviceSN( int iIndex, PCONTROLLERINFO pcontrollerinfo )
{
  char 	szDevice[ 32 ];
  int		idx;

  if ( 0>=pcontrollerinfo->ulSerialNumber )
  {
		sprintf( szDevice, "/dev/galilpci%d", pcontrollerinfo->ulRelativeNumber );
		
	  controller[iIndex].iDriver = open( szDevice, O_RDWR | O_NONBLOCK );
	  
	  if ( -1 == controller[iIndex].iDriver )
	  {
			#ifdef DMC_DEBUG
			char s[ 255 ];
			sprintf( s, strerror(  errno ) );
	  	DMCTrace("   Could not open device driver. %s\n",  strerror(  errno ) );
			#endif
	  	return DMCERROR_DRIVER;
	  }
	  return 0l;
  }	
	else
	for( idx=0; idx<MAX_DEVICES; idx++ )
	{  
		sprintf( szDevice, "/dev/galilpci%d", idx );
		
	  controller[iIndex].iDriver = open( szDevice, O_RDWR | O_NONBLOCK );
	  
	  if ( -1 == controller[iIndex].iDriver )
	  {
			#ifdef DMC_DEBUG
	  	DMCTrace("   Could not open device driver. %s\n",  strerror(  errno ) );
			#endif
	  	return DMCERROR_DRIVER;
	  }
	  else
	  {
			if ( !ioctl( controller[ iIndex ].iDriver, GALIL_PCI_VALIDATESN, &pcontrollerinfo->ulSerialNumber ) )
			{
	      close( controller[ iIndex ].iDriver);
			}
			else	return 0l;	// we found the correct serial number
	}	}
  
	return DMCERROR_DRIVER;
}

//
//	Purpose	:	Common bus functions
//
long OpenDriver( int iIndex, PCONTROLLERINFO pcontrollerinfo )
{
	switch( pcontrollerinfo->fControllerType )
	{
		case ControllerTypeISABus:	
			return ValidateISADeviceSN( iIndex, pcontrollerinfo );
		case ControllerTypePCIBus:	
			return ValidatePCIDeviceSN( iIndex, pcontrollerinfo );
	}

 	return DMCERROR_DRIVER;
}

//
//	Purpose	:	close driver
//	
//	Returns	:	zero
//
long CloseDriver(int iIndex)
{
   if (controller[iIndex].iDriver > 0)   
      close(controller[iIndex].iDriver);
      
   controller[iIndex].iDriver = 0;

   return 0L;
}

#endif // DMC_ISABUS || DMC_PCIBUS

