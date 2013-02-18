#include "dmclnx.h"

#ifdef DMC_FIRMWARE

LONG FAR GALILCALL DMCSendFirmwareFile(HANDLEDMC hdmc, PSZ pszFileName)
{
	long	rc = 0L;
	long	lTimeout;
	long	fEndOfFile = 0L;
	int	i;
	int	iIndex;
	int	iBufferSize;
	int	iInput = 0;
	int	fOK = FALSE;
	int	fInTheMonitor = FALSE;
	FILE*	file;
	char	szHeader[32];
	char	szInput[DOWNLOAD_BUFFER_SIZE];
	char	szResponse[64];
	ULONG	ulBytesRead;
	ULONG	ulBytesWritten;
	ULONG	ulCurrentTime;
	ULONG	ulTimeoutTime;

#ifdef DMC_DEBUG
	DMCTrace("Entering DMCSendFirmwareFile.\n");
#endif

	iIndex = Handle2Index(hdmc);
	if (iIndex == -1)
	{
#ifdef DMC_DEBUG
		DMCTrace("  Invalid handle.\n");
		DMCTrace("Leaving DMCSendFirmwareFile.\n");
#endif
		return DMCERROR_HANDLE;
	}

	if (!pszFileName)
	{
#ifdef DMC_DEBUG
		DMCTrace("   File name is NULL.\n");
		DMCTrace("Leaving DMCSendFirmwareFile.\n");
#endif
		return DMCERROR_FILE;
	}

	/* Are we in the monitor (the OS for the controller)? */

	/* Check to see if we are in the monitor */
	rc = DMCWriteData(hdmc, "\r", 1L, &ulBytesWritten);

	ulCurrentTime = DMCGetTime();
	ulTimeoutTime = ulCurrentTime + 5000L;

	rc = 0L;

	fOK = FALSE;
	do
	{
		ulCurrentTime = DMCGetTime();
		if (ulCurrentTime > ulTimeoutTime)
			rc = DMCERROR_TIMEOUT;
		else
		{
			szResponse[0] = '\0';
			ulBytesRead = 0L;
			rc = DMCReadData(hdmc, szResponse, sizeof(szResponse), &ulBytesRead);
			if (ulBytesRead)
			{
				szResponse[(size_t)ulBytesRead] = '\0';
				if (strchr(szResponse, '>'))
				{
					fOK = TRUE;
					fInTheMonitor = TRUE;
				}
				if (strchr(szResponse, ':') || strchr(szResponse, '?'))
				{
					fOK = TRUE;
				}
			}
		}

	} while (rc == 0L && (ulBytesRead || fOK == FALSE)); // Read until time-out or all data is read

	rc = 0L;

	/* Turn echo off */
	if (!fInTheMonitor)
	{
		rc = DMCCommand(hdmc, "EO0\r", szResponse, sizeof(szResponse));
		if (rc)
		{
#ifdef DMC_DEBUG
			DMCTrace("   Could not turn echo off.\n");
			DMCTrace("Leaving DMCSendFirmwareFile.\n");
#endif
			return rc;
		}

		DMCClear(hdmc);
	}

#ifdef DMC_DEBUG
	DMCTrace("   File name <%s>.\n", pszFileName);
#endif

	if ((file = fopen(pszFileName, "r")) == NULL)
	{
#ifdef DMC_DEBUG
		DMCTrace("   File error.\n");
		DMCTrace("Leaving DMCSendFirmwareFile.\n");
#endif
		return DMCERROR_FILE;
	}

	/* Check the file header */
	for (i = 0; i < 16; i++)
	{
		if ((iInput = fgetc(file)) == EOF)
			break;
		szHeader[i] = (char)iInput;
	}
	szHeader[i] = 0;

	if (i < 16 || stricmp(szHeader,"S00600004844521B"))
	{
#ifdef DMC_DEBUG
		DMCTrace("   Invalid firmware file.\n");
		DMCTrace("Leaving DMCSendFirmwareFile.\n");
#endif
		fclose(file);
		return DMCERROR_FILE;
	}

	/* Save the original time-out value */
	lTimeout = controller[iIndex].controllerinfo.ulTimeout;
	/* Decrease the time-out value to 250 milliseconds */
	controller[iIndex].controllerinfo.ulTimeout = 250;

	/* Turn on monitor mode */
	if (!fInTheMonitor)
	{
		rc = TurnOnMonitor(hdmc);
		if (rc)
		{
#ifdef DMC_DEBUG
			DMCTrace("   Could not enter monitor mode.\n");
			DMCTrace("Leaving DMCSendFirmwareFile.\n");
#endif
			controller[iIndex].controllerinfo.ulTimeout = lTimeout;
			fclose(file);
			return DMCERROR_FIRMWARE;
		}
	}

	/* Increase the time-out value to 10 minutes */
	controller[iIndex].controllerinfo.ulTimeout = 600000L;

	/* Turn off test input line */
	rc = DMCWriteData(hdmc, "@\r", 2L, &ulBytesWritten);

	ulCurrentTime = DMCGetTime();
	ulTimeoutTime = ulCurrentTime + 5000L;

	rc = 0L;

	do
	{
		ulCurrentTime = DMCGetTime();
		if (ulCurrentTime > ulTimeoutTime)
			rc = DMCERROR_TIMEOUT;
		else
			rc = DMCReadData(hdmc, szResponse, sizeof(szResponse), &ulBytesRead);
	} while (ulBytesRead == 0L && rc == 0L);

	rc = 0L;

	/* Turn off USB for monitor mode - serial controllers only */
	if (controller[iIndex].controllerinfo.fControllerType == ControllerTypeSerial)
	{
#ifdef DMC_DEBUG
		DMCTrace("   Turning off USB for monitor.\n");
#endif
		rc = DMCWriteData(hdmc, "#\r", 2, &ulBytesWritten);
		Delay(250);

		ulCurrentTime = DMCGetTime();
		ulTimeoutTime = ulCurrentTime + 5000L;

		rc = 0L;

		fOK = FALSE;
		do
		{
			ulCurrentTime = DMCGetTime();
			if (ulCurrentTime > ulTimeoutTime)
				rc = DMCERROR_TIMEOUT;
			else
			{
				szResponse[0] = '\0';
				rc = DMCReadData(hdmc, szResponse, sizeof(szResponse), &ulBytesRead);
				if (ulBytesRead)
				{
					szResponse[(size_t)ulBytesRead] = '\0';
					if (strchr(szResponse, ':') || strchr(szResponse, '?') || strchr(szResponse, '>'))
					{
						fOK = TRUE;
					}
				}
			}
		/* Read until time-out or all data is read */
		} while (rc == 0L && (ulBytesRead || fOK == FALSE));

		rc = 0L;
	}

	/* Send echo off for monitor mode - serial controllers only */
	if (controller[iIndex].controllerinfo.fControllerType == ControllerTypeSerial)
	{
#ifdef DMC_DEBUG
		DMCTrace("   Sending echo off for monitor.\n");
#endif
		rc = DMCWriteData(hdmc, "EO0\r", 4, &ulBytesWritten);
		Delay(250);

		/* Echo off is important, try again if not successful */
		if (rc || ulBytesWritten != 4)
		{
			rc = DMCWriteData(hdmc, "EO0\r", 4, &ulBytesWritten);
			Delay(250);
		}

		ulCurrentTime = DMCGetTime();
		ulTimeoutTime = ulCurrentTime + 5000L;

		rc = 0L;

		fOK = FALSE;
		do
		{
			ulCurrentTime = DMCGetTime();
			if (ulCurrentTime > ulTimeoutTime)
				rc = DMCERROR_TIMEOUT;
			else
			{
				szResponse[0] = '\0';
				rc = DMCReadData(hdmc, szResponse, sizeof(szResponse), &ulBytesRead);
				if (ulBytesRead)
				{
					szResponse[(size_t)ulBytesRead] = '\0';
					if (strchr(szResponse, ':') || strchr(szResponse, '?') || strchr(szResponse, '>'))
					{
						fOK = TRUE;
					}
				}
			}
		/* Read until time-out or all data is read */
		} while (rc == 0L && (ulBytesRead || fOK == FALSE));

		rc = 0L;
	}

#ifdef DMC_DEBUG
	DMCTrace("   Writing firmware header.\n");
#endif

	/* Start the sending the firmware file */
	strcpy(szHeader, "F5AA5,C33C");
	szHeader[10] = CONTROL_M;
	szHeader[11] = 0;

	rc = DMCWriteData(hdmc, szHeader, 11L, &ulBytesWritten);
	if (ulBytesWritten != 11L)
		rc = DMCERROR_FIRMWARE;

	if (controller[iIndex].controllerinfo.fControllerType == ControllerTypeSerial)
		iBufferSize = 1;
	else
		iBufferSize = DOWNLOAD_BUFFER_SIZE - 1;

#ifdef DMC_DEBUG
	DMCTrace("   Writing firmware data.\n");
#endif

	while (!fEndOfFile && !rc)
	{
		iInput = 0;
		memset(szInput, '\0', sizeof(szInput));

		/* Fill the buffer with characters from the firmware file */
		for (i = 0; i < iBufferSize; i++)
		{
			iInput = fgetc(file);
			if (iInput == EOF)
			{
				fEndOfFile = 1L;
				break;
			}
			else
				szInput[i] = (char)iInput;
		}

		/* Send the buffer to the controller */
		if (szInput[0])
		{
/*
#ifdef DMC_DEBUG
			DMCTrace("   Sending character <%02X>\n", (int)szInput[0]);
#endif
*/
			rc = DMCWriteData(hdmc, szInput, strlen(szInput), &ulBytesWritten);
			if (ulBytesWritten == (ULONG)strlen(szInput))
				rc = 0L;
			else
			{
#ifdef DMC_DEBUG
				DMCTrace("   Could not write characters.\n");
#endif
				break;
			}

			/* The controller should not send any response, other than a colon */
			DMCReadData(hdmc, szResponse, sizeof(szResponse) - 1, &ulBytesRead);
			if (ulBytesRead)
			{
				szResponse[(size_t)ulBytesRead] = '\0';
				/* Controller should respond with \r\n: when firmware download is complete */
				if (strcmp(szResponse, "\r\n:") && strcmp(szResponse, "\r\n") && strcmp(szResponse, "\r"))
				{
#ifdef DMC_DEBUG
					DMCTrace("   Controller responds to firmware download with unexpected response <%s>.\n", szResponse);
#endif
					rc = DMCERROR_FIRMWARE;
				}
			}
		}
	}

	if (rc)
	{
#ifdef DMC_DEBUG
		DMCTrace("   Error sending firmware file.\n");
		DMCTrace("Leaving DMCSendFirmwareFile.\n");
#endif
		controller[iIndex].controllerinfo.ulTimeout = lTimeout;
		fclose(file);
		return DMCERROR_FIRMWARE;
	}

	/* No matter whether update is successful or not, send a CR right
		away to avoid infinite waiting. This CR should get a :, or >. */
	iInput = CONTROL_M;
	while (iInput && !rc)
	{
		szInput[0] = (char)iInput;
		rc = DMCWriteData(hdmc, szInput, 1L, &ulBytesWritten);
		if (ulBytesWritten == 1L)
		{
			rc = 0L;
			iInput = 0;
		}
	}

	if (!rc)
		rc = CheckResult(hdmc);

	controller[iIndex].controllerinfo.ulTimeout = lTimeout;
	fclose(file);
#ifdef DMC_DEBUG
	DMCTrace("Leaving DMCSendFirmwareFile.\n");
#endif
	return rc;
}

