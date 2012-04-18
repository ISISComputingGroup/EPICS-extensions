/***************************************************************
* StreamDevice Support                                         *
*                                                              *
* (C) 1999 Dirk Zimoch (zimoch@delta.uni-dortmund.de)          *
* (C) 2005 Dirk Zimoch (dirk.zimoch@psi.ch)                    *
*                                                              *
* This is the kernel of StreamDevice.                          *
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

#include "StreamCore.h"
#include "StreamError.h"
#include <ctype.h>
#include <stdlib.h>

enum Commands { end_cmd, in_cmd, out_cmd, wait_cmd, event_cmd, exec_cmd };
const char* commandStr[] = { "end", "in", "out", "wait", "event", "exec" };

inline const char* commandName(unsigned char i)
{
    return i > exec_cmd ? "invalid" : commandStr[i];
}

/// debug functions /////////////////////////////////////////////

static char* printCommands(StreamBuffer& buffer, const char* c)
{
    unsigned long timeout;
    unsigned long eventnumber;
    unsigned short cmdlen;
    while (1)
    {
        switch(*c++)
        {
            case end_cmd:
                return buffer();
            case in_cmd:
                buffer.append("    in \"");
                c = StreamProtocolParser::printString(buffer, c);
                buffer.append("\";\n");
                break;
            case out_cmd:
                buffer.append("    out \"");
                c = StreamProtocolParser::printString(buffer, c);
                buffer.append("\";\n");
                break;
            case wait_cmd:
                timeout = extract<unsigned long>(c);
                buffer.printf("    wait %ld;\n", timeout);
                break;
            case event_cmd:
                eventnumber = extract<unsigned long>(c);
                timeout = extract<unsigned long>(c);
                buffer.printf ("    event(%ld) %ld;\n", eventnumber, timeout);
                break;
            case exec_cmd:
                buffer.append("    exec \"");
                cmdlen = extract<unsigned short>(c);
                c = StreamProtocolParser::printString(buffer, c);
                buffer.append("\";\n");
                break;
            default:
                buffer.append("\033[31;1mGARBAGE: ");
                c = StreamProtocolParser::printString(buffer, c-1);
                buffer.append("\033[0m\n");
        }
    }
}

void StreamCore::
printProtocol()
{
    StreamBuffer buffer;
    printf("%s {\n", protocolname());
    printf("  extraInput    = %s;\n",
      (flags & IgnoreExtraInput) ? "ignore" : "error");
    printf("  lockTimeout   = %ld; # ms\n", lockTimeout);
    printf("  readTimeout   = %ld; # ms\n", readTimeout);
    printf("  replyTimeout  = %ld; # ms\n", replyTimeout);
    printf("  writeTimeout  = %ld; # ms\n", writeTimeout);
    printf("  pollPeriod    = %ld; # ms\n", pollPeriod);
    printf("  maxInput      = %ld; # bytes\n", maxInput);
    StreamProtocolParser::printString(buffer.clear(), inTerminator());
    printf("  inTerminator  = \"%s\";\n", buffer());
        StreamProtocolParser::printString(buffer.clear(), outTerminator());
    printf("  outTerminator = \"%s\";\n", buffer());
        StreamProtocolParser::printString(buffer.clear(), separator());
    printf("  separator     = \"%s\";\n", buffer());
    if (onInit)
        printf("  @Init {\n%s  }\n",
        printCommands(buffer.clear(), onInit()));
    if (onReplyTimeout)
        printf("  @ReplyTimeout {\n%s  }\n",
        printCommands(buffer.clear(), onReplyTimeout()));
    if (onReadTimeout)
        printf("  @ReadTimeout {\n%s  }\n",
        printCommands(buffer.clear(), onReadTimeout()));
    if (onWriteTimeout)
        printf("  @WriteTimeout {\n%s  }\n",
        printCommands(buffer.clear(), onWriteTimeout()));
    if (onMismatch)
        printf("  @Mismatch {\n%s  }\n",
        printCommands(buffer.clear(), onMismatch()));
    debug("StreamCore::printProtocol: commands=%s\n", commands.expand()());
    printf("\n%s}\n",
        printCommands(buffer.clear(), commands()));
}

///////////////////////////////////////////////////////////////////////////

StreamCore* StreamCore::first = NULL;

StreamCore::
StreamCore()
{
    businterface = NULL;
    flags = None;
    next = NULL;
    unparsedInput = false;
    // add myself to list of streams
    StreamCore** pstream;
    for (pstream = &first; *pstream; pstream = &(*pstream)->next);
    *pstream = this;
}

StreamCore::
~StreamCore()
{
    debug("~StreamCore(%s) %p\n", name(), this);
    releaseBus();
    // remove myself from list of all streams
    StreamCore** pstream;
    for (pstream = &first; *pstream; pstream = &(*pstream)->next)
    {
        if (*pstream == this)
        {
            *pstream = next;
            break;
        }
    }
}

bool StreamCore::
attachBus(const char* busname, int addr, const char* param)
{
    releaseBus();
    businterface = StreamBusInterface::find(this, busname, addr, param);
    if (!businterface)
    {
        error("Businterface '%s' not found for '%s'\n",
            busname, name());
        return false;
    }
    debug("StreamCore::attachBus(busname=\"%s\", addr=%i, param=\"%s\") businterface=%p\n",
        busname, addr, param, businterface);
    return true;
}

void StreamCore::
releaseBus()
{
    if (businterface)
    {
        if (flags & BusOwner)
        {
            busUnlock();
        }
        busRelease();
        businterface = NULL;
    }
}

// Parse the protocol

bool StreamCore::
parse(const char* filename, const char* _protocolname)
{
    protocolname = _protocolname;
    // extract substitutions from protocolname "name(sub1,sub2)"
    int i = protocolname.find('(');
    if (i >= 0)
    {
        while (i >= 0)
        {
            protocolname[i] = '\0'; // replace '(' and ',' with '\0'
            i = protocolname.find(',', i+1);
        }
        // should have closing parentheses
        if (protocolname[-1] != ')')
        {
            error("Missing ')' after substitutions '%s'\n", _protocolname);
            return false;
        }
        protocolname.truncate(-1); // remove ')'
    }
    StreamProtocolParser::Protocol* protocol;
    protocol = StreamProtocolParser::getProtocol(filename, protocolname);
    if (!protocol)
    {
        error("while reading protocol '%s' for '%s'\n", protocolname(), name());
        return false;
    }
    if (!compile(protocol))
    {
        delete protocol;
        error("while compiling protocol '%s' for '%s'\n", _protocolname, name());
        return false;
    }
    delete protocol;
    return true;
}

bool StreamCore::
compile(StreamProtocolParser::Protocol* protocol)
{
    const char* extraInputNames [] = {"error", "ignore", NULL};

    // default values for protocol variables
    flags &= ~IgnoreExtraInput;
    lockTimeout = 5000;
    readTimeout = 100;
    replyTimeout = 1000;
    writeTimeout = 100;
    maxInput = 0;
    pollPeriod = 1000;
    
    unsigned short ignoreExtraInput = false;
    if (!protocol->getEnumVariable("extrainput", ignoreExtraInput,
        extraInputNames))
    {
        return false;
    }
    if (ignoreExtraInput) flags |= IgnoreExtraInput;
    if (!(protocol->getNumberVariable("locktimeout", lockTimeout) &&
        protocol->getNumberVariable("readtimeout", readTimeout) &&
        protocol->getNumberVariable("replytimeout", replyTimeout) &&
        protocol->getNumberVariable("writetimeout", writeTimeout) &&
        protocol->getNumberVariable("maxinput", maxInput) &&
        // use replyTimeout as default for pollPeriod
        protocol->getNumberVariable("replytimeout", pollPeriod) &&
        protocol->getNumberVariable("pollperiod", pollPeriod)))
    {
        return false;
    }
    if (!(protocol->getStringVariable("terminator", inTerminator) &&
        protocol->getStringVariable("terminator", outTerminator) &&
        protocol->getStringVariable("interminator", inTerminator) &&
        protocol->getStringVariable("outterminator", outTerminator) &&
        protocol->getStringVariable("separator", separator)))
    {
        return false;
    }
    if (!(protocol->getCommands(NULL, commands, this) &&
        protocol->getCommands("@init", onInit, this) &&
        protocol->getCommands("@writetimeout", onWriteTimeout, this) &&
        protocol->getCommands("@replytimeout", onReplyTimeout, this) &&
        protocol->getCommands("@readtimeout", onReadTimeout, this) &&
        protocol->getCommands("@mismatch", onMismatch, this)))
    {
        return false;
    }
    return protocol->checkUnused();
}

bool StreamCore::
compileCommand(StreamProtocolParser::Protocol* protocol,
    StreamBuffer& buffer, const char* command, const char*& args)
{
    unsigned long timeout = 0;

    if (strcmp(command, commandStr[in_cmd]) == 0)
    {
        buffer.append(in_cmd);
        if (!protocol->compileString(buffer, args,
            ScanFormat, this))
        {
            return false;
        }
        buffer.append(StreamProtocolParser::eos);
        return true;
    }
    if (strcmp(command, commandStr[out_cmd]) == 0)
    {
        buffer.append(out_cmd);
        if (!protocol->compileString(buffer, args,
            PrintFormat, this))
        {
            return false;
        }
        buffer.append(StreamProtocolParser::eos);
        return true;
    }
    if (strcmp(command, commandStr[wait_cmd]) == 0)
    {
        buffer.append(wait_cmd);
        if (!protocol->compileNumber(timeout, args))
        {
            return false;
        }
        buffer.append(&timeout, sizeof(timeout));
        return true;
    }
    if (strcmp(command, commandStr[event_cmd]) == 0)
    {
        if (!busSupportsEvent())
        {
            protocol->errorMsg(getLineNumber(command),
                    "Events not supported by businterface.\n");
            return false;
        }
        unsigned long eventmask = 0xffffffff;
        buffer.append(event_cmd);
        if (*args == '(')
        {
            if (!protocol->compileNumber(eventmask, ++args))
            {
                return false;
            }
            if (*args != ')')
            {
                protocol->errorMsg(getLineNumber(command),
                    "Expect ')' instead of: '%s'\n", args);
                return false;
            }
            args++;
        }
        buffer.append(&eventmask, sizeof(eventmask));
        if (*args)
        {
            if (!protocol->compileNumber(timeout, args))
            {
                return false;
            }
        }
        buffer.append(&timeout, sizeof(timeout));
        return true;
    }
    if (strcmp(command, commandStr[exec_cmd]) == 0)
    {
        buffer.append(exec_cmd);
        if (!protocol->compileString(buffer, args,
            NoFormat, this))
        {
            return false;
        }
        buffer.append(StreamProtocolParser::eos);
        return true;
    }
    protocol->errorMsg(getLineNumber(command),
        "Unknown command name '%s'\n", command);
    return false;
}

// Run the protocol

bool StreamCore::
startProtocol(StartMode startMode)
{
    MutexLock lock(this);
    debug("StreamCore::startProtocol(%s, startMode=%s)\n", name(),
        startMode == StartNormal ? "StartNormal" :
        startMode == StartInit ? "StartInit" :
        startMode == StartAsync ? "StartAsync" : "Invalid");
    if (!businterface)
    {
        error("%s: No businterface attached\n", name());
        return false;
    }
    flags &= ~ClearOnStart;
    switch (startMode)
    {
        case StartInit:
            flags |= InitRun;
            break;
        case StartAsync:
            if (!busSupportsAsyncRead())
            {
                error("%s: Businterface does not support async mode\n", name());
                return false;
            }
            flags |= AsyncMode;
            break;
        case StartNormal:
            break;
    }
    if (!commands)
    {
        error ("%s: No protocol loaded\n", name());
        return false;
    }
    commandIndex = (startMode == StartInit) ? onInit() : commands();
    runningHandler = Success;
    busSetEos(inTerminator(), inTerminator.length());
    protocolStartHook();
    return evalCommand();
}

void StreamCore::
finishProtocol(ProtocolResult status)
{
    if (flags & BusPending)
    {
        error("StreamCore::finishProtocol(%s): Still waiting for %s%s%s\n",
            name(),
            flags & LockPending ? "lockSuccess() " : "",
            flags & WritePending ? "writeSuccess() " : "",
            flags & WaitPending ? "timerCallback()" : "");
        status = Fault;
    }
    flags &= ~(AcceptInput|AcceptEvent);
    if (runningHandler)
    {
        // get original error status
        if (status == Success) status = runningHandler;
    }
    else
    {
        // look for error handler
        char* handler;
        switch (status)
        {
            case WriteTimeout:
                handler = onWriteTimeout();
                break;
            case ReplyTimeout:
                handler = onReplyTimeout();
                break;
            case ReadTimeout:
                handler = onReadTimeout();
                break;
            case Abort:
                // cancel anything running
            case Fault:
                // get rid of all the rubbish whe might have collected
                inputBuffer.clear();
            default:
                handler = NULL;
        }
        if (handler)
        {
            // save original error status
            runningHandler = status;
            // execute handler
            commandIndex = handler;
            evalCommand();
            return;
        }
    }
    debug("StreamCore::finishProtocol(%s, status=%s) %sbus owner\n",
        name(),
        status==0 ? "Success" :
        status==1 ? "LockTimeout" :
        status==2 ? "WriteTimeout" :
        status==3 ? "ReplyTimeout" :
        status==4 ? "ReadTimeout" :
        status==5 ? "ScanError" :
        status==6 ? "FormatError" :
        status==7 ? "Abort" :
        status==8 ? "Fault" : "Invalid",
        flags & BusOwner ? "" : "not ");
    if (flags & BusOwner)
    {
        busUnlock();
        flags &= ~BusOwner;
    }
    protocolFinishHook(status);
}

bool StreamCore::
evalCommand()
{
    if (flags & BusPending)
    {
        error("StreamCore::evalCommand(%s): Still waiting for %s%s%s",
            name(),
            flags & LockPending ? "lockSuccess() " : "",
            flags & WritePending ? "writeSuccess() " : "",
            flags & WaitPending ? "timerCallback()" : "");
        return false;
    }
    activeCommand = commandIndex;
    debug("StreamCore::evalCommand(%s): activeCommand = %s\n",
        name(), commandName(*activeCommand));
    switch (*commandIndex++)
    {
        case out_cmd:
            flags &= ~(AcceptInput|AcceptEvent);
            return evalOut();
        case in_cmd:
            flags &= ~AcceptEvent;
            return evalIn();
        case wait_cmd:
            flags &= ~(AcceptInput|AcceptEvent);
            return evalWait();
        case event_cmd:
            flags &= ~AcceptInput;
            return evalEvent();
        case exec_cmd:
            return evalExec();
        case end_cmd:
            finishProtocol(Success);
            return true;
        default:
            error("INTERNAL ERROR (%s): illegal command code 0x%02x\n",
                name(), *activeCommand);
            flags &= ~BusPending;
            finishProtocol(Fault);
            return false;
    }
}

// Handle 'out' command

bool StreamCore::
evalOut()
{
    inputBuffer.clear(); // flush all unread input
    unparsedInput = false;
    outputLine.clear();
    if (!formatOutput())
    {
        finishProtocol(FormatError);
        return false;
    }
    outputLine.append(outTerminator);
    debug ("StreamCore::evalOut: outputLine = \"%s\"\n", outputLine.expand()());
    if (*commandIndex == in_cmd)  // prepare for early input
    {
        flags |= AcceptInput;
    }
    if (*commandIndex == event_cmd)  // prepare for early event
    {
        flags |= AcceptEvent;
    }
    if (!(flags & BusOwner))
    {
        debug ("StreamCore::evalOut(%s): lockRequest(%li)\n",
            name(), flags & InitRun ? 0 : lockTimeout);
        flags |= LockPending;
        if (!busLockRequest(flags & InitRun ? 0 : lockTimeout))
        {
            return false;
        }
        return true;
    }
    flags |= WritePending;
    if (!busWriteRequest(outputLine(), outputLine.length(), writeTimeout))
    {
        return false;
    }
    return true;
}

bool StreamCore::
formatOutput()
{
    char command;
    const char* fieldName = NULL;
    const char* formatstring;
    while ((command = *commandIndex++) != StreamProtocolParser::eos)
    {
        switch (command)
        {
            case StreamProtocolParser::format_field:
            {
                debug("StreamCore::formatOutput(%s): StreamProtocolParser::format_field\n",
                    name());
                // code layout:
                // field <StreamProtocolParser::eos> addrlen AddressStructure formatstring <StreamProtocolParser::eos> StreamFormat [info]
                fieldName = commandIndex;
                commandIndex += strlen(commandIndex)+1;
                unsigned short addrlen = extract<unsigned short>(commandIndex);
                fieldAddress.set(commandIndex, addrlen);
                commandIndex += addrlen;
            }
            case StreamProtocolParser::format:
            {
                // code layout:
                // formatstring <StreamProtocolParser::eos> StreamFormat [info]
                formatstring = commandIndex;
                while (*commandIndex++); // jump after <StreamProtocolParser::eos>
                StreamFormat fmt = extract<StreamFormat>(commandIndex);
                fmt.info = commandIndex; // point to info string
                commandIndex += fmt.infolen;
                debug("StreamCore::formatOutput(%s): format = %%%s\n",
                    name(), formatstring);
                if (fmt.type == pseudo_format)
                {
                    if (!StreamFormatConverter::find(fmt.conv)->
                        printPseudo(fmt, outputLine))
                    {
                        error("%s: Can't print pseudo value '%%%s'\n",
                            name(), formatstring);
                        return false;
                    }
                    continue;
                }
                flags &= ~Separator;
                if (!formatValue(fmt, fieldAddress ? fieldAddress() : NULL))
                {
                    if (fieldName)
                        error("%s: Can't format field '%s' with '%%%s'\n",
                            name(), fieldName, formatstring);
                    else
                        error("%s: Can't format value with '%%%s'\n",
                            name(), formatstring);
                    return false;
                }
                fieldAddress.clear();
                fieldName = NULL;
                continue;
            }
            case esc:
                // escaped literal byte
                command = *commandIndex++;
            default:
                // literal byte
                outputLine.append(command);
        }
    }
    return true;
}

void StreamCore::
printSeparator()
{
    if (!(flags & Separator))
    {
        flags |= Separator;
        return;
    }
    if (!separator) return;
    long i = 0;
    if (separator[0] == ' ') i++; // ignore leading space
    for (; i < separator.length(); i++)
    {
        if (separator[i] == StreamProtocolParser::skip) continue; // wildcard
        if (separator[i] == esc) i++;       // escaped literal byte
        outputLine.append(separator[i]);
    }
}

bool StreamCore::
printValue(const StreamFormat& fmt, long value)
{
    if (fmt.type != long_format && fmt.type != enum_format)
    {
        error("%s: printValue(long) called with %%%c format\n",
            name(), fmt.conv);
        return false;
    }
    printSeparator();
    return StreamFormatConverter::find(fmt.conv)->
        printLong(fmt, outputLine, value);
}

bool StreamCore::
printValue(const StreamFormat& fmt, double value)
{
    if (fmt.type != double_format)
    {
        error("%s: printValue(double) called with %%%c format\n",
            name(), fmt.conv);
        return false;
    }
    printSeparator();
    return StreamFormatConverter::find(fmt.conv)->
        printDouble(fmt, outputLine, value);
}

bool StreamCore::
printValue(const StreamFormat& fmt, char* value)
{
    if (fmt.type != string_format)
    {
        error("%s: printValue(char*) called with %%%c format\n",
            name(), fmt.conv);
        return false;
    }
    printSeparator();
    return StreamFormatConverter::find(fmt.conv)->
        printString(fmt, outputLine, value);
}

void StreamCore::
lockCallback(StreamBusInterface::IoStatus status)
{
    MutexLock lock(this);
    debug("StreamCore::lockCallback(%s, status=%s)\n",
        name(), status ? "Timeout" : "Success");
    if (!(flags & LockPending))
    {
        error("StreamCore::lockCallback(%s) called unexpectedly\n",
            name());
        return;
    }
    flags &= ~LockPending;
    flags |= BusOwner;
    if (status != StreamBusInterface::ioSuccess)
    {
        finishProtocol(LockTimeout);
        return;
    }
    flags |= WritePending;
    if (!busWriteRequest(outputLine(), outputLine.length(), writeTimeout))
    {
        finishProtocol(Fault);
    }
}

void StreamCore::
writeCallback(StreamBusInterface::IoStatus status)
{
    MutexLock lock(this);
    debug("StreamCore::writeCallback(%s, status=%s)\n",
        name(), status ? "Timeout" : "Success");
    if (!(flags & WritePending))
    {
        error("StreamCore::writeCallback(%s) called unexpectedly\n",
            name());
        return;
    }
    flags &= ~WritePending;
    if (status != StreamBusInterface::ioSuccess)
    {
        finishProtocol(WriteTimeout);
        return;
    }
    evalCommand();
}

// Handle 'in' command

bool StreamCore::
evalIn()
{
    flags |= AcceptInput;
    long expectedInput;

    expectedInput = maxInput;
    if (inputBuffer && unparsedInput)
    {
        // handle early input
        debug("StreamCore::evalIn(%s): early input: %s\n",
            name(), inputBuffer.expand()());
        expectedInput = readCallback(lastInputStatus, NULL, 0);
        if (!expectedInput)
        {
            // no more input needed
            return true;
        }
    }
    if (flags & AsyncMode)
    {
        // release bus
        if (flags & BusOwner)
        {
            debug("StreamCore::evalIn(%s): unlocking bus\n",
                name());
            busUnlock();
            flags &= ~BusOwner;
        }
        busReadRequest(pollPeriod, readTimeout,
            expectedInput, true);
        return true;
    }
    busReadRequest(replyTimeout, readTimeout,
        expectedInput, false);
    // continue with readCallback() in another thread
    return true;
}

long StreamCore::
readCallback(StreamBusInterface::IoStatus status,
    const void* input, long size)
// returns number of bytes to read additionally

{
    MutexLock lock(this);
    lastInputStatus = status;

#ifndef NO_TEMPORARY
    debug("StreamCore::readCallback(%s, status=%s input=\"%s\", size=%ld)\n",
        name(),
        status == 0 ? "ioSuccess" :
        status == 1 ? "ioTimeout" :
        status == 2 ? "ioNoReply" :
        status == 3 ? "ioEnd" :
        status == 4 ? "ioFault" : "Invalid",
        StreamBuffer(input, size).expand()(), size);
#endif

    if (!(flags & AcceptInput))
    {
        error("StreamCore::readCallback(%s) called unexpectedly\n",
            name());
        return 0;
    }
    flags &= ~AcceptInput;
    unparsedInput = false;
    switch (status)
    {
        case StreamBusInterface::ioTimeout:
            // timeout is valid end if we have no terminator
            // and number of input bytes is not limited
            if (!inTerminator && !maxInput)
            {
                status = StreamBusInterface::ioEnd;
            }
            // else timeout might be ok if we find a terminator
            break;
        case StreamBusInterface::ioSuccess:
        case StreamBusInterface::ioEnd:
            break;
        case StreamBusInterface::ioNoReply:
            if (flags & AsyncMode)
            {
                // just restart in asyn mode
                debug("StreamCore::readCallback(%s) no async input: just restart\n",
                    name());
                evalIn();
                return 0;
            }
            error("%s: No reply from device within %ld ms\n",
                name(), replyTimeout);
            inputBuffer.clear();
            finishProtocol(ReplyTimeout);
            return 0;
        case StreamBusInterface::ioFault:
            error("%s: I/O error from device\n", name());
            finishProtocol(Fault);
            return 0;
    }
    inputBuffer.append(input, size);
    debug("StreamCore::readCallback(%s) inputBuffer=\"%s\", size %ld\n",
        name(), inputBuffer.expand()(), inputBuffer.length());
    if (*activeCommand != in_cmd)
    {
        // early input, stop here and wait for in command
        // -- Should we limit size of inputBuffer? --
        if (inputBuffer) unparsedInput = true;
        return 0;
    }
    
    // prepare to parse the input
    const char *commandStart = commandIndex;
    long end = -1;
    long termlen = 0;
    
    if (inTerminator)
    {
        // look for terminator
        end = inputBuffer.find(inTerminator);
        if (end >= 0) termlen = inTerminator.length();
        debug("StreamCore::readCallback(%s) inTerminator %sfound\n",
            name(), end >= 0 ? "" : "not ");
    }
    if (status == StreamBusInterface::ioEnd && end < 0)
    {
        // no terminator but end flag
        debug("StreamCore::readCallback(%s) end flag received\n",
            name());
        end = inputBuffer.length();
    }
    if (maxInput && end < 0 && (long)maxInput <= inputBuffer.length())
    {
        // no terminator but maxInput bytes read
        debug("StreamCore::readCallback(%s) maxInput size reached\n",
            name());
        end = maxInput;
    }
    if (maxInput && end > (long)maxInput)
    {
        // limit input length to maxInput (ignore terminator)
        end = maxInput;
        termlen = 0;
    }
    if (end < 0)
    {
        // no end found
        if (status != StreamBusInterface::ioTimeout)
        {
            // input is incomplete - wait for more
            debug("StreamCore::readCallback(%s) wait for more input\n",
                name());
            flags |= AcceptInput;
            if (maxInput)
                return maxInput - inputBuffer.length();
            else
                return -1;
        }
        // try to parse what we got
        end = inputBuffer.length();
        if (!(flags & AsyncMode))
        {
            error("%s: Timeout after reading %ld bytes \"%s%s\"\n",
                name(), end, end > 20 ? "..." : "",
                inputBuffer.expand(-20)());
        }
    }

    inputLine.set(inputBuffer(), end);
    bool matches = matchInput();
    inputBuffer.remove(end + termlen);
    if (inputBuffer) unparsedInput = true;

    if (!matches)
    {
        if (status == StreamBusInterface::ioTimeout)
        {
            // we have not forgotten the timeout
            finishProtocol(ReadTimeout);
            return 0;
        }
        if (flags & AsyncMode)
        {
            debug("StreamCore::readCallback(%s) async match failure: just restart\n",
                name());
            commandIndex = commandStart;
            evalIn();
            return 0;
        }
        debug("StreamCore::readCallback(%s) match failure\n",
            name());
        finishProtocol(ScanError);
        return 0;
    }
    if (status == StreamBusInterface::ioTimeout)
    {
        // we have not forgotten the timeout
        finishProtocol(ReadTimeout);
        return 0;
    }
    // end input mode and do next command
    flags &= ~(AsyncMode|AcceptInput);
    // -- should we tell someone that input has finished? --
    evalCommand();
    return 0;
}

bool StreamCore::
matchInput()
{
    char command;
    const char* formatstring;
    
    consumedInput = 0;
    
    while ((command = *commandIndex++) != StreamProtocolParser::eos)
    {
        switch (command)
        {
            case StreamProtocolParser::format_field:
            {
                // code layout:
                // field <StreamProtocolParser::eos> addrlen AddressStructure formatstring <StreamProtocolParser::eos> StreamFormat [info]
                commandIndex += strlen(commandIndex)+1;
                unsigned short addrlen = extract<unsigned short>(commandIndex);
                fieldAddress.set(commandIndex, addrlen);
                commandIndex += addrlen;
            }
            case StreamProtocolParser::format:
            {
                int consumed;
                // code layout:
                // formatstring <eos> StreamFormat [info]
                formatstring = commandIndex;
                while (*commandIndex++ != StreamProtocolParser::eos); // jump after <eos>
                
                StreamFormat fmt = extract<StreamFormat>(commandIndex);
                fmt.info = commandIndex;
                commandIndex += fmt.infolen;
                if (fmt.flags & skip_flag || fmt.type == pseudo_format)
                {
                    long ldummy;
                    double ddummy;
                    switch (fmt.type)
                    {
                        case long_format:
                        case enum_format:
                            consumed = StreamFormatConverter::find(fmt.conv)->
                                scanLong(fmt, inputLine(consumedInput), ldummy);
                            break;
                        case double_format:
                            consumed = StreamFormatConverter::find(fmt.conv)->
                                scanDouble(fmt, inputLine(consumedInput), ddummy);
                            break;
                        case string_format:
                            consumed = StreamFormatConverter::find(fmt.conv)->
                                scanString(fmt, inputLine(consumedInput), NULL, 0);
                            break;
                        case pseudo_format:
                            // pass complete input
                            consumed = StreamFormatConverter::find(fmt.conv)->
                                scanPseudo(fmt, inputLine, consumedInput);
                            break;
                        default:
                            error("INTERNAL ERROR (%s): illegal format.type 0x%02x\n",
                                name(), fmt.type);
                            return false;
                    }
                    if (consumed < 0)
                    {
                        if (!(flags & AsyncMode))
                        {
                            error("%s: Input \"%s%s\" does not match format %%%s\n",
                                name(), inputLine.expand(consumedInput, 20)(),
                                inputLine.length()-consumedInput > 20 ? "..." : "",
                                formatstring);
                        }
                        return false;
                    }
                    consumedInput += consumed;
                    break;
                }
                flags &= ~Separator;
                if (!matchValue(fmt, fieldAddress ? fieldAddress() : NULL))
                {
                    if (!(flags & AsyncMode))
                    {
                        if (flags & ScanTried)
                            error("%s: Input \"%s%s\" does not match format %%%s\n",
                                name(), inputLine.expand(consumedInput, 20)(),
                                inputLine.length()-consumedInput > 20 ? "..." : "",
                                formatstring);
                        else
                            error("%s: Can't scan value with format %%%s\n",
                                name(), formatstring);
                    }
                    return false;
                }
                // matchValue() has already removed consumed bytes from inputBuffer
                fieldAddress = NULL;
                break;
            }
            case StreamProtocolParser::skip:
                // ignore next input byte
                consumedInput++;
                break;
            case esc:
                // escaped literal byte
                command = *commandIndex++;
            default:
                // literal byte
                if (consumedInput >= inputLine.length())
                {
                    if (!(flags & AsyncMode))
                        error("%s: Input \"%s%s\" too short. No match for byte \"%s\"\n",
                            name(), 
                            inputLine.length() > 20 ? "..." : "",
                            inputLine.expand(-20)(),
                            StreamBuffer(&command,1).expand()());
                    return false;
                }
                if (command != inputLine[consumedInput])
                {
                    if (!(flags & AsyncMode))
                    {
                        int i = 0;
                        while (commandIndex[i] >= ' ') i++;
                        error("%s: Byte %ld in \"%s\" does not match expected \"%s\"\n",
                        name(), consumedInput,
                        inputLine.expand()(),
                        StreamBuffer(commandIndex-1,i+1).expand()());
                    }
                    return false;
                }
                consumedInput++;
        }
    }
    long surplus = inputLine.length()-consumedInput;
    if (surplus > 0 && !(flags & IgnoreExtraInput))
    {
        if (!(flags & AsyncMode))
            error("%s: %ld bytes surplus input \"%s%s\"\n",
                name(), surplus,
                inputLine.expand(consumedInput, 19)(),
                surplus > 20 ? "..." : "");
        return false;
    }
    return true;
}

bool StreamCore::
matchSeparator()
{
    if (!(flags & Separator))
    {
        flags |= Separator;
        return true;
    }
    if (!separator) return true;
    long i = 0;
    if (separator[0] == ' ')
    {
        i++;
        // skip leading whitespaces
        while (isspace(inputLine[consumedInput++]));
    }
    for (; i < separator.length(); i++,consumedInput++)
    {
        if (!inputLine[consumedInput]) return false;
        if (separator[i] == StreamProtocolParser::skip) continue; // wildcard
        if (separator[i] == esc) i++;       // escaped literal byte
        if (separator[i] != inputLine[consumedInput]) return false;
    }
    return true;
}

bool StreamCore::
scanSeparator()
{
    // for compatibility only
    // read and remove separator
    if (!matchSeparator()) return false;
    flags &= ~Separator;
    return true;
}

long StreamCore::
scanValue(const StreamFormat& fmt, long& value)
{
    if (fmt.type != long_format && fmt.type != enum_format)
    {
        error("%s: scanValue(long&) called with %%%c format\n",
            name(), fmt.conv);
        return -1;
    }
    flags |= ScanTried;
    if (!matchSeparator()) return -1;
    long consumed = StreamFormatConverter::find(fmt.conv)->
        scanLong(fmt, inputLine(consumedInput), value);
    debug("StreamCore::scanValue(%s, format=%%%c, long) input=\"%s\"\n",
        name(), fmt.conv, inputLine.expand(consumedInput)());
    if (consumed < 0 ||
        consumed > inputLine.length()-consumedInput) return -1;
    debug("StreamCore::scanValue(%s) scanned %li\n",
        name(), value);
    flags |= GotValue;
    return consumed;
}

long StreamCore::
scanValue(const StreamFormat& fmt, double& value)
{
    if (fmt.type != double_format)
    {
        error("%s: scanValue(double&) called with %%%c format\n",
            name(), fmt.conv);
        return -1;
    }
    flags |= ScanTried;
    if (!matchSeparator()) return -1;
    long consumed = StreamFormatConverter::find(fmt.conv)->
        scanDouble(fmt, inputLine(consumedInput), value);
    debug("StreamCore::scanValue(%s, format=%%%c, double) input=\"%s\"\n",
        name(), fmt.conv, inputLine.expand(consumedInput)());
    if (consumed < 0 ||
        consumed > inputLine.length()-consumedInput) return -1;
    debug("StreamCore::scanValue(%s) scanned %#g\n",
        name(), value);
    flags |= GotValue;
    return consumed;
}

long StreamCore::
scanValue(const StreamFormat& fmt, char* value, long maxlen)
{
    if (fmt.type != string_format)
    {
        error("%s: scanValue(char*) called with %%%c format\n",
            name(), fmt.conv);
        return -1;
    }
    if (maxlen < 0) maxlen = 0;
    flags |= ScanTried;
    if (!matchSeparator()) return -1;
    long consumed = StreamFormatConverter::find(fmt.conv)->
        scanString(fmt, inputLine(consumedInput), value, maxlen);
    debug("StreamCore::scanValue(%s, format=%%%c, char*, maxlen=%ld) input=\"%s\"\n",
        name(), fmt.conv, maxlen, inputLine.expand(consumedInput)());
    if (consumed < 0 ||
        consumed > inputLine.length()-consumedInput) return -1;
#ifndef NO_TEMPORARY
    debug("StreamCore::scanValue(%s) scanned \"%s\"\n",
        name(), StreamBuffer(value, consumed).expand()());
#endif
    flags |= GotValue;
    return consumed;
}

// Handle 'event' command

bool StreamCore::
evalEvent()
{
    // code layout:
    // eventmask timeout
    unsigned long eventMask = extract<unsigned long>(commandIndex);
    unsigned long eventTimeout = extract<unsigned long>(commandIndex);
    if (flags & AsyncMode && eventTimeout == 0)
    {
        if (flags & BusOwner)
        {
            busUnlock();
            flags &= ~BusOwner;
        }
    }
    flags |= AcceptEvent;
    busAcceptEvent(eventMask, eventTimeout);
    return true;
}

void StreamCore::
eventCallback(StreamBusInterface::IoStatus status)
{
    MutexLock lock(this);
    if (!(flags & AcceptEvent))
    {
        error("StreamCore::eventCallback(%s) called unexpectedly\n",
            name());
        return;
    }
    debug("StreamCore::eventCallback(%s, status=%s)\n",
        name(),
        status == 0 ? "ioSuccess" :
        status == 1 ? "ioTimeout" :
        status == 2 ? "ioNoReply" :
        status == 3 ? "ioEnd" :
        status == 4 ? "ioFault" : "Invalid");
    flags &= ~AcceptEvent;
    switch (status)
    {
        case StreamBusInterface::ioTimeout:
            error("%s: No event from device\n", name());
            finishProtocol(ReplyTimeout);
            return;
        case StreamBusInterface::ioSuccess:
            evalCommand();
            return;
        default:
            error("%s: Event error from device\n", name());
            finishProtocol(Fault);
            return;
    }
}

// Handle 'wait' command

bool StreamCore::
evalWait()
{
    unsigned long waitTimeout = extract<unsigned long>(commandIndex);
    flags |= WaitPending;
    startTimer(waitTimeout);
    return true;
}

void StreamCore::
timerCallback()
{
    MutexLock lock(this);
    debug ("StreamCore::timerCallback(%s)\n", name());
    if (!(flags & WaitPending))
    {
        error("StreamCore::timerCallback(%s) called unexpectedly\n",
            name());
        return;
    }
    flags &= ~WaitPending;
    evalCommand();
}


bool StreamCore::
evalExec()
{
    outputLine.clear();
    formatOutput();
    if (!execute())
    {
        error("%s: executing command \"%s\"\n", name(), outputLine());
        return false;
    }
    return true;
}

void StreamCore::
execCallback(StreamBusInterface::IoStatus status)
{
    switch (status)
    {
        case StreamBusInterface::ioSuccess:
            evalCommand();
            return;
        default:
            error("%s: Shell command \"%s\" failed\n",
                name(), outputLine());
            finishProtocol(Fault);
            return;
    }
}

bool StreamCore::execute()
{
    error("%s: Command 'exec' not implemented on this system\n",
        name());
    return false;
}