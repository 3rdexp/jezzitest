
// skin.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"
#include "skin.h"

#include "../skinitf.h"
#include "../base/skinmgr.h"

#define INITGUID
#include "../base/skiniid.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//
//	Note!
//
//		If this DLL is dynamically linked against the MFC
//		DLLs, any functions exported from this DLL which
//		call into MFC must have the AFX_MANAGE_STATE macro
//		added at the very beginning of the function.
//
//		For example:
//
//		extern "C" BOOL PASCAL EXPORT ExportedFunction()
//		{
//			AFX_MANAGE_STATE(AfxGetStaticModuleState());
//			// normal function body here
//		}
//
//		It is very important that this macro appear in each
//		function, prior to any calls into MFC.  This means that
//		it must appear as the first statement within the 
//		function, even before any object variable declarations
//		as their constructors may generate calls into the MFC
//		DLL.
//
//		Please see MFC Technical Notes 33 and 58 for additional
//		details.
//

// CSkinApp

BEGIN_MESSAGE_MAP(CSkinApp, CWinApp)
END_MESSAGE_MAP()


/*
class SkinModule : public ATL::CAtlDllModuleT< SkinModule >
{
public :
	// DECLARE_LIBID(LIBID_blank_dllLib)
	// DECLARE_REGISTRY_APPID_RESOURCEID(IDR_BLANK_DLL, "{0A272182-8F3F-44AB-923A-62C965D11DD5}")
};

__declspec(selectany) SkinModule _AtlModule;
*/
ATL::CComModule _Module;




// CSkinApp construction

CSkinApp::CSkinApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}


// The one and only CSkinApp object

CSkinApp theApp;

// CSkinApp initialization

BOOL CSkinApp::InitInstance()
{
	CWinApp::InitInstance();

//	BOOL ret = _AtlModule.DllMain(DLL_PROCESS_ATTACH, 0);

	_Module.Init(0, m_hInstance);

	CoInitialize(NULL);
	init_library(m_hInstance);
	return TRUE;
}

int CSkinApp::ExitInstance()
{
	CWinApp::ExitInstance();

	//BOOL ret = _AtlModule.DllMain(DLL_PROCESS_DETACH, 0);

	_Module.Term();

	uninit_library(m_hInstance);
	CoInitialize(NULL);
	return 0;
}


bool CSkinApp::init_library(HINSTANCE hInst)
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

bool CSkinApp::uninit_library(HINSTANCE hInst)
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

    return true;
}