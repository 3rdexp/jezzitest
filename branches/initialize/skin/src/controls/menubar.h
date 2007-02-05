
#pragma once

//////////////////////////////////////////////////////////////////////
// CXPMenuBar class version 2.12
//		simulates a Dev Studio style dockable menu bar.
//		based on PixieLib written by Paul DiLascia<www.dilascia.com>
//
// version history
//		2.12 : support OLE menu carelessly.
//		2.11 : WindowMenu fixed by VORGA.
//		2.10 : CMenuDockBar's problem fixed again and again.
//		2.08 : give up precise ComputeMenuTrackPoint
//		2.07 : Sychronizing with frame activation problem fixed
//		2.06 : CXPMenuItem::ComputeMenuTrackPoint fixed a little
//		2.05 : CMenuDockBar fixed
//			 : Inactive state problem fixed
//		2.04 : bug with ::TrackPopupEx carelessly fixed 
//           : synchronizing TrackPopup animation with win98 effect
//
//						written by MB <mb2@geocities.co.jp> 1999.11.27
//////////////////////////////////////////////////////////////////////

//#include "../base/SkinCtrl.h"

#include <vector>
#include <tmschema.h>
#include "../skinitf.h"
#include "../base/subclass.h"

namespace Skin {

	using ATL::CComPtr;
	using WTL::CDCHandle;
	using WTL::CMenuHandle;
	using namespace WTL;

#define CBRS_ALIGN_LEFT     0x1000L
#define CBRS_ALIGN_TOP      0x2000L
#define CBRS_ALIGN_RIGHT    0x4000L
#define CBRS_ALIGN_BOTTOM   0x8000L
#define CBRS_ALIGN_ANY      0xF000L

#define AFX_IDM_WINDOW_FIRST            0xE130
#define AFX_IDM_WINDOW_LAST             0xE13F
#define AFX_IDM_FIRST_MDICHILD          0xFF00  // window list starts here

	enum FrameStyle
	{
		DIALOG_STYLE = 0,
		NORMAL_STYLE = 1,
		SDI_STYLE	 = 2,
		MDI_STYLE    = 3
	};

	class CSkinMenuBar;

	// CXPMenuItem interface
	class CXPMenuItem
	{
		// Construction
	public:
		CXPMenuItem();

		// Attributes
		BYTE GetStyle() const { return m_fsStyle; }
		void ModifyState(BYTE fsRemove, BYTE fsAdd);
		BYTE GetState() const { return m_fsState; }
		CSize GetHorizontalSize() const;
		CRect GetRect() const { return m_rcItem; }
		TCHAR GetAccessKey() const { return m_cAccessKey; }

		// Overidables
		virtual void Update(CDCHandle dc, CSkinMenuBar* pMenuBar) = 0;
		virtual void Layout(CPoint, BOOL bHorz) = 0;
		virtual void TrackPopup(CSkinMenuBar* pBar, HWND hWndSentCmd) = 0;

		// Implementation
	public:
		virtual ~CXPMenuItem() { }

	protected:
		BYTE m_fsStyle;    // menu item style
		BYTE m_fsState;    // menu item state
		CRect m_rcItem;    // current rect
		CSize m_sizeHorz;  // ordinary horizontal size
		TCHAR m_cAccessKey;// access key (Alt key + X)
	};


	// CXPMenuItem style flags
	#define MISTYLE_TRACKABLE		0x01
	#define MISTYLE_WRAPPABLE		0x02
		// CXPMenuItem state flags
	#define MISTATE_HOT             0x01
	#define MISTATE_PRESSED         0x02
	#define MISTATE_HORZ            0x04
	#define MISTATE_HIDDEN          0x08
	#define MISTATE_WRAP            0x10
	#define MISTATE_INACTIVE		0x20

	//////////////////////////////////////////////////////////////////////
	// CMenuIcon class
	class CMenuIcon : public CXPMenuItem  
	{
		// Construction
	public:
		CMenuIcon(CSkinMenuBar* pMenuBar);

