#include "dmclnx.h"
#include "../PCIDriver/pciioctl.h"  //for DATA_RECORD_SIZE
 
/* Global variables */
CHAR szFileTrace[MAX_PATH];
CONTROLLER controller[MAX_CONTROLLERS];

LONG FAR GALILCALL DMCInitLibrary()
{
   // Initialize all global variables 
   memset(szFileTrace, '\0', sizeof(szFileTrace));
   memset(controller, '\0', sizeof(controller));

   return 0L;
}

//
// Purpose 	:	Main entry point for Galil controled open
//
LONG FAR GALILCALL DMCOpen(PCONTROLLERINFO pcontrollerinfo, PHANDLEDMC phdmc)
{
	LONG  rc = 0L;
  	int   iIndex;
	CHAR szTempCommand[COMMAND_BUFFER_SIZE];
	CHAR szTempResponse[RESPONSE_BUFFER_SIZE];
	
	szTempCommand[0] = '\0';
	szTempResponse[0] = '\0';
  

#ifdef DMC_DEBUG
  DMCTrace("Entering DMCOpen.\n");
#endif   

  // Check arguments
  if (!pcontrollerinfo || !phdmc)
  {
#ifdef DMC_DEBUG  
  	DMCTrace("   One or more arguments is NULL or 0.\n");
    DMCTrace("Leaving DMCOpen.\n");
#endif   
    return DMCERROR_ARGUMENT;  
  }
   
  // check struct size
  if (pcontrollerinfo->cbSize != sizeof(CONTROLLERINFO))
  {
#ifdef DMC_DEBUG
  	DMCTrace("   The CONTROLLERINFO structure is invalid.\n");
    DMCTrace("Leaving DMCOpen.\n");
#endif   
    return DMCERROR_ARGUMENT;  
  }
   
  *phdmc = -1;
   
  iIndex = AddIndex(pcontrollerinfo);
  if (iIndex == -1)
  {
#ifdef DMC_DEBUG  
  	DMCTrace("   All available controller handles are in use.\n");
    DMCTrace("Leaving DMCOpen.\n");     
#endif      
   	return DMCERROR_CONTROLLER;
  }

  if (iIndex == DMCERROR_MEMORY)
  {
#ifdef DMC_DEBUG  
  	DMCTrace("   Out of memory.\n");
    DMCTrace("Leaving DMCOpen.\n");     
    return DMCERROR_MEMORY;
#endif      
  }

#ifdef DMC_DEBUG  
  DMCTrace("   Added index <%d>.\n", iIndex);
#endif   

  // Create a controller handle
  *phdmc = Index2Handle(iIndex);
#ifdef DMC_DEBUG  
  DMCTrace("   Generated controller handle <%d>.\n", *phdmc); 
#endif

	switch ( pcontrollerinfo->fControllerType )
	{
		case ControllerTypeISABus:
  	case ControllerTypePCIBus:
	  	rc = OpenDriver( iIndex, pcontrollerinfo );
 	   	if (rc)
 	   	{
#ifdef DMC_DEBUG  
  	 		DMCTrace("   Could not open device driver.\n");
      	DMCTrace("Leaving DMCOpen.\n");     
#endif
      	return rc;
  		}	break;
		
		case ControllerTypeSerial:
	  	rc = OpenPort(iIndex);
  	  if (rc)
   	 	{
#ifdef DMC_DEBUG  
    		DMCTrace("   Could not open serial port.\n");
      	DMCTrace("Leaving DMCOpen.\n");     
#endif
      	return rc;
  		}	break;

		case ControllerTypeEthernet:
	  	rc = OpenSocket(iIndex);
	    if (rc)
	    {
#ifdef DMC_DEBUG  
	    	DMCTrace("   Could not open socket.\n");
	      DMCTrace("Leaving DMCOpen.\n");     
#endif
	      return rc;
		}	}
		
   // Check to see if we are in the monitor - the controller's OS
   {
      int   i;
      ULONG ulCurrentTime;
      ULONG ulTimeout;
      ULONG ulBytes;

      // Empty the FIFO
      rc = ReadData(iIndex, szTempResponse, sizeof(szTempResponse), &ulBytes);

			//if ( rc )
      for (i = 0; i < 5; i++)
      {
         rc = WriteData(iIndex, ";", 1, &ulBytes);

         ulCurrentTime = DMCGetTime();
         ulTimeout = ulCurrentTime + 1000;

         do
         {
            szTempResponse[0] = '\0';
            ulCurrentTime = DMCGetTime();
            if (ulCurrentTime > ulTimeout)
               rc = DMCERROR_TIMEOUT;
            else
               rc = ReadData(iIndex, szTempResponse, sizeof(szTempResponse), &ulBytes);
            if (szTempResponse[0] == '\r')
               strcpy(szTempResponse, &szTempResponse[1]);
            if (szTempResponse[0] == '\n')
               strcpy(szTempResponse, &szTempResponse[1]);
         } while ((szTempResponse[0] != ':' && szTempResponse[0] != '?' && szTempResponse[0] != '>') && (rc == 0));

         /* If a ? is returned, do it one more time */
         if (szTempResponse[0] != '?')
            break;
      }

      if (szTempResponse[0] == '>')
      {
#ifdef DMC_DEBUG     
         DMCTrace("   Warning: The controller is in the monitor.\n");
         DMCTrace("Leaving DMCOpen.\n");
#endif         
         return DMCWARNING_MONITOR;
      }
   }

#ifdef DMC_DATARECORDACCESS
   // Check the Data Record Access data record size
   if (	controller[ iIndex ].controllerinfo.fControllerType == ControllerTypeISABus ||
      	controller[ iIndex ].controllerinfo.fControllerType == ControllerTypePCIBus	)
   {
   		switch( controller[ iIndex ].controllerinfo.usModelID )
   		{
   			case MODEL_1600:
   			case MODEL_1800:	
   				controller[ iIndex ].controllerinfo.hardwareinfo.businfo.fDataRecordAccess = DataRecordAccessFIFO;
   		}

      if (controller[iIndex].controllerinfo.hardwareinfo.businfo.fDataRecordAccess)
      {
         rc = DMCCommand(*phdmc, "QZ\r", szTempResponse, sizeof(szTempResponse));
         if (rc == DMCNOERROR)
         {
            int   iAxes;
            char* pch;
   
            /* Get the number of axes */
            pch = strtok(szTempResponse, ",");
            if (pch)
               iAxes = atoi(pch);
            /* Get the size of the general offsets */
            pch = strtok(NULL, ",");
            if (pch)
               controller[iIndex].usMaxGeneralOffset = (USHORT)atoi(pch);
            pch = strtok(NULL, ",");
            if (pch)
               controller[iIndex].usMaxGeneralOffset += (USHORT)atoi(pch);
            /* Get the size of the axis offsets */
            pch = strtok(NULL, ",");
            if (pch)
               controller[iIndex].usMaxAxisOffset = (USHORT)atoi(pch);
            controller[iIndex].usRevision = controller[iIndex].usMaxAxisOffset + controller[iIndex].usMaxGeneralOffset;
            controller[iIndex].usMaxGeneralOffset += (controller[iIndex].usMaxAxisOffset * iAxes);
            /* Check Data Record record size */
            if (controller[iIndex].usDataRecordSize == 0)
            {
               controller[iIndex].usDataRecordSize = controller[iIndex].usMaxGeneralOffset;
               /* Check data record buffers */
               controller[iIndex].pbDataRecord = (PBYTE)calloc(1, DATA_RECORD_SIZE); //controller[iIndex].usDataRecordSize);
               if (!controller[iIndex].pbDataRecord)
                  rc = DMCERROR_MEMORY;
               controller[iIndex].pTemp1 = (PBYTE)calloc(1, DATA_RECORD_SIZE); //controller[iIndex].usDataRecordSize);
               if (!controller[iIndex].pTemp1)
                  rc = DMCERROR_MEMORY;
               controller[iIndex].pTemp2 = (PBYTE)calloc(1, DATA_RECORD_SIZE); //controller[iIndex].usDataRecordSize);
               if (!controller[iIndex].pTemp2)
                  rc = DMCERROR_MEMORY;
               if (rc == DMCERROR_MEMORY)             
               {
                  controller[iIndex].controllerinfo.hardwareinfo.businfo.fDataRecordAccess = DataRecordAccessNone;
#ifdef DMC_DEBUG              
                  DMCTrace("   Could not turn on Data Record Access. Could not allocate memory for data record buffers.\n", rc);
#endif               
               }           
            }           
         }
         else
         {
            controller[iIndex].controllerinfo.hardwareinfo.businfo.fDataRecordAccess = DataRecordAccessNone;
#ifdef DMC_DEBUG        
            DMCTrace("   Could not turn on Data Record Access. Error returned from QZ command.\n", rc);
#endif         
         }
   
#ifdef DMC_DMA   
         /* Start the DMA hardware if being used */
         if (controller[iIndex].controllerinfo.hardwareinfo.businfo.fDataRecordAccess == DataRecordAccessDMA &&
            controller[iIndex].controllerinfo.hardwareinfo.businfo.usDMAChannel < 4)
         {
            InitDMA(iIndex);
            if (controller[iIndex].controllerinfo.hardwareinfo.businfo.usRefreshRate == 0)
               controller[iIndex].controllerinfo.hardwareinfo.businfo.usRefreshRate = 4;
            sprintf(szTempCommand, "DR%d\r", controller[iIndex].controllerinfo.hardwareinfo.businfo.usRefreshRate);
            rc = DMCCommand(*phdmc, szTempCommand, szTempResponse, sizeof(szTempResponse));
            if (rc != DMCNOERROR)
            {
#ifdef DMC_DEBUG     
               DMCTrace("   Could not turn on Data Record Access. Controller returns <%ld>.\n", rc);
#endif         
               rc = 0L;
            }
         }
#endif /* DMC_DMA */
   
         if (controller[iIndex].controllerinfo.hardwareinfo.businfo.fDataRecordAccess == DataRecordAccessFIFO) 
         {
            if (controller[iIndex].controllerinfo.hardwareinfo.businfo.usRefreshRate == 0)
               controller[iIndex].controllerinfo.hardwareinfo.businfo.usRefreshRate = 4;
            sprintf(szTempCommand, "DR-%d\r", controller[iIndex].controllerinfo.hardwareinfo.businfo.usRefreshRate);
            rc = DMCCommand(*phdmc, szTempCommand, szTempResponse, sizeof(szTempResponse));
            if (rc != DMCNOERROR)
            {
#ifdef DMC_DEBUG
               DMCTrace("   Could not turn on Data Record Access. Controller returns <%ld>.\n", rc);
#endif
               rc = 0L;
            }
         }
      }
   }
#endif /* DMC_DATARECORDACCESS */

   /* Reset the FIFO (this will also check communications) */
   rc = DMCClear(*phdmc);

   /* Clear any remaining characters */
   if (CharAvailableInput(iIndex))
   {
      ULONG ulBytesRead;
      ReadData(iIndex, szTempResponse, sizeof(szTempResponse), &ulBytesRead);
      memset(controller[iIndex].szResponse, '\0', sizeof(controller[iIndex].szResponse));
      memset(controller[iIndex].szUnsolicitedResponse, '\0', sizeof(controller[iIndex].szUnsolicitedResponse));
   }  

   /* Turn echo off */
   DMCCommand(*phdmc, "EO0\r", szTempResponse, sizeof(szTempResponse));

#ifdef DMC_DEBUG  
   DMCTrace("Leaving DMCOpen.\n");
#endif   
   return rc;
}

