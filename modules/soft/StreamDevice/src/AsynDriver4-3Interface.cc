/***************************************************************
* StreamDevice Support                                         *
*                                                              *
* (C) 2005 Dirk Zimoch (dirk.zimoch@psi.ch)                    *
*                                                              *
* This is the interface to asyn driver for StreamDevice.       *
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

#include "StreamBusInterface.h"
#include "StreamError.h"
#include "StreamBuffer.h"
#include <epicsTime.h>
#include <epicsTimer.h>
#include <asynDriver.h>
#include <asynOctet.h>
#include <asynInt32.h>
#include <asynUInt32Digital.h>

/* How things are implemented:

synchonous io:

lockRequest()
    call pasynManager->blockProcessCallback(),
    call pasynCommon->connect() only if
        lockTimeout is unlimited (0) and port is not yet connected
    call pasynManager->queueRequest()
    when request is handled
        call lockCallback(ioSuccess)
    if request fails
        call lockCallback(ioTimeout)

writeRequest()
    call pasynManager->queueRequest()
    when request is handled
        call pasynOctet->flush()
        call pasynOctet->writeRaw()
        if writeRaw() times out
            call writeCallback(ioTimeout)
        if writeRaw fails otherwise
            call writeCallback(ioFault)
        if writeRaw succeeds and all bytes have been written
            call writeCallback(ioSuccess)
        if not all bytes can be written
            call pasynManager->queueRequest() again
    if request fails
        call writeCallback(ioTimeout)

readRequest()
    call pasynManager->queueRequest()
    when request is handled
        call pasynOctet->setInputEos()
        call pasynOctet->read()
        if read() times out at the first byte
            call readCallback(ioNoReply)
        if read() times out at next bytes
            call readCallback(ioTimeout,buffer,received)
        if read() fails otherwise
            call readCallback(ioFault,buffer,received)
        if read() is successfull and reason is ASYN_EOM_END
            or ASYN_EOM_EOS
            call readCallback(ioEnd,buffer,received)
        if read() is successfull and no end is detected
            call readCallback(ioSuccess,buffer,received)
        if readCallback() has returned true (wants more input)
            loop back to the pasynOctet->read() call
    if request fails
        call readCallback(ioFault)

unlock()
    call pasynManager->unblockProcessCallback()

asynchonous input support ("I/O Intr"):

pasynOctet->registerInterruptUser(...,intrCallbackOctet,...) is called
initially. This calls intrCallbackOctet() every time input is received,
but only if someone else is doing a read. Thus, if nobody reads
something, arrange for periodical read polls.

enableAsyncRead()
    set asynReadEnabled true
    when input arrives for any asynUser
        call readCallback(ioSuccess,buffer,received)
        if readCallback() has returned true (wants more input)
            start timer(readTimeout) while waiting for more input
            if timer expires
                call readCallback(ioTimeout)
        else start timer(replyTimeout) for new poll cycle
            when timer expires
                call pasynManager->queueRequest()
                when request is handeled
                    do the same as in readRequest() (see above)
                if request fails
                    start timer(replyTimeout) for new poll cycle


disableAsyncRead()
    set asynReadEnabled false
    That means that intrCallbackOctet() ignores asyncronous input

*/

extern "C" {
static void handleRequest(asynUser*);
static void handleTimeout(asynUser*);
static void intrCallbackOctet(void* pvt, asynUser *pasynUser,
    char *data, size_t numchars, int eomReason);
static void intrCallbackInt32(void* pvt, asynUser *pasynUser,
    epicsInt32 data);
static void intrCallbackUInt32(void* pvt, asynUser *pasynUser,
    epicsUInt32 data);
}

enum IoAction {
    None, Lock, Write, Read, AsyncRead, AsyncReadMore, ReceiveEvent
};

static const char* ioActionStr[] = {
    "None", "Lock", "Write", "Read",
    "AsyncRead", "AsyncReadMore", "ReceiveEvent"
};

class AsynDriverInterface : StreamBusInterface , epicsTimerNotify
{
    

