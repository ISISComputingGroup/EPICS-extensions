#include <stdio.h>

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
#include <atlconv.h>
#include <atlsafe.h>
#include <comdef.h>


#include <string>
#include <vector>
#include <map>
#include <list>
#include <stdexcept>
#include <sstream>
#include <fstream>
#include <iostream>

//#import "progid:isisicp.Idae" named_guids
//#import "isisicp.tlb" named_guids
//#import "Seci.tlb" named_guids
#import "LabVIEW.tlb" named_guids

//#include "Poco/DOM/DOMParser.h"
//#include "Poco/DOM/Document.h"
//#include "Poco/DOM/NodeIterator.h"
//#include "Poco/DOM/NodeFilter.h"
//#include "Poco/DOM/AutoPtr.h"
//#include "Poco/DOM/Element.h"
//#include "Poco/DOM/Text.h"
//#include "Poco/SAX/InputSource.h"

#include <Poco/Util/AbstractConfiguration.h>
#include <Poco/Util/XMLConfiguration.h>
//#include <Poco/AutoPtr.h>

#include <Poco/Path.h>

#include "isis_stuff.h"
#include "variant_utils.h"

ISISSTUFF::ISISSTUFF(const char *portName, const char *configFile)
{
		CoInitializeEx(NULL, COINIT_MULTITHREADED);
//		std::ifstream in(configFile);
//		Poco::XML::InputSource src(in);
//		Poco::XML::DOMParser parser;
//		Poco::AutoPtr<Poco::XML::Document> pDoc = parser.parse(&src);
//		Poco::XML::NodeIterator it(pDoc, Poco::XML::NodeFilter::SHOW_ALL);   // or SHOW_ELEMENTS ?
//		Poco::XML::Node* pNode = it.nextNode();

//		while (pNode)
//		{
//			std::cout<<pNode->nodeName()<<":"<< pNode->nodeValue()<<std::endl;
//			pNode = it.nextNode();
//		}

    m_cfg = new Poco::Util::XMLConfiguration(configFile);
  	m_extint = Poco::Path(m_cfg->getString("extint[@path]")).toString().c_str();

}

int ISISSTUFF::testlv()
{
	CComPtr<LabVIEW::_Application> lv;
	HRESULT hr = lv.CoCreateInstance(LabVIEW::CLSID_Application, 0, CLSCTX_LOCAL_SERVER);

	if (FAILED(hr))
	{
//		AtlReportError(GetObjectCLSID(), "CoCreateInstance failed"); 
		return -1;
	}

	CComBSTR ccombstr((char *)lv->GetVersion());

	if (ccombstr.Length() == 0)
	{
		//Did not talk to LabVIEW
//		AtlReportError(GetObjectCLSID(), "Failed to communicate with LabVIEW"); 
		return -1;
	}
	
	return 0;
}

void ISISSTUFF::getViRef(BSTR vi_name, bool reentrant, LabVIEW::VirtualInstrumentPtr &vi)
{
	UINT len = SysStringLen(vi_name);
	std::wstring ws(vi_name, SysStringLen(vi_name));

	vi_map_t::iterator it = m_vimap.find(ws);
	if(it != m_vimap.end())
	{
		try
		{
			vi = it->second.vi_ref;
			vi->GetExecState();
		}
		catch(...)
		{
			//Gets here if VI ref is not longer valid
			createViRef(vi_name, reentrant, vi);
		}
	}
	else
	{
		createViRef(vi_name, reentrant, vi);
	}
}

