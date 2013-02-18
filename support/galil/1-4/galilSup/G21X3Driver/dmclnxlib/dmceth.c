#include "dmclnx.h"

#ifdef DMC_ETHERNET

long OpenSocket(int iIndex)
{
   long            rc;
   int             TTL = 1;
   unsigned int    cbTTL = sizeof(TTL);
   struct ip_mreq  mreq;

   SOCKADDR_IN SockAddressC;
   SOCKADDR_IN SockAddressS;

   /* Open a socket for primary communciations */
   if (controller[iIndex].controllerinfo.hardwareinfo.socketinfo.fProtocol == EthernetProtocolUDP)
      controller[iIndex].socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
   else
      controller[iIndex].socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
   if (controller[iIndex].socket == SOCKET_ERROR)
   {
#ifdef DMC_DEBUG
      DMCTrace("Could not open a socket. Return code <%ld>.\n", errno);
#endif
      return DMCERROR_DRIVER;
   }

   if (controller[iIndex].controllerinfo.hardwareinfo.socketinfo.fProtocol == EthernetProtocolUDP)
      controller[iIndex].ulMessageSize = MAX_WINSOCK_UDP_WRITE;
   else
      controller[iIndex].ulMessageSize = MAX_WINSOCK_TCP_WRITE;

   SockAddressC.sin_family = AF_INET;                           
   SockAddressC.sin_port = 0;
   SockAddressC.sin_addr.s_addr = INADDR_ANY;

   /* Bind to the socket - needed for 2 way communication (peer-to-peer) and to use setsockopt */
   rc = bind(controller[iIndex].socket, (const struct sockaddr FAR *)&SockAddressC,
      sizeof(SockAddressC));
   if (rc == SOCKET_ERROR)
   {
#ifdef DMC_DEBUG
      DMCTrace("Could not bind to socket. Return code <%ld>.\n", errno);
#endif
      close(controller[iIndex].socket);
      return DMCERROR_DRIVER;
   }

   if (controller[iIndex].controllerinfo.hardwareinfo.socketinfo.fProtocol == EthernetProtocolTCP)
   {
      int iTemp;
      int iLen;

      iTemp = 1;
      rc = setsockopt(controller[iIndex].socket, IPPROTO_TCP, TCP_NODELAY, (const char*)&iTemp, sizeof(iTemp));
      if (rc == SOCKET_ERROR)
      {
#ifdef DMC_DEBUG
         DMCTrace("Could not set socket option for TCP_NODELAY. Return code <%ld>.\n", errno);
#endif
      }

      iTemp = 0;
      iLen = sizeof(iTemp);
      rc = getsockopt(controller[iIndex].socket, IPPROTO_TCP, TCP_NODELAY, (char*)&iTemp, &iLen);
      if (rc == SOCKET_ERROR)
      {
#ifdef DMC_DEBUG
         DMCTrace("Could not get socket option for TCP_NODELAY. Return code <%ld>.\n", errno);
#endif
      }

      if (iTemp != 1)
      {
#ifdef DMC_DEBUG
         DMCTrace("TCP_NODELAY option was not set.\n", errno);
#endif
      }
   }

   SockAddressS.sin_family = AF_INET;
   SockAddressS.sin_port = htons(GALIL_TCPIP_PORT);
   SockAddressS.sin_addr.s_addr = inet_addr((char *)controller[iIndex].controllerinfo.hardwareinfo.socketinfo.szIPAddress);

   /* Connect to the host address */
   rc = connect(controller[iIndex].socket, (struct sockaddr *)&SockAddressS,
      sizeof(SockAddressS));

   if (rc == SOCKET_ERROR)
   {
#ifdef DMC_DEBUG
      DMCTrace("Could not connect to host address. Return code <%ld>.\n", errno);
#endif
      close(controller[iIndex].socket);
      return DMCERROR_DRIVER;
   }

   /* Open a socket for multi-cast communciations */
   controller[iIndex].socketMulticast = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
   if (controller[iIndex].socketMulticast == SOCKET_ERROR)
   {
#ifdef DMC_DEBUG
      DMCTrace("Could not open a multicast socket. Return code <%ld>.\n", errno);
#endif
      controller[iIndex].socketMulticast = 0;
      return DMCERROR_DRIVER;
   }

   SockAddressC.sin_family = AF_INET;                           
   SockAddressC.sin_port = 0;
   SockAddressC.sin_addr.s_addr = INADDR_ANY;

   // Bind to the socket - needed for 2 way communication (peer-to-peer) and to use setsockopt
   rc = bind(controller[iIndex].socketMulticast,
      (struct sockaddr *)&SockAddressC,
      sizeof(SockAddressC));
   if (rc == SOCKET_ERROR)
   {
#ifdef DMC_DEBUG
      DMCTrace("Could not bind to multicast socket. Return code <%ld>.\n", errno);
#endif
      close(controller[iIndex].socketMulticast);
      return DMCERROR_DRIVER;
   }

   rc = getsockopt(controller[iIndex].socketMulticast, IPPROTO_IP, IP_MULTICAST_TTL, (char *)&TTL, &cbTTL);
   if (rc != SOCKET_ERROR)
   {
      if (TTL != 1)
      {
         TTL = 1;
         rc = setsockopt(controller[iIndex].socketMulticast, IPPROTO_IP, IP_MULTICAST_TTL, (char *)&TTL, sizeof(TTL));
         if (rc == SOCKET_ERROR)
         {
#ifdef DMC_DEBUG
            DMCTrace("Could not set socket option for IP_MULTICAST_TTL Return code <%ld>.\n", errno);
#endif
         }
      }
   }

   // Join the multi-cast group
   mreq.imr_multiaddr.s_addr = inet_addr(MULTICAST_IPADDRESS);
   mreq.imr_interface.s_addr = INADDR_ANY;

   rc = setsockopt(controller[iIndex].socketMulticast, IPPROTO_IP, IP_ADD_MEMBERSHIP,
      (char*)&mreq, sizeof(mreq));
   if (rc == SOCKET_ERROR)
   {
#ifdef DMC_DEBUG
      DMCTrace("Could not join to multicast group. Return code <%ld>.\n", errno);
#endif
      close(controller[iIndex].socketMulticast);
      return DMCERROR_DRIVER;
   }

   return 0L;
}

