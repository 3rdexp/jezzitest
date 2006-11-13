#include "stdafx.h"

#include "../skinitf.h"
#include "../base/skinmgr.h"

#define INITGUID
#include "../base/skiniid.h"

static bool init_library(HINSTANCE hInst)
{
    using namespace Skin;
    using namespace ATL;

    CComPtr<ISkinMgr> spmgr;
    GetSkinMgr(&spmgr);

    if (spmgr && S_OK == spmgr->LoadTheme("test.cfg"))
    {   
        spmgr->InitControls(hInst, SKINCTL_ALL);
    }
    return true;
}

static bool uninit_library(HINSTANCE hInst)
{
    using namespace Skin;
    using namespace ATL;

    CComPtr<ISkinMgr> spmgr;
    GetSkinMgr(&spmgr);
    if (spmgr)
        spmgr->UninitControls(hInst, SKINCTL_ALL);

    delete gpMgr; 
    return true;
}



class SkinModule : public ATL::CAtlDllModuleT< SkinModule >
{
public :
	// DECLARE_LIBID(LIBID_blank_dllLib)
	// DECLARE_REGISTRY_APPID_RESOURCEID(IDR_BLANK_DLL, "{0A272182-8F3F-44AB-923A-62C965D11DD5}")
};

__declspec(selectany) SkinModule _AtlModule;

#ifdef __AFX_H__

class Cmfc_dllApp : public CWinApp
{
public:
    Cmfc_dllApp() {}

    // 重写
public:
    virtual BOOL InitInstance();
    virtual int ExitInstance();

    DECLARE_MESSAGE_MAP()
};

BEGIN_MESSAGE_MAP(Cmfc_dllApp, CWinApp)
END_MESSAGE_MAP()


BOOL Cmfc_dllApp::InitInstance()
{
    CWinApp::InitInstance();
    init_library(m_hInstance);

    return TRUE;
}


int Cmfc_dllApp::ExitInstance()
{
    CWinApp::ExitInstance();
    uninit_library(m_hInstance);
    return 0;
}

#else // __AFX_H__

BOOL WINAPI DllMain(HINSTANCE hInst, DWORD dwReason, LPVOID lpReserved)
{
	BOOL ret = _AtlModule.DllMain(dwReason, lpReserved);
	if (dwReason == DLL_PROCESS_ATTACH)
	{
        CoInitialize(NULL);
		init_library(hInst);
	}
	else if (dwReason == DLL_PROCESS_DETACH)
	{
		uninit_library(hInst);

		HRESULT hr = _AtlModule.DllCanUnloadNow();
		ASSERT(SUCCEEDED(hr));

        CoUninitialize();
	}
	return ret;
}

#endif

// TODO: 防止有dll's window还存在就退出dll，让调用者检测或者调用DllCanUnloadNow？ find a good way.
STDAPI DllCanUnloadNow(void)
{
	return _AtlModule.DllCanUnloadNow();
}