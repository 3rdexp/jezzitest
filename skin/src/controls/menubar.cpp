#include "stdafx.h"

#include "menubar.h"

namespace Skin {

std::vector<CSkinMenuBar* >	CSkinMenuBar::m_MenuBarlst;
static CSkinMenuBar*	g_pMenuBar = NULL;	//处理点击菜单的操作
static HHOOK			g_hMsgHook = NULL;
static HHOOK			g_hKeyHook = NULL;

//////////////////////////////////////////////////////////////////////
// I've found string resource of "More windows" in "user.exe".
// But I can't load it, so please replace a following with your language.
static const TCHAR _szMoreWindows[] = _T("&More windows...");

//////////////////////////////////////////////////////////////////////
// used for OLE menu (easy fix)
static BOOL _bWindowMenuSendCmd = FALSE;
static int _nPrevIndexForCmd = -1;

const int cxBorder2 = ::GetSystemMetrics(SM_CXBORDER)*2;
const int cyBorder2 = ::GetSystemMetrics(SM_CYBORDER)*2;

// common resources
static CFont _fontHorzMenu, _fontVertMenu;
static int _cyHorzFont, _cyMenuOnBar, _cyTextMargin;
const int CXTEXTMARGIN = 5;

static BOOL _InitCommonResources(BOOL bForce = FALSE)
{
	if (bForce == FALSE && _fontHorzMenu.m_hObject != NULL)
		return TRUE;// no need to reinitialize

	// clean up
	_fontHorzMenu.DeleteObject();
	_fontVertMenu.DeleteObject();

	// create fonts
	NONCLIENTMETRICS info; info.cbSize = sizeof(info);
	::SystemParametersInfo(SPI_GETNONCLIENTMETRICS, sizeof(info), &info, 0);
	if (!_fontHorzMenu.CreateFontIndirect(&info.lfMenuFont))
		return FALSE;

	// create vertical font
	info.lfMenuFont.lfEscapement = -900;
	info.lfMenuFont.lfOrientation = -900;
	if (!_fontVertMenu.CreateFontIndirect(&info.lfMenuFont))
		return FALSE;

	// get font height
	_cyHorzFont = abs(info.lfMenuFont.lfHeight);

	// calc Y text margin
	_cyMenuOnBar = info.iMenuHeight;
	_cyMenuOnBar = max(_cyMenuOnBar, ::GetSystemMetrics(SM_CYSMICON));
	_cyTextMargin = (_cyMenuOnBar - _cyHorzFont) / 2;

	return TRUE;
}

// I wanted to control popup point, but I've fount we can never get popupmenu rect before popup.
// even if not owner draw menu...
static CPoint _ComputeMenuTrackPoint(HWND hWnd, const CRect& rcItem, DWORD dwStyle, UINT& fuFlags, TPMPARAMS& tpm)
{
	fuFlags = TPM_LEFTBUTTON | TPM_LEFTALIGN | TPM_HORIZONTAL;
	tpm.cbSize = sizeof(tpm);
	CPoint pt;
	CRect& rcExclude = (CRect&)tpm.rcExclude;
	//CWnd::GetDesktopWindow()->GetWindowRect(&rcExclude);

	CRect rcFrame;
	::GetWindowRect(hWnd, &rcFrame);

	switch (dwStyle & CBRS_ALIGN_ANY)
	{
	case CBRS_ALIGN_RIGHT:
	case CBRS_ALIGN_LEFT:
		pt = CPoint(rcItem.right+1, rcItem.top-1);
		// to avoid strange menu flip, won't do : [rcExclude.right = rcItem.right+1;]
		// I want to use : fuFlags |= TPM_HORNEGANIMATION;
		break;

	default: //	case CBRS_ALIGN_TOP:
		pt = CPoint(rcItem.left-1 + rcFrame.left , rcItem.bottom + rcFrame.top );
		//rcExclude.bottom = rcItem.bottom+1;// <- insead of [fuFlags |= TPM_VERPOSANIMATION;]

		break;
	}
	return pt;
}

static int _CalcTextWidth(const CString& strText)
{
	CWindowDC dc(NULL);
	CRect rcText(0, 0, 0, 0);
	CFont* pOldFont = dc.SelectObject(&_fontHorzMenu);
	dc.DrawText(strText, &rcText, DT_SINGLELINE | DT_CALCRECT);
	dc.SelectObject(pOldFont);

	return rcText.Width();
}

//////////////////////////////////////////////////////////////////////
// CXPMenuItem interface
CXPMenuItem::CXPMenuItem()
{
	m_fsStyle = 0;
	m_fsState = 0;
	m_rcItem.SetRectEmpty();
	m_sizeHorz = CSize(0, 0);
	m_cAccessKey = 0;
}

void CXPMenuItem::ModifyState(BYTE fsRemove, BYTE fsAdd)
{
	m_fsState = (m_fsState & ~fsRemove) | fsAdd;
}

CSize CXPMenuItem::GetHorizontalSize() const
{
	if (m_fsState & MISTATE_HIDDEN)
		return CSize(0, 0);
	else
		return m_sizeHorz;
}
//////////////////////////////////////////////////////////////////////
// CMenuIcon class
CMenuIcon::CMenuIcon(CSkinMenuBar* pMenuBar)
{
	ASSERT(pMenuBar);
	m_pMenuBar = pMenuBar;
	m_hIconWinLogo = AfxGetApp()->LoadStandardIcon(IDI_WINLOGO);
	ASSERT(m_hIconWinLogo);

	m_fsStyle |= MISTYLE_TRACKABLE;
	m_fsState |= MISTATE_HIDDEN;
	m_sizeHorz = CSize(::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON));
}

CMenuIcon::~CMenuIcon()
{
	if (m_hIconWinLogo != NULL)
		::FreeResource(m_hIconWinLogo);
}

void CMenuIcon::OnActivateChildWnd()
{
	//LTRACE(_T("CMenuIcon::OnActivateChildWnd\n"));

	ASSERT(m_pMenuBar);

	HWND hWndMDIClient = m_pMenuBar->m_hWndMDIClient;
	ASSERT(::IsWindow(hWndMDIClient));

	BOOL bMaximized = FALSE;
	HWND hWndChild = (HWND)::SendMessage(hWndMDIClient,
		WM_MDIGETACTIVE, 0, (LPARAM)&bMaximized);

	if (bMaximized == FALSE) {
		//LTRACE(_T("    not maximized\n"));
		m_fsState |= MISTATE_HIDDEN;
	}
	else {
		//LTRACE(_T("    maximized\n"));
		m_fsState &= ~MISTATE_HIDDEN;
	}

	m_hDocIcon = (HICON)::GetClassLong(hWndChild, GCL_HICONSM);

	if (m_hDocIcon == NULL) // if hWndChild doesn't have own icon
		m_hDocIcon = m_hIconWinLogo;
}

void CMenuIcon::Update(CDCHandle dc, CSkinMenuBar* pMenuBar)
{
	if (m_fsState & MISTATE_HIDDEN)
		return;

	ASSERT(m_hDocIcon);
	//ASSERT(m_rcItem.IsRectEmpty() == FALSE);
	::DrawIconEx(dc.m_hDC, m_rcItem.left, m_rcItem.top, m_hDocIcon,
		m_rcItem.Width(), m_rcItem.Height(), 0, NULL, DI_NORMAL);
}

void CMenuIcon::TrackPopup(CSkinMenuBar* pBar, HWND hWndSentCmd)
{
	ASSERT(!(m_fsState & MISTATE_HIDDEN));
	ASSERT(m_pMenuBar);

	HWND hWndMDIClient = m_pMenuBar->m_hWndMDIClient;
	ASSERT(::IsWindow(hWndMDIClient));

	BOOL bMaximized = FALSE;
	HWND hWndChild = (HWND)::SendMessage(hWndMDIClient,
		WM_MDIGETACTIVE, 0, (LPARAM)&bMaximized);
	ASSERT(bMaximized);
	HMENU hSysMenu = ::GetSystemMenu(hWndChild, FALSE);
	ASSERT(::IsMenu(hSysMenu));	
	CRect rcItem(m_rcItem);	
	//m_pMenuBar->ClientToScreen(rcItem);
	UINT fuFlags; TPMPARAMS tpm;
	CPoint pt = _ComputeMenuTrackPoint(pBar->GetSafeHwnd(), rcItem, CBRS_ALIGN_TOP, fuFlags, tpm);

	::TrackPopupMenuEx(hSysMenu, fuFlags, pt.x, pt.y, hWndSentCmd, &tpm);
}

void CMenuIcon::Layout(CPoint point, BOOL bHorz)
{
	if (bHorz)
		m_fsState |= MISTATE_HORZ;
	else
		m_fsState &= ~MISTATE_HORZ;

	if (m_fsState & MISTATE_HIDDEN)
	{
		m_rcItem.SetRectEmpty();
		return;
	}

	m_rcItem = CRect(point, CSize(::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON)));
}


//////////////////////////////////////////////////////////////////////
// CMenuButton class	
CMenuButton::CMenuButton(HMENU hMenu, int nIndex)
{
	ASSERT(::IsMenu(hMenu));
	ASSERT(nIndex >= 0);

	m_fsStyle |= (MISTYLE_TRACKABLE | MISTYLE_WRAPPABLE);
	InitButtonStringAndSubMenuHandle(hMenu, nIndex);
	InitHorizontalButtonSize();
	InitAccessKeyAndVerticalLinePoint();
}

