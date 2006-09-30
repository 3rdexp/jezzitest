#pragma once

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

	HICON m_hIcon;
	CFont font;

	BEGIN_MSG_MAP(CDlgButton)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		COMMAND_ID_HANDLER(IDOK, OnOk)
		COMMAND_ID_HANDLER(IDCANCEL, OnCancel)
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

		
		return 0;
	}

	LRESULT OnOk(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
	{
		EndDialog(IDOK);
		return 0;
	}

	LRESULT OnCancel(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
	{
		EndDialog(IDCANCEL);
		return 0 ;
	}
};