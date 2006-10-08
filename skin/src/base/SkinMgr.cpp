// SkinMgr.cpp: Implement interface ISkinMgr
//

#include "stdafx.h"

#include "scheme.h"
#include "SkinScheme.h"
#include "SkinMgr.h"

#include "../controls/button.h"
#include "../controls/frame.h"
#include "../controls/dialog.h"
#include "../controls/edit.h"
#include "../controls/combobox.h"
#include "../controls/tab.h"
#include "../controls/progress.h"
#include "../controls/spin.h"
#include "../controls/trackbar.h"
#include "../controls//header.h"
#include "../controls/statusbar.h"
#include "../controls/menu.h"
#include "../controls/toolbar.h"
#include "../controls/rebar.h"
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
		typedef SkinButton<CButton> ssbuton;
		bool f = ssbuton::Install(hInst);
		if (f)
			_installed_type |= SKINCTL_BUTTON;
	}

	if (!(_installed_type & SKINCTL_EDIT) && (dwType & SKINCTL_EDIT) )
	{
		// typedef SkinButton<CBitmapButton> ssbuton;
		typedef SkinEdit<CEdit> ssedit;
		bool f = ssedit::Install(hInst);
		if (f)
			_installed_type |= SKINCTL_EDIT;
	}

	if (!(_installed_type & SKINCTL_COMBOBOX) && (dwType & SKINCTL_COMBOBOX) )
	{
		// typedef SkinButton<CBitmapButton> ssbuton;
		typedef SkinComboBox<CComboBox> sscombobox;
		bool f = sscombobox::Install(hInst);
		if (f)
			_installed_type |= SKINCTL_COMBOBOX;
	}

	if (!(_installed_type & SKINCTL_TAB) && (dwType & SKINCTL_TAB) )
	{
		// typedef SkinButton<CBitmapButton> ssbuton;
		typedef SkinTabCtrl<CTabCtrl> sstab;
		bool f = sstab::Install(hInst);
		if (f)
			_installed_type |= SKINCTL_TAB;
	}

	if (!(_installed_type & SKINCTL_PROGRESS) && (dwType & SKINCTL_PROGRESS) )
	{
		// typedef SkinButton<CBitmapButton> ssbuton;
		typedef SkinProgressCtrl<CProgressBarCtrl> ssprogress;
		bool f = ssprogress::Install(hInst);
		if (f)
			_installed_type |= SKINCTL_PROGRESS;
	}

	if (!(_installed_type & SKINCTL_SPIN) && (dwType & SKINCTL_SPIN) )
	{
		// typedef SkinButton<CBitmapButton> ssbuton;
		typedef SkinUpDownCtrl<CUpDownCtrl> ssspin;
		bool f = ssspin::Install(hInst);
		if (f)
			_installed_type |= SKINCTL_SPIN;
	}

	if (!(_installed_type & SKINCTL_TRACKBAR) && (dwType & SKINCTL_TRACKBAR) )
	{
		// typedef SkinButton<CBitmapButton> ssbuton;
		typedef SkinTrackBarCtrl<CTrackBarCtrl> sstrackbar;
		bool f = sstrackbar::Install(hInst);
		if (f)
			_installed_type |= SKINCTL_TRACKBAR;
	}

	if (!(_installed_type & SKINCTL_HEADER) && (dwType & SKINCTL_HEADER) )
	{
		// typedef SkinButton<CBitmapButton> ssbuton;
		typedef SkinHeaderCtrl<CHeaderCtrl> ssheader;
		bool f = ssheader::Install(hInst);
		if (f)
			_installed_type |= SKINCTL_HEADER;
	}
    
	if (!(_installed_type & SKINCTL_STATUS) && (dwType & SKINCTL_STATUS) )
	{
		// typedef SkinButton<CBitmapButton> ssbuton;
		typedef SkinStatusBarCtrl<CStatusBarCtrl> ssstatusbar;
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

	if (!(_installed_type & SKINCTL_TOOLBAR) && (dwType & SKINCTL_TOOLBAR) )
	{
		typedef SkinToolBarCtrl<CToolBarCtrl> skintoolbar;
		bool f = skintoolbar::Install(hInst);
		if (f)
			_installed_type |= SKINCTL_TOOLBAR;
	}

	if (!(_installed_type & SKINCTL_REBAR) && (dwType & SKINCTL_REBAR) )
	{
		typedef SkinReBarCtrl<CReBarCtrl> skinrebar;
		bool f = skinrebar::Install(hInst);
		if (f)
			_installed_type |= SKINCTL_REBAR;
	}

	/*
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
    
	*/
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
            CComObject<SkinScheme> * pss = 0;
            CComObject<SkinScheme>::CreateInstance(&pss);
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
	HRESULT hr = CComObject<SkinMgr>::CreateInstance(&p);
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
		gpMgr = new CComObjectGlobal<SkinMgr>();

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

