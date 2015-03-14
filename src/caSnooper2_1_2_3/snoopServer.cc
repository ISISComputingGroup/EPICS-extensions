/*************************************************************************\
* Copyright (c) 2002 The University of Chicago, as Operator of Argonne
* National Laboratory.
* Copyright (c) 2002 The Regents of the University of California, as
* Operator of Los Alamos National Laboratory.
* This file is distributed subject to a Software License Agreement found
* in the file LICENSE that is included with this distribution. 
\*************************************************************************/
// CaSnooper: Server that logs broadcasts

#include <math.h>
#include <cadef.h>
#include "snoopServer.h"
#include "ut.h"
#include "gddApps.h"

#define DEBUG_SORT 0
#define DEBUG_HASH 0
#define DEBUG_TIMER 0
#define DEBUG_PROCESS_STAT 0
#define DEBUG_STAT 0
#define DEBUG_PUT 0

#define ULONG_DIFF(n1,n2) (((n1) >= (n2))?((n1)-(n2)):((n1)+(ULONG_MAX-(n2))))

#define HOST_NAMESIZE 256

#if EPICS_REVISION > 13
#include <osiSock.h>
#else
#include <bsdSocketResource.h>
// server.h is private and not in base/include.  We are including it
// so we can access casCtx, which is not normally possible (by
// intent).  Note: The src files have to be available to do this.
#include "server.h"
inline casCoreClient * casCtx::getClient() const 
{
    return this->pClient;
}
#endif

// Static initializations
unsigned long dataNode::nodeCount=0;
snoopData *dataNode::dataArray=(snoopData *)0;

char connTable[][10]={
    "NC",     /* cs_never_conn */
    "NC",     /* cs_prev_conn */
    "C",      /* cs_conn */
    "NC",     /* cs_closed */
};

////////////////////////////////////////////////////////////////////////
//                   snoopServer
////////////////////////////////////////////////////////////////////////

// snoopServer::snoopServer() //////////////////////////////////////////
snoopServer::snoopServer(char *prefixIn, char *individualNameIn, 
  int nCheckIn, int nPrintIn, int nSigmaIn, double nLimitIn) :
    processTime(0.0),
    reportFlag(0),
    resetFlag(0),
    quitFlag(0),
    enabled(0),
    nCheck(nCheckIn),
    nPrint(nPrintIn),
    nSigma(nSigmaIn),
    nLimit(nLimitIn),
    dataArray((snoopData *)0),
    doStats(0),
    statPrefix(0),
    statPrefixLength(0),
    individualName(individualNameIn),
    individualCount(0)
{
  // Initialize the PV list
#if EPICS_REVISION > 13
  // No longer required
#else
    pvList.init(2048u);
#endif

  // Set the individual name
    if(individualNameIn && *individualNameIn) individualName=individualNameIn;
    else individualName=INDIVIDUAL_NAME;

  // Initialize the stats
    initStats(prefixIn);
    requestCount=0u;

  // Set the select mask to everything supported
#if EPICS_REVISION > 13
    selectMask=(alarmEventMask()|valueEventMask()|logEventMask());
#else
    selectMask=(alarmEventMask|valueEventMask|logEventMask);
#endif
}

// snoopServer::~snoopServer ///////////////////////////////////////////
snoopServer::~snoopServer(void)
{
  // Clear the pvList
#if EPICS_REVISION > 13
    pvList.traverse(&dataNode::destroy);
#else
    pvList.destroyAllEntries();
#endif
}

// snoopServer::clearStat //////////////////////////////////////////////
void snoopServer::clearStat(int type)
{
    statTable[type].pv=NULL;
}

// snoopServer::pvExistTest ////////////////////////////////////////////
// This function is called by Base 3.13 and earlier
pvExistReturn snoopServer::pvExistTest(const casCtx& ctx, const char *pvName)
{
#if EPICS_REVISION > 13
  // Should not get here
    return pverDoesNotExistHere;
#else
    casClient *pClient=(casClient *)ctx.getClient();

  // Return if data taking is not enabled
    if(!enabled) return pverDoesNotExistHere;

  // Get the caNetAddr
    const caNetAddr &addr=pClient->fetchRespAddr();

    return pvExistTest(ctx,addr,pvName);
#endif
}