LONG TurnOnMonitor(HANDLEDMC hdmc)
{
	LONG	rc = 0L;
	int	i;
	char	szCommand[8];
	char	szResponse[32];
	ULONG	ulBytesWritten;

   /* For compiler warning */
	rc = rc;

	/* Clear FIFO first and stop last szCommand */
	DMCClear(hdmc);

	szCommand[0] = CONTROL_M;
	szCommand[1] = 0;

	/* The first CR should get a ':', '?', or '>' */
	rc = DMCCommand(hdmc, szCommand, szResponse, sizeof(szResponse));

	/* The second CR should get a ':', or '>' */
	rc = DMCCommand(hdmc, szCommand, szResponse, sizeof(szResponse));

	/* Are we already in monitor mode? */
	if (strchr(szResponse, '>'))
		return 0L;

	/* Activate operating system monitor */
	szCommand[0] = CONTROL_O;
	szCommand[1] = CONTROL_S;
	szCommand[2] = CONTROL_M;
	szCommand[3] = 0;

	rc = DMCWriteData(hdmc, szCommand, 3L, &ulBytesWritten);
	if (ulBytesWritten != 3L)
		return DMCERROR_COMMAND;

	szCommand[0] = CONTROL_M;
	szCommand[1] = 0;

	/* We should definitely get a '>' now */
	for (i = 0; i < 10; i++)
	{
		rc = DMCCommand(hdmc, szCommand, szResponse, sizeof(szResponse));
		if (strchr(szResponse, '>'))
			return 0L;
	}

	return DMCERROR_FIRMWARE;
}