LONG FAR GALILCALL DMCClose(HANDLEDMC hdmc)
{
   LONG  rc = 0L;
   int   iIndex;

#ifdef DMC_DEBUG  
   DMCTrace("Entering DMCClose.\n");
#endif

   iIndex = Handle2Index(hdmc);
   if (iIndex == -1)
   {
#ifdef DMC_DEBUG  
      DMCTrace("   Invalid handle.\n");
      DMCTrace("Leaving DMCClose.\n");
#endif      
      return DMCERROR_HANDLE;
   }

#ifdef DMC_DATARECORDACCESS
   if (controller[iIndex].controllerinfo.fControllerType == ControllerTypeISABus ||
      controller[iIndex].controllerinfo.fControllerType == ControllerTypePCIBus)
   {
      if (controller[iIndex].controllerinfo.hardwareinfo.businfo.fDataRecordAccess)
      {
         ULONG ulBytesWritten;
         //WriteData(iIndex, "DR0\r", 4, &ulBytesWritten);
   
#ifdef DMC_DMA
         if (controller[iIndex].controllerinfo.hardwareinfo.businfo.fDataRecordAccess == DataRecordAccessDMA)
            UninitDMA(iIndex);
#endif /* DMC_DMA */

         if (controller[iIndex].pbDataRecord)
            free(controller[iIndex].pbDataRecord);
         controller[iIndex].pbDataRecord = NULL;
         if (controller[iIndex].pTemp1)
            free(controller[iIndex].pTemp1);
         controller[iIndex].pTemp1 = NULL;
         if (controller[iIndex].pTemp2)
            free(controller[iIndex].pTemp2);
         controller[iIndex].pTemp2 = NULL;
      }
   }  
#endif /* DMC_DATARECORDACCESS */

#if defined(DMC_ISABUS) || defined(DMC_PCIBUS)
   if (controller[iIndex].controllerinfo.fControllerType == ControllerTypeISABus ||
      controller[iIndex].controllerinfo.fControllerType == ControllerTypePCIBus)
      CloseDriver(iIndex);
#endif /* DMC_ISABUS || DMC_PCIBUS */

#ifdef DMC_SERIAL
   if (controller[iIndex].controllerinfo.fControllerType == ControllerTypeSerial)
      ClosePort(iIndex);
#endif /* DMC_SERIAL */   

#ifdef DMC_ETHERNET
   if (controller[iIndex].controllerinfo.fControllerType == ControllerTypeEthernet)
      CloseSocket(iIndex);
#endif /* DMC_ETHERNET */   
   
   DeleteIndex(iIndex);
#ifdef DMC_DEBUG  
   DMCTrace("   Deleted index <%d>.\n", iIndex);   
#endif

#ifdef DMC_DEBUG  
   DMCTrace("Leaving DMCClose.\n");
#endif   
   return rc;
}
/*
LONG FAR GALILCALL DMCEnableInterrupts(HANDLEDMC hdmc, pid_t pid)
{
   LONG  rc = 0L;
   int   iIndex;

#ifdef DMC_DEBUG
   DMCTrace("Entering DMCEnableInterrupts.\n");
#endif   

   iIndex = Handle2Index(hdmc);
   if (iIndex == -1)
   {
#ifdef DMC_DEBUG
      DMCTrace("   Invalid handle.\n");
      DMCTrace("Leaving DMCEnableInterrupts.\n");
#endif      
      return DMCERROR_HANDLE;
   }

   controller[iIndex].controllerinfo.pid = pid;

#ifdef DMC_DEBUG
   DMCTrace("Leaving DMCEnableInterrupts.\n");
#endif 

   return rc;
}

LONG FAR GALILCALL DMCDisableInterrupts(HANDLEDMC hdmc)
{
   LONG  rc = 0L;
   int   iIndex;

#ifdef DMC_DEBUG
   DMCTrace("Entering DMCDisableInterrupts.\n");
#endif

   iIndex = Handle2Index(hdmc);
   if (iIndex == -1)
   {
#ifdef DMC_DEBUG
      DMCTrace("   Invalid handle.\n");
      DMCTrace("Leaving DMCDisableInterrupts.\n");
#endif      
      return DMCERROR_HANDLE;
   }

#ifdef DMC_DEBUG
      DMCTrace("Leaving DMCDisableInterrupts.\n");
#endif 

   return rc;
}
*/
//
//
//
LONG FAR GALILCALL DMCCommand(HANDLEDMC hdmc, PSZ pszCommand, PCHAR pchResponse,ULONG cbResponse)
{
	LONG  rc = 0L;
	LONG  saverc = 0L;
	int   iIndex;
	ULONG ulCurrentTime;
	ULONG ulTimeout;
	ULONG ulBytesRead;
	ULONG ulBytesWritten;
	int   iCommands = 0;
	int   iResponses = 0;
	char* pch;
	char* pchTempResponse = NULL;
	CHAR szTempCommand[COMMAND_BUFFER_SIZE];
	CHAR szTempResponse[RESPONSE_BUFFER_SIZE];
	
	szTempCommand[0] = '\0';
	szTempResponse[0] = '\0';
	
#ifdef DMC_DEBUG
	DMCTrace("Entering DMCCommand.\n");
#endif   
	
	iIndex = Handle2Index(hdmc);
	if (iIndex == -1)
	{
#ifdef DMC_DEBUG
	  DMCTrace("   Invalid handle.\n");
	  DMCTrace("Leaving DMCCommand.\n");
#endif      
	  return DMCERROR_HANDLE;
	}
	
	controller[iIndex].fBinaryCommand = 0;
#ifdef DMC_ETHERNET
	controller[iIndex].fReadMulticast = 0;
#endif
	
	// NOTE: the length of pszCommand must be less than COMMAND_BUFFER_SIZE - 2
	
	// Create a default command if none exists
	if (pszCommand && *pszCommand)
	{
	  strncpy(szTempCommand, pszCommand, sizeof(szTempCommand) - 2);
	  if (szTempCommand[strlen(szTempCommand)-1] != '\r' && szTempCommand[strlen(szTempCommand)-1] != ';')
	     strcat(szTempCommand, "\r");
	}
	else
	  strcpy(szTempCommand, "\r");
	
#ifdef DMC_DEBUG
	DMCTrace("   Command <%s>.\n", szTempCommand);
#endif
	
	// Use a temporary response buffer if none exists
	if (pchResponse && cbResponse)
	  pchTempResponse = pchResponse;
	else
	{
	  pchTempResponse = szTempResponse;
	  cbResponse = sizeof(szTempResponse);
	}
	
	// Clear any remaining characters from previous command - note that any unsolicited responses
	// will be buffered in szUnsolicitedResponse
	if (CharAvailableInput(iIndex))
	  ReadData(iIndex, szTempResponse, sizeof(szTempResponse), &ulBytesRead);
	
	*pchTempResponse = '\0';
	
	// Count how many commands are to be sent
	for (pch = strchr(szTempCommand, ';'); pch; pch = strchr(pch, ';'))
	{
	  iCommands++;
	  pch++;
	}
	
	for (pch = strchr(szTempCommand, '\r'); pch; pch = strchr(pch, '\r'))
	{
	  iCommands++;
	  pch++;
	}
	
	// The controller does not send a response for the DL command; also, the user may have input a
	//  pure number in response to the IN command
	if (!strncmp(szTempCommand, "DL", 2) 	|| 
			!strncmp(szTempCommand, "UL", 2) 	|| 
			!strncmp(szTempCommand, "LS", 2) 	|| 
			isdigit(*szTempCommand) 					|| 
			*szTempCommand == '-')
	  iCommands--;
	
	rc = WriteData(iIndex, szTempCommand, strlen(szTempCommand), &ulBytesWritten);
	if (rc || ulBytesWritten == 0)
	{
#ifdef DMC_DEBUG
	  DMCTrace("  A timeout ocurred waiting for the controller to recieve the command.\n");
	  DMCTrace("Leaving DMCCommand.\n");
#endif
	  return rc;
	}
	
	rc = 0L;
	
	memset(controller[iIndex].szResponse, '\0',  sizeof(controller[iIndex].szResponse));
	
	if (controller[iIndex].controllerinfo.ulTimeout)
	{
	  ulCurrentTime = DMCGetTime();
	  ulTimeout = ulCurrentTime + controller[iIndex].controllerinfo.ulTimeout;
	}
	
	// Wait until a response is received or a timeout occurs
	while (!CharAvailableInput(iIndex) && !rc)
	{
	  // If time-out is 0, ignore time-out errors
	  if (controller[iIndex].controllerinfo.ulTimeout)
	  {
	     ulCurrentTime = DMCGetTime();
		 //printf("Response? ulCurrentTime<%i> ulTimeout<%i>.\n", ulCurrentTime, ulTimeout);
	     if (ulCurrentTime > ulTimeout)
	        rc = DMCERROR_TIMEOUT;
	  }
	  else
	     break;            
	}
	
	if (rc)
	{
#ifdef DMC_DEBUG  
	  DMCTrace("  A timeout ocurred waiting for a response from the controller.\n");
	  DMCTrace("Leaving DMCCommand.\n");
#endif      
	  return rc;
	}
	
	if (controller[iIndex].controllerinfo.ulTimeout)
	{
	  ulCurrentTime = DMCGetTime();
	  ulTimeout = ulCurrentTime + controller[iIndex].controllerinfo.ulTimeout;
	}     
	
	// Delay the read
	/*
	if (controller[iIndex].controllerinfo.ulDelay)
	  sleep(controller[iIndex].controllerinfo.ulDelay / 1000);
	*/
	
	// Receive the response
	do
	{
	  usleep(1);
	  rc = ReadData(iIndex, pchResponse, cbResponse, &ulBytesRead);
	  // If this is a DMC-1400, the UI command must be treated special
	  if ((controller[iIndex].controllerinfo.hardwareinfo.businfo.fInterruptStyle == DMC1400InterruptStyle) &&
	     (strstr(szTempCommand, "UI")))
	  {
	     strncpy(pchResponse, controller[iIndex].szResponse, (size_t)cbResponse);
	     memset(controller[iIndex].szResponse, '\0',  sizeof(controller[iIndex].szResponse));
#ifdef DMC_DEBUG        
	     DMCTrace("Leaving DMCCommand.\n");
#endif
	     return 0;
	  }     
	  
	  // Count how many responses were received
	  if (strncmp(szTempCommand, "UL", 2))
	  {
	  	iResponses = 0;
	    if (strncmp(szTempCommand, "LS", 2))
	    {
	       for (pch = strchr(pchResponse, ':'); pch; pch = strchr(pch, ':'))
	       {
	    	   iResponses++;
	         pch++;
	       }
	       for (pch = strchr(pchResponse, '?'); pch; pch = strchr(pch, '?'))
	       {
	         iResponses++;
	         pch++;
	       }
	       if (iResponses >= iCommands)
	      	 break;
	  }	 }
	  else
	  {
	    if (strchr(pchResponse, 0x1A))
	    	break;
	  }        
	
	  if (rc)
	     saverc = rc;
	     
	  if (rc == 0 || rc == DMCERROR_BUFFERFULL)
	  {
	     if (!ulBytesRead)
	     {
	        // Do not wait for a response because time-out is zero
	        if (!controller[iIndex].controllerinfo.ulTimeout)
	        {
	           saverc = 0;
	           rc = 0;
	           break;
	        }
	        // Check clock
	        ulCurrentTime = DMCGetTime();
			//printf("Receive? ulCurrentTime<%i> ulTimeout<%i>.\n", ulCurrentTime, ulTimeout);
	        if (ulCurrentTime > ulTimeout)
	           rc = DMCERROR_TIMEOUT;
	     }
	     else
	     {
	        // Reset clock
	        if (controller[iIndex].controllerinfo.ulTimeout)
	        {
	           ulCurrentTime = DMCGetTime();
	           ulTimeout = ulCurrentTime + controller[iIndex].controllerinfo.ulTimeout;
		}	}	}
	} while ((rc == 0 || rc == DMCERROR_BUFFERFULL));
	
	if (saverc)
	  rc = saverc;
	            
	// Do no report time-out errors on program listing
	if (!strncmp(szTempCommand, "LS", 2))
	{
	  if (rc == DMCERROR_TIMEOUT)
	     rc = 0L;
	}
	
	// User wants to ignore time-out errors
	if (rc == DMCERROR_TIMEOUT && !controller[iIndex].controllerinfo.ulTimeout)
	  rc = 0L;
	
	if (pchResponse && *pchResponse)
	{
	  if (strchr(pchResponse, '?'))
	     rc = DMCERROR_COMMAND;
	}
	
#ifdef DMC_DEBUG
	DMCTrace("   Response <%s>.\n", pchResponse);
	DMCTrace("Leaving DMCCommand.\n");
#endif  
	return rc;
}