void CMenuButton::InitButtonStringAndSubMenuHandle(HMENU hMenu, int nIndex)
{
	// get menu button Text
	TCHAR szText[256];
	MENUITEMINFO info; ::memset(&info, 0, sizeof(MENUITEMINFO));
	info.cbSize		= sizeof(MENUITEMINFO);
	info.fMask		= MIIM_ID | MIIM_TYPE;
	info.dwTypeData = szText;
	info.cch		= sizeof(szText);
	::GetMenuItemInfo(hMenu, nIndex, TRUE, &info);
	m_strBtn = CString(szText);

	m_hSubMenu = ::GetSubMenu(hMenu, nIndex);
	if (!m_hSubMenu) 
	{
		m_nID = ::GetMenuItemID(hMenu, nIndex);
		ASSERT(m_nID != -1);
	}
	else 
	{
		m_nID = -1;
	}
}

void CMenuButton::InitHorizontalButtonSize()
{
	// get menu button Text size
	ASSERT(m_strBtn.IsEmpty() == FALSE);
	m_sizeHorz.cx = _CalcTextWidth(m_strBtn) + CXTEXTMARGIN*2;
	m_sizeHorz.cy = _cyHorzFont + _cyTextMargin*2;
}

void CMenuButton::InitAccessKeyAndVerticalLinePoint()
{
	int nIndex = m_strBtn.Find('&');
	if (nIndex + 1 == m_strBtn.GetLength()) 
	{
		TRACE(_T("warning : & is bad position, access key is invalid.\n"));
		m_cAccessKey = 0;
		m_ptLineFrom = m_ptLineTo = CPoint(0, 0);
		return;
	}

	m_cAccessKey = m_strBtn[nIndex + 1];// -1 + 1 = 0; it's ok

	if (nIndex == -1) 
	{
		m_ptLineFrom = m_ptLineTo = CPoint(0, 0);
	}
	else if (nIndex == 0)
	{
		m_ptLineFrom = CPoint(_cyTextMargin, CXTEXTMARGIN);
		m_ptLineTo	 = CPoint(_cyTextMargin, CXTEXTMARGIN + _CalcTextWidth(m_strBtn.Left(nIndex+2)));
	}
	else 
	{
		m_ptLineFrom = CPoint(_cyTextMargin, CXTEXTMARGIN + _CalcTextWidth(m_strBtn.Left(nIndex)));
		m_ptLineTo = CPoint(_cyTextMargin, CXTEXTMARGIN + _CalcTextWidth(m_strBtn.Left(nIndex+2)));
	}
}

void CMenuButton::Layout(CPoint point, BOOL bHorz)
{
	if (bHorz)
		m_fsState |= MISTATE_HORZ;
	else
		m_fsState &= ~MISTATE_HORZ;

	if (m_fsState & MISTATE_HIDDEN) {
		m_rcItem.SetRectEmpty();
		return;
	}

	if (bHorz) 
	{
		m_rcItem = CRect(point, m_sizeHorz);
	}
	else 
	{
		m_rcItem = CRect(point, CSize(m_sizeHorz.cy, m_sizeHorz.cx));
	}
}

void CMenuButton::Update(CDCHandle dc, CSkinMenuBar* pMenuBar)
{
	if (m_fsState & MISTATE_HIDDEN)
		return;

	// clean background
	//COLORREF clr = ::GetSysColor(COLOR_3DFACE);
	//dc.FillSolidRect( m_rcItem, clr );

	/*
	<area state="1" name="PBS_NORMAL"    rc="144 298 217 319" step_horz="4" step_vert="4" />
	<area state="2" name="PBS_HOT"        rc="219 298 292 319" step_horz="4" step_vert="4" />
	<area state="3" name="PBS_PRESSED"    rc="294,298 367 319" step_horz="4" step_vert="4" />
	<area state="5" name="PBS_DEFAULTED" rc="446 298 519 319" step_horz="4" step_vert="4" />
	<area state="4" name="PBS_DISABLED"  rc="369 298 442 319" step_horz="4" step_vert="4" />
	*/

	int nState = 1;
	if (m_fsState & MISTATE_HOT)
	{
		nState = 2;
		//DrawHot(dc);
	}
	else if (m_fsState & MISTATE_PRESSED)
	{
		nState = 3;
		//DrawPressed(dc);
	}
	/*
	else 
	{
		//DrawNone(dc);
	}
	*/
	if ( pMenuBar )
		pMenuBar->DrawBackground(dc.m_hDC, 1, nState, &m_rcItem, NULL );

	HFONT hOldFont = dc.SelectFont( _fontHorzMenu );

	if ( pMenuBar )
		pMenuBar->DrawText(dc.m_hDC, 1, nState, m_strBtn.GetBuffer(m_strBtn.GetLength()), DT_SINGLELINE | DT_CENTER | DT_VCENTER, 0, &m_rcItem);

	dc.SelectFont(hOldFont);

	
}

void CMenuButton::TrackPopup(CSkinMenuBar* pBar, HWND hWndSentCmd)
{
	TRACE(_T("CMenuButton::TrackPopup\n"));
	ASSERT(pBar);
	ASSERT(!(m_fsState & MISTATE_HIDDEN));
	CSkinMenuBar* pMenuBar = pBar;
	ASSERT(pMenuBar);

	// "! menu" (no sub menu)
	if (!m_hSubMenu) 
	{
		ASSERT(m_nID != -1);
		::SendMessage(hWndSentCmd, WM_COMMAND, (WPARAM)m_nID, (LPARAM)pBar->GetSafeHwnd());		
		return;
	}

	CRect rcItem(m_rcItem);	
	//pMenuBar->ClientToScreen(rcItem);
	UINT fuFlags; TPMPARAMS tpm;
	CPoint pt = _ComputeMenuTrackPoint(pBar->GetSafeHwnd(), rcItem, CBRS_ALIGN_TOP, fuFlags, tpm);

	if (m_hSubMenu == pMenuBar->m_hWindowMenu)
		_bWindowMenuSendCmd = TRUE;
	else
		_bWindowMenuSendCmd = FALSE;

	::TrackPopupMenuEx(m_hSubMenu, fuFlags,
		pt.x, pt.y, hWndSentCmd, &tpm);
}

void CMenuButton::DrawHorzText(CDCHandle dc, CPoint ptOffset)
{
	COLORREF clr = (m_fsState & MISTATE_INACTIVE) ? 
		::GetSysColor(COLOR_GRAYTEXT) : ::GetSysColor(COLOR_WINDOWTEXT);
	dc.SetTextColor(clr);

	CRect rcBtn = m_rcItem;
	dc.SetBkMode(TRANSPARENT);
	HFONT hOldFont = dc.SelectFont( _fontHorzMenu );
	// I know precise text size, but better to leave this job to Windows
	// *****fixed by andi, thanks!*****
	dc.DrawText(m_strBtn,m_strBtn.GetLength(), rcBtn + ptOffset,
		DT_SINGLELINE | DT_CENTER | DT_VCENTER);

	dc.SelectFont(hOldFont);
}

void CMenuButton::DrawVertText(CDCHandle dc, CPoint ptOffset)
{
	/*
	COLORREF clr = (m_fsState & MISTATE_INACTIVE) ? 
		::GetSysColor(COLOR_GRAYTEXT) : ::GetSysColor(COLOR_WINDOWTEXT);
	SetTextColor(hdc, clr);

	CRect rcBtn = m_rcItem;
	int nLength = m_strBtn.GetLength();
	int nIndex = m_strBtn.Find('&');
	CString strBtn = m_strBtn.Left(nIndex) + m_strBtn.Right(nLength - (nIndex+1));

	// fixed for WinNT. *****fixed by Simon, thanks!*****
	int iGraphicsMode = ::GetGraphicsMode(hdc);
	::SetGraphicsMode(hdc, GM_ADVANCED);

	SetBkMode(hdc, TRANSPARENT);
	CFont* pOldFont = SelectObject(hdc, &_fontVertMenu);
	// I know precise text size
	CRect rcString = CRect(
		CPoint(rcBtn.right - _cyTextMargin, rcBtn.top + CXTEXTMARGIN), m_sizeHorz);
	pDC->DrawText(strBtn, rcString + ptOffset,
		DT_SINGLELINE | DT_NOCLIP | DT_NOPREFIX);// don't forget DT_NOCLIP

	pDC->SelectObject(pOldFont);

	// CDC::DrawText is poor, so we have to draw vertical line by ourselves
	CPen pen(PS_SOLID, 0, clr);
	CPen* pOldPen = pDC->SelectObject(&pen);

	pDC->MoveTo(rcBtn.TopLeft() + m_ptLineFrom + ptOffset);
	pDC->LineTo(rcBtn.TopLeft() + m_ptLineTo + ptOffset);

	pDC->SelectObject(pOldPen);
	::SetGraphicsMode( pDC->m_hDC, iGraphicsMode );
	*/
}

void CMenuButton::DrawHot(CDCHandle dc)
{
	COLORREF crHighLight = ::GetSysColor (COLOR_HIGHLIGHT);
	//CPenDC pen (cDC, crHighLight);
	if (m_fsState & MISTATE_HORZ) 
	{
		// draw pressed button
		//pDC->DrawEdge(m_rcItem, BDR_RAISEDINNER, BF_RECT);
		dc.FillSolidRect(m_rcItem,crHighLight);
		dc.Draw3dRect(m_rcItem, crHighLight, crHighLight);
		DrawHorzText(dc);
	}
	else 
	{
		//pDC->DrawEdge(m_rcItem, BDR_RAISEDINNER, BF_RECT);
		dc.FillSolidRect(m_rcItem,crHighLight);
		dc.Draw3dRect(m_rcItem,crHighLight, crHighLight);
		DrawVertText(dc);
	}
}

