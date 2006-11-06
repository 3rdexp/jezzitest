#pragma once

#include <tmschema.h>
#include <atlmisc.h>

#include "../base/SkinDraw.h"

namespace Skin{

class ButtonDraw;

#if 0
class ATL_NO_VTABLE ButtonConfig : public CComTearOffObjectBase<ButtonDraw>
	, public ISkinConfig  
{
public:
	typedef ButtonConfig thisType;
	BEGIN_COM_MAP(thisType)
		COM_INTERFACE_ENTRY_IID(IID_ISkinConfig, ISkinConfig)
	END_COM_MAP()

	STDMETHOD_(LPCSTR, GetName)(int iPartId, int iStateId, BOOL* fUseDefault)
	{
#if 1
		SKINCONFIG_BEGIN_PART(BP_PUSHBUTTON)
			SKINCONFIG_ITEM(PBS_DEFAULTED, "button_default")
			SKINCONFIG_ITEM(PBS_DISABLED, "button_disable")
			SKINCONFIG_ITEM(PBS_HOT, "button_focus")
			SKINCONFIG_ITEM(PBS_NORMAL, "button_normal")
			SKINCONFIG_ITEM(PBS_PRESSED, "button_down")
			SKINCONFIG_USE_DEFAULT(fUseDefault)
		SKINCONFIG_END_PART()

		SKINCONFIG_BEGIN_PART(BP_CHECKBOX)
			SKINCONFIG_ITEM(CBS_CHECKEDDISABLED, "checkbox_checked_disable")
			SKINCONFIG_ITEM(CBS_CHECKEDHOT, "checkbox_checked")
			SKINCONFIG_ITEM(CBS_CHECKEDNORMAL, "checkbox_checked")
			SKINCONFIG_ITEM(CBS_CHECKEDPRESSED, "checkbox_checked_down")

			SKINCONFIG_ITEM(CBS_UNCHECKEDDISABLED, "checkbox_disable")
			SKINCONFIG_ITEM(CBS_UNCHECKEDHOT, "checkbox_notchecked")
			SKINCONFIG_ITEM(CBS_UNCHECKEDNORMAL, "checkbox_notchecked")
			SKINCONFIG_ITEM(CBS_UNCHECKEDPRESSED, "checkbox_notchecked_down")
			SKINCONFIG_USE_DEFAULT(fUseDefault)
		SKINCONFIG_END_PART()

		SKINCONFIG_BEGIN_PART(BP_RADIOBUTTON)
			SKINCONFIG_ITEM(RBS_CHECKEDDISABLED, "radio_selected_disable")
			SKINCONFIG_ITEM(RBS_CHECKEDHOT, "radio_selected")
			SKINCONFIG_ITEM(RBS_CHECKEDNORMAL, "radio_selected")
			SKINCONFIG_ITEM(RBS_CHECKEDPRESSED, "radio_selected_down")

			SKINCONFIG_ITEM(RBS_UNCHECKEDDISABLED, "radio_disable")
			SKINCONFIG_ITEM(RBS_UNCHECKEDHOT, "radio_normal")
			SKINCONFIG_ITEM(RBS_UNCHECKEDNORMAL, "radio_normal")
			SKINCONFIG_ITEM(RBS_UNCHECKEDPRESSED , "radio_normal_down")
			SKINCONFIG_USE_DEFAULT(fUseDefault)
		SKINCONFIG_END_PART()
#else
		if(xPart == iPartId)
		{
			if(xState == iStateId)
				return xName;

			*fUseDefault = FALSE;
		}
#endif
		return 0;
	}

	// 实现时本函数可以不实现的，不支持增长
	STDMETHOD(AddItem)(int iPartId, int iStateId, LPCSTR szItemName)
	{
		return E_NOTIMPL;
	}

	STDMETHOD_(LPCSTR, GetColor)(int iPartId, int iStateId, BOOL* fUseDefault)
	{
#if 1
		SKINCONFIG_BEGIN_PART(BP_PUSHBUTTON)
			SKINCONFIG_ITEM(PBS_DEFAULTED, "button_text_normal")
			SKINCONFIG_ITEM(PBS_DISABLED, "button_text_disable")
			SKINCONFIG_ITEM(PBS_HOT, "button_text_focus")
			SKINCONFIG_ITEM(PBS_NORMAL, "button_text_normal")
			SKINCONFIG_ITEM(PBS_PRESSED, "button_text_normal")
			SKINCONFIG_USE_DEFAULT(fUseDefault)
		SKINCONFIG_END_PART()

		SKINCONFIG_BEGIN_PART(BP_CHECKBOX)
			SKINCONFIG_ITEM(CBS_CHECKEDDISABLED, "button_text_disable")
			SKINCONFIG_ITEM(CBS_CHECKEDHOT, "button_text_focus")
			SKINCONFIG_ITEM(CBS_CHECKEDNORMAL, "button_text_normal")
			SKINCONFIG_ITEM(CBS_CHECKEDPRESSED, "button_text_normal")

			SKINCONFIG_ITEM(CBS_UNCHECKEDDISABLED, "button_text_disable")
			SKINCONFIG_ITEM(CBS_UNCHECKEDHOT, "button_text_focus")
			SKINCONFIG_ITEM(CBS_UNCHECKEDNORMAL, "button_text_normal")
			SKINCONFIG_ITEM(CBS_UNCHECKEDPRESSED, "button_text_normal")
			SKINCONFIG_USE_DEFAULT(fUseDefault)
		SKINCONFIG_END_PART()

		SKINCONFIG_BEGIN_PART(BP_RADIOBUTTON)
			SKINCONFIG_ITEM(RBS_CHECKEDDISABLED, "button_text_disable")
			SKINCONFIG_ITEM(RBS_CHECKEDHOT, "button_text_focus")
			SKINCONFIG_ITEM(RBS_CHECKEDNORMAL, "button_text_normal")
			SKINCONFIG_ITEM(RBS_CHECKEDPRESSED, "button_text_normal")

			SKINCONFIG_ITEM(RBS_UNCHECKEDDISABLED, "button_text_disable")
			SKINCONFIG_ITEM(RBS_UNCHECKEDHOT, "button_text_focus")
			SKINCONFIG_ITEM(RBS_UNCHECKEDNORMAL, "button_text_normal")
			SKINCONFIG_ITEM(RBS_UNCHECKEDPRESSED , "button_text_normal")
			SKINCONFIG_USE_DEFAULT(fUseDefault)
		SKINCONFIG_END_PART()
#else
		if(xPart == iPartId)
		{
			if(xState == iStateId)
				return xName;

			*fUseDefault = FALSE;
		}
#endif
		return 0;
	}

	// 实现时本函数可以不实现的，不支持增长
	STDMETHOD(AddColor)(int iPartId, int iStateId, LPCSTR szItemName)
	{
		return E_NOTIMPL;
	}
};
#endif // #if 0

class ATL_NO_VTABLE ButtonDraw : public CComObjectRoot
	, public SkinDrawImpl<ButtonDraw, SKINCTL_BUTTON>
{
public:
	typedef ButtonDraw thisType;
	BEGIN_COM_MAP(thisType)
		COM_INTERFACE_ENTRY_IID(IID_ISkinDraw, ISkinDraw)
	END_COM_MAP()

	DECLARE_GET_CONTROLLING_UNKNOWN()

	DECLARE_PROTECT_FINAL_CONSTRUCT()
	HRESULT FinalConstruct()
	{
		return S_OK;
	}
	void FinalRelease()
	{
	}

	STDMETHOD_(HBRUSH, GetColorBrush)(int iStateId)
	{
		return 0;
	}

	STDMETHOD_(BOOL, IsThemeBackgroundPartiallyTransparent)(int iPartId, int iStateId)
	{
		// TODO: 可以从配置文件中得到该值。
		return TRUE;
	}

	STDMETHOD(DrawBackground)(HDC hdc, int iPartId, int iStateId, const RECT *pRect, 
		const RECT *pClipRect)
	{
		if( iPartId == BP_PUSHBUTTON )
			return DrawPushButtonBackground(hdc, iPartId, iStateId, pRect, pClipRect);
		else if( iPartId == BP_CHECKBOX )
			return DrawCheckBackground(hdc, iPartId, iStateId, pRect, pClipRect);
		else if( iPartId == BP_RADIOBUTTON )	
			return DrawCheckBackground(hdc, iPartId, iStateId, pRect, pClipRect);

		return S_FALSE;
	}
	STDMETHOD(DrawText)(HDC hdc, int iPartId, int iStateId, LPCSTR szText, DWORD dwTextFlags,
		DWORD dwTextFlags2, const RECT *pRect)
	{
		if( iPartId == BP_PUSHBUTTON )
			return DrawPushButtonText(hdc, iPartId, iStateId, szText, dwTextFlags, dwTextFlags2, pRect);
		else if( iPartId == BP_CHECKBOX )
			return DrawCheckText(hdc, iPartId, iStateId, szText, dwTextFlags, dwTextFlags2, pRect);
		else if( iPartId == BP_RADIOBUTTON )
			return DrawCheckText(hdc, iPartId, iStateId, szText, dwTextFlags, dwTextFlags2, pRect);

		return S_FALSE;
	}
	
	STDMETHOD(DrawParentBackground)(HWND hwnd, HDC hdc, RECT *prc)
	{
		// TODO: SetClip use prc
		CWindow wndparent( GetParent(hwnd) );
		CWindow wndchild(hwnd);
#if 0
		if( wndparent.GetStyle() & WS_CLIPCHILDREN )
		{
			LRESULT lRes = wndparent.SendMessage(WM_CTLCOLORDLG, (WPARAM)(HDC)hdc, (LPARAM)wndparent.m_hWnd);
			if( lRes )
				FillRect(hdc, prc, (HBRUSH)lRes);
		}
		else
		{
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
			wndparent.SendMessage(WM_PRINTCLIENT, (WPARAM)dcMem, PRF_CLIENT | PRF_ERASEBKGND | PRF_CHECKVISIBLE);

			HDC h = GetDC(0);
			BitBlt(h, 10, 10, rcparent.Width(), rcparent.Height(), dcMem, 0, 0, SRCCOPY);
			ReleaseDC(0, h);
			
			// 2把memdc上的绘制到hdc上面
			POINT pt;
			pt.x = prc->left;
			pt.y = prc->top;			
			ClientToScreen(hwnd, &pt);
			wndparent.ScreenToClient(&pt);		
			
			// memory dc
			::BitBlt(hdc, prc->left, prc->top, prc->right - prc->left, prc->bottom - prc->top, dcMem, pt.x, pt.y, SRCCOPY);
			::SelectObject(dcMem, pOldBmp);
			::DeleteObject(bmpMemBg);
			::DeleteDC(dcMem);

		}
#endif
		// 直接使用纯色填充
		HBRUSH br = (HBRUSH)wndparent.SendMessage(WM_CTLCOLORDLG, (WPARAM)hdc, (LPARAM)hwnd);
		if (br)
			FillRect(hdc, prc, br);

		// TODO: 检测 WM_PRINTCLIENT 返回值，确定 Dialog 是否有底图
#if 1
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

	STDMETHOD(DrawPushButtonBackground)(HDC hdc, int iPartId, int iStateId, const RECT *pRect, 
		const RECT *pClipRect)
	{
#if 0
		CComPtr<ISkinMgr> spmgr;
		HRESULT hr = GetSkinMgr(&spmgr);
		if( FAILED(hr) ) return hr;

		ASSERT(spmgr);

		CComPtr<ISkinScheme> spss;
		hr = spmgr->GetCurentScheme(&spss);
		if( FAILED(hr) ) return hr;

		// TODO: 分多段绘制
		LPCSTR szName = InternalGetName(iPartId, iStateId);
		CRect rc;
		spss->GetRect(szName, &rc);

		CRect rect(*pRect);

		int nMode = SetStretchBltMode(hdc, HALFTONE );
		// 完全 BitBlt, 原大小
		if (rc.Width() == rect.Width() && rc.Height() == rect.Height())
		{
			spss->TransparentDraw(hdc, szName, rect.left, rect.top);		
		}
		else if (rc.Height() == rect.Height())// 高度一致时，分3次绘制
		{
			int step = rc.Width() / 3;
			spss->TransparentDraw(hdc, szName, rect.left, rect.top, step, rc.Height(), 0, 0, step, rc.Height());
			spss->Draw(hdc, szName, step, rect.top, rect.right - 2 * step, rc.Height(), step, 0, step, rc.Height());
			spss->TransparentDraw(hdc, szName, rect.right - step, rect.top, step, rc.Height(), rc.Width() - step, 0, step, rc.Height());
		}
		else // 分 9 段绘制, 高度自适应
		{
			const int step = rc.Height() / 3;
			// 左上角
			spss->TransparentDraw(hdc, szName, rect.left, rect.top, step, step, 0, 0, step, step);		
			// 上部中间
			spss->Draw(hdc, szName, rect.left + step, rect.top, rect.Width() - 2 * step, step, step, 0, rc.Width() - 2 * step, step);				
			// 上部右边
			spss->TransparentDraw(hdc, szName, rect.right - step, rect.top, step, step, rc.Width() - step, 0, step, step);						

			// 中间部分 左边
			spss->Draw(hdc, szName, rect.left, rect.top + step, step, rect.Height() - 2 * step, 0, step, step, rc.Height() - 2 * step);								

			// 中间部分 中部
			spss->Draw(hdc, szName, rect.left + step, rect.top + step, rect.Width() - 2 * step, rect.Height() - 2 * step, step, step, step, step);								

			// 中间部分 右边
			spss->Draw(hdc, szName, rect.right - step  , rect.top + step, step, rect.Height() - 2 * step, rc.Width() - step, step, step, step);										

			// 下面部分 左边
			spss->TransparentDraw(hdc, szName, rect.left, rect.bottom - step, step, step, 0, rc.Height() - step, step, step);

			// 下面部分 中间
			spss->Draw(hdc, szName, step, rect.bottom - step, rect.right - 2 * step, step, step, rc.Height() - step, rc.Width() - 2 * step, step);														

			// 下面部分 右边
			spss->TransparentDraw(hdc, szName, rect.right - step , rect.bottom - step, step, step, rc.Width() - step, rc.Height() - step, step, step);

		}
		SetStretchBltMode(hdc, nMode );
		// TODO: Region 策略
#endif
		// TODO: 应该可以支持透明/办透明的背景
		return S_OK;
	}

	STDMETHOD(DrawCheckBackground)(HDC hdc, int iPartId, int iStateId, const RECT *pRect, 
		const RECT *pClipRect)
	{
#if 0
		CComPtr<ISkinMgr> spmgr;
		HRESULT hr = GetSkinMgr(&spmgr);
		if( FAILED(hr) ) return hr;

		ASSERT(spmgr);

		CComPtr<ISkinScheme> spss;
		hr = spmgr->GetCurentScheme(&spss);
		if( FAILED(hr) ) return hr;

		// TODO: 分多段绘制
		LPCSTR szName = InternalGetName(iPartId, iStateId);
		CRect rc;
		spss->GetRect(szName, &rc);

		CRect rect(*pRect);
		RECT rt = rect;
		rt.top = (rect.Height() - rc.Width()) / 2;
		rt.right = rt.left + rc.Width();
		rt.bottom = rt.top + rc.Height();

		int nMode = SetStretchBltMode(hdc, HALFTONE );

		spss->TransparentDraw(hdc, szName, rt.left, rt.top, rt.right - rt.left, rt.bottom - rt.top, 0, 0, rc.Width(), rc.Height());

		SetStretchBltMode(hdc, nMode );
#endif
		return S_OK;
	}

	STDMETHOD(DrawPushButtonText)(HDC hdc, int iPartId, int iStateId, LPCSTR szText, DWORD dwTextFlags,
		DWORD dwTextFlags2, const RECT *pRect)
	{
		CComPtr<ISkinScheme> spss = GetCurrentScheme();
		RECT rc = *pRect;
		::DrawText(hdc, szText, -1, &rc, dwTextFlags);
#if 0
		CComPtr<ISkinMgr> spmgr;
		HRESULT hr = GetSkinMgr(&spmgr);
		if( FAILED(hr) ) return hr;

		ASSERT(spmgr);

		CComPtr<ISkinScheme> spss;
		hr = spmgr->GetCurentScheme(&spss);
		if( FAILED(hr) ) return hr;

		SetBkMode(hdc, TRANSPARENT);
		SetTextColor(hdc, spss->GetColor(InternalGetColor(iPartId, iStateId)));		
		HGDIOBJ hOldFont = (HFONT)SelectObject(hdc,(HFONT)GetStockObject(DEFAULT_GUI_FONT));		
		RECT rc(*pRect);
		if (iStateId == PBS_PRESSED)
		{
			rc.left += 2;			
			rc.top += 2;			
		}
		::DrawText(hdc, szText, -1, &rc, dwTextFlags);
		SelectObject(hdc,hOldFont);		
#endif
		return S_OK;
	}

	STDMETHOD(DrawCheckText)(HDC hdc, int iPartId, int iStateId, LPCSTR szText, DWORD dwTextFlags,
		DWORD dwTextFlags2, const RECT *pRect)
	{
#if 0
		CComPtr<ISkinMgr> spmgr;
		HRESULT hr = GetSkinMgr(&spmgr);
		if( FAILED(hr) ) return hr;

		ASSERT(spmgr);

		CComPtr<ISkinScheme> spss;
		hr = spmgr->GetCurentScheme(&spss);
		if( FAILED(hr) ) return hr;

		SetBkMode(hdc, TRANSPARENT);
		SetTextColor(hdc, spss->GetColor(InternalGetColor(iPartId, iStateId)));

		HGDIOBJ hOldFont = (HFONT)SelectObject(hdc,(HFONT)GetStockObject(DEFAULT_GUI_FONT));		

		LPCSTR szName = InternalGetName(iPartId, iStateId);
		CRect rcImage;
		spss->GetRect(szName, &rcImage);

		RECT rect(*pRect);
		rect.left += rcImage.Width() + 4;
		rect.top += 1;
		::DrawText(hdc, szText, -1, &rect, DT_LEFT | DT_VCENTER | DT_SINGLELINE);//dwTextFlags);
		SelectObject(hdc,hOldFont);		
#endif
		return S_OK;
	}

	STDMETHOD(DrawIcon)(HDC, int iPartId, int iStateId, const RECT *pRect)
	{
		return S_OK;
	}

	STDMETHOD(DrawEdge)(HDC, int iPartId, int iStateId, const RECT *pDestRect, 
		UINT uEdge, UINT uFlags, OPTIONAL OUT RECT *pContentRect)
	{
		return S_OK;
	}
};

}; // namespace Skin