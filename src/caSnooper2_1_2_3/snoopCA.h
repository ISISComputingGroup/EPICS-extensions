/*************************************************************************\
* Copyright (c) 2002 The University of Chicago, as Operator of Argonne
* National Laboratory.
* Copyright (c) 2002 The Regents of the University of California, as
* Operator of Los Alamos National Laboratory.
* This file is distributed subject to a Software License Agreement found
* in the file LICENSE that is included with this distribution. 
\*************************************************************************/
/* snoopClient.h: Modifications to CA */

#ifdef __cplusplus
#define NU(x)
#else
#define NU(x)=x
#endif

#ifdef ALLOCATE_STORAGE
#define EXTERN
#else
#define EXTERN extern
#endif

#include "bsdSocketResource.h"
#include "caProto.h"

#ifdef __cplusplus
extern "C" {
#endif

EXTERN unsigned long udpCount;
EXTERN void (*udpMsgHook)(const struct sockaddr_in *net_addr,
  const struct sockaddr_in *in_addr, ca_uint16_t cmd);
    
#ifdef __cplusplus
}  /* Close scope of 'extern "C"' declaration */
#endif
