#pragma once

#include "Resource.h"
#include "./commonctl/DlgTabCtrl.h"
#include "backbaseDlg.h"
#include "backloginDlg.h"
//#include "weblistDlg.h"

class CWebListDlg;

class CWebInfoDlg : public CDialogImpl<CWebInfoDlg>
{
public:

	CWebInfoDlg()
	{
		_pWebListDlg = NULL;
	}

	~CWebInfoDlg()
	{

	}
	enum {IDD = IDD_WEBINFODLG};

	BEGIN_MSG_MAP(CWebInfoDlg)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		COMMAND_ID_HANDLER(IDOK, OnOk)
		COMMAND_ID_HANDLER(IDCANCEL, OnCancel)		

		MSG_WM_SIZE(OnSize)
		
		NOTIFY_CODE_HANDLER(TCN_SELCHANGE, OnSelChange)
		REFLECT_NOTIFICATIONS()

	END_MSG_MAP()

	BOOL PreTranslateMessage(MSG* pMsg)
	{
		if (pMsg->message == WM_KEYDOWN)
		{
			if(pMsg->wParam == VK_RETURN)
			{
				if(::IsDialogMessage(m_hWnd, pMsg))
					return TRUE;
			}				
		}	
		return FALSE;
	}

	LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		Init();		
		return 0;
	}

	LRESULT OnOk(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{

		return 0;
	}

	LRESULT OnCancel(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{

		return 0;
	}

	void OnSize(UINT wParam, CSize size)
	{
		DefWindowProc();
		int cx = size.cx;
		int cy = size.cy;

		if( !_ctrlTab.IsWindow() || !_ctrlViews.IsWindow() ) 
			return;
		RECT rc;
		GetClientRect(&rc);
		RECT rcPager = { rc.left, rc.top, rc.right, rc.bottom - 24 };
		_ctrlViews.MoveWindow(&rcPager);
		RECT rcTab = { rc.left, rc.bottom - 24, rc.right, rc.bottom };
		_ctrlTab.MoveWindow(&rcTab);
	
	}

	LRESULT OnSelChange(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)
	{
		_ctrlViews.SetCurSel( _ctrlTab.GetCurSel() );
		return 0;
	}

	//funciton
	void Init()
	{
		_ctrlTab.Create(m_hWnd, rcDefault, NULL, WS_VISIBLE | WS_CHILD | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | TCS_BOTTOM | TCS_TOOLTIPS);
		
		TCITEM tci = { 0 };
		tci.mask = TCIF_TEXT | TCIF_IMAGE;
		tci.pszText = _T("基本信息");
		tci.iImage = 0;
		_ctrlTab.InsertItem(0, &tci);

		tci.pszText = _T("登录参数");
		tci.iImage = 1;
		_ctrlTab.InsertItem(1, &tci);
		
		tci.pszText = _T("注册参数");
		tci.iImage = 1;
		_ctrlTab.InsertItem(2, &tci);

		_ctrlTab.SetCurSel(0);

		_ctrlViews.Create(m_hWnd, rcDefault);
		_backBaseDlg.Create( _ctrlViews );
		_backBaseDlg._pWebInfoDlg = this;

		_backLoginDlg.SetType( POST_LOGIN );
		_backLoginDlg.Create( _ctrlViews );
		_backLoginDlg._pWebInfoDlg = this;

		_backRegisterDlg.SetType( POST_REGISTER );
		_backRegisterDlg.Create( _ctrlViews );
		_backRegisterDlg._pWebInfoDlg = this;

		_ctrlViews.AddItem( _backBaseDlg );
		_ctrlViews.AddItem( _backLoginDlg );
		_ctrlViews.AddItem( _backRegisterDlg );

		_ctrlViews.SetCurSel( 0 );
	}

public:
		
	CTabCtrl			_ctrlTab;
	CDlgContainerCtrl	_ctrlViews;
	CBackBaseDlg		_backBaseDlg;
	CBackLoginDlg		_backLoginDlg;
	CBackLoginDlg		_backRegisterDlg;

	CWebListDlg*		_pWebListDlg;

};