// This function is called by Base 3.14 and later
pvExistReturn snoopServer::pvExistTest(const casCtx& ctx, const caNetAddr& addr,
  const char* pvName)
{
    char hostName[HOST_NAMESIZE]="";
    char name[NAMESIZE+HOST_NAMESIZE]="";
    dataNode *node=(dataNode *)0;
    int i=0,len;

  // Return if data taking is not enabled
    if(!enabled) return pverDoesNotExistHere;
    requestCount++;

  // Make the hash name
    struct sockaddr_in inaddr=(struct sockaddr_in)addr;
    ipAddrToA(&inaddr,hostName,HOST_NAMESIZE);
    if(strlen(pvName) > NAMESIZE-1) printf("Extra long pv name; truncating %s\n", pvName);
    strncpy(name,pvName,NAMESIZE-1);
    len=strlen(name);
    name[len++]=DELIMITER;
    strncpy(name+len,hostName,NAMESIZE-len);
    name[NAMESIZE-1]='\0';
    stringId id(name,stringId::refString);

  // See if we have it
    node=pvList.lookup(id);
    if(node) {
	node->getData()->incrCount();
    } else {
	snoopData *sData = new snoopData(name);
	if(sData) {
	    node = new dataNode(name,*sData,pvList);
	    if(node) {
		node->getData()->incrCount();
		pvList.add(*node);
	    } else {
		delete sData;
		errMsg("Unable to create node for %s\n",name);
	    }
	}
    }

  // Check for the individual name
    if(individualName && !strcmp(pvName,individualName)) {
	individualCount++;
    }
    
  // Check for stat PVs
    if(doStats) {
	for(i=0; i < statCount; i++) {
	    if(strcmp(pvName,statTable[i].pvName) == 0) {
		return pverExistsHere;
	    }
	}
    }

    return pverDoesNotExistHere;
}

// snoopServer::createPV ///////////////////////////////////////////////
pvCreateReturn snoopServer::createPV(const casCtx &NU(ctx), const char *pvName)
{
  // Create stat PVs
    if(doStats) {
	for(int i=0; i < statCount; i++) {
	    if(strcmp(pvName,statTable[i].pvName)==0) {
		if(statTable[i].pv==NULL) {
		    statTable[i].pv=new snoopStat(this,pvName,i);
		}
#if DEBUG_PUT
		print("createPV: %d %x %s\n",i,statTable[i],pvName);
#endif		
		return pvCreateReturn(*statTable[i].pv);
	    }
	}
    }

    return S_casApp_pvNotFound;
}

// snoopServer::makeArray //////////////////////////////////////////////
// Make an array of the hash table.
int snoopServer::makeArray(unsigned long *nVals)
{
  // Return if already allocated
    if(dataArray) return SS_ERROR;

  // Find out how many nodes
    dataNode::setNodeCount(0u);
    pvList.traverse(&dataNode::addToNodeCount);
    *nVals=dataNode::getNodeCount();
#if DEBUG_HASH
    print("nVals=%lu\n",*nVals);
#endif

    dataArray=new snoopData[*nVals];
    if(!dataArray) {
	*nVals=0;
	return SS_ERROR;
    }

  // Fill the dataArray
    dataNode::setNodeCount(0u);
    dataNode::setDataArray(dataArray);
    pvList.traverse(&dataNode::addToDataArray);
    *nVals=dataNode::getNodeCount();

#if DEBUG_HASH
    print("nVals=%lu\n",*nVals);
    for(i=0; i < *nVals; i++) {
	print("%3d %s\n",i,dataArray[i].getName());
    }
#endif

    return SS_OK;
}

