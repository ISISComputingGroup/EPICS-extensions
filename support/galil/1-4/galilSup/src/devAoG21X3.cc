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
 * 
 * -----------------------------------------------------------------------------
*/


#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <alarm.h>
#include <dbDefs.h>
#include <dbAccess.h>
#include <recSup.h>
#include <recGbl.h>
#include <devSup.h>
#include <special.h>
#include <aoRecord.h>

#include "GalilInterface.h"
#include <epicsExport.h>

#include <errlog.h>

/* The following must match the definition in choiceGbl.ascii */
#define LINEAR 1

/* Create the dset for devAoModicon */
static long init_record(struct aoRecord *pao);
static long write_ao(struct aoRecord *pao);
static long special_linconv(struct aoRecord *pao, int after);

struct {
	long		number;
	DEVSUPFUN	report;
	DEVSUPFUN	init;
	DEVSUPFUN	init_record;
	DEVSUPFUN	get_ioint_info;
	DEVSUPFUN	write_ao;
	DEVSUPFUN	special_linconv;
	} devAoG21X3 =
		{
		6,
		NULL,
		NULL,
		(DEVSUPFUN)init_record,
		NULL,
		(DEVSUPFUN)write_ao,
		(DEVSUPFUN)special_linconv
		};
epicsExportAddress(dset,devAoG21X3);

static long
init_record(struct aoRecord *pao)
{
	struct vmeio *pvmeio;
	int status;

	/* ai.out must be an VME_IO */
	if (pao->out.type != VME_IO)
		{
		errlogPrintf("%s: Illegal INP field\n", pao->name);
		return(S_db_badField);
		}
	
	pvmeio = (struct vmeio *)&(pao->out.value);	
	status = GalilSanityCheck(pvmeio->card, pvmeio->signal, pvmeio->parm);
	if (status < 0)
		{
		errlogPrintf("%s: No Galil Controller or bad address\n", pao->name);
		return -1;
		}
	else
		status = GalilWriteReal(pvmeio->card,(unsigned short)pvmeio->signal, pvmeio->parm, pao->val);
	pao->udf = 0;
	//Success, and do not convert
	return 2;
}

static long
write_ao(struct aoRecord *pao)
{
	struct vmeio *pvmeio;
	int status;
	pvmeio = (struct vmeio *)&(pao->out.value);	
	status = GalilWriteReal(pvmeio->card, pvmeio->signal, pvmeio->parm,pao->val);
 	return 0;
}

static long
special_linconv(struct aoRecord *pao, int after)
{
	return 0;
}
