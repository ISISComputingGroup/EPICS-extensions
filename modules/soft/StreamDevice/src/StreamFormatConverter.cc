/***************************************************************
* StreamDevice Support                                         *
*                                                              *
* (C) 1999 Dirk Zimoch (zimoch@delta.uni-dortmund.de)          *
* (C) 2005 Dirk Zimoch (dirk.zimoch@psi.ch)                    *
*                                                              *
* This is the format converter base and includes the standard  *
* format converters for StreamDevice.                          *
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

#include "StreamFormatConverter.h"
#include "StreamFormat.h"
#include "StreamError.h"

StreamFormatConverter* StreamFormatConverter::
registered [256];

void StreamFormatConverter::
provides(const char* provided)
{
    const unsigned char* p;
    for (p = reinterpret_cast<const unsigned char*>(provided);
        *p; p++)
    {
        registered[*p] = this;
    }
}

int StreamFormatConverter::
printLong(const StreamFormat& fmt, StreamBuffer&, long)
{
    error("Unimplemented printLong method\n for %%%c format",
        fmt.conv);
    return false;
}

int StreamFormatConverter::
printDouble(const StreamFormat& fmt, StreamBuffer&, double)
{
    error("Unimplemented printDouble method for %%%c format\n",
        fmt.conv);
    return false;
}

int StreamFormatConverter::
printString(const StreamFormat& fmt, StreamBuffer&, const char*)
{
    error("Unimplemented printString method for %%%c format\n",
        fmt.conv);
    return false;
}

int StreamFormatConverter::
printPseudo(const StreamFormat& fmt, StreamBuffer&)
{
    error("Unimplemented printPseudo method for %%%c format\n",
        fmt.conv);
    return false;
}

int StreamFormatConverter::
scanLong(const StreamFormat& fmt, const char*, long&)
{
    error("Unimplemented scanLong method for %%%c format\n",
        fmt.conv);
    return -1;
}

int StreamFormatConverter::
scanDouble(const StreamFormat& fmt, const char*, double&)
{
    error("Unimplemented scanDouble method for %%%c format\n",
        fmt.conv);
    return -1;
}

int StreamFormatConverter::
scanString(const StreamFormat& fmt, const char*, char*, size_t)
{
    error("Unimplemented scanString method for %%%c format\n",
        fmt.conv);
    return -1;
}

int StreamFormatConverter::
scanPseudo(const StreamFormat& fmt, StreamBuffer&, long&)
{
    error("Unimplemented scanPseudo method for %%%c format\n",
        fmt.conv);
    return -1;
}

static void copyFormatString(StreamBuffer& info, const char* source)
{
    const char* p = source - 1;
    while (*p != '%' && *p != ')') p--;
    info.append('%');
    while (++p != source-1) info.append(*p);
}

// Standard Long Converter for 'diouxX'

class StreamStdLongConverter : public StreamFormatConverter
{
    int parse(const StreamFormat& fmt, StreamBuffer& output, const char*& value, bool scanFormat);
    int printLong(const StreamFormat& fmt, StreamBuffer& output, long value);
    int scanLong(const StreamFormat& fmt, const char* input, long& value);
};

int StreamStdLongConverter::
parse(const StreamFormat& fmt, StreamBuffer& info,
    const char*& source, bool scanFormat)
{
    if (scanFormat && (fmt.flags & alt_flag)) return false;
    copyFormatString(info, source);
    info.append('l');
    info.append(fmt.conv);
    if (scanFormat) info.append("%n");
    return long_format;
}

int StreamStdLongConverter::
printLong(const StreamFormat& fmt, StreamBuffer& output, long value)
{
    output.printf(fmt.info, value);
    return true;
}

int StreamStdLongConverter::
scanLong(const StreamFormat& fmt, const char* input, long& value)
{
    int length = -1;
    if (fmt.flags & skip_flag)
    {
        if (sscanf(input, fmt.info, &length) < 0) return -1;
    }
    else
    {
        if (sscanf(input, fmt.info, &value, &length) < 1) return -1;
    }
    return length;
}

RegisterConverter (StreamStdLongConverter, "diouxX");

// Standard Double Converter for 'feEgG'

class StreamStdDoubleConverter : public StreamFormatConverter
{
    virtual int parse(const StreamFormat&, StreamBuffer&, const char*&, bool);
    virtual int printDouble(const StreamFormat&, StreamBuffer&, double);
    virtual int scanDouble(const StreamFormat&, const char*, double&);
};

int StreamStdDoubleConverter::
parse(const StreamFormat& fmt, StreamBuffer& info,
    const char*& source, bool scanFormat)
{
    if (scanFormat && (fmt.flags & alt_flag))
    {
        error("Use of modifier '#' not allowed with %%%c input conversion\n",
            fmt.conv);
        return false;
    }
    copyFormatString(info, source);
    if (scanFormat) info.append('l');
    info.append(fmt.conv);
    if (scanFormat) info.append("%n");
    return double_format;
}

int StreamStdDoubleConverter::
printDouble(const StreamFormat& fmt, StreamBuffer& output, double value)
{
    output.printf(fmt.info, value);
    return true;
}

int StreamStdDoubleConverter::
scanDouble(const StreamFormat& fmt, const char* input, double& value)
{
    int length = -1;
    if (fmt.flags & skip_flag)
    {
        if (sscanf(input, fmt.info, &length) < 0) return -1;
    }
    else
    {
        if (sscanf(input, fmt.info, &value, &length) < 1) return -1;
    }
    return length;
}

RegisterConverter (StreamStdDoubleConverter, "feEgG");

// Standard String Converter for 's'

class StreamStdStringConverter : public StreamFormatConverter
{
    virtual int parse(const StreamFormat&, StreamBuffer&, const char*&, bool);
    virtual int printString(const StreamFormat&, StreamBuffer&, const char*);
    virtual int scanString(const StreamFormat&, const char*, char*, size_t);
};

int StreamStdStringConverter::
parse(const StreamFormat& fmt, StreamBuffer& info,
    const char*& source, bool scanFormat)
{
    if (fmt.flags & (sign_flag|space_flag|zero_flag|alt_flag))
    {
        error("Use of modifiers '+', ' ', '0', '#' "
            "not allowed with %%%c conversion\n",
            fmt.conv);
        return false;
    }
    copyFormatString(info, source);
    info.append(fmt.conv);
    if (scanFormat) info.append("%n");
    return string_format;
}

int StreamStdStringConverter::
printString(const StreamFormat& fmt, StreamBuffer& output, const char* value)
{
    output.printf(fmt.info, value);
    return true;
}

int StreamStdStringConverter::
scanString(const StreamFormat& fmt, const char* input,
    char* value, size_t maxlen)
{
    int length = -1;
    if (*input == '\0')
    {
        // match empty string
        value[0] = '\0';
        return 0;
    }
    if (fmt.flags & skip_flag)
    {
        if (sscanf(input, fmt.info, &length) < 0) return -1;
    }
    else
    {
        char tmpformat[10];
        const char* f;
        if (maxlen <= fmt.width || fmt.width == 0)
        {
            // assure not to read too much
            sprintf(tmpformat, "%%%d%c%%n", maxlen-1, fmt.conv);
            f = tmpformat;
        }
        else
        {
            f = fmt.info;
        }
        if (sscanf(input, f, value, &length) < 1) return -1;
        if (length < 0) return -1;
        value[length] = '\0';
#ifndef NO_TEMPORARY
        debug("StreamStdStringConverter::scanString: length=%d, value=%s\n",
            length, StreamBuffer(value,length).expand()());
#endif
    }
    return length;
}

RegisterConverter (StreamStdStringConverter, "s");

// Standard Characters Converter for 'c'

class StreamStdCharsConverter : public StreamStdStringConverter
{
    virtual int parse(const StreamFormat&, StreamBuffer&, const char*&, bool);
    virtual int printLong(const StreamFormat&, StreamBuffer&, long);
    // scanString is inherited from %s format
};

int StreamStdCharsConverter::
parse(const StreamFormat& fmt, StreamBuffer& info,
    const char*& source, bool scanFormat)
{
    if (fmt.flags & (sign_flag|space_flag|zero_flag|alt_flag))
    {
        error("Use of modifiers '+', ' ', '0', '#' "
            "not allowed with %%%c conversion\n",
            fmt.conv);
        return false;
    }
    copyFormatString(info, source);
    info.append(fmt.conv);
    if (scanFormat)
    {
        info.append("%n");
        return string_format;
    }
    return long_format;
}

int StreamStdCharsConverter::
printLong(const StreamFormat& fmt, StreamBuffer& output, long value)
{
    output.printf(fmt.info, value);
    return true;
}

RegisterConverter (StreamStdCharsConverter, "c");

// Standard Charset Converter for '['

class StreamStdCharsetConverter : public StreamFormatConverter
{
    virtual int parse(const StreamFormat&, StreamBuffer&, const char*&, bool);
    virtual int scanString(const StreamFormat&, const char*, char*, size_t);
    // no print method, %[ is readonly
};

int StreamStdCharsetConverter::
parse(const StreamFormat& fmt, StreamBuffer& info,
    const char*& source, bool scanFormat)
{
    if (!scanFormat)
    {
        error("Format conversion %%[ is only allowed in input formats\n");
        return false;
    }
    if (fmt.flags & (left_flag|sign_flag|space_flag|zero_flag|alt_flag))
    {
        error("Use of modifiers '-', '+', ' ', '0', '#'"
            "not allowed with %%%c conversion\n",
            fmt.conv);
        return false;
    }
    info.printf("%%%d[", fmt.width);
    while (*source && *source != ']')
    {
        if (*source == esc) source++;
        info.append(*source++);
    }
    if (!*source) {
        error("Missing ']' after %%[ format conversion\n");
        return false;
    }
    source++; // consume ']'
    info.append("]%n");
    return string_format;
}

int StreamStdCharsetConverter::
scanString(const StreamFormat& fmt, const char* input,
    char* value, size_t maxlen)
{
    int length = -1;
    if (fmt.flags & skip_flag)
    {
        if (sscanf (input, fmt.info, &length) < 0) return -1;
    }
    else
    {
        char tmpformat[256];
        const char* f;
        if (maxlen <= fmt.width || fmt.width == 0)
        {
            const char *p = strchr (fmt.info, '[');
            // assure not to read too much
            sprintf(tmpformat, "%%%d%s", maxlen-1, p);
            f = tmpformat;
        }
        else
        {
            f = fmt.info;
        }
        if (sscanf(input, f, value, &length) < 1) return -1;
        if (length < 0) return -1;
        value[length] = '\0';
        debug("StreamStdCharsetConverter::scanString: length=%d, value=%s\n",
            length, value);
    }
    return length;
}

RegisterConverter (StreamStdCharsetConverter, "[");
