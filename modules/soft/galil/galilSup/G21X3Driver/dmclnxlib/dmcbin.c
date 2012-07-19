#include "dmclnx.h"

#ifdef DMC_BINARYCOMMANDS

LONG FAR GALILCALL DMCBinaryCommand(HANDLEDMC hdmc, PBYTE pbCommand, ULONG ulCommandLength,
	PCHAR pchResponse, ULONG cbResponse)
{
	LONG  rc = 0L;
	LONG  saverc = 0L;
	int   iIndex;
	ULONG ulCurrentTime;
	ULONG ulTimeout;
	ULONG ulBytesRead;
	ULONG ulBytesWritten;
	CHAR szTempResponse[RESPONSE_BUFFER_SIZE];

#ifdef DMC_DEBUG
	DMCTrace("Entering DMCBinaryCommand.\n");
#endif

	*pchResponse = '\0';

	iIndex = Handle2Index(hdmc);
	if (iIndex == -1)
	{
#ifdef DMC_DEBUG
		DMCTrace("  Invalid handle.\n");
		DMCTrace("Leaving DMCBinaryCommand.\n");
#endif
		return DMCERROR_HANDLE;
	}

	controller[iIndex].fBinaryCommand = 1;
#ifdef DMC_ETHERNET
   controller[iIndex].fReadMulticast = 0;
#endif

	/* Clear any remaining characters from previous command */
	if (CharAvailableInput(iIndex))
	{
		ReadData(iIndex, szTempResponse, sizeof(szTempResponse), &ulBytesRead);
		memset(controller[iIndex].szResponse, '\0', sizeof(controller[iIndex].szResponse));
	}

	rc = WriteData(iIndex, (PCHAR)pbCommand, ulCommandLength, &ulBytesWritten);
	if (rc || ulBytesWritten == 0)
	{
#ifdef DMC_DEBUG
		DMCTrace("  A timeout ocurred waiting for the controller to recieve the command.\n");
		DMCTrace("Leaving DMCBinaryCommand.\n");
#endif
		controller[iIndex].fBinaryCommand = 0;
		return rc;
	}

	rc = 0L;

	memset(controller[iIndex].szResponse, '\0',  sizeof(controller[iIndex].szResponse));

	if (controller[iIndex].controllerinfo.ulTimeout)
	{
		ulCurrentTime = DMCGetTime();
		ulTimeout = ulCurrentTime + controller[iIndex].controllerinfo.ulTimeout;
	}

	/* Wait until a response is received or a timeout occurs */
	while (!CharAvailableInput(iIndex) && !rc)
	{
		if (controller[iIndex].controllerinfo.ulTimeout)
		{
			ulCurrentTime = DMCGetTime();
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
		DMCTrace("Leaving DMCBinaryCommand.\n");
#endif
		controller[iIndex].fBinaryCommand = 0;
		return rc;
	}

	if (controller[iIndex].controllerinfo.ulTimeout)
	{
		ulCurrentTime = DMCGetTime();
		ulTimeout = ulCurrentTime + controller[iIndex].controllerinfo.ulTimeout;
	}

	/* Receive the response */
	do
	{
		rc = ReadData(iIndex, pchResponse, cbResponse, &ulBytesRead);
		if (rc)
			saverc = rc;
		if (strchr(pchResponse, ':'))
		{
			saverc = 0L;
			rc = 0L;
			break;
		}
		if (strchr(pchResponse, '?'))
		{
			saverc = 0L;
			rc = DMCERROR_COMMAND;
			break;
		}
		if (rc == 0L || rc == DMCERROR_BUFFERFULL)
		{
			if (!ulBytesRead)
			{
				/* Do not wait for a response because time-out is zero */
				if (!controller[iIndex].controllerinfo.ulTimeout)
				{
					saverc = 0L;
					rc = 0L;
					break;
				}
				/* Check clock */
				ulCurrentTime = DMCGetTime();
				if (ulCurrentTime > ulTimeout)
					rc = DMCERROR_TIMEOUT;
			}
			else
			{
				/* Reset clock */
				if (controller[iIndex].controllerinfo.ulTimeout)
				{
					ulCurrentTime = DMCGetTime();
					ulTimeout = ulCurrentTime + controller[iIndex].controllerinfo.ulTimeout;
				}
			}
		}
	} while ((rc == 0L || rc == DMCERROR_BUFFERFULL));

	if (saverc)
		rc = saverc;

	controller[iIndex].fBinaryCommand = 0;

#ifdef DMC_DEBUG
	DMCTrace("Leaving DMCBinaryCommand.\n");
#endif
	return rc;
}

#define TEMP_COMMAND_BUFFER_SIZE    256
#define TEMP_RESPONSE_BUFFER_SIZE   512

LONG FAR GALILCALL DMCSendBinaryFile(HANDLEDMC hdmc, PSZ pszFileName)
{
	LONG  rc = 0L;

	int   i;
	int   done = 0;
	int   axis_counts = 0;
	int   data_size = 0;
	int   quit = 0;
	int   input_char = 0;
	int   number_format = 0;
	int   command_length = 0;
	char  response_string[TEMP_RESPONSE_BUFFER_SIZE];
	FILE  *file = 0;
	unsigned char command_buffer[TEMP_COMMAND_BUFFER_SIZE];
	unsigned char axis_bits = 0;

#ifdef DMC_DEBUG
	DMCTrace("Entering DMCSendBinaryFile.\n");
#endif

	if (Handle2Index(hdmc) == -1)
	{
#ifdef DMC_DEBUG
		DMCTrace("  Invalid handle.\n");
		DMCTrace("Leaving DMCSendBinaryFile.\n");
#endif
		return DMCERROR_HANDLE;
	}

	if ((file = fopen(pszFileName, "rb")) == NULL)
	{
#ifdef DMC_DEBUG
		DMCTrace("   File error.\n");
		DMCTrace("Leaving DMCSendBinaryFile.\n");
#endif
		return DMCERROR_FILE;
	}

	command_buffer[0] = 0;

	do
	{
		if (command_buffer[0])
		{
			response_string[0] = 0;
			rc = DMCBinaryCommand(hdmc, command_buffer, command_length,
				response_string, sizeof(response_string));
#ifdef DMC_DEBUG
			DMCTrace("   Response = %s.\n", response_string);
#endif
			if (rc == DMCERROR_BUFFERFULL)
				rc = 0L;
			else if (rc == DMCNOERROR)
			{
				if (strchr(response_string, '?'))
					rc = DMCERROR_COMMAND;
			}
			if (rc)
				break;
		}

		if (feof(file))
			break;
		else if (!input_char)
		{
			input_char = fgetc(file);
			if (feof(file))
				break;
		}

		if (input_char & BINARY_COMMAND_BIT)
		{
			command_buffer[0] = (UCHAR)input_char;
			input_char = fgetc(file);
			if (feof(file))
			{
				fclose(file);
#ifdef DMC_DEBUG
				DMCTrace("   End of file reached before end of binary command.\n");
				DMCTrace("Leaving DMCSendBinaryFile.\n");
#endif
				return DMCERROR_FILE;
			}
			command_buffer[1] = (UCHAR)input_char;
         input_char = fgetc(file);
			if (feof(file))
			{
				fclose(file);
#ifdef DMC_DEBUG
				DMCTrace("   End of file reached before end of binary command.\n");
				DMCTrace("Leaving DMCSendBinaryFile.\n");
#endif
				return DMCERROR_FILE;
			}
			command_buffer[2] = (UCHAR)input_char;
         input_char = fgetc(file);
			if (feof(file))
			{
				fclose(file);
#ifdef DMC_DEBUG
				DMCTrace("   End of file reached before end of binary command.\n");
				DMCTrace("Leaving DMCSendBinaryFile.\n");
#endif
				return DMCERROR_FILE;
			}
			command_buffer[3] = (UCHAR)input_char;
			command_length = 4;

			number_format = command_buffer[1];
			axis_bits = command_buffer[3];
			axis_counts = Total_binary_axis_counts(axis_bits);
			data_size = number_format * axis_counts;
			if (number_format > 0)
			{
				for (i = 0; i < data_size; i++)
				{
            	input_char = fgetc(file);
					if (feof(file))
					{
						fclose(file);
#ifdef DMC_DEBUG
						DMCTrace("   End of file reached before end of binary command.\n");
						DMCTrace("Leaving DMCSendBinaryFile.\n");
#endif
						return DMCERROR_FILE;
					}
					command_buffer[command_length++] = (UCHAR)input_char;
				}
			}
			command_buffer[command_length] = 0;
			input_char = 0;
		}
		else
		{
			quit = 0;
			command_buffer[command_length++] = (UCHAR)input_char;
			do
			{
				input_char = fgetc(file);
				if (feof(file))
					quit = 1;
				else if (input_char & BINARY_COMMAND_BIT)
					quit = 1;
				else if (input_char <= 26)
				{
					input_char = 0;
					quit = 1;
				}
				else
					command_buffer[command_length++] = (UCHAR)input_char;
			} while (!quit);
			command_buffer[command_length++] = 13;
			command_buffer[command_length] = 0;
			if (!strncmp((char*)command_buffer, "REM", 3))  //CC
			{
				command_buffer[0] = 0;
				command_length = 0;
			}
		}
	} while (!done);

	fclose(file);

#ifdef DMC_DEBUG
	DMCTrace("Leaving DMCSendBinaryFile.\n");
#endif
	return rc;
}

int Total_binary_axis_counts(unsigned char axis_bits)
{
	int i;
	int axis_counts = 0;
	unsigned char mask = 1;

	for (i = 0; i < 8; i++)
	{
		if (axis_bits & mask)
			axis_counts++;
		mask *= 2;
	}

	return axis_counts;
}

LONG FAR GALILCALL DMCCommand_BinaryToAscii(HANDLEDMC hdmc, PBYTE pbBinaryCommand,
	ULONG ulBinaryCommandLength, PSZ pszAsciiResult, ULONG cbAsciiResult,
	ULONG FAR *pulAsciiResultLength)
{
	LONG  rc = 0L;

	hdmc = hdmc;

#ifdef DMC_DEBUG
	DMCTrace("Entering DMCCommand_BinaryToAscii.\n");
#endif

/* We do not need a HANDLEDMC for this function
	if (Handle2Index(hdmc) == -1)
	{
#ifdef DMC_DEBUG
		DMCTrace("  Invalid handle.\n");
		DMCTrace("Leaving DMCCommand_BinaryToAscii.\n");
#endif
		return DMCERROR_HANDLE;
	}
*/

	rc = CommandBinaryToAscii(pbBinaryCommand, ulBinaryCommandLength,
		pszAsciiResult, cbAsciiResult, (long*)pulAsciiResultLength);

#ifdef DMC_DEBUG
	DMCTrace("Leaving DMCCommand_BinaryToAscii.\n");
#endif
	return rc;
}

LONG FAR GALILCALL DMCCommand_AsciiToBinary(HANDLEDMC hdmc, PSZ pszAsciiCommand,
	ULONG ulAsciiCommandLength, PBYTE pbBinaryResult, ULONG cbBinaryResult,
	ULONG FAR *pulBinaryResultLength)
{
	LONG  rc = 0L;

	hdmc = hdmc;

#ifdef DMC_DEBUG
	DMCTrace("Entering DMCCommand_AsciiToBinary.\n");
#endif

/* We do not need a HANDLEDMC for this function
	if (Handle2Index(hdmc) == -1)
	{
#ifdef DMC_DEBUG
		DMCTrace("  Invalid handle.\n");
		DMCTrace("Leaving DMCCommand_AsciiToBinary.\n");
#endif
		return DMCERROR_HANDLE;
	}
*/

	rc = CommandAsciiToBinary(pszAsciiCommand, ulAsciiCommandLength,
		pbBinaryResult, cbBinaryResult, (long*)pulBinaryResultLength);

#ifdef DMC_DEBUG
	DMCTrace("Leaving DMCCommand_AsciiToBinary.\n");
#endif
	return rc;
}

LONG FAR GALILCALL DMCFile_AsciiToBinary(HANDLEDMC hdmc, PSZ pszInputFileName, PSZ pszOutputFileName)
{
	LONG  rc = 0L;

	hdmc = hdmc;

#ifdef DMC_DEBUG
	DMCTrace("Entering DMCFile_AsciiToBinary.\n");
#endif

/* We do not need a HANDLEDMC for this function
	if (Handle2Index(hdmc) == -1)
	{
#ifdef DMC_DEBUG
		DMCTrace("  Invalid handle.\n");
		DMCTrace("Leaving DMCFile_AsciiToBinary.\n");
#endif
		return DMCERROR_HANDLE;
	}
*/

	rc = FileAsciiToBinary(pszInputFileName, pszOutputFileName);

#ifdef DMC_DEBUG
	DMCTrace("Leaving DMCFile_AsciiToBinary.\n");
#endif
	return rc;
}

LONG FAR GALILCALL DMCFile_BinaryToAscii(HANDLEDMC hdmc, PSZ pszInputFileName, PSZ pszOutputFileName)
{
	LONG  rc = 0L;

	hdmc = hdmc;

#ifdef DMC_DEBUG
	DMCTrace("Entering DMCFile_BinaryToAscii.\n");
#endif

/* We do not need a HANDLEDMC for this function
	if (Handle2Index(hdmc) == -1)
	{
#ifdef DMC_DEBUG
		DMCTrace("  Invalid handle.\n");
		DMCTrace("Leaving DMCFile_BinaryToAscii.\n");
#endif
		return DMCERROR_HANDLE;
	}
*/

	rc = FileBinaryToAscii(pszInputFileName, pszOutputFileName);

#ifdef DMC_DEBUG
	DMCTrace("Leaving DMCFile_BinaryToAscii.\n");
#endif
	return rc;
}

LONG FAR GALILCALL DMCReadSpecialConversionFile(HANDLEDMC hdmc, PSZ pszFileName)
{
	LONG  rc = 0L;

#ifdef DMC_DEBUG
	DMCTrace("Entering DMCReadSpecialConversionFile.\n");
#endif

	if (Handle2Index(hdmc) == -1)
	{
#ifdef DMC_DEBUG
		DMCTrace("  Invalid handle.\n");
		DMCTrace("Leaving DMCReadSpecialConversionFile.\n");
#endif
		return DMCERROR_HANDLE;
	}

	rc = ReadSpecialConversionFile(pszFileName);

#ifdef DMC_DEBUG
	DMCTrace("Leaving DMCReadSpecialConversionFile.\n");
#endif
	return rc;
}

#endif /* DMC_BINARYCOMMANDS */
