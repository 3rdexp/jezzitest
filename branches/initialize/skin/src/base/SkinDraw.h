// SkinDraw.h: interface for the SkinDraw class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SKINDRAW_H__ECE45FB1_7DEB_4E14_AC98_D31304CE4B69__INCLUDED_)
#define AFX_SKINDRAW_H__ECE45FB1_7DEB_4E14_AC98_D31304CE4B69__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <tmschema.h>

namespace Skin {

// 帮助快速实现 ISkinDraw
template<class T, int nPartId>
class ATL_NO_VTABLE SkinDrawImpl : public ISkinDraw
{
public:
	STDMETHOD(DrawBackground)(HDC hdc, int iPartId, int iStateId, const RECT *pRect, 
		const RECT *pClipRect)
	{
		// get ISkinSchmem
		// pss->Draw(
		return S_OK;
	}
	
	STDMETHOD(SetParamString)(DrawParam dp, LPCSTR szValue)
	{
		if(dp == DP_CONFIG_NAME)
		{
			if (szValue) 
				_cofig_name = szValue;
			else
				_cofig_name.clear();
		}
		return S_OK;
	}

	STDMETHOD_(HBRUSH, GetBrush)(int iColorID)
	{
		return 0;
	}

protected:
	ISkinScheme* GetCurrentScheme()
	{
		CComPtr<ISkinMgr> spmgr;
		CComPtr<ISkinScheme> spss;

		HRESULT hr = GetSkinMgr(&spmgr);
		ASSERT(spmgr);
		if (spmgr)
		{
			spmgr->GetCurentScheme(&spss);
		}

		return spss.Detach();
	}

	COLORREF GetColor(int iPartId, int iStateId)
	{
	}

#if 0
	ISkinConfig * GetDefaultConfig()
	{
		CComPtr<ISkinMgr> spmgr;
		if( S_OK == GetSkinMgr(&spmgr) && spmgr )
		{
			ISkinConfig* psc = 0;
			HRESULT hr = spmgr->GetDefaultConfig( nControlType, &psc);
			ASSERT( SUCCEEDED(hr) );
			return psc;
		}
		return 0;
	}
	LPCSTR InternalGetName(int iPartId, int iStateId)
	{
		//ASSERT(_spCustomConfig);

		LPCSTR szName = NULL;
		BOOL fUseDefault = TRUE;
		if( _spCustomConfig)
		{
			fUseDefault = FALSE;
			szName = _spCustomConfig->GetName(iPartId, iStateId, &fUseDefault);
		}
		if( !szName && fUseDefault )
		{
			CComPtr<ISkinConfig> sp;
			sp.p = GetDefaultConfig();
			if(sp)			
				szName = sp->GetName(iPartId, iStateId, NULL);
		}

		return szName;
	}

