/*
 Copyright (c) 2000 
 Author: Konstantin Boukreev 
 E-mail: konstantin@mail.primorye.ru 

 Created: 06.08.2000 14:12:34
 Version: 1.0.0

*/

#if !defined(AFX_ABOUTDLG_H__8767AF1F_3756_4A6A_991F_7CAD7165DBB2__INCLUDED_)
#define AFX_ABOUTDLG_H__8767AF1F_3756_4A6A_991F_7CAD7165DBB2__INCLUDED_

class CAboutDlg : 
	public CDialogImpl<CAboutDlg>,
	public CAuxThunk<CAboutDlg>
{
	CHyperLink	m_link;
	DWORD			m_timer;	
	CContainedWindow m_warning;

public:

	CAboutDlg() :
		m_warning(_T("STATIC"), this, 1)
	{
		m_timer = 0;
		InitThunk((TMFP)OnTimer, this);
	}

	enum { IDD = IDD_ABOUTBOX };

	BEGIN_MSG_MAP(CAboutDlg)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
//		MESSAGE_HANDLER(WM_CTLCOLORSTATIC, OnCtlColorStatic)		
		COMMAND_ID_HANDLER(IDOK, OnCloseCmd)
		COMMAND_ID_HANDLER(IDCANCEL, OnCloseCmd)
		ALT_MSG_MAP(1)
			MESSAGE_HANDLER(WM_PAINT, OnPaintWarning)
			MESSAGE_HANDLER(WM_ERASEBKGND, OnEraseBkgndWarning)
	END_MSG_MAP()

	LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		CenterWindow(GetParent());
	
		// IDC_HYPERLINK
		m_link.SetHyperLink(_T("mailto:konstantin@mail.ptimorye.ru"));
		m_link.SetLabel(_T("konstantin@mail.ptimorye.ru"));
		m_link.SubclassWindow(GetDlgItem(IDC_HYPERLINK));		
	
		// fix bug with SS_CENTER style
		RECT rc;
		m_link.GetClientRect(&rc);
		CopyRect(&m_link.m_rcLink, &rc);

		m_timer = ::SetTimer(0, 0, 60, (TIMERPROC)GetThunk());

		m_warning.SubclassWindow(GetDlgItem(IDC_WARNING));

		return TRUE;
	}

	LRESULT OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		::KillTimer(0, m_timer);
		return 0;
	}

	LRESULT OnCloseCmd(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		EndDialog(wID);
		return 0;
	}

	VOID OnTimer(HWND, UINT, UINT_PTR, DWORD)	
	{
		HWND hWnd = GetDlgItem(IDC_WARNING);
		::InvalidateRect(hWnd, 0, FALSE);
		::UpdateWindow(hWnd);
	}

	LRESULT OnPaintWarning(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		static TCHAR szText[] = _T("Pre-release (beta)");
		
		static int bRed = 255;		
		static int bDelta = -10;
		
		if (bRed < 30)
			bDelta = 10;
		else if (bRed > 245)
			bDelta = -10;

		bRed += bDelta;		

		RECT rc;
		m_warning.GetClientRect(&rc);

		CPaintDC dc(m_warning);
		CDC dcMem;		
		CBitmap bitmap;

		dcMem.CreateCompatibleDC(dc);
		bitmap.CreateCompatibleBitmap(dc, rc.right,  rc.bottom);	
		HBITMAP hOldBitmap = dcMem.SelectBitmap(bitmap);		
		
		dcMem.FillRect(&rc, GetSysColorBrush(COLOR_BTNFACE));
		dcMem.SetTextColor(RGB(bRed,0,0));
		dcMem.SetBkMode(TRANSPARENT);
		dcMem.SelectFont((HFONT)GetStockObject(DEFAULT_GUI_FONT));	
		dcMem.SetTextAlign(TA_CENTER);		
		dcMem.TextOut((rc.left + rc.right) / 2, 0, szText, sizeof szText / sizeof TCHAR - 1);
		dc.BitBlt(0, 0, rc.right,  rc.bottom, dcMem, 0, 0, SRCCOPY);
		dcMem.SelectBitmap(hOldBitmap);	

		return 0;
	}

	LRESULT OnEraseBkgndWarning(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		return 1;
	}
	
		
};

#endif // !defined(AFX_ABOUTDLG_H__8767AF1F_3756_4A6A_991F_7CAD7165DBB2__INCLUDED_)