		// Operations
		void OnActivateChildWnd();

		// Overidables
		virtual void Update(CDCHandle dc, CSkinMenuBar* pMenuBar);
		virtual void Layout(CPoint, BOOL bHorz);
		virtual void TrackPopup(CSkinMenuBar* pBar, HWND hWndSentCmd);

		// Implementation
	public:
		virtual ~CMenuIcon();

	private:
		CSkinMenuBar* m_pMenuBar;
		HICON m_hDocIcon;
		HICON m_hIconWinLogo;// used on View which has no own Doc icon
	};

	
	
	//////////////////////////////////////////////////////////////////////
	// CMenuButton class
	class CMenuButton : public CXPMenuItem  
	{
		// Construction
	public:
		CMenuButton(HMENU hMenu, int nIndex);

		// Overidables
		virtual void Update(CDCHandle dc, CSkinMenuBar* pMenuBar);
		virtual void Layout(CPoint, BOOL bHorz);
		virtual void TrackPopup(CSkinMenuBar* pBar, HWND hWndSentCmd);

		// Implementation
	public:
		virtual ~CMenuButton() { }

	private:
		CString m_strBtn;                // string on button
		CPoint  m_ptLineFrom, m_ptLineTo;// vertical line point on button
		HMENU   m_hSubMenu;              // handle to sub menu
		UINT    m_nID;                   // Item ID (if no sub menu)

		// Implementation helper
		// used on Update
		void DrawHot(CDCHandle dc);
		void DrawPressed(CDCHandle dc);
		void DrawNone(CDCHandle dc);
		void DrawHorzText(CDCHandle dc, CPoint ptOffset = CPoint(0, 0));
		void DrawVertText(CDCHandle dc, CPoint ptOffset = CPoint(0, 0));
		// used on constructing
		void InitButtonStringAndSubMenuHandle(HMENU hMenu, int nIndex);
		void InitHorizontalButtonSize();
		void InitAccessKeyAndVerticalLinePoint();
	};

	//////////////////////////////////////////////////////////////////////
	// CMenuControl class
	class CMenuControl : public CXPMenuItem  
	{
		// Construction
	public:
		CMenuControl(CSkinMenuBar* pMenuBar);

		// Operations
		void OnActivateChildWnd();
		BOOL OnMouseMsg(UINT msg, UINT nFlags, WTL::CPoint pt);
		void ForceDrawControl(CDCHandle dc);
		void DelayLayoutAndDraw(CDCHandle dc, CSize sizeBar);

		// Overidables
		virtual void Update(CDCHandle dc, CSkinMenuBar* pMenuBar);
		virtual void Layout(CPoint, BOOL bHorz);
		virtual void TrackPopup(CSkinMenuBar* pBar, HWND hWndSentCmd) { ASSERT(TRUE); }

		// Implementation
	public:
		virtual ~CMenuControl() { }

	private:
		CSkinMenuBar* m_pMenuBar;
		CRect m_arrCaption[3];
		BOOL  m_bDown;
		int	  m_nTracking;

		// Implementation helper
		void DrawControl(CDCHandle dc, int nIndex, BOOL bDown);
		int HitTest(CPoint point);
		CSize GetCaptionSize();
	};


	/////////////////////////////////////////////////////////////////////////////
	// CXPMenuBarFrameHook
	class CMainFrameHook : public CSubclassWnd
	{
	public:
		CMainFrameHook();
		BOOL Install(CSkinMenuBar* pMenuBar, HWND hWndToHook);
		virtual ~CMainFrameHook();

	protected:
		virtual LRESULT WindowProc(UINT nMsg, WPARAM wParam, LPARAM lParam);

	private:
		CSkinMenuBar* m_pMenuBar;
	};

	class CMDIClientHook : public CSubclassWnd
	{
	public:
		CMDIClientHook();
		BOOL Install(CSkinMenuBar* pMenuBar, HWND hWndToHook);
		virtual ~CMDIClientHook();