    asynUser* pasynUser;
    asynCommon* pasynCommon;
    void* pvtCommon;
    asynOctet* pasynOctet;
    void* pvtOctet;
    void* intrPvtOctet;
    asynInt32* pasynInt32;
    void* pvtInt32;
    void* intrPvtInt32;
    asynUInt32Digital* pasynUInt32;
    void* pvtUInt32;
    void* intrPvtUInt32;
    IoAction ioAction;
    double lockTimeout;
    double writeTimeout;
    double readTimeout;
    double replyTimeout;
    long expectedLength;
    unsigned long eventMask;
    unsigned long receivedEvent;
    StreamBuffer inputBuffer;
    const char* outputBuffer;
    size_t outputSize;
    epicsTimerQueueActive* timerQueue;
    epicsTimer* timer;

    AsynDriverInterface(Client* client);
    ~AsynDriverInterface();

    // StreamBusInterface methods
    bool lockRequest(unsigned long lockTimeout_ms);
    bool unlock();
    bool writeRequest(const void* output, size_t size,
        unsigned long writeTimeout_ms);
    bool readRequest(unsigned long replyTimeout_ms,
        unsigned long readTimeout_ms, long expectedLength, bool async);
    bool acceptEvent(unsigned long mask, unsigned long replytimeout_ms);
    bool supportsEvent();
    bool supportsAsyncRead();

    // epicsTimerNotify methods
    epicsTimerNotify::expireStatus expire(const epicsTime &);

    // local methods
    bool connectToBus(const char* busname, int addr);
    void lockHandler();
    void writeHandler();
    void readHandler();
    bool connectToAsynPort();
    void asynReadHandler(char *data, size_t numchars);
    asynQueuePriority priority() {
        return static_cast<asynQueuePriority>
            (StreamBusInterface::priority());
    }
    void startTimer(double timeout) {
        timer->start(*this, timeout);
    }
    void cancelTimer() {
        timer->cancel();
    }

    // asynUser callback functions
    friend void handleRequest(asynUser*);
    friend void handleTimeout(asynUser*);
    friend void intrCallbackOctet(void* pvt, asynUser *pasynUser,
        char *data, size_t numchars, int eomReason);
    friend void intrCallbackInt32(void* pvt, asynUser *pasynUser,
        epicsInt32 data);
    friend void intrCallbackUInt32(void* pvt, asynUser *pasynUser,
        epicsUInt32 data);
public:
    // static creator method
    static StreamBusInterface* getBusInterface(Client* client,
        const char* busname, int addr, const char* param);

};

RegisterStreamBusInterface(AsynDriverInterface);

AsynDriverInterface::
AsynDriverInterface(Client* client) : StreamBusInterface(client)
{
    pasynCommon = NULL;
    pasynOctet = NULL;
    intrPvtOctet = NULL;
    pasynInt32 = NULL;
    intrPvtInt32 = NULL;
    pasynUInt32 = NULL;
    intrPvtUInt32 = NULL;
    eventMask = 0;
    receivedEvent = 0;
    pasynUser = pasynManager->createAsynUser(handleRequest,
        handleTimeout);
    assert(pasynUser);
    pasynUser->userPvt = this;
    timerQueue = &epicsTimerQueueActive::allocate(true);
    assert(timerQueue);
    timer = &timerQueue->createTimer();
    assert(timer);
}

AsynDriverInterface::
~AsynDriverInterface()
{
    timer->cancel();

    if (intrPvtInt32)
    {
        // Int32 event interface is connected
        pasynInt32->cancelInterruptUser(pvtInt32,
            pasynUser, intrPvtInt32);
    }
    if (intrPvtUInt32)
    {
        // UInt32 event interface is connected
        pasynUInt32->cancelInterruptUser(pvtUInt32,
            pasynUser, intrPvtUInt32);
    }
    if (pasynOctet)
    {
        // octet stream interface is connected
        int wasQueued;
        if (intrPvtOctet)
        {
            pasynOctet->cancelInterruptUser(pvtOctet,
                pasynUser, intrPvtOctet);
        }
        pasynManager->cancelRequest(pasynUser, &wasQueued);
        // does not return until running handler has finished
    }
    // Now, no handler is running any more and none will start.

    timer->destroy();
    timerQueue->release();
    pasynManager->disconnect(pasynUser);
    pasynManager->freeAsynUser(pasynUser);
    pasynUser = NULL;
}

