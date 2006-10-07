/*
 Copyright (c) 2000 
 Author: Konstantin Boukreev 
 E-mail: konstantin@mail.primorye.ru 

 Created: 07.07.2000 18:11:08
 Version: 1.0.0

 MainFrm.h : interface of the CMainFrame class
 
 based on MTPad sample from MS SDK 12/01/99
*/

#if !defined(AFX_MAINFRM_H__44FA1498_FAD8_47D3_B3DB_6E5CA86E2C2B__INCLUDED_)
#define AFX_MAINFRM_H__44FA1498_FAD8_47D3_B3DB_6E5CA86E2C2B__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

namespace {

// filter string
const TCHAR* sFilter = 
			_T("All Files (*.*)\0*.*\0")
			_T("Text Files (*.txt)\0*.txt\0")
			_T("RTF Files (*.rtf)\0*.rtf\0");
}

class CMainFrame : 
	public CFrameWindowImpl<CMainFrame>, 
	public CUpdateUI<CMainFrame>,
	public CMessageFilter, 
	public CIdleHandler,
	public IMainFrame,
	public CPrintJobInfo
{	
	enum {
		FILE_MENU_POSITION = 0,
		RECENT_MENU_POSITION = 14,
		ID_FORMAT_TOOLBAR = 101,
		ID_PRINT_TOOLBAR,
		IDC_FONTNAME,
		IDC_FONTSIZE,
		IDC_SHEETS
	};
		
//	CCommandBarCtrl m_CmdBar;
	RamonSmit_CCommandBarCtrl m_CmdBar;
	CRecentDocumentList m_mru;
	CMultiPaneStatusBarCtrl m_sbar;
	CProgressBarCtrl m_progress;	
	kFontCombo m_comboFont;
	kFlatCombo m_comboSize;
	kSheetsCtrl m_view;
	
	bool m_bModified;

	BOOL m_bShowToolBar;
	BOOL m_bShowFormatBar;
	BOOL  m_bShowStatusBar;

	kOption m_defOption;

	DWORD m_dwTimeRefreshStatusBar;

//// print
	CPrinterT<true> printer;
	CDevModeT<true> devmode;
	CPrintPreviewWindow prev;
	RECT m_rcMargin;
	HWND m_hWndToolBarPP;
	CSimpleValArray<long> m_arrPages;

public:
	CMainFrame() 
	{		
		m_bModified = false;

		m_bShowToolBar = TRUE;
		m_bShowFormatBar = TRUE;
		m_bShowStatusBar = TRUE;

		m_dwTimeRefreshStatusBar = 0;

		// print
		m_rcMargin.left = m_rcMargin.right = m_rcMargin.top = m_rcMargin.bottom = 1000;		
		m_hWndToolBarPP = 0;
		printer.OpenDefaultPrinter();
		devmode.CopyFromPrinter(printer);
	}

	DECLARE_FRAME_WND_CLASS(NULL, IDR_MAINFRAME)

	BEGIN_MSG_MAP(CMainFrame)
		MESSAGE_HANDLER(WM_CREATE, OnCreate)
		MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
		MESSAGE_HANDLER(WM_SETFOCUS, OnSetFocus)
		MESSAGE_HANDLER(WM_CONTEXTMENU, OnContextMenu)
		MESSAGE_HANDLER(WM_INPUTLANGCHANGE, OnRefreshKeyboardLayout)
		MESSAGE_HANDLER(WM_CLOSE, OnClose)
				
		//
		if (uMsg == WM_COMMAND) {
			if (m_view.ProcessWindowMessage(hWnd, uMsg, wParam, lParam, lResult, 0))
				return TRUE;
		}

		if (((uMsg == WM_COMMAND) || 
			(uMsg == WM_NOTIFY && (((LPNMHDR)lParam)->idFrom == ID_FORMAT_TOOLBAR) && (((LPNMHDR)lParam)->code == TBN_DROPDOWN)))
			&& !m_view.IsEmpty())
		{ 
			kSheetView* sheet = m_view.GetAt(m_view.Active());			
			if (sheet->ProcessWindowMessage(hWnd, uMsg, wParam, lParam, lResult, 0))			
				return TRUE;			
		}
		//

		COMMAND_ID_HANDLER(ID_FILE_PRINT, OnFilePrint)
		COMMAND_ID_HANDLER(ID_FILE_PRINT_PREVIEW, OnFilePrintPreview)
		COMMAND_ID_HANDLER(ID_FILE_PRINT_SETUP, OnFilePageSetup)
		COMMAND_ID_HANDLER(ID_PP_CLOSE, OnPrintPreviewClose)
		COMMAND_ID_HANDLER(ID_PP_BACK, OnPrintPreviewBack)
		COMMAND_ID_HANDLER(ID_PP_FORWARD, OnPrintPreviewForward)		
		COMMAND_ID_HANDLER(ID_APP_EXIT, OnFileExit)
		COMMAND_ID_HANDLER(ID_FILE_NEW, OnFileNew)		
		COMMAND_ID_HANDLER(ID_FILE_NEW_WINDOW, OnFileNewWindow)
		COMMAND_ID_HANDLER(ID_FILE_OPEN, OnFileOpen)
		COMMAND_ID_HANDLER(ID_FILE_SAVE, OnFileSave)
		COMMAND_ID_HANDLER(ID_FILE_SAVE_AS, OnFileSave)
		COMMAND_ID_HANDLER(ID_FILE_CLOSE, OnFileClose)	
		COMMAND_ID_HANDLER(ID_VIEW_TOOLBAR, OnViewToolBar)
		COMMAND_ID_HANDLER(ID_VIEW_FORMAT, OnViewFormat)
		COMMAND_ID_HANDLER(ID_VIEW_STATUS_BAR, OnViewStatusBar)
		COMMAND_ID_HANDLER(ID_OPTION, OnOption)				
		COMMAND_ID_HANDLER(ID_APP_ABOUT, OnAppAbout)		
		COMMAND_HANDLER(IDC_FONTNAME, CBN_SELCHANGE, OnSelectFontName) 
		COMMAND_HANDLER(IDC_FONTSIZE, CBN_SELCHANGE, OnSelectFontSize)		
		COMMAND_RANGE_HANDLER(ID_FILE_MRU_FIRST, ID_FILE_MRU_LAST, OnFileRecent)
		
		NOTIFY_CODE_HANDLER(TBN_DROPDOWN, OnToolbarDropDown)
		NOTIFY_HANDLER(IDC_SHEETS, kSheetsCtrl::SC_ONCHANGESHEET, OnChangeSheet)
		
		CHAIN_MSG_MAP(CUpdateUI<CMainFrame>)
		CHAIN_MSG_MAP(CFrameWindowImpl<CMainFrame>)

		REFLECT_NOTIFICATIONS()		
	END_MSG_MAP()

	BEGIN_UPDATE_UI_MAP(CMainFrame)
		UPDATE_ELEMENT(ID_FILE_SAVE,			UPDUI_MENUPOPUP | UPDUI_TOOLBAR)
		UPDATE_ELEMENT(ID_FILE_SAVE_AS,		UPDUI_MENUPOPUP)
		UPDATE_ELEMENT(ID_FILE_CLOSE,			UPDUI_MENUPOPUP | UPDUI_TOOLBAR)
		UPDATE_ELEMENT(ID_FILE_PRINT,			UPDUI_MENUPOPUP | UPDUI_TOOLBAR)
		UPDATE_ELEMENT(ID_FILE_PRINT_PREVIEW,	UPDUI_MENUPOPUP)
		UPDATE_ELEMENT(ID_FILE_PRINT_SETUP,	UPDUI_MENUPOPUP)
		UPDATE_ELEMENT(ID_EDIT_REDO,			UPDUI_MENUPOPUP | UPDUI_TOOLBAR)
		UPDATE_ELEMENT(ID_EDIT_UNDO,			UPDUI_MENUPOPUP | UPDUI_TOOLBAR)
		UPDATE_ELEMENT(ID_EDIT_CUT, 			UPDUI_MENUPOPUP | UPDUI_TOOLBAR)
		UPDATE_ELEMENT(ID_EDIT_COPY,			UPDUI_MENUPOPUP | UPDUI_TOOLBAR)
		UPDATE_ELEMENT(ID_EDIT_PASTE,			UPDUI_MENUPOPUP | UPDUI_TOOLBAR)
		UPDATE_ELEMENT(ID_EDIT_CLEAR,			UPDUI_MENUPOPUP | UPDUI_TOOLBAR)
		UPDATE_ELEMENT(ID_EDIT_SELECT_ALL,	UPDUI_MENUPOPUP)
		UPDATE_ELEMENT(ID_EDIT_FIND,			UPDUI_MENUPOPUP | UPDUI_TOOLBAR)
		UPDATE_ELEMENT(ID_EDIT_REPEAT,		UPDUI_MENUPOPUP)
		UPDATE_ELEMENT(ID_EDIT_REPLACE,		UPDUI_MENUPOPUP)
		UPDATE_ELEMENT(ID_FORMAT_FONT,		UPDUI_MENUPOPUP | UPDUI_TOOLBAR)
		UPDATE_ELEMENT(ID_FORMAT_COLOR,		UPDUI_MENUPOPUP | UPDUI_TOOLBAR)
		UPDATE_ELEMENT(ID_FORMAT_BGRN,		UPDUI_MENUPOPUP | UPDUI_TOOLBAR)
		UPDATE_ELEMENT(ID_FORMAT_HIGHLIGHT,	UPDUI_MENUPOPUP | UPDUI_TOOLBAR)		
		UPDATE_ELEMENT(ID_FORMAT_BOLD,		UPDUI_MENUPOPUP | UPDUI_TOOLBAR)
		UPDATE_ELEMENT(ID_FORMAT_ITALIC,		UPDUI_MENUPOPUP | UPDUI_TOOLBAR)
		UPDATE_ELEMENT(ID_FORMAT_UNDERLINE,	UPDUI_MENUPOPUP | UPDUI_TOOLBAR)
		UPDATE_ELEMENT(ID_FORMAT_CNT,			UPDUI_MENUPOPUP | UPDUI_TOOLBAR)
		UPDATE_ELEMENT(ID_FORMAT_LEFT,		UPDUI_MENUPOPUP | UPDUI_TOOLBAR)
		UPDATE_ELEMENT(ID_FORMAT_RIGHT,		UPDUI_MENUPOPUP | UPDUI_TOOLBAR)
		UPDATE_ELEMENT(ID_FORMAT_JST,			UPDUI_MENUPOPUP | UPDUI_TOOLBAR)
		UPDATE_ELEMENT(ID_EDIT_WORD_WRAP,	UPDUI_MENUPOPUP)		
		UPDATE_ELEMENT(ID_VIEW_TOOLBAR,		UPDUI_MENUPOPUP)
		UPDATE_ELEMENT(ID_VIEW_FORMAT,		UPDUI_MENUPOPUP)
		UPDATE_ELEMENT(ID_VIEW_STATUS_BAR,	UPDUI_MENUPOPUP)
	END_UPDATE_UI_MAP()

	virtual BOOL PreTranslateMessage(MSG* pMsg)
	{	
		if (m_view.PreTranslateMessage(pMsg))
			return TRUE;

		if(CFrameWindowImpl<CMainFrame>::PreTranslateMessage(pMsg))
			return TRUE;

		return FALSE;
	}

	virtual BOOL OnIdle()
	{
		UpdateUIAll();
		return FALSE;
	}

	LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{

		ReadFromRegistry();

		// create command bar window
		HWND hWndCmdBar = m_CmdBar.Create(m_hWnd, rcDefault, NULL, ATL_SIMPLE_CMDBAR_PANE_STYLE);
		// atach menu
		m_CmdBar.AttachMenu( GetMenu());
		// load command bar images
		m_CmdBar.LoadImages(IDR_MAINFRAME);
		m_CmdBar.LoadImages(IDR_FORMAT);
		// remove old menu
		SetMenu(NULL);

		HWND hWndToolBar = CreateSimpleToolBarCtrl(m_hWnd, IDR_MAINFRAME, FALSE, ATL_SIMPLE_TOOLBAR_PANE_STYLE);	
		HWND hWndToolBarFormat = CreateFormatToolbar();

		CreateSimpleReBar(ATL_SIMPLE_REBAR_NOBORDER_STYLE);
		AddSimpleReBarBand(hWndCmdBar);
		AddSimpleReBarBand(hWndToolBar, NULL, TRUE);
		AddSimpleReBarBand(hWndToolBarFormat, NULL, FALSE);

		CreateSimpleStatusBar();
		m_sbar.SubclassWindow(m_hWndStatusBar);
		int arrParts[] =
		{
			ID_DEFAULT_PANE,
			ID_LANG_PANE,
			ID_ROW_PANE,
			ID_COL_PANE
		};
		m_sbar.SetPanes(arrParts, sizeof(arrParts) / sizeof(int), false);

		m_hWndClient = m_view.Create(m_hWnd, rcDefault, NULL, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN, WS_EX_CLIENTEDGE,
			IDC_SHEETS);

		UIAddToolBar(hWndToolBar);
		UIAddToolBar(hWndToolBarFormat); 

		UISetCheck(ID_VIEW_TOOLBAR, m_bShowToolBar);
		UISetCheck(ID_VIEW_FORMAT,	 m_bShowFormatBar);
		UISetCheck(ID_VIEW_STATUS_BAR, m_bShowStatusBar);

		CMessageLoop* pLoop = _Module.GetMessageLoop();
		pLoop->AddMessageFilter(this);
		pLoop->AddIdleHandler(this);

		HMENU hMenu = m_CmdBar.GetMenu();
		HMENU hFileMenu = ::GetSubMenu(hMenu, FILE_MENU_POSITION);
#ifdef _DEBUG
		// absolute position, can change if menu changes
		TCHAR szMenuString[100];
		::GetMenuString(hFileMenu, RECENT_MENU_POSITION, szMenuString, sizeof(szMenuString), MF_BYPOSITION);
		ATLASSERT(lstrcmp(szMenuString, _T("Recent &Files")) == 0);
#endif //_DEBUG
		HMENU hMruMenu = ::GetSubMenu(hFileMenu, RECENT_MENU_POSITION);
		m_mru.SetMenuHandle(hMruMenu);

		m_mru.ReadFromRegistry(kPadRegKey);
		
	//set drop down style
		CToolBarCtrl m_tbar(hWndToolBar);
		CToolBarCtrl m_fbar(hWndToolBarFormat);
		
		m_tbar.SetExtendedStyle(TBSTYLE_EX_DRAWDDARROWS); // | TBSTYLE_EX_HIDECLIPPEDBUTTONS
		m_fbar.SetExtendedStyle(TBSTYLE_EX_DRAWDDARROWS); 

		TBBUTTONINFO tbi;
		tbi.cbSize = sizeof TBBUTTONINFO;
		tbi.dwMask = TBIF_STYLE;
		tbi.fsStyle = TBSTYLE_DROPDOWN;
		
		m_tbar.SetButtonInfo(ID_FILE_OPEN, &tbi);
		m_fbar.SetButtonInfo(ID_FORMAT_COLOR, &tbi);
		m_fbar.SetButtonInfo(ID_FORMAT_BGRN, &tbi);
		m_fbar.SetButtonInfo(ID_FORMAT_HIGHLIGHT, &tbi);

		::SendMessage(m_hWndToolBar, RB_SHOWBAND, 1, m_bShowToolBar);	// toolbar is band #1
		::SendMessage(m_hWndToolBar, RB_SHOWBAND, 2, m_bShowFormatBar);	// toolbar is band #2
		::ShowWindow(m_hWndStatusBar, m_bShowStatusBar ? SW_SHOWNOACTIVATE : SW_HIDE);

		CRegKey key;			
		if (ERROR_SUCCESS == key.Create(HKEY_CURRENT_USER, kPadRegKey))
		{
			RestoreBandInfo(key);
			m_defOption.Load(key);
		}
		
		Send_RefreshStatusBar(_T("Ready"));		
		return 0;
	}

	LRESULT OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
	{
		if (m_hWndToolBarPP)
			RemovePrintBand();

		CRegKey key;			
		if (ERROR_SUCCESS == key.Create(HKEY_CURRENT_USER, kPadRegKey))
		{
			if (!IsIconic())
			{
				RECT rc;
				GetWindowRect(&rc);
				RegSetValueEx(key, _T("WindowRect"), 0, REG_BINARY, (CONST BYTE *)&rc, sizeof RECT);	
			}

			key.SetValue(m_bShowToolBar,   _T("ToolBar"));
			key.SetValue(m_bShowFormatBar, _T("FormatBar"));
			key.SetValue(m_bShowStatusBar, _T("StatusBar"));
			
			SaveBandsInfo(key);
			m_defOption.Save(key);
		}

		m_mru.WriteToRegistry(kPadRegKey);
		bHandled = FALSE;		
		return 0;
	}

	LRESULT OnClose(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
	{
		bHandled = FALSE;

		for (unsigned int i = 0; i < m_view.Count(); i++)
		{			
			if (IDCANCEL == QuerySave(m_view.GetAt(i))) {
				Send_RefreshStatusBar(_T("Ok. Cancel exit"));
				bHandled = TRUE;
				break;
			}
		}
		return 0;
	}
	
	LRESULT OnSetFocus(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		BOOL b;
		OnRefreshKeyboardLayout(0, 0, 0, b);

		if(!m_view.IsEmpty()) {
			kSheetView* sheet =  m_view.GetAt(m_view.Active());		
			sheet->SetFocus(); 
		}
		return 0;
	}
	
	LRESULT OnFileExit(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		PostMessage(WM_CLOSE);
		return 0;
	}

	LRESULT OnFileNew(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		kSheetView* sheet = m_view.Add();		
		sheet->InitNew(); 
		sheet->SetOption(m_defOption);
		sheet->SetModify(FALSE);
		m_view.Active(m_view.Count() - 1);

		Send_RefreshStatusBar(_T("Ok. Created new file %s"), sheet->Name());
		return 0;
	}

	LRESULT OnFileNewWindow(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		::PostThreadMessage(_Module.m_dwMainThreadID, WM_USER, 0, 0L);
		Send_RefreshStatusBar(_T("Ok. Created new window"));
		return 0;
	}

	LRESULT OnFileOpen(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		CFileDialog dlg(TRUE, 0, _T(""), OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_EXPLORER, sFilter);
		int r = dlg.DoModal();

		if (r == IDOK)
		{
			DoFileOpen(dlg.m_ofn.lpstrFile);				
		}				
		return 0;
	}	

	LRESULT OnFileRecent(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{			
		TCHAR sFile[MAX_PATH];
		if(m_mru.GetFromList(wID, sFile))
		{										
			if(DoFileOpen(sFile, false))
				m_mru.MoveToTop(wID);
			else
				m_mru.RemoveFromList(wID);													
		}
		else
		{
			::MessageBeep(MB_ICONERROR);
			Send_RefreshStatusBar(_T("Error. Cannot get file name from mru list"));
		}
		return 0;
	}

	LRESULT OnFileSave(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		ATLASSERT(!m_view.IsEmpty());

		kSheetView* sheet =  m_view.GetAt(m_view.Active());
				
		if ((ID_FILE_SAVE == wID) && lstrlen(sheet->Path())) 
		{
			DoFileSave(sheet, 0);
			return 0;
		}		
		
		
		CFileDialog dlg(FALSE, 0, sheet->Name(), OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_EXPLORER, sFilter);
		dlg.m_ofn.nFilterIndex = 3; //.rtf
		int r = dlg.DoModal();
		if (r != IDOK)
			return 0;			
			
		if (dlg.m_ofn.nFilterIndex != 1) //*.*				
		{
			const TCHAR* p = kSheetView::RFind(dlg.m_ofn.lpstrFile, _T("\\.")); //fix
			if ((p == 0) || (*p == _T('\\'))) 
			{
				if (dlg.m_ofn.nFilterIndex == 2)
					lstrcat(dlg.m_ofn.lpstrFile, _T(".txt"));
				else if (dlg.m_ofn.nFilterIndex == 3)
					lstrcat(dlg.m_ofn.lpstrFile, _T(".rtf"));
			} 								
		}

		bool bUpdateTitle = lstrcmp(dlg.m_ofn.lpstrFile, sheet->Path()) != 0;

		if (DoFileSave(sheet, dlg.m_ofn.lpstrFile) && bUpdateTitle)
		{
			UpdateTitle();
			m_view.Invalidate();
			m_view.UpdateWindow();
		}
		return 0;		
	}

	LRESULT OnFileClose(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		ATLASSERT(!m_view.IsEmpty());
		kSheetView* sheet =  m_view.GetAt(m_view.Active());
		if (IDCANCEL != QuerySave(sheet)) 
		{						
			Send_RefreshStatusBar(_T("Ok. %s closed"), lstrlen(sheet->Path()) ? sheet->Path() : sheet->Name() );
			m_view.Close(m_view.Active());				
		}

		return 0;
	}

	LRESULT OnViewToolBar(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{				
		m_bShowToolBar = !m_bShowToolBar;

		CReBarCtrl rebar(m_hWndToolBar);
		UINT iIndex = rebar.IdToIndex(ATL_IDW_BAND_FIRST + 1); // toolbar is band #1		
		rebar.ShowBand(iIndex, m_bShowToolBar);
		UISetCheck(ID_VIEW_TOOLBAR, m_bShowToolBar);
		UpdateLayout();
		return 0;
	}

	LRESULT OnViewFormat(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		m_bShowFormatBar = !m_bShowFormatBar;

		CReBarCtrl rebar(m_hWndToolBar);
		UINT iIndex = rebar.IdToIndex(ATL_IDW_BAND_FIRST + 2); // formatbar is band #2		
		rebar.ShowBand(iIndex, m_bShowFormatBar);		
		UISetCheck(ID_VIEW_FORMAT, m_bShowFormatBar);
		UpdateLayout();
		return 0;
	}
	
	LRESULT OnViewStatusBar(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		m_bShowStatusBar = !::IsWindowVisible(m_hWndStatusBar);
		::ShowWindow(m_hWndStatusBar, m_bShowStatusBar ? SW_SHOWNOACTIVATE : SW_HIDE);
		UISetCheck(ID_VIEW_STATUS_BAR, m_bShowStatusBar);
		UpdateLayout();
		return 0;
	}
	
	LRESULT OnOption(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		kOptionDlg *dlg = 0;

		if (m_view.IsEmpty())
		{
			dlg = new kOptionDlg( m_defOption, kOptionDlg::applyDefault );
		}
		else
		{
			kOption option;
			m_view.GetAt(m_view.Active())->GetOption(option);
			dlg = new kOptionDlg( option, kOptionDlg::applyActive );
		}
		
		if ((IDOK == dlg->DoModal()) || (0 != dlg->m_nChange))
		{			
			switch (dlg->m_nApply)
			{				
				case kOptionDlg::applyDefault: 
					m_defOption = dlg->m_option;
					break;
				case kOptionDlg::applyActive: 
					m_view.GetAt(m_view.Active())->SetOption(dlg->m_option, dlg->m_nChange);
					break;				
				case kOptionDlg::applyActiveDefault: 	
					m_defOption = dlg->m_option;
					m_view.GetAt(m_view.Active())->SetOption(dlg->m_option, dlg->m_nChange);
					break; 				
				default:
					ATLASSERT(0);
			}
		}

		delete dlg;					
		return 0;
	}

	LRESULT OnAppAbout(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		CAboutDlg dlg;
		dlg.DoModal();
		return 0;
	}

	LRESULT OnContextMenu(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		if (!m_view.IsEmpty()) 
		{
			if ((HWND)wParam == m_view.m_hWnd)
			{
				CMenu menuContext;
				menuContext.LoadMenu(IDR_CONTEXTMENU);
				CMenuHandle menuPopup(menuContext.GetSubMenu(0));
				m_CmdBar.TrackPopupMenu(menuPopup, TPM_LEFTALIGN | TPM_RIGHTBUTTON, LOWORD(lParam), HIWORD(lParam));
			}

			return 0;
		}
		
		bHandled = FALSE;		
		return 0;
	}

	void UpdateUIAll()
	{		
		bool bViewExists = !m_view.IsEmpty();
		
		bool bCanUndo = false;
		bool bCanRedo = false;
		bool bCanPaste = false;
		bool bHasSelection = false;
		bool bNotEmpty = false;
		bool bWordWrap = false;	
		bool bIsBold = false;
		bool bIsItalic = false;
		bool bIsUnderline = false;
		bool bIsLeftAlign = false;
		bool bIsRightAlign = false;
		bool bIsCntAlign = false;
		bool bIsJustify = false;
		bool bUpdateTitle = true;
		
		m_comboFont.EnableWindow(bViewExists);
		m_comboSize.EnableWindow(bViewExists);

		if (bViewExists) 
		{
			kSheetView* sheet =  m_view.GetAt(m_view.Active());

			bCanUndo = sheet->CanUndo() != 0;
			bCanRedo = sheet->CanRedo() != 0;
			bCanPaste = sheet->CanPaste() != 0;  
			bHasSelection = sheet->HasSelection() != 0;
			bNotEmpty = sheet->GetWindowTextLength() > 0;
			bWordWrap = sheet->IsWordWrap();
			bIsBold = sheet->IsEffect(CFM_BOLD, CFE_BOLD);
			bIsItalic = sheet->IsEffect(CFM_ITALIC, CFE_ITALIC);
			bIsUnderline = sheet->IsEffect(CFM_UNDERLINE, CFE_UNDERLINE);
			
			switch (sheet->GetAlign()) 
			{
				case PFA_LEFT: bIsLeftAlign = true;		break;
				case PFA_RIGHT: bIsRightAlign = true;	break;
				case PFA_CENTER: bIsCntAlign = true;	break;
				case PFA_JUSTIFY: bIsJustify = true;	break;
				default: break;
			}

			// update toolbar combo box
			if (GetFocus() == sheet->m_hWnd) 
			{
				CHARFORMAT2 cf;
				TCHAR szName[LF_FACESIZE];
				m_comboFont.GetWindowText(szName, LF_FACESIZE );								
				sheet->GetCharFormatEx(cf, CFM_FACE | CFM_SIZE); 

				if (lstrcmp(szName, cf.szFaceName))				
					if (m_comboFont.SetSelFont(cf) == CB_ERR)
						m_comboFont.SetCurSel( -1 );

				int iComboSize = 0;
				int i = m_comboSize.GetCurSel();				
				if (i != CB_ERR) iComboSize = font_size[i];
				
				int iSelSize = cf.yHeight / 20;
				if (iComboSize != iSelSize) 
				{
					TCHAR str[10];
					wsprintf(str, _T("%d"), iSelSize);
					if (m_comboSize.SelectString(-1, str) == CB_ERR)
						m_comboSize.SetCurSel( -1 );
				}
			}

			bUpdateTitle = m_bModified != sheet->IsDirty();
			OnRefreshColRow();
		}
		
		if (bUpdateTitle)
		{
			UpdateTitle();
		}
		
		UIEnable(ID_FILE_SAVE, bViewExists);
		UIEnable(ID_FILE_SAVE_AS, bViewExists);
		UIEnable(ID_FILE_CLOSE, bViewExists);
		UIEnable(ID_FILE_PRINT, bViewExists);
		UIEnable(ID_FILE_PRINT_PREVIEW, bViewExists);
		UIEnable(ID_FILE_PRINT_SETUP, bViewExists);
		UIEnable(ID_EDIT_WORD_WRAP, bViewExists);		
		UIEnable(ID_FORMAT_FONT, bViewExists);
		UIEnable(ID_FORMAT_COLOR, bViewExists);
		UIEnable(ID_FORMAT_BGRN, bViewExists);
		UIEnable(ID_FORMAT_HIGHLIGHT, bViewExists);
		UIEnable(ID_FORMAT_BOLD, bViewExists);
		UIEnable(ID_FORMAT_ITALIC, bViewExists);
		UIEnable(ID_FORMAT_UNDERLINE, bViewExists);
		UIEnable(ID_FORMAT_CNT, bViewExists);
		UIEnable(ID_FORMAT_LEFT, bViewExists);
		UIEnable(ID_FORMAT_RIGHT, bViewExists);
		UIEnable(ID_FORMAT_JST, false);	//bViewExists
		UIEnable(ID_EDIT_UNDO, bCanUndo);
		UIEnable(ID_EDIT_REDO, bCanRedo);		
		UIEnable(ID_EDIT_PASTE, bCanPaste);
		UIEnable(ID_EDIT_CUT, bHasSelection);
		UIEnable(ID_EDIT_COPY, bHasSelection);
		UIEnable(ID_EDIT_CLEAR, bHasSelection);		
		UIEnable(ID_EDIT_SELECT_ALL, bNotEmpty);
		UIEnable(ID_EDIT_FIND, bNotEmpty);
		UIEnable(ID_EDIT_REPEAT, bNotEmpty);
		UIEnable(ID_EDIT_REPLACE, bNotEmpty);
		UISetCheck(ID_EDIT_WORD_WRAP, bWordWrap);
		UISetCheck(ID_FORMAT_BOLD, bIsBold);
		UISetCheck(ID_FORMAT_ITALIC, bIsItalic);
		UISetCheck(ID_FORMAT_UNDERLINE, bIsUnderline);
		UISetCheck(ID_FORMAT_LEFT, bIsLeftAlign);
		UISetCheck(ID_FORMAT_RIGHT, bIsRightAlign);
		UISetCheck(ID_FORMAT_CNT, bIsCntAlign);
		UISetCheck(ID_FORMAT_JST, bIsJustify);
		
		UIUpdateToolBar();		
		UpdateStatusBar();
	}

	LRESULT OnRefreshKeyboardLayout (UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& /*bHandled*/)
	{
		TCHAR sLang[MAX_PATH] = {0};
		HKL hkl = (HKL)lParam;
		if (0 == hkl) 
		{			
			hkl = GetKeyboardLayout(GetCurrentThreadId());
		}

		GetLocaleInfo(MAKELCID(hkl, SORT_DEFAULT), LOCALE_SLANGUAGE, sLang, MAX_PATH);
		m_sbar.SetText(1, sLang);
		return 0;
	}

	LRESULT OnRefreshColRow()	
	{		
		ATLASSERT(!m_view.IsEmpty());

		kSheetView* sheet = m_view.GetAt(m_view.Active());

		int nRow = 0; int nCol = 0;
		sheet->GetCurrentRowCol(nRow, nCol);
		
		TCHAR szBuff[100];
		wsprintf(szBuff, _T("Row: %i"), nRow);
		m_sbar.SetPaneText(ID_ROW_PANE, szBuff);
		wsprintf(szBuff, _T("Col: %i"), nCol);
		m_sbar.SetPaneText(ID_COL_PANE, szBuff);

		return 0;
	}	

	LRESULT OnToolbarDropDown(int /*idCtrl*/, LPNMHDR pnmh, BOOL& /*bHandled*/)	
	{
		NMTOOLBAR* ptb = (NMTOOLBAR *) pnmh;		
		ATLASSERT(ptb->iItem == ID_FILE_OPEN);

		RECT rc;

		CToolBarCtrl tbar(pnmh->hwndFrom);	
		BOOL b = tbar.GetItemRect(tbar.CommandToIndex(ptb->iItem), &rc);	
		b;
		ATLASSERT(b);			
		tbar.MapWindowPoints(HWND_DESKTOP, (POINT *)&rc, 2);
				
		CMenuHandle menuPopup(m_mru.GetMenuHandle());
		m_CmdBar.TrackPopupMenu(menuPopup, TPM_LEFTALIGN | TPM_RIGHTBUTTON, rc.left, rc.bottom);		
		return 0;
	}

	HWND CreateFormatToolbar ()
	{
		HWND hWndToolBarFormat = CreateSimpleToolBarCtrl(m_hWnd, IDR_FORMAT, FALSE, ATL_SIMPLE_TOOLBAR_PANE_STYLE, ID_FORMAT_TOOLBAR);

		if (!hWndToolBarFormat) 
			return 0;
				
		CToolBarCtrl m_fbar(hWndToolBarFormat);
				
		CClientDC dc(m_hWnd);
		dc.SelectFont((HFONT) GetStockObject( DEFAULT_GUI_FONT ));		
		TEXTMETRIC tm;
		dc.GetTextMetrics( &tm );
		int cxChar = tm.tmAveCharWidth;
		int cyChar = tm.tmHeight + tm.tmExternalLeading;
		int cx = (LF_FACESIZE + 4) * cxChar;
		int cy = 16 * cyChar;
		
		RECT rc;

		TBBUTTONINFO tbi;
		tbi.cbSize = sizeof TBBUTTONINFO;		
		tbi.dwMask = TBIF_STYLE | TBIF_SIZE;
		tbi.fsStyle = TBSTYLE_SEP;
		tbi.cx = (unsigned short)cx;
		
		m_fbar.SetButtonInfo(1, &tbi); 						
		m_fbar.GetItemRect(0, &rc); 

	//	int left = rc.left; 
		rc.right -= cxChar * 8;
		rc.bottom = cy;
		
		HWND hWndFont =  m_comboFont.CreateEx(m_hWnd, rc, -1, IDC_FONTNAME); 
		hWndFont;
		ATLASSERT(hWndFont != 0);
		
		rc.left = rc.right + 2; rc.right = rc.left + cxChar * 8 - 2;
		HWND hWndSize =  m_comboSize.Create(m_hWnd, rc, 0, WS_CHILD|WS_TABSTOP|WS_VSCROLL|WS_VISIBLE|CBS_DROPDOWN, 0, IDC_FONTSIZE, 0);
		hWndSize;
		ATLASSERT(hWndSize != 0);
		
		m_comboSize.SetParent (hWndToolBarFormat);
		m_comboFont.SetParent(hWndToolBarFormat); 
		m_comboSize.SetFont((HFONT)GetStockObject( DEFAULT_GUI_FONT ));
		
		m_comboSize.LimitText(4);

		TCHAR str[10];
		
		for( int i=0; i < sizeof font_size / sizeof font_size[0]; i++ )
		{
			wsprintf(str, "%d", font_size[i] );			
			m_comboSize.AddString( str );
		}
		
		return hWndToolBarFormat;
	}
	
	LRESULT OnSelectFontName(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{		
		ATLASSERT(!m_view.IsEmpty());

		CHARFORMAT2 cf; 
		memset(&cf, 0, sizeof CHARFORMAT2);
		
		m_comboFont.GetLBText( m_comboFont.GetCurSel(), cf.szFaceName );						

		kSheetView* sheet =  m_view.GetAt(m_view.Active());			
		sheet->SetCharFormatEx(cf, CFM_FACE); 		
		return 0;
	}

	LRESULT OnSelectFontSize(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{	
		ATLASSERT(!m_view.IsEmpty());

		CHARFORMAT2 cf; 
		memset(&cf, 0, sizeof CHARFORMAT2);
				
		cf.yHeight = 160; //e.g. 8 point
		int i = m_comboSize.GetCurSel();				
		if (i != CB_ERR) 
			cf.yHeight = font_size[i] * 20;
				
		kSheetView* sheet =  m_view.GetAt(m_view.Active());			
		sheet->SetCharFormatEx(cf, CFM_SIZE); 				
		return 0;
	}

	LRESULT UpdateTitle()
	{		
		TCHAR sTitle[256] = {0};
		::LoadString(_Module.GetResourceInstance(), IDR_MAINFRAME, sTitle, 256);

		if (!m_view.IsEmpty()) 
		{
			kSheetView* sheet =  m_view.GetAt(m_view.Active());
	
			lstrcat(sTitle, _T(" - "));
			lstrcat(sTitle, sheet->Name());
			if (sheet->IsDirty())
				lstrcat(sTitle, _T("*"));
			m_bModified = sheet->IsDirty();
		}	
		
		SetWindowText(sTitle);
		return 0;
	}

	LRESULT OnChangeSheet(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)	
	{
		UpdateTitle();
		return 0;
	}

	DWORD QuerySave(kSheetView* sheet)
	{
		if (!sheet->IsDirty()) 
			return IDYES;
				
		TCHAR sTitle[256] = {0};
		::LoadString(_Module.GetResourceInstance(), IDR_MAINFRAME, sTitle, 256);


		TCHAR sMsg[_MAX_PATH + 20];
		wsprintf(sMsg, _T("Save changes to %s?"), lstrlen(sheet->Path()) ? sheet->Path() : sheet->Name());
		DWORD dwRes = MessageBox (sMsg, sTitle, MB_YESNOCANCEL | MB_ICONQUESTION | MB_DEFBUTTON1);

		if (IDYES == dwRes)
		{
			BOOL b;
			OnFileSave(0, ID_FILE_SAVE, 0, b);
		}		
		return dwRes;		
	}	

	virtual bool DoFileOpen (const TCHAR* sPath, bool bUpdateRecent = true)
	{
		kSheetView* sheet = m_view.Add();	
		sheet->SetOption(m_defOption);
		sheet->SetModify(FALSE);
	
		UpdateProgressBar();
		CWaitCursor wait;

		bool b = sheet->Load(sPath);

		UpdateProgressBar(false);
				
		if (b)
		{
			m_view.Active(m_view.Count() - 1);			
			if (bUpdateRecent)			
				m_mru.AddToList(sPath);	
			
		}
		else
		{			
			m_view.Close(m_view.Count() - 1);			
		}

		Send_RefreshStatusBar(b ? _T("Ok. %s opened.") : 
			_T("Error. Cannot open file %s"), sPath);
		return b;
	}

	bool DoFileSave (kSheetView* sheet, const TCHAR* sPath)
	{	
		UpdateProgressBar();
		CWaitCursor wait;

		bool b = sheet->Save(sPath);

		UpdateProgressBar(false);
				
		if (b && sPath)
		{
			m_mru.AddToList(sPath);						
		}

		Send_RefreshStatusBar(b ? _T("Ok. %s saved") : _T("Error. Cannot save %s"), sheet->Path());		
		return b;
	}


	void UpdateStatusBar()
	{
		#define TIME_DELAY (1000 * 30)

		if (m_dwTimeRefreshStatusBar && 
			((GetTickCount() - m_dwTimeRefreshStatusBar) > TIME_DELAY)) 
		{
			m_sbar.SetText(0, _T("Ready"), SBT_NOBORDERS);
			m_dwTimeRefreshStatusBar = 0;
		}	

	}

	void UpdateProgressBar ( bool b = true )
	{
		if (b)
		{
			Send_RefreshStatusBar(_T(""));
			ATLASSERT(!m_progress.IsWindow());

			RECT rc;

			m_sbar.GetPaneRect(0, &rc);
			InflateRect(&rc, -2, -3);		
			rc.right = min(100, rc.right);
					
			HWND hWnd = m_progress.Create(m_sbar, rc, 0, WS_CHILD|WS_VISIBLE);
			hWnd;
			ATLASSERT(hWnd != 0);		

		}
		else
		{
			ATLASSERT(m_progress.IsWindow());
			m_progress.DestroyWindow();
		}
	}

	void ReadFromRegistry()
	{
		CRegKey key;			
		if (ERROR_SUCCESS == key.Open(HKEY_CURRENT_USER, kPadRegKey))
		{
			//key.QueryValue(m_bShowToolBar,   _T("ToolBar"));
			//key.QueryValue(m_bShowFormatBar, _T("FormatBar"));
			//key.QueryValue(m_bShowStatusBar, _T("StatusBar"));			
		}		
	}

	////////////////////
	// IFrame interface

	virtual HWND GetWindow()
	{
		return m_hWnd;
	}

	virtual CProgressBarCtrl* GetProgressBar()
	{
		return &m_progress;
	}

	virtual void Send_RefreshStatusBar(const TCHAR* pStr, ...)
	{		
		ATLASSERT(pStr != 0);
		
		va_list args;	
		va_start(args, pStr);

		TCHAR buffer[512] = {0};

		int nLen = wvsprintf(buffer, pStr, args);
		nLen;

		ATLASSERT(nLen < 512);
		ATLASSERT(nLen == lstrlen(buffer));

		m_dwTimeRefreshStatusBar = GetTickCount();
		m_sbar.SetText(0, buffer, SBT_NOBORDERS);

		va_end(args);
	}

	////////
	// save\restore bands

	bool SaveBandsInfo (HKEY hKeyParent)
	{
		CRegKey key;
		if (ERROR_SUCCESS != key.Create(hKeyParent, _T("ReBar")))
			return false;

		CReBarCtrl rebar(m_hWndToolBar);
		int nCount = rebar.GetBandCount();

		REBARBANDINFO rbbi;
		rbbi.cbSize = sizeof REBARBANDINFO;
		rbbi.fMask = RBBIM_SIZE | RBBIM_STYLE | RBBIM_ID;
				
		for (int x = 0; x < nCount; x++)
		{
			TCHAR sBand[10] = {0};
			rbbi.wID = 0;

			rebar.GetBandInfo(x, &rbbi);
			UINT iIndex = rebar.IdToIndex(rbbi.wID);						
			wsprintf(sBand, _T("%u"), rbbi.wID);
			key.SetValue(MAKELONG(rbbi.cx, MAKEWORD(rbbi.fStyle & RBBS_BREAK, iIndex)), sBand);	
		}
		return true;
	}

	bool RestoreBandInfo (HKEY hKeyParent)
	{
		CRegKey key;
		if (ERROR_SUCCESS != key.Open(hKeyParent, _T("ReBar")))
			return false;

		CReBarCtrl rebar(m_hWndToolBar);
		int nCount = rebar.GetBandCount();

		REBARBANDINFO rbbi;
		rbbi.cbSize = sizeof REBARBANDINFO;
		
		for (int x = 0; x < nCount; x++)
		{	
			TCHAR sBand[10] = {0};
			DWORD dwValue = 0;

			rbbi.wID = ATL_IDW_BAND_FIRST + x;
			wsprintf(sBand, _T("%u"), rbbi.wID);
			key.QueryValue(dwValue, sBand);			
			
			rbbi.cx = LOWORD(dwValue);
			WORD w = HIWORD(dwValue);
			bool bBreak = LOBYTE(w) != 0;
			UINT uiIndex = (UINT)HIBYTE(w);

			UINT uiCurrent = rebar.IdToIndex(rbbi.wID);
			if (uiCurrent != uiIndex)
				rebar.MoveBand(uiCurrent, uiIndex);

			rbbi.fMask = RBBIM_STYLE;
			rebar.GetBandInfo(uiIndex, &rbbi);
			
			if (bBreak)
				rbbi.fStyle |= RBBS_BREAK;
			else
				rbbi.fStyle &= ~RBBS_BREAK;

			rbbi.fMask = RBBIM_SIZE | RBBIM_STYLE | RBBIM_ID;
			rebar.SetBandInfo(uiIndex, &rbbi);			
		}		
		return true;
	}

	//////////////////////////////////////////////////
	// print

	LRESULT OnFilePrint(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		if(!LayoutPages())
		{
			MessageBox(_T("Print operation failed"), _T("kPad"), MB_ICONERROR | MB_OK);
			return 0;
		}

		CPrintDialog dlg(FALSE);
		dlg.m_pd.hDevMode = devmode.CopyToHDEVMODE();
		dlg.m_pd.hDevNames = printer.CopyToHDEVNAMES();
		dlg.m_pd.nMinPage = 1;
		dlg.m_pd.nMaxPage = (WORD)m_arrPages.GetSize();
		dlg.m_pd.nFromPage = 1;
		dlg.m_pd.nToPage = (WORD)m_arrPages.GetSize();
		dlg.m_pd.Flags &= ~PD_NOPAGENUMS;

		if (dlg.DoModal() == IDOK)
		{
			devmode.CopyFromHDEVMODE(dlg.m_pd.hDevMode);
			printer.ClosePrinter();
			printer.OpenPrinter(dlg.m_pd.hDevNames, devmode.m_pDevMode);

			CPrintJob job;
			int nMin=0;
			int nMax = m_arrPages.GetSize()-1;
			if (dlg.m_pd.Flags | PD_PAGENUMS)
			{
				nMin = dlg.m_pd.nFromPage-1;
				nMax = dlg.m_pd.nToPage-1;
			}

			job.StartPrintJob(false, printer, devmode.m_pDevMode, this, _T("kPad Document"), nMin, nMax);
		}

		GlobalFree(dlg.m_pd.hDevMode);
		GlobalFree(dlg.m_pd.hDevNames);

		return 0;
	}

	LRESULT OnFilePrintPreview(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		bool bRet = LayoutPages();
		if(!bRet)
		{
			MessageBox(_T("Print preview operation failed"), _T("kPad"), MB_ICONERROR | MB_OK);
			return 0;
		}

		prev.SetPrintPreviewInfo(printer, devmode.m_pDevMode, this, 0, m_arrPages.GetSize() - 1);
		prev.SetPage(0);
		RECT rcPos;
		GetClientRect(&rcPos);
		prev.Create(m_hWnd, rcPos);
		prev.ShowWindow(SW_SHOW);
		prev.SetWindowPos(HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE|SWP_NOSIZE|SWP_SHOWWINDOW);

		m_view.ShowWindow(SW_HIDE);		
		m_hWndClient = prev;

		ATLASSERT(m_hWndToolBarPP == 0);
		m_hWndToolBarPP = CreateSimpleToolBarCtrl(m_hWnd, IDR_PRINTPREVIEWBAR, FALSE, ATL_SIMPLE_TOOLBAR_PANE_STYLE, ID_PRINT_TOOLBAR);		
		AddSimpleReBarBandCtrl(m_hWndToolBar, m_hWndToolBarPP, ATL_IDW_BAND_FIRST + 3, 0, TRUE, 0, TRUE);

		CReBarCtrl rebar(m_hWndToolBar);
		rebar.ShowBand(rebar.IdToIndex(ATL_IDW_BAND_FIRST + 1), FALSE); // toolbar 
		rebar.ShowBand(rebar.IdToIndex(ATL_IDW_BAND_FIRST + 2), FALSE); // formatbar

		UpdateLayout();
		return 0;
	}

	LRESULT OnFilePageSetup(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		CPageSetupDialog dlg;
		dlg.m_psd.hDevMode = devmode.CopyToHDEVMODE();
		dlg.m_psd.hDevNames = printer.CopyToHDEVNAMES();
		dlg.m_psd.rtMargin = m_rcMargin;

		if (dlg.DoModal() == IDOK)
		{
			devmode.CopyFromHDEVMODE(dlg.m_psd.hDevMode);
			printer.ClosePrinter();
			printer.OpenPrinter(dlg.m_psd.hDevNames, devmode.m_pDevMode);
			m_rcMargin = dlg.m_psd.rtMargin;
		}

		GlobalFree(dlg.m_psd.hDevMode);
		GlobalFree(dlg.m_psd.hDevNames);

		return 0;
	}

	LRESULT OnPrintPreviewClose(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		prev.DestroyWindow();
		m_hWndClient = m_view;
		m_view.ShowWindow(SW_SHOW);		
		RemovePrintBand();
		UpdateLayout();
		return 0;
	}

	LRESULT OnPrintPreviewForward(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		prev.NextPage();
		return 0;
	}
	
	LRESULT OnPrintPreviewBack(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		prev.PrevPage();					
		return 0;
	}

	//print job info callback
	virtual bool IsValidPage(UINT /*nPage*/)
	{
		return true;
	}

	virtual bool PrintPage(UINT nPage, HDC hDC)
	{
		if (nPage >= (UINT)m_arrPages.GetSize())
			return false;

		RECT rcPage;
		rcPage.left = rcPage.top = 0;
		rcPage.bottom = GetDeviceCaps(hDC, PHYSICALHEIGHT);
		rcPage.right = GetDeviceCaps(hDC, PHYSICALWIDTH);

		rcPage.right = MulDiv(rcPage.right, 1440, GetDeviceCaps(hDC, LOGPIXELSX));
		rcPage.bottom = MulDiv(rcPage.bottom, 1440, GetDeviceCaps(hDC, LOGPIXELSY));

		RECT rcOutput = rcPage;
		//convert from 1/1000" to twips
		rcOutput.left += MulDiv(m_rcMargin.left, 1440, 1000);
		rcOutput.right -= MulDiv(m_rcMargin.right, 1440, 1000);
		rcOutput.top += MulDiv(m_rcMargin.top, 1440, 1000);
		rcOutput.bottom -= MulDiv(m_rcMargin.bottom, 1440, 1000);

		
		FORMATRANGE fr;
		fr.hdc = hDC;
		fr.hdcTarget = hDC;
		fr.rc = rcOutput;
		fr.rcPage = rcPage;
		fr.chrg.cpMin = 0;
		fr.chrg.cpMax = -1;
		fr.chrg.cpMin = m_arrPages[nPage];

		// We have to adjust the origin because 0,0 is not at the corner of the paper
		// but is at the corner of the printable region
		int nOffsetX = GetDeviceCaps(hDC, PHYSICALOFFSETX);
		int nOffsetY = GetDeviceCaps(hDC, PHYSICALOFFSETY);
		SetViewportOrgEx(hDC, -nOffsetX, -nOffsetY, NULL);

		kSheetView* sheet = m_view.GetAt(m_view.Active());
		
		sheet->SendMessage(EM_FORMATRANGE, TRUE, (LPARAM)&fr);
		sheet->SendMessage(EM_DISPLAYBAND, 0, (LPARAM)&rcOutput);

		//Cleanup cache in richedit
		sheet->SendMessage(EM_FORMATRANGE, FALSE, 0);
		return true;
	}

	bool LayoutPages()
	{
		CDC dc = printer.CreatePrinterDC(devmode);
		if(dc.IsNull())
			return false;

		RECT rcPage;
		rcPage.left = rcPage.top = 0;
		rcPage.bottom = GetDeviceCaps(dc, PHYSICALHEIGHT);
		rcPage.right = GetDeviceCaps(dc, PHYSICALWIDTH);
		// We have to adjust the origin because 0,0 is not at the corner of the paper
		// but is at the corner of the printable region
		int nOffsetX = dc.GetDeviceCaps(PHYSICALOFFSETX);
		int nOffsetY = dc.GetDeviceCaps(PHYSICALOFFSETY);
		dc.SetViewportOrg(-nOffsetX, -nOffsetY);
		rcPage.right = MulDiv(rcPage.right, 1440, GetDeviceCaps(dc, LOGPIXELSX));
		rcPage.bottom = MulDiv(rcPage.bottom, 1440, GetDeviceCaps(dc, LOGPIXELSY));

		RECT rcOutput = rcPage;
		//convert from 1/1000" to twips
		rcOutput.left += MulDiv(m_rcMargin.left, 1440, 1000);
		rcOutput.right -= MulDiv(m_rcMargin.right, 1440, 1000);
		rcOutput.top += MulDiv(m_rcMargin.top, 1440, 1000);
		rcOutput.bottom -= MulDiv(m_rcMargin.bottom, 1440, 1000);
		
		FORMATRANGE fr;
		fr.hdc = dc;
		fr.hdcTarget = dc;
		fr.rc = rcOutput;
		fr.rcPage = rcPage;
		fr.chrg.cpMin = 0;
		fr.chrg.cpMax = -1;

		kSheetView* sheet = m_view.GetAt(m_view.Active());

		LONG n = (LONG)sheet->GetTextLength();
		m_arrPages.RemoveAll();
		while (1)
		{
			m_arrPages.Add(fr.chrg.cpMin);
			LONG lRet = (LONG)sheet->SendMessage(EM_FORMATRANGE, FALSE, (LPARAM)&fr);
			if((lRet - fr.chrg.cpMin) == -1)
			{
				m_arrPages.RemoveAt(m_arrPages.GetSize()-1);
				break;
			}
			else
				fr.chrg.cpMin = lRet;
			if (fr.chrg.cpMin >= n)
				break;
		}

		sheet->SendMessage(EM_FORMATRANGE, FALSE, 0);
		return true;
	}
	
	void RemovePrintBand()
	{
		ATLASSERT(::IsWindow(m_hWndToolBarPP));

		CReBarCtrl rebar(m_hWndToolBar);		
		rebar.DeleteBand(rebar.IdToIndex(ATL_IDW_BAND_FIRST + 3)); // delete printbar				
		rebar.ShowBand(rebar.IdToIndex(ATL_IDW_BAND_FIRST + 1), m_bShowToolBar); // toolbar 
		rebar.ShowBand(rebar.IdToIndex(ATL_IDW_BAND_FIRST + 2), m_bShowFormatBar); // formatbar 
		
		::DestroyWindow(m_hWndToolBarPP);
		m_hWndToolBarPP = 0;
	}
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MAINFRM_H__44FA1498_FAD8_47D3_B3DB_6E5CA86E2C2B__INCLUDED_)
