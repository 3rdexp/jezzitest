// SkinMgr.cpp: Implement interface ISkinMgr
//

#include "stdafx.h"

#include "scheme.h"
#include "SkinScheme.h"
#include "SkinMgr.h"




#ifndef __IStream_INTERFACE_DEFINED__
 #error "IStream Need"
#endif

WINCOMMCTRLAPI HIMAGELIST WINAPI ImageList_Read(LPSTREAM pstm);
WINCOMMCTRLAPI BOOL       WINAPI ImageList_Write(HIMAGELIST himl, LPSTREAM pstm);

WINCOMMCTRLAPI HRESULT WINAPI ImageList_ReadEx(DWORD dwFlags, LPSTREAM pstm, REFIID riid, PVOID* ppv);
WINCOMMCTRLAPI HRESULT WINAPI ImageList_WriteEx(HIMAGELIST himl, DWORD dwFlags, LPSTREAM pstm);

#include <objidl.h>
#include <commctrl.h>
#pragma comment(lib, "ComCtl32")

#include <atlctrls.h>

#include "../controls/frame.h"

#if 1
#include "../controls/menu.h"
#include "../controls/dialog.h"
#include "../controls/button.h"
#include "../controls/edit.h"
#include "../controls/combobox.h"
#include "../controls/tab.h"
#include "../controls/progress.h"
#include "../controls/spin.h"
#include "../controls/trackbar.h"
#include "../controls//header.h"
#include "../controls/statusbar.h"
#include "../controls/toolbar.h"
#include "../controls//listview.h"
#include "../controls/scrollbar.h"
#include "../libcoolsb/coolscroll.h"
#include "../libcoolsb/coolsb_detours.h"
#include "../base/SkinHook.h"
#endif