// interface function getBusInterface():
// do we have this bus/addr ?
StreamBusInterface* AsynDriverInterface::
getBusInterface(Client* client,
    const char* busname, int addr, const char*)
{
    AsynDriverInterface* interface = new AsynDriverInterface(client);
    if (interface->connectToBus(busname, addr))
    {
        debug ("AsynDriverInterface::getBusInterface(%s, %d): "
            "new Interface allocated\n",
            busname, addr);
        return interface;
    }
    delete interface;
    return NULL;
}

// interface function supportsEvent():
// can we handle the StreamDevice command 'event'?
bool AsynDriverInterface::
supportsEvent()
{
    return (pasynInt32 != NULL) || (pasynUInt32 != NULL);
}

bool AsynDriverInterface::
supportsAsyncRead()
{
    if (intrPvtOctet) return true;

    // hook "I/O Intr" support
    if (pasynOctet->registerInterruptUser(pvtOctet, pasynUser,
        intrCallbackOctet, this, &intrPvtOctet) != asynSuccess)
    {
        error("%s: bus does not support asynchronous input: %s\n",
            clientName(), pasynUser->errorMessage);
        return false;
    }
    return true;
}

bool AsynDriverInterface::
connectToBus(const char* busname, int addr)
{
    if (pasynManager->connectDevice(pasynUser, busname, addr) !=
        asynSuccess)
    {
        // asynDriver does not know this busname/address
        return false;
    }

    asynInterface* pasynInterface;

    // find the asynCommon interface
    pasynInterface = pasynManager->findInterface(pasynUser,
        asynCommonType, true);
    if(!pasynInterface)
    {
        error("%s: bus %s does not support asynCommon interface\n",
            clientName(), busname);
        return false;
    }
    pasynCommon = static_cast<asynCommon*>(pasynInterface->pinterface);
    pvtCommon = pasynInterface->drvPvt;

    // find the asynOctet interface
    pasynInterface = pasynManager->findInterface(pasynUser,
        asynOctetType, true);
    if(!pasynInterface)
    {
        error("%s: bus %s does not support asynOctet interface\n",
            clientName(), busname);
        return false;
    }
    pasynOctet = static_cast<asynOctet*>(pasynInterface->pinterface);
    pvtOctet = pasynInterface->drvPvt;

    // look for interfaces for events
    pasynInterface = pasynManager->findInterface(pasynUser,
        asynInt32Type, true);
    if(pasynInterface)
    {
        pasynInt32 = static_cast<asynInt32*>(pasynInterface->pinterface);
        pvtInt32 = pasynInterface->drvPvt;
        pasynUser->reason = ASYN_REASON_SIGNAL; // required for GPIB
        if (pasynInt32->registerInterruptUser(pvtInt32, pasynUser,
            intrCallbackInt32, this, &intrPvtInt32) == asynSuccess)
        {
            return true;
        }
        error("%s: bus %s does not allow to register for "
            "Int32 interrupts: %s\n",
            clientName(), busname, pasynUser->errorMessage);
        pasynInt32 = NULL;
        intrPvtInt32 = NULL;
    }

    // no asynInt32 available, thus try asynUInt32
    pasynInterface = pasynManager->findInterface(pasynUser,
        asynUInt32DigitalType, true);
    if(pasynInterface)
    {
        pasynUInt32 =
            static_cast<asynUInt32Digital*>(pasynInterface->pinterface);
        pvtUInt32 = pasynInterface->drvPvt;
        pasynUser->reason = ASYN_REASON_SIGNAL;
        if (pasynUInt32->registerInterruptUser(pvtUInt32,
            pasynUser, intrCallbackUInt32, this, 0xFFFFFFFF,
            &intrPvtUInt32) == asynSuccess)
        {
            return true;
        }
        error("%s: bus %s does not allow to register for "
            "UInt32 interrupts: %s\n",
            clientName(), busname, pasynUser->errorMessage);
        pasynUInt32 = NULL;
        intrPvtUInt32 = NULL;
    }

    // no event interface available, never mind
    return true;
}

