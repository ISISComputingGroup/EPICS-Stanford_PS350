#ifndef ISISDAE_INTERFACE_H
#define ISISDAE_INTERFACE_H

/// \file lvDCOMInterface.h header for #lvDCOMInterface class.

#include <stdio.h>

//#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
#include <windows.h>
#include <tchar.h>
//#include <sys/stat.h>
//#include <process.h>
//#include <fcntl.h>

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

#include <epicsMutex.h>
#include <epicsThread.h>
#include <epicsExit.h>

//#import "isisicp.tlb" named_guids
// The above statement would generate isisicp.tlh and isisicp.tli but we include pre-built versions here
#include "isisicp.tlh"

/// Options that can be passed from EPICS iocsh via #lvDCOMConfigure command.
/// In the iocBoot st.cmd file you will need to add the relevant integer enum values together and pass this single integer value.
enum isisdaeOptions
{
    viWarnIfIdle = 1, 				///< If the LabVIEW VI is idle when we connect to it, issue a warning message  
	viStartIfIdle = 2, 				///< If the LabVIEW VI is idle when we connect to it, attempt to start it
	viStopOnExitIfStarted = 4, 		///< On IOC exit, stop any LabVIEW VIs that we started due to #viStartIfIdle being specified
	viAlwaysStopOnExit = 8			///< On IOC exit, stop any LabVIEW VIs that we have connected to
};	

class isisdaeInterface
{
public:
	isisdaeInterface(const char* host, int options, const char* username, const char* password);
	long nParams();
	void getParams(std::map<std::string,std::string>& res);
//	template<typename T> void setLabviewValue(const char* param, const T& value);
//	template<typename T> void getLabviewValue(const char* param, T* value);
//	template<typename T> void getLabviewValue(const char* param, T* value, size_t nElements, size_t& nIn);
	virtual ~isisdaeInterface() { }
	unsigned long getGoodFrames();
	unsigned long getRawFrames();
	double getGoodUAH();
	int beginRun();
	

private:
	std::string m_host;
	std::string m_progid;
	CLSID m_clsid;
	std::string m_username;
	std::string m_password;
	int m_options; ///< the various #lvDCOMOptions currently in use
	epicsMutex m_lock;
	CComPtr<isisicpLib::Idae> m_icp;
	COAUTHIDENTITY* m_pidentity;

	COAUTHIDENTITY* createIdentity(const std::string& user, const std::string& domain, const std::string& pass);
	HRESULT setIdentity(COAUTHIDENTITY* pidentity, IUnknown* pUnk);
	static void epicsExitFunc(void* arg);
//	void stopVis(bool only_ones_we_started);
	bool checkOption(isisdaeOptions option) { return ( m_options & static_cast<int>(option) ) != 0; }
	void checkConnection();
};

#endif /* ISISDAE_INTERFACE_H */