//
//
//
LONG FAR GALILCALL DMCGetUnsolicitedResponse(HANDLEDMC hdmc, PCHAR pchResponse, ULONG cbResponse)
{
   LONG  rc = 0L;
   int   iIndex;
   ULONG ulBytesRead = 0L;
   CHAR szTempResponse[RESPONSE_BUFFER_SIZE];
   
   szTempResponse[0] = '\0';

#ifdef DMC_DEBUG        
   DMCTrace("Entering DMCGetUnsolicitedResponse.\n");
#endif

   iIndex = Handle2Index(hdmc);
   if (iIndex == -1)
   {
      rc = DMCERROR_HANDLE;
#ifdef DMC_DEBUG        
      DMCTrace("   Invalid handle.\n");
      DMCTrace("Leaving DMCGetUnsolicitedResponse.\n");
#endif
      return rc;
   }

   if (!pchResponse || !cbResponse)
   {
#ifdef DMC_DEBUG        
      DMCTrace("   One or more arguments is NULL or 0.\n");
      DMCTrace("Leaving DMCGetUnsolicitedResponse.\n");
#endif      
      return DMCERROR_ARGUMENT;
   }

   /* See if more unsolicited response data is available */
   if (CharAvailableInput(iIndex))
      rc = ReadData(iIndex, szTempResponse, sizeof(szTempResponse), &ulBytesRead);

   strncpy(pchResponse, controller[iIndex].szUnsolicitedResponse, (size_t)(cbResponse - 1));

   if (cbResponse - 1 < strlen(controller[iIndex].szUnsolicitedResponse))
   {
      /* Shift the buffer */
      strcpy(controller[iIndex].szUnsolicitedResponse,
         &(controller[iIndex].szUnsolicitedResponse[(size_t)(cbResponse - 1)]));
   }
   else
   {
      /* Reset the buffer */
      memset(controller[iIndex].szUnsolicitedResponse, '\0',
         sizeof(controller[iIndex].szUnsolicitedResponse));
   }

#ifdef DMC_DEBUG
   DMCTrace("Leaving DMCGetUnsolicitedResponse.\n");
#endif
   return rc;
}

LONG FAR GALILCALL DMCReadData(HANDLEDMC hdmc, PCHAR pchBuffer,
   ULONG cbBuffer, PULONG pulBytesRead)
{
   LONG  rc = 0L;
   int   iIndex;

   if (!pchBuffer || !cbBuffer || !pulBytesRead)
      return DMCERROR_ARGUMENT;

   *pulBytesRead = 0;

   iIndex = Handle2Index(hdmc);
   if (iIndex == -1)
      return DMCERROR_HANDLE;

   rc = ReadData(iIndex, pchBuffer, cbBuffer, pulBytesRead);

#ifdef DMC_DEBUG
   if (*pulBytesRead && cbBuffer < RESPONSE_BUFFER_SIZE)
   {
      char  szTemp[RESPONSE_BUFFER_SIZE];
      strncpy(szTemp, pchBuffer, (size_t)(cbBuffer - 1));
      szTemp[(size_t)(cbBuffer - 1)] = '\0';
      DMCTrace("DMCReadData: Data read <%s>.\n", szTemp);
   }
   DMCTrace("DMCReadData: Bytes read <%ld>.\n", *pulBytesRead);
#endif

   return rc;
}

LONG FAR GALILCALL DMCWriteData(HANDLEDMC hdmc, PCHAR pchBuffer,
   ULONG cbBuffer, PULONG pulBytesWritten)
{
   LONG  rc = 0L;
   int   iIndex;

   if (!pchBuffer || !cbBuffer || !pulBytesWritten)
      return DMCERROR_ARGUMENT;

   *pulBytesWritten = 0;

   iIndex = Handle2Index(hdmc);
   if (iIndex == -1)
      return DMCERROR_HANDLE;

   rc = WriteData(iIndex, pchBuffer, cbBuffer, pulBytesWritten);

#ifdef DMC_DEBUG
   if (*pulBytesWritten && cbBuffer < COMMAND_BUFFER_SIZE)
   {
      char  szTemp[COMMAND_BUFFER_SIZE];
      strncpy(szTemp, pchBuffer, (size_t)(cbBuffer - 1));
      szTemp[(size_t)(cbBuffer - 1)] = '\0';
      DMCTrace("DMCWriteData: Data written <%s>.\n", szTemp);
   }
   DMCTrace("DMCWriteData: Bytes written <%ld>.\n", *pulBytesWritten);
#endif

   return rc;
}

LONG FAR GALILCALL DMCGetAdditionalResponse(HANDLEDMC hdmc, PCHAR pchResponse, ULONG cbResponse)
{
   LONG  rc = 0L;
   int   iIndex;
   ULONG ulBytesRead;

#ifdef DMC_DEBUG
   DMCTrace("Entering DMCGetAdditionalResponse.\n");
#endif

   iIndex = Handle2Index(hdmc);
   if (iIndex == -1)
   {
      rc = DMCERROR_HANDLE;
#ifdef DMC_DEBUG
      DMCTrace("   Invalid handle.\n");
      DMCTrace("Leaving DMCGetAdditionalResponse.\n");
#endif      
      return rc;
   }

   if (!pchResponse || !cbResponse)
   {
#ifdef DMC_DEBUG
      DMCTrace("   One or more arguments is NULL or 0.\n");
      DMCTrace("Leaving DMCGetAdditionalResponse.\n");
#endif      
      return DMCERROR_ARGUMENT;
   }

   strncpy(pchResponse, controller[iIndex].szResponse,
      (size_t)(cbResponse - 1));

   memset(controller[iIndex].szResponse, '\0',
      sizeof(controller[iIndex].szResponse));

   /* See if more response data is available */
   if (CharAvailableInput(iIndex))
      rc = ReadData(iIndex, pchResponse, cbResponse, &ulBytesRead);

#ifdef DMC_DEBUG
   DMCTrace("Leaving DMCGetAdditionalResponse.\n");
#endif   
   return rc;
}

LONG FAR GALILCALL DMCGetAdditionalResponseLen(HANDLEDMC hdmc, PULONG pulResponseLen)
{
   LONG  rc = 0L;
   int   iIndex;

#ifdef DMC_DEBUG
   DMCTrace("Entering DMCGetAdditionalResponseLen.\n");
#endif

   iIndex = Handle2Index(hdmc);
   if (iIndex == -1)
   {
#ifdef DMC_DEBUG
      DMCTrace("   Invalid handle.\n");
      DMCTrace("Leaving DMCGetAdditionalResponseLen.\n");
#endif      
      return DMCERROR_HANDLE;
   }

   if (!pulResponseLen)
   {
#ifdef DMC_DEBUG
      DMCTrace("   One or more arguments is NULL or 0.\n");
      DMCTrace("Leaving DMCGetAdditionalResponseLen.\n");
#endif      
      return DMCERROR_ARGUMENT;
   }

   *pulResponseLen = (ULONG)strlen(controller[iIndex].szResponse);

#ifdef DMC_DEBUG
   DMCTrace("Leaving DMCGetAdditionalResponseLen.\n");
#endif   
   return rc;
}

#ifdef DMC_INTERRUPTS

long AddFunc(long l1, long l2)
{
	return l1+l2;
}

LONG FAR GALILCALL DMCGetInterruptStatus(HANDLEDMC hdmc, PUSHORT pusStatus)
{
	
   long lPadding1=0; //STA: Compiler was overwriting hdmc with contents of rc.
   long lPadding2=3;
   lPadding1 = 2 + lPadding2;
   LONG lMorePadding = AddFunc(lPadding1, lPadding2);
   LONG  rc = 0L;
   int   iIndex;

#ifdef DMC_DEBUG
   DMCTrace("Entering DMCGetInterruptStatus.\n");
#endif

   iIndex = Handle2Index(hdmc);
   if (iIndex == -1)
   {
      rc = DMCERROR_HANDLE;
#ifdef DMC_DEBUG
      DMCTrace("   Invalid handle.\n");
      DMCTrace("Leaving DMCGetInterruptStatus.\n");
#endif      
      return rc;
   }

   switch (controller[iIndex].controllerinfo.fControllerType)
   {
      default:
         return 0;      
#ifdef DMC_ISABUS
      case ControllerTypeISABus:
#endif
#ifdef DMC_PCIBUS
      case ControllerTypePCIBus:
      	if (controller[iIndex].controllerinfo.usModelID == MODEL_1417)
      	{
      	 	char command[5], buffer[256] = "";
      	 	char* pch;
	  		sprintf(command, "IV11\r");
	  		rc = DMCCommand(hdmc, command, buffer, sizeof(buffer));
	  		pch = strchr(buffer, '\v');
      		*pusStatus = (unsigned short)atoi(pch);
      	}
      	else
         	rc = BUSGetInterruptStatus(iIndex, pusStatus);
         break;
#endif
   }

#ifdef DMC_DEBUG
   DMCTrace("Leavinging DMCGetInterruptStatus.\n");
#endif 
   return rc;
}
#endif /* DMC_INTERRUPTS */

