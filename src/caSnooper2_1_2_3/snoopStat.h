/*************************************************************************\
* Copyright (c) 2002 The University of Chicago, as Operator of Argonne
* National Laboratory.
* Copyright (c) 2002 The Regents of the University of California, as
* Operator of Los Alamos National Laboratory.
* This file is distributed subject to a Software License Agreement found
* in the file LICENSE that is included with this distribution. 
\*************************************************************************/
// CaSnooper Statistics

// Based on gateStat class for the Gateway

#ifndef SNOOP_STAT_H
#define SNOOP_STAT_H

#include "aitTypes.h"

#include "epicsVersion.h"

#if EPICS_REVISION > 13
#define WRITE_CONST const
#else
#define WRITE_CONST
#endif

class gdd;
class snoopServer;

class snoopStat : public casPV
{
  public:
    snoopStat(snoopServer* serv,const char* n, int t);
    virtual ~snoopStat(void);
    
  // CA server interface functions
    virtual caStatus interestRegister(void);
    virtual void interestDelete(void);
    virtual aitEnum bestExternalType(void) const;
    virtual caStatus read(const casCtx &ctx, gdd &prototype);
    virtual caStatus write(const casCtx &ctx, WRITE_CONST gdd &value);
    virtual unsigned maxSimultAsyncOps(void) const;
    virtual const char *getName() const;
    
    void postData(long val);
    void postData(unsigned long val);
    void postData(double val);
    
  private:
    gdd* value;
    gdd *attr;
    int post_data;
    int type;
    snoopServer* serv;
    char* name;
};

#endif