// interface function: we want exclusive access to the device
// lockTimeout_ms=0 means "block here" (used in @init)
bool AsynDriverInterface::
lockRequest(unsigned long lockTimeout_ms)
{
    debug("AsynDriverInterface::lockRequest(%s, %ld msec)\n",
        clientName(), lockTimeout_ms);
    asynStatus status;
    lockTimeout = lockTimeout_ms ? lockTimeout_ms*0.001 : -1.0;
    if (!lockTimeout_ms)
    {
        if (!connectToAsynPort()) return false;
    }
    ioAction = Lock;
    status = pasynManager->queueRequest(pasynUser, priority(),
        lockTimeout);
    if (status != asynSuccess)
    {
        error("%s lockRequest: pasynManager->queueRequest() failed: %s\n",
            clientName(), pasynUser->errorMessage);
        return false;
    }
    // continues with handleRequest() or handleTimeout()
    return true;
}

bool AsynDriverInterface::
connectToAsynPort()
{
    asynStatus status;
    int connected;

    debug("AsynDriverInterface::connectToAsynPort(%s)\n",
        clientName());
    status = pasynManager->isConnected(pasynUser, &connected);
    if (status != asynSuccess)
    {
        error("%s: pasynManager->isConnected() failed: %s\n",
            clientName(), pasynUser->errorMessage);
        return false;
    }
    if (!connected)
    {
        status = pasynCommon->connect(pvtCommon, pasynUser);
        debug("AsynDriverInterface::connectToAsynPort(%s): "
                "status=%d\n",
            clientName(), status);
        if (status != asynSuccess)
        {
            error("%s: pasynCommon->connect() failed: %s\n",
                clientName(), pasynUser->errorMessage);
            return false;
        }
    }
    return true;
}

// now, we can have exclusive access (called by asynManager)
void AsynDriverInterface::
lockHandler()
{
    debug("AsynDriverInterface::lockHandler(%s)\n",
        clientName());
    pasynManager->blockProcessCallback(pasynUser, false);
    lockCallback(ioSuccess);
}

// interface function: we don't need exclusive access any more
bool AsynDriverInterface::
unlock()
{
    debug("AsynDriverInterface::unlock(%s)\n",
        clientName());
    pasynManager->unblockProcessCallback(pasynUser, false);
    return true;
}

// interface function: we want to write something
bool AsynDriverInterface::
writeRequest(const void* output, size_t size,
    unsigned long writeTimeout_ms)
{
#ifndef NO_TEMPORARY
    debug("AsynDriverInterface::writeRequest(%s, \"%s\", %ld msec)\n",
        clientName(), StreamBuffer(output, size).expand()(),
        writeTimeout_ms);
#endif

    asynStatus status;
    outputBuffer = (char*)output;
    outputSize = size;
    writeTimeout = writeTimeout_ms*0.001;
    ioAction = Write;
    status = pasynManager->queueRequest(pasynUser, priority(),
        writeTimeout);
    if (status != asynSuccess)
    {
        error("%s writeRequest: pasynManager->queueRequest() failed: %s\n",
            clientName(), pasynUser->errorMessage);
        return false;
    }
    // continues with handleRequest() or handleTimeout()
    return true;
}

