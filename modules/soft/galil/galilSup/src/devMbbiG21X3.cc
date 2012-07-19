#include <stdio.h>
#include <string.h>

#include <alarm.h>
#include <cvtTable.h>
#include <dbDefs.h>
#include <dbAccess.h>
#include <recSup.h>
#include <recGbl.h>
#include <devSup.h>
#include <dbScan.h>
#include <link.h>

#include <mbbiDirectRecord.h>

#include "GalilInterface.h"

#include <epicsExport.h>
#include <errlog.h>

static long init_mbbiDirect_record(struct mbbiDirectRecord *pmbbi);
static long read_mbbiDirect(struct mbbiDirectRecord *pmbbi);

struct  {
	long number;
	DEVSUPFUN report;
	DEVSUPFUN init;
	DEVSUPFUN init_record;
	DEVSUPFUN get_ioint_info;
	DEVSUPFUN write_bo;
	} devMbbiDirectG21X3 =
		{
		   5,
		NULL,
		NULL,
		(DEVSUPFUN)init_mbbiDirect_record,
		NULL,
		(DEVSUPFUN)read_mbbiDirect
		};
epicsExportAddress(dset,devMbbiDirectG21X3);

static long
init_mbbiDirect_record(struct mbbiDirectRecord *pmbbi)
{
	struct vmeio *pvmeio;
	int status;
	double value;

	/* mbbi.inp must be an AB_IO */
	if (pmbbi->inp.type != VME_IO)
		{
		errlogPrintf("%s: Illegal INP field\n", pmbbi->name);
		return (S_db_badField);
		}
		
	pvmeio = (struct vmeio *)&(pmbbi->inp.value);	

	if(pmbbi->nobt == 0)
		 pmbbi->mask = 0xffff;
	pmbbi->mask <<= pmbbi->shft;
	
	status = GalilSanityCheck(pvmeio->card, pvmeio->signal, pvmeio->parm);

	if (status < 0)
		{
		errlogPrintf("%s: No Galil controller, or bad address, or unknown command\n", pmbbi->name);
		return -1;
		}
	else
		status = GalilReadReal(pvmeio->card,(unsigned short)pvmeio->signal, pvmeio->parm, &value);
		
	pmbbi->rval = (epicsInt32)value;
	pmbbi->val =  (epicsInt32)value;
	pmbbi->udf = 0;
	return 0;
}

static long
read_mbbiDirect(struct mbbiDirectRecord *pmbbi)
{
	struct vmeio *pvmeio;
	int status;
	double value;

	pvmeio = (struct vmeio *)&(pmbbi->inp.value);
	status = GalilReadReal(pvmeio->card, (unsigned short)pvmeio->signal, pvmeio->parm, &value);

	pmbbi->rval = (epicsInt32)value;
	pmbbi->val =  (epicsInt32)value;
	return status;
}
