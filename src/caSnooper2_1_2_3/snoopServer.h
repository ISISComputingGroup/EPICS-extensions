/*************************************************************************\
* Copyright (c) 2002 The University of Chicago, as Operator of Argonne
* National Laboratory.
* Copyright (c) 2002 The Regents of the University of California, as
* Operator of Los Alamos National Laboratory.
* This file is distributed subject to a Software License Agreement found
* in the file LICENSE that is included with this distribution. 
\*************************************************************************/
// CaSnooper: Server that logs broadcasts

#define NAMESIZE 80

#define DELIMITER '\t'
#define CA_PEND_IO_TIME 10.

#define DEFAULT_PREFIX "CaSnoop"
#define PREFIX_SIZE 11

#define INDIVIDUAL_NAME "CaSnoop.test"

#define NCHECK_DEFAULT -1
#define NPRINT_DEFAULT -1
#define NSIGMA_DEFAULT -1
#define NLIMIT_DEFAULT -1.0

// Interval for rate statistics in seconds
#define RATE_STATS_INTERVAL 1u

#include "caSnooperVersion.h"

#define SS_OK 0
#define SS_ERROR -1

#ifndef NELEMENTS
#  define NELEMENTS(A) (sizeof(A)/sizeof(A[0]))
#endif

#ifdef __cplusplus
#define NU(x)
#else
#define NU(x)=(x)
#endif

#include <string.h>
#include <stdio.h>

#include "epicsAssert.h"
#include "casdef.h"
#include "gddAppFuncTable.h"
#include "resourceLib.h"
#include "snoopStat.h"
#include "epicsVersion.h"

#if EPICS_REVISION > 13
#include "epicsTimer.h"
#define osiTime epicsTime
#else
#include "osiTimer.h"
#endif

// Statistics PVs
#define statRequestRate    0
#define statIndividualRate 1
#define statReport         2
#define statReset          3
#define statQuit           4
#define statCheck          5
#define statPrint          6
#define statSigma          7
#define statLimit          8
// Number of statistics PVs  (Must be consistent with above)
#define statCount          9

class snoopRateStatsTimer;
class snoopData;
class dataNode;
class snoopServer;

typedef struct _snoopServerStats
{
    const char *name;
    char *pvName;
    snoopStat *pv;
    double initValue;
    const char *units;
    short precision;
} snoopServerStats;

#if EPICS_REVISION > 13
class snoopRateStatsTimer : public epicsTimerNotify
{
  public:
    snoopRateStatsTimer(epicsTimerQueue &queue,
      double intervalIn, snoopServer *m) : 
      interval(intervalIn), startTime(epicsTime::getCurrent()),
      serv(m), timer(queue.createTimer()) {}
    virtual expireStatus expire(const epicsTime &curTime);
    void start() { timer.start(*this,interval); }
    void stop() { timer.cancel(); }
  protected:
    virtual ~snoopRateStatsTimer() { timer.destroy(); }
  private:
    double interval;
    epicsTime startTime;
    snoopServer *serv;
    epicsTimer &timer;
};
#else
class snoopRateStatsTimer : public osiTimer
{
  public:
    snoopRateStatsTimer(const osiTime &delay, snoopServer *m) :
      osiTimer(delay), interval(delay), startTime(osiTime::getCurrent()),
      serv(m) {}
    virtual void expire();
    virtual const osiTime delay() const { return interval; }
    virtual osiBool again() const { return osiTrue; }
    virtual const char *name() const { return "snoopRateStatsTimer"; }
  private:
    osiTime interval;
    osiTime startTime;
    snoopServer *serv;
};
#endif

class snoopData
{
  public:
    snoopData();
    snoopData(const char *nameIn);
    snoopData(const snoopData &snoopDataIn);
    snoopData &operator=(const snoopData &snoopDataIn);
    const char *getName(void) const { return name; }
    void incrCount(void) { count++; }
    unsigned long getCount(void) const { return count; }
    
  private:
    unsigned long count;
    char name[NAMESIZE];
};

class dataNode : public stringId, public tsSLNode<dataNode>
{
  public:
    dataNode(const char *name, snoopData& data,
      resTable<dataNode,stringId> &listIn) :
      stringId(name),
      pData(&data),
      list(listIn) { };
    ~dataNode(void) { };
    
    snoopData* getData(void) { return pData; }
    void destroy(void)
      {
	  list.remove(*this);
	  if(pData) delete pData;
	  delete this;
      }
    
    static void setNodeCount(unsigned long nodeCountIn) { nodeCount=nodeCountIn; }
    static unsigned long getNodeCount(void) { return nodeCount; }
    void addToNodeCount(void) { nodeCount++; }

    static void setDataArray(snoopData *dataArrayIn) { dataArray=dataArrayIn; }
    static snoopData *getDataArray(void) { return dataArray; }
    void addToDataArray(void) { dataArray[nodeCount++]=*getData(); }

  private:
    dataNode(void);
    snoopData* pData;
    resTable<dataNode,stringId> &list;
    static unsigned long nodeCount;
    static snoopData *dataArray;
};

class snoopServer : public caServer
{
  public:
    snoopServer(char *prefixIn, char *individualNameIn, 
      int nCheckIn, int nPrintIn, int nSigmaIn, double nLimitIn);
    ~snoopServer(void);
    
    void enable(void) { enabled=1; }
    void disable(void) { enabled=0; }
    void report(void);
    void reset(void);
    void show(unsigned level) const;
    void setProcessTime(double processTimeIn) { processTime = processTimeIn; }
    double getprocessTime(void) const {return processTime; };
    int doReport(void) const { return reportFlag; };
    int doReset(void) const { return resetFlag; };
    int doQuit(void) const { return quitFlag; };
    pvExistReturn pvExistTest(const casCtx &ctx, const char *pPvName);
    pvExistReturn pvExistTest(const casCtx& ctx, const caNetAddr& addr,
      const char* pvname);
    pvCreateReturn createPV(const casCtx &ctx, const char *pPvName);
    
    resTable<dataNode,stringId> *getPvList(void) { return &pvList; }

    casEventMask getSelectMask(void) const { return selectMask; }
    
    snoopServerStats *getStatTable(int type) { return &statTable[type]; }
    void setStat(int type,double val);
    void setStat(int type,unsigned long val);
    void processStat(int type,double val);
    void clearStat(int type);
    void initStats(char *prefix);
    const char *getPrefix(void) const { return statPrefix; }
    const char *getIndividualName(void) const { return individualName; }

    unsigned long getRequestCount() const { return requestCount; }
    unsigned long getIndividualCount() const { return individualCount; }

  private:
    int makeArray(unsigned long *nVals);
    int sortArray(unsigned long **index, unsigned long nVals);
    
    double processTime;
    int reportFlag;
    int resetFlag;
    int quitFlag;
    int enabled;
    int nCheck;
    int nPrint;
    int nSigma;
    double nLimit;
    snoopData *dataArray;
    resTable<dataNode,stringId> pvList;

    casEventMask selectMask;

    snoopServerStats statTable[statCount];
    int doStats;
    const char *statPrefix;
    int statPrefixLength;
    const char *individualName;
    unsigned long requestCount;
    unsigned long individualCount;
};
