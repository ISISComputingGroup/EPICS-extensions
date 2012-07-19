/* devAiG21X3.c - Device Support Routines */
/*
 *      Author: Bob Dalesio
 *      Date:   04-16-98
 *
 *      Experimental Physics and Industrial Control System (EPICS)
 *
 *      Copyright 1991, the Regents of the University of California,
 *      and the University of Chicago Board of Governors.
 *
 *      This software was produced under  U.S. Government contracts:
 *      (W-7405-ENG-36) at the Los Alamos National Laboratory,
 *      and (W-31-109-ENG-38) at Argonne National Laboratory.
 *
 *      Initial development by:
 *              The Controls and Automation Group (AT-8)
 *              Ground Test Accelerator
 *              Accelerator Technology Division
 *              Los Alamos National Laboratory
 *
 *      Co-developed with
 *              The Controls and Computing Group
 *              Accelerator Systems Division
 *              Advanced Photon Source
 *              Argonne National Laboratory
 *
 * Adapted for use with Galil Motion Controllers over ethernet by Australian Synchrotron, Inc.
 * Where applicable, modifications use the following copyright
 * -----------------------------------------------------------------------------
*/

#include <stdio.h>
#include <string.h>

#include <alarm.h>
#include <cvtTable.h>
#include <dbDefs.h>
#include <dbAccess.h>
#include <recSup.h>
#include <recGbl.h>
#include <devSup.h>
#include <link.h>
#include <aiRecord.h>

#include <epicsExport.h>
#include "GalilInterface.h"

#include <errlog.h>

static long init_ai(struct aiRecord *pai);
static long read_ai(struct aiRecord *pai);
static long special_linconv(struct aiRecord *pai, int after);

struct	
{
	long		number;
	DEVSUPFUN	report;
	DEVSUPFUN	init;
	DEVSUPFUN	init_record;
	DEVSUPFUN	get_ioint_info;
	DEVSUPFUN	read_ai;
	DEVSUPFUN	special_linconv;
} devAiG21X3 = {
		6,
		NULL,
		NULL,
		(DEVSUPFUN)init_ai,
		NULL,
		(DEVSUPFUN)read_ai,
		(DEVSUPFUN)special_linconv
		};

epicsExportAddress(dset,devAiG21X3);

static long
init_ai(struct aiRecord *pai)
{
	struct vmeio *pvmeio;
	int status;
	double value;

	/* ai.inp must be an VME_IO */
	if (pai->inp.type != VME_IO)
		{
		errlogPrintf("%s: Illegal INP field\n", pai->name);
		return(S_db_badField);
		}
	
	pvmeio = (struct vmeio *)&(pai->inp.value);	
	status = GalilSanityCheck(pvmeio->card, pvmeio->signal, pvmeio->parm);
	if (status < 0)
		{
		errlogPrintf("%s: No Galil Controller or bad address\n", pai->name);
		return -1;
		}
	else
		status = GalilReadReal(pvmeio->card,(unsigned short)pvmeio->signal, pvmeio->parm,&value);
	pai->rval = (epicsInt32)value;
	pai->val = value;
	pai->udf = 0;
	return 0;
}

static long
read_ai(struct aiRecord *pai)
{
	double value;
	struct vmeio 	*pvmeio;
	int status;

	pvmeio = (struct vmeio *)&(pai->inp.value);
	status = GalilReadReal(pvmeio->card, (unsigned short)pvmeio->signal, pvmeio->parm, &value);

	pai->rval = (epicsInt32)value;
	pai->val = value;
	pai->udf = 0;
	return status;
}

static long special_linconv(struct aiRecord *pai, int after)
{
	return 0;
}