void CMenuButton::DrawPressed(CDCHandle dc)
{
	COLORREF crHighLight = ::GetSysColor (COLOR_BTNSHADOW);
	if (m_fsState & MISTATE_HORZ) 
	{
		//pDC->DrawEdge(m_rcItem, BDR_SUNKENOUTER, BF_RECT);// draw pressed button
		dc.FillSolidRect (m_rcItem, ::GetSysColor (COLOR_3DFACE));
		dc.Draw3dRect(m_rcItem,crHighLight, crHighLight);
		DrawHorzText(dc/*, CPoint(1, 1)*/);
	}
	else 
	{
		//pDC->DrawEdge(m_rcItem, BDR_SUNKENOUTER, BF_RECT);
		dc.FillSolidRect (m_rcItem, ::GetSysColor (COLOR_3DFACE));
		dc.Draw3dRect(m_rcItem,crHighLight, crHighLight);
		DrawVertText(dc/*, CPoint(1, 1)*/);
	}
}

void CMenuButton::DrawNone(CDCHandle dc)
{
	if (m_fsState & MISTATE_HORZ)
	{
		DrawHorzText(dc);
	}
	else 
	{
		DrawVertText(dc);
	}
}

//////////////////////////////////////////////////////////////////////
// CMenuControl class
#define CX_GAP_CAPTION 2

CMenuControl::CMenuControl(CSkinMenuBar* pMenuBar)
{
	ASSERT(pMenuBar);
	m_pMenuBar = pMenuBar;
	m_bDown = FALSE;
	m_nTracking = -1;

	m_fsState |= MISTATE_HIDDEN;

	CSize sizeCaption = GetCaptionSize();
	m_sizeHorz = CSize(sizeCaption.cx*3 + CX_GAP_CAPTION + 1, sizeCaption.cy + 2);
}

void CMenuControl::Update(CDCHandle dc,  CSkinMenuBar* pMenuBar)
{
	// do nothing
}

void CMenuControl::Layout(CPoint point, BOOL bHorz)
{
	//LTRACE(_T("CMenuControl::Layout bHorz:%d\n"), bHorz);

	if (bHorz)
		m_fsState |= MISTATE_HORZ;
	else
		m_fsState &= ~MISTATE_HORZ;

	if (m_fsState & MISTATE_HIDDEN) {
		m_rcItem.SetRectEmpty();
		return;
	}

	// just layout easily
	if (bHorz) 
	{
		m_rcItem = CRect(point, m_sizeHorz);
	}
	else
	{
		m_rcItem = CRect(point, CSize(m_sizeHorz.cy, m_sizeHorz.cx));
	}
}

void CMenuControl::DelayLayoutAndDraw(CDCHandle dc, CSize sizeBar)
{
	//LTRACE(_T("CMenuControl::DelayLayoutAndDraw\n"));
	//LTRACE(_T("    %d, %d\n"), sizeBar.cx, sizeBar.cy);
	// layout
	CSize sizeCaption = GetCaptionSize();
	int cxCaption = sizeCaption.cx;
	int cyCaption = sizeCaption.cy;

	if (m_fsState & MISTATE_HORZ) 
	{
		CRect rcCaption;
		rcCaption.right = sizeBar.cx;
		rcCaption.bottom = sizeBar.cy;
		rcCaption.left = rcCaption.right - cxCaption;
		rcCaption.top = rcCaption.bottom - cyCaption;

		m_arrCaption[0] = rcCaption;

		rcCaption -= CPoint(cxCaption+CX_GAP_CAPTION, 0);
		m_arrCaption[1] = rcCaption;

		rcCaption -= CPoint(cxCaption, 0);
		m_arrCaption[2] = rcCaption;

		m_rcItem = CRect(m_arrCaption[2].left, m_arrCaption[2].top,
			m_arrCaption[0].right, m_arrCaption[0].bottom);
	}
	else 
	{
		CRect rcCaption;
		rcCaption.left = 0;
		rcCaption.bottom = sizeBar.cy;
		rcCaption.right = rcCaption.left + cxCaption;
		rcCaption.top = rcCaption.bottom - cyCaption;

		m_arrCaption[0] = rcCaption;	

		rcCaption -= CPoint(0, cyCaption+CX_GAP_CAPTION);
		m_arrCaption[1] = rcCaption;

		rcCaption -= CPoint(0, cyCaption);
		m_arrCaption[2] = rcCaption;

		m_rcItem = CRect(m_arrCaption[2].left, m_arrCaption[2].top,
			m_arrCaption[0].right, m_arrCaption[0].bottom);
	}

	if (m_fsState & MISTATE_HIDDEN)
	{
		//LTRACE(_T("    hidden\n"));
		return;
	}

	// draw frame controls
	for (int i = 0; i < 3; ++i) 
	{
		DrawControl(dc, i, FALSE);
	}	
}

BOOL CMenuControl::OnMouseMsg(UINT msg, UINT nFlags, WTL::CPoint pt)
{
	if (m_fsState & MISTATE_HIDDEN)
		return FALSE;

	if (msg == WM_LBUTTONDOWN)
	{
		//LTRACE(_T("    WM_LBUTTONDOWN\n"));
		m_nTracking = HitTest(pt);
		if (m_nTracking >= 0) 
		{
			WTL::CWindowDC dc ( m_pMenuBar->GetSafeHwnd() );
			DrawControl(dc.m_hDC, m_nTracking, TRUE);
			m_bDown = TRUE;
			//::SetCapture(m_pMenuBar->GetSafeHwnd());				  // grab mouse input
			return TRUE;
		}
	}
	else if ((msg == WM_MOUSEMOVE) && m_nTracking >= 0)
	{
		// mouse moved, and I am tracking: possibly draw button up/down
		BOOL bOldDown = m_bDown;
		m_bDown = m_arrCaption[m_nTracking].PtInRect(pt);
		if (bOldDown != m_bDown)
		{
			// up/down state changed: need to redraw button
			WTL::CWindowDC dc ( m_pMenuBar->GetSafeHwnd() );
			DrawControl(dc.m_hDC, m_nTracking, m_bDown);
		}
		return TRUE; // handled

	}
	else if (msg == WM_LBUTTONUP && m_nTracking >= 0) 
	{
		// user released the mouse and I am tracking: do button command
		//ReleaseCapture(); // let go the mouse
		if (m_bDown)
		{
			// if button was down when released: draw button up, and do system cmd
			WTL::CWindowDC dc ( m_pMenuBar->GetSafeHwnd() );
			DrawControl(dc.m_hDC, m_nTracking, FALSE);
			//CFrameWnd* pFrame = m_pMenuBar->GetTopLevelFrame()->GetActiveFrame();
			//ASSERT_VALID(pFrame);
			static int syscmd[3] =
			{ /*SC_MOUSEMENU,*/ SC_CLOSE, SC_RESTORE, SC_MINIMIZE };
			// SendMessage occur access violation!
			// *****fixed by Koay Kah Hoe. Thanks!*****
			BOOL bMax;
			HWND hChild = m_pMenuBar->GetActiveChildWnd( bMax );
			::PostMessage(hChild, WM_SYSCOMMAND, syscmd[m_nTracking], 0);
		}
		m_nTracking = -1; // stop tracking
		return TRUE;	  // handled (eat)
	}

	return FALSE;
}

int CMenuControl::HitTest(CPoint point)
{
	for (int i = 0; i < 3; ++i)
	{
		if (m_arrCaption[i].PtInRect(point))
			return i;
	}

	return -1;
}

void CMenuControl::DrawControl(CDCHandle dc, int nIndex, BOOL bPressed)
{
	//LTRACE(_T("CMenuControl::DrawControl\n"));
	// draw frame controls
	
	CRect& rc = m_arrCaption[nIndex];
	//LTRACE(_T("    %d, %d, %d, %d\n"), rc.left, rc.top, rc.right, rc.bottom);
	/*
	static UINT dfcs[3] = { DFCS_CAPTIONCLOSE, DFCS_CAPTIONRESTORE, DFCS_CAPTIONMIN };
	UINT uState = dfcs[nIndex];
	uState |= DFCS_FLAT;
	dc.DrawFrameControl(rc, DFC_CAPTION, uState);
	*/
	int nState = 1;
	if ( bPressed )
		nState = 3;
	if ( m_pMenuBar )
	{
		m_pMenuBar->DrawBackground( dc.m_hDC, nIndex + 2, nState, rc, NULL );
	}
}

void CMenuControl::ForceDrawControl(CDCHandle dc)
{
	if (m_fsState & MISTATE_HIDDEN)
		return;

	for (int i = 0; i < 3; ++i) 
	{
		DrawControl(dc, i, FALSE);
	}
}

void CMenuControl::OnActivateChildWnd()
{
	//LTRACE(_T("CMenuControl::OnActivateChildWnd\n"));
	ASSERT(m_pMenuBar);

	//CWnd* pFrame = m_pMenuBar->GetTopLevelFrame();
	//ASSERT_VALID(pFrame);
	//CMDIFrameWnd* pMDIFrame = STATIC_DOWNCAST(CMDIFrameWnd, pFrame);
	HWND hWndMDIClient = m_pMenuBar->m_hWndMDIClient;
	ASSERT(::IsWindow(hWndMDIClient));

	BOOL bMaximized = FALSE;
	HWND hWndChild = (HWND)::SendMessage(hWndMDIClient,
		WM_MDIGETACTIVE, 0, (LPARAM)&bMaximized);

	if (bMaximized == FALSE) 
	{
		m_fsState |= MISTATE_HIDDEN;
	}
	else
	{
		m_fsState &= ~MISTATE_HIDDEN;
	}
}

