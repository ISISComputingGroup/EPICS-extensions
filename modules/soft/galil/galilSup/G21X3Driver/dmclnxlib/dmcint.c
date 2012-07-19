#include "dmclnx.h"
#include "../PCIDriver/pciioctl.h"
//#include "../ISADriver/isaioctl.h"

#ifdef DMC_INTERRUPTS

long BUSGetInterruptStatus(int iIndex, PUSHORT pusStatus) //passing argument as 0 is normal:  sleep until interrupt.  passing argument as 1 means clear the status byte queue (does not sleep)
{
	long lRetCode=0;
	long item = 3;
	int nArg = *pusStatus;

/*	if ( controller[iIndex].controllerinfo.fControllerType == ControllerTypeISABus )
   		item = GALIL_ISA_INTSTATUS;
	else*/
		item = GALIL_PCI_INTSTATUS;

	lRetCode = ioctl( controller[ iIndex ].iDriver, item, &nArg );
	*pusStatus = nArg;
   	
	return lRetCode;
}

#endif