// now, we can write (called by asynManager)
void AsynDriverInterface::
writeHandler()
{
    debug("AsynDriverInterface::writeHandler(%s)\n",
        clientName());
    asynStatus status;
    size_t written = 0;
    pasynUser->timeout = writeTimeout;

    // discard any early input or early events
    status = pasynOctet->flush(pvtOctet, pasynUser);
    receivedEvent = 0;

    if (status != asynSuccess)
    {
        error("%s: pasynOctet->flush() failed: %s\n",
                clientName(), pasynUser->errorMessage);
        writeCallback(ioFault);
        return;
    }
    status = pasynOctet->write(pvtOctet, pasynUser,
        outputBuffer, outputSize, &written);
    switch (status)
    {
        case asynSuccess:
            outputBuffer += written;
            outputSize -= written;
            if (outputSize > 0)
            {
                status = pasynManager->queueRequest(pasynUser,
                    priority(), lockTimeout);
                if (status != asynSuccess)
                {
                    error("%s writeHandler: "
                        "pasynManager->queueRequest() failed: %s\n",
                        clientName(), pasynUser->errorMessage);
                    writeCallback(ioFault);
                }
                // continues with handleRequest() or handleTimeout()
                return;
            }
            writeCallback(ioSuccess);
            return;
        case asynTimeout:
            writeCallback(ioTimeout);
            return;
        case asynOverflow:
            error("%s: asynOverflow: %s\n",
                clientName(), pasynUser->errorMessage);
            writeCallback(ioFault);
            return;
        case asynError:
            error("%s: asynError: %s\n",
                clientName(), pasynUser->errorMessage);
            writeCallback(ioFault);
            return;
    }
}

// interface function: we want to read something
bool AsynDriverInterface::
readRequest(unsigned long replyTimeout_ms, unsigned long readTimeout_ms,
    long _expectedLength, bool async)
{
    asynStatus status;
    debug("AsynDriverInterface::readRequest(%s, %ld msec reply, "
        "%ld msec read, expect %ld bytes, asyn=%s)\n",
        clientName(), replyTimeout_ms, readTimeout_ms,
        _expectedLength, async?"yes":"no");
    readTimeout = readTimeout_ms*0.001;
    replyTimeout = replyTimeout_ms*0.001;
    double queueTimeout;
    expectedLength = _expectedLength;
    if (async)
    {
        ioAction = AsyncRead;
        queueTimeout = 0.0;
        // first poll for input,
        // later poll periodically if no other input arrives
        // from intrCallbackOctet()
    }
    else {
        ioAction = Read;
        queueTimeout = replyTimeout;
    }
    status = pasynManager->queueRequest(pasynUser,
        priority(), queueTimeout);
    if (status != asynSuccess)
    {
        error("%s readRequest: pasynManager->queueRequest() failed: %s\n",
            clientName(), pasynUser->errorMessage);
        return false;
    }
    // continues with handleRequest() or handleTimeout()
    return true;
}

