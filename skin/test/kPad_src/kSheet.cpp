/*
 Copyright (c) 2000 
 Author: Konstantin Boukreev 
 E-mail: konstantin@mail.primorye.ru 

 Created: 26.06.2000 19:19:32
 Version: 1.0.0
*/

#include "stdafx.h"
#include "kSheet.h"
#include "common.h"

kSheetsCtrl::kSheetsCtrl() :
	m_winArrow((TCHAR*)CScrollBar::GetWndClassName(), this, 1),
	m_winScroll((TCHAR*)CScrollBar::GetWndClassName(), this, 2)	
{
	m_iActive = -1;
	m_xMoveBar = 0;
	m_xPages = 0;

	m_bShowScrollBar = true;
	
	m_hCursor = 0;
	m_bTrack = false;

	SetRectEmpty(&m_rcClient);
	SetRectEmpty(&m_rcScroll);

	m_szArrow.cx = ::GetSystemMetrics(SM_CXHSCROLL) * 2;
	m_szArrow.cy = ::GetSystemMetrics(SM_CYVSCROLL);

	LOGFONT lf = {
		80, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, 0, 0, 0, 0, _T("Tahoma")
	};

	m_font.CreatePointFontIndirect(&lf);
	lf.lfWeight = FW_BOLD;
	m_bfont.CreatePointFontIndirect(&lf);
}

kSheetsCtrl::~kSheetsCtrl()
{
	for (int i = 0; i < m_sheets.GetSize(); i++)
		delete m_sheets[i];
}

//////////////////////////
// interface

kSheetView* kSheetsCtrl::Add()
{
	kSheetView* sheet = new kSheetView;
	m_sheets.Add(sheet);

	HWND hWnd = sheet->Create(m_hWnd, m_rcClient, NULL, WS_CHILD | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_VSCROLL // | WS_HSCROLL 
			 | ES_AUTOHSCROLL | ES_AUTOVSCROLL | ES_MULTILINE | ES_NOHIDESEL | ES_SAVESEL, 0);
	hWnd;
	ATLASSERT(hWnd != 0);

	return sheet;
}

void kSheetsCtrl::Close(int iIndex)
{	
	ATLASSERT(iIndex >= 0 && iIndex < m_sheets.GetSize());
	
	int iActive = m_iActive;
	m_iActive = -1;
	
	kSheetView* sheet = m_sheets[iIndex];
	m_sheets.RemoveAt(iIndex);
	sheet->DestroyWindow();
	delete sheet;

	if (m_sheets.GetSize() == 0)
	{		
		m_winArrow.ShowWindow(SW_HIDE);
		m_winScroll.ShowWindow(SW_HIDE);				
		Invalidate();
		UpdateWindow();

		OnNotify();
	}
	else
	{	
		if (iIndex <= iActive)
			iActive = max(0, iActive - 1);

		Active(iActive);
		RedrawNeed();
	}
}

void kSheetsCtrl::Active(int iActive)
{	
	ATLASSERT(iActive != -1);

	bool bFirst = false;

	if (m_iActive != -1)
		m_sheets[m_iActive]->ShowWindow(SW_HIDE);
	 else 
		bFirst = true;
	
	m_iActive = iActive;		

	m_sheets[m_iActive]->SetWindowPos(0, &m_rcClient, SWP_SHOWWINDOW | SWP_NOMOVE | SWP_NOZORDER);
	m_sheets[m_iActive]->SetFocus();

	OnNotify();

	if (bFirst) 
	{
		RECT rc;
		GetClientRect(&rc);
		m_winArrow.ShowWindow(SW_SHOW);
		m_winScroll.ShowWindow(SW_SHOW);	
		ApplyLayout((rc.right - rc.left), (rc.bottom - rc.top));		
	}

	UpdateScrollBar();
	RedrawNeed();	
}

/////////////////////////////////
//

BOOL kSheetsCtrl::PreTranslateMessage(MSG* pMsg)
{
	if (IsEmpty())
		return FALSE;

	if (m_FindDlg.IsWindow() && m_FindDlg.IsDialogMessage(pMsg))
	{		
		return TRUE;
	}	
	else if (m_ReplaceDlg.IsWindow() && m_ReplaceDlg.IsDialogMessage(pMsg)) 
	{		
		return TRUE;
	}	

	ATLASSERT(m_iActive != -1);
	
	if ((pMsg->message == WM_KEYDOWN) && (pMsg->wParam == VK_TAB)) 
	{
		int i = 0;
		if (GetKeyState(VK_CONTROL) < 0) 
			i = 1;		 
		else if (GetKeyState(VK_SHIFT) < 0) 
			i = -1;
		
		if (i != 0)
		{
			int iActive = m_iActive + i;
			if (iActive == m_sheets.GetSize())
				iActive = 0;
			else if (iActive < 0)
				iActive = m_sheets.GetSize() - 1;
			Active(iActive);
			return TRUE;
		}
	}		

	return m_sheets[m_iActive]->PreTranslateMessage(pMsg);
}