	protected:
		virtual LRESULT WindowProc(UINT nMsg, WPARAM wParam, LPARAM lParam);

	private:
		CSkinMenuBar* m_pMenuBar;
	};

	class CSkinMenuBar	
	{
	private:

		enum MenuConfig
		{
			MENU_LEFT	= 5,
			MENU_TOP	= 5,// TODO

		};

		enum TrackingState { none = 0, button, popup, buttonmouse };

		static WTL::CRect _CalcTextRect( const char* szText, HFONT fontMenu )
		{
			WTL::CWindowDC dc(NULL);
			WTL::CRect rcText(0, 0, 0, 0);

			HFONT hOldFont = dc.SelectFont(fontMenu);
			dc.DrawText(szText, -1, &rcText, DT_SINGLELINE | DT_CALCRECT);
			dc.SelectFont(hOldFont);

			return rcText;
		}

		static HFONT GetCtrlFont(HWND hwnd)
		{
			HFONT hFont;
			//如果sendmessage得到的是NULL,则使用的是系统字体
			//if ( (hFont = (HFONT)::SendMessage(hwnd,WM_GETFONT,0,0))==NULL)
			hFont = (HFONT)::GetStockObject(SYSTEM_FONT);

			return hFont;
		}
		int GetNextOrPrevButton(int nIndex, BOOL bPrev)
		{
			int nCount = GetItemCount();
			if (bPrev) {				// <-
				--nIndex;
				if (nIndex < 0)
					nIndex = nCount - 1;
			}
			else {						// ->
				++nIndex;
				if (nIndex >= nCount)
					nIndex = 0;
			}

			if (!(m_MenuLst[nIndex]->GetStyle() & MISTYLE_TRACKABLE) ||
				(m_MenuLst[nIndex]->GetState() & MISTATE_HIDDEN)) {
					return GetNextOrPrevButton(nIndex, bPrev);
				}

				return nIndex;
		}

		int GetSchemeWidth(int iPartId, int iStateId = 0)
		{
			return GetSchemeSize(iPartId, iStateId).cx;
		}

		int GetSchemeHeight(int iPartId, int iStateId = 0)
		{
			return GetSchemeSize(iPartId, iStateId).cy;
		}

		SIZE GetSchemeSize(int iPartId, int iStateId = 0)
		{
			CRect rc = GetSchemeRect(iPartId, iStateId);
			return rc.Size();
		}

		RECT GetSchemeRect(int iPartId, int iStateId = 0)
		{
			RECT rc;
			BOOL f = m_scheme->GetRect(DIALOG, iPartId, iStateId, &rc);
			ASSERT(f);
			return rc;
		}

		COLORREF GetSchemeColor(int iPartId, int iStateId, int iPropId)
		{
			// GetColor(int iClassId, int iPartId, int iStateId, int iPropId, COLORREF *pColor)
			COLORREF ret;
			BOOL f = m_scheme->GetColor(DIALOG, iPartId, iStateId, iPropId, &ret);
			ASSERT(f);
			return ret;
		}



		static CSkinMenuBar* GetActiveMenuBar()
		{
			int nCount = (int)m_MenuBarlst.size();
			for ( int i = 0; i < nCount; i++ )
			{
				FRAMESTATES* pFs = m_MenuBarlst[i]->m_pframe_state;
				if ( *(pFs) == FS_ACTIVE ) 
					return m_MenuBarlst[i];
			}
			return NULL;
		}



		int GetItemCount()
		{
			return (int) m_MenuLst.size();
		}

		HWND GetCmdSentOleWnd();

	public:
		CSkinMenuBar();

		~CSkinMenuBar()
		{
			DeleteItems();
		}

		void SetMenu( HMENU hMenu )
		{
			m_hMenu = hMenu;

			if ( !m_scheme )
			{
				CComPtr<ISkinMgr> mgr;
				if (S_OK == GetSkinMgr(&mgr) && mgr)
					mgr->GetCurentScheme(&m_scheme);
			}
		}