// now, we can read (called by asynManager)
void AsynDriverInterface::
readHandler()
{
#ifndef NO_TEMPORARY
    debug("AsynDriverInterface::readHandler(%s) eoslen=%d:%s\n",
        clientName(), eoslen, StreamBuffer(eos, eoslen).expand()());
#endif

    while (eoslen > 0)
    {
        if (pasynOctet->setInputEos(pvtOctet, pasynUser,
            eos, eoslen) == asynSuccess) break;
        eos++; eoslen--;
        if (eoslen < 0)
        {
            error("%s: warning: pasynOctet->setInputEos() failed: %s\n",
                clientName(), pasynUser->errorMessage);
        }
    }

    bool async = (ioAction == AsyncRead);
    long maxlen = expectedLength > 0 ?
        expectedLength : inputBuffer.capacity()-1;
    char* buffer = inputBuffer.clear().reserve(maxlen);
    int bytesToRead = 1;
    pasynUser->timeout = async ? 0.0 : replyTimeout;
    ioAction = Read;
    bool waitForReply = true;
    size_t received;
    int eomReason;
    asynStatus status;
    long readMore;

    while (1)
    {
        readMore = 0;
        received = 0;
        debug("AsynDriverInterface::readHandler(%s): "
                "read(..., bytesToRead=%d, ...) timeout=%f seconds\n",
                clientName(), bytesToRead, pasynUser->timeout);
        status = pasynOctet->read(pvtOctet, pasynUser,
            buffer, bytesToRead, &received, &eomReason);

#ifndef NO_TEMPORARY
        debug("AsynDriverInterface::readHandler(%s): "
                "received %d of %d bytes \"%s\" status=%d eomReason=%d\n",
            clientName(), received, bytesToRead,
            StreamBuffer(buffer, received).expand()(),
            status, eomReason);
#endif

        switch (status)
        {
            case asynSuccess:
                readMore = readCallback(
                    eomReason & (ASYN_EOM_END|ASYN_EOM_EOS) ?
                    ioEnd : ioSuccess,
                    buffer, received);
                break;
            case asynTimeout:
                if (received == 0 && waitForReply)
                {
                    // reply timeout
                    if (async)
                    {
                        debug("AsynDriverInterface::readHandler(%s): "
                            "no async input, start timer %f seconds\n",
                            clientName(), replyTimeout);
                        // start next poll after timer expires
                        ioAction = AsyncRead;
                        if (replyTimeout != 0.0) startTimer(replyTimeout);
                        return;
                    }
                    debug("AsynDriverInterface::readHandler(%s): "
                        "no reply\n",
                        clientName());
                    readMore = readCallback(ioNoReply);
                    break;
                }
                // read timeout
                readMore = readCallback(ioTimeout, buffer, received);
                break;
            case asynOverflow:
                // deliver whatever we have
                error("%s: asynOverflow: %s\n",
                    clientName(), pasynUser->errorMessage);
                readCallback(ioFault, buffer, received);
                break;
            case asynError:
                error("%s: asynError: %s\n",
                    clientName(), pasynUser->errorMessage);
                readCallback(ioFault, buffer, received);
                break;
        }
        if (!readMore) break;
        if (readMore > 0)
        {
            bytesToRead = readMore;
        }
        else
        {
            bytesToRead = inputBuffer.capacity()-1;
        }
        debug("AsynDriverInterface::readHandler(%s) "
            "readMore=%ld bytesToRead=%d\n",
            clientName(), readMore, bytesToRead);
        pasynUser->timeout = readTimeout;
        waitForReply = false;
    }
}

void intrCallbackOctet(void* /*pvt*/, asynUser *pasynUser,
    char *data, size_t numchars, int /*eomReason*/)
{
    int wasQueued;
    AsynDriverInterface* interface =
        static_cast<AsynDriverInterface*>(pasynUser->userPvt);
    if (interface->ioAction == AsyncRead ||
        interface->ioAction == AsyncReadMore)
    {
    // cancel possible readTimeout or poll timer
        interface->cancelTimer();
    // cancel possible read poll
        pasynManager->cancelRequest(interface->pasynUser, &wasQueued);
        interface->asynReadHandler(data, numchars);
    }
}

// get asynchronous input
void AsynDriverInterface::
asynReadHandler(char *buffer, size_t received)
{
#ifndef NO_TEMPORARY
    debug("AsynDriverInterface::asynReadHandler(%s, buffer=\"%s\", "
            "received=%d)\n",
        clientName(), StreamBuffer(buffer, received).expand()(),
        received);
#endif

    bool readMore = true;
    if (received)
    {
        // process what we got and look if we need more data
        readMore = readCallback(ioSuccess, buffer, received);
    }
    if (readMore)
    {
        // wait for more input
        ioAction = AsyncReadMore;
        startTimer(readTimeout);
    }
    else
    {
        // start next poll after timer expires
        ioAction = AsyncRead;
        startTimer(replyTimeout);
    }
}

// interface function: we want to receive an event
bool AsynDriverInterface::
acceptEvent(unsigned long mask, unsigned long replytimeout_ms)
{
    if (receivedEvent & mask)
    {
        // handle early events
        receivedEvent = 0;
        eventCallback(ioSuccess);
        return true;
    }
    eventMask = mask;
    ioAction = ReceiveEvent;
    startTimer(replytimeout_ms*0.001);
    return true;
}

