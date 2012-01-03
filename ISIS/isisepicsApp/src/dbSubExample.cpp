#include <stdio.h>
#include <dbDefs.h>
#include <registryFunction.h>
#include <subRecord.h>
#include <aSubRecord.h>
#include <epicsExport.h>

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
#include <windows.h>

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS      // some CString constructors will be explicit
#include <atlbase.h>
#include <atlstr.h>
#include <atlcom.h>
#include <atlwin.h>
#include <atltypes.h>
#include <atlctl.h>
#include <atlhost.h>

#include <string>
#include <vector>
#include <map>
#include <list>
#include <stdexcept>
#include <sstream>

//#import "progid:isisicp.Idae" named_guids
#import "isisicp.tlb" named_guids
#import "Seci.tlb" named_guids
#import "LabVIEW.tlb" named_guids

typedef HRESULT (isisicpLib::Idae::*comm_func_t)(BSTR* messages);
typedef HRESULT (isisicpLib::Idae::*comm_func_i_t)(_bstr_t name, long value, BSTR* messages);

static const char* comm_names[] = { "BEGIN", "END", "PAUSE", "RESUME", "ABORT", "UPDATE", "STORE", "QUIT", "WAIT", "ENDWAIT", NULL };
static comm_func_t comm_funcs[] = { &isisicpLib::Idae::beginRun, &isisicpLib::Idae::endRun, 
	&isisicpLib::Idae::pauseRun, &isisicpLib::Idae::resumeRun, &isisicpLib::Idae::abortRun, 
	&isisicpLib::Idae::updateCRPT, &isisicpLib::Idae::storeCRPT, &isisicpLib::Idae::quit, 
	&isisicpLib::Idae::startSEWait, &isisicpLib::Idae::endSEWait, NULL };

static const int ncomm = sizeof(comm_names) / sizeof(const char*);

static const char* comm_i_names[] = { "SETVAL", NULL };
static comm_func_i_t comm_i_funcs[] = { &isisicpLib::Idae::setICPValueLong, NULL };

static const int ncomm_i = sizeof(comm_i_names) / sizeof(const char*);

static int mySubDebug = 0;

static long mySubInit(subRecord *precord)
{
    if (mySubDebug)
        printf("Record %s called mySubInit(%p)\n",
               precord->name, (void*) precord);
	CoInitializeEx(NULL, COINIT_MULTITHREADED);
    return 0;
}

static long mySubProcess(subRecord *precord)
{
    if (mySubDebug)
	{
        printf("Record %s called mySubProcess(%p)\n",
               precord->name, (void*) precord);
	}
	CComPtr<isisicpLib::Idae> icp;
	HRESULT hr;
	try
	{
		hr = icp.CoCreateInstance(isisicpLib::CLSID_dae, 0, CLSCTX_LOCAL_SERVER);
		if (FAILED(hr))
		{
			throw _com_error(hr);
		}
		BSTR messages;
		int command = static_cast<int>(0.5 + precord->a);
//		int command_arg = static_cast<int>(0.5 + precord->b);
		if (command < 0  || command >= ncomm)
		{
			throw std::runtime_error("invalid command");
		}		
		// execute command comm_names[command]
		hr = (icp->*comm_funcs[command])(&messages);
		if (FAILED(hr))
		{
			throw _com_error(hr);
		}
//		std::cerr << COLE2CT(messages) << std::endl;
	}
	catch (const _com_error& ex)
	{
//		std::cerr << ex.Source() << ": " << ex.ErrorMessage() << ": " << ex.Description() << std::endl;
	}
	catch (const std::exception& ex)
	{
//		std::cerr << ex.what() << std::endl;
	}
	precord->val = 0.0;
//	precord->udf = 0;
	return 0;
}

static long mySubGFProcess(subRecord *precord)
{
    if (mySubDebug)
	{
        printf("Record %s called mySubGFProcess(%p)\n",
               precord->name, (void*) precord);
	}
	CComPtr<isisicpLib::Idae> icp;
	HRESULT hr;
	LONG frames;
	try
	{
		hr = icp.CoCreateInstance(isisicpLib::CLSID_dae, 0, CLSCTX_LOCAL_SERVER);
		if (FAILED(hr))
		{
			throw _com_error(hr);
		}
		BSTR messages;
		frames = icp->getGoodFramesTotal(&messages);
//		std::cerr << COLE2CT(messages) << std::endl;
	}
	catch (const _com_error& ex)
	{
//		std::cerr << ex.Source() << ": " << ex.ErrorMessage() << ": " << ex.Description() << std::endl;
	}
	catch (const std::exception& ex)
	{
//		std::cerr << ex.what() << std::endl;
	}
	precord->val = frames;
	precord->udf = 0;
	return 0;
}

static long myAsubInit(aSubRecord *precord)
{
    if (mySubDebug)
        printf("Record %s called myAsubInit(%p)\n",
               precord->name, (void*) precord);
    return 0;
}

static long myAsubProcess(aSubRecord *precord)
{
    if (mySubDebug)
        printf("Record %s called myAsubProcess(%p)\n",
               precord->name, (void*) precord);
    return 0;
}

/* Register these symbols for use by IOC code: */
extern "C" 
{
epicsExportAddress(int, mySubDebug);
epicsRegisterFunction(mySubInit);
epicsRegisterFunction(mySubProcess);
epicsRegisterFunction(mySubGFProcess);
epicsRegisterFunction(myAsubInit);
epicsRegisterFunction(myAsubProcess);
}
