#include "stdafx.h"

#include "../skinitf.h"
#include "../base/skinmgr.h"

#define INITGUID
#include "../base/skiniid.h"


class SkinModule : public CAtlDllModuleT< SkinModule >
{
public :
	// DECLARE_LIBID(LIBID_blank_dllLib)
	// DECLARE_REGISTRY_APPID_RESOURCEID(IDR_BLANK_DLL, "{0A272182-8F3F-44AB-923A-62C965D11DD5}")
};

__declspec(selectany) SkinModule _AtlModule;

BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
	using namespace Skin;
	hInstance;
	BOOL ret = _AtlModule.DllMain(dwReason, lpReserved);
	if (dwReason == DLL_PROCESS_ATTACH)
	{
        CoInitialize(NULL);
		CComPtr<ISkinMgr> spmgr;
		GetSkinMgr(&spmgr);
		if (spmgr && S_OK == spmgr->LoadTheme("test.cfg"))
        {   
            spmgr->InitControls(hInstance, SKINCTL_ALL);
        }
	}
	else if (dwReason == DLL_PROCESS_DETACH)
	{
		{
		CComPtr<ISkinMgr> spmgr;
		GetSkinMgr(&spmgr);
		if (spmgr)
			spmgr->UninitControls(hInstance, SKINCTL_ALL);
		}

		delete gpMgr;

		HRESULT hr = _AtlModule.DllCanUnloadNow();
		ASSERT(SUCCEEDED(hr));

        CoUninitialize();
	}
	return ret;
}

// TODO: 防止有dll's window还存在就退出dll，让调用者检测或者调用DllCanUnloadNow？ find a good way.
STDAPI DllCanUnloadNow(void)
{
	return _AtlModule.DllCanUnloadNow();
}