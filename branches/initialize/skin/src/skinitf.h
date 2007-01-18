// SkinItf.h : Skin SDK Interface
//

#if !defined(_SKINITF_H_)
#define _SKINITF_H_

#pragma once

#include <comdef.h>


#ifndef SKIN_NO_NAMESPACE
namespace Skin{
#endif

// TODO: ����һ������
#define DLLEXPORT   __declspec(dllexport)


#ifndef ASSERT
  #define ASSERT ATLASSERT
#endif

#ifndef NM_COOLSB_CUSTOMDRAW
	#define NM_COOLSB_CUSTOMDRAW (0-0xfffU)
#endif
// ------------------------------------------------------
// �ؼ�����
// Ϊʲô�� skincontrol_class_id������ skincontrol_flag��
// skincontrol_class_id ֱ�������������������ļ��У��ͳ���ִ����
// skincontrol_flag ������ SkinMgr::InitControls �У�����һ�����������control

// TODO: ɾ����Щ�����ĺֱ꣬��д��������

#ifndef CCI_DEF
#define CCI_DEF(val, name) name = val,
#endif

enum skincontrol_class_id
{
    CCI_DEF( 1, BUTTON)
    CCI_DEF( 2, REBAR)
    CCI_DEF( 3, TOOLBAR)
    CCI_DEF( 4, STATUS)
    CCI_DEF( 5, MENU)
    CCI_DEF( 6, LISTVIEW)
    CCI_DEF( 7, HEADER)
    CCI_DEF( 8, PROGRESS)
    CCI_DEF( 9, TAB)
    CCI_DEF(10, TRACKBAR)
    CCI_DEF(11, TOOLTIP)
    CCI_DEF(12, TREEVIEW)
    CCI_DEF(13, SPIN)
    CCI_DEF(14, PAGE)
    CCI_DEF(15, SCROLLBAR)
    CCI_DEF(16, EDIT)
    CCI_DEF(17, COMBOBOX)

    CCI_DEF(18, DIALOG)

    // Non-client ����
    CCI_DEF(19, WINDOW)
	CCI_DEF(20, CONTROLBAR)
	CCI_DEF(21, LISTBOX)
	CCI_DEF(22, IPADDRESS)
	CCI_DEF(23, DATATIMEPICKER)
	CCI_DEF(24, MONTHCALCTL)
};

#undef CCI_DEF

#ifndef SKIN_TYPE
#define SKIN_TYPE(name) SKINCTL_##name =  1 << name,
#endif

enum skincontrol_flag
{
    SKINCTL_NONE = 0,

    SKIN_TYPE( BUTTON)
    SKIN_TYPE( REBAR)
    SKIN_TYPE( TOOLBAR)
    SKIN_TYPE( STATUS)
    SKIN_TYPE( MENU)
    SKIN_TYPE( LISTVIEW)
    SKIN_TYPE( HEADER)
    SKIN_TYPE( PROGRESS)
    SKIN_TYPE( TAB)
    SKIN_TYPE( TRACKBAR)
    SKIN_TYPE( TOOLTIP)
    SKIN_TYPE( TREEVIEW)
    SKIN_TYPE( SPIN)
    SKIN_TYPE( PAGE)
    SKIN_TYPE( SCROLLBAR)
    SKIN_TYPE( EDIT)
    SKIN_TYPE( COMBOBOX)

	SKIN_TYPE( IPADDRESS)
	SKIN_TYPE( DATATIMEPICKER)
	SKIN_TYPE( MONTHCALCTL )

    SKIN_TYPE( DIALOG)

    // Non-client ����
    SKIN_TYPE( WINDOW)
	SKIN_TYPE( LISTBOX )

    SKINCTL_SYSDEFAULT   = 0x000FFFFF,
    SKINCTL_EXTENSION    = 0xFFF00000,

    SKINCTL_ALL      = 0xFFFFFFFF,
};

#undef SKIN_TYPE

// ------------------------------------------------------
// dwType: SKINCTL_XXXX,�ؼ�����
// ����Dll�Զ������еĿؼ�Ӧ��Skin���û�Ҳ���Կ���
BOOL WINAPI SkinInitControls(DWORD dwType, BOOL fInstall);

//���÷Ǳ�׼��Dialog ����VB��
BOOL WINAPI SetDialogSkin(HWND hWnd, BOOL bSingle);

//����Frame
BOOL WINAPI SetFrameSkin(HWND hWnd, BOOL bSingle);

// call ISkinMgr::SetCurrentScheme
BOOL WINAPI SkinSetCurrentScheme(LPCTSTR pszFilePath);

BOOL WINAPI RemoveDialogSkin(HWND hWnd);

// ------------------------------------------------------
// Foward declare
struct ISkinMgr;
struct ISkinDraw;
struct ISkinScheme;

// ------------------------------------------------------
// 1 ���������Щ���͵Ŀؼ���ҪSkin
// 2 ����,����ȱʡ��ISkinScheme

#undef  INTERFACE
#define INTERFACE ISkinMgr
DECLARE_INTERFACE_(ISkinMgr, IUnknown)
{
	STDMETHOD(InitControls)(HINSTANCE hInst, DWORD dwType) PURE;
	STDMETHOD(UninitControls)(HINSTANCE hInst, DWORD dwType) PURE;

	STDMETHOD(LoadTheme)(LPCSTR file) PURE;
	// Scheme
	STDMETHOD(GetScheme)(LPCSTR style, ISkinScheme ** ppScheme) PURE;
	STDMETHOD(GetCurentScheme)(ISkinScheme ** ppScheme) PURE;
	STDMETHOD(SetCurrentScheme)(LPCSTR style) PURE;
};

HRESULT WINAPI GetSkinMgr(ISkinMgr ** pMgr);

// ------------------------------------------------------
#undef  INTERFACE
#define INTERFACE IDCCache
DECLARE_INTERFACE_(IDCCache, IUnknown)
{
	STDMETHOD(SetBitmap)(HBITMAP, COLORREF) PURE;
	STDMETHOD_(ULONG, Width)() PURE;
	STDMETHOD_(ULONG, Height)() PURE;
	STDMETHOD_(HDC, GetDC)() PURE;
	STDMETHOD_(COLORREF, TranslateColor)() PURE;
	STDMETHOD_(HBITMAP, GetBitmap)() PURE;
};

enum DrawParam
{
	DP_CONFIG_NAME = 1,
};

// ------------------------------------------------------
// ���ƶ���,�������ؼ��Ļ���
// SkinDefaultʹ��ISkinScheme����
// �û������Լ�ʵ��

#undef  INTERFACE
#define INTERFACE ISkinDraw
DECLARE_INTERFACE_(ISkinDraw, IUnknown)
{
	// �ڷ�MemoryDC�У�����pClipRect�ǳ������壬�ڱ�Skin��ʵ���п��Բ�ʹ��
	STDMETHOD(DrawBackground)(HDC, int iClassId, int iPartId, int iStateId, const RECT *pRect, 
		const RECT *pClipRect) PURE;

	STDMETHOD(DrawParentBackground)(HWND hwnd, HDC hdc, RECT *prc) PURE;

	STDMETHOD_(BOOL, IsThemeBackgroundPartiallyTransparent)(int iClassId, int iPartId, int iStateId) PURE;

	// dwTextFlags, dwTextFlags2 �Ķ���� MSDN: DrawThemeText
	STDMETHOD(DrawText)(HDC, int iClassId, int iPartId, int iStateId, LPCSTR szText, DWORD dwTextFlags,
		DWORD dwTextFlags2, const RECT *pRect) PURE;

	STDMETHOD(DrawIcon)(HDC, int iClassId, int iPartId, int iStateId, const RECT *pRect, HIMAGELIST himl, int iImageIndex) PURE;

	// uEdge, uFlags, pContentRect ���ڿ��Բ�ʹ��
	STDMETHOD(DrawEdge)(HDC, int iClassId, int iPartId, int iStateId, 
		const RECT *pDestRect, UINT uEdge, UINT uFlags, OPTIONAL OUT RECT *pContentRect) PURE;

	STDMETHOD(SetScheme)(ISkinScheme *pScheme) PURE;
};


namespace ToDelete {
// ------------------------------------------------------
// iPartId + iStateId => string

#undef  INTERFACE
#define INTERFACE ISkinConfig
DECLARE_INTERFACE_(ISkinConfig, IUnknown)
{
	// �����ṹ�� Part + State => ItemName => ISkinScheme Rect/Color

	// *fUseDefault: һ����ISkinDraw����
	// False: ��ʹ��Default�� ISkinConfig ʵ�֣����ܾͻ���ʧ�ܲ���
	// True: ʹ��Default�� ISkinConfig
	STDMETHOD_(LPCSTR, GetName)(int iPartId, int iStateId, BOOL* fUseDefault = NULL) PURE;
	
	STDMETHOD_(LPCSTR, GetColor)(int iPartId, int iStateId, BOOL* fUseDefault = NULL) PURE;
	// ʵ��ʱ���������Բ�ʵ�ֵģ���֧������
	STDMETHOD(AddItem)(int iPartId, int iStateId, LPCSTR szItemName) PURE;
	STDMETHOD(AddColor)(int iPartId, int iStateId, LPCSTR szItemName) PURE;
};



// ----------------------------------------------------
// ��������ʵ�� ISkinConfig �ĺ�
// ����˵���� SkinConfig.h ��ʵ��
#define SKINCONFIG_BEGIN_PART(xPart)   if(xPart == iPartId) {
#define SKINCONFIG_ITEM(xState, xName)    if(xState == iStateId) return xName;
#define SKINCONFIG_END_PART()            }

#define SKINCONFIG_USE_DEFAULT(x) if(x) *x = TRUE;
#define SKINCONFIG_NOT_USE_DEFAULT(x) if(x) *x = FALSE;


#if 0
namespace SkinDefaultImpl
{
	// App �ܷ�����Ϣ���Ƶ���Ԫ�ض�Ӧ�� ISkinScheme �е� Item
	// SendMessage( WMS_SETSTYLE, (PART<<16 | STATE), "xp_button_disable" );

	/*
		SkinButton WindowProc
			ISkinConfig* pcfg = new CSkinGrowConfig;
		    pcfg->AddItem

		// 
	*/
	// �ܹ��ɴ��롢��Ϣ���Ƶ�����
	class CSkinGrowConfig : public ISkinConfig
	{
	private:
		// map<(PART<<16 | STATE), "xp_button_disable">
	};

	class ATL_NO_VTABLE SkinButtonDraw : public ISkinDraw
	{
	public:
		STDMETHOD(DrawBackground)(HDC, int iPartId, int iStateId, const RECT *pRect, 
			const RECT *pClipRect)
		{
			// if iPartId != Button return E_UNEXPECTED
			ComPtr<ISkinScheme> spss; // = ISkinMgr::GetCurentScheme
			spss->Draw( InternalGetName(iPartId, iStateId), ...);
		}
	private:
		LPCSTR InternalGetName(int iPartId, int iStateId)
		{
			LPCSTR szName = NULL;
			BOOL fUseDefault = TRUE;
			if( _spCustomConfig && szName = _spCustomConfig->GetName(iPartId, iStateId, &fUseDefault) )
				return szName;
			if( !fUseDefault )
				return NULL;

			if( _spDefaultConfig )
				szName = _spDefaultConfig->GetName(iPartId, iStateId, &fUseDefault) )
			return szName;
		}
	private:
		CComPtr<ISkinConfig> _spDefaultConfig; // ISkinMgr::GetDefaultConfig
		CComPtr<ISkinConfig> _spCustomConfig;  // 
	};
};

namespace App
{
	// 1 App ʵ�� ISkinDraw �ı�ؼ��Ļ�����Ϊ
	class MyDraw : public ISkinDraw
	{
	};

	HWND hButton = GetDlgItem(ID_MYBUTTON_ID);
	SendMessage(hButton, WMS_SETDRAW, new MyDraw, 0);

	
	// 2 Change Scheme Part Name��Appʵ�ּ�ISkinConfig ***����***�ı���SkinScheme�е�������
	class ATL_NO_VTABLE MyConfig : public ISkinConfig
	{
	public:
		STDMETHOD_(LPCSTR, GetName)(int iPartId, int iStateId, BOOL* fUseDefault)
		{
			// CONFIG_ITEM(PartId, StateId, "name") ��ȼ�������Ĵ���
			// �����Ӧ���� SkinDefaultImpl ����
			// SkinDefaultImpl ��ҲӦ�ò��������ʽ��ʵ��
			if(iPartId == BUTTON)
			{
				if(iStateId = BUTTON_DISABLE)
					return "xp_button_disable";
			}

			BEGIN_PART(button) // if(
				CONFIG_ITEM(BUTTON_DISABLE, "xp_button_disable");
				CONFIG_ITEM(BUTTON_DISABLE, "xp_button_disable");
			END_PART()

			CONFIG_ITEM(BP_BUTTON, BUTTON_DISABLE, "xp_button_disable");
		}

		// �û��ܸ��Ӽ򵥵�ʵ��
		{
			// 
			CONFIG_PART_ITEM(BUTTON, BUTTON_DISABLE, "xp_button_disable");
			*fUseDefault = TRUE;
			return NULL;
		}
	};

#if 0	
	pfoo->SetItemName(iPartId, int iStateId, LPCSTR szName); // new_left config.xml

	SetItemNamePrefix

	// SetItemName(iPartId, int iStateId, LPCSTR szName)

	pfoo->SetStyle("xp_style");

	ISkinMgr::CreateDraw(PART=Button, pfoo) => ISkinDraw => Button;
	SendMessage()
#endif
};



namespace ControlImpl
{
	void Button_OnPaint()
	{
		// ��ס����������ʱ���ȵõõ�
		HFONT hFont = SendMessage��WM_GETFONT, m_hWnd, 0, 0 );
		SelectObject(MemoryDC, hFont);

		// TODO: DrawText ��ʵ��ʹ�� AtlGetDefaultFont ����Ҳ��һ���򵥰취
		ISkinDraw::DrawText(MemoryDC, ...);
	}

	/*
	private:
	ISkinDraw * pdraw = ISkinMgr::GetDrawControl
	*/
};
#endif
} // namespace ToDelete

//------------------------------------------------------------------------------

#undef  INTERFACE
#define INTERFACE ISkinScheme
DECLARE_INTERFACE_(ISkinScheme, IUnknown)
{
	STDMETHOD_(BOOL, Draw)(HDC hdc, int iClassId, int iPartId, int iStateId, 
        long dx, long dy, long dcx = 0, long dcy = 0, DWORD dwRop = SRCCOPY) PURE;
	STDMETHOD_(BOOL, Draw)(HDC hdc, int iClassId, int iPartId, int iStateId, 
        long dx, long dy, long dcx, long dcy, long sx, long sy, long scx, long scy, 
        DWORD dwRop = SRCCOPY) PURE;
	
	STDMETHOD_(BOOL, TransparentDraw)(HDC hdc, int iClassId, int iPartId, int iStateId, 
        long dx, long dy, long dcx = 0, long dcy = 0) PURE;
	STDMETHOD_(BOOL, TransparentDraw)(HDC hdc, int iClassId, int iPartId, int iStateId, 
        long dx, long dy, long dcx, long dcy,long sx, long sy, long scx, long scy) PURE;
	STDMETHOD_(BOOL, TransparentDraw)(HDC hdc, int iClassId, int iPartId, int iStateId, 
        const RECT* lprc) PURE;

    STDMETHOD(DrawParentBackground)(HWND hwnd, HDC hdc, RECT *prc) PURE;
    STDMETHOD_(BOOL, IsThemeBackgroundPartiallyTransparent)(int iClassId, int iPartId, 
        int iStateId) PURE;
    // �ڷ�MemoryDC�У�����pClipRect�ǳ������壬�ڱ�Skin��ʵ���п��Բ�ʹ��
    STDMETHOD(DrawBackground)(HDC, int iClassId, int iPartId, int iStateId, const RECT *pRect, 
        const RECT *pClipRect) PURE;

    STDMETHOD(DrawText)(HDC, int iClassId, int iPartId, int iStateId, LPCSTR szText, 
        DWORD dwTextFlags, DWORD dwTextFlags2, const RECT *pRect) PURE;

    STDMETHOD(DrawIcon)(HDC, int iClassId, int iPartId, int iStateId, const RECT *pRect, 
        HIMAGELIST himl, int iImageIndex) PURE;

    // uEdge, uFlags, pContentRect ���ڿ��Բ�ʹ��
    STDMETHOD(DrawEdge)(HDC, int iClassId, int iPartId, int iStateId, 
        const RECT *pDestRect, UINT uEdge, UINT uFlags, OPTIONAL OUT RECT *pContentRect) PURE;

	STDMETHOD_(BOOL, GetColor) (int iClassId, int iPartId, int iStateId, int iPropId, COLORREF * pColor) PURE;
	STDMETHOD_(BOOL, GetRect)  (int iClassId, int iPartId, int iStateId, RECT *pRect) PURE;
	STDMETHOD_(BOOL, GetFont)  (int iClassId, int iPartId, int iStateId, int iPropId, LOGFONT *pFont) PURE;
	
	STDMETHOD_(BOOL, GetBitmap)(int iClassId, int iPartId, int iStateId, int iPropId, HBITMAP *pBitmap) PURE;
	STDMETHOD_(BOOL, GetRegion)(int iClassId, int iPartId, int iStateId, int iPropId, HRGN *pRegion) PURE;

	STDMETHOD_(BOOL, ChangeSchemeColor)( COLORREF clr ) PURE;
	STDMETHOD_(BOOL, ClearSchemeColor)(  ) PURE;
};


HRESULT WINAPI CreatetDefaultSkinScheme(ISkinScheme** ppss);
DLLEXPORT HRESULT WINAPI GetCurrentScheme(ISkinScheme** ppScheme);
DLLEXPORT HRESULT WINAPI InstallSkinScrollBar(HWND hWnd);
DLLEXPORT HRESULT WINAPI HandleSkinScrollCustomDraw(int wParam, LPNMHDR lParam);
DLLEXPORT LRESULT WINAPI SkinFrameProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
DLLEXPORT LRESULT WINAPI SkinFramehWnd(HWND hWnd);
DLLEXPORT LRESULT WINAPI SkinMDIFramehWnd(HWND hWnd);
DLLEXPORT LRESULT WINAPI SkinSDIFramehWnd(HWND hWnd);

//DLLEXPORT HRESULT WINAPI ChangeCurrentSchemeColor(COLORREF clr);



/*
  
  DllMain ProcessAttach
  {
	1 ��ȡһ������(�󶨵Ŀؼ�����)��ȱʡ�����еĿؼ�ʵʩSkin��������ò���Ҫ�󶨲����κβ���
	2 ����Ψһʵ�� ISkinMgr
	3 ���1�İ�(SetClasLong)�����û�д��ڴ��������ܲ���Ҫ�����κε���Դ(config.xml, bitmap)
	4 
  }

*/


// -----------------------------------------------------------
//
HRGN WINAPI BitmapToRegion(HBITMAP hBmp, COLORREF cTransparentColor, COLORREF cTolerance, const LPRECT prcSrounce);
HRGN WINAPI RegulateRegion(HRGN hrgn, const SIZE* pszDest);
BOOL WINAPI TransparentBlt2(HDC hdcDest,int nXOriginDest,int nYOriginDest,int nWidthDest,int nHeightDest,
					   HDC hdcSrc,int nXOriginSrc,int nYOriginSrc,int nWidthSrc,int nHeightSrc,COLORREF clrTransparent);

//HRESULT WINAPI InstallScrollBar(HWND hWnd);



#ifndef SKIN_NO_NAMESPACE
}
#endif



// -----------------------------------------------------------
#endif // !defined(_SKINITF_H_)
