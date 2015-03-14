/*************************************************************************\
* Copyright (c) 2002 The University of Chicago, as Operator of Argonne
* National Laboratory.
* Copyright (c) 2002 The Regents of the University of California, as
* Operator of Los Alamos National Laboratory.
* This file is distributed subject to a Software License Agreement found
* in the file LICENSE that is included with this distribution. 
\*************************************************************************/
/* dbVerbose.c */
/*
 * dbVerbose.c,v 1.3 2009/10/23 16:29:27 jba Exp
 */

#include <stdlib.h>
#include <stdio.h>
#include <dbStaticLib.h>
#include <errlog.h>

int main(int argc,char **argv)
{
    DBBASE *pdbbase = 0;
    char   *path = 0;
    char   *sub = 0;
    long    status = 0;

    if(argc<3)
    {
        fprintf(stderr,
            "\nUsage: dbVerbose file.dbd file.db [outputfile]\n"
            "\tfile.dbd    Fully expanded database definitions\n"
            "\tfile.db     Database\n"
            "\toutputfile  Name of output file\n");
        exit(0);
    }

    status = dbReadDatabase(&pdbbase,argv[1],path,sub);
    if(status)
    { 
        fprintf(stderr,"Error on input for file %s",argv[1]);
        errMessage(status," from dbReadDatabase");
    }


    status = dbReadDatabase(&pdbbase,argv[2],path,sub);
    if(status)
    { 
        fprintf(stderr,"Error on input for file %s",argv[2]);
        errMessage(status," from dbReadDatabase");
    }

    if(argc>3)
        status=dbWriteRecord(pdbbase,argv[4],0,1);
    else
        status=dbWriteRecordFP(pdbbase,stdout,0,1);
    if(status)
    { 
        fprintf(stderr,"Error on output ");
        errMessage(status," from dbWriteRec");
    }

    return(0);
}
