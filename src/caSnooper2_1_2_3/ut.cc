/*************************************************************************\
* Copyright (c) 2002 The University of Chicago, as Operator of Argonne
* National Laboratory.
* Copyright (c) 2002 The Regents of the University of California, as
* Operator of Los Alamos National Laboratory.
* This file is distributed subject to a Software License Agreement found
* in the file LICENSE that is included with this distribution. 
\*************************************************************************/
// Global utilities

#define LPRINTF_SIZE 1024     // Danger: Fixed size

#include "snoopServer.h"

// errMsg //////////////////////////////////////////////////////////////
int errMsg(const char *fmt, ...)
{
    va_list vargs;
    static char lstring[LPRINTF_SIZE];
    
    va_start(vargs,fmt);
    (void)vsprintf(lstring,fmt,vargs);
    va_end(vargs);
    
    if(lstring[0] != '\0') {
	fprintf(stderr,"%s\n",lstring);
	fflush(stderr);
    }
    
    return 0;
}

// hsort ///////////////////////////////////////////////////////////////
// Heap sort routine.  Sorts an array of length n into descending order
// and puts the sorted indices in indx.
void hsort(double array[], unsigned long  indx[], unsigned long n)
{
    unsigned long l,j,ir,indxt,i;
    double q;

  // All done if none or one element
    if(n == 0) return;
    if(n == 1) {
	indx[0]=0;
	return;
    }
    
  // Initialize indx array
    for(j=0; j < n; j++) indx[j]=j;
  // Loop over elements
    l=(n>>1);
    ir=n-1;
    for(;;) {
	if(l > 0) q=array[(indxt=indx[--l])];
	else {
	    q=array[(indxt=indx[ir])];
	    indx[ir]=indx[0];
	    if(--ir == 0) {
		indx[0]=indxt;
		return;
	    }
	}
	i=l;
	j=(l<<1)+1;
	while(j <= ir) {
	    if(j < ir && array[indx[j]] > array[indx[j+1]]) j++;
	    if(q > array[indx[j]]) {
		indx[i]=indx[j];
		j+=((i=j)+1);
		
	    }
	    else break;
	}
	indx[i]=indxt;
    }
}

// timeStamp ///////////////////////////////////////////////////////////
char *timeStamp(void)
  // Gets current time and puts it in a static array
  // The calling program should copy it to a safe place
  //   e.g. strcpy(savetime,timestamp());
{
    static char timeStampStr[16];
    long now;
    struct tm *tblock;
    
    time(&now);
    tblock=localtime(&now);
    strftime(timeStampStr,20,"%b %d %H:%M:%S",tblock);
    
    return timeStampStr;
}

// print ///////////////////////////////////////////////////////////////
void print(const char *fmt, ...)
{
    va_list vargs;
    static char lstring[1024];  /* DANGER: Fixed buffer size */
    
    va_start(vargs,fmt);
    vsprintf(lstring,fmt,vargs);
    va_end(vargs);
    
    if(lstring[0] != '\0') {
	printf("%s",lstring);
    }
    fflush(stdout);
}

// timeSpec ////////////////////////////////////////////////////////////
struct timespec *timeSpec(void)
  // Gets current time and puts it in a static timespec struct
  // For use by gdd::setTimeStamp, which will copy it
{
    static struct timespec ts;
    osiTime osit(osiTime::getCurrent());
  // EPICS is 20 years ahead of its time
#if EPICS_REVISION > 13
    ts=osit;
#else
    ts.tv_sec=(time_t)osit.getSecTruncToLong()-631152000ul;
    ts.tv_nsec=osit.getNSecTruncToLong();
#endif
    
    return &ts;
}