LONG FAR GALILCALL DMCClear(HANDLEDMC hdmc)
{
   LONG  rc = 0L;
   int   iIndex;

#ifdef DMC_DEBUG
   DMCTrace("Entering DMCClear.\n");
#endif   

   iIndex = Handle2Index(hdmc);
   if (iIndex == -1)
   {
      rc = DMCERROR_HANDLE;
#ifdef DMC_DEBUG
      DMCTrace("   Invalid handle.\n");
      DMCTrace("Leaving DMCClear.\n");
#endif      
      return rc;
   }

   switch (controller[iIndex].controllerinfo.fControllerType)
   {
      default:
         return 0;      
#ifdef DMC_ISABUS
      case ControllerTypeISABus:
         rc = ISABUSClear(iIndex);
         break;
#endif /* DMC_ISABUS */
#ifdef DMC_PCIBUS
      case ControllerTypePCIBus:
         rc = PCIBUSClear(iIndex);
         break;
#endif /* DMC_PCIBUS */
#ifdef DMC_SERIAL
      case ControllerTypeSerial:
         rc = SERIALClear(iIndex);
         break;
#endif /* DMC_SERIAL */
#ifdef DMC_ETHERNET
      case ControllerTypeEthernet:
         rc = ETHERNETClear(iIndex);
         break;
#endif /* DMC_ETHERNET */
   }

   	// Make sure communications are OK
   	//{
  	//   int   i;
	 	//	
   	//   for (i = 0, rc = 1; rc != 0 && i < 5; i++)
   	//      rc = DMCCommand(hdmc, "\r", szTempResponse, sizeof(szTempResponse));
  	//}

#ifdef DMC_DEBUG
   DMCTrace("Leaving DMCClear.\n");
#endif   
   return rc;
}

LONG FAR GALILCALL DMCDownloadFile(HANDLEDMC hdmc, PSZ pszFileName, PSZ pszLabel)
{
   LONG  rc = 0L;
   int   iIndex;
   FILE* file;
   char  *pch;
   char  szLine[100];
   ULONG ulBytesRead;
   ULONG ulBytesWritten;
   LONG  ulTimeout;
   int   bSentEndOfDownload = FALSE;
   CHAR szTempCommand[COMMAND_BUFFER_SIZE];
   CHAR szTempResponse[RESPONSE_BUFFER_SIZE];
   
   szTempCommand[0] = '\0';
   szTempResponse[0] = '\0';

#ifdef DMC_DEBUG
   DMCTrace("Entering DMCDownloadFile.\n");
#endif   

   iIndex = Handle2Index(hdmc);
   if (iIndex == -1)
   {
#ifdef DMC_DEBUG
      DMCTrace("   Invalid handle.\n");
      DMCTrace("Leaving DMCDownloadFile.\n");
#endif      
      return DMCERROR_HANDLE;
   }

   if (!pszFileName)
   {
#ifdef DMC_DEBUG
      DMCTrace("   File name is NULL.\n");
      DMCTrace("Leaving DMCDownloadFile.\n");
#endif      
      return DMCERROR_ARGUMENT;
   }

   // Turn echo off
   rc = DMCCommand(hdmc, "EO0\r", szTempResponse, sizeof(szTempResponse));
   if (rc)
   {
#ifdef DMC_DEBUG
      DMCTrace("   Could not turn echo off.\n");
      DMCTrace("Entering DMCDownloadFile.\n");
#endif      
      return rc;
   }

#ifdef DMC_DEBUG
   DMCTrace("   File name <%s>.\n", pszFileName);
#endif

   if ((file = fopen(pszFileName, "r")) == NULL)
   {
#ifdef DMC_DEBUG
      DMCTrace("   File error.\n");
      DMCTrace("Leaving DMCDownloadFile.\n");
#endif      
      return DMCERROR_FILE;
   }

   // Send the download command
   strcpy(szTempCommand, "DL");
   if (pszLabel)
   {
      if (*pszLabel)
      {
#ifdef DMC_DEBUG
         DMCTrace("   Label <%s>.\n", pszLabel);
#endif         
         if (*pszLabel == '#')
            strncat(szTempCommand, pszLabel, sizeof(szTempCommand)-4);
         else
         {
            strcat(szTempCommand, "#");
            strncat(szTempCommand, pszLabel, sizeof(szTempCommand)-5);
         }
      }
   }
   strcat(szTempCommand, "\r");

   ulTimeout = controller[iIndex].controllerinfo.ulTimeout;
   controller[iIndex].controllerinfo.ulTimeout = 10;
   rc = DMCCommand(hdmc, szTempCommand, szTempResponse, sizeof(szTempResponse));
   controller[iIndex].controllerinfo.ulTimeout = ulTimeout;
   if (rc == DMCERROR_TIMEOUT)
      rc = 0L;        
   
   if (!rc)
   {
      if (strchr(szTempResponse, '?'))
      {
#ifdef DMC_DEBUG
         DMCTrace("   Download command (DL) failed.\n");
#endif         
         rc = DMCERROR_COMMAND;
      }
   }
   else
   {
#ifdef DMC_DEBUG
      DMCTrace("   Download command (DL) failed.\n");
#endif      
   }

   if (!rc)
   {
      while(fgets(szLine, sizeof(szLine), file))
      {
         if (!strncasecmp(szLine, "REM", 3) || szLine[0] == '\r' || szLine[0] == '\n')
            continue;

         pch = strchr(szLine, '\n');
         if (pch)
            *pch = '\0';

         pch = strchr(szLine, '\r');
         if (!pch)
            strcat(szLine, "\r");

         DMCTrace("   Line <%s>.\n", szLine);

         /* Send the line */
         rc = WriteData(iIndex, szLine, strlen(szLine), &ulBytesWritten);
         if (rc)
            break;
         if (ulBytesWritten == 0L)
         {
            rc = DMCERROR_TIMEOUT;
#ifdef DMC_DEBUG
            DMCTrace("  A timeout ocurred waiting for a response from the controller.\n");
#endif            
            break;
         }

         CharAvailableInput(iIndex);
            
         // Get the response if any
         if (CharAvailableInput(iIndex))
         {
            rc = ReadData(iIndex, szTempResponse, sizeof(szTempResponse), &ulBytesRead);
            if (strchr(szTempResponse, '?'))
            {
               rc = DMCERROR_DOWNLOAD;
               break;
  }	}	}	}

  // Get the response if any
  if (CharAvailableInput(iIndex))
  {
	  rc = ReadData(iIndex, szTempResponse, sizeof(szTempResponse), &ulBytesRead);
	  if (strchr(szTempResponse, '?'))
	    rc = DMCERROR_DOWNLOAD;
  }
   
   if (!rc)
   {
      bSentEndOfDownload = TRUE;
      rc = DMCCommand(hdmc, "\\", szTempResponse, sizeof(szTempResponse));
      if (!rc)
      {
         if (strchr(szTempResponse, '?'))
            rc = DMCERROR_DOWNLOAD;
      }
   }
   
   // Clear the error
   if (rc == DMCERROR_DOWNLOAD || rc == DMCERROR_TIMEOUT)
   {
      if (!bSentEndOfDownload)
         DMCCommand(hdmc, "\\", szTempResponse, sizeof(szTempResponse));         
      DMCClear(hdmc);
   }     

   fclose(file);

#ifdef DMC_DEBUG
   DMCTrace("Leaving DMCDownloadFile.\n");
#endif   

   return rc;
}

LONG FAR GALILCALL DMCDownloadFromBuffer(HANDLEDMC hdmc, PSZ pszBuffer, PSZ pszLabel)
{
   LONG  rc = 0L;
   int   iIndex;
   char  *pch;
   char  *pchToken;
   char  szLine[100];
   ULONG ulBytesRead;
   ULONG ulBytesWritten;
   LONG  ulTimeout;
   int   bSentEndOfDownload = FALSE;
   CHAR szTempCommand[COMMAND_BUFFER_SIZE];
   CHAR szTempResponse[RESPONSE_BUFFER_SIZE];
   
   szTempCommand[0] = '\0';
   szTempResponse[0] = '\0';

#ifdef DMC_DEBUG
   DMCTrace("Entering DMCDownloadFromBuffer.\n");
#endif   

   iIndex = Handle2Index(hdmc);
   if (iIndex == -1)
   {
#ifdef DMC_DEBUG   
      DMCTrace("   Invalid handle.\n");
      DMCTrace("Leaving DMCDownloadFromBuffer.\n");
#endif      
      return DMCERROR_HANDLE;
   }

   if (!pszBuffer)
   {
#ifdef DMC_DEBUG   
      DMCTrace("   Buffer is NULL.\n");
      DMCTrace("Leaving DMCDownloadFromBuffer.\n");
#endif      
      return DMCERROR_ARGUMENT;
   }

   /* Turn echo off */
   rc = DMCCommand(hdmc, "EO0\r", szTempResponse, sizeof(szTempResponse));
   if (rc)
   {
#ifdef DMC_DEBUG      
      DMCTrace("   Could not turn echo off.\n");
      DMCTrace("Entering DMCDownloadFromBuffer.\n");
#endif      
      return rc;
   }

   // Send the download command
   strcpy(szTempCommand, "DL");
   if (pszLabel)
   {
      if (*pszLabel)
      {
#ifdef DMC_DEBUG      
         DMCTrace("   Label <%s>.\n", pszLabel);
#endif         
         if (*pszLabel == '#')
            strncat(szTempCommand, pszLabel, sizeof(szTempCommand)-4);
         else
         {
            strcat(szTempCommand, "#");
            strncat(szTempCommand, pszLabel, sizeof(szTempCommand)-5);
         }
      }
   }
   strcat(szTempCommand, "\r");

   ulTimeout = controller[iIndex].controllerinfo.ulTimeout;
   controller[iIndex].controllerinfo.ulTimeout = 10;
   rc = DMCCommand(hdmc, szTempCommand, szTempResponse, sizeof(szTempResponse));
   controller[iIndex].controllerinfo.ulTimeout = ulTimeout;
   if (rc == DMCERROR_TIMEOUT)
      rc = 0L;        
   
   if (!rc)
   {
      if (strchr(szTempResponse, '?'))
      {
#ifdef DMC_DEBUG      
         DMCTrace("   Download command (DL) failed.\n");
#endif         
         rc = DMCERROR_COMMAND;
      }
   }
   else
   {
#ifdef DMC_DEBUG   
      DMCTrace("   Download command (DL) failed.\n");
#endif      
   }

   if (!rc)
   {
      pchToken = strtok(pszBuffer, "\n\r\x80\x1A");
      if (!pchToken)
      {
         DMCCommand(hdmc, "\\", szTempResponse, sizeof(szTempResponse));
         return DMCERROR_FILE;
      }

      do
      {
         strncpy(szLine, pchToken, sizeof(szLine) - 2);

         if (strncasecmp(szLine, "REM", 3) && szLine[0] != '\r' && szLine[0] != '\n' && szLine[0] != '\0')
         {
            pch = strchr(szLine, '\n');
            if (pch)
               *pch = '\0';

            pch = strchr(szLine, '\r');
            if (!pch)
               strcat(szLine, "\r");
               
#ifdef DMC_DEBUG
            DMCTrace("   Line <%s>.\n", szLine);
#endif         

            /* Send the line */
            rc = WriteData(iIndex, szLine, strlen(szLine), &ulBytesWritten);
	    
	    usleep(1000);
	    
            if (rc)
               break;
            if (ulBytesWritten == 0L)
            {
               rc = DMCERROR_TIMEOUT;
#ifdef DMC_DEBUG               
               DMCTrace("  A timeout ocurred waiting for a response from the controller.\n");
#endif               
               break;
            }
            
            CharAvailableInput(iIndex);              

            /* Get the response if any */
            if (CharAvailableInput(iIndex))
            {
               rc = ReadData(iIndex, szTempResponse, sizeof(szTempResponse), &ulBytesRead);
               if (strchr(szTempResponse, '?'))
               {
                  rc = DMCERROR_DOWNLOAD;
                  break;
               }
            }
         }
         pchToken = strtok(NULL, "\n\r\x80\x1A");
      } while (pchToken);
   }

   /* Get the response if any */
   if (CharAvailableInput(iIndex))
   {
      rc = ReadData(iIndex, szTempResponse, sizeof(szTempResponse), &ulBytesRead);
      if (strchr(szTempResponse, '?'))
         rc = DMCERROR_DOWNLOAD;
   }

   if (!rc)
   {
      bSentEndOfDownload = TRUE;
      rc = DMCCommand(hdmc, "\\", szTempResponse, sizeof(szTempResponse));
      if (!rc)
      {
         if (strchr(szTempResponse, '?'))
            rc = DMCERROR_DOWNLOAD;
      }
   }
   
   /* Clear the error */
   if (rc == DMCERROR_DOWNLOAD || rc == DMCERROR_TIMEOUT)
   {
      if (!bSentEndOfDownload)
         DMCCommand(hdmc, "\\", szTempResponse, sizeof(szTempResponse));
      DMCClear(hdmc);
   }

#ifdef DMC_DEBUG
   DMCTrace("Leaving DMCDownloadFromBuffer.\n");
#endif
   return rc;
}