void ISISSTUFF::createViRef(BSTR vi_name, bool reentrant, LabVIEW::VirtualInstrumentPtr &vi)
{
	std::wstring ws(vi_name, SysStringLen(vi_name));

	CComPtr<LabVIEW::_Application> lv;
	HRESULT hr = lv.CoCreateInstance(LabVIEW::CLSID_Application, 0, CLSCTX_LOCAL_SERVER);

	if (FAILED(hr))
	{
		throw std::runtime_error("getViRef failed (could not connect to LabVIEW)");
	}

	if (reentrant)
	{
		vi = lv->GetVIReference(vi_name, "", 1, 8);
	}
	else
	{
		//If a VI is reentrant then always get it as reentrant
		vi = lv->GetVIReference(vi_name, "", 0, 0);
		if (vi->IsReentrant)
		{
			vi = lv->GetVIReference(vi_name, "", 1, 8);
			reentrant = true;
		}
	}

	ViRef viref;
	viref.vi_ref = vi;
	viref.reentrant = reentrant;
	m_vimap[ws] = viref;
}

template <typename T>
void ISISSTUFF::getLabviewValue(const std::string& portName, int addr, T* value)
{
	CComVariant v;
	std::string vi_name_xpath = Poco::format("item[@name='%s'].vi[@path]", portName);
	std::string control_name_xpath = Poco::format("item[@name='%s'].vi.control[@id=%d].read[@target]", portName, addr);
	// Use Poco::Path to convert to native (windows) style path as config file is UNIX style
	CComBSTR vi_name(Poco::Path(m_cfg->getString(vi_name_xpath)).toString().c_str());
	CComBSTR control_name(m_cfg->getString(control_name_xpath).c_str());
    getLabviewValue(vi_name, control_name, &v);
	v.ChangeType(CVarTypeInfo<T>::VT);
	*value = v.*(CVarTypeInfo<T>::pmField);	
}

void ISISSTUFF::getLabviewValue(BSTR vi_name, BSTR control_name, VARIANT* value)
{
	try
	{
		LabVIEW::VirtualInstrumentPtr vi;
		getViRef(vi_name, false, vi);
	
		*value = vi->GetControlValue(control_name).Detach();

		vi.Detach();
	}
	catch(...)
	{
		VariantInit(value);
	}
}

template <typename T>
void ISISSTUFF::setLabviewValue(const std::string& portName, int addr, const T& value, bool use_ext)
{
	CComVariant v(value), results;
	std::string vi_name_xpath = Poco::format("item[@name='%s'].vi[@path]", portName);
	std::string control_name_xpath = Poco::format("item[@name='%s'].vi.control[@id=%d].set[@target]", portName, addr);
	// Use Poco::Path to convert to native (windows) style path as config file is UNIX style
	CComBSTR vi_name(Poco::Path(m_cfg->getString(vi_name_xpath)).toString().c_str());
	CComBSTR control_name(m_cfg->getString(control_name_xpath).c_str());
	if (use_ext)
	{
		setLabviewValueExt(vi_name, control_name, v, &results);	
	}
	else
	{
		setLabviewValue(vi_name, control_name, v);	
	}
}

void ISISSTUFF::setLabviewValue(BSTR vi_name, BSTR control_name, const VARIANT& value)
{
	HRESULT hr = S_OK;
	
	try
	{
		LabVIEW::VirtualInstrumentPtr vi;
		getViRef(vi_name, false, vi);
		hr = vi->SetControlValue(control_name, value);
		vi.Detach();
	}
	catch(...)
	{
		hr = -1;
	}

	if (FAILED(hr))
	{
		throw std::runtime_error("SetLabviewValue failed");
	}
}

void ISISSTUFF::setLabviewValueExt(BSTR vi_name, BSTR control_name, const VARIANT& value, VARIANT* results)
{

	CComSafeArray<BSTR> names(6);
	names[0].AssignBSTR(_bstr_t(L"VI Name"));
	names[1].AssignBSTR(_bstr_t(L"Control Name"));
	names[2].AssignBSTR(_bstr_t(L"String Control Value"));
	names[3].AssignBSTR(_bstr_t(L"Variant Control Value"));
	names[4].AssignBSTR(_bstr_t(L"Machine Name"));
	names[5].AssignBSTR(_bstr_t(L"Return Message"));

	_variant_t n;
	n.vt = VT_ARRAY | VT_BSTR;
	n.parray = names.Detach();

	CComSafeArray<VARIANT> values(6);
	values[0] = vi_name;
	values[1] = control_name;
	//values[2] = 
	values[3] = value;
	//values[4] = 
	//values[5] = 

	_variant_t v;
	v.vt = VT_ARRAY | VT_VARIANT;
	v.parray = values.Detach();

	//Must be called as reentrant!
	callLabview(m_extint, n, v, true, results);
}

