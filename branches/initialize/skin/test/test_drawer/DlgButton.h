#pragma once

#include <Winuser.h>
#include <Windows.h>

class CDlgButton : public CDialogImpl<CDlgButton>
{
public:
	enum { IDD = IDD_DLG_BUTTON };
	
	CButton	m_Btn2;
	CButton	m_Btn1;
	CButton	m_Btn3;
	CButton	m_Btn4;
	//}}AFX_DATA
	CBitmap bmp;

	CTabCtrl m_tab;
	HICON m_hIcon;
	CFont font;

	CScrollBar scrollbar;

	BEGIN_MSG_MAP(CDlgButton)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		COMMAND_ID_HANDLER(IDOK, OnOk)
		COMMAND_ID_HANDLER(IDCANCEL, OnCancel)
		MESSAGE_HANDLER(WM_TIMER, OnTimer)
	END_MSG_MAP()
	
	LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		m_Btn1.Attach(GetDlgItem(IDC_BUTTON1));
		m_Btn2.Attach(GetDlgItem(IDC_BUTTON2));
		m_Btn3.Attach(GetDlgItem(IDC_BUTTON3));
		m_Btn4.Attach(GetDlgItem(IDC_BUTTON4));
		
		m_hIcon = (HICON)::LoadImage(_Module.GetResourceInstance(), MAKEINTRESOURCE(IDR_MAINFRAME), 
			IMAGE_ICON, ::GetSystemMetrics(SM_CXICON), ::GetSystemMetrics(SM_CYICON), LR_DEFAULTCOLOR);

		m_Btn3.SetIcon( m_hIcon );


		bmp.LoadBitmap( IDB_BITMAP1 );
		m_Btn2.SetBitmap( bmp );
		//m_Btn2.SetIcon( m_hIcon );

		// TODO: Add extra initialization here
		LOGFONT Font;
		Font.lfHeight = 18; 
		Font.lfWidth = 0; 
		Font.lfEscapement = 0; 
		Font.lfOrientation = 0; 
		Font.lfWeight = 400; 
		Font.lfItalic = 0; 
		Font.lfUnderline = 0; 
		Font.lfStrikeOut = 0; 
		Font.lfCharSet = 0; 
		Font.lfOutPrecision = 3; 
		Font.lfClipPrecision = 2; 
		Font.lfQuality = 1; 
		Font.lfPitchAndFamily = 0; 
		strcpy( Font.lfFaceName , "New Times" ); 


		if( font.CreateFontIndirect( &Font ) )
		{
			m_Btn1.SetFont( font );
		}

		m_tab.Attach(GetDlgItem(IDC_TAB1));
		m_tab.InsertItem(0, "tab1");
		m_tab.InsertItem(1, "tab2");
		m_tab.InsertItem(2, "tab3");
		m_tab.InsertItem(3, "tab4");
		m_tab.InsertItem(4, "tab5");
		m_tab.InsertItem(5, "tab6");

		
		CProgressBarCtrl progress;
		progress.Attach(GetDlgItem(IDC_PROGRESS1));
		progress.SetRange(0, 100);

		scrollbar.Attach( GetDlgItem(IDC_SCROLLBAR2) );
		SCROLLINFO info;
		info.cbSize = sizeof(SCROLLINFO);     
		info.fMask = SIF_ALL;     
		info.nMin = 0;     
		info.nMax = 100; 
		info.nPage = 10;     
		info.nPos = 5;    
		info.nTrackPos = 2; 
		scrollbar.SetScrollInfo(&info);


		SetTimer(1, 500, NULL);

		return 0;
	}

	LRESULT OnTimer(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		CProgressBarCtrl progress;
		progress.Attach(GetDlgItem(IDC_PROGRESS1));
		int nPos = progress.GetPos();
		nPos ++;
		if ( nPos > 100 )
			nPos = 0;
		progress.SetPos(nPos);

		return 0;
	}

	LRESULT OnOk(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
	{
		//EndDialog(IDOK);
		int nCount = m_tab.GetItemCount();
		TCITEM item;
		m_tab.GetItem( 0, &item);
		RECT rc;
		m_tab.GetItemRect ( 0, &rc );
	
		//scrollbar.Attach( GetDlgItem(IDC_SCROLLBAR2) );

		SCROLLBARINFO  info;
		info.cbSize = sizeof(info);
		scrollbar.GetScrollBarInfo(&info);
	
		return 0;
	}

	LRESULT OnCancel(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
	{
		EndDialog(IDCANCEL);
		return 0 ;
	}
};