LONG FAR GALILCALL DMCUploadFile(HANDLEDMC hdmc, PSZ pszFileName)
{
   LONG     rc = 0L;
   int      iIndex;
   FILE*    file;
   int      fDoLoop = TRUE;
   ULONG    ulBytes;
   ULONG    cbResponse = 1024L;
   char*    pszResponse;
   CHAR szTempResponse[RESPONSE_BUFFER_SIZE];
   
   szTempResponse[0] = '\0';

   DMCTrace("Entering DMCUploadFile.\n");

   iIndex = Handle2Index(hdmc);
   if (iIndex == -1)
   {
#ifdef DMC_DEBUG
      DMCTrace("   Invalid handle.\n");
      DMCTrace("Leaving DMCDownloadFromBuffer.\n");
#endif
      return DMCERROR_HANDLE;
   }

   if (!pszFileName)
   {
#ifdef DMC_DEBUG
      DMCTrace("   File name is NULL.\n");
      DMCTrace("Leaving DMCUploadFile.\n");
#endif
      return DMCERROR_ARGUMENT;
   }

   pszResponse = (char*)calloc(1, (size_t)cbResponse);
   if (!pszResponse)
   {
#ifdef DMC_DEBUG
      DMCTrace("   Out of memory.\n");
      DMCTrace("Leaving DMCUploadFile.\n");
#endif
      return DMCERROR_MEMORY;
   }

   DMCClear(hdmc);

   /* Turn echo off */
   rc = DMCCommand(hdmc, "EO0\r", szTempResponse, sizeof(szTempResponse));
   if (rc)
   {
      free(pszResponse);
#ifdef DMC_DEBUG
      DMCTrace("   Could not turn echo off.\n");
      DMCTrace("Leaving DMCUploadFile.\n");
#endif
      return rc;
   }

   if ((file = fopen(pszFileName, "wb")) == NULL)
   {
      free(pszResponse);
#ifdef DMC_DEBUG
      DMCTrace("   File error.\n");
      DMCTrace("Leaving DMCUploadFile.\n");
#endif
      return DMCERROR_FILE;
   }

   /* Send the upload command */
   rc = DMCWriteData(hdmc, "UL\r", 3, &ulBytes);
   if (ulBytes == 0L)
      rc = DMCERROR_COMMAND;

   if (rc)
      fDoLoop = FALSE;

   while (fDoLoop)
   {
      char* pch;

      *pszResponse = '\0';
      
      rc = DMCReadData(hdmc, pszResponse, cbResponse, &ulBytes);

      pch = strchr(pszResponse, 0x1A);
      if (pch)
      {
         *pch = '\0';
         fDoLoop = FALSE;
      }

      if (*pszResponse == '\r' && strlen(pszResponse) == 1)
         *pszResponse = '\0';

      if (rc)
         fDoLoop = FALSE;
      else
      {      
         if (*pszResponse)
         {         
            fputs(pszResponse, file);
#ifdef DMC_DEBUG
            DMCTrace("   File <%s>.\n", pszResponse);
#endif         
         }
      }            
   }

   fclose(file);

   free(pszResponse);

#ifdef DMC_DEBUG
   DMCTrace("Leaving DMCUploadFile.\n");
#endif
   return rc;
}

LONG FAR GALILCALL DMCUploadToBuffer(HANDLEDMC hdmc, PCHAR pchBuffer, ULONG cbBuffer)
{
   LONG  rc = 0L;
   CHAR szTempResponse[RESPONSE_BUFFER_SIZE];
   
   szTempResponse[0] = '\0';

#ifdef DMC_DEBUG
   DMCTrace("Entering DMCUploadToBuffer.\n");
#endif

   *pchBuffer = '\0';

   // Turn echo off
   rc = DMCCommand(hdmc, "EO0\r", szTempResponse, sizeof(szTempResponse));
   if (rc)
   {
#ifdef DMC_DEBUG
      DMCTrace("   Could not turn echo off.\n");
      DMCTrace("Entering DMCUploadToBuffer.\n");
#endif      
      return rc;
   }

   // Send the upload command
   rc = DMCCommand(hdmc, "UL\r", pchBuffer, cbBuffer);
   if (!rc)
   {
      if (*pchBuffer == '?')
         rc = DMCERROR_COMMAND;
      else
      {
         char* pch;

         pch = strrchr(pchBuffer, 0x1A);
         if (pch)
            *pch = '\0';

         if (pchBuffer[strlen(pchBuffer)-1] == '\n')
            pchBuffer[strlen(pchBuffer)-1] = '\0';
         if (pchBuffer[strlen(pchBuffer)-1] == '\r')
            pchBuffer[strlen(pchBuffer)-1] = '\0';
#ifdef DMC_DEBUG
         DMCTrace("   File <%s>.\n", pchBuffer);
#endif         
      }
   }

#ifdef DMC_DEBUG
   DMCTrace("Leaving DMCUploadToBuffer.\n");
#endif
   return rc;
}

LONG FAR GALILCALL DMCSendFile(HANDLEDMC hdmc, PSZ pszFileName)
{
   LONG  rc = 0L;
   int   iIndex;  
   FILE* file;
   char  fDoLoop = TRUE;
   CHAR szTempCommand[COMMAND_BUFFER_SIZE];
   CHAR szTempResponse[RESPONSE_BUFFER_SIZE];
   
   szTempCommand[0] = '\0';
   szTempResponse[0] = '\0';

#ifdef DMC_DEBUG
   DMCTrace("Entering DMCSendFile.\n");
#endif

   iIndex = Handle2Index(hdmc);
   if (iIndex == -1)
   {
#ifdef DMC_DEBUG   
      DMCTrace("   Invalid handle.\n");
      DMCTrace("Leaving DMCSendFile.\n");
#endif      
      return DMCERROR_HANDLE;
   }

   if (!pszFileName)
   {
#ifdef DMC_DEBUG   
      DMCTrace("   File name is NULL.\n");
      DMCTrace("Leaving DMCSendFile.\n");
#endif      
      return DMCERROR_ARGUMENT;
   }

   if ((file = fopen(pszFileName, "r")) == NULL)
   {
#ifdef DMC_DEBUG
      DMCTrace("   File error.\n");
      DMCTrace("Leaving DMCSendFile.\n");
#endif
      return DMCERROR_FILE;
   }

   while (!rc && fDoLoop)
   {
      if (fgets(szTempCommand, sizeof(szTempCommand), file) == NULL)
         fDoLoop = FALSE;
      else
      {
         char *pch;
         if (szTempCommand[0] != '\n' && szTempCommand[0] != '\r')
         {
            pch = strchr(szTempCommand, '\n');
            if (pch != NULL)
               *pch = '\0';
            if (!strchr(szTempCommand, '\r'))
               strcat(szTempCommand, "\r");
            if (strncasecmp(szTempCommand, "REM", 3))
            {
               szTempResponse[0] = '\0';
#ifdef DMC_DEBUG
               DMCTrace("   Command <%s>.\n", szTempCommand);
#endif
               rc = DMCCommand(hdmc, szTempCommand, szTempResponse, sizeof(szTempResponse));
#ifdef DMC_DEBUG
               DMCTrace("   Response <%s>.\n", szTempResponse);
#endif
               if (rc == DMCERROR_BUFFERFULL)
                  rc = 0L;
               else if (rc == DMCNOERROR)
               {
                  if (strchr(szTempResponse, '?'))
                     rc = DMCERROR_COMMAND;
               }
            }
         }
      }
   }

   fclose(file);

#ifdef DMC_DEBUG
   DMCTrace("Leaving DMCSendFile.\n");
#endif
   return rc;
}

LONG FAR GALILCALL DMCSetTimeout(HANDLEDMC hdmc, LONG ulTimeout)
{
   LONG  rc = 0L;
   int   iIndex;

#ifdef DMC_DEBUG
   DMCTrace("Enmtering DMCSetTimeout.\n");
#endif

   iIndex = Handle2Index(hdmc);
   if (iIndex == -1)
   {
#ifdef DMC_DEBUG  
      DMCTrace("   Invalid handle.\n");
      DMCTrace("Leaving DMCSetTimeout.\n");
#endif      
      return DMCERROR_HANDLE;
   }

   controller[iIndex].controllerinfo.ulTimeout = ulTimeout;

#ifdef DMC_DEBUG
   DMCTrace("Leaving DMCSetTimeout.\n");
#endif
   return rc;
}

LONG FAR GALILCALL DMCGetTimeout(HANDLEDMC hdmc, PLONG pulTimeout)
{
   LONG  rc = 0L;
   int   iIndex;

#ifdef DMC_DEBUG
   DMCTrace("Enmtering DMCGetTimeout.\n");
#endif

   iIndex = Handle2Index(hdmc);
   if (iIndex == -1)
   {
#ifdef DMC_DEBUG  
      DMCTrace("   Invalid handle.\n");
      DMCTrace("Leaving DMCGetTimeout.\n");
#endif      
      return DMCERROR_HANDLE;
   }

   if (!pulTimeout)
   {
#ifdef DMC_DEBUG  
      DMCTrace("   Time-out is NULL.\n");
      DMCTrace("Leaving DMCGetTimeout.\n");
#endif      
      return DMCERROR_ARGUMENT;
   }

   *pulTimeout = controller[iIndex].controllerinfo.ulTimeout;

#ifdef DMC_DEBUG
   DMCTrace("Leaving DMCGetTimeout.\n");
#endif
   return rc;
}