		HMENU GetMenu()
		{
			return m_hMenu;
		}

		void SetWnd( HWND hWnd, FrameStyle frameStyle = NORMAL_STYLE )
		{
			m_hWnd = hWnd;
			// 做两个事情
			BOOL bFind = FALSE;
			for ( int i = 0; i < m_MenuBarlst.size(); i++)
			{
				if ( m_MenuBarlst[i] == this )
				{
					bFind = TRUE;
					break;
				}
			}
			if ( !bFind )
				m_MenuBarlst.push_back( this );

			m_hookFrame.Install(this, m_hWnd );

			if ( frameStyle == MDI_STYLE )
			{
				m_bMDIApp = TRUE;
				m_hWndMDIClient = ::FindWindowEx( m_hWnd, NULL, "MDIClient", NULL );   
				ASSERT(::IsWindow(m_hWndMDIClient));
				m_hookMDIClient.Install(this, m_hWndMDIClient);
			}
		}

		HWND GetSafeHwnd()
		{
			return m_hWnd;
		}

		HWND GetActiveChildWnd(BOOL& bMaximized)
		{
			if (!m_hWndMDIClient)
				return NULL;

			BOOL bMax = FALSE;
			HWND hWnd = (HWND)::SendMessage(m_hWndMDIClient,
				WM_MDIGETACTIVE, 0, (LPARAM)&bMax);
			bMaximized = bMax;

			return hWnd;
		}

		void SetFrameState( FRAMESTATES* pFs )
		{
			m_pframe_state = pFs;
		}

		STDMETHOD(GetCurentScheme)(ISkinScheme ** ppScheme)
		{
			ASSERT(m_scheme && ppScheme);
			return m_scheme.CopyTo(ppScheme);
		}

		BOOL MapAccessKey(TCHAR cAccessKey, int& nIndex)
		{
			for (int i = 0; i < GetItemCount(); ++i)
			{
				// fixing point
				char cKey = m_MenuLst[i]->GetAccessKey();
				if (toupper(cKey)/*_totupper(cKey)*/ == cAccessKey) 
				{// *****fixed by andi, thanx!*****
					nIndex = i;
					return TRUE;
				}
			}
			return FALSE;
		}

		/*
		int	PtInMenu(  WTL::CPoint point )
		{
			WTL::CRect rcw;
			::GetWindowRect( m_hWnd, &rcw);

			WTL::CPoint offsetPt = point;

			offsetPt.x -= rcw.left;
			offsetPt.y -= rcw.top;

			for (int i = 0; i < GetItemCount(); ++i) {
				CXPMenuItem* pItem = m_MenuLst[i];
				CRect rcItem = pItem->GetRect();

				if ((pItem->GetStyle() & MISTYLE_TRACKABLE) &&
					rcItem.PtInRect(offsetPt))
					return i;
			}

			return -1;

		}
		*/

		int CalcMenuBarHeight()
		{
			int nCount = GetItemCount();
			if ( nCount == 0 )
				return 0;
			else
				return CalcSize( nCount ).cy;

			//	return m_MenuLst[ nCount - 1 ]->GetRect().bottom - m_MenuLst[ 0 ]->GetRect().top;
		}


		RECT CalcMenuItemRect( int nItemIndex )
		{
			if ( GetItemCount() > nItemIndex )
				return m_MenuLst[ nItemIndex ]->GetRect();
			else
			{
				RECT rc;
				rc.left = rc.right = rc.top = rc.bottom = 0;
				return rc;
			}
		}

		void DrawBackground( HDC hdc, int nPart, int nState, const RECT *pRect, const RECT *pClipRect)
		{
			if ( m_scheme )
				m_scheme->DrawBackground( hdc, MENU, nPart, nState, pRect, pClipRect );
		}

		void DrawText( HDC hdc, int nPart, int nState, LPCSTR szText, DWORD dwTextFlags, DWORD dwTextFlags2, const RECT *pRect)
		{
			if ( m_scheme )
				m_scheme->DrawText( hdc, MENU, nPart, nState, szText, dwTextFlags, dwTextFlags2, pRect );
		}