namespace Skin {

//	class CBitmapButton

STDMETHODIMP SkinMgr::InitControls(HINSTANCE hInst, DWORD dwType)
{
	HRESULT hr = S_OK;
	if (dwType & _installed_type)
		hr = HRESULT_FROM_WIN32(ERROR_PARTITION_FAILURE);


	// TODO: 这样的代码就没有办法写得好看一点?
	if (!(_installed_type & SKINCTL_BUTTON) && (dwType & SKINCTL_BUTTON) )
	{
		// typedef SkinButton<CBitmapButton> ssbuton;
        typedef SkinButton<WTL::CButton> ssbuton;
		bool f = ssbuton::Install(hInst);
		if (f)
			_installed_type |= SKINCTL_BUTTON;
	}

	if (!(_installed_type & SKINCTL_EDIT) && (dwType & SKINCTL_EDIT) )
	{
        typedef SkinEdit<WTL::CEdit> ssedit;
		bool f = ssedit::Install(hInst);
		if (f)
			_installed_type |= SKINCTL_EDIT;
	}

	if (!(_installed_type & SKINCTL_COMBOBOX) && (dwType & SKINCTL_COMBOBOX) )
	{
		typedef SkinComboBox<WTL::CComboBox> sscombobox;
		bool f = sscombobox::Install(hInst);
		if (f)
			_installed_type |= SKINCTL_COMBOBOX;
	}

	if (!(_installed_type & SKINCTL_TAB) && (dwType & SKINCTL_TAB) )
	{
		typedef SkinTabCtrl<WTL::CTabCtrl> sstab;
		bool f = sstab::Install(hInst);
		if (f)
			_installed_type |= SKINCTL_TAB;
	}

	if (!(_installed_type & SKINCTL_PROGRESS) && (dwType & SKINCTL_PROGRESS) )
	{
        typedef SkinProgressCtrl<WTL::CProgressBarCtrl> ssprogress;
		bool f = ssprogress::Install(hInst);
		if (f)
			_installed_type |= SKINCTL_PROGRESS;
	}

	if (!(_installed_type & SKINCTL_SPIN) && (dwType & SKINCTL_SPIN) )
	{
		typedef SkinUpDownCtrl<WTL::CUpDownCtrl> ssspin;
		bool f = ssspin::Install(hInst);
		if (f)
			_installed_type |= SKINCTL_SPIN;
	}

	if (!(_installed_type & SKINCTL_TRACKBAR) && (dwType & SKINCTL_TRACKBAR) )
	{
		typedef SkinTrackBarCtrl<WTL::CTrackBarCtrl> sstrackbar;
		bool f = sstrackbar::Install(hInst);
		if (f)
			_installed_type |= SKINCTL_TRACKBAR;
	}

	if (!(_installed_type & SKINCTL_HEADER) && (dwType & SKINCTL_HEADER) )
	{
		typedef SkinHeaderCtrl<WTL::CHeaderCtrl> ssheader;
		bool f = ssheader::Install(hInst);
		if (f)
			_installed_type |= SKINCTL_HEADER;
	}
    
	if (!(_installed_type & SKINCTL_STATUS) && (dwType & SKINCTL_STATUS) )
	{
		typedef SkinStatusBarCtrl<WTL::CStatusBarCtrl> ssstatusbar;
		bool f = ssstatusbar::Install(hInst);
		if (f)
			_installed_type |= SKINCTL_STATUS;
	}

    if (!(_installed_type & SKINCTL_MENU) && (dwType & SKINCTL_MENU) )
    {
        typedef SkinMenu<> skinmenu;
        bool f = skinmenu::Install(hInst);
        if (f)
            _installed_type |= SKINCTL_MENU;
    }

	/*
	if (!(_installed_type & SKINCTL_TOOLBAR) && (dwType & SKINCTL_TOOLBAR) )
	{
		typedef SkinToolBarCtrl<WTL::CToolBarCtrl> skintoolbar;
		bool f = skintoolbar::Install(hInst);
		if (f)
			_installed_type |= SKINCTL_TOOLBAR;
	}
	*/

	if (!(_installed_type & SKINCTL_LISTVIEW) && (dwType & SKINCTL_LISTVIEW) )
	{
		typedef SkinListViewCtrl<WTL::CListViewCtrl> skinlistview;
		bool f = skinlistview::Install(hInst);
		if (f)
			_installed_type |= SKINCTL_LISTVIEW;
	}

	CoolSB_InitializeApp();
	
	if (!(_installed_type & SKINCTL_SCROLLBAR) && (dwType & SKINCTL_SCROLLBAR) )
	{
		typedef SkinScrollBar<WTL::CScrollBar> skinscroll;
		bool f = skinscroll::Install(hInst);
		if (f)
			_installed_type |= SKINCTL_SCROLLBAR;
	}

	/*
	if (!(_installed_type & SKINCTL_REBAR) && (dwType & SKINCTL_REBAR) )
	{
		typedef SkinReBarCtrl<CReBarCtrl> skinrebar;
		bool f = skinrebar::Install(hInst);
		if (f)
			_installed_type |= SKINCTL_REBAR;
	}
    */

    if (!(_installed_type & SKINCTL_DIALOG) && (dwType & SKINCTL_DIALOG) )
    {       
        bool f = SkinDialog::Install(hInst);
        if (f)
            _installed_type |= SKINCTL_DIALOG;
    }

    if (!(_installed_type & SKINCTL_WINDOW) && (dwType & SKINCTL_WINDOW) )
    {
        bool f = SkinFrame::Install(hInst);
        if (f)
            _installed_type |= SKINCTL_WINDOW;
    }    
	
	CSkinHook::Initialize();

	// value => type
	// 1 => SkinButton
	// 2 => SkinEdit
	// template<T
	// 

	return S_OK;
}

STDMETHODIMP SkinMgr::UninitControls(HINSTANCE hInst, DWORD dwType)
{
	if (dwType & SKINCTL_BUTTON )
	{
//		typedef SkinButton<CButton> ssbuton;
//		ssbuton::Uninstall(hInst);
	}

//	CoolSB_UninitializeApp();
	return S_OK;
}

STDMETHODIMP SkinMgr::LoadTheme(LPCSTR file)
{
    ASSERT(file);

    // 1 尝试绝对路径
    char path[MAX_PATH];
    lstrcpyn(path, file, MAX_PATH);
    if (!PathFileExists(path))
    {
        // 相对路径
        GetCurrentDirectory(MAX_PATH, path);
        PathAppend(path, file);
    }
    
    bool f = false;
    if (file && _holder.parse_scheme(path))
    {
        f = true;
        _spCurrentScheme.Release();
        GetScheme("default", &_spCurrentScheme);

        ASSERT(_spCurrentScheme);
    }
    return f ? S_OK : E_FAIL;
}

STDMETHODIMP SkinMgr::GetScheme(LPCSTR style, ISkinScheme ** ppScheme)
{
    if (style && ppScheme)
    {
        const scheme_data * psd = _holder.get(style);
        if (psd)
        {
            ATL::CComObject<SkinScheme> * pss = 0;
            ATL::CComObject<SkinScheme>::CreateInstance(&pss);
            pss->SetSchemeData(psd, _holder.get_cache());

            return pss->QueryInterface(IID_ISkinScheme, (void**)ppScheme);
        }
    }
    return E_FAIL;
}

HRESULT WINAPI GetSkinMgr(ISkinMgr ** pMgr)
{
#if 0
	CComObject<SkinMgr> * p;
	HRESULT hr = ATL::CComObject<SkinMgr>::CreateInstance(&p);
	if (SUCCEEDED(hr))
	{
		p->QueryInterface(IID_ISkinMgr, (void**)pMgr);
	}
	return hr;
#elif 0
	static CComObjectGlobal<SkinMgr> * p = new CComObjectGlobal<SkinMgr>();
	HRESULT hr = E_OUTOFMEMORY;
	if (p)
	{
		hr = p->QueryInterface(IID_ISkinMgr, (void**)pMgr);
	}
	return hr;
#else
	HRESULT hr = E_OUTOFMEMORY;
	if (!gpMgr)
        gpMgr = new ATL::CComObjectGlobal<SkinMgr>();

    if (gpMgr)
		hr = gpMgr->QueryInterface(IID_ISkinMgr, (void**)pMgr);
#endif
	return hr;
}



HRESULT WINAPI GetCurrentScheme(ISkinScheme** ppScheme)
{
    CComPtr<ISkinMgr> p;
    GetSkinMgr(&p);

    if (p)
        return p->GetCurentScheme(ppScheme);

    return E_FAIL;
}

} // namespace Skin

