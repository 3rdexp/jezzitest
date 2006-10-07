/*
 Copyright (c) 2000 
 Author: Konstantin Boukreev 
 E-mail: konstantin@mail.primorye.ru 

 Created: 11.07.2000 14:23:49
 Version: 1.0.0

*/

#ifndef _OptionDlg_8118f492_cb27_4b5d_a57a_60666dd352e5
#define _OptionDlg_8118f492_cb27_4b5d_a57a_60666dd352e5

#if _MSC_VER > 1000 
#pragma once
#endif // _MSC_VER > 1000

class kOptionDlg : 
	public CDialogImpl<kOptionDlg>
{
	kColorPickerButton m_btnTextColor;
	kColorPickerButton m_btnBackColor;

	kFontCombo m_comboFontName;
	kFlatCombo m_comboFontSize;
	kFlatCombo m_comboApply;

	int m_nUpdate;

public:
	enum {applyDefault, applyActive, applyActiveDefault};
	kOption m_option;
	int m_nApply;
	int m_nChange;

public:
	kOptionDlg(kOption option, int nApply) :
		m_option(option), 
		m_nApply(nApply), 
		m_nChange(0)
	{		
		m_nUpdate = 0;
	}

	enum { IDD = IDD_OPTION };

	BEGIN_MSG_MAP(CAboutDlg)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
		COMMAND_ID_HANDLER(IDOK, OnOk)
		COMMAND_ID_HANDLER(IDAPPLY, OnApply)
		COMMAND_ID_HANDLER(IDCANCEL, OnCancel)
		
		COMMAND_CODE_HANDLER(EN_CHANGE, OnUpdateUI)

		COMMAND_HANDLER(IDC_COMBO_FONT, CBN_EDITCHANGE, OnUpdateUI)
		COMMAND_HANDLER(IDC_COMBO_FONT, CBN_SELCHANGE, OnUpdateUI)
		COMMAND_HANDLER(IDC_COMBO_SIZE, CBN_EDITCHANGE, OnUpdateUI)
		COMMAND_HANDLER(IDC_COMBO_SIZE, CBN_SELCHANGE, OnUpdateUI)
		COMMAND_HANDLER(IDC_TEXT_COLOR, BN_CLICKED, OnUpdateUI)
		COMMAND_HANDLER(IDC_TEXT_COLOR, BN_CLICKED, OnUpdateUI)
		COMMAND_HANDLER(IDC_BACK_COLOR, BN_CLICKED, OnUpdateUI)
		COMMAND_HANDLER(IDC_BACK_COLOR, BN_CLICKED, OnUpdateUI)
		COMMAND_HANDLER(IDC_AUTO_URL, BN_CLICKED, OnUpdateUI)
		COMMAND_HANDLER(IDC_WORD_WRAP, BN_CLICKED, OnUpdateUI)
				
		REFLECT_NOTIFICATIONS()	
	END_MSG_MAP()

	LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		CenterWindow(GetParent());
		
		m_comboApply.SubclassWindow(GetDlgItem(IDC_COMBO_APPLY));			
		m_comboFontSize.SubclassWindow(GetDlgItem(IDC_COMBO_SIZE));

		RECT rc;
		::GetWindowRect(GetDlgItem(IDC_COMBO_FONT), &rc);
		::DestroyWindow(GetDlgItem(IDC_COMBO_FONT));
		ScreenToClient(&rc);
		rc.bottom += 150;
		m_comboFontName.CreateEx(m_hWnd, rc, -1, IDC_COMBO_FONT);

////// !
		m_comboFontSize.LimitText(4);
		TCHAR str[10];		
		for( int i=0; i < sizeof font_size / sizeof font_size[0]; i++ )
		{
			wsprintf(str, "%d", font_size[i] );			
			m_comboFontSize.AddString( str );
		}
/////////

		TCHAR* sApply [] = {
			_T("Default"),
			_T("Active view"),	
			_T("Active view and default") };

		int nCount = (m_nApply == applyDefault) ? 1 : sizeof sApply / sizeof sApply[0];
		for( int x = 0; x < nCount; x++ )
			m_comboApply.AddString( sApply[x] );
				
		m_btnTextColor.SubclassWindow(GetDlgItem(IDC_TEXT_COLOR));
		m_btnBackColor.SubclassWindow(GetDlgItem(IDC_BACK_COLOR));
		
		SetData();

		m_nUpdate = 0;
		::EnableWindow(GetDlgItem(IDOK), FALSE);
		::EnableWindow(GetDlgItem(IDAPPLY), FALSE);
		
		return TRUE;
	}

	LRESULT OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{		
		return 0;
	}

	LRESULT OnOk(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		GetData();

		int nApply = m_comboApply.GetCurSel();
		if (nApply != m_nApply)
		{
			m_nApply = nApply;
			m_nChange = kOption::opAll;
		}

		EndDialog(wID);
		return 0;
	}

	LRESULT OnApply(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		GetData();
		::EnableWindow(GetDlgItem(IDAPPLY), FALSE);
		::EnableWindow(GetDlgItem(IDOK), TRUE);
		return 0;
	}

	LRESULT OnCancel(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		EndDialog(wID);
		return 0;
	}

	LRESULT OnUpdateUI(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& bHandled)
	{
		bHandled = FALSE;

		if (!m_nUpdate)
		{
			::EnableWindow(GetDlgItem(IDAPPLY), TRUE);
			::EnableWindow(GetDlgItem(IDOK), TRUE);
		}

		switch (wID)
		{
			case IDC_COMBO_FONT:		m_nUpdate |= kOption::opFontName; break;
			case IDC_COMBO_SIZE:		m_nUpdate |= kOption::opFontSize; break;
			case IDC_TEXT_COLOR:		m_nUpdate |= kOption::opTextColor; break;
			case IDC_BACK_COLOR:		m_nUpdate |= kOption::opBackColor; break;
			case IDC_LEFT_MARGIN:	m_nUpdate |= kOption::opLeftMargin; break;
			case IDC_RIGHT_MARGIN:	m_nUpdate |= kOption::opRightMargin; break;			
			case IDC_TAB_COUNT:		m_nUpdate |= kOption::opTabCount; break;
			case IDC_UNDO_SIZE:		m_nUpdate |= kOption::opUndoSize; break;
			case IDC_MAX_BUFFER:		m_nUpdate |= kOption::opMaxBuffer; break;
			case IDC_AUTO_URL:		m_nUpdate |= kOption::opAutoURL; break;
			case IDC_WORD_WRAP:		m_nUpdate |= kOption::opWordWrap; break;			
			
			default:
				ATLASSERT(0);
		}
		
		return 0;
	}
	

	void SetData()
	{
		SetDlgItemInt(IDC_RIGHT_MARGIN, m_option.RightMargin);
		SetDlgItemInt(IDC_LEFT_MARGIN, m_option.LeftMargin);
		SetDlgItemInt(IDC_TAB_COUNT, m_option.TabCount);
		SetDlgItemInt(IDC_UNDO_SIZE, m_option.UndoSize);
		SetDlgItemInt(IDC_MAX_BUFFER, m_option.MaxBuffer);

		TCHAR sSize[10];
		wsprintf(sSize, _T("%u"), m_option.FontSize);
		m_comboFontSize.SelectString(-1, sSize);
		m_comboFontName.SelectString(-1, m_option.szFontName);				
		m_comboApply.SetCurSel(m_nApply);

		m_btnTextColor.m_clr = m_option.TextColor;
		m_btnBackColor.m_clr = m_option.BackColor;

		CheckDlgButton(IDC_AUTO_URL, m_option.AutoURL ? BST_CHECKED : BST_UNCHECKED);
		CheckDlgButton(IDC_WORD_WRAP, m_option.WordWrap ? BST_CHECKED : BST_UNCHECKED);
	}

	void GetData()
	{
		if (m_nUpdate & kOption::opFontName)
			m_comboFontName.GetWindowText(m_option.szFontName, LF_FACESIZE);
		if (m_nUpdate & kOption::opFontSize)
			m_option.FontSize = min(72, max(6, GetDlgItemInt(IDC_COMBO_SIZE)));
		if (m_nUpdate & kOption::opTextColor)
			m_option.TextColor = m_btnTextColor.m_clr;
		if (m_nUpdate & kOption::opBackColor)
			m_option.BackColor = m_btnBackColor.m_clr;
		if (m_nUpdate & kOption::opLeftMargin)
			m_option.LeftMargin = GetDlgItemInt(IDC_LEFT_MARGIN);
		if (m_nUpdate & kOption::opRightMargin)
			m_option.RightMargin = GetDlgItemInt(IDC_RIGHT_MARGIN);
		if (m_nUpdate & kOption::opTabCount)		
			m_option.TabCount = GetDlgItemInt(IDC_TAB_COUNT);
		if (m_nUpdate & kOption::opUndoSize)
			m_option.UndoSize = GetDlgItemInt(IDC_UNDO_SIZE);
		if (m_nUpdate & kOption::opMaxBuffer)
			m_option.MaxBuffer = GetDlgItemInt(IDC_MAX_BUFFER);
		if (m_nUpdate & kOption::opAutoURL)
			m_option.AutoURL = IsDlgButtonChecked(IDC_AUTO_URL) == BST_CHECKED;
		if (m_nUpdate & kOption::opWordWrap)
			m_option.WordWrap = IsDlgButtonChecked(IDC_WORD_WRAP) == BST_CHECKED;	
				
		m_nChange |= m_nUpdate;
		m_nUpdate = 0;		
	}
};

#endif //_OptionDlg_8118f492_cb27_4b5d_a57a_60666dd352e5