CSize CMenuControl::GetCaptionSize()
{
	NONCLIENTMETRICS info; info.cbSize = sizeof(info);
	::SystemParametersInfo(SPI_GETNONCLIENTMETRICS, sizeof(info), &info, 0);

	// due to my own feeling
	return CSize(info.iMenuHeight - info.iBorderWidth*2, info.iMenuHeight - info.iBorderWidth*4);
}

/////////////////////////////////////////////////////////////////////////////
// CXPMenuBar insists own bar line 
/////////////////////////////////////////////////////////////////////////////
// CXPMenuBar OLE menu support
//		MFC does'nt do command routing for other process server.
//		::TrackPopupMenuEx won't accept HWND of other process and
//		we have to determine a message target(ole server window or not)
//		as ::OleCreateMenuDescriptor do.
//		This is a hard coding. 
//		First menu(ordinarily File menu) and WindowMenu regarded as container's own menu.
//		Some components can't update toolbar button and statusbar pane.
HWND CSkinMenuBar::OleMenuDescriptor(BOOL& bSend, UINT nMsg, WPARAM wParam, LPARAM lParam)
{
	/*
	CWnd* pOleWnd = GetCmdSentOleWnd();
	if (pOleWnd == NULL)
		return NULL;
	HWND hWndSentCmd = NULL;

	HMENU hMenu = NULL;
	if (nMsg == WM_INITMENUPOPUP || nMsg == WM_INITMENU)
		hMenu = (HMENU)wParam;
	else if (nMsg == WM_MENUSELECT)
		hMenu = (HMENU)lParam;

	switch (nMsg) {
	case WM_INITMENUPOPUP:
	case WM_INITMENU:
	case WM_MENUSELECT:
		bSend = TRUE;
		if (m_nTrackingState == popup) {
			LTRACE2(_T("    now popup\n"));
			if (m_bMDIApp) {
				LTRACE2(_T("    this is MDI\n"));
				if (m_nCurIndex == 0 || m_nCurIndex == 1 || hMenu == m_hWindowMenu) {
					LTRACE2(_T("    it's container menu, send to frame\n"));
					return NULL;
				}
			}
			else {
				LTRACE2(_T("    it's container menu, send to frame\n"));
				if (m_nCurIndex == 0) {
					return NULL;
				}
			}
			LTRACE2(_T("    it's server menu, send to server\n"));
			return pOleWnd->GetSafeHwnd();
		}
		break;

	case WM_COMMAND:
		bSend = FALSE;
		if (m_bMDIApp) {
			LTRACE2(_T("    this is MDI\n"));
			if (_nPrevIndexForCmd == 0 || _nPrevIndexForCmd == 1 || _bWindowMenuSendCmd) {
				LTRACE2(_T("    it's container menu, send to frame\n"));
				return NULL;
			}
		}
		else {
			if (_nPrevIndexForCmd == 0) {
				LTRACE2(_T("    it's container menu, send to frame\n"));
				return NULL;
			}
		}
		LTRACE2(_T("    it's server menu, send to server\n"));
		return pOleWnd->GetSafeHwnd();
	}
	*/
	return NULL;// send to frame
}

CWnd* CSkinMenuBar::GetCmdSentOleWnd()
{
	// *****fixed by VORGA, thanks!*****
	/*
	CWnd* pWnd = AfxGetMainWnd();
	if (pWnd == NULL || !pWnd->IsFrameWnd())
		return NULL;

	CFrameWnd* pFrame = NULL;
	if (m_bMDIApp) {
		CMDIFrameWnd *pMDIFrame = STATIC_DOWNCAST(CMDIFrameWnd, pWnd);
		if (pMDIFrame == NULL)
			return NULL;
		pFrame = pMDIFrame->GetActiveFrame();
	}
	else {
		pFrame = STATIC_DOWNCAST(CFrameWnd, pWnd);
	}

	if (pFrame == NULL)
		return NULL;

	CDocument* pDoc = pFrame->GetActiveDocument();
	if (pDoc != NULL && pDoc->IsKindOf(RUNTIME_CLASS(COleDocument))) {
		COleDocument* pOleDoc = STATIC_DOWNCAST(COleDocument, pDoc);
		ASSERT_VALID(pOleDoc);
		COleClientItem* pClientItem = pOleDoc->GetInPlaceActiveItem(pFrame);
		CWnd* pWnd = (pClientItem == NULL) ? NULL : pClientItem->GetInPlaceWindow();
		if (pWnd != NULL) {
			//			TCHAR str[256];
			//			::GetClassName(pWnd->GetSafeHwnd(), str, 256);
			//			LTRACE2(_T("    server wnd : %s\n"), str);
			return pWnd;
		}
	}
	*/
	return NULL;
}

/////////////////////////////////////////////////////////////////////////////
// CMDIClientHook 
CMDIClientHook::CMDIClientHook()
{
	m_pMenuBar = NULL;
}

BOOL CMDIClientHook::Install(CSkinMenuBar* pMenuBar, HWND hWndToHook)
{
	ASSERT(pMenuBar);
	ASSERT(m_pMenuBar == NULL);
	m_pMenuBar = pMenuBar;
	return HookWindow(hWndToHook);
}

CMDIClientHook::~CMDIClientHook()
{
}

LRESULT CMDIClientHook::WindowProc(UINT nMsg, WPARAM wParam, LPARAM lParam)
{
	ASSERT(m_pMenuBar);

	switch (nMsg)
	{
	case WM_MDISETMENU:			// only sent to MDI client window
		// Setting new frame/window menu: bypass MDI. wParam is new menu.
		if (wParam) {
			//LTRACE(_T("CXPMenuBar::WM_MDISETMENU 0x%04x\n"), wParam);
			m_pMenuBar->OnSetMenu((HMENU)wParam, (HMENU)lParam);
		}
		return 0;

	case WM_MDIREFRESHMENU:		// only sent to MDI client window
		// Normally, would call DrawMenuBar, but I have the menu, so eat it.
		//LTRACE(_T("CXPMenuBar::WM_MDIREFRESHMENU\n"));
		return 0;

		//	case WM_PAINT:
		//LTRACE(_T("CXPMenuBar:WM_PAINT\n"));
		// If you use no Doc-View project, when child window maximized,
		// WM_PAINT never sent! so this trick never do.
		// So this code moved to OnUpdateCmdUI.

		// After changing the MDI maximized state, the client window gets a
		// paint message. This is the most convenient place to find out; there
		// is no WM_MDIMAXIMIZED message.
		//		if (/*m_pWndHooked->*/m_hWnd == m_pMenuBar->m_hWndMDIClient)
		//			m_pMenuBar->CheckActiveChildWndMaximized();
		//		break;
	}

	return CSubclassWnd::WindowProc(NULL, nMsg, wParam, lParam);
}

/////////////////////////////////////////////////////////////////////////////
// CMainFrameHook 
CMainFrameHook::CMainFrameHook()
{
	m_pMenuBar = NULL;
}

BOOL CMainFrameHook::Install(CSkinMenuBar* pMenuBar, HWND hWndToHook)
{
	ASSERT(pMenuBar);
	ASSERT(m_pMenuBar == NULL);
	m_pMenuBar = pMenuBar;
	return HookWindow(hWndToHook);
}

CMainFrameHook::~CMainFrameHook()
{
}

LRESULT CMainFrameHook::WindowProc(UINT nMsg, WPARAM wParam, LPARAM lParam)
{
	ASSERT(m_pMenuBar);
	// be care for other windows(MainFrame) hooking
	// possible called when already this wnd destroyed (WM_NCACTIVATE is)
	if (!::IsWindow(m_pMenuBar->GetSafeHwnd())) 
	{
		return CSubclassWnd::WindowProc( NULL, nMsg, wParam, lParam);
	}

	BOOL bSend = FALSE;
	if (HWND hWndServer = m_pMenuBar->OleMenuDescriptor(bSend, nMsg, wParam, lParam)) 
	{
		// OLE wnd will handle message
		if (bSend)
			return ::SendMessage(hWndServer, nMsg, wParam, lParam);
		else
			return ::PostMessage(hWndServer, nMsg, wParam, lParam);
	}

	switch (nMsg) 
	{
	case WM_MENUSELECT:
		m_pMenuBar->OnMenuSelect((HMENU)lParam, (UINT)LOWORD(wParam));
		break;

	case WM_INITMENUPOPUP:
		if (!HIWORD(lParam) && (HMENU)wParam == m_pMenuBar->m_hWindowMenu)
			m_pMenuBar->OnInitMenuPopup(CMenu::FromHandle((HMENU)wParam),
			LOWORD(lParam), (BOOL)HIWORD(lParam));
		break;

	case WM_NCACTIVATE:
		m_pMenuBar->OnFrameNcActivate((BOOL)wParam);
		break;

	case WM_SYSCOLORCHANGE:
	case WM_SETTINGCHANGE:
		TRACE(_T("CXPMenuBar::WM_SETTINGCHANGE\n"));
		// It's enough to reinitialize common resources once.
		m_pMenuBar->OnSettingChange(wParam, lParam);
		break;
	}

	//if (nMsg == CXPMenuBar::WM_GETMENU)
	//	return (LRESULT)m_pMenuBar->m_hMenu;

	return CSubclassWnd::WindowProc(NULL, nMsg, wParam, lParam);
}