// snoopServer::report /////////////////////////////////////////////////
void snoopServer::report(void)
{
    chid *pChid=(chid *)0;
    unsigned long nNodes=0,nNodes1,nRequestsTotal,nRequests;
    unsigned long *index=0,ii;
    double x;
    double max=0.0;
    double sum=0.0;
    double sumsq=0.0;
    double sigma, nSigmaVal, avg;
    unsigned long il,jl;
    int i;
    int status;
    char name[NAMESIZE];
    char *ptr;

  // Reset the report flag
    if(doStats) {
	reportFlag=0;
	setStat(statReport,(unsigned long)resetFlag);
    }

  // Check time
    if(processTime <= 0.0) {
	print("snoopServer::report: The time interval is zero\n");
	return;
    }

  // Make an array out of the hash table
    status=makeArray(&nNodes);
    if(status != SS_OK) {
	errMsg("snoopServer::report: Cannot make data array");
	return;
    }
    if(status != SS_OK) {
	print("\nThere is no data to report\n");
	return;
    }

  // Loop over the nodes to get data for statistics
    nRequests=nRequestsTotal=0;
    for(il=0; il < nNodes; il++) {
	nRequests=dataArray[il].getCount();
	nRequestsTotal+=nRequests;
	x=(double)nRequests;
	sum+=x;
	sumsq+=(x*x);
	if(x > max) max=x;
    }

  // Calculate statistics
    if(nNodes == 0) {
	avg=0;
	sigma=0;
    } else if (nNodes == 1) {
	avg=sum;
	sigma=0;
    } else {
	avg=sum/(double)nNodes;
	sigma=sqrt((sumsq-avg*avg)/(nNodes-1));
    }
    print("\n%s:\nThere were %ld requests to check for PV existence "
      "for %ld different PVs.\n",
      timeStamp(),nRequestsTotal,nNodes);
    print("  Max(Hz):   %.2f\n",max/processTime);
    print("  Mean(Hz):  %.2f\n",avg/processTime);
    print("  StDev(Hz): %.2f\n",sigma/processTime);


  // Print the n sigma values
    if(nSigma >= 0) {
	nSigmaVal=nSigma*sigma;
	print("\nThe following were above the %d-sigma value (%.2f):\n",
	  nSigma,nSigmaVal);
	max=sum=sumsq=0.0;
	nRequestsTotal=nNodes1=0;
	for(il=jl=0; il < nNodes; il++) {
	    nRequests=dataArray[il].getCount();
	    if((double)nRequests > nSigmaVal) {
		strcpy(name,dataArray[il].getName());
		ptr=strchr(name,DELIMITER);
		if(ptr) *ptr='\0';
		else ptr=name;
		print("%4ld %-30s %-33s %.2f\n",++jl,ptr+1,name,
		  dataArray[il].getCount()/processTime);
	    } else {
		nNodes1++;
		nRequestsTotal+=nRequests;
		x=(double)nRequests;
		sum+=x;
		sumsq+=(x*x);
		if(x > max) max=x;
	    }
	}
	
      // Calculate statistics without outliers
	if(nNodes1 == 0) {
	    avg=0;
	    sigma=0;
	} else if (nNodes1 == 1) {
	    avg=sum;
	    sigma=0;
	} else {
	    avg=sum/(double)nNodes1;
	    sigma=sqrt((sumsq-avg*avg)/(nNodes1-1));
	}
	print("\nThere were %ld requests for %ld PVs excluding these.\n",
	  nRequestsTotal,nNodes1);
	print("  Max(Hz):   %.2f\n",max/processTime);
	print("  Mean(Hz):  %.2f\n",avg/processTime);
	print("  StDev(Hz): %.2f\n",sigma/processTime);
    }

  // Print the top PVs
    int nPrint0;
    if(nPrint == 0) nPrint0 = nNodes;
    else nPrint0=nPrint;
    if(nPrint0 > (int)nNodes) nPrint0 = (int)nNodes;
    if(nPrint0 > 0) {
	if(!index) {
	    status=sortArray(&index,nNodes);
	    if(status != SS_OK || !index) {
		if(index) {
		    delete [] index;
		    index=0;
		}
		errMsg("snoopServer::report: Cannot sort data");
		goto CLEANUP;
	    }
	}
	print("\nPVs with top %d requests:\n",nPrint0);
	for(i=0; i < nPrint0; i++) {
	    ii=index[i];
	    strcpy(name,dataArray[ii].getName());
	    ptr=strchr(name,DELIMITER);
	    if(ptr) *ptr='\0';
	    else ptr=name;
	    print("%4ld %-30s %-33s %.2f\n",i+1,ptr+1,name,
	      dataArray[ii].getCount()/processTime);
	}
    }

  // Print the PVs over the limit
    if(nLimit >= 0.0) {
	if(!index) {
	    status=sortArray(&index,nNodes);
	    if(status != SS_OK || !index) {
		if(index) {
		    delete [] index;
		    index=0;
		}
		errMsg("snoopServer::report: Cannot sort data");
		goto CLEANUP;
	    }
	}
	print("\nPVs over %.2f Hz\n",nLimit);
	for(il=0; il < nNodes; il++) {
	    ii=index[il];
	    if(dataArray[ii].getCount()/processTime < nLimit) break;
	    strcpy(name,dataArray[ii].getName());
	    ptr=strchr(name,DELIMITER);
	    if(ptr) *ptr='\0';
	    else ptr=name;
	    print("%4ld %-30s %-33s %.2f\n",il+1,ptr+1,name,
	      dataArray[ii].getCount()/processTime);
	}
    }

  // Check PV existence
    int nCheck0;
    if(nCheck == 0) nCheck0 = nNodes;
    else nCheck0=nCheck;
    if(nCheck0 > (int)nNodes) nCheck0 = (int)nNodes;
    if(nCheck0 > 0) {
	if(!index) {
	    status=sortArray(&index,nNodes);
	    if(status != SS_OK || !index) {
		if(index) {
		    delete [] index;
		    index=0;
		}
		errMsg("snoopServer::report: Cannot sort data");
		goto CLEANUP;
	    }
	}
	print("\nConnection status for top %d PVs after %.2f sec:\n",
	  nCheck0,CA_PEND_IO_TIME);

      // Allocate chids
	pChid = new chid[nCheck0];
	if(!pChid) {
	    errMsg("snoopServer::report: Cannot allocate space for CA");
	    goto CLEANUP;
	}

      // Initialize CA
	status=ca_task_initialize();
	if(status != ECA_NORMAL) {
	    errMsg("snoopServer::report: "
	      "ca_task_initialize failed:\n%s",ca_message(status));
	    goto CLEANUP;
	}

      // Search
	for(i=0; i < nCheck0; i++) {
	    ii=index[i];
	    strcpy(name,dataArray[ii].getName());
	    ptr=strchr(name,DELIMITER);
	    if(ptr) *ptr='\0';
	    else ptr=name;
	    status=ca_search(name,&pChid[i]);
	    if(status != ECA_NORMAL) {
		errMsg("snoopServer::report: [%d %s] "
		  " ca_search failed:\n%s",ii,dataArray[ii].getName(),
		  ca_message(status));
	    }
	}

      // Wait
	ca_pend_io(CA_PEND_IO_TIME);
	
      // Print results
	for(i=0; i < nCheck0; i++) {
	    ii=index[i];
	    strcpy(name,dataArray[ii].getName());
	    ptr=strchr(name,DELIMITER);
	    if(ptr) *ptr='\0';
	    else ptr=name;
	    print("%4ld %-30s %-33s %-2s %.2f\n",i+1,ptr+1,name,
	      connTable[ca_state(pChid[i])],
	      dataArray[ii].getCount()/processTime);
	}
      // Close CA
	status=ca_task_exit();
	if(status != ECA_NORMAL) {
	    errMsg("snoopServer::report: "
	      "ca_task_exit failed:\n%s",ca_message(status));
	}

    }

  CLEANUP:
    if(dataArray) {
	delete [] dataArray;
	dataArray=(snoopData *)0;
	dataNode::setDataArray((snoopData *)0);
    }
    if(index) {
	delete [] index;
    }
    if(pChid) {
	delete [] pChid;
    }
}