/////////////////////////
// window message

LRESULT kSheetsCtrl::OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	RECT rc = {0,0,0,0};
	
	HWND hWnd = m_winArrow.Create(m_hWnd, rc, 0, WS_VISIBLE|WS_CHILD|WS_CLIPSIBLINGS|SBS_HORZ, 0, 100);	
	ATLASSERT(hWnd != 0);

	hWnd = m_winScroll.Create(m_hWnd, rc, 0, WS_VISIBLE|WS_CHILD|WS_CLIPSIBLINGS|SBS_HORZ, 0, 101);	
	ATLASSERT(hWnd != 0);

	UpdateArrow();
	DragAcceptFiles();

	m_fr.m_hWndOwner = m_hWnd;
	return 0;
}

LRESULT kSheetsCtrl::OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	DragAcceptFiles(FALSE);
	return 0;
}
				
LRESULT kSheetsCtrl::OnSize(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
{
	if (wParam == SIZE_MINIMIZED) return 0;
	if (IsEmpty()) return 0;
	ApplyLayout(LOWORD(lParam), HIWORD(lParam));	
	return 0;
}
		
LRESULT kSheetsCtrl::OnMouseMove(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& /*bHandled*/)
{
	if (IsEmpty()) return 0;
	if (!m_bShowScrollBar) return 0;
	
	int x = LOWORD(lParam);		
	if (HitMoverBar(x)) 
	{
		m_hCursor = SetCursor(LoadCursor(0, IDC_SIZEWE));		
	}
	else 
	{
		if (m_bTrack)
		{
			SetMoveBar(x);									
			m_hCursor = SetCursor(LoadCursor(0, IDC_SIZEWE));
		}
		else if (m_hCursor)
		{
			SetCursor(m_hCursor);
			m_hCursor = 0;
		}
	}
	return 0;
}	

LRESULT kSheetsCtrl::OnLButtonUp(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	if (IsEmpty()) return 0;
	if (m_bShowScrollBar && m_bTrack)
	{
		ReleaseCapture();			
		m_bTrack = false;
	}	
	return 0;
}

LRESULT kSheetsCtrl::OnLButtonDown(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& /*bHandled*/)
{
	if (IsEmpty()) return 0;
	int x = LOWORD(lParam);		
	
	if (m_bShowScrollBar && HitMoverBar(x))
	{		
		SetCapture();
		m_bTrack = true;
	}
	else
	{
		int n = HitTest(LOWORD(lParam), HIWORD(lParam));
		if (n != m_iActive && n != -1)
			Active(n);			
	}
	return 0;
}

LRESULT kSheetsCtrl::OnPaint(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	CPaintDC dc(m_hWnd);

	if (!IsEmpty()) 
	{				
		RECT rc = {0, 0, m_rcScroll.right - m_rcScroll.left, m_rcScroll.bottom - m_rcScroll.top};
		RECT rc1 = {rc.right - GetSystemMetrics(SM_CXVSCROLL), rc.top, rc.right, rc.bottom};	
			
		CDC dcMem;		
		CBitmap bitmap;

		dcMem.CreateCompatibleDC(dc);
		bitmap.CreateCompatibleBitmap(dc, rc.right,  rc.bottom);	
		HBITMAP hOldBitmap = dcMem.SelectBitmap(bitmap);		

		DrawSheets(dcMem, rc);
		DrawMoveBar(dcMem, rc);		
		dcMem.FillRect(&rc1, GetSysColorBrush(COLOR_BTNFACE));

		dc.BitBlt(m_rcScroll.left, m_rcScroll.top, rc.right,  rc.bottom, dcMem, 0, 0, SRCCOPY);

		dcMem.SelectBitmap(hOldBitmap);	
	}	
	return 0;
}

LRESULT kSheetsCtrl::OnEnLink(int /*idCtrl*/, LPNMHDR pnmh, BOOL& /*bHandled*/)
{
	ATLASSERT(!IsEmpty());
	m_sheets[m_iActive]->OnEnLink((ENLINK *)pnmh);
	return 0;
}

///////////////////////////////////
//

void kSheetsCtrl::SetMoveBar (int x)
{
	if ((x == m_xMoveBar)) return;

	RECT rc = m_rcScroll;	
	int w = rc.right - rc.left;
	int h = rc.bottom - rc.top;

	if ((x > m_szArrow.cx) && (x < (w - m_szArrow.cx)))
	{		
		int old = m_xMoveBar;
		m_xMoveBar = x;
		int cx = GetSystemMetrics(SM_CXVSCROLL);
		m_winScroll.SetWindowPos(0, m_xMoveBar + 4, rc.top, w - m_xMoveBar - 4 - cx, h, SWP_NOZORDER); 		

		int n = m_xPages - m_xMoveBar;		
		m_winArrow.SetScrollRange(0, max(0, n), TRUE);
			
		if (m_xMoveBar < old)
		{		
			CClientDC dc(m_hWnd);
			DrawMoveBar(dc, m_rcScroll);
			POINT pts[] = {{m_xMoveBar, 0}, {old, 0}};
			DrawCurve(dc, GetSysColor(COLOR_BTNFACE), pts, 2, true);					
		}
		else
		{
			RedrawNeed();		
		}				
	}
}

int kSheetsCtrl::HitTest(int xMouse, int)
{
	if (xMouse >= m_xMoveBar || xMouse <= m_szArrow.cx)
		return -1;
	
	CClientDC dc(m_hWnd);
	HFONT hOldFont = dc.SelectFont(m_font);	
	
	int iReturn = -1;
	int x = m_szArrow.cx + 10;
	int delta = m_winArrow.GetScrollPos(); 
//	int xStart = delta + x;	
	int i = 0;
	xMouse += delta;

	for (int idx = 0; idx < m_sheets.GetSize(); idx++)
	{		
		kSheetView* pSheet = m_sheets[idx];				
		RECT rt = {x, 0, 0, 0};

		dc.DrawText(pSheet->Name(), -1, &rt, DT_CALCRECT);

		if ((xMouse >= (rt.left - 4)) && (xMouse <= (rt.right + 4)))
		{
			iReturn = i;
			break;
		}

		if ((x - delta) >= m_xMoveBar)
			break;	
		
		x += rt.right - rt.left + 14;		
		i++;
	}

	if (hOldFont)	
		dc.SelectFont(hOldFont);			
	return iReturn;
}

bool kSheetsCtrl::HitMoverBar (int x)
{
	int left = m_xMoveBar - 1;
	int right = left + 4;
	return ((x >= left) && (x <= right));
}

void kSheetsCtrl::RedrawNeed()
{	
	if (IsWindow())
	{
		RECT rc = m_rcScroll;		
	//	rc.left = m_szArrow.cx; 
	//	rc.right = m_xMoveBar;
		InvalidateRect(&rc, FALSE); 
		UpdateWindow();
	}
}

void kSheetsCtrl::ApplyLayout(int nWidth, int nHeight)
{	
	int cx = GetSystemMetrics(SM_CXVSCROLL);
	
	SetRect(&m_rcClient, 0, 0, nWidth, nHeight - m_szArrow.cy);
	SetRect(&m_rcScroll, 0, m_rcClient.bottom, nWidth, nHeight);
	
	if (!m_bShowScrollBar)
	{
		m_xMoveBar = nWidth;
	}
	else
	{
		if (!m_xMoveBar)
		{
			m_xMoveBar = nWidth / 4 * 3;	
			m_xPages = m_xMoveBar; 
		}
		else
		{
			int rborder = nWidth - cx - m_szArrow.cx - 4;
			m_xMoveBar = max(m_xMoveBar, nWidth / 2);		
			m_xMoveBar = min(m_xMoveBar, rborder);		
		}	
	}

	int i = 2;
	i += (m_bShowScrollBar ? 1 : 0);

	ATLASSERT(m_iActive != -1);
	
	HDWP hdwp = BeginDeferWindowPos(i);		
		if (m_bShowScrollBar)
			m_winScroll.DeferWindowPos(hdwp, 0, m_xMoveBar + 4, m_rcScroll.top, nWidth - m_xMoveBar - 4 - cx, m_szArrow.cy, SWP_NOZORDER); 		
		m_winArrow.DeferWindowPos(hdwp, 0, 0, m_rcScroll.top, m_szArrow.cx, m_szArrow.cy, SWP_NOZORDER); 
		m_sheets[m_iActive]->DeferWindowPos(hdwp, 0, 0, 0, nWidth, nHeight - m_szArrow.cy, SWP_NOZORDER); 
	EndDeferWindowPos(hdwp);

	UpdateArrow();
	UpdateScrollBar();
}


////////////////////////////////////////////////
// scroll

LRESULT kSheetsCtrl::OnRequestResize(int idCtrl, LPNMHDR pnmh, BOOL& /*bHandled*/)
{
	REQRESIZE *prs = reinterpret_cast<REQRESIZE *>(pnmh);

	HWND hWnd = prs->nmhdr.hwndFrom;
	::SendMessage(hWnd, WM_NOTIFY, idCtrl, (LPARAM)pnmh);

	if (m_iActive != -1) //!
		UpdateScrollBar();
	return 0;
}

LRESULT kSheetsCtrl::OnHScrollNotify(WORD /*wNotifyCode*/, WORD /*wID*/, HWND hWndCtl, BOOL& /*bHandled*/)
{	
	kSheetView* sheet = m_sheets[m_iActive];

	hWndCtl;
	ATLASSERT(hWndCtl == sheet->m_hWnd);

	POINT pt;
	int nMin, nMax;

	sheet->SendMessage(EM_GETSCROLLPOS, 0, (LPARAM)&pt);	
	m_winScroll.GetScrollRange(&nMin, &nMax);
	m_winScroll.SetScrollPos(min(pt.x, nMax));

	return 0;
}

LRESULT kSheetsCtrl::OnHScroll(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
{
	HWND hWnd = (HWND) lParam;
	if (m_winArrow == hWnd)
	{		
		SCROLLINFO si;
		si.cbSize = sizeof SCROLLINFO;
		si.fMask = SIF_RANGE | SIF_POS;			
		m_winArrow.GetScrollInfo(&si);	

		int delta = 0;
		
		switch (LOWORD(wParam))
		{
			case SB_LINELEFT: 
				delta = - DELTA_ARROW; 
				break;
			case SB_LINERIGHT: 
				delta =  DELTA_ARROW; 
				break;
			case SB_ENDSCROLL: 
				UpdateArrow(); 
				return 0;
			default: 
				ATLASSERT(0); 
				break;
		}

		int n = min(si.nPos + delta, si.nMax);
		n = max(0, n);
	
		if (n != si.nPos)
		{				
			m_winArrow.SetScrollPos(n);
			RedrawNeed();
		}
	} 
	else if (m_winScroll == hWnd)
	{
		
		kSheetView* sheet = m_sheets[m_iActive];
		sheet->SendMessage(uMsg, wParam, lParam);		

		if (SB_THUMBPOSITION == LOWORD(wParam)) {
			BOOL b;
			OnHScrollNotify(0, 0, sheet->m_hWnd, b);
		}
	}
	return 0;
}

void kSheetsCtrl::UpdateScrollBar ()
{
	ATLASSERT(m_iActive != -1);
	kSheetView* sheet = m_sheets[m_iActive];

	RECT rc;
	sheet->GetWindowRect(&rc);	
	int cx = rc.right - rc.left;

	if (cx > sheet->GetRequestWidth())	
		m_winScroll.EnableScrollBar(ESB_DISABLE_BOTH);	
	else
		m_winScroll.SetScrollRange(0, sheet->GetRequestWidth() - cx, TRUE);			
}

void kSheetsCtrl::UpdateArrow ()
{
	m_winArrow.EnableScrollBar(ESB_ENABLE_BOTH);

	SCROLLINFO si; 
	si.cbSize = sizeof SCROLLINFO;
	si.fMask = SIF_RANGE | SIF_POS;
	m_winArrow.GetScrollInfo(&si);
		
	if (si.nPos >= si.nMax)
		m_winArrow.EnableScrollBar(ESB_DISABLE_RIGHT);
	if (si.nPos <= si.nMin)
		m_winArrow.EnableScrollBar(ESB_DISABLE_LEFT);	
}

//////////////////////////
// draw code

void kSheetsCtrl::DrawSheets(HDC hdc, RECT& rcDraw)
{
	CDCHandle dc(hdc);

	RECT rc = {m_szArrow.cx, rcDraw.top, m_xMoveBar, rcDraw.bottom};
	dc.FillRect(&rc, GetSysColorBrush(COLOR_BTNFACE));

	HFONT hOldFont = dc.SelectFont(m_font);		
	dc.SetBkMode(TRANSPARENT);	
	dc.SetTextColor(GetSysColor(COLOR_WINDOWTEXT));

	CRgn rgnActive;
	RECT rtActive = {0, 0, 0, 0}; 
	POINT pActivePage[4];

	int y = rc.top;
	int h = rc.bottom - rc.top;
//	int w = rc.right - rc.left; 
	int x = rc.left + 10; // !
	int delta = m_winArrow.GetScrollPos(); 
	int xStart = delta + x;	
//	int cxHide = 0;
	int i = 0;

	POINT pts[] = {{0, y}, {m_xMoveBar + 4, y}};
	DrawCurve(dc, GetSysColor(COLOR_WINDOWFRAME), pts, 2, true);
 		
	for (int idx = 0; idx < m_sheets.GetSize(); idx++)
	{		
		kSheetView* pSheet = m_sheets[idx];		
		RECT rt = {x, y, 0, 0};
		
		dc.DrawText(pSheet->Name(), -1, &rt, DT_CALCRECT);

		if (((rt.right + 10) >= xStart) && ((x - delta) < m_xMoveBar))
		{			
			rt.left -= delta + 4;	
			rt.right -= delta - 4;			
			rt.top = y; 
			rt.bottom = y + h; 
					
			POINT pPage[4] = {
				{rt.left - 6, y}, 
				{rt.left, rt.bottom}, 
				{rt.right, rt.bottom}, 
				{rt.right + 6, y} };
					 
			CRgn rgn;
			rgn.CreatePolygonRgn(pPage, 4, WINDING);
			dc.FillRgn(rgn, GetSysColorBrush(COLOR_BTNFACE));
			dc.FrameRgn(rgn, GetSysColorBrush(COLOR_WINDOWFRAME), 1, 1);	
			dc.DrawText(pSheet->Name(), -1, &rt, DT_VCENTER | DT_SINGLELINE | DT_CENTER);
					
			if (i == m_iActive)
			{
				rgnActive.CreatePolygonRgn(pPage, 4, ALTERNATE);
				rtActive = rt;
				memcpy(pActivePage, pPage, sizeof POINT * 4);
			}
			x += rt.right - rt.left + 6;
		}
		else			
			x += rt.right - rt.left + 14;		//4+4+6
		i++;
	}	
	
	m_xPages = x;
	if (m_xPages > m_xMoveBar)	
		m_winArrow.SetScrollRange(0, m_xPages - m_xMoveBar, TRUE);
	else
		UpdateArrow();			

	if ((m_iActive != -1) && rgnActive)
	{		
		kSheetView* pSheet = m_sheets[m_iActive];		
		dc.SelectFont(m_bfont);	

		CBrush br;
		br.CreateSolidBrush(pSheet->GetBackgroundColor());
		dc.FillRgn(rgnActive, br); 
		dc.FrameRgn(rgnActive, GetSysColorBrush(COLOR_WINDOWFRAME), 1, 1);
		dc.DrawText(pSheet->Name(), -1, &rtActive, DT_VCENTER | DT_SINGLELINE | DT_CENTER);

		POINT pts[] = {{pActivePage[0].x, pActivePage[0].y}, {pActivePage[3].x, pActivePage[3].y}};
		DrawCurve(dc, pSheet->GetBackgroundColor(), pts, 2, true);
	}
	
	if (hOldFont)	
		dc.SelectFont(hOldFont);			
}

void kSheetsCtrl::DrawMoveBar(HDC hdc, RECT& rcDraw)
{
	RECT rc = {m_xMoveBar, rcDraw.top + 1, m_xMoveBar + 2, rcDraw.bottom};

	POINT pt0[] = {{rc.left, rc.top}, {rc.left, rc.bottom}};
	POINT pt1[] = {{rc.left + 1, rc.top}, {rc.left + 1, rc.bottom}};
	POINT pt2[] = {{rc.left + 2, rc.top}, {rc.left + 2, rc.bottom}};

	DrawCurve(hdc, GetSysColor(COLOR_WINDOWFRAME), pt0, 2, true);	
	DrawCurve(hdc, GetSysColor(COLOR_3DHILIGHT), pt1, 2, true);
	DrawCurve(hdc, GetSysColor(COLOR_BTNFACE), pt2, 2, true);
}

void kSheetsCtrl::DrawCurve(HDC hdc, COLORREF clr, POINT* pts, int n, bool bMove)
{
	CDCHandle dc(hdc);

	CPen pen;
	pen.CreatePen(PS_SOLID, 1, clr); 
	HPEN hOldPen = dc.SelectPen(pen);		
	
	int  i = 0;
	if (bMove)
	{
		dc.MoveTo(pts[0].x, pts[0].y, 0); 
		i = 1;
	}

	for (; i < n; i++)
		dc.LineTo(pts[i].x, pts[i].y);	
	
	dc.SelectPen(hOldPen);	
}

//////////////////////////////////////////////////////
// find

LRESULT kSheetsCtrl::OnEditFind(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	if (m_FindDlg.IsWindow()) 
	{
		m_FindDlg.SetFocus();
		return 1;
	}
	
	if (m_ReplaceDlg.IsWindow())
	{
		m_ReplaceDlg.SendMessage(WM_CLOSE);
	}

	if(m_FindDlg.CreateEx(&m_fr) == 0)
	{		
		::MessageBeep((UINT)-1);
		return 1;
	}
	
	m_FindDlg.ShowWindow(SW_NORMAL);
	return 0;
}
LRESULT kSheetsCtrl::OnEditFindNext(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	if(m_fr.m_szFindWhat[0] == 0)
		return OnEditFind(wNotifyCode, wID, hWndCtl, bHandled);

	ATLASSERT(!IsEmpty());
	kSheetView* sheet = m_sheets[m_iActive];

	if (!sheet->DoFindNext(&m_fr))
		::MessageBeep((UINT)-1);
	return 0;
}
LRESULT kSheetsCtrl::OnEditReplace(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	if (m_ReplaceDlg.IsWindow()) 
	{
		m_ReplaceDlg.SetFocus();
		return 1;
	}
	
	if (m_FindDlg.IsWindow())
	{
		m_FindDlg.SendMessage(WM_CLOSE);
	}

	if(m_ReplaceDlg.CreateEx(&m_fr) == 0)
	{		
		::MessageBeep((UINT)-1);
		return 1;
	}
	
	m_ReplaceDlg.ShowWindow(SW_NORMAL);
	return 0;
}

LRESULT kSheetsCtrl::OnFindReplaceCmd(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& /*bHandled*/)
{
	lParam;
	ATLASSERT(lParam == (LPARAM)&m_fr);
	ATLASSERT(!IsEmpty());

	kSheetView* sheet = m_sheets[m_iActive];

	if(m_fr.FindNext())
	{
		if(!sheet->DoFindNext(&m_fr)) {
			g_Context.m_frame->Send_RefreshStatusBar(_T("Error. Can't found '%s'"), m_fr.m_szFindWhat); 
			::MessageBeep((UINT)-1);
		}
		else {		
			int nRow = 0; int nCol = 0;				
			sheet->GetCurrentRowCol(nRow, nCol, true);
			g_Context.m_frame->Send_RefreshStatusBar(_T("Ok. Found '%s' in {%d, %d}"), m_fr.m_szFindWhat, nRow, nCol); 
		}
	}
	else if(m_fr.ReplaceCurrent())
	{
		if(!sheet->DoReplaceCurrent(&m_fr))
			::MessageBeep((UINT)-1);
	}
	else if(m_fr.ReplaceAll())
	{
		if(!sheet->DoReplaceAll(&m_fr))
			::MessageBeep((UINT)-1);
	}

	if(m_fr.IsTerminating()) {				
		// nothing
	}
	
	return 0;
}

//////////////////////////////
// drop files 

LRESULT kSheetsCtrl::OnDropFiles(UINT, WPARAM wParam, LPARAM, BOOL&)
{
	DoDropFiles((HDROP) wParam);
	return 0;
}

LRESULT kSheetsCtrl::OnEnDropFiles(int /*idCtrl*/, LPNMHDR pnmh, BOOL& /*bHandled*/)
{
	ENDROPFILES *df = (ENDROPFILES *)pnmh;
	DoDropFiles((HDROP) df->hDrop );	
	return 0;
}

void kSheetsCtrl::DoDropFiles(HDROP hDrop)
{
	TCHAR sPath[MAX_PATH] = {0};
	DragQueryFile(hDrop, 0, sPath, MAX_PATH);
	if (lstrlen(sPath))
		g_Context.m_frame->DoFileOpen(sPath, true); 			
	DragFinish(hDrop);
}