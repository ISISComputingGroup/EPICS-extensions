/***************************************************************
* StreamDevice Support                                         *
*                                                              *
* (C) 2005 Dirk Zimoch (dirk.zimoch@psi.ch)                    *
*                                                              *
* This is error and debug message handling of StreamDevice.    *
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

#include "StreamError.h"
#include <stdarg.h>
#include <string.h>
#include <time.h>

int streamDebug = 0;
FILE* StreamDebugFile = NULL;

/* You can globally change the printTimestamp function
   by setting the StreamPrintTimestampFunction variable
   to your own function.
*/
static void printTimestamp(FILE* file)
{
    time_t t;
    struct tm tm, *p_tm;
    char buffer [40];
    time(&t);
#ifdef _WIN32
    p_tm = localtime(&t);
#else
    localtime_r(&t, &tm);
	p_tm = &tm;
#endif /* _WIN32 */
    strftime(buffer, 40, "%Y/%m/%d %H:%M:%S ", p_tm);
    fprintf(file, buffer);
}

void (*StreamPrintTimestampFunction)(FILE* file) = printTimestamp;

void StreamError(const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    fprintf(stderr, "\033[31;1m");
    StreamPrintTimestampFunction(stderr);
    vfprintf(stderr, fmt, args);
    fprintf(stderr, "\033[0m");
    va_end(args);
    if (StreamDebugFile)
    {
        va_start(args, fmt);
        StreamPrintTimestampFunction(StreamDebugFile);
        vfprintf(StreamDebugFile, fmt, args);
        fflush(StreamDebugFile);
        va_end(args);
    }
}

int StreamDebugClass::
print(const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    const char* f = strrchr(file, '/');
    if (f) f++; else f = file;
    FILE* fp = StreamDebugFile ? StreamDebugFile : stderr;
    StreamPrintTimestampFunction(fp);
    fprintf(fp, "%s:%d: ", f, line);
    vfprintf(fp, fmt, args);
    fflush(fp);
    va_end(args);
    return 1;
}

