#ifndef INCLUDE_SERIAL
#define INCLUDE_SERIAL

#include <sys/types.h>
#include <sys/stat.h>

#include <fcntl.h>
#include <errno.h>
#ifndef _WIN32
#include <unistd.h>
#include <termios.h>
#include <sys/uio.h>
#endif

#define MAX_COMMPORTS   8
#define NO_MORE_DATA    -99

extern long SERIALReadData(int iIndex, PCHAR pchResponse, ULONG cbResponse, PULONG pulBytesRead);
extern long SERIALWriteData(int iIndex, PCHAR pchCommand, ULONG cbCommand, PULONG pulBytesWritten);
extern long SERIALClear(int iIndex);
extern int SERIALCharAvailableInput(int iIndex);

long OpenPort(int iIndex);
long ClosePort(int iIndex);
int ReadyToRead(int iIndex);
int ReadyToWrite(int iIndex);
long ReadAChar(int iIndex, PCHAR pch);
long WriteAChar(int iIndex, CHAR ch);

#endif /* INCLUDE_SERIAL */