// snoopServer::reset //////////////////////////////////////////////////
void snoopServer::reset(void)
{
  // Clear the pvList
#if EPICS_REVISION > 13
    pvList.traverse(&dataNode::destroy);
#else
    pvList.destroyAllEntries();
#endif
    print("\n%s Reset\n",timeStamp());

  // Reset the reset flag
    resetFlag=0;
    setStat(statReport,(unsigned long)resetFlag);

  // Do not zero requestCount, IndividualCpount, etc. since it will
  // screw up the rates
}

// snoopServer::initStat ///////////////////////////////////////////////
void snoopServer::initStats(char *prefix)
{
    int i;
    
  // Define the prefix for the server stats
    if(!prefix) {
	for(i=0; i < statCount; i++) {
	    statTable[i].pv=NULL;
	}
	doStats=0;
	return;
    }

    if(*prefix) {
      // Use the specified one
	statPrefix=prefix;
    } else {
	statPrefix=DEFAULT_PREFIX;
    }
    statPrefixLength=strlen(statPrefix);
    doStats=1;

    
  // Set up PV names for server stats and fill them into the statTable
    for(i=0; i < statCount; i++) {
	switch(i) {
	case statRequestRate:
	    requestCount=0u;
	    statTable[i].name="requestRate";
	    statTable[i].initValue=0.0;
	    statTable[i].units="Hz";
	    statTable[i].precision=2;
	    break;
	case statIndividualRate:
	    individualCount=0u;
	    statTable[i].name="individualRate";
	    statTable[i].initValue=0.0;
	    statTable[i].units="Hz";
	    statTable[i].precision=2;
	    break;
	case statReport:
	    reportFlag=0;
	    statTable[i].name="report";
	    statTable[i].initValue=0.0;
	    statTable[i].units="";
	    statTable[i].precision=0;
	    break;
	case statReset:
	    resetFlag=0;
	    statTable[i].name="reset";
	    statTable[i].initValue=(double)resetFlag;
	    statTable[i].units="";
	    statTable[i].precision=0;
	    break;
	case statQuit:
	    quitFlag=0;
	    statTable[i].name="quit";
	    statTable[i].initValue=(double)quitFlag;
	    statTable[i].units="";
	    statTable[i].precision=0;
	    break;
	case statCheck:
	    statTable[i].name="nCheck";
	    statTable[i].initValue=(double)nCheck;
	    statTable[i].units="";
	    statTable[i].precision=0;
	    break;
	case statPrint:
	    statTable[i].name="nPrint";
	    statTable[i].initValue=(double)nPrint;
	    statTable[i].units="";
	    statTable[i].precision=0;
	    break;
	case statSigma:
	    statTable[i].name="nSigma";
	    statTable[i].initValue=(double)nSigma;
	    statTable[i].units="";
	    statTable[i].precision=0;
	    break;
	case statLimit:
	    statTable[i].name="nLimit";
	    statTable[i].initValue=nLimit;
	    statTable[i].units="";
	    statTable[i].precision=2;
	    break;
	}
	
	statTable[i].pvName=new char[statPrefixLength+1+strlen(statTable[i].name)+1];
	sprintf(statTable[i].pvName,"%s.%s",statPrefix,statTable[i].name);
	statTable[i].pv=NULL;
    }
}