	LPCSTR InternalGetColor(int iPartId, int iStateId)
	{
		//ASSERT(_spCustomConfig);

		LPCSTR szName = NULL;
		BOOL fUseDefault = TRUE;
		if( _spCustomConfig)
		{
			fUseDefault = FALSE;
			szName = _spCustomConfig->GetColor(iPartId, iStateId, &fUseDefault);
		}
		if( !szName && fUseDefault )
		{
			CComPtr<ISkinConfig> sp;
			sp.Attach(GetDefaultConfig());
			if(sp)			
				szName = sp->GetColor(iPartId, iStateId, NULL);
		}

		return szName;
	}
protected:
	CComPtr<ISkinConfig> _spCustomConfig;
#endif // #if 0
	std::string _cofig_name;
};


// ver 2
class ATL_NO_VTABLE DefaultDraw : public CComObjectRoot
	, public ISkinDraw
{
public:
	typedef DefaultDraw thisType;
	BEGIN_COM_MAP(thisType)
		COM_INTERFACE_ENTRY_IID(IID_ISkinDraw, ISkinDraw)
	END_COM_MAP()

	STDMETHOD(DrawBackground)(HDC hdc, int iClassId, int iPartId, int iStateId, const RECT *pRect, 
		const RECT *pClipRect)
	{
		BOOL f = E_POINTER;
		if (_spScheme)
			f = _spScheme->Draw( hdc, iClassId, iPartId, iStateId, pRect->left, pRect->top,
					pRect->right - pRect->left, pRect->bottom - pRect->top );
		return f;
	}

	STDMETHOD(DrawParentBackground)(HWND hwnd, HDC hdc, RECT *prc)
	{
		CWindow wndparent( GetParent(hwnd) );
		CWindow wndchild(hwnd);

		// 直接使用纯色填充
		HBRUSH br = (HBRUSH)wndparent.SendMessage(WM_CTLCOLORDLG, (WPARAM)hdc, (LPARAM)hwnd);
		if (br)
			FillRect(hdc, prc, br);

		// TODO: 检测 WM_PRINTCLIENT 返回值，确定 Dialog 是否有底图
#if 0
		CRect rcparent;
		wndparent.GetClientRect(&rcparent);

		// memory dc
		HDC dcMem = ::CreateCompatibleDC(hdc);
		ASSERT( dcMem );
		HBITMAP bmpMemBg = ::CreateCompatibleBitmap(hdc, rcparent.Width(), rcparent.Height());
		ASSERT( bmpMemBg );
		HGDIOBJ pOldBmp = ::SelectObject(dcMem, bmpMemBg);
		ASSERT( pOldBmp );

		// 1绘制再memdc 上面
		LRESULT lRes = wndparent.SendMessage(WM_PRINTCLIENT, (WPARAM)dcMem, PRF_CLIENT | PRF_ERASEBKGND | PRF_CHECKVISIBLE);

		// memory dc
		//::BitBlt(hdc, prc->left, prc->top, prc->right - prc->left, prc->bottom - prc->top, dcMem, pt.x, pt.y, SRCCOPY);
		::SelectObject(dcMem, pOldBmp);
		::DeleteObject(bmpMemBg);
		::DeleteDC(dcMem);
#endif
		return S_OK;
	}

	STDMETHOD_(BOOL, IsThemeBackgroundPartiallyTransparent)(int iClassId, int iPartId, int iStateId)
	{
		if (iClassId == SKINCTL_BUTTON )
			return TRUE;
		return FALSE;
	}

	// dwTextFlags, dwTextFlags2 的定义见 MSDN: DrawThemeText
	STDMETHOD(DrawText)(HDC hdc, int iClassId, int iPartId, int iStateId, LPCSTR szText, DWORD dwTextFlags,
		DWORD dwTextFlags2, const RECT *pRect)
	{
		// TODO: logfont
		
		COLORREF cr;
		if (pRect && _spScheme && _spScheme->GetColor(iClassId, iPartId, iStateId, TMT_TEXTCOLOR, &cr))
		{
			RECT rc = *pRect;

			int mode = SetBkMode(hdc, TRANSPARENT);
			::DrawText(hdc, szText, -1, &rc, dwTextFlags);
			SetBkMode(hdc, mode);
		}
		return S_OK;
	}

	STDMETHOD(DrawIcon)(HDC hdc, int iClassId, int iPartId, int iStateId, const RECT *pRect, HIMAGELIST himl, int iImageIndex)
	{
		return S_OK;
	}

	// uEdge, uFlags, pContentRect 现在可以不使用
	STDMETHOD(DrawEdge)(HDC hdc, int iClassId, int iPartId, int iStateId, 
		const RECT *pDestRect, UINT uEdge, UINT uFlags, OPTIONAL OUT RECT *pContentRect)
	{
		return S_OK;
	}

	STDMETHOD(SetScheme)(ISkinScheme *pScheme)
	{
		_spScheme = pScheme;
		return S_OK;
	}
private:
	CComPtr<ISkinScheme> _spScheme;
};


};// namespace Skin



#endif // !defined(AFX_SKINDRAW_H__ECE45FB1_7DEB_4E14_AC98_D31304CE4B69__INCLUDED_)
