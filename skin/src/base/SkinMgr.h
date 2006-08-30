// SkinMgr.h: Implement interface ISkinMgr
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SKINMGR_H__03C770FD_2015_4E2A_858D_9BB71D1E6BD0__INCLUDED_)
#define AFX_SKINMGR_H__03C770FD_2015_4E2A_858D_9BB71D1E6BD0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "../skinitf.h"
#include "skiniid.h"
#include "scheme.h"

namespace Skin {

class ATL_NO_VTABLE SkinMgr : public CComObjectRoot
	, public ISkinMgr
{
public:
	typedef SkinMgr thisType;
	BEGIN_COM_MAP(thisType)
		COM_INTERFACE_ENTRY_IID(IID_ISkinMgr, ISkinMgr)
	END_COM_MAP()

	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct()
	{
		_installed_type = 0;
		
		return S_OK;
	}

	void FinalRelease()
	{
	}

// ISkinMgr
	STDMETHOD(InitControls)(HINSTANCE hInst, DWORD dwType);
	STDMETHOD(UninitControls)(HINSTANCE hInst, DWORD dwType);

	STDMETHOD(LoadTheme)(LPCSTR file)
	{
        bool f = false;
        if (file && _holder.parse_scheme(file))
        {
            f = true;
            _spCurrentScheme.Release();
            GetScheme("default", &_spCurrentScheme);
        }
		return f ? S_OK : E_FAIL;
	}

	STDMETHOD(GetScheme)(LPCSTR style, ISkinScheme ** ppScheme);
	STDMETHOD(GetCurentScheme)(ISkinScheme ** ppScheme)
    {
        return _spCurrentScheme.CopyTo(ppScheme);
    }

	STDMETHOD(SetCurrentScheme)(LPCSTR style)
	{
        if (style)
        {
            // TODO: check ÷ÿ∏¥…Ë÷√
            CComPtr<ISkinScheme> p;            
            if (S_OK == GetScheme(style, &p))
                _spCurrentScheme = p;
        }
		return S_OK;
	}

private:
    scheme_holder _holder;

	CComPtr<ISkinScheme> _spCurrentScheme;
	unsigned _installed_type;
};

__declspec(selectany) CComObjectGlobal<SkinMgr> * gpMgr = 0;


}; // namespace Skin


#endif // !defined(AFX_SKINMGR_H__03C770FD_2015_4E2A_858D_9BB71D1E6BD0__INCLUDED_)
