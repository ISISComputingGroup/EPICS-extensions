/*************************************************************************\
* Copyright (c) 2002 The University of Chicago, as Operator of Argonne
* National Laboratory.
* Copyright (c) 2002 The Regents of the University of California, as
* Operator of Los Alamos National Laboratory.
* This file is distributed subject to a Software License Agreement found
* in the file LICENSE that is included with this distribution. 
\*************************************************************************/

// Class definitions for caSnooperB

#define NAMESIZE 64

#include "cadef.h"
#define SS_OK 0
#define SS_ERROR -1

#include "caSnooperBVersion.h"

#include "resourceLib.h"
#include "caProto.h"

class snoopData
{
  public:
    snoopData();
    snoopData(const char *nameIn);
    snoopData::snoopData(const snoopData &snoopDataIn);
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


class snoopClient
{
  public:
    snoopClient(int nPrintIn, double nLimitIn);
    ~snoopClient(void);

    void processUDP(const char *name, ca_uint16_t cmd);
    
    void report(void);
    void reset(void);
    void setProcessTime(double processTimeIn) { processTime = processTimeIn; }
    double getprocessTime(void) const {return processTime; };
    int doReport(void) const { return reportFlag; };
    int doReset(void) const { return resetFlag; };
    int doQuit(void) const { return quitFlag; };
    
    resTable<dataNode,stringId> *getIOCList(void) { return &iocList; }

    unsigned long getUDPCount() const { return udpCount; }

  private:
    int makeArray(unsigned long *nVals);
    int sortArray(unsigned long **index, unsigned long nVals);
    
    double processTime;
    int reportFlag;
    int resetFlag;
    int quitFlag;
    int nPrint;
    double nLimit;
    snoopData *dataArray;
    resTable<dataNode,stringId> iocList;

    unsigned long udpCount;
};
