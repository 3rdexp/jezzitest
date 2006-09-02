// SkinMgr.cpp: Implement interface ISkinMgr
//

#include "stdafx.h"

#include "scheme.h"
#include "SkinScheme.h"
#include "SkinMgr.h"

#include "../controls/button.h"
#include "../controls/frame.h"
#include "../controls/dialog.h"


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

    if (!(_installed_type & SKINCTL_WINDOW) && (dwType & SKINCTL_WINDOW) )
    {
        bool f = SkinFrame::Install(hInst);
        if (f)
            _installed_type |= SKINCTL_WINDOW;
    }
    if (!(_installed_type & SKINCTL_DIALOG) && (dwType & SKINCTL_DIALOG) )
    {       
        bool f = SkinDialog::Install(hInst);
        if (f)
            _installed_type |= SKINCTL_DIALOG;
    }

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

} // namespace Skin