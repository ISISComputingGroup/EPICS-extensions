#include <alarm.h>
#include <cvtTable.h>
#include <dbDefs.h>
#include <dbAccess.h>
#include <recSup.h>
#include <recGbl.h>
#include <devSup.h>
#include <dbScan.h>
#include <link.h>
#include <mbboDirectRecord.h>

#include "GalilInterface.h"

#include <epicsExport.h>
#include <errlog.h>

static long init_mbboDirect_record(struct mbboDirectRecord *pmbbo);
static long write_mbboDirect(struct mbboDirectRecord *pmbbo);

struct  {
	long		number;
	DEVSUPFUN	report;
	DEVSUPFUN	init;
	DEVSUPFUN	init_record;
	DEVSUPFUN	get_ioint_info;
	DEVSUPFUN	write_mbbo;
	} devMbboDirectG21X3 =
		{
		5,
		NULL,
		NULL,
		(DEVSUPFUN)init_mbboDirect_record,
		NULL,
		(DEVSUPFUN)write_mbboDirect
		};
epicsExportAddress(dset,devMbboDirectG21X3);

static long
init_mbboDirect_record(struct mbboDirectRecord *pmbbo)
{
	struct vmeio *pvmeio;
	int status;

	if (pmbbo->out.type != VME_IO)
		{
		errlogPrintf("%s: Illegal INP field\n", pmbbo->name);
		return (S_db_badField);
		}
		
	pvmeio = (struct vmeio *)&(pmbbo->out.value);	

	if (pmbbo->nobt == 0)
		pmbbo->mask = 0xffff;
	pmbbo->mask <<= pmbbo->shft;
	
	pvmeio = (struct vmeio *)&(pmbbo->out.value);	
	status = GalilSanityCheck(pvmeio->card, pvmeio->signal, pvmeio->parm);
	if (status < 0)
		{
		errlogPrintf("%s: No Galil controller, or bad address, or unknown command\n", pmbbo->name);
		return -1;
		}
	else
		status = GalilWriteReal(pvmeio->card,(unsigned short)pvmeio->signal, pvmeio->parm, pmbbo->val);
	pmbbo->udf = 0;
	return 0;
}

static long
write_mbboDirect(struct mbboDirectRecord *pmbbo)
{
	struct vmeio *pvmeio;
	int status;
	pvmeio = (struct vmeio *)&(pmbbo->out.value);	
	status = GalilWriteReal(pvmeio->card, pvmeio->signal, pvmeio->parm,pmbbo->val);
 	return 0;
}
