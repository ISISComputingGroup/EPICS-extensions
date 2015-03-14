/*************************************************************************\
* Copyright (c) 2002 The University of Chicago, as Operator of Argonne
* National Laboratory.
* Copyright (c) 2002 The Regents of the University of California, as
* Operator of Los Alamos National Laboratory.
* This file is distributed subject to a Software License Agreement found
* in the file LICENSE that is included with this distribution. 
\*************************************************************************/

// CaSnooper: Server that logs broadcasts

#define DEBUG_TIME 0

#if 0
// The following are used for the modified base
#  define ALLOCATE_STORAGE
#  include "snoopCA.h"
#endif

#include "fdManager.h"
#include "snoopServer.h"
#include "ut.h"

// Interval for rate statistics in seconds
#define RATE_STATS_INTERVAL 1u
#define DELAY .01     // 10 ms

// Function prototypes
extern int main(int argc, const char **argv);
static void usage();

//
// main()
// (example single threaded ca server tool main loop)
//
extern int main(int argc, const char **argv)
{
    snoopRateStatsTimer *statTimer = NULL;
    double delay=DELAY;
    int doStats = 0;
    snoopServer *pCAS;
    int debugLevel = 0u;
    double executionTime;
    double waitTime = 0.0;
    int forever = 1;
    char *prefix = NULL;
    char *individualName = NULL;
    int nCheck = NCHECK_DEFAULT;
    int nPrint = NPRINT_DEFAULT;
    int nSigma = NSIGMA_DEFAULT;
    double nLimit = NLIMIT_DEFAULT;
    int i;
    
    for (i=1; i<argc; i++) {
	if(sscanf(argv[i],"-c %d", &nCheck)==1) {
	    continue;
	}
	if(sscanf(argv[i], "-d %d", &debugLevel)==1) {
	    continue;
	}
	if(!strcmp(argv[i], "-h")) {
	    usage();
	    return(0);
	}
	if(!strncmp(argv[i],"-i",2)) {
	    individualName=(char *)malloc(NAMESIZE*sizeof(char));
	    if(individualName) {
		strncpy(individualName,&argv[i][2],NAMESIZE);
		individualName[NAMESIZE-1]='\0';
	    }
	    continue;
	}
	if(sscanf(argv[i],"-l %lf", &nLimit)==1) {
	    continue;
	}
	if(sscanf(argv[i],"-p %d", &nPrint)==1) {
	    continue;
	}
	if(!strncmp(argv[i],"-n",2)) {
	    prefix=(char *)malloc(PREFIX_SIZE*sizeof(char));
	    if(prefix) {
		strncpy(prefix,&argv[i][2],PREFIX_SIZE);
		prefix[PREFIX_SIZE-1]='\0';
		doStats = 1;
	    }
	    continue;
	}
	if(sscanf(argv[i],"-s %d", &nSigma)==1) {
	    continue;
	}
	if(sscanf(argv[i],"-t %lf", &executionTime)==1) {
	    forever = aitFalse;
	    continue;
	}
	if(sscanf(argv[i],"-w %lf", &waitTime)==1) {
	    continue;
	}
	print("Unknown option: \"%s\"\n", argv[i]);
	usage();
	return(1);
    }
    
  // Start
    print("Starting %s at %s\n",CASNOOPER_VERSION_STRING,timeStamp());
    print("%s\n",EPICS_VERSION_STRING);

  // Create the server
    pCAS = new snoopServer(prefix,individualName,nCheck,nPrint,nSigma,nLimit);
    if(!pCAS) {
	print("Could not create server\n");
	return(-1);
    }
    pCAS->setDebugLevel(debugLevel);
    pCAS->disable();
    print("Individual Name is %s\n",
      pCAS->getIndividualName()?pCAS->getIndividualName():"NULL");
    if(doStats) {
	print("PV name prefix is %s\n",
	  pCAS->getPrefix()?pCAS->getPrefix():"NULL");
    } else {
	print("Internal PV names are not being published\n");
    }
    
  // Main loop
    osiTime begin(osiTime::getCurrent());
    
  // Loop here until the specified wait time
    double delay0=(double)(osiTime::getCurrent() - begin);
    double wait=waitTime;
#if DEBUG_TIME
    print("delay0=%g wait=%g\n",delay0,wait);
#endif
    while(delay0 < wait) {
#if EPICS_REVISION > 13
	fileDescriptorManager.process(delay0);
#else
	osiTime osiDelay(delay0);
	fileDescriptorManager.process(osiDelay);
#endif
	delay0=(double)(osiTime::getCurrent() - begin);
#if DEBUG_TIME
	print("delay0=%g wait=%g\n",delay0,wait);
#endif
    }

  // Initialize stat counters
    if(doStats) {
#if EPICS_REVISION > 13
      // Start a default timer queue
	epicsTimerQueueActive &queue = 
	  epicsTimerQueueActive::allocate(true);
	statTimer = new snoopRateStatsTimer(queue,RATE_STATS_INTERVAL,pCAS);
	if(statTimer) {
	  // Call the expire routine to initialize it
	    statTimer->expire(epicsTime::getCurrent());
	  // Then start the timer
	    statTimer->start();
	} else {
	    print("Could not start statistics timer\n");
	}
#else
	osiTime rateStatsDelay(RATE_STATS_INTERVAL,0u);
	statTimer = new snoopRateStatsTimer(rateStatsDelay,pCAS);
      // Call the expire routine to initialize it
	if(statTimer) {
	    statTimer->expire();
	} else {
	    print("Could not start statistics timer\n");
	}
#endif
    }
    
  // Start the processing
    double processedTime = 0.0;
    pCAS->enable();
    osiTime start(osiTime::getCurrent());
    while (aitTrue) {
#if EPICS_REVISION > 13
	fileDescriptorManager.process(delay);
#else
	osiTime osiDelay(delay);
	fileDescriptorManager.process(osiDelay);
#endif
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
    print("\nCaSnooper terminating after %.2f seconds [%.2f minutes]\n",
      elapsedTime,elapsedTime/60.);
    print("  Data collected for %.2f seconds [%.2f minutes]\n",
      processedTime,processedTime/60.);
#if 0
    print("\nServer Statistics:\n");
    pCAS->show(2u);
#endif

  // Do final report
    if(!forever) pCAS->report();
    delete pCAS;

    return(0);
}

void usage()
{
    print(
      "%s %s\n"
      "Usage: caSnooper [options]\n"
      "  Options:\n"
      "    -c<integer>  Check validity of top n requests (0 means all)\n"
      "    -d<integer>  Set debug level to n\n"
      "    -h           Help (This message)\n"
      "    -i<string>   Specify a PV name to watch individually\n"
      "    -l<decimal>  Print all requests over n Hz\n"
      "    -p<integer>  Print top n requests (0 means all)\n"
      "    -n[<string>] Make internal PV names available\n"
      "                   Use string as prefix for internal PV names\n"
      "                   (%d chars max length) Default string is: %s\n"
      "    -s<integer>  Print all requests over n sigma\n"
      "    -t<decimal>  Run n seconds, then print report\n"
      "    -w<decimal>  Wait n sec before collecting data\n"
      "\n", 
      CASNOOPER_VERSION_STRING,EPICS_VERSION_STRING,PREFIX_SIZE-1,DEFAULT_PREFIX);
}
