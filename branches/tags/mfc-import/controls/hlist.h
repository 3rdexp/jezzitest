#pragma once

#include "skindraw.h"

namespace SkinDefault
{

	class ListBoxDraw;

	class ATL_NO_VTABLE ListBoxConfig : public CComTearOffObjectBase<ListBoxDraw>
		, public ISkinConfig  
	{
	public:
		typedef ListBoxConfig thisType;
		BEGIN_COM_MAP(thisType)
			COM_INTERFACE_ENTRY_IID(IID_ISkinConfig, ISkinConfig)
		END_COM_MAP()

		STDMETHOD_(LPCSTR, GetName)(int iPartId, int iStateId, BOOL* fUseDefault)
		{
			SKINCONFIG_BEGIN_PART(LVP_LISTITEM)
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
			SKINCONFIG_BEGIN_PART(LVP_LISTITEMOUTBORDER)
				SKINCONFIG_ITEM(LIS_DISABLED, "edit_border")
				SKINCONFIG_ITEM(LIS_HOT, "edit_border")
				SKINCONFIG_ITEM(LIS_NORMAL, "edit_border")
				SKINCONFIG_ITEM(LIS_SELECTED, "edit_border")
				SKINCONFIG_ITEM(LIS_SELECTEDNOTFOCUS, "edit_border")
				SKINCONFIG_USE_DEFAULT(fUseDefault)
			SKINCONFIG_END_PART()

			SKINCONFIG_BEGIN_PART(LVP_LISTITEMINBORDER)
				SKINCONFIG_ITEM(LIS_DISABLED, "edit_inside_border")
				SKINCONFIG_ITEM(LIS_HOT, "edit_inside_border")
				SKINCONFIG_ITEM(LIS_NORMAL, "edit_inside_border")
				SKINCONFIG_ITEM(LIS_SELECTED, "edit_inside_border")
				SKINCONFIG_ITEM(LIS_SELECTEDNOTFOCUS, "edit_inside_border")				
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

	class ATL_NO_VTABLE ListBoxDraw : public CComObjectRoot
		, public SkinDrawImpl<ListBoxDraw, SKINCTL_LISTBOX>
	{
	public:
		typedef ListBoxDraw thisType;
		BEGIN_COM_MAP(thisType)
			COM_INTERFACE_ENTRY_IID(IID_ISkinDraw, ISkinDraw)
			COM_INTERFACE_ENTRY_CACHED_TEAR_OFF(IID_ISkinConfig, ListBoxConfig, _spCustomConfig.p)
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
			return S_FALSE;
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

	private:
	};

}; // namespace 