void ISISSTUFF::callLabview(BSTR vi_name, VARIANT& names, VARIANT& values, VARIANT_BOOL reentrant, VARIANT* results)
{
	HRESULT hr = S_OK;

	try
	{
		LabVIEW::VirtualInstrumentPtr vi;

		if (reentrant)
		{
			getViRef(vi_name, true, vi);
		}
		else
		{
			getViRef(vi_name, false, vi);
		}

		hr = vi->Call(&names, &values);
		vi.Detach();

		CComVariant var(values);
		var.Detach(results);
	}
	catch(...)
	{
		hr = -1;
	}

	if (FAILED(hr))
	{
		throw std::runtime_error("CallLabviewValue failed");
	}
}


void ISISSTUFF::getViState(BSTR vi_name, VARIANT* value)
{
	HRESULT hr = S_OK;

	try
	{
		LabVIEW::VirtualInstrumentPtr vi;
		getViRef(vi_name, false, vi);
		CComVariant wrapper;
		wrapper.ChangeType(VT_INT);
		wrapper.Detach(value);
		value->intVal = vi->ExecState;
		vi.Detach();
	}
	catch(...)
	{
		hr = -1;
	}

	if (FAILED(hr))
	{
		throw std::runtime_error("GetViState failed");
	}
}

void ISISSTUFF::startVi(BSTR vi_name)
{
	HRESULT hr = S_OK;

	try
	{
		LabVIEW::VirtualInstrumentPtr vi;
		getViRef(vi_name, false, vi);
		// LabVIEW::ExecStateEnum.eIdle = 1
		if (vi->ExecState == 1)
		{
			vi->Run(true);
		}
		vi.Detach();
	}
	catch(...)
	{
		hr = -1;
	}

	if (FAILED(hr))
	{
		throw std::runtime_error("StartVi failed");
	}
}

void ISISSTUFF::stopVi(BSTR vi_name)
{
	HRESULT hr = S_OK;

	try
	{
		LabVIEW::VirtualInstrumentPtr vi;
		getViRef(vi_name, false, vi);
		// LabVIEW::ExecStateEnum.eIdle = 1
		// LabVIEW::ExecStateEnum.eRunTopLevel = 2
		if (vi->ExecState == 1 || vi->ExecState == 2)
		{
			vi->Abort();
		}
		vi.Detach();
	}
	catch(...)
	{
		hr = -1;
	}

	if (FAILED(hr))
	{
		throw std::runtime_error("StopVi failed");
	}
}

void ISISSTUFF::closeViFrontPanel(BSTR vi_name)
{
	HRESULT hr = S_OK;

	try
	{
		LabVIEW::VirtualInstrumentPtr vi;
		getViRef(vi_name, false, vi);
		hr = vi->CloseFrontPanel();
		vi.Detach();
	}
	catch(...)
	{
		hr = -1;
	}

	if (FAILED(hr))
	{
		throw std::runtime_error("CloseViFrontPanel failed");
	}
}


template void ISISSTUFF::setLabviewValue(const std::string& portName, int addr, const double& value, bool use_ext);
template void ISISSTUFF::setLabviewValue(const std::string& portName, int addr, const int& value, bool use_ext);

template void ISISSTUFF::getLabviewValue(const std::string& portName, int addr, double* value);
template void ISISSTUFF::getLabviewValue(const std::string& portName, int addr, int* value);

