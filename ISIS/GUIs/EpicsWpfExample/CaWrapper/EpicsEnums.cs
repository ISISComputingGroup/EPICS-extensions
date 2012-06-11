using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace CaWrapper
{
    public enum Severity
    {
        CA_K_WARNING = 0,
        CA_K_SUCCESS = 1,
        CA_K_ERROR = 2,
        CA_K_INFO = 3,
        CA_K_SEVERE = 4,
        CA_K_FATAL = 6,
    }

    public enum ErrorCode
    {
        //Error codes are returned as ints, but in EPICS they are returned as uint16 where the three
        //least significant bits represent the severity.
        //So for ECA_NORMAL we get 00000001 => 00000 + 001 where the first part is the errorcode (0 = normal)
        //and the second part is the Severity (1 = success)
        //Likewise, for timeout we get 80 => 01010000 => 01010 + 000 => ECA_TIMEOUT (10) + CA_K_WARNING (0)
        //This is a bit of a pain in the neck and is why we have strange enum values
        ECA_NORMAL = 1,
        ECA_MAXIOC = 10,
        ECA_UKNHOST = 18,
        ECA_UKNSERV = 26,
        ECA_SOCK = 34,
        ECA_CONN = 40,
        ECA_ALLOCMEM = 48,
        ECA_UKNCHAN = 56,
        ECA_UKNFIELD = 64,
        ECA_TOLARGE = 72,
        ECA_TIMEOUT = 80,
        ECA_NOSUPPORT = 88,
        ECA_STRTOBIG = 96,
        ECA_DISCONNCHID = 106,
        ECA_BADTYPE = 114,
        ECA_CHIDNOTFND = 123,
        ECA_CHIDRETRY = 131,
        ECA_INTERNAL = 142,
        ECA_DBLCLFAIL = 144,
        ECA_GETFAIL = 152,
        ECA_PUTFAIL = 160,
        ECA_ADDFAIL = 168,
        ECA_BADCOUNT = 176,
        ECA_BADSTR = 184,
        ECA_DISCONN = 194,
        ECA_DBLCHNL = 200,
        ECA_EVDISALLOW = 210,
        ECA_BUILDGET = 216,
        ECA_NEEDSFP = 224,
        ECA_OVEVFAIL = 232,
        ECA_BADMONID = 242,
        ECA_NEWADDR = 248,
        ECA_NEWCONN = 262,
        ECA_NOCACTX = 264,
        ECA_DEFUNCT = 278,
        ECA_EMPTYSTR = 280,
        ECA_NOREPEATER = 288,
        ECA_NOCHANMSG = 296,
        ECA_DLCKREST = 304,
        ECA_SERVBEHIND = 312,
        ECA_NOCAST = 320,
        ECA_BADMASK = 330,
        ECA_IODONE = 339,
        ECA_IOINPROGRESS = 347,
        ECA_BADSYNCGRP = 354,
        ECA_PUTCBINPROG = 362,
        ECA_NORDACCESS = 368,
        ECA_NOWTACCESS = 376,
        ECA_ANACHRONISM = 386,
        ECA_NOSEARCHADDR = 392,
        ECA_NOCONVERT = 400,
        ECA_BADCHID = 410,
        ECA_BADFUNCPTR = 418,
        ECA_ISATTACHED = 424,
        ECA_UNAVAILINSERV = 432,
        ECA_CHANDESTROY = 440,
        ECA_BADPRIORITY = 450,
        ECA_NOTTHREADED = 458,
        ECA_16KARRAYCLIENT = 464,
        ECA_CONNSEQTMO = 472,
    }

    public enum SubscriptionMask
    {
        DBE_VALUE = 1,
        DBE_LOG = 2,
        DBE_ALARM = 4,
        DBE_PROPERTY = 8,
    }

    public enum ChannelType
    {
        TYPENOTCONN = -1,
        DBR_STRING = 0,
        DBR_INT = 1,
        DBR_SHORT = 1,
        DBR_FLOAT = 2,
        DBR_ENUM = 3,
        DBR_CHAR = 4,
        DBR_LONG = 5,
        DBR_DOUBLE = 6,
        DBR_STS_STRING = 7,
        DBR_STS_INT = 8,
        DBR_STS_SHORT = 8,
        DBR_STS_FLOAT = 9,
        DBR_STS_ENUM = 10,
        DBR_STS_CHAR = 11,
        DBR_STS_LONG = 12,
        DBR_STS_DOUBLE = 13,
        DBR_TIME_STRING = 14,
        DBR_TIME_INT = 15,
        DBR_TIME_SHORT = 15,
        DBR_TIME_FLOAT = 16,
        DBR_TIME_ENUM = 17,
        DBR_TIME_CHAR = 18,
        DBR_TIME_LONG = 19,
        DBR_TIME_DOUBLE = 20,
        DBR_GR_STRING = 21,
        DBR_GR_INT = 22,
        DBR_GR_SHORT = 22,
        DBR_GR_FLOAT = 23,
        DBR_GR_ENUM = 24,
        DBR_GR_CHAR = 25,
        DBR_GR_LONG = 26,
        DBR_GR_DOUBLE = 27,
        DBR_CTRL_STRING = 28,
        DBR_CTRL_INT = 29,
        DBR_CTRL_SHORT = 29,
        DBR_CTRL_FLOAT = 30,
        DBR_CTRL_ENUM = 31,
        DBR_CTRL_CHAR = 32,
        DBR_CTRL_LONG = 33,
        DBR_CTRL_DOUBLE = 34,
        DBR_STSACK_STRING = 37,
        DBR_CLASS_NAME = 38,
    }

    public enum ChannelState
    {
        cs_never_conn,
        cs_prev_conn,
        cs_conn,
        cs_closed,
    }
}
