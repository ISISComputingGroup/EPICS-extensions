/*************************************************************************\
* Copyright (c) 2002 The University of Chicago, as Operator of Argonne
* National Laboratory.
* Copyright (c) 2002 The Regents of the University of California, as
* Operator of Los Alamos National Laboratory.
* This file is distributed subject to a Software License Agreement found
* in the file LICENSE that is included with this distribution. 
\*************************************************************************/
// CaSnooperB: Check IOC beacons

#include <math.h>
#include "snoopClient.h"
#include "ut.h"

#define DEBUG_SORT 0
#define DEBUG_HASH 0
#define DEBUG_TIMER 0
#define DEBUG_PROCESS_STAT 0
#define DEBUG_STAT 0

#define ULONG_DIFF(n1,n2) (((n1) >= (n2))?((n1)-(n2)):((n1)+(ULONG_MAX-(n2))))

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
//                   snoopClient
////////////////////////////////////////////////////////////////////////

// snoopClient::snoopClient() //////////////////////////////////////////
snoopClient::snoopClient(int nPrintIn, double nLimitIn) :
    reportFlag(0),
    resetFlag(0),
    quitFlag(0),
    processTime(0.0),
    nPrint(nPrintIn),
    nLimit(nLimitIn),
    udpCount(0),
    dataArray((snoopData *)0)
{
  // Initialize the IOC list
    iocList.init(2048u);
}

// snoopClient::~snoopClient ///////////////////////////////////////////
snoopClient::~snoopClient(void)
{
  // Clear the IOC list
    iocList.destroyAllEntries();
}

// snoopClient::~snoopClient ///////////////////////////////////////////
void snoopClient::processUDP(const char *name, ca_uint16_t cmd)
{
    char cmdName[NAMESIZE];
    const char *hashName=(char *)0;
    dataNode *node=(dataNode *)0;

  // Check the cmd
    if(cmd == CA_PROTO_RSRV_IS_UP) {
	hashName=name;
    } else {
      // Determine the command
	switch(cmd) {
	case CA_PROTO_NOOP:
	    sprintf(cmdName,"[CA_PROTO_NOOP]");
	    break;
	case CA_PROTO_SEARCH:
	    sprintf(cmdName,"[CA_PROTO_SEARCH]");
	    break;
	case CA_PROTO_NOT_FOUND:
	    sprintf(cmdName,"[CA_PROTO_NOT_FOUND]");
	    break;
	case CA_PROTO_RSRV_IS_UP:
	    sprintf(cmdName,"[CA_PROTO_RSRV_IS_UP]");
	    break;
	case REPEATER_CONFIRM:
	    sprintf(cmdName,"[REPEATER_CONFIRM]");
	    break;
	default:
	    sprintf(cmdName,"[UnknownMessageType]");
	    break;
	}
	hashName=cmdName;
    }

  // Make the hash name
    stringId id(hashName,stringId::refString);

  // See if we have it
    node=iocList.lookup(id);
    if(node) {
	node->getData()->incrCount();
    } else {
	snoopData *sData = new snoopData(hashName);
	if(sData) {
	    node = new dataNode(hashName,*sData,iocList);
	    if(node) {
		node->getData()->incrCount();
		iocList.add(*node);
	    } else {
		delete sData;
		errMsg("Unable to create node for %s\n",hashName);
	    }
	}
    }
}

// snoopClient::makeArray //////////////////////////////////////////////
// Make an array of the hash table.
int snoopClient::makeArray(unsigned long *nVals)
{
  // Return if already allocated
    if(dataArray) return SS_ERROR;

  // Find out how many nodes
    dataNode::setNodeCount(0u);
    iocList.traverse(&dataNode::addToNodeCount);
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
    iocList.traverse(&dataNode::addToDataArray);
    *nVals=dataNode::getNodeCount();

#if DEBUG_HASH
    print("nVals=%lu\n",*nVals);
    for(i=0; i < *nVals; i++) {
	print("%3d %s\n",i,dataArray[i].getName());
    }
#endif

    return SS_OK;
}

// snoopClient::report /////////////////////////////////////////////////
void snoopClient::report(void)
{
    unsigned long nNodes=0,nRequestsTotal,nRequests;
    unsigned long *index=0,ii;
    double x;
    double max=0.0;
    double sum=0.0;
    double sumsq=0.0;
    double sigma,avg;
    unsigned long i;
    int status;
    char name[NAMESIZE];

  // Reset the report flag
    reportFlag=0;

  // Check time
    if(processTime <= 0.0) {
	print("snoopClient::report: The time interval is zero\n");
	return;
    }

  // Make an array out of the hash table
    status=makeArray(&nNodes);
    if(status != SS_OK) {
	errMsg("snoopClient::report: Cannot make data array");
	return;
    }
    if(status != SS_OK) {
	print("\nThere is no data to report\n");
	return;
    }

  // Loop over the nodes to get data for statistics
    nRequests=nRequestsTotal=0;
    for(i=0; i < nNodes; i++) {
	nRequests=dataArray[i].getCount();
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
    print("\n%s:\nThere were %ld UDP messages "
      "for %ld different servers.\n",
      timeStamp(),nRequestsTotal,nNodes);
    print("  Min(sec):   %.3f\n",processTime/max);
    print("  Mean(sec):  %.3f\n",processTime/avg);
    print("  StDev(sec): %.3f\n",processTime/sigma);


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
		errMsg("snoopClient::report: Cannot sort data");
		goto CLEANUP;
	    }
	}
	print("\nServers with %d shortest UDP periods:\n",nPrint0);
	for(i=0; i < nPrint0; i++) {
	    ii=index[i];
	    strcpy(name,dataArray[ii].getName());
	    print("%4ld %-20s %.2f\n",i+1,name,
	      processTime/dataArray[ii].getCount());
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
		errMsg("snoopClient::report: Cannot sort data");
		goto CLEANUP;
	    }
	}
	print("\nServers with periods under %.2f sec:\n",nLimit);
	for(i=0; i < nNodes; i++) {
	    ii=index[i];
	    if(processTime/dataArray[ii].getCount() > nLimit) break;
	    strcpy(name,dataArray[ii].getName());
	    print("%4ld %-20s %.2f\n",i+1,name,
	      processTime/dataArray[ii].getCount());
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
}

// snoopClient::reset //////////////////////////////////////////////////
void snoopClient::reset(void)
{
  // Clear the IOC list
    iocList.destroyAllEntries();
    print("\n%s Reset\n",timeStamp());

  // Reset the reset flag
    resetFlag=0;
}

// snoopClient::sortArray //////////////////////////////////////////////
int snoopClient::sortArray(unsigned long **index, unsigned long nVals)
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
	errMsg("snoopClient::sortArray: Cannot allocate space for index");
	rc=SS_ERROR;
	goto ERROR;
    }
    if(!vals) {
	errMsg("snoopClient::sortArray: Cannot allocate space for values");
	rc=SS_ERROR;
	goto ERROR;
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

  ERROR:
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