LONG CheckResult(HANDLEDMC hdmc)
{
	LONG	rc = DMCERROR_FIRMWARE;
	int	i;
	char	szCommand[8];
	char	szResponse[32];
	ULONG	ulBytesRead;

	for (i = 0; i < 10; i++)
	{
		DMCCommand(hdmc, "\r", szResponse, sizeof(szResponse));
		/* OK */
		if (szResponse[0] == ':' || !strncmp(szResponse, "\r\r\n:", 4) || !strncmp(szResponse, "\r\n:", 3))
		{
			rc = 0L;
			break;
		}
		/* Still in monitor - attempt to break out */
		else if (szResponse[0] == '>' || !strncmp(szResponse, "\r\r\n>", 4) || !strncmp(szResponse, "\r\n>", 3))
		{
			DMCCommand(hdmc, "G\r", szResponse, sizeof(szResponse));
			break;
		}
	}

	/* Master reset the controller */
	szCommand[0] = 18;
	szCommand[1] = 19;
	szCommand[2] = '\r';
	szCommand[3] = '\0';

	DMCCommand(hdmc, szCommand, szResponse, sizeof(szResponse));

	/* Turn echo off */
	DMCCommand(hdmc, "EO0\r", szResponse, sizeof(szResponse));

	Delay(250);

	/* If there is anything left in the FIFO, read it out */
	DMCReadData(hdmc, szResponse, sizeof(szResponse) - 1, &ulBytesRead);

	return rc;
}

VOID Delay(ULONG ulDelay)
{
	ULONG	ulCurrentTime;
	ULONG	ulTimeoutTime;

	ulCurrentTime = DMCGetTime();
	ulTimeoutTime = ulCurrentTime + ulDelay;

	while (ulCurrentTime < ulTimeoutTime)
		ulCurrentTime = DMCGetTime();
}

#endif /* DMC_FIRMWARE */
