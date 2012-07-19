#ifndef INCLUDE_ISABUS
#define INCLUDE_ISABUS

extern long ISABUSReadData(int iIndex, PCHAR pchResponse, ULONG cbResponse, PULONG pulBytesRead);
extern long ISABUSWriteData(int iIndex, PCHAR pchCommand, ULONG cbCommand, PULONG pulBytesWritten);
extern long ISABUSClear(int iIndex);
extern int ISABUSCharAvailableInput(int iIndex);

#endif /* INCLUDE_ISABUS */