LONG FAR GALILCALL DMCDiagnosticsOn(HANDLEDMC hdmc, PSZ pszFileName, SHORT fAppend)
{
   FILE* fileTrace;
   
   hdmc = hdmc;

   if (!pszFileName)
      return DMCERROR_ARGUMENT;

   if (fAppend)
      fileTrace = fopen(pszFileName, "a");
   else
      fileTrace = fopen(pszFileName, "w");

   if (fileTrace)
      fclose(fileTrace);
   else
      return DMCERROR_FILE;

   strncpy(szFileTrace, pszFileName, sizeof(szFileTrace) - 1);

   return 0L;
}

LONG FAR GALILCALL DMCDiagnosticsOff(HANDLEDMC hdmc)
{
   hdmc = hdmc;

   szFileTrace[0] = '\0';

   return 0L;
}

LONG FAR GALILCALL DMCError(HANDLEDMC hdmc, LONG lError, PCHAR pchMessage,
   ULONG cbMessage)
{
   LONG  rc = 0L;
   CHAR szTempResponse[RESPONSE_BUFFER_SIZE];
   
   szTempResponse[0] = '\0';

#ifdef DMC_DEBUG
   DMCTrace("Entering DMCError.\n");
#endif   

   if (!pchMessage || !cbMessage)
   {
#ifdef DMC_DEBUG
      DMCTrace("   One or more arguments is NULL or 0.\n");
      DMCTrace("Leaving DMCError.\n");
#endif      
      return DMCERROR_ARGUMENT;
   }

   *pchMessage = '\0';

   switch (lError)
   {

      default:
         strncpy(pchMessage, "Unknown error.", (size_t)(cbMessage - 1L));
         break;

      case 0L:
         break;

      case DMCERROR_TIMEOUT:
         strncpy(pchMessage, "A time-out occurred while waiting for a response from the controller.", (size_t)(cbMessage - 1L));
         break;

      case DMCERROR_COMMAND:
         if (Handle2Index(hdmc) != -1)
         {
            rc = DMCCommand(hdmc, "TC1\r", pchMessage, cbMessage);
            if (strchr(pchMessage, '?'))
               rc = DMCERROR_COMMAND;
            if (!rc)
            {
               while (*(pchMessage + strlen(pchMessage) - 1) == ':' ||
                  *(pchMessage + strlen(pchMessage) - 1) == '\r' ||
                  *(pchMessage + strlen(pchMessage) - 1) == '\n')
                  *(pchMessage + strlen(pchMessage) - 1) = '\0';
               if (*pchMessage)
                  strcat(pchMessage, ".");
               else
                  strncpy(pchMessage, "Unknown command error.", (size_t)(cbMessage - 1L));                  
            }
            else
               strncpy(pchMessage, "Could not retrieve error code from controller.", (size_t)(cbMessage - 1L));
         }
         else
            strncpy(pchMessage, "Could not retrieve error code from controller.", (size_t)(cbMessage - 1L));
         break;
      
      case DMCERROR_DOWNLOAD:      
      {
         if (Handle2Index(hdmc) != -1)
         {
            int   i;
            int   iLineNumber = -1;
            char  szTempMessage1[256] = "";

            /* Get the line number in error */
            for (i = 0; i < 10; i++)
            {
               szTempResponse[0] = '\0';
               rc = DMCCommand(hdmc, "MG_ED\r", szTempResponse, sizeof(szTempResponse));                 
               if (strchr(szTempResponse, '?'))
                  rc = DMCERROR_COMMAND;
               if (!rc)
               {
                  iLineNumber = atoi(szTempResponse);
                  break;
               }
            }           

            /* Get the cause of the error */
            rc = DMCCommand(hdmc, "TC1\r", szTempMessage1, sizeof(szTempMessage1));               

            if (rc == 0 && szTempMessage1[0])
            {
               char  szTempMessage2[256];

               while (szTempMessage1[strlen(szTempMessage1) - 1] == ':' ||
                  szTempMessage1[strlen(szTempMessage1) - 1] == '\r' ||
                  szTempMessage1[strlen(szTempMessage1) - 1] == '\n')
                  szTempMessage1[strlen(szTempMessage1) - 1] = '\0';

               if (iLineNumber >= 0)
                  sprintf(szTempMessage2, "%s. Line Number in error = %d.", szTempMessage1, iLineNumber);                     
               else
                  strcpy(szTempMessage2, szTempMessage1);                     
               strncpy(pchMessage, szTempMessage2, (size_t)(cbMessage - 1L));
            }
            else
               strncpy(pchMessage, "Download error - probable cause is a line too long or too many lines.", (size_t)(cbMessage - 1L));
         }
         break;
      }
      
      case DMCERROR_CONTROLLER:
         strncpy(pchMessage, "Galil controller could not be found in Windows registry.", (size_t)(cbMessage - 1L));
         break;

      case DMCERROR_FILE:
         strncpy(pchMessage, "File could not be opened.", (size_t)(cbMessage - 1L));
         break;

      case DMCERROR_DRIVER:
         strncpy(pchMessage, "Device driver could not be opened, or a read or write error occured.", (size_t)(cbMessage - 1L));
         break;

      case DMCERROR_HANDLE:
         strncpy(pchMessage, "Invalid Galil controller handle.", (size_t)(cbMessage - 1L));
         break;

      case DMCERROR_HMODULE:
         strncpy(pchMessage, "Support dynamic link library could not be loaded.", (size_t)(cbMessage - 1L));
         break;

      case DMCERROR_MEMORY:
         strncpy(pchMessage, "Out of memory.", (size_t)(cbMessage - 1L));
         break;

      case DMCERROR_BUFFERFULL:
         strncpy(pchMessage, "Response from the controller was larger than the response buffer supplied.", (size_t)(cbMessage - 1L));
         break;

      case DMCERROR_RESPONSEDATA:
         strncpy(pchMessage, "Response from the controller overflowed the internal additional response buffer.", (size_t)(cbMessage - 1L));
         break;

      case DMCERROR_DMA: 
         strncpy(pchMessage, "Could not communicate with DMA channel.", (size_t)(cbMessage - 1L));
         break;

      case DMCERROR_ARGUMENT:
         strncpy(pchMessage, "One or more required arguments to a DMC API function call was NULL.", (size_t)(cbMessage - 1L));
         break;

      case DMCERROR_DATARECORD:
         strncpy(pchMessage, "Could not access DMC data record.", (size_t)(cbMessage - 1L));
         break;

      case DMCERROR_FIRMWARE:
         strncpy(pchMessage, "Could not update the controller's firmware.", (size_t)(cbMessage - 1L));
         break;

      case DMCERROR_CONVERSION:
         strncpy(pchMessage, "Could not convert DMC command (ASCII to binary or binary to ASCII).", (size_t)(cbMessage - 1L));
         break;

      case DMCERROR_REGISTRY:
         strncpy(pchMessage, "Could not access or modify the controller's registry information.", (size_t)(cbMessage - 1L));
         break;

      case DMCERROR_RESOURCE:
         strncpy(pchMessage, "Windows reports a resource conflict with the current hardware configuration.", (size_t)(cbMessage - 1L));
         break;

      case DMCERROR_BUSY:
         strncpy(pchMessage, "Controller is busy and not ready to accept commands.", (size_t)(cbMessage - 1L));
         break;

		case DMCERROR_DEVICE_DISCONNECTED:
         strncpy(pchMessage, "Controller has been disconnected from the communications channel (USB or Ethernet).", (size_t)(cbMessage - 1L));
         break;
   }

#ifdef DMC_DEBUG
   DMCTrace("   Error <%s>.\n", pchMessage);
#endif   

#ifdef DMC_DEBUG
   DMCTrace("Leaving DMCError.\n");
#endif
   return rc;
}

LONG FAR GALILCALL DMCReset(HANDLEDMC hdmc)
{
   LONG  rc = 0L;
   int   iIndex;  
   CHAR szTempResponse[RESPONSE_BUFFER_SIZE];
   
   szTempResponse[0] = '\0';

#ifdef DMC_DEBUG
   DMCTrace("Entering DMCReset.\n");
#endif
   
   iIndex = Handle2Index(hdmc);
   if (iIndex == -1)
   {
#ifdef DMC_DEBUG
      DMCTrace("   Invalid handle.\n");
      DMCTrace("Leaving DMCReset.\n");
#endif      
      return DMCERROR_HANDLE;
   }

   rc = DMCCommand(hdmc, "RS\r", szTempResponse, sizeof(szTempResponse));
   if (strchr(szTempResponse, '?'))
      rc = DMCERROR_COMMAND;  
   
   if (!rc)
      rc = DMCCommand(hdmc, "EO0\r", szTempResponse, sizeof(szTempResponse));
   else
   {
#ifdef DMC_DEBUG      
      DMCTrace("   RS command failed.\n");      
#endif      
   }
   if (!rc)
      ;
   else
   {
#ifdef DMC_DEBUG
      DMCTrace("   EO command failed.\n");
#endif
   }

#ifdef DMC_DEBUG
   DMCTrace("Leaving DMCReset.\n");
#endif   

   return rc;
}

LONG FAR GALILCALL DMCMasterReset(HANDLEDMC hdmc)
{
   LONG  rc = 0L;
   int   iIndex;  
   CHAR szTempCommand[COMMAND_BUFFER_SIZE];
   CHAR szTempResponse[RESPONSE_BUFFER_SIZE];
   
   szTempCommand[0] = '\0';
   szTempResponse[0] = '\0';

#ifdef DMC_DEBUG
   DMCTrace("Entering DMCMasterReset.\n");
#endif   

   iIndex = Handle2Index(hdmc);
   if (iIndex == -1)
   {
#ifdef DMC_DEBUG   
      DMCTrace("   Invalid handle.\n");
      DMCTrace("Leaving DMCMasterReset.\n");
#endif      
      return DMCERROR_HANDLE;
   }

   szTempCommand[0] = 18;
   szTempCommand[1] = 19;
   szTempCommand[2] = '\r';
   szTempCommand[3] = '\0';

   rc = DMCCommand(hdmc, szTempCommand, szTempResponse, sizeof(szTempResponse));
   if (strchr(szTempResponse, '?'))
      rc = DMCERROR_COMMAND;  

   if (!rc)
      rc = DMCCommand(hdmc, "EO0\r", szTempResponse, sizeof(szTempResponse));
   else
   {
#ifdef DMC_DEBUG      
      DMCTrace("   ^R^S command failed.\n");      
#endif      
   }
   if (!rc)
      ;
   else
   {
#ifdef DMC_DEBUG
      DMCTrace("   EO command failed.\n");
#endif      
   }

#ifdef DMC_DEBUG
   DMCTrace("Leaving DMCMasterReset.\n");
#endif   

   return rc;
}

LONG FAR GALILCALL DMCVersion(HANDLEDMC hdmc, PCHAR pchVersion, ULONG cbVersion)
{
   LONG  rc = 0L;
   int   iIndex;  
   CHAR szTempCommand[COMMAND_BUFFER_SIZE];
   CHAR szTempResponse[RESPONSE_BUFFER_SIZE];
   
   szTempCommand[0] = '\0';
   szTempResponse[0] = '\0';

#ifdef DMC_DEBUG
   DMCTrace("Entering DMCVersion.\n");
#endif

   iIndex = Handle2Index(hdmc);
   if (iIndex == -1)
   {
#ifdef DMC_DEBUG
      DMCTrace("   Invalid handle.\n");
      DMCTrace("Leaving DMCVersion.\n");
#endif      
      return DMCERROR_HANDLE;
   }

   if (!pchVersion || !cbVersion)
   {
#ifdef DMC_DEBUG
      DMCTrace("   One or more arguments is NULL or 0.\n");
      DMCTrace("Leaving DMCError.\n");
#endif      
      return DMCERROR_ARGUMENT;
   }

   szTempCommand[0] = 18;
   szTempCommand[1] = 22;
   szTempCommand[2] = '\r';
   szTempCommand[3] = '\0';

   *pchVersion = '\0';

   rc = DMCCommand(hdmc, szTempCommand, pchVersion, cbVersion);
   if (strchr(pchVersion, '?'))
      rc = DMCERROR_COMMAND;  

   if (!rc)
   {
      char* pch;

      if (!*pchVersion)
      {
         DMCTrace("   No version information.\n");
         return DMCERROR_COMMAND;
      }

      szTempResponse[0] = '\0';

      pch = strchr(pchVersion, '\r');
      if (pch)
         *pch = '\0';

      /* Find the revision string */
      pch = strchr(pchVersion, 'v');
      if (!pch)
         pch = strchr(pchVersion, 'V');
      
      /* Is the revision a standard revision? */
      if (pch)
         pch += 2;
      else
      {
         pch = pchVersion + 3;
         while (isdigit(*pch) && *pch)
            pch++; 
         if (!*pch)
            pch = pchVersion;
      }

      if (strstr(pchVersion, "DMC610"))
         sprintf(szTempResponse, "Galil DMC-610 1 axis controller revision %s", pch);
      else if (strstr(pchVersion, "DMC620"))
         sprintf(szTempResponse, "Galil DMC-620 2 axis controller revision %s", pch);
      else if (strstr(pchVersion, "DMC630"))
         sprintf(szTempResponse, "Galil DMC-630 3 axis controller revision %s", pch);
      else if (strstr(pchVersion, "DMC710"))
         sprintf(szTempResponse, "Galil DMC-710 1 axis controller revision %s", pch);
      else if (strstr(pchVersion, "DMC720"))
         sprintf(szTempResponse, "Galil DMC-720 2 axis controller revision %s", pch);
      else if (strstr(pchVersion, "DMC730"))
         sprintf(szTempResponse, "Galil DMC-730 3 axis controller revision %s", pch);
      else if (strstr(pchVersion, "DMC740"))
         sprintf(szTempResponse, "Galil DMC-740 4 axis controller revision %s", pch);
      else if (strstr(pchVersion, "DMC1010"))
         sprintf(szTempResponse, "Galil DMC-1010 1 axis controller revision %s", pch);
      else if (strstr(pchVersion, "DMC1020"))
         sprintf(szTempResponse, "Galil DMC-1020 2 axis controller revision %s", pch);
      else if (strstr(pchVersion, "DMC1030"))
         sprintf(szTempResponse, "Galil DMC-1030 3 axis controller revision %s", pch);
      else if (strstr(pchVersion, "DMC1040"))
         sprintf(szTempResponse, "Galil DMC-1040 4 axis controller revision %s", pch);
      else if (strstr(pchVersion, "DMC1050"))
         sprintf(szTempResponse, "Galil DMC-1050 5 axis controller revision %s", pch);
      else if (strstr(pchVersion, "DMC1060"))
         sprintf(szTempResponse, "Galil DMC-1060 6 axis controller revision %s", pch);
      else if (strstr(pchVersion, "DMC1070"))
         sprintf(szTempResponse, "Galil DMC-1070 7 axis controller revision %s", pch);
      else if (strstr(pchVersion, "DMC1080"))
         sprintf(szTempResponse, "Galil DMC-1080 8 axis controller revision %s", pch);
      else if (strstr(pchVersion, "DMC1210"))
         sprintf(szTempResponse, "Galil DMC-1210 1 axis controller revision %s", pch);
      else if (strstr(pchVersion, "DMC1220"))
         sprintf(szTempResponse, "Galil DMC-1220 2 axis controller revision %s", pch);
      else if (strstr(pchVersion, "DMC1230"))
         sprintf(szTempResponse, "Galil DMC-1230 3 axis controller revision %s", pch);
      else if (strstr(pchVersion, "DMC1240"))
         sprintf(szTempResponse, "Galil DMC-1240 4 axis controller revision %s", pch);
      else if (strstr(pchVersion, "DMC1250"))
         sprintf(szTempResponse, "Galil DMC-1250 5 axis controller revision %s", pch);
      else if (strstr(pchVersion, "DMC1260"))
         sprintf(szTempResponse, "Galil DMC-1260 6 axis controller revision %s", pch);
      else if (strstr(pchVersion, "DMC1270"))
         sprintf(szTempResponse, "Galil DMC-1270 7 axis controller revision %s", pch);
      else if (strstr(pchVersion, "DMC1280"))
         sprintf(szTempResponse, "Galil DMC-1280 8 axis controller revision %s", pch);
      else if (strstr(pchVersion, "DMC1410"))
         sprintf(szTempResponse, "Galil DMC-1410 1 axis controller revision %s", pch);
      else if (strstr(pchVersion, "DMC1411"))
         sprintf(szTempResponse, "Galil DMC-1411 1 axis controller revision %s", pch);
      else if (strstr(pchVersion, "DMC1412"))
         sprintf(szTempResponse, "Galil DMC-1412 1 axis controller revision %s", pch);
      else if (strstr(pchVersion, "DMC1414"))
         sprintf(szTempResponse, "Galil DMC-1414 1 axis controller revision %s", pch);
      else if (strstr(pchVersion, "DMC1415"))
         sprintf(szTempResponse, "Galil DMC-1415 1 axis controller revision %s", pch);
      else if (strstr(pchVersion, "DMC1510"))
         sprintf(szTempResponse, "Galil DMC-1510 1 axis controller revision %s", pch);
      else if (strstr(pchVersion, "DMC1520"))
         sprintf(szTempResponse, "Galil DMC-1520 2 axis controller revision %s", pch);
      else if (strstr(pchVersion, "DMC1530"))
         sprintf(szTempResponse, "Galil DMC-1530 3 axis controller revision %s", pch);
      else if (strstr(pchVersion, "DMC1540"))
         sprintf(szTempResponse, "Galil DMC-1540 4 axis controller revision %s", pch);
      else if (strstr(pchVersion, "DMC1550"))
         sprintf(szTempResponse, "Galil DMC-1550 5 axis controller revision %s", pch);
      else if (strstr(pchVersion, "DMC1560"))
         sprintf(szTempResponse, "Galil DMC-1560 6 axis controller revision %s", pch);
      else if (strstr(pchVersion, "DMC1570"))
         sprintf(szTempResponse, "Galil DMC-1570 7 axis controller revision %s", pch);
      else if (strstr(pchVersion, "DMC1580"))
         sprintf(szTempResponse, "Galil DMC-1580 8 axis controller revision %s", pch);
      else if (strstr(pchVersion, "DMC1610"))
         sprintf(szTempResponse, "Galil DMC-1610 1 axis controller revision %s", pch);
      else if (strstr(pchVersion, "DMC1620"))
         sprintf(szTempResponse, "Galil DMC-1620 2 axis controller revision %s", pch);
      else if (strstr(pchVersion, "DMC1630"))
         sprintf(szTempResponse, "Galil DMC-1630 3 axis controller revision %s", pch);
      else if (strstr(pchVersion, "DMC1640"))
         sprintf(szTempResponse, "Galil DMC-1640 4 axis controller revision %s", pch);
      else if (strstr(pchVersion, "DMC1650"))
         sprintf(szTempResponse, "Galil DMC-1650 5 axis controller revision %s", pch);
      else if (strstr(pchVersion, "DMC1660"))
         sprintf(szTempResponse, "Galil DMC-1660 6 axis controller revision %s", pch);
      else if (strstr(pchVersion, "DMC1670"))
         sprintf(szTempResponse, "Galil DMC-1670 7 axis controller revision %s", pch);
      else if (strstr(pchVersion, "DMC1680"))
         sprintf(szTempResponse, "Galil DMC-1680 8 axis controller revision %s", pch);
      else if (strstr(pchVersion, "DMC1710"))
         sprintf(szTempResponse, "Galil DMC-1710 1 axis controller revision %s", pch);
      else if (strstr(pchVersion, "DMC1720"))
         sprintf(szTempResponse, "Galil DMC-1720 2 axis controller revision %s", pch);
      else if (strstr(pchVersion, "DMC1730"))
         sprintf(szTempResponse, "Galil DMC-1730 3 axis controller revision %s", pch);
      else if (strstr(pchVersion, "DMC1740"))
         sprintf(szTempResponse, "Galil DMC-1740 4 axis controller revision %s", pch);
      else if (strstr(pchVersion, "DMC1750"))
         sprintf(szTempResponse, "Galil DMC-1750 5 axis controller revision %s", pch);
      else if (strstr(pchVersion, "DMC1760"))
         sprintf(szTempResponse, "Galil DMC-1760 6 axis controller revision %s", pch);
      else if (strstr(pchVersion, "DMC1770"))
         sprintf(szTempResponse, "Galil DMC-1770 7 axis controller revision %s", pch);
      else if (strstr(pchVersion, "DMC1780"))
         sprintf(szTempResponse, "Galil DMC-1780 8 axis controller revision %s", pch);
      else if (strstr(pchVersion, "DMC1810"))
         sprintf(szTempResponse, "Galil DMC-1810 1 axis controller revision %s", pch);
      else if (strstr(pchVersion, "DMC1820"))
         sprintf(szTempResponse, "Galil DMC-1820 2 axis controller revision %s", pch);
      else if (strstr(pchVersion, "DMC1830"))
         sprintf(szTempResponse, "Galil DMC-1830 3 axis controller revision %s", pch);
      else if (strstr(pchVersion, "DMC1840"))
         sprintf(szTempResponse, "Galil DMC-1840 4 axis controller revision %s", pch);
      else if (strstr(pchVersion, "DMC1850"))
         sprintf(szTempResponse, "Galil DMC-1850 5 axis controller revision %s", pch);
      else if (strstr(pchVersion, "DMC1860"))
         sprintf(szTempResponse, "Galil DMC-1860 6 axis controller revision %s", pch);
      else if (strstr(pchVersion, "DMC1870"))
         sprintf(szTempResponse, "Galil DMC-1870 7 axis controller revision %s", pch);
      else if (strstr(pchVersion, "DMC1880"))
         sprintf(szTempResponse, "Galil DMC-1880 8 axis controller revision %s", pch);
      else if (strstr(pchVersion, "DMC1812"))
         sprintf(szTempResponse, "Galil DMC-1812 1 axis controller revision %s", pch);
      else if (strstr(pchVersion, "DMC1822"))
         sprintf(szTempResponse, "Galil DMC-1822 2 axis controller revision %s", pch);
      else if (strstr(pchVersion, "DMC1832"))
         sprintf(szTempResponse, "Galil DMC-1832 3 axis controller revision %s", pch);
      else if (strstr(pchVersion, "DMC1842"))
         sprintf(szTempResponse, "Galil DMC-1842 4 axis controller revision %s", pch);      
      else if (strstr(pchVersion, "DMC2010"))
         sprintf(szTempResponse, "Galil DMC-2010 1 axis controller revision %s", pch);
      else if (strstr(pchVersion, "DMC2020"))
         sprintf(szTempResponse, "Galil DMC-2020 2 axis controller revision %s", pch);
      else if (strstr(pchVersion, "DMC2030"))
         sprintf(szTempResponse, "Galil DMC-2030 3 axis controller revision %s", pch);
      else if (strstr(pchVersion, "DMC2040"))
         sprintf(szTempResponse, "Galil DMC-2040 4 axis controller revision %s", pch);
      else if (strstr(pchVersion, "DMC2050"))
         sprintf(szTempResponse, "Galil DMC-2050 5 axis controller revision %s", pch);
      else if (strstr(pchVersion, "DMC2060"))
         sprintf(szTempResponse, "Galil DMC-2060 6 axis controller revision %s", pch);
      else if (strstr(pchVersion, "DMC2070"))
         sprintf(szTempResponse, "Galil DMC-2070 7 axis controller revision %s", pch);
      else if (strstr(pchVersion, "DMC2080"))
         sprintf(szTempResponse, "Galil DMC-2080 8 axis controller revision %s", pch);
      else if (strstr(pchVersion, "DMC2110"))
         sprintf(szTempResponse, "Galil DMC-2110 1 axis controller revision %s", pch);
      else if (strstr(pchVersion, "DMC2120"))
         sprintf(szTempResponse, "Galil DMC-2120 2 axis controller revision %s", pch);
      else if (strstr(pchVersion, "DMC2130"))
         sprintf(szTempResponse, "Galil DMC-2130 3 axis controller revision %s", pch);
      else if (strstr(pchVersion, "DMC2140"))
         sprintf(szTempResponse, "Galil DMC-2140 4 axis controller revision %s", pch);
      else if (strstr(pchVersion, "DMC2150"))
         sprintf(szTempResponse, "Galil DMC-2150 5 axis controller revision %s", pch);
      else if (strstr(pchVersion, "DMC2160"))
         sprintf(szTempResponse, "Galil DMC-2160 6 axis controller revision %s", pch);
      else if (strstr(pchVersion, "DMC2170"))
         sprintf(szTempResponse, "Galil DMC-2170 7 axis controller revision %s", pch);
      else if (strstr(pchVersion, "DMC2180"))
         sprintf(szTempResponse, "Galil DMC-2180 8 axis controller revision %s", pch);
      else
      {
#ifdef DMC_DEBUG
         DMCTrace("   Could not parse version information <%s>.\n", pchVersion);
#endif         
      }
   }

   if (szTempResponse[0])
      strncpy(pchVersion, szTempResponse, (size_t)cbVersion - 1);

#ifdef DMC_DEBUG
   DMCTrace("Leaving DMCVersion.\n");
#endif   
   return rc;
}

LONG FAR GALILCALL DMCArrayDownload(HANDLEDMC hdmc, PSZ pszQueueName,
   USHORT usFirstElement, USHORT usLastElement, PCHAR pchData, ULONG cbData,
   PULONG pulBytesWritten)
{
   LONG     rc = 0L;
   int      iIndex = -1;
   ULONG      ulBytes = 0L;
   ULONG      i;
   CHAR szTempCommand[COMMAND_BUFFER_SIZE];
   CHAR szTempResponse[RESPONSE_BUFFER_SIZE];
   
   szTempCommand[0] = '\0';
   szTempResponse[0] = '\0';

   DMCTrace("Entering DMCArrayDownload.\n");

   iIndex = Handle2Index(hdmc);
   if (iIndex == -1)
   {
      DMCTrace("   Invalid handle.\n");
      DMCTrace("Leaving DMCArrayDownload.\n");
      return DMCERROR_HANDLE;
   }

   if (!pszQueueName || !pchData || !cbData || !pulBytesWritten)
   {
      DMCTrace("   One or more arguments is NULL or 0.\n");
      DMCTrace("Leaving DMCArrayDownload.\n");
      return DMCERROR_ARGUMENT;
   }

   /* Controller does not like \n, or spaces - zeros are harmless */
   for (i = 0; i < cbData; i++)
   {
      if (*(pchData + i) == '\n' || *(pchData + i) == ' ')
         *(pchData + i) = '0';
   }

   *pulBytesWritten = 0L;

   sprintf(szTempCommand, "QD %s[],%u,%u\r", pszQueueName, usFirstElement, usLastElement);

   /* Send the QD command to the device driver */
   rc = DMCWriteData(hdmc, szTempCommand, strlen(szTempCommand), &ulBytes);

   if (rc == DMCNOERROR && ulBytes > 0L)
   {
      /* Send the array elements */
      rc = DMCWriteData(hdmc, pchData, cbData, pulBytesWritten);
      /* Terminate the QD command */
      szTempCommand[0] = 26;
      DMCWriteData(hdmc, szTempCommand, 1, &ulBytes);

      if (rc == DMCNOERROR)
      {
         long   lCurrentTime;
         long   lTimeout;

         /* Get the response from the controller */
         lCurrentTime = DMCGetTime();
         lTimeout = lCurrentTime + MAX(1000L, (long)controller[iIndex].controllerinfo.ulTimeout);
         do
         {
            lCurrentTime = DMCGetTime();
            if (lCurrentTime > lTimeout)
               rc = DMCERROR_TIMEOUT;
            else
            {
               rc = DMCReadData(hdmc, szTempResponse, sizeof(szTempResponse), &ulBytes);
               szTempResponse[ulBytes] = '\0';
            }
         } while ((!strchr(szTempResponse, ':')) && (!strchr(szTempResponse, '?')) && (rc == DMCNOERROR));

         if (rc == DMCNOERROR)
         {
            /* Check for a command error */
            if (strchr(szTempResponse, '?'))
               rc = DMCERROR_COMMAND;
         }
      }
   }

   /* Send a dummy command to clear buffers/fifo */
   DMCCommand(hdmc, "\r", szTempResponse, sizeof(szTempResponse));

   DMCTrace("Leaving DMCArrayDownload.\n");

   return rc;
}

LONG FAR GALILCALL DMCArrayUpload(HANDLEDMC hdmc, PSZ pszQueueName,
   USHORT usFirstElement, USHORT usLastElement, PCHAR pchData, ULONG cbData,
   PULONG pulBytesRead, SHORT fComma)
{
   LONG  rc = 0L;
   int   iIndex = -1;
   char  szCMD[64] = "";

   DMCTrace("Entering DMCArrayUpload.\n");

   iIndex = Handle2Index(hdmc);
   if (iIndex == -1)
   {
      DMCTrace("   Invalid handle.\n");
      DMCTrace("Leaving DMCArrayUpload.\n");
      return DMCERROR_HANDLE;
   }

   if (!pszQueueName || !pchData || !cbData || !pulBytesRead)
   {
      DMCTrace("   One or more arguments is NULL or 0.\n");
      DMCTrace("Leaving DMCArrayUpload.\n");
      return DMCERROR_ARGUMENT;
   }

   *pulBytesRead = 0L;

   sprintf(szCMD, "QU %s[],%u,%u,%d\r", pszQueueName, usFirstElement, usLastElement, fComma);

   /* Send the QU command */
   rc = DMCCommand(hdmc, szCMD, pchData, cbData);

   if (strchr(pchData, '?'))
   {
      *pchData = '\0';
      rc = DMCERROR_COMMAND;
   }
   else
   {
      /* Get rid of termination characters */
      while (pchData[strlen(pchData) - 1] == 26 || pchData[strlen(pchData) - 1] == ':' ||
         pchData[strlen(pchData) - 1] == '\r' || pchData[strlen(pchData) - 1] == '\n')
            pchData[strlen(pchData) - 1] = '\0';
   }

   *pulBytesRead = strlen(pchData);

   DMCTrace("Leaving DMCArrayUpload.\n");

   return rc;
}

LONG FAR GALILCALL DMCWaitForMotionComplete(HANDLEDMC hdmc, PSZ pszAxes, SHORT fDispatchMsgs)
{
   int            i;
   int            bLoop;
   int            iAxes;
   long           rc = 0L;
   char           szCmd[13];
   char           szResponse[32];
   int barMTServo[8] = {1,1,1,1,1,1,1,1};
//   MOTIONTHREAD*  pmt = (MOTIONTHREAD*)pArgs;

//    if (!pmt)
//    {
//       ExitThread((DWORD)DMCERROR_DRIVER);
//       return (DWORD)DMCERROR_DRIVER;
//    }

   // How many axes?
   iAxes = strlen(pszAxes);

   // STA: 3/9/05
   // Servo or stepper?

   DMCTrace("    Checking Motor Type.\n");

   for (i = 0; i < iAxes; i++)
   {
      sprintf(szCmd, "MG_MT%c\r", pszAxes[i]);
      DMCTrace("    Sending MG_MTx.\n");
      rc = DMCCommand(hdmc, szCmd, szResponse, sizeof(szResponse));

      if (rc == DMCNOERROR)
      {
         if (abs(atoi(szResponse)) != 1)
         {
            barMTServo[i] = 0;
            DMCTrace("    Motor is a stepper.\n");
         }
         else
            DMCTrace("    Motor is a servo.\n");
      }
      //else play the odds, assume servo. :)  For example MG_MTS will cause error -2
   }

   for (i = 0; i < iAxes; i++)
   {
      // Check each axis in turn
      bLoop = TRUE;

      if (barMTServo[i])
         sprintf(szCmd, "MG_BG%c\r", pszAxes[i]);
      else
         sprintf(szCmd, "MG_RP%c-_TD%c\r", pszAxes[i], pszAxes[i]);

      // Loop while motion is not complete
      while (bLoop)
      {
         rc = DMCCommand(hdmc, szCmd, szResponse, sizeof(szResponse));
         if (rc == DMCNOERROR)
         {
            if (!atoi(szResponse))
               bLoop = FALSE;
            else
               usleep(5000);
         }
         else if (rc == DMCERROR_TIMEOUT)
            ; // Do nothing
         else
            bLoop = FALSE;
      }
   }

//    if (pmt->ThreadId)
//       PostThreadMessage(pmt->ThreadId, WM_MOTIONCOMPLETE, 0, 0);
// 
//    pmt->rc = rc;

/*
   ExitThread(0);
*/
   return rc;
}