// snoopServer::processStat ////////////////////////////////////////////
void snoopServer::processStat(int type, double val)
{
    switch(type) {
    case statReport:
	if(val > 0.0) reportFlag=1;
	break;
    case statReset:
	if(val > 0.0) resetFlag=1;
	break;
    case statQuit:
	if(val > 0.0) quitFlag=1;
	break;
    case statCheck:
	nCheck=(int)floor(val+.5);
	break;
    case statPrint:
	nPrint=(int)floor(val+.5);
	break;
    case statSigma:
	nSigma=(int)floor(val+.5);
	break;
    case statLimit:
	nLimit=val;
	break;
    }

#if DEBUG_PROCESS_STAT
    print("snoopServer::processStat:\n"
      "val=%.2f nCheck=%d nPrint=%d nSigma=%d nLimit=%.2f\n",
      val,nCheck,nPrint,nSigma,nLimit);
#endif
}

// snoopServer::setStat ////////////////////////////////////////////////
void snoopServer::setStat(int type, double val)
{
    if(doStats && statTable[type].pv) statTable[type].pv->postData(val);
#if DEBUG_STAT
    print("snoopServer::setStat: type=%d val=%.2f\n",
      type,val);
#endif
}

void snoopServer::setStat(int type, unsigned long val)
{
    if(doStats && statTable[type].pv) statTable[type].pv->postData(val);
}

// snoopServer::show ///////////////////////////////////////////////////
void snoopServer::show(unsigned level) const
{
  //
  // server tool specific show code goes here
  //
    
  //
  // print information about ca server libarary
  // internals
  //
  //    print("caServer:\n");
    this->caServer::show(level);
}

// snoopServer::sortArray //////////////////////////////////////////////
int snoopServer::sortArray(unsigned long **index, unsigned long nVals)
{
    int rc=SS_OK;
    unsigned long i;

  // Check if the index has already been made
    if(*index) return SS_OK;
    if(nVals <= 0) return SS_ERROR;
    if(!dataArray) return SS_ERROR;

  // Allocate space
    *index = new unsigned long[nVals];
    double *vals = new double[nVals];
    if(!*index) {
	errMsg("snoopServer::sortArray: Cannot allocate space for index");
	rc=SS_ERROR;
	goto HANDLE_ERROR;
    }
    if(!vals) {
	errMsg("snoopServer::sortArray: Cannot allocate space for values");
	rc=SS_ERROR;
	goto HANDLE_ERROR;
    }

  // Fill the vals array
    for(i=0; i < nVals; i++) {
	vals[i]=dataArray[i].getCount();
    }

  // Sort
    hsort(vals,*index,nVals);

#if DEBUG_SORT
    print("\nDebug sort\n");
    for(i=0; i < nVals; i++) {
	print("%3lu %.2f -> %3lu %.2f\n",
	  i,vals[i],(*index)[i],vals[(*index)[i]]);
    } 
#endif

    goto CLEAN;

  HANDLE_ERROR:
    if(*index) {
	delete *index;
	*index=0;
    }

  CLEAN:
    if(vals) delete [] vals;

    return rc;
}

