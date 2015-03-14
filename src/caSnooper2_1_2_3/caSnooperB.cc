/*************************************************************************\
* Copyright (c) 2002 The University of Chicago, as Operator of Argonne
* National Laboratory.
* Copyright (c) 2002 The Regents of the University of California, as
* Operator of Los Alamos National Laboratory.
* This file is distributed subject to a Software License Agreement found
* in the file LICENSE that is included with this distribution. 
\*************************************************************************/
// Program to test CA modifications

#define CA_PEND_LOOP_TIME 10
#define PVNAME "CaSnooperB.test"

#define NPRINT_DEFAULT -1
#define NLIMIT_DEFAULT -1.0

#define ALLOCATE_STORAGE

#include <string.h>
#include <stdio.h>

#include "osiTimer.h"
#include "caSnooperBVersion.h"
#include "snoopClient.h"
#include "snoopCA.h"
#include "ut.h"

// Function prototypes
extern int main(int argc, char **argv);
static void usage();

// C Function prototypes
extern "C" {
    void processUDPMsg0(const struct sockaddr_in *net_addr,
      const struct sockaddr_in *in_addr, ca_uint16_t cmd);
    void processUDPMsg(const struct sockaddr_in *net_addr,
      const struct sockaddr_in *in_addr, ca_uint16_t cmd);
}

// Global variables
snoopClient *pCAS;

int doReport=1;
int doReset=0;
int doQuit=0;

extern int main(int argc, char **argv)
{
    float executionTime;
    int forever = 1;
    int nPrint = NPRINT_DEFAULT;
    float nLimit = NLIMIT_DEFAULT;
    int status;
    chid chid1;
    int i;

  // Initialize global variables
    udpCount=0;
    udpMsgHook=processUDPMsg;
    
  // Parse the command line
    for (i=1; i<argc; i++) {
	if(!strcmp(argv[i], "-h")) {
	    usage();
	    return(0);
	}
	if(sscanf(argv[i],"-l %f", &nLimit)==1) {
	    continue;
	}
	if(sscanf(argv[i],"-p %d", &nPrint)==1) {
	    continue;
	}
	if(sscanf(argv[i],"-t %f", &executionTime)==1) {
	    forever = 0;
	    continue;
	}
	print("Unknown option: \"%s\"\n", argv[i]);
	usage();
	return(1);
    }

  // Initialize CA
    status=ca_task_initialize();
    if(status != ECA_NORMAL) {
	errMsg("ca_task_initialize failed:\n%s",ca_message(status));
	return(-1);
    }
    
  // Search
    status=ca_search(PVNAME,&chid1);
    if(status != ECA_NORMAL) {
	errMsg("ca_search failed: for %s\n%s",
	  PVNAME,
	  ca_message(status));
    }

  // Make snoopClient
    pCAS = new snoopClient(nPrint,nLimit);
    if(!pCAS) {
	return(-1);
    }
    
  // Main loop
    print("Starting %s at %s\n",CASNOOPERB_VERSION_STRING,timeStamp());
    osiTime begin(osiTime::getCurrent());
    
  // Start the processing
    double processedTime = 0.0;
    osiTime start(osiTime::getCurrent());
    while(1) {
	ca_pend_io(CA_PEND_LOOP_TIME);
	processedTime=(double)(osiTime::getCurrent() - start);
	pCAS->setProcessTime(processedTime);
	if(forever) {
	} else {
	    if(processedTime > executionTime) break;
	}
	if(pCAS->doReport()) pCAS->report();
	if(pCAS->doReset()) pCAS->reset();
	if(pCAS->doQuit()) break;
    }

  // Print timing
    double elapsedTime=processedTime+(double)(start-begin);
    print("\nCaSnooperB terminating after %.2f seconds [%.2f minutes]\n",
      elapsedTime,elapsedTime/60.);
    
  // Do final report
    if(!forever) pCAS->report();
    delete pCAS;
    
  // Close CA
    status=ca_task_exit();
    if(status != ECA_NORMAL) {
	errMsg("ca_task_exit failed:\n%s",ca_message(status));
    }

    return(0);
}

void processUDPMsg0(const struct sockaddr_in *net_addr,
  const struct sockaddr_in *in_addr, ca_uint16_t cmd)
{
    char srcName[64],iocName[64],cmdName[64];

  // Convert net address to name
    ipAddrToA(net_addr,srcName,sizeof(srcName));

  // Convert in address to name
    if(in_addr) {
	ipAddrToA(in_addr,iocName,sizeof(iocName));
    }

  // Determine the command
    switch(cmd) {
    case CA_PROTO_NOOP:
	sprintf(cmdName,"CA_PROTO_NOOP");
	break;
    case CA_PROTO_SEARCH:
	sprintf(cmdName,"CA_PROTO_SEARCH");
	break;
    case CA_PROTO_NOT_FOUND:
	sprintf(cmdName,"CA_PROTO_NOT_FOUND");
	break;
    case CA_PROTO_RSRV_IS_UP:
	sprintf(cmdName,"CA_PROTO_RSRV_IS_UP");
	break;
    case REPEATER_CONFIRM:
	sprintf(cmdName,"REPEATER_CONFIRM");
	break;
    default:
	sprintf(cmdName,"Error");
	break;
    }

    print("%s [%s] %s\n",timeStamp(),cmdName,in_addr?iocName:"");
}

void processUDPMsg(const struct sockaddr_in *net_addr,
  const struct sockaddr_in *in_addr, ca_uint16_t cmd)
{
    dataNode *node=(dataNode *)0;
    char iocName[NAMESIZE];
    static int ifirst=1;

  // Print net_addr (caRepeater) the first time
    if(ifirst && net_addr) {
	char sourceName[NAMESIZE];

	ipAddrToA(net_addr,sourceName,sizeof(sourceName));
	print("Beacon source address: %s\n",sourceName);
	ifirst=0;
    }

  // Convert in address to name
    if(in_addr) {
	ipAddrToA(in_addr,iocName,sizeof(iocName));
    } else {
	*iocName='\0';
    }

    pCAS->processUDP(iocName,cmd);
}

void usage()
{
    print(
      "%s\n"
      "Usage: caSnooperB [options]\n"
      "  Options:\n"
      "    -h           Help (This message)\n"
      "    -l<decimal>  Print all requests under n sec\n"
      "    -p<integer>  Print top n requests (0 means all)\n"
      "    -t<decimal>  Run n seconds, then print report\n"
      "\n", 
      CASNOOPERB_VERSION_STRING);
	
}
