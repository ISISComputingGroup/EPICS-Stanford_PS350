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

#include <Poco/Util/AbstractConfiguration.h>
#include <Poco/Util/XMLConfiguration.h>
#include <Poco/Format.h>
#include <Poco/RWLock.h>

struct ViRef
{
	LabVIEW::VirtualInstrumentPtr vi_ref;
	bool reentrant;
};


class ISISSTUFF
{
public:
	ISISSTUFF(const char *portName, const char *configFile, const char* host);
	template<typename T> void setLabviewValue(const std::string& portName, int addr, const T& value);
	template<typename T> void getLabviewValue(const std::string& portName, int addr, T* value);

private:
	typedef std::map<std::wstring, ViRef> vi_map_t;
	vi_map_t m_vimap;
	Poco::RWLock m_lock;
	Poco::Util::AbstractConfiguration *m_cfg;
	CComBSTR m_extint;
	std::string m_host;

	int testlv();
	std::string doPath(const std::string& xpath);
	void getViRef(BSTR vi_name, bool reentrant, LabVIEW::VirtualInstrumentPtr &vi);
	void createViRef(BSTR vi_name, bool reentrant, LabVIEW::VirtualInstrumentPtr &vi);
	void getLabviewValue(BSTR vi_name, BSTR control_name, VARIANT* value);
	void setLabviewValue(BSTR vi_name, BSTR control_name, const VARIANT& value);
	void setLabviewValueExt(BSTR vi_name, BSTR control_name, const VARIANT& value, VARIANT* results);
	void callLabview(BSTR vi_name, VARIANT& names, VARIANT& values, VARIANT_BOOL reentrant, VARIANT* results);
	void getViState(BSTR vi_name, VARIANT* value);
	void startVi(BSTR vi_name);
	void stopVi(BSTR vi_name);
	void closeViFrontPanel(BSTR vi_name);
	COAUTHIDENTITY* createIdentity(const std::string& user, const std::string& domain, const std::string& pass);
	HRESULT setIdentity(COAUTHIDENTITY* pidentity, IUnknown* pUnk);


};