long CloseSocket(int iIndex)
{
   shutdown(controller[iIndex].socket, 2);
   close(controller[iIndex].socket);

   return 0L;
}

long ETHERNETReadData(int iIndex, PCHAR pchResponse, ULONG cbResponse, PULONG pulBytesRead)
{
   LONG  rc;
   CHAR  szBuffer[RESPONSE_BUFFER_SIZE];
   ULONG ulBytesRead = 0L;

   *pulBytesRead = 0L;

   memset(szBuffer, '\0', sizeof(szBuffer));

   if (controller[iIndex].fReadMulticast)
      rc = ReadSocketMulticast(iIndex, szBuffer, sizeof(szBuffer), &ulBytesRead);
   else
      rc = ReadSocket(iIndex, szBuffer, sizeof(szBuffer), &ulBytesRead); 

   if (rc == 0L && ulBytesRead > 0L)  /* No problem */
   {
      ULONG i;

      for (i = 0; i < ulBytesRead; i++)
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

   *pulBytesRead = ulBytesRead;

   return rc;
}

long ETHERNETWriteData(int iIndex, PSZ pchCommand, ULONG cbCommand, PULONG pulBytesWritten)
{
   LONG   rc;

   *pulBytesWritten = 0L;

   /* Is this a multi-cast command? */
   if (*pchCommand == '!')
   {
      controller[iIndex].fReadMulticast = 1;
      rc = WriteSocketMulticast(iIndex, pchCommand, cbCommand, pulBytesWritten);
   }
   else
   {
      controller[iIndex].fReadMulticast = 0;
      rc = WriteSocket(iIndex, pchCommand, cbCommand, pulBytesWritten);
   }

   if (rc)
      controller[iIndex].fReadMulticast = 0;

   return rc;
}

int ETHERNETCharAvailableInput(int iIndex)
{
   return 1;
}

long ETHERNETClear(int iIndex)
{
   LONG   rc;

   rc = send(controller[iIndex].socket, "\\", 1, MSG_OOB);

   /* Probem? */
   if (rc == SOCKET_ERROR)           
   {
#ifdef DMC_DEBUG
      DMCTrace("   Write error. Error code <%d>.\n", errno);
#endif
      if (errno ==ECONNRESET)
         rc = DMCERROR_DEVICE_DISCONNECTED;
      else
         rc = DMCERROR_DRIVER;
   }
   else
      rc = DMCNOERROR;

   return rc;
}


long ReadSocket(int iIndex, PCHAR pchResponse, ULONG cbResponse, PULONG pulBytesRead)
{
   long   rc = 0L;
   long   readrc;
   fd_set ReadSet;
   int done;

   struct timeval Timeout;

   Timeout.tv_sec = 0;
   Timeout.tv_usec = 0;

   FD_ZERO(&ReadSet);
   FD_SET(controller[iIndex].socket, &ReadSet);

   /* Is available data? */
   /* restart select system call if interrupted by a signal*/
   done = 0;
   while (done == 0)
   	{
   	readrc = select(controller[iIndex].socket + 1, &ReadSet, NULL, NULL, &Timeout);
	if (readrc == SOCKET_ERROR)
		done=(errno==EINTR) ? 0 : 1;
	else
		done = 1;
	}
   if (readrc == SOCKET_ERROR)
   {
#ifdef DMC_DEBUG
      DMCTrace("   Read error (select). Error code <%d>.\n", errno);
#endif
      if (errno == ECONNRESET)
         rc = DMCERROR_DEVICE_DISCONNECTED;
      else
         rc = DMCERROR_DRIVER;
      return rc;
   }
   else
   {
      if (!readrc)
         return 0;
      if (!(FD_ISSET(controller[iIndex].socket, &ReadSet)))
         return 0;
   }

   readrc = recv(controller[iIndex].socket, pchResponse, (int)cbResponse, 0);

   /* Probem? */
   if (readrc == SOCKET_ERROR)           
   {
#ifdef DMC_DEBUG
      DMCTrace("   Read error. Error code <%d>.\n", errno);
#endif
      if (errno == ECONNRESET)
         rc = DMCERROR_DEVICE_DISCONNECTED;
      else
         rc = DMCERROR_DRIVER;
   }
   else
      *pulBytesRead = readrc;

   return rc;
}

long ReadSocketMulticast(int iIndex, PCHAR pchResponse, ULONG cbResponse, PULONG pulBytesRead)
{
   long     rc = 0L;
   long     readrc;
   int      iLength = sizeof(SOCKADDR_IN);
   fd_set   ReadSet;
   ULONG    ulTotalBytes = 0;   
   int done;

   struct timeval Timeout;

   SOCKADDR_IN SockAddressS;

   Timeout.tv_sec = 0;
   Timeout.tv_usec = 0;

   FD_ZERO((fd_set FAR*)&ReadSet);
   FD_SET(controller[iIndex].socketMulticast, (fd_set FAR*)&ReadSet);

   /* Is available data? */
   while (1)
   {
      /*restart select system call if interrupted by a signal*/
      done = 0;
      while (done == 0)
	{
	readrc = select(0, &ReadSet, NULL, NULL, &Timeout);
	if (readrc == SOCKET_ERROR)
			done=(errno==EINTR) ? 0 : 1;
		else
			done = 1;
	}
      if (readrc == SOCKET_ERROR)
      {
#ifdef DMC_DEBUG
         DMCTrace("   Read error (select). Error code <%d>.\n", errno);
#endif
         if (errno == ECONNRESET)
            rc = DMCERROR_DEVICE_DISCONNECTED;
         else
            rc = DMCERROR_DRIVER;
         break;
      }
      else
      {
         if (!(FD_ISSET(controller[iIndex].socketMulticast, (fd_set FAR*)&ReadSet)))
            break;
      }

      readrc = recvfrom(controller[iIndex].socketMulticast,
         pchResponse + ulTotalBytes, (int)(cbResponse - ulTotalBytes), 0,
         (struct sockaddr FAR *)&SockAddressS, &iLength);

      /* Probem? */
      if (readrc == SOCKET_ERROR)           
      {
#ifdef DMC_DEBUG
         DMCTrace("   Read error. Error code <%d>.\n", errno);
#endif
         if (errno == ECONNRESET)
            rc = DMCERROR_DEVICE_DISCONNECTED;
         else
            rc = DMCERROR_DRIVER;
         break;
      }

      ulTotalBytes += (ULONG)readrc;
      if (ulTotalBytes >= cbResponse)
         break;
   }

   *pulBytesRead = ulTotalBytes;

   return rc;
}

long WriteSocket(int iIndex, PCHAR pchCommand, ULONG cbCommand, PULONG pulBytesWritten)
{
   long   rc = 0L;
   long   writerc;
   ULONG  ulBytesToSend = cbCommand;
   ULONG  ulLength = MIN(cbCommand, controller[iIndex].ulMessageSize);
   ULONG  ulBytesWritten = 0L;

   while (ulBytesToSend)
   {
      writerc = send(controller[iIndex].socket, pchCommand, (int)ulLength, 0);

      /* Probem? */
      if (writerc == SOCKET_ERROR)           
      {
#ifdef DMC_DEBUG
         DMCTrace("   Write error. Error code <%d>.\n", errno);
#endif
         if (errno == ECONNRESET)
            rc = DMCERROR_DEVICE_DISCONNECTED;
         else
            rc = DMCERROR_DRIVER;
         break;
      }
      else
         ulBytesWritten = writerc;

      *pulBytesWritten += ulBytesWritten;

      ulBytesToSend -= ulLength;
      pchCommand += ulLength;

      if (ulBytesToSend < controller[iIndex].ulMessageSize)
         ulLength = ulBytesToSend;
   }

#ifdef DMC_DEBUG
   DMCTrace("   Bytes written <%ld>.\n", *pulBytesWritten);
#endif
   
   return rc;
}

long WriteSocketMulticast(int iIndex, PCHAR pchCommand, ULONG cbCommand, PULONG pulBytesWritten)
{
   long   rc = 0L;
   long   writerc;
   ULONG  ulBytesToSend = cbCommand;
   ULONG  ulLength = MIN(cbCommand, controller[iIndex].ulMessageSize);
   ULONG  ulBytesWritten = 0L;

   SOCKADDR_IN  SockAddressS;

   SockAddressS.sin_family = AF_INET;
   SockAddressS.sin_port = 0;
   SockAddressS.sin_addr.s_addr = inet_addr((char *)MULTICAST_IPADDRESS);

   while (ulBytesToSend)
   {
      writerc = sendto(controller[iIndex].socketMulticast, pchCommand, (int)ulLength, 0,
         (struct sockaddr *)&SockAddressS, sizeof(SockAddressS));

      /* Probem? */
      if (writerc == SOCKET_ERROR)           
      {
#ifdef DMC_DEBUG
         DMCTrace("   Write error. Error code <%d>.\n", errno);
#endif
         if (errno == ECONNRESET)
            rc = DMCERROR_DEVICE_DISCONNECTED;
         else
            rc = DMCERROR_DRIVER;
         break;
      }
      else
         ulBytesWritten = writerc;

      *pulBytesWritten += ulBytesWritten;

      ulBytesToSend -= ulLength;
      pchCommand += ulLength;

      if (ulBytesToSend < controller[iIndex].ulMessageSize)
         ulLength = ulBytesToSend;
   }

#ifdef DMC_DEBUG
   DMCTrace("   Bytes written <%ld>.\n", *pulBytesWritten);
#endif
   
   return rc;
}

LONG FAR GALILCALL DMCAssignIPAddress(PCONTROLLERINFO pcontrollerinfo, int fVerbose)
{
   long        rc = 0L;
   short       ServerPort = 67;
   short       ClientPort = 68;
   int         nLen;
   char        szBuf[1024];
   ULONG       ulIPAddress;
   USHORT      usModel = 0;
   USHORT      usSerialNumber = 0;
   DWORD       ulCurrentTime, ulTimeoutTime;
   int         bControllerFound = FALSE;
   int         bConfigured = FALSE;
   int         bOptVal = TRUE;
   int         bError = FALSE;
   SOCKET      sock;
   SOCKADDR_IN saServer;
   SOCKADDR_IN saTemp;
   SOCKADDR_IN saClient;

   if ((ulIPAddress = inet_addr(pcontrollerinfo->hardwareinfo.socketinfo.szIPAddress)) == INADDR_NONE)
   {
#ifdef DMC_DEBUG
      DMCTrace("IP Address is not valid.\n");
#endif
      if (fVerbose)
         printf("IP Address is not valid.\n");
      return DMCERROR_ARGUMENT;
   }

   /* Open a socket */
   sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
   if (sock == SOCKET_ERROR)
   {
#ifdef DMC_DEBUG
      DMCTrace("Could not open a socket. Return code <%ld>.\n", errno);
#endif
      if (fVerbose)
         printf("Could not open a socket. Return code <%ld>.\n", errno);
      return DMCERROR_DRIVER;
   }

   saServer.sin_family = AF_INET;
   saServer.sin_port = htons(ServerPort);
   saServer.sin_addr.s_addr = INADDR_ANY;

   /* Bind to the socket - needed for 2 way communication (peer-to-peer) */
   rc = bind(sock, (struct sockaddr *)&saServer,
      sizeof(saServer));
   if (rc == SOCKET_ERROR)
   {
#ifdef DMC_DEBUG
      DMCTrace("Could not bind to a Windows socket. Return code <%ld>.\n", errno);
#endif
      if (fVerbose)
         printf("Could not bind to a Windows socket. Return code <%ld>.\n", errno);
      close(sock);
      return DMCERROR_DRIVER;
   }

   rc = setsockopt(sock, SOL_SOCKET, SO_BROADCAST, (char FAR *)&bOptVal, sizeof(int));
   if (rc == SOCKET_ERROR)
   {
#ifdef DMC_DEBUG
      DMCTrace("Could not set socket options. Return code <%ld>.\n", errno);
#endif
      if (fVerbose)
         printf("Could not set socket options. Return code <%ld>.\n", errno);
      close(sock);
      return DMCERROR_DRIVER;
   }

   saClient.sin_family = AF_INET;
   saClient.sin_port = htons(ClientPort);
   saClient.sin_addr.s_addr = INADDR_BROADCAST;

   if (fVerbose)
      printf("Searching for Galil Ethernet controllers...\n");

   ulTimeoutTime = DMCGetTime() + 15000L;
   while (1)
   {
      ulCurrentTime = DMCGetTime();
      if (ulCurrentTime > ulTimeoutTime)
         break;

      if (!WillReadBlock(sock))
      {
         memset(szBuf, 0, sizeof(szBuf));
         nLen = sizeof(saTemp);
         rc = recvfrom(sock, szBuf, sizeof(szBuf), 0,
            (struct sockaddr*)&saTemp, &nLen);
         if (rc == SOCKET_ERROR)
         {
#ifdef DMC_DEBUG
            DMCTrace("Error returned from \"recvfrom\". Return code <%ld>.\n", errno);
            DMCTrace("An error occurred attempting to read a BOOTP packet. Return code %ld.\n", errno);
#endif
            if (fVerbose)           
            {
               printf("Error returned from \"recvfrom\". Return code <%ld>.\n", errno);
               printf("An error occurred attempting to read a BOOTP packet. Return code %ld.\n", errno);
            }
            bError = TRUE;
            break;
         }

         if (0 < (nLen = FillBootpRep((unsigned char *)szBuf, rc /* contains bytes read */, ulIPAddress, &usModel, &usSerialNumber)))
         {
            bControllerFound = TRUE;
            if (usModel)
            {
               CHAR   szModel[32];

               if (usModel == MODEL_2100)
                  strcpy(szModel, DMC2100);
               else if (usModel == MODEL_1415)
                  strcpy(szModel, DMC1415);
               else
                  strcpy(szModel, "unknown");
#ifdef DMC_DEBUG
               DMCTrace("An Ethernet controller (model %s) with serial number %u was found.\n",
                  szModel, usSerialNumber);
#endif
               if (fVerbose)
                  printf("An Ethernet controller (model %s) with serial number %u was found.\n",
                     szModel, usSerialNumber);
            }
            else
            {
#ifdef DMC_DEBUG
               DMCTrace("An Ethernet controller with serial number %u was found.\n",
                  usSerialNumber);
#endif
               if (fVerbose)
                  printf("An Ethernet controller with serial number %u was found.\n",
                     usSerialNumber);
            }

            /* Assign IP address then cancel search */
            if (usSerialNumber == pcontrollerinfo->ulSerialNumber)
            {
               rc = sendto(sock, szBuf, nLen, 0,
                  (struct sockaddr*)&saClient, sizeof(struct sockaddr));   
               if (rc == SOCKET_ERROR)
               {
#ifdef DMC_DEBUG
                  DMCTrace("Error returned from \"sendto\". Return code <%ld>.\n", errno);
                  DMCTrace("An error occurred attempting to write a BOOTP packet. Return code %ld.\n", rc);
#endif
                  if (fVerbose)
                  {
                     printf("Error returned from \"sendto\". Return code <%ld>.\n", errno);
                     printf("An error occurred attempting to write a BOOTP packet. Return code %ld.\n", rc);
                  }
                  bError = TRUE;
               }
               else
               {
                  bConfigured = TRUE;
#ifdef DMC_DEBUG
                  DMCTrace("Ethernet controller serial number %u was configured with IP Address %s.\n",
                     usSerialNumber, pcontrollerinfo->hardwareinfo.socketinfo.szIPAddress);
#endif
                  if (fVerbose)
                  {
                     printf("Ethernet controller serial number %u was configured with IP Address %s.\n",
                        usSerialNumber, pcontrollerinfo->hardwareinfo.socketinfo.szIPAddress);
                  }
               }
               break;
            }
         }
      }
   }

   close(sock);

   if (fVerbose && !bConfigured)
      printf("No Galil Ethernet controllers were configured.\n");

   return DMCNOERROR;
}

int FillBootpRep(unsigned char * data, int length, ULONG ulIPAddress, PUSHORT pusModel, PUSHORT pusSerialNumber)
{
   unsigned char chByte1, chByte2;

   if (length < 300)
   {
      /* Packet too short */
      return(-2);
   }

   /* bootp reply */
   data[0] = 2;

   *pusModel = MODEL_UNKNOWN;

   if (0 != ntohl(*(long*)&data[12]))  /* client ip */
      return(-1);

   if (0 != ntohl(*(long*)&data[16]))  /* your ip */
      return(-1);
   else
      *(long*)&data[16] = ulIPAddress;

   if (0 != ntohl(*(long*)&data[20]))  /* server ip */
      return(-1);

   if (0 != ntohl(*(long*)&data[24]))  /* gateway ip */
      return(-1);

   if (0 != strlen((char*)&data[44]))  /* server hostname */
      return(-1);

   if (0 != strlen((char*)&data[108])) /* boot filename */
      return(-1);

   if (0x63825363 != ntohl(*(long*)&data[236])) /* vendor specific area "magic cookie" */
      return(-1);

   /* check for Galil vendor id */
   if (data[28] != 0x00)
      return(-3);
   if (data[29] != 0x50)
      return(-3);
   if (data[30] != 0x4C)
      return(-3);

   /* model */
   if (data[31] == 0)
      *pusModel = MODEL_2100;
   else if (data[31] == 8)
      *pusModel = MODEL_1415;

   /* serial number */
   chByte1 = data[32];
   chByte2 = data[33];

   *pusSerialNumber = MAKEWORD(chByte2, chByte1);

   return(length);
}

int WillReadBlock(SOCKET sock)
{
   int      rc = 0;
   int      fRet = TRUE;
   fd_set   ReadSet;
   struct   timeval Timeout;
   int done;
 
   /* Immediately return from select() */
   Timeout.tv_sec = 0;
   Timeout.tv_usec = 0;
   /* Set out socket descriptor 'bit' in the read mask */
   FD_ZERO(&ReadSet);
   FD_SET(sock, &ReadSet);

   /* Check if "read" will block */
   /*restart select system call if interrupted by a signal*/
   done = 0;
   while (done == 0)
   	{
   	rc = select(sock + 1, &ReadSet, NULL, NULL, &Timeout);
	if (rc == SOCKET_ERROR)
		done=(errno==EINTR) ? 0 : 1;
	else
		done = 1;
	}
	
   if (rc == SOCKET_ERROR)
   {
      rc = errno;
   }
   else
   {
      if (rc)
      {
         if (FD_ISSET(sock, &ReadSet))
            fRet = FALSE; /* Read operation will succeed without blocking */
      }
   }

   return fRet;
}
#endif /* DMC_ETHERNET */