CSkinMenuBar::CSkinMenuBar()
{
	m_hMenu = 0;
	m_bLoop = FALSE;

	m_bProcessLeftArrow		= TRUE;
	m_bProcessRightArrow	= TRUE;
	//m_pselectedMenuItem		= NULL;
	m_pframe_state			= NULL;

	m_hWndMDIClient			= 0;
	m_hWndActiveChild		= 0;

	m_pMenuIcon				= NULL;
	m_hWindowMenu			= 0;
	m_bMDIApp				= FALSE;
	m_pMenuControl			= NULL;
	m_nTrackingState		= none;
	m_nCurIndex				= -1;
	m_bIgnoreAlt			= FALSE;
	m_bMDIMaximized			= FALSE;

	_InitCommonResources();
}

/////////////////////////////////////////////////////////////////////////////
// CXPMenuBar system hook
void CSkinMenuBar::OnMenuSelect(HMENU hMenu, UINT nIndex)
{
	//	LTRACE(_T("CXPMenuBar::OnMenuSelect\n"));
	if (m_nTrackingState == popup) 
	{
		m_bProcessRightArrow = (::GetSubMenu(hMenu, nIndex) == NULL);
		HMENU hSubMenu = ::GetSubMenu(hMenu, m_nCurIndex);
		if (hSubMenu == NULL)
			return;
		m_bProcessLeftArrow = (hMenu == hSubMenu);
	}
}


LRESULT CSkinMenuBar::InitKeyAccess()
{
	g_hKeyHook = ::SetWindowsHookEx(WH_KEYBOARD,
		KeyInputFilter, NULL, GetCurrentThreadId());// m_bLoop may become TRUE
	return 0;
}

LRESULT CSkinMenuBar::UnInitKeyAccess()
{
	::UnhookWindowsHookEx (g_hKeyHook);
	g_hKeyHook = 0;
	return 0;
}

LRESULT CALLBACK CSkinMenuBar::KeyInputFilter(int code, WPARAM wParam, LPARAM lParam)
{
	/*
	CSkinMenuBar* pMenuBar = GetActiveMenuBar();
	BOOL bKeyUp = (GetAsyncKeyState(MK_LBUTTON) & 0x8000);
	if ( code == HC_ACTION && g_hKeyHook && pMenuBar )
	{
		BOOL bAlt = HIWORD(lParam) & KF_ALTDOWN;	// Alt key presed?
		TCHAR vkey = wParam;						// + X key
		if (vkey == VK_MENU ||
			(vkey == VK_F10 && !((GetKeyState(VK_SHIFT) & 0x80000000) ||
			(GetKeyState(VK_CONTROL) & 0x80000000) || bAlt)))
		{
				// only alt key pressed
			if ( bKeyUp )
			{
				switch (pMenuBar->m_nTrackingState) 
				{
					case none:
						{
							if ( pMenuBar->m_bIgnoreAlt == TRUE )
							{
								//						LTRACE(_T("    ignore ALT key up\n"));
								pMenuBar->m_bIgnoreAlt = FALSE;
								break;
							}
							if (pMenuBar->m_bMDIApp) 
							{
								pMenuBar->UpdateBar(button, pMenuBar->GetNextOrPrevButton(0, FALSE));
							}
							else 
							{
								pMenuBar->UpdateBar(button, 0);
							}
							break;
						}
					
					case button:
						pMenuBar->UpdateBar();
						break;
					case buttonmouse:
						break;	// do nothing
				}
			}
				return TRUE;
		}
		
		//else if ((nMsg == WM_SYSKEYDOWN || nMsg == WM_KEYDOWN)) {
		else if ( !bKeyUp )
		{
			if ( pMenuBar->m_nTrackingState == button )
			{
				
				switch (vkey)
				{
				case VK_LEFT:
				case VK_RIGHT:
					{
						int nNewIndex  = pMenuBar->GetNextOrPrevButton( pMenuBar->m_nCurIndex, vkey == VK_LEFT);
						pMenuBar->UpdateBar(button, nNewIndex);
						return TRUE;
					}
				case VK_SPACE:
				case VK_UP:
				case VK_DOWN:
					pMenuBar->TrackPopup( pMenuBar->m_nCurIndex );
					return TRUE;

				case VK_ESCAPE:
					pMenuBar->UpdateBar();
					return TRUE;
				}
			
			}

			// Alt + X pressed
			if ((bAlt || pMenuBar->m_nTrackingState == button) && _istalnum(vkey)) 
			{
				int nIndex;
				if ( pMenuBar->MapAccessKey(vkey, nIndex) == TRUE )
				{
					pMenuBar->UpdateBar();
					pMenuBar->TrackPopup(nIndex);
					return TRUE;		// eat it!
				}
				else if (pMenuBar->m_nTrackingState==button && !bAlt) 
				{
					//					MessageBeep(0);		// if you want
					return TRUE;
				}
			}

			if ( pMenuBar->m_nTrackingState > 0 )
			{	// unknown key
				if ( pMenuBar->m_nTrackingState != buttonmouse )
				{	// if tracked with mouse, don't update bar
					pMenuBar->UpdateBar();
				}
			}
		}
	}

	*/
	BOOL bAlt = HIWORD(lParam) & KF_ALTDOWN;	// Alt key presed?
	if ( bAlt && (wParam > 0) && (VK_MENU != wParam) )
	{
		BOOL bKeyUp = (GetAsyncKeyState(MK_LBUTTON) & 0x8000);
		if ( !bKeyUp )
		{
			BOOL bShift = (GetAsyncKeyState(MK_SHIFT) & 0x8000);
			BOOL bCtrl = (GetAsyncKeyState(MK_CONTROL)& 0x8000);
			BOOL bFKey = ((wParam >= VK_F1) && (wParam <= VK_F12));
			BOOL bEscape = (wParam == VK_ESCAPE );
			BOOL bDelete = (wParam == VK_DELETE );

			if  ( !(bCtrl || bFKey || bEscape || bDelete) ) 
			{
				int nIndex = -1;
				// 查找那个MenuBar合适
				CSkinMenuBar* pMenuBar = GetActiveMenuBar();
				if ( pMenuBar )
				{
					BOOL bRet = pMenuBar->MapAccessKey( (TCHAR)wParam, nIndex );
					if ( bRet )
					{
						//m_nCurIndex = nIndex;

						//WTL::CWindowDC dc( pMenuBar->m_hWnd );
						//pMenuBar->EtchedMenuBarItem( (HDC)dc, pMenuBar->GetMenu(), nIndex, MS_SELECTED);
						pMenuBar->UpdateBar(button, nIndex);
						pMenuBar->TrackPopup(nIndex);
						return 0;
					}
				}
			}

		}
	}

	return CallNextHookEx(g_hKeyHook, code, wParam, lParam);
}


LRESULT CALLBACK CSkinMenuBar::MenuInputFilter(int code, WPARAM wParam, LPARAM lParam)
{
	return (
		code == MSGF_MENU &&
		g_pMenuBar && 
		g_pMenuBar->OnMenuInput( *((MSG*)lParam) )
		) ? TRUE : CallNextHookEx(g_hMsgHook, code, wParam, lParam);
}
/*
void CSkinMenuBar::InitMenu( HWND hWnd, CMenuHandle menu, int nFrameLeft, int nCaptionHeight )
{
	//得到Menu的各个菜单

	//g_pMenuBar = this;

	CPoint pt;
	int border_left_width = nFrameLeft;

	pt.x = border_left_width;
	pt.y = nCaptionHeight;

	WTL::CRect rcClient;
	::GetClientRect( hWnd, rcClient );

	m_MenuRectLst.clear();
	m_AccessKeyLst.clear();

	for ( int i = 0; i < menu.GetMenuItemCount(); ++i )
	{
		const int string_size = menu.GetMenuString(i, 0, 0, MF_BYPOSITION);

		WTL::CRect rcMenu(0, 0, 0, 0);
		char cAccessKey = 0;

		if (string_size > 0)
		{
			char* sz = new char[string_size + 1];
			int n = menu.GetMenuString(i, sz, string_size  + 1, MF_BYPOSITION);
			ASSERT(n == string_size);
			if (n)
			{
				char *pdest;
				pdest = strstr( sz, "&" );
				if ( pdest )
				{
					int  result;
					result = (int)(pdest - sz + 1);
					if ( result < n )
					{
						cAccessKey = sz[result];
					}
				}

				WTL::CRect rcText = _CalcTextRect( sz, GetCtrlFont(hWnd) );	

				rcMenu.left = pt.x;
				rcMenu.right = pt.x + rcText.Width() + 2 * MENU_LEFT;
				rcMenu.top = pt.y;
				rcMenu.bottom = pt.y + rcText.Height() + 2 * MENU_TOP;
				

				//判断一下是否要换行
				if ( rcMenu.right > rcClient.Width() && pt.x >= border_left_width )
				{
					pt.x = border_left_width;
					pt.y = rcMenu.bottom;
					rcMenu.left = pt.x;
					rcMenu.right = pt.x + rcText.Width() + 2 * MENU_LEFT;
					rcMenu.top = pt.y;
					rcMenu.bottom = pt.y + rcText.Height() + 2 * MENU_TOP;

					pt.x = rcMenu.right;
				}
				else
				{
					pt.x = rcMenu.right;
				}

				m_MenuRectLst.push_back( rcMenu );



			}
			else
				m_MenuRectLst.push_back( rcMenu );
			delete [] sz;
		}
		else
		{
			m_MenuRectLst.push_back( rcMenu );
		}

		m_AccessKeyLst.push_back( cAccessKey );
	}


}

*/
void CSkinMenuBar::TrackPopup( int nIndex )
{
	m_bLoop = TRUE;
	m_nCurIndex = nIndex;

	while ( m_bLoop )
	{
		//UpdateWindow();	// force to repaint when button hidden by other window
		UpdateBar(popup, m_nCurIndex);

		// install hook
		ASSERT(g_pMenuBar == NULL);


		//SkinFrame
		g_pMenuBar = this;
		//ASSERT(g_hMsgHook == NULL);

		m_bLoop = FALSE;
		g_hMsgHook = ::SetWindowsHookEx(WH_MSGFILTER,
			MenuInputFilter, NULL, GetCurrentThreadId());// m_bLoop may become TRUE

		// popup!!
		m_nTrackingState = popup;
		_nPrevIndexForCmd = m_nCurIndex;
		m_MenuLst[m_nCurIndex]->TrackPopup( this, m_hWnd );

		// uninstall hook
		::UnhookWindowsHookEx(g_hMsgHook);
		g_hMsgHook = NULL;
		g_pMenuBar = NULL;
	}

	UpdateBar();
}
void CSkinMenuBar::GetClientRect(CRect& rc)
{

}
BOOL CSkinMenuBar::IsValidIndex(int nIndex) 
{
	return 0 <= nIndex && nIndex < GetItemCount(); 
}

