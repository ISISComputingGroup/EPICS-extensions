/***************************************************************
* StreamDevice Support                                         *
*                                                              *
* (C) 1999 Dirk Zimoch (zimoch@delta.uni-dortmund.de)          *
* (C) 2005 Dirk Zimoch (dirk.zimoch@psi.ch)                    *
*                                                              *
* This is the header for the EPICS interface to StreamDevice.  *
* Please refer to the HTML files in ../doc/ for a detailed     *
* documentation.                                               *
*                                                              *
* If you do any changes in this file, you are not allowed to   *
* redistribute it any more. If there is a bug or a missing     *
* feature, send me an email and/or your patch. If I accept     *
* your changes, they will go to the next release.              *
*                                                              *
* DISCLAIMER: If this software breaks something or harms       *
* someone, it's your problem.                                  *
*                                                              *
***************************************************************/

#ifndef devStream_h
#define devStream_h

#define STREAM_MAJOR 2
#define STREAM_MINOR 1

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
extern const char StreamVersion[];
#ifdef __cplusplus
}
#endif /* __cplusplus */

#if defined(__vxworks) || defined(vxWorks)
#include <vxWorks.h>
#endif

#include <stdio.h>
#include <epicsVersion.h>
#include <dbCommon.h>
#include <dbScan.h>
#include <devSup.h>
#include <dbFldTypes.h>
#include <dbAccess.h>
#include <epicsVersion.h>

#if (EPICS_VERSION==3 && EPICS_REVISION>=14)
#include <epicsExport.h>
#else
#define epicsExportAddress(a,b);
#endif

#ifndef OK
#define OK 0
#endif

#ifndef ERROR
#define ERROR -1
#endif

#define DO_NOT_CONVERT 2
#define INIT_RUN (!interruptAccept)

#ifdef _WIN32
#ifdef STREAM_EXPORTS
#define STREAM_DLL_EXPORT __declspec(dllexport)
#else
#define STREAM_DLL_EXPORT __declspec(dllimport)
#endif
#else
#define STREAM_DLL_EXPORT
#endif
extern STREAM_DLL_EXPORT FILE* StreamDebugFile;

#ifdef __cplusplus
extern "C" {
#endif

typedef const struct format_s {
    unsigned char type;
    const struct StreamFormat* priv;
} format_t;

typedef long (*streamIoFunction) (dbCommon*, format_t*);

long streamInit(int after);
long streamInitRecord(dbCommon *record, struct link *ioLink,
    streamIoFunction readData, streamIoFunction writeData);
long streamReport(int interest);
long streamReadWrite(dbCommon *record);
long streamGetIointInfo(int cmd, dbCommon *record, IOSCANPVT *ppvt);
long streamPrintf(dbCommon *record, format_t *format, ...);
long streamScanSep(dbCommon *record);
long streamScanfN(dbCommon *record, format_t *format,
    void*, size_t maxStringSize);

/* backward compatibility stuff */
#define devStreamIoFunction streamIoFunction
#define devStreamInit streamInit
#define devStreamInitRecord streamInitRecord
#define devStreamReport streamReport
#define devStreamRead streamReadWrite
#define devStreamWrite streamReadWrite
#define devStreamGetIointInfo streamGetIointInfo
#define devStreamPrintf streamPrintf
#define devStreamPrintSep(record) (0)
#define devStreamScanSep streamScanSep
#define devStreamScanf(record, format, value) \
    streamScanfN(record, format, value, MAX_STRING_SIZE)
#define streamScanf(record, format, value) \
    streamScanfN(record, format, value, MAX_STRING_SIZE)
#define streamRead streamReadWrite
#define streamWrite streamReadWrite
#define streamReport NULL

#ifdef __cplusplus
}
#endif

#endif
