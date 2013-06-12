#include <stdio.h>

//#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
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
#include <algorithm>

#include "isisdaeInterface.h"
#include "variant_utils.h"

#include <macLib.h>
#include <epicsGuard.h>

#define MAX_PATH_LEN 256

static epicsThreadOnceId onceId = EPICS_THREAD_ONCE_INIT;

static void initCOM(void*)
{
	CoInitializeEx(NULL, COINIT_MULTITHREADED);
}

/// The Microsoft ATL _com_error is not derived from std::exception hence this bit of code to throw our own COMexception() instead
void __stdcall _com_raise_error(HRESULT hr, IErrorInfo* perrinfo) 
{
	_com_error com_error(hr, perrinfo);
//	std::string message = "(" + com_error.Source() + ") " + com_error.Description();
	std::string message = com_error.Description();  // for LabVIEW generated messages, Description() already includes Source()
    throw COMexception(message, hr);
}


/// \param[in] configSection @copydoc initArg1
/// \param[in] configFile @copydoc initArg2
/// \param[in] host @copydoc initArg3
/// \param[in] options @copydoc initArg4
/// \param[in] progid @copydoc initArg5
/// \param[in] username @copydoc initArg6
/// \param[in] password @copydoc initArg7
isisdaeInterface::isisdaeInterface(const char* host, int options, const char* username, const char* password) : 
            m_pidentity(NULL), m_options(options), m_username(username != NULL? username : ""), m_password(password != NULL ? password : "")
{
	epicsThreadOnce(&onceId, initCOM, NULL);
    if (host != NULL && host[0] != '\0') 
	{
	    m_host = host;
	}
	else
	{
//		char name_buffer[MAX_COMPUTERNAME_LENGTH + 1];
//		DWORD name_size = MAX_COMPUTERNAME_LENGTH + 1;
//		if ( GetComputerNameEx(ComputerNameNetBIOS, name_buffer, &name_size) != 0 )
//		{
//			m_host = name_buffer;
//		}
//		else
//		{
//			m_host = "localhost";
//		}			
		m_host = "localhost";
	}
	epicsAtExit(epicsExitFunc, this);
	m_clsid = isisicpLib::CLSID_dae;
		wchar_t* progid_str = NULL;
		if ( ProgIDFromCLSID(m_clsid, &progid_str) == S_OK )
		{
			m_progid = CW2CT(progid_str);
			CoTaskMemFree(progid_str);
		}
		else
		{
			m_progid = "isisicp.dae";
		}
	wchar_t* clsid_str = NULL;
	if ( StringFromCLSID(m_clsid, &clsid_str) == S_OK )
	{
		std::cerr << "Using ProgID \"" << m_progid << "\" CLSID " << CW2CT(clsid_str) << std::endl;
		CoTaskMemFree(clsid_str);
	}
	else
	{
		std::cerr << "StringFromCLSID() failed" << std::endl;
	}
}

void isisdaeInterface::epicsExitFunc(void* arg)
{
    isisdaeInterface* dcomint = static_cast<isisdaeInterface*>(arg);
	if (dcomint == NULL)
	{
		return;
	}
//	if ( dcomint->checkOption(viAlwaysStopOnExit) )
//	{
//		dcomint->stopVis(false);
//	}
//	else if ( dcomint->checkOption(viStopOnExitIfStarted) )
//	{
//		dcomint->stopVis(true);
//	}
}

long isisdaeInterface::nParams()
{
	long n = 0;
	n = 10;
	return n;
}

void isisdaeInterface::getParams(std::map<std::string,std::string>& res)
{
	res.clear();
}

COAUTHIDENTITY* isisdaeInterface::createIdentity(const std::string& user, const std::string&  domain, const std::string& pass)
{
	if (user.size() == 0)
	{
		return NULL;
	}
    COAUTHIDENTITY* pidentity = new COAUTHIDENTITY;
    pidentity->Domain = (USHORT*)strdup(domain.c_str());
    pidentity->DomainLength = static_cast<ULONG>(strlen((const char*)pidentity->Domain));
    pidentity->Flags = SEC_WINNT_AUTH_IDENTITY_ANSI;
    pidentity->Password = (USHORT*)strdup(pass.c_str());
    pidentity->PasswordLength = static_cast<ULONG>(strlen((const char*)pidentity->Password));
    pidentity->User = (USHORT*)strdup(user.c_str());
    pidentity->UserLength = static_cast<ULONG>(strlen((const char*)pidentity->User));
    return pidentity;
}

HRESULT isisdaeInterface::setIdentity(COAUTHIDENTITY* pidentity, IUnknown* pUnk)
{
    HRESULT hr;
    if (pidentity != NULL)
    {
       hr = CoSetProxyBlanket(pUnk, RPC_C_AUTHN_WINNT, RPC_C_AUTHZ_NONE, NULL, 
            RPC_C_AUTHN_LEVEL_DEFAULT, RPC_C_IMP_LEVEL_IMPERSONATE, pidentity, EOAC_NONE);
        if (FAILED(hr))
        {
			std::cerr << "setIdentity failed" << std::endl;
            return hr;
        }
    }
    return S_OK;
}


void isisdaeInterface::checkConnection()
{
	epicsThreadOnce(&onceId, initCOM, NULL);
	HRESULT hr;
	epicsGuard<epicsMutex> _lock(m_lock);
	if ( (m_icp != NULL) && (m_icp->areYouThere() == S_OK) )
	{
		;
	}
	else if (m_host.size() > 0)
	{
		std::cerr << "(Re)Making connection to ISISICP on " << m_host << std::endl;
		CComBSTR host(m_host.c_str());
		m_pidentity = createIdentity(m_username, m_host, m_password);
		COAUTHINFO* pauth = new COAUTHINFO;
		COSERVERINFO csi = { 0, NULL, NULL, 0 };
		pauth->dwAuthnSvc = RPC_C_AUTHN_WINNT;
		pauth->dwAuthnLevel = RPC_C_AUTHN_LEVEL_DEFAULT;
		pauth->dwAuthzSvc = RPC_C_AUTHZ_NONE;
		pauth->dwCapabilities = EOAC_NONE;
		pauth->dwImpersonationLevel = RPC_C_IMP_LEVEL_IMPERSONATE;
		pauth->pAuthIdentityData = m_pidentity;
		pauth->pwszServerPrincName = NULL;
		csi.pwszName = host;
		csi.pAuthInfo = pauth;
		MULTI_QI mq[ 1 ] = { 0 }; 
		mq[ 0 ].pIID = &isisicpLib::IID_Idae; // &IID_IDispatch; 
		mq[ 0 ].pItf = NULL; 
		mq[ 0 ].hr   = S_OK; 
		hr = CoCreateInstanceEx( m_clsid, NULL, CLSCTX_REMOTE_SERVER | CLSCTX_LOCAL_SERVER, &csi, 1, mq ); 
		if( FAILED( hr ) ) 
		{ 
			hr = CoCreateInstanceEx( m_clsid, NULL, CLSCTX_ALL, &csi, 1, mq );
		}
		if( FAILED( hr ) ) 
		{
 			throw COMexception("CoCreateInstanceEx (ISISICP) ", hr);
		} 
		if( S_OK != mq[ 0 ].hr || NULL == mq[ 0 ].pItf ) 
		{ 
 			throw COMexception("CoCreateInstanceEx (ISISICP)(mq) ", mq[ 0 ].hr);
		} 
		setIdentity(m_pidentity, mq[ 0 ].pItf);
		m_icp.Release();
		m_icp.Attach( reinterpret_cast< isisicpLib::Idae* >( mq[ 0 ].pItf ) ); 
	}
	else
	{
		std::cerr << "(Re)Making local connection to ISISICP" << std::endl;
		m_pidentity = NULL;
		m_icp.Release();
		hr = m_icp.CoCreateInstance(m_clsid, NULL, CLSCTX_LOCAL_SERVER);
		if( FAILED( hr ) ) 
		{
 			throw COMexception("CoCreateInstance (ISISICP) ", hr);
		} 
	}
}


unsigned long isisdaeInterface::getGoodFrames()
{
	BSTR messages = NULL;
	checkConnection();
	return m_icp->getGoodFramesTotal(&messages);
}

unsigned long isisdaeInterface::getRawFrames()
{
	BSTR messages = NULL;
	checkConnection();
	return m_icp->getRawFramesTotal(&messages);
}

double isisdaeInterface::getGoodUAH()
{
	BSTR messages = NULL;
	checkConnection();
	return m_icp->getGoodUAmpH(&messages);
}

int isisdaeInterface::beginRun()
{
	BSTR messages = NULL;
	checkConnection();
	return m_icp->beginRun(&messages);
}