////////////////////////////////////////////////////////////////////////
//                   snoopData
////////////////////////////////////////////////////////////////////////

// snoopData::snoopData ////////////////////////////////////////////////
snoopData::snoopData() :
    count(0)
{
    *name='\0';
}

snoopData::snoopData(const char *nameIn) :
    count(0)
{
    strcpy(name,nameIn);
}

// Copy constructor
snoopData::snoopData(const snoopData &snoopDataIn)
{
    count=snoopDataIn.getCount();
    strcpy(name,snoopDataIn.getName());
}

snoopData &snoopData::operator=(const snoopData &snoopDataIn)
{
    if(this != &snoopDataIn) {
	count=snoopDataIn.getCount();
	strcpy(name,snoopDataIn.getName());
    }
    return *this;
}

////////////////////////////////////////////////////////////////////////
//                   snoopRateStatsTimer
////////////////////////////////////////////////////////////////////////

// snoopRateStatsTimer::expire /////////////////////////////////////////
#if EPICS_REVISION > 13
epicsTimerNotify::expireStatus
snoopRateStatsTimer::expire(const epicsTime &curTime)
{
    static int first=1;
    static epicsTime prevTime;
    double delTime;
    static unsigned long rrPrevCount;
    static unsigned long srPrevCount;
    unsigned long rrCurCount=serv->getRequestCount();
    unsigned long srCurCount=serv->getIndividualCount();
    double rrRate;
    double srRate;
    
  // Initialize the first time
    if(first) {
	prevTime=curTime;
	rrPrevCount=rrCurCount;
	srPrevCount=srCurCount;
	first=0;
    }
    delTime=(double)(curTime-prevTime);
    
  // Calculate the request rate
    rrRate=(delTime > 0)?(double)(ULONG_DIFF(rrCurCount,rrPrevCount))/
      delTime:0.0;
    serv->setStat(statRequestRate,rrRate);
    srRate=(delTime > 0)?(double)(ULONG_DIFF(srCurCount,srPrevCount))/
      delTime:0.0;
    serv->setStat(statIndividualRate,srRate);
    
#if DEBUG_TIMER
    print("snoopRateStatsTimer::expire():\n");
    print("  rrCurCount=%ld rrPrevCount=%ld rrRate=%g\n",
      rrCurCount,rrPrevCount,rrRate);
    print("  srCurCount=%ld srPrevCount=%ld srRate=%g\n",
      srCurCount,srPrevCount,srRate);
#endif

  // Reset the values for previous
    prevTime=curTime;
    rrPrevCount=rrCurCount;
    srPrevCount=srCurCount;

  // Set to continue
    return epicsTimerNotify::expireStatus(restart,interval);
}
#else
void snoopRateStatsTimer::expire()
{
    static int first=1;
    static osiTime prevTime;
    osiTime curTime=osiTime::getCurrent();
    double delTime;
    static unsigned long rrPrevCount;
    static unsigned long srPrevCount;
    unsigned long rrCurCount=serv->getRequestCount();
    unsigned long srCurCount=serv->getIndividualCount();
    double rrRate;
    double srRate;
    
  // Initialize the first time
    if(first) {
	prevTime=curTime;
	rrPrevCount=rrCurCount;
	srPrevCount=srCurCount;
	first=0;
    }
    delTime=(double)(curTime-prevTime);
    
  // Calculate the request rate
    rrRate=(delTime > 0)?(double)(ULONG_DIFF(rrCurCount,rrPrevCount))/
      delTime:0.0;
    serv->setStat(statRequestRate,rrRate);
    srRate=(delTime > 0)?(double)(ULONG_DIFF(srCurCount,srPrevCount))/
      delTime:0.0;
    serv->setStat(statIndividualRate,srRate);
    
#if DEBUG_TIMER
    print("snoopRateStatsTimer::expire():\n");
    print("  rrCurCount=%ld rrPrevCount=%ld rrRate=%g\n",
      rrCurCount,rrPrevCount,rrRate);
    print("  srCurCount=%ld srPrevCount=%ld srRate=%g\n",
      srCurCount,srPrevCount,srRate);
#endif

  // Reset the values for previous
    prevTime=curTime;
    rrPrevCount=rrCurCount;
    srPrevCount=srCurCount;
}
#endif
