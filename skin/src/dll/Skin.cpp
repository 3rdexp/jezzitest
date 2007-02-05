
// skin.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"


#include "../skinitf.h"
#include "../base/skinmgr.h"

#define INITGUID
#include "../base/skiniid.h"

class SkinModule : public ATL::CAtlDllModuleT< SkinModule >
{
public :
	// DECLARE_LIBID(LIBID_blank_dllLib)
	// DECLARE_REGISTRY_APPID_RESOURCEID(IDR_BLANK_DLL, "{0A272182-8F3F-44AB-923A-62C965D11DD5}")
};

SkinModule _AtlModule;



void init_library(HINSTANCE hInst)
{
	using namespace Skin;
	using namespace ATL;

	CComPtr<ISkinMgr> spmgr;
	GetSkinMgr(&spmgr);

	if (spmgr && S_OK == spmgr->LoadTheme("test.cfg"))
	{   
		spmgr->InitControls(hInst, SKINCTL_ALL);
	}
}

void uninit_library(HINSTANCE hInst)
{
	using namespace Skin;
	using namespace ATL;

	{
		CComPtr<ISkinMgr> spmgr;
		GetSkinMgr(&spmgr);
		if (spmgr)
			spmgr->UninitControls(hInst, SKINCTL_ALL);
	}

	delete gpMgr; 
}

BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
	//using namespace Skin;
	using namespace ATL;
	hInstance;

	BOOL ret = _AtlModule.DllMain(dwReason, lpReserved);
	if (dwReason == DLL_PROCESS_ATTACH)
	{
		CoInitialize(NULL);
		init_library( hInstance );
		
	}
	else if (dwReason == DLL_PROCESS_DETACH)
	{
	
		uninit_library( hInstance );

		HRESULT hr = _AtlModule.DllCanUnloadNow();
		//ASSERT(SUCCEEDED(hr));
	
		CoUninitialize();
	}
	return ret;
}

// TODO: 防止有dll's window还存在就退出dll，让调用者检测或者调用DllCanUnloadNow？ find a good way.
STDAPI DllCanUnloadNow(void)
{
	return _AtlModule.DllCanUnloadNow();
}