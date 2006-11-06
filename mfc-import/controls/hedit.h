#pragma once

#include "skindraw.h"

namespace SkinDefault
{

	class EditDraw;

	class ATL_NO_VTABLE EditConfig : public CComTearOffObjectBase<EditDraw>
		, public ISkinConfig  
	{
	public:
		typedef EditConfig thisType;
		BEGIN_COM_MAP(thisType)
			COM_INTERFACE_ENTRY_IID(IID_ISkinConfig, ISkinConfig)
		END_COM_MAP()

		STDMETHOD_(LPCSTR, GetName)(int iPartId, int iStateId, BOOL* fUseDefault)
		{
			SKINCONFIG_BEGIN_PART(EP_EDITTEXT)
				SKINCONFIG_USE_DEFAULT(fUseDefault)
			SKINCONFIG_END_PART()

			SKINCONFIG_BEGIN_PART(EP_EDITBKGND)
				SKINCONFIG_USE_DEFAULT(fUseDefault)
			SKINCONFIG_END_PART()

			SKINCONFIG_BEGIN_PART(EP_EDITOUTBORDER)
				SKINCONFIG_USE_DEFAULT(fUseDefault)
			SKINCONFIG_END_PART()

			SKINCONFIG_BEGIN_PART(EP_EDITINBORDER)
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

			SKINCONFIG_BEGIN_PART(EP_EDITTEXT)
				SKINCONFIG_ITEM(ETS_NORMAL, "edit_text")
				SKINCONFIG_ITEM(ETS_READONLY, "edit_readonly")
				SKINCONFIG_ITEM(ETS_ASSIST, "edit_text")
				SKINCONFIG_ITEM(ETS_DISABLED, "edit_readonly")
				SKINCONFIG_ITEM(ETS_FOCUSED, "edit_text")
				SKINCONFIG_ITEM(ETS_HOT, "edit_text")
				SKINCONFIG_ITEM(ETS_SELECTED, "edit_text")
				SKINCONFIG_USE_DEFAULT(fUseDefault)
			SKINCONFIG_END_PART()

			SKINCONFIG_BEGIN_PART(EP_EDITBKGND)
				SKINCONFIG_ITEM(ETS_NORMAL, "edit_back")
				SKINCONFIG_ITEM(ETS_READONLY, "edit_back")
				SKINCONFIG_ITEM(ETS_ASSIST, "edit_back")
				SKINCONFIG_ITEM(ETS_DISABLED, "edit_back")
				SKINCONFIG_ITEM(ETS_FOCUSED, "edit_back")
				SKINCONFIG_ITEM(ETS_HOT, "edit_back")
				SKINCONFIG_ITEM(ETS_SELECTED, "edit_back")
				SKINCONFIG_USE_DEFAULT(fUseDefault)
			SKINCONFIG_END_PART()

			SKINCONFIG_BEGIN_PART(EP_EDITOUTBORDER)
				SKINCONFIG_ITEM(ETS_NORMAL, "edit_border")
				SKINCONFIG_ITEM(ETS_READONLY, "edit_border")
				SKINCONFIG_ITEM(ETS_ASSIST, "edit_border")
				SKINCONFIG_ITEM(ETS_DISABLED, "edit_border")
				SKINCONFIG_ITEM(ETS_FOCUSED, "edit_border")
				SKINCONFIG_ITEM(ETS_HOT, "edit_border")
				SKINCONFIG_ITEM(ETS_SELECTED, "edit_border")
				SKINCONFIG_USE_DEFAULT(fUseDefault)
			SKINCONFIG_END_PART()

			SKINCONFIG_BEGIN_PART(EP_EDITINBORDER)
				SKINCONFIG_ITEM(ETS_NORMAL, "edit_inside_border")
				SKINCONFIG_ITEM(ETS_READONLY, "edit_inside_border")
				SKINCONFIG_ITEM(ETS_ASSIST, "edit_inside_border")
				SKINCONFIG_ITEM(ETS_DISABLED, "edit_inside_border")
				SKINCONFIG_ITEM(ETS_FOCUSED, "edit_inside_border")
				SKINCONFIG_ITEM(ETS_HOT, "edit_inside_border")
				SKINCONFIG_ITEM(ETS_SELECTED, "edit_inside_border")
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

	class ATL_NO_VTABLE EditDraw : public CComObjectRoot
		, public SkinDrawImpl<EditDraw, SKINCTL_EDIT>
	{
	public:
		typedef EditDraw thisType;
		BEGIN_COM_MAP(thisType)
			COM_INTERFACE_ENTRY_IID(IID_ISkinDraw, ISkinDraw)
			COM_INTERFACE_ENTRY_CACHED_TEAR_OFF(IID_ISkinConfig, EditConfig, _spCustomConfig.p)
		END_COM_MAP()

		DECLARE_GET_CONTROLLING_UNKNOWN()

		DECLARE_PROTECT_FINAL_CONSTRUCT()
		HRESULT FinalConstruct()
		{
			return S_OK;
		}
		void FinalRelease()
		{
			_brBkgnd.Detach();
		}

		STDMETHOD_(HBRUSH, GetColorBrush)(int iColorID)
		{
			if (_brBkgnd.GetSafeHandle())
			{
				return (HBRUSH)_brBkgnd;
			}
			else
			{
				// create 
				CComPtr<ISkinMgr> spmgr;
				HRESULT hr = GetSkinMgr(&spmgr);
				if( FAILED(hr) ) return 0;

				ASSERT(spmgr);

				CComPtr<ISkinScheme> spss;
				hr = spmgr->GetCurentScheme(&spss);
				if( FAILED(hr) ) return 0;

				COLORREF crBack = spss->ExtractColor(InternalGetColor(EP_EDITBKGND, ETS_NORMAL));
				_brBkgnd.CreateSolidBrush( crBack );
				return (HBRUSH)_brBkgnd;
			}
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
		CBrush _brBkgnd;
	};

}; // namespace 