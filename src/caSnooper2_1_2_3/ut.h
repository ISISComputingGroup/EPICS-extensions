/*************************************************************************\
* Copyright (c) 2002 The University of Chicago, as Operator of Argonne
* National Laboratory.
* Copyright (c) 2002 The Regents of the University of California, as
* Operator of Los Alamos National Laboratory.
* This file is distributed subject to a Software License Agreement found
* in the file LICENSE that is included with this distribution. 
\*************************************************************************/
// CaSnooper Utilities

// Function prototypes
int errMsg(const char *fmt, ...);
void hsort(double array[], unsigned long  indx[], unsigned long n);
void print(const char *fmt, ...);
char *timeStamp(void);
struct timespec *timeSpec(void);