void CSkinMenuBar::DrawMenuBar(CDCHandle dc, CMenuHandle menu, const WTL::CRect& rcw, int iSpecialItem, MENUSTATES ms)
{
	ASSERT(!menu.IsNull());
	ASSERT(!dc.IsNull());
	//ASSERT(::IsMenu(menu.m_hMenu));

	if ( !::IsMenu(menu.m_hMenu) )
		return;

	WTL::CRect rcmb = CalcMenuBarRect(rcw, *m_pframe_state);

	int r = dc.SaveDC();

	// xp only
	//        BOOL flat_menu = FALSE;
	//        SystemParametersInfo(SPI_GETFLATMENU, 0, &flat_menu, 0);

	// bar background
	COLORREF cr =  GetSchemeColor(WP_MENUBAR, 0, TMT_MENUBAR);
	CBrush br;

	br.CreateSolidBrush(cr);
	dc.FillRect(&rcmb, br);
	// TraceRect("calc: ", &rc);

	int c = menu.GetMenuItemCount();
	//        MENUBARINFO barinfo = {0};
	//        barinfo.cbSize = sizeof(MENUBARINFO);

	// bar item
	for ( int i = 0; i < c; ++i )
	{
		RECT rcmi = CalcMenuItemRect(i);

		// TRACE("item %d fFoucs %d", i, barinfo.fFocused);    TraceRect("",&barinfo.rcBar);
		MENUSTATES ms_real = MS_NORMAL;
		if (i - 1 == iSpecialItem)
			ms_real = ms;
		DrawMenuBarItem(dc, menu, rcmi, i, ms_real);
	}

	// release
	dc.RestoreDC(r);
}




// Not Used Now.
// 在 MemDC 中绘制 Bar，还包括 Menu 的状态: hover, click, leave
void CSkinMenuBar::EtchedMenuBar( CDCHandle dc, const CMenuHandle menu, const WTL::CRect& rcw, int iSpecialItem, MENUSTATES ms )
{
	WTL::CDC dcm;
	dcm.CreateCompatibleDC(dc);

	int r = dcm.SaveDC();

	WTL::CRect rcbar = CalcMenuBarRect(rcw, *m_pframe_state);

	WTL::CBitmap bg;
	bg.CreateCompatibleBitmap(dc, rcbar.Width(), rcbar.Height());
	dcm.SelectBitmap(bg);

	DrawMenuBar((HDC)dcm, menu, rcw, iSpecialItem, ms);

	dc.BitBlt(rcbar.left, rcbar.top, rcbar.Width(), rcbar.Height(), 
		dcm, rcbar.left, rcbar.top, SRCCOPY);
	dcm.RestoreDC(r);
}

// 1 创建MemoryDC
// 2 计算nItemIndex所在位置
// 3 调用 DrawMenuBarItem 绘制
void CSkinMenuBar::EtchedMenuBarItem(CDCHandle dc, CMenuHandle menu, UINT nItemIndex, MENUSTATES ms)
{
	// bar item rect
	WTL::CRect rcw;
	::GetWindowRect(m_hWnd, &rcw);
	rcw.OffsetRect(-rcw.left, -rcw.top);

	WTL::CRect rcmb = CalcMenuBarRect(rcw, *m_pframe_state);

	WTL::CRect rcmi = CalcMenuItemRect(nItemIndex);

	// Memory DC
	WTL::CDC dcm;
	dcm.CreateCompatibleDC(dc);

	int r = dcm.SaveDC();

	WTL::CBitmap bg;
	bg.CreateCompatibleBitmap(dc, rcw.Width(), rcw.Height());
	dcm.SelectBitmap(bg);

	DrawMenuBarItem((HDC)dcm, menu, rcmi, nItemIndex, ms);

#if 0
	HDC dct = ::GetDC(0);
	BitBlt(dct, 0, 0, rcmi.Width(), rcmi.Height(), dcm, 0, 0, SRCCOPY);
	::ReleaseDC(0, dct);
#endif

	dc.BitBlt(rcmi.left, rcmi.top, rcmi.Width(), rcmi.Height(), 
		dcm, rcmi.left, rcmi.top, SRCCOPY);

	dcm.RestoreDC(r);
}

