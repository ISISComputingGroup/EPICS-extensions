#ifndef INCLUDE_INTERRUPTS
#define INCLUDE_INTERRUPTS

//#include <asm/irq.h>

extern LONG BUSGetInterruptStatus(int iIndex, PUSHORT pusStatus);

#define MAX_INTERRUPTS 16

typedef struct _INTQUEUE
{
	int			Count;
	USHORT	usInterrupt[MAX_INTERRUPTS + 1];
} INTQUEUE;

#endif /* INCLUDE_INTERRUPTS */