void intrCallbackInt32(void* /*pvt*/, asynUser *pasynUser, epicsInt32 data)
{
    AsynDriverInterface* interface =
        static_cast<AsynDriverInterface*>(pasynUser->userPvt);
    debug("AsynDriverInterface::intrCallbackInt32 (%s, %ld)\n",
        interface->clientName(), (long int) data);
    if (interface->eventMask)
    {
        if (data & interface->eventMask)
        {
            interface->eventMask = 0;
            interface->eventCallback(AsynDriverInterface::ioSuccess);
        }
        return;
    }
    // store early events
    interface->receivedEvent = data;
}

void intrCallbackUInt32(void* /*pvt*/, asynUser *pasynUser,
    epicsUInt32 data)
{
    AsynDriverInterface* interface =
        static_cast<AsynDriverInterface*>(pasynUser->userPvt);
    debug("AsynDriverInterface::intrCallbackUInt32 (%s, %ld)\n",
        interface->clientName(), (long int) data);
    if (interface->eventMask)
    {
        if (data & interface->eventMask)
        {
            interface->eventMask = 0;
            interface->eventCallback(AsynDriverInterface::ioSuccess);
        }
        return;
    }
    // store early events
    interface->receivedEvent = data;
}

epicsTimerNotify::expireStatus AsynDriverInterface::
expire(const epicsTime &)
{
    debug("AsynDriverInterface::expire(%s)\n", clientName());
    switch (ioAction)
    {
        case ReceiveEvent:
            // timeout while waiting for event
            ioAction = None;
            eventCallback(ioTimeout);
            return noRestart;
        case AsyncReadMore:
            // timeout after reading some async data
            readCallback(ioTimeout);
            ioAction = AsyncRead;
            startTimer(replyTimeout);
            return noRestart;
        case AsyncRead:
            // no async input for some time, thus let's poll
            // queueRequest might fail if another request just queued
            pasynManager->queueRequest(pasynUser,
                asynQueuePriorityLow, replyTimeout);
            // continues with handleRequest() or handleTimeout()
            return noRestart;
        default:
            error("INTERNAL ERROR (%s): expire() unexpected ioAction %s\n",
                clientName(), ioActionStr[ioAction]);
            return noRestart;
    }
}

// asynUser callbacks to pasynManager->queueRequest()

void handleRequest(asynUser* pasynUser)
{
    AsynDriverInterface* interface =
        static_cast<AsynDriverInterface*>(pasynUser->userPvt);
    debug("AsynDriverInterface::handleRequest(%s) %s\n",
        interface->clientName(), ioActionStr[interface->ioAction]);
    switch (interface->ioAction)
    {
        case Lock:
            interface->lockHandler();
            break;
        case Write:
            interface->writeHandler();
            break;
        case AsyncRead: // polled async input
            interface->readHandler();
            break;
        case Read:      // sync input
            interface->readHandler();
            break;
        default:
            error("INTERNAL ERROR (%s): "
                "handleRequest() unexpected ioAction %s\n",
                interface->clientName(), ioActionStr[interface->ioAction]);
    }
}

void handleTimeout(asynUser* pasynUser)
{
    AsynDriverInterface* interface =
        static_cast<AsynDriverInterface*>(pasynUser->userPvt);
    debug("AsynDriverInterface::handleTimeout(%s) %s\n",
        interface->clientName(), ioActionStr[interface->ioAction]);
    switch (interface->ioAction)
    {
        case Lock:
            interface->lockCallback(AsynDriverInterface::ioTimeout);
            break;
        case Write:
            interface->writeCallback(AsynDriverInterface::ioTimeout);
            break;
        case Read:
            interface->readCallback(AsynDriverInterface::ioFault, NULL, 0);
            break;
        case AsyncRead: // async poll failed, try later
            debug("AsynDriverInterface::handleTimeout(%s): "
                    "restart timer(%g seconds)\n",
                interface->clientName(), interface->replyTimeout);
                interface->startTimer(interface->replyTimeout);
            break;
        default:
            error("INTERNAL ERROR (%s): handleTimeout() "
                "unexpected ioAction %s\n",
                interface->clientName(), ioActionStr[interface->ioAction]);
    }
}