void CSkinMenuBar::DrawMenuBarItem(CDCHandle dc, CMenuHandle menu, const WTL::CRect& rcItem, UINT nItemIndex, MENUSTATES ms)
{
	//        static bool pause = false;
	//        if (pause && ms == MS_NORMAL)
	//        {
	//            // __asm int 3;
	//            pause = false;
	//        }
	//        if (ms == MS_DEMOTED)
	//            pause = true;
	//        
	//        TRACE("DrawMenuItem: %d %x, selectted:%d\n", nItemIndex, ms, _selectedMenuItem);

	ASSERT(nItemIndex >= 0);
	int sd = dc.SaveDC();

	dc.SetBkMode(TRANSPARENT);

	CPen penBorder;
	// TODO: ? MS_NORMAL 状态使用bar的背景，没有边框
	int iProp = ((ms == MS_NORMAL) ? TMT_MENU_BORDER : TMT_MENUBORDER_HILIGHT);
	COLORREF cr = GetSchemeColor(WP_MENUBAR, 0, iProp);
	penBorder.CreatePen(PS_SOLID, 1, cr);
	dc.SelectPen(penBorder);

	iProp = ((ms == MS_NORMAL) ? TMT_MENU: TMT_MENUHILIGHT);
	CBrush brHilight;
	brHilight.CreateSolidBrush(GetSchemeColor(WP_MENUBAR, 0, iProp));
	dc.SelectBrush(brHilight);

	dc.Rectangle(rcItem);

	// font
	/*
	CFont fontMenu;
	LOGFONT logFontMenu;

	NONCLIENTMETRICS nm = {0};
	nm.cbSize = sizeof (NONCLIENTMETRICS);
	SystemParametersInfo(SPI_GETNONCLIENTMETRICS, nm.cbSize, &nm, 0);
	logFontMenu = nm.lfMenuFont;

	fontMenu.CreateFontIndirect(&logFontMenu);
	dc.SelectFont(fontMenu);
	*/
	dc.SelectFont( GetCtrlFont( m_hWnd ) );

	dc.SetTextColor(0); // 黑色字体, TODO:

	const int string_size = menu.GetMenuString(nItemIndex, 0, 0, MF_BYPOSITION);
	if (string_size > 0)
	{
		char* sz = new char[string_size + 1];
		int n = menu.GetMenuString(nItemIndex, sz, string_size  + 1, MF_BYPOSITION);
		ASSERT(n == string_size);
		if (n)
			dc.DrawText(sz, n, (LPRECT)&rcItem, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
		delete [] sz;
	}

	// TODO: OwnerDraw
	if (false)
	{
		MENUITEMINFO menuInfo = {0};
		menuInfo.cbSize = sizeof(menuInfo);
		menuInfo.fMask = MIIM_DATA|MIIM_TYPE|MIIM_ID;
		if (menu.GetMenuItemInfo(nItemIndex, TRUE, &menuInfo))
		{
			//                if(menuInfo.fType&MF_OWNERDRAW)
			//                {
			//                    DebugBreak();
			//                }
		}
	}

	dc.RestoreDC(sd);
}

void CSkinMenuBar::OnSetMenu(HMENU hNewMenu, HMENU hWindowMenu)
{
	ASSERT(m_pMenuIcon );
	// We can get active MDI child window on this message!
	BOOL bMax = FALSE;
	HWND hWndChild = GetActiveChildWnd(bMax);

	if (!m_hWndActiveChild || m_hWndActiveChild != hWndChild) 
	{// an active child window changed
		TRACE(_T("CXPMenuBar::OnSetMenu---an active child window changed\n"));
		m_hWndActiveChild = hWndChild;
		// tell MenuIcon child window has been changed
		m_pMenuIcon->OnActivateChildWnd();
	}

	if (!m_hMenu || m_hMenu != hNewMenu) 
	{	// menu changed
		TRACE(_T("CXPMenuBar::OnSetMenu---menu changed\n"));
		LoadMenu(hNewMenu, hWindowMenu);	// set menubar menu
		::SetMenu(m_hWnd, NULL);			// clear frame menu
	}
}

HMENU CSkinMenuBar::LoadMenu(HMENU hMenu, HMENU hWindowMenu)
{
	ASSERT(::IsMenu(hMenu));
	ASSERT(this);

	//CFrameWnd* pFrame = GetParentFrame();
	if (::GetMenu( m_hWnd ) != NULL) {
		// not to make MFC ignore SetMenu(NULL), post it.
		::SetMenu(m_hWnd, NULL);			// clear frame menu
		//PostMessage(MB_SET_MENU_NULL, (WPARAM)pFrame->GetSafeHwnd());
	}

	HMENU hOldMenu = m_hMenu;
	m_hMenu = hMenu;// menu is shared with MFC

	// initialize Items 
	VERIFY(InitItems());

	if (hMenu) 
	{
		m_hWindowMenu = hWindowMenu;
		//RefreshBar();// and menubar itself
	}

	return hOldMenu;
}


void CSkinMenuBar::OnFrameNcActivate(BOOL bActive) 
{
	if (FS_ACTIVE == *m_pframe_state)
		bActive = TRUE;
	if (!::IsWindowEnabled(m_hWnd))
		bActive = FALSE;
	/*
	if (bActive == m_bFrameActive)
		return;

	if (!bActive) {
		for (int i = 0; i < GetItemCount(); ++i) 
		{
			m_arrItem[i]->ModifyState(0, MISTATE_INACTIVE);
		}
	}
	else
	{
		for (int i = 0; i < GetItemCount(); ++i) 
		{
			m_arrItem[i]->ModifyState(MISTATE_INACTIVE, 0);
		}
	}

	m_bFrameActive = bActive;

	// InvalidateRect(NULL); is better, but too late
	// while clicking the application title bar (like IE5)
	// so we have to redraw now!
	CClientDC dc(this);
	DoPaint(&dc);
	*/
}

LRESULT CSkinMenuBar::OnSettingChange(WPARAM wParam, LPARAM lParam)
{
	// reinitialize fonts etc
	if (!_InitCommonResources(TRUE)) 
	{
		TRACE(_T("Failed to create bar resource\n"));
		return FALSE;
	}

	VERIFY(InitItems());

	//CFrameWnd* pFrame = GetParentFrame();
	//ASSERT_VALID(pFrame);
	//pFrame->RecalcLayout();
	return 0;
}

void CSkinMenuBar::DeleteItems()
{
	for ( int i = 0; i < m_MenuLst.size(); i++ )
	{
		delete m_MenuLst[i];
	}

	m_MenuLst.clear();
	
	m_pMenuIcon = NULL;
	m_pMenuControl = NULL;;
}

BOOL CSkinMenuBar::InitItems()
{
	ASSERT(m_hMenu);

	// clean up all items
	DeleteItems();

	if ( m_bMDIApp )
	{
		m_pMenuIcon = new CMenuIcon(this);
		m_MenuLst.push_back( m_pMenuIcon );
	}
	// buttons
	for (int i = 0; i < ::GetMenuItemCount(m_hMenu); ++i) 
	{
		m_MenuLst.push_back(new CMenuButton(m_hMenu, i));
		//m_arrItem.Add(new CMenuButton(m_hMenu, i));
	}

	if (m_bMDIApp)
	{
		// icon
		
		// frame control
		m_pMenuControl = new CMenuControl(this);
		m_MenuLst.push_back(m_pMenuControl);

		// reinitializing
		m_pMenuIcon->OnActivateChildWnd();
		m_pMenuControl->OnActivateChildWnd();
	}

	CalcLayout();

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CXPMenuBar layout
int CSkinMenuBar::GetClipBoxLength()
{
	CRect rcFrame; 
	::GetWindowRect( m_hWnd, rcFrame );

	int border_left_width = GetSchemeWidth(WP_FRAMELEFT, CS_ACTIVE);	
	int border_right_width = GetSchemeWidth(WP_FRAMERIGHT, CS_ACTIVE);

	//const int cxFrameBorder = ::GetSystemMetrics(SM_CXFRAME);
	int cxNonClient = border_left_width + border_right_width;
	return rcFrame.Width() - cxNonClient;
}

void CSkinMenuBar::SizeMenuBar(int nLength, BOOL bVert)
{
	//LTRACE("CXPMenuBar::SizeMenuBar\n");
	int nCount = GetItemCount(); ASSERT(nCount > 0);
	//LTRACE("    just each one wrapping\n");
	WrapMenuBar(nCount, nLength);
		
	//LTRACE("out SizeMenuBar\n");
}

// set m_bWrapped by nWidth
int CSkinMenuBar::WrapMenuBar(int nCount, int nWidth)
{
	//	LTRACE(_T("CXPMenuBar::WrapMenuBar\n"));
	int nResult = 0;
	int x = 0;

	for (int i = 0; i < nCount; ++i) 
	{
		CXPMenuItem* pItem = m_MenuLst[i];
		if ( i+1 == nCount )
			return ++nResult;

		if (x + pItem->GetHorizontalSize().cx> nWidth)
		{// itself is over
			if (pItem->GetStyle() & MISTYLE_WRAPPABLE) 
			{
				pItem->ModifyState(0, MISTATE_WRAP);
				++nResult;
				x = 0;
			}
		}
		else if (x + pItem->GetHorizontalSize().cx + 
			m_MenuLst[i+1]->GetHorizontalSize().cx> nWidth)
		{
			if (pItem->GetStyle() & MISTYLE_WRAPPABLE)
			{
				pItem->ModifyState(0, MISTATE_WRAP);
				++nResult;
				x = 0;
			}
		}
		else
		{
			pItem->ModifyState(MISTATE_WRAP, 0);
			x += pItem->GetHorizontalSize().cx;
		}
	}

	return nResult + 1;
}

CSize CSkinMenuBar::CalcLayout()
{
	ASSERT(this);
	ASSERT(::IsWindow(m_hWnd));

	int nCount = GetItemCount();

	CSize sizeResult(0, 0);

	if (nCount > 0)
	{
		SizeMenuBar(GetClipBoxLength());
		CalcItemLayout(nCount, FALSE);// added
		sizeResult = CalcSize(nCount);
		sizeResult.cx = GetClipBoxLength();
	}
/*
	//BLOCK: Adjust Margins
	{
		CRect rect; rect.SetRectEmpty();
		_CalcInsideRect(rect, (dwMode & LM_HORZ));
		sizeResult.cy -= rect.Height();
		sizeResult.cx -= rect.Width();

		CSize size = CControlBar::CalcFixedLayout((dwMode & LM_STRETCH), (dwMode & LM_HORZ));
		sizeResult.cx = max(sizeResult.cx, size.cx);
		sizeResult.cy = max(sizeResult.cy, size.cy);
	}
*/
	return sizeResult;
}

// calc only size, by using m_bWrapped
CSize CSkinMenuBar::CalcSize(int nCount)
{
	ASSERT(nCount > 0);
	CPoint cur(0,0);
	CSize sizeResult(0,0);
	int nWrap = 0;
	for (int i = 0; i < nCount; ++i) 
	{
		CXPMenuItem* pItem = m_MenuLst[i];
		sizeResult.cx = max(cur.x + pItem->GetHorizontalSize().cx, sizeResult.cx);
		sizeResult.cy = max(cur.y + pItem->GetHorizontalSize().cy, sizeResult.cy);

		cur.x += pItem->GetHorizontalSize().cx;

		if (pItem->GetState() & MISTATE_WRAP) {
			//LTRACE(_T("    nIndex:%d is wrapped\n"), i);
			cur.x = 0;// reset x pos
			cur.y += pItem->GetHorizontalSize().cy;
			++nWrap;
		}
	}

	return sizeResult;
}

void CSkinMenuBar::CalcItemLayout(int nCount, BOOL bVert)
{
	ASSERT(nCount > 0);
	//int x = 0;
	//int y = 0;

	int x = GetSchemeWidth(WP_FRAMELEFT, CS_ACTIVE);
	int y = GetSchemeHeight(WP_CAPTION, CS_ACTIVE);

	if (!bVert) 
	{
		for (int i = 0; i < nCount; ++i) 
		{
			CXPMenuItem* pItem = m_MenuLst[i];
			CPoint ptItem(x, y);
			pItem->Layout(ptItem, TRUE);// layout by itself!
			if (pItem->GetState() & MISTATE_WRAP) 
			{
				x = GetSchemeWidth(WP_FRAMELEFT, CS_ACTIVE);// reset x to 0
				y += pItem->GetRect().Height();
			}
			else
				x += pItem->GetRect().Width();
		}		
	}
	else
	{
		for (int i = 0; i < nCount; ++i) 
		{
			CXPMenuItem* pItem = m_MenuLst[i];
			CPoint	ptItem(0, y);
			pItem->Layout(ptItem, FALSE);	// layout by itself
			y += pItem->GetRect().Height();
		}
	}
}

void CSkinMenuBar::OnInitMenuPopup(CMenu* pPopupMenu, UINT nIndex, BOOL bSysMenu)
{
	TRACE(_T("CXPMenuBar::OnInitMenuPopup\n"));
	CMenu menu;
	menu.Attach((HMENU)m_hWindowMenu);

	// scan for first window command
	int n = menu.GetMenuItemCount();
	BOOL bAddSeperator = TRUE;
	for (int iPos=0; iPos<n; iPos++) 
	{
		if (menu.GetMenuItemID(iPos) >= AFX_IDM_FIRST_MDICHILD)
		{
			bAddSeperator = FALSE;
			break;
		}
	}

	// iPos is either first window item, or end if none found.

	// delete everything after.
	while (iPos < (int)menu.GetMenuItemCount())
		menu.RemoveMenu(iPos, MF_BYPOSITION);

	// get active window so I can check its menu item
	ASSERT(m_hWndMDIClient);
	HWND hwndActive = (HWND)::SendMessage(m_hWndMDIClient,
		WM_MDIGETACTIVE, 0, NULL);

	// append window names in the form "# title"
	// *****fixed by VORGA, thanks!*****
	int iWin;
	int nID = AFX_IDM_FIRST_MDICHILD;
	CString sWinName, sMenuItem;
	HWND hwnd;
	for (iWin = 1; iWin <= 10; iWin++, nID++)
	{
		hwnd = ::GetDlgItem(m_hWndMDIClient, nID);
		if (hwnd == NULL)
			break;

		if (bAddSeperator)
		{
			menu.InsertMenu(iPos++, MF_BYPOSITION | MF_SEPARATOR);
			bAddSeperator = FALSE;
		}

		if (iWin < 10)
		{
			CWnd::FromHandle(hwnd)->GetWindowText(sWinName);
			sMenuItem.Format(_T("&%d %s"), iWin, (LPCTSTR)sWinName);
			menu.InsertMenu(iPos, MF_BYPOSITION, nID, sMenuItem);
			if (hwnd == hwndActive)
				menu.CheckMenuItem(iPos, MF_BYPOSITION | MF_CHECKED);
		}
		else
		{
			menu.InsertMenu(iPos, MF_BYPOSITION, nID, _szMoreWindows);
		}
		iPos++;
	}

	menu.Detach();
}


void CSkinMenuBar::UpdateBar(TrackingState nState, int nNewIndex)
{
	if (m_nTrackingState == buttonmouse)	
		m_bIgnoreAlt = FALSE;// if prev state is BUTTONMOUSE, always should be FALSE!

	m_nTrackingState = nState;

#ifdef _DEBUG
	//	static LPCTSTR lpszStates[] = { _T("NONE"), _T("BUTTON"), _T("POPUP"), _T("BUTTONMOUSE") };
	//	LTRACE(_T("CXPMenuBar::UpdateBar state to %s, button=%d\n"),
	//		lpszStates[nState], nNewIndex);
#endif

	// clean up
	if (IsValidIndex(m_nCurIndex)) 
	{
		CXPMenuItem* pItem = m_MenuLst[m_nCurIndex];
		WTL::CWindowDC dc( m_hWnd );
		pItem->ModifyState(MISTATE_HOT | MISTATE_PRESSED, 0);
		pItem->Update(dc.m_hDC, this );

		// draw captions forcefully
		if (m_pMenuControl) 
		{
			CRect rcCross = m_pMenuControl->GetRect() & m_MenuLst[m_nCurIndex]->GetRect();
			if (!rcCross.IsRectEmpty()) 
			{
				m_pMenuControl->ForceDrawControl(dc.m_hDC);
			}
		}

		m_nCurIndex = -1;
	}

	if (nState != none) 
	{
		ASSERT(IsValidIndex(nNewIndex));
		m_nCurIndex = nNewIndex;
		CXPMenuItem* pItem = m_MenuLst[m_nCurIndex];
		WTL::CWindowDC dc( m_hWnd );

		if (nState == button || nState == buttonmouse) 
		{
			pItem->ModifyState(MISTATE_PRESSED, MISTATE_HOT);
			pItem->Update(dc.m_hDC, this);
		}
		else if (nState == popup) 
		{
			pItem->ModifyState(MISTATE_HOT, MISTATE_PRESSED);
			pItem->Update( dc.m_hDC , this );
		}

		// draw captions forcefully
		if (m_pMenuControl)
		{
			CRect rcCross = m_pMenuControl->GetRect() & m_MenuLst[m_nCurIndex]->GetRect();
			if (!rcCross.IsRectEmpty()) 
			{
				m_pMenuControl->ForceDrawControl( dc.m_hDC );
			}
		}
	}
	else 
	{
		// must be default parameter
		ASSERT(nNewIndex == -1);
	}

	m_bProcessRightArrow = m_bProcessLeftArrow = TRUE;
	TRACE( "m_nCurIndex is %d\r\n", m_nCurIndex );
}

int CSkinMenuBar::HitTestOnTrack(CPoint point)
{
	WTL::CRect rcw;
	::GetWindowRect( m_hWnd, &rcw);

	WTL::CPoint offsetPt = point;

	offsetPt.x -= rcw.left;
	offsetPt.y -= rcw.top;

	for (int i = 0; i < GetItemCount(); ++i)
	{
		CXPMenuItem* pItem = m_MenuLst[i];
		CRect rcItem = pItem->GetRect();

		if ((pItem->GetStyle() & MISTYLE_TRACKABLE) &&
			rcItem.PtInRect(offsetPt))
			return i;
	}

	return -1;
}

void CSkinMenuBar::OnMouseMove(UINT nFlags, WTL::CPoint point)
{
	//	LTRACE(_T("CXPMenuBar::OnMouseMove\n"));

	if (m_pMenuControl && m_pMenuControl->OnMouseMsg(WM_MOUSEMOVE, nFlags, point)) 
	{
		//CControlBar::OnMouseMove(nFlags, point);
		return;
	}

	int nIndex = HitTestOnTrack(point);
	if (IsValidIndex(nIndex)) 
	{	
		if (m_nCurIndex == -1 || m_nCurIndex != nIndex) 
		{// if other button
			UpdateBar(buttonmouse, nIndex);// button made hot with mouse

			//if (!m_nIDEvent)
			//	m_nIDEvent = SetTimer(1, 100, NULL);
		}
	}
	else 
	{
		UpdateBar();
	}
}

void CSkinMenuBar::OnLButtonDown(UINT nFlags, WTL::CPoint point) 
{
	//	LTRACE("CXPMenuBar::OnLButtonDown\n");
	WTL::CRect rcw;
	::GetWindowRect( m_hWnd, &rcw);

	WTL::CPoint offsetPt = point;

	offsetPt.x -= rcw.left;
	offsetPt.y -= rcw.top;
	if (m_pMenuControl && m_pMenuControl->OnMouseMsg(WM_LBUTTONDOWN, nFlags, offsetPt))
	{
		return;		// eat it!
	}	

	int nIndex = HitTestOnTrack(point);
	if (IsValidIndex(nIndex) &&	(m_MenuLst[nIndex]->GetStyle() & MISTYLE_TRACKABLE)) 
	{
		TrackPopup(nIndex);
		return;		// eat it!
	}
}

void CSkinMenuBar::OnLButtonUp(UINT nFlags, WTL::CPoint point) 
{
	// TODO: 
	//	LTRACE(_T("CXPMenuBar::OnLButtonUp\n"));
	if (m_pMenuControl && m_pMenuControl->OnMouseMsg(WM_LBUTTONUP, nFlags, point)) 
	{
		return;
	}	
}

void CSkinMenuBar::OnMouseLeave()
{
	//UpdateBar();
}


void CSkinMenuBar::DoPaint(CDCHandle dc)
{
	//	if (m_bDelayedButtonLayout)
	//		Layout();

	//LTRACE(_T("CXPMenuBar::DoPaint\n"));
	//CRect rect;	GetClientRect(rect);
	if ( m_bMDIApp )
		CheckActiveChildWndMaximized();

	WTL::CRect rcw;
	::GetWindowRect( m_hWnd, rcw );
	rcw.OffsetRect(-rcw.left, -rcw.top);
	WTL::CRect rcmb = CalcMenuBarRect(rcw, *m_pframe_state);

	// xp only
	//        BOOL flat_menu = FALSE;
	//        SystemParametersInfo(SPI_GETFLATMENU, 0, &flat_menu, 0);

	// bar background

	int r = dc.SaveDC();

	//COLORREF cr =  GetSchemeColor(WP_MENUBAR, 0, TMT_MENUBAR);
	//CBrush br;

	//br.CreateSolidBrush(cr);
	//dc.FillRect(&rcmb, br);

	if ( m_scheme )
		m_scheme->DrawBackground(dc.m_hDC, MENU, 0, 1, &rcmb, NULL );
	// draw items
	for (int i = 0; i < m_MenuLst.size(); ++i) 
	{
		m_MenuLst[i]->Update( dc , this);
	}

	// delay draw captions 
	if (m_pMenuControl) 
	{
		int caption_height = GetSchemeHeight(WP_CAPTION, CS_ACTIVE);
		m_pMenuControl->DelayLayoutAndDraw(dc, CSize(GetClipBoxLength(), caption_height + rcmb.Height()));
	}

	// release
	dc.RestoreDC(r);

}

void CSkinMenuBar::CheckActiveChildWndMaximized()
{
	//LTRACE(_T("CXPMenuBar::CheckActiveChildWndMaximized\n"));
	ASSERT(m_pMenuControl);
	ASSERT(m_pMenuIcon);

	BOOL bMaximized = FALSE;
	GetActiveChildWnd(bMaximized);
	if (m_bMDIMaximized != bMaximized) 
	{
		TRACE(_T("CXPMenuBar::CheckActiveChildWndMaximized---state changed, refreshing\n"));
		m_bMDIMaximized = bMaximized;
		m_pMenuControl->OnActivateChildWnd();
		m_pMenuIcon->OnActivateChildWnd();
		CalcLayout();
	}
}

}; // namespace 