		RECT CalcMenuBarRect(const WTL::CRect& rcw, FRAMESTATES fs)
		{
			CAPTIONSTATES caption_state = (fs == FS_ACTIVE) ? CS_ACTIVE : CS_INACTIVE;
			int caption_height = GetSchemeHeight(WP_CAPTION, caption_state);
			int bottom_height = GetSchemeHeight(WP_FRAMEBOTTOM, caption_state);	
			int border_left_width = GetSchemeWidth(WP_FRAMELEFT, caption_state);	
			int border_right_width = GetSchemeWidth(WP_FRAMERIGHT, caption_state);

			WTL::CRect rc = rcw;
			rc.top += caption_height;
			rc.left += border_left_width;
			rc.right -= border_right_width;
			rc.bottom = rc.top + CalcMenuBarHeight();
			return rc;
		}

		//void InitMenu( HWND hWnd, CMenuHandle menu, int nFrameLeft, int nCaptionHeight );
		//******************************************************************
		BOOL OnMenuInput(  MSG& m )
		{
			int nMsg = m.message;
			WTL::CPoint pt = (WTL::CPoint)m.lParam;
			switch (nMsg) 
			{
			case WM_MOUSEMOVE:
				{
					int nIndex = HitTestOnTrack(pt);
					if (IsValidIndex(nIndex) && nIndex != m_nCurIndex) {
						// defferent button clicked
						::SendMessage( m_hWnd, WM_CANCELMODE, 0, 0 ); // destroy popupped menu
						UpdateBar();							// clean up
						m_nCurIndex = nIndex;
						m_bLoop = TRUE;							// continue loop
					}
				}

				break;

			case WM_LBUTTONDOWN:
				if (HitTestOnTrack(pt) != -1 && HitTestOnTrack(pt) == m_nCurIndex) 
				{
					// same button clicked
					::SendMessage( m_hWnd, WM_CANCELMODE, 0, 0 ); // destroy popupped menu
					UpdateBar(button, m_nCurIndex);
					m_bLoop = FALSE;						// out of loop
					return TRUE;							// eat it!
				}

				break;


			case WM_KEYDOWN:
				{
					TCHAR vKey = m.wParam;

					if ((vKey == VK_LEFT  && m_bProcessLeftArrow) ||
						(vKey == VK_RIGHT && m_bProcessRightArrow))
					{
						// no submenu
						int nNewIndex = GetNextOrPrevButton(m_nCurIndex, vKey==VK_LEFT);
						::SendMessage( m_hWnd, WM_CANCELMODE, 0, 0 ); // destroy popupped menu
						UpdateBar();
						m_nCurIndex = nNewIndex;
						m_bLoop = TRUE;							// continue loop			
						return TRUE;							// eat it!
					}
				}
				break;

			case WM_SYSKEYDOWN:
				//		LTRACE(_T("    m_bIgnore = TRUE\n"));
				m_bIgnoreAlt = TRUE;					// next SysKeyUp will be ignored
				break;
			}
			return FALSE;	// pass along...
		}


		
		void TrackPopup(int nIndex);

		static LRESULT CALLBACK MenuInputFilter(int code, WPARAM wParam, LPARAM lParam);
		static LRESULT CALLBACK KeyInputFilter(int code, WPARAM wParam, LPARAM lParam);

		static LRESULT InitKeyAccess();
		static LRESULT UnInitKeyAccess();

		void DrawMenuBar(CDCHandle dc, CMenuHandle menu, const WTL::CRect& rcw, int iSpecialItem, MENUSTATES ms );
		void EtchedMenuBar(CDCHandle dc, const CMenuHandle menu, const WTL::CRect& rcw, int iSpecialItem, MENUSTATES ms);
		void EtchedMenuBarItem(CDCHandle dc, CMenuHandle menu, UINT nItemIndex, MENUSTATES ms);
		void DrawMenuBarItem(CDCHandle dc, CMenuHandle menu, const WTL::CRect& rcItem, UINT nItemIndex, MENUSTATES ms);

		//from CXPMenuBar class version 2.12
		void OnSetMenu(HMENU hNewMenu, HMENU hWindowMenu);
		HMENU LoadMenu(HMENU hMenu, HMENU hMenuWindow);
		BOOL InitItems();
		HWND OleMenuDescriptor(BOOL& bSend, UINT nMsg, WPARAM wParam, LPARAM lParam);
		void OnMenuSelect(HMENU hMenu, UINT nIndex);
		void OnFrameNcActivate(BOOL bActive);
		LRESULT OnSettingChange(WPARAM wParam, LPARAM lParam);
		void DeleteItems();
		void OnInitMenuPopup(HMENU hPopupMenu, UINT nIndex, BOOL bSysMenu);
		void GetClientRect(CRect& rc);
		BOOL IsValidIndex(int nIndex);

		void OnMouseMove(UINT nFlags, WTL::CPoint point);
		void OnMouseLeave();
		void OnLButtonDown(UINT nFlags, WTL::CPoint point);
		void OnLButtonUp(UINT nFlags, WTL::CPoint point) ;

		void UpdateBar(TrackingState nState = none, int nNewIndex = -1);
		int HitTestOnTrack(CPoint point);
		CSize CalcLayout();
		int GetClipBoxLength();
		void SizeMenuBar(int nLength, BOOL bVert = FALSE);
		void CalcItemLayout(int nCount, BOOL bVert = FALSE);
		CSize CalcSize(int nCount);
		int WrapMenuBar(int nCount, int nWidth);
		void DoPaint(CDCHandle dc);
		void CheckActiveChildWndMaximized();
	private:
		//std::vector<WTL::CRect> m_MenuRectLst;

		std::vector<CXPMenuItem*>	m_MenuLst;
		
		//std::vector<TCHAR>		m_AccessKeyLst;


		HMENU	m_hMenu;
		BOOL	m_bLoop;
		BOOL	m_bProcessLeftArrow;
		BOOL	m_bProcessRightArrow;

		CComPtr<ISkinScheme> m_scheme;

		HWND	m_hWnd;	//frame or dialog


		FRAMESTATES* m_pframe_state;

		static std::vector<CSkinMenuBar* >	m_MenuBarlst;//是否考虑用map?

		CMenuIcon* m_pMenuIcon;
		CMenuControl* m_pMenuControl;
		
		TrackingState m_nTrackingState;
		int				m_nCurIndex;

		CMainFrameHook m_hookFrame;
		BOOL m_bFrameActive;// synchronize with Frame Activation
		CMDIClientHook m_hookMDIClient;
		BOOL			m_bMDIMaximized;

	public:
//		UINT*			m_pselectedMenuItem;

		HWND	m_hWndMDIClient;
		HWND	m_hWndActiveChild;
		HMENU	m_hWindowMenu;
		BOOL	m_bMDIApp;
		BOOL	m_bIgnoreAlt;
	};

	

	// Menu 扩充的 part and property 
	// BEGIN_TM_CLASS_PARTS(WINDOW)
#define WP_SYSTEM_LAST  WP_SMALLFRAMEBOTTOMSIZINGTEMPLATE
#define WP_MENUBAR      WP_SYSTEM_LAST + 1  // WP_MENU 已经被使用了

	// TMT_MENU             ? menu item background
	// TMT_MENUTEXT         The color of text drawn on a menu.
	// TMT_MENUHILIGHT      The color of highlight drawn on a menu item when the 
	//                      user moves the mouse over it.
	// TMT_MENUBAR          The color of the menu bar. 
	//                      ? bar background
#define TMT_MENU_BORDER         TMT_MENUBAR + 1     // item border
#define TMT_MENUBORDER_HILIGHT  TMT_MENUBAR + 2     // hilight border

}; // namespace 