#include "dmclnx.h"

#ifdef DMC_SERIAL

long SERIALReadData(int iIndex, PCHAR pchResponse, ULONG cbResponse, PULONG pulBytesRead)
{
   LONG  rc = 0L;
   CHAR  szBuffer[RESPONSE_BUFFER_SIZE];
   ULONG iBytesRead = 0L;

   *pulBytesRead = 0L;
   
   memset(szBuffer, '\0', sizeof(szBuffer));

   rc = read(controller[iIndex].iDriver, szBuffer, MIN(cbResponse - 1, sizeof(szBuffer) - 1));
   if (rc > 0L)
   {
      iBytesRead = rc;
      rc = 0L;
   }
   else if (rc == -1)
   {
      if (errno == EAGAIN)
         rc = 0L;
      else
      {
#ifdef DMC_DEBUG
         DMCTrace("   Read error. Error number <%d>.\n", errno);
#endif
         rc = DMCERROR_DRIVER;
       }
   }

   if (rc == 0L && iBytesRead > 0L)  /* No problem */
   {
      ULONG i;

      for (i = 0; i < iBytesRead; i++)
      {
         /* Is this a binary command? */
         if (controller[iIndex].fBinaryCommand)
         {
            if (i < cbResponse)
               pchResponse[(size_t)i] = szBuffer[(size_t)i];
         }
         else
         {
            /* Separate repsonses (from commands) from unsolicited responses (from messages) */
            if (isascii((int)szBuffer[(size_t)i]))
            {
               if (pchResponse)
               {
                  if (strlen(pchResponse) < cbResponse - 1)
                     /* User buffer can hold more data */
                     strncat(pchResponse, &szBuffer[(size_t)i], 1);
                  else
                  {
                     /* User buffer is full - try to save the data */
                     rc = DMCERROR_BUFFERFULL;
                     if (strlen(controller[iIndex].szResponse) < sizeof(controller[iIndex].szResponse) - 1)
                        strncat(controller[iIndex].szResponse, &szBuffer[(size_t)i], 1);
                     else
                        rc = DMCERROR_RESPONSEDATA;
                  }
               }
            }
            else
            {
               szBuffer[(size_t)i] ^= 0x80;
               if (strlen(controller[iIndex].szUnsolicitedResponse) < sizeof(controller[iIndex].szUnsolicitedResponse) - 1)
                  strncat(controller[iIndex].szUnsolicitedResponse, &szBuffer[(size_t)i], 1);
            }
         }
      }
   }

   *pulBytesRead = iBytesRead;

   return rc;
}

long SERIALWriteData(int iIndex, PSZ pchCommand, ULONG cbCommand, PULONG pulBytesWritten)
{
   LONG     rc = 0L;
   ULONG    ulCurrentTime;
   ULONG    ulTimeout;
   int      iReadyToWrite = 0;
   ULONG    iBytesWritten;
   ULONG    i;
   PCHAR    pch;

   *pulBytesWritten = 0L;

#ifdef DMC_DEBUG
   DMCTrace("   Waiting for OK to send.\n");
#endif   

   ulCurrentTime = DMCGetTime();
   ulTimeout = ulCurrentTime + controller[iIndex].controllerinfo.ulTimeout;

   do
   {
      ulCurrentTime = DMCGetTime();
      if (ulCurrentTime > ulTimeout)
      {
#ifdef DMC_DEBUG      
         DMCTrace("   Time-out waiting for OK to send.\n");
#endif         
         rc = DMCERROR_TIMEOUT;
         break;
      }
      iReadyToWrite = ReadyToWrite(iIndex);
   }
   while ((rc == 0) && !iReadyToWrite);

   if (iReadyToWrite)
   {
#ifdef DMC_DEBUG   
      DMCTrace("   OK to send.\n");
#endif
      /* Send the data */
      for (i = 0L, pch = pchCommand, iBytesWritten = 0L; i < cbCommand; i++, pch++, iBytesWritten++)
      {
         rc = WriteAChar(iIndex, *pch);
         if (rc)
            break;         
      }         
   }
   else
   {
#ifdef DMC_DEBUG   
      DMCTrace("   Not OK to send.\n");
#endif      
   }      

   *pulBytesWritten = iBytesWritten;

   return rc;
}

int SERIALCharAvailableInput(int iIndex)
{
   LONG  rc = 1 /*dev_ischars(controller[iIndex].iDriver)*/;

   return rc ? TRUE : FALSE;
}

long SERIALClear(int iIndex)
{
   LONG  rc = 0L;
   ULONG iBytesWritten;

   rc = SERIALWriteData(iIndex, "\\;", 2, &iBytesWritten);

   return rc;
}

long OpenPort(int iIndex)
{
   int     i;
   long    rc;
   char    szDevice[32];
   ULONG   ulCommSpeed;
   struct  termios CommAttribs;

   if (controller[iIndex].controllerinfo.hardwareinfo.serialinfo.usCommPort > MAX_COMMPORTS)
   {
#ifdef DMC_DEBUG
      DMCTrace("   Invalid comm port. Setting comm port to 1.\n");
#endif
      controller[iIndex].controllerinfo.hardwareinfo.serialinfo.usCommPort = 1;
   }

   sprintf(szDevice, "/dev/ttyS%d", controller[iIndex].controllerinfo.hardwareinfo.serialinfo.usCommPort);

   controller[iIndex].iDriver = open(szDevice, O_RDWR | O_NONBLOCK);
   if (controller[iIndex].iDriver == -1)
   {
#ifdef DMC_DEBUG
      DMCTrace("   Could not open comm port. Error number <%d>.\n", errno);
#endif
      printf("   Could not open comm port. Error number <%d>.\n", errno);
      return DMCERROR_DRIVER;
   }

   rc = tcgetattr(controller[iIndex].iDriver, &CommAttribs);
   if (rc == -1)
   {
#ifdef DMC_DEBUG
      DMCTrace("   Could not get comm port attributes. Error number <%d>.\n", errno);
#endif
      printf("   Could not get comm port attributes. Error number <%d>.\n", errno);
      return DMCERROR_DRIVER;
   }

   CommAttribs.c_iflag = 0;
   CommAttribs.c_oflag = 0;
   CommAttribs.c_cflag = CREAD | CS8 | CRTSCTS;
   CommAttribs.c_lflag = IEXTEN;

   for (i = 0; i < NCCS; i++)
      CommAttribs.c_cc[i] = 0;

   switch (controller[iIndex].controllerinfo.hardwareinfo.serialinfo.ulCommSpeed)
   {
   	case 38400L:
   		ulCommSpeed = B38400;
   		break;
      case 19200L:
   		ulCommSpeed = B19200;
   		break;
      case 9600L:
   		ulCommSpeed = B9600;
   		break;
      case 4800L:
   		ulCommSpeed = B4800;
   		break;
      case 2400L:
   		ulCommSpeed = B2400;
   		break;
      case 1200L:
   		ulCommSpeed = B1200;
   		break;
      case 300L:
   		ulCommSpeed = B300;
   		break;
      default:
         /* Illegal setting, use default value */
#ifdef DMC_DEBUG
         DMCTrace("   Invalid baud rate. Setting baud rate to 9600.\n");
#endif
         controller[iIndex].controllerinfo.hardwareinfo.serialinfo.ulCommSpeed = 9600L;
         ulCommSpeed = B9600;
         break;
	}      

   rc = cfsetispeed(&CommAttribs, (speed_t)ulCommSpeed);
   if (rc == -1)
   {
#ifdef DMC_DEBUG
      DMCTrace("   Could not set input comm speed. Error number <%d>.\n", errno);
#endif
      printf("   Could not set input comm speed. Error number <%d>.\n", errno);
      return DMCERROR_DRIVER;
   }

   rc = cfsetospeed(&CommAttribs, (speed_t)ulCommSpeed);
   if (rc == -1)
   {
#ifdef DMC_DEBUG
      DMCTrace("   Could not set ouput comm speed. Error number <%d>.\n", errno);
#endif
      printf("   Could not set ouput comm speed. Error number <%d>.\n", errno);
      return DMCERROR_DRIVER;
   }

   rc = tcsetattr(controller[iIndex].iDriver, TCSANOW, &CommAttribs);
   if (rc == -1)
   {
#ifdef DMC_DEBUG
      DMCTrace("   Could not set comm port attributes. Error number <%d>.\n", errno);
#endif
      printf("   Could not set comm port attributes. Error number <%d>.\n", errno);
      return DMCERROR_DRIVER;
   }

	printf("   OK!\n");

   return 0L;
}

long ClosePort(int iIndex)
{
   if (controller[iIndex].iDriver > 0)   
      close(controller[iIndex].iDriver);
      
   controller[iIndex].iDriver = 0;

   return 0L;
}

int ReadyToRead(int iIndex)
{
   /*
   unsigned rc = dev_state(controller[iIndex].iDriver, 0, 0);

   return ((rc & _DEV_EVENT_RXRDY) ? TRUE : FALSE);
   */
   return TRUE;
}

int ReadyToWrite(int iIndex)
{
   /*
   unsigned rc = dev_state(controller[iIndex].iDriver, 0, 0);

   return ((rc & _DEV_EVENT_TXRDY) ? TRUE : FALSE);   
   */
   return TRUE;
}

long ReadAChar(int iIndex, PCHAR pch)
{
   int   rc;
   char  chTemp;

   *pch = 0;
      
   if (ReadyToRead(iIndex))
   {
      rc = read(controller[iIndex].iDriver, &chTemp, 1);
      if (rc == 0)
         return NO_MORE_DATA;
      else if (rc == -1)
      {
         if (errno == EAGAIN)
            return NO_MORE_DATA;

#ifdef DMC_DEBUG
         DMCTrace("   Read error. Error number <%d>.\n", errno);
#endif
         return DMCERROR_DRIVER;
      }
   }
   else 
      return NO_MORE_DATA;

   *pch = chTemp;

   return 0L;
}

long WriteAChar(int iIndex, CHAR ch)
{
   int   rc;
   char  chTemp = ch;

   rc = write(controller[iIndex].iDriver, &chTemp, 1);
   if (rc == -1)
   {
#ifdef DMC_DEBUG
      DMCTrace("   Write error. Error number <%d>.\n", errno);
#endif
   }

   tcdrain(controller[iIndex].iDriver);
 
   return 0L;
}

#endif /* DMC_SERIAL */
