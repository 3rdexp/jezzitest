#pragma once

#include "skindraw.h"

namespace SkinDefault
{

	class ComboboxDraw;

	class ATL_NO_VTABLE ComboboxConfig : public CComTearOffObjectBase<ComboboxDraw>
		, public ISkinConfig  
	{
	public:
		typedef ComboboxConfig thisType;
		BEGIN_COM_MAP(thisType)
			COM_INTERFACE_ENTRY_IID(IID_ISkinConfig, ISkinConfig)
		END_COM_MAP()

		STDMETHOD_(LPCSTR, GetName)(int iPartId, int iStateId, BOOL* fUseDefault)
		{
			SKINCONFIG_BEGIN_PART(CP_DROPDOWNBUTTON)
				SKINCONFIG_ITEM(CBXS_DISABLED, "cmbox_dropbtn_disable")
				SKINCONFIG_ITEM(CBXS_HOT, "cmbox_dropbtn_normal")
				SKINCONFIG_ITEM(CBXS_NORMAL, "cmbox_dropbtn_normal")
				SKINCONFIG_ITEM(CBXS_PRESSED, "cmbox_dropbtn_down")				
				SKINCONFIG_USE_DEFAULT(fUseDefault)
			SKINCONFIG_END_PART()

			return 0;
		}

		// 实现时本函数可以不实现的，不支持增长
		STDMETHOD(AddItem)(int iPartId, int iStateId, LPCSTR szItemName)
		{
			return E_NOTIMPL;
		}

		STDMETHOD_(LPCSTR, GetColor)(int iPartId, int iStateId, BOOL* fUseDefault)
		{

			SKINCONFIG_BEGIN_PART(CP_COMBOBOXOUTBORDER)
				SKINCONFIG_ITEM(CBXS_DISABLED, "combobox_outside_border")
				SKINCONFIG_ITEM(CBXS_HOT, "combobox_outside_border")
				SKINCONFIG_ITEM(CBXS_NORMAL, "combobox_outside_border")
				SKINCONFIG_ITEM(CBXS_PRESSED, "combobox_outside_border")				
				SKINCONFIG_USE_DEFAULT(fUseDefault)
			SKINCONFIG_END_PART()

			SKINCONFIG_BEGIN_PART(CP_COMBOBOXINBORDER)
				SKINCONFIG_ITEM(CBXS_DISABLED, "combobox_inside_border")
				SKINCONFIG_ITEM(CBXS_HOT, "combobox_inside_border")
				SKINCONFIG_ITEM(CBXS_NORMAL, "combobox_inside_border")
				SKINCONFIG_ITEM(CBXS_PRESSED, "combobox_inside_border")				
				SKINCONFIG_USE_DEFAULT(fUseDefault)
			SKINCONFIG_END_PART()
			return 0;
		}

		// 实现时本函数可以不实现的，不支持增长
		STDMETHOD(AddColor)(int iPartId, int iStateId, LPCSTR szItemName)
		{
			return E_NOTIMPL;
		}
	};

	class ATL_NO_VTABLE ComboboxDraw : public CComObjectRoot
		, public SkinDrawImpl<ComboboxDraw, SKINCTL_COMBOBOX>
	{
	public:
		typedef ComboboxDraw thisType;
		BEGIN_COM_MAP(thisType)
			COM_INTERFACE_ENTRY_IID(IID_ISkinDraw, ISkinDraw)
			COM_INTERFACE_ENTRY_CACHED_TEAR_OFF(IID_ISkinConfig, ComboboxConfig, _spCustomConfig.p)
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

		STDMETHOD_(HBRUSH, GetColorBrush)(int iColorID)
		{
			return 0;
		}

		STDMETHOD(DrawBackground)(HDC hdc, int iPartId, int iStateId, const RECT *pRect, 
			const RECT *pClipRect)
		{
			//2003-07-11, remove gray border when button is down
			
			CDC dc;
			dc.Attach(hdc);
			
			CRect rcFill(*pRect);

			rcFill.left = rcFill.right - GetSystemMetrics(SM_CXHTHUMB) - 2;
			rcFill.right = rcFill.left + 2;

			FillRect(dc.m_hDC, &rcFill, (HBRUSH)GetStockObject(WHITE_BRUSH));
			//dc.FillSolidRect(&rcFill, RGB(255, 255, 255));
			
			

			CComPtr<ISkinMgr> spmgr;
			HRESULT hr = GetSkinMgr(&spmgr);
			if( FAILED(hr) ) return hr;

			ASSERT(spmgr);

			CComPtr<ISkinScheme> spss;
			hr = spmgr->GetCurentScheme(&spss);
			if( FAILED(hr) ) return hr;

			////////////////////////////////////////////////////////////////
			CRect rcItem(*pRect);
			// Cover up dark 3D shadow.
			LPCSTR szColor = InternalGetColor(CP_COMBOBOXOUTBORDER, iStateId);

			dc.Draw3dRect(rcItem, spss->ExtractColor(szColor), spss->ExtractColor(szColor));
			
			
			rcItem.DeflateRect(1,1);
			
			if (iStateId == CBXS_DISABLED) 
			{
				//2003-07-11
				dc.Draw3dRect(rcItem, ::GetSysColor(COLOR_BTNHIGHLIGHT),::GetSysColor(COLOR_BTNHIGHLIGHT));
			}
			else 
			{
				dc.Draw3dRect(rcItem, spss->ExtractColor(InternalGetColor(CP_COMBOBOXINBORDER, iStateId))
					, spss->ExtractColor(InternalGetColor(CP_COMBOBOXINBORDER, iStateId))); 
			}
			// Cover up dark 3D shadow on drop arrow.
			rcItem.DeflateRect(1,1);
			rcItem.left = rcItem.right - ::GetSystemMetrics(SM_CXHTHUMB);
			rcItem.DeflateRect(1,1);

			CRect rcClient(*pRect);			
			CRect rcButton(rcClient.right - GetSystemMetrics(SM_CXHTHUMB) , rcClient.top + 2, rcClient.right - 1, rcClient.bottom - 1);

			// fix icon combox draw
			FillRect(dc.m_hDC, &rcButton, (HBRUSH)GetStockObject(WHITE_BRUSH));
			rcButton.left += 1;
			
			LPCSTR szName = InternalGetName(iPartId, iStateId);
			CRect rcbmp;
			spss->ExtractRect(szName, &rcbmp);
			spss->Draw(dc, szName, rcButton.right - rcbmp.Width() - 1, rcButton.bottom - rcbmp.Height() -1);

			dc.Detach();

			return S_OK;
		}
		STDMETHOD(DrawText)(HDC hdc, int iPartId, int iStateId, LPCSTR szText, DWORD dwTextFlags,
			DWORD dwTextFlags2, const RECT *pRect)
		{
			return S_FALSE;
		}

		STDMETHOD(DrawParentBackground)(HWND hwnd, HDC hdc, RECT *prc)
		{
			// TODO: SetClip use prc	
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

}; // namespace 