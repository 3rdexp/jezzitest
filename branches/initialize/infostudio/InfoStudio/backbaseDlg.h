#pragma once

#include "Resource.h"
#include "infomanage.h"
#include "./util.h"
#include <atlctrls.h>

class CWebInfoDlg;
extern CInfoManage* _pInfoManage;


class CBackBaseDlg : public CDialogImpl<CBackBaseDlg>
{
public:

	CBackBaseDlg()
	{
		_pWebInfo		= NULL;
		_pWebInfoDlg	= NULL;
	}

	~CBackBaseDlg()
	{
		_pWebInfo = NULL;
	}
	enum {IDD = IDD_BACKBASEDLG};

	BEGIN_MSG_MAP(CBackBaseDlg)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		COMMAND_ID_HANDLER(IDOK, OnOk)
		COMMAND_ID_HANDLER(IDCANCEL, OnCancel)		

		MSG_WM_SIZE(OnSize)
		
		COMMAND_ID_HANDLER(IDC_BUTTON_SAVE, OnSave)
		COMMAND_ID_HANDLER(IDC_BUTTON_ADD, OnAdd)

	END_MSG_MAP()

	LRESULT OnAdd (WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);

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

	
	LRESULT OnSave(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		SaveInfo( _pWebInfo, FALSE );
		return 0;
	}

	

	void OnSize(UINT wParam, CSize size)
	{
		DefWindowProc();
		int cx = size.cx;
		int cy = size.cy;
	}

	//funciton
	void Init()
	{
		WTL::CComboBox box;
		box.Attach(GetDlgItem(IDC_COMBO_LANGUAGE));
		box.Clear();
		box.AddString( _T("1-简体") );
		box.AddString( _T("2-英文") );
		box.AddString( _T("3-繁体") );
		box.SetCurSel( 0 );

		box.Detach();
		box.Attach(GetDlgItem(IDC_COMBO_GRADE));
		box.Clear();
		for ( int i = 0; i < 10; i++ )
		{
			CString str;
			str.Format( _T("%d分"),i + 1 );
			box.AddString( str );
		}
		box.SetCurSel( 5 );

	}

	void ReadInfo( CWebInfo* pWebInfo )
	{
		//读取该网站的基础信息，这些信息都能从原来的数据结构里面读取
		if ( !_pInfoManage )
			return;
		
		_pWebInfo = pWebInfo;

		if( pWebInfo )
		{

			CComboBox box;
			box.Attach(GetDlgItem(IDC_COMBO_LANGUAGE));
			box.SetCurSel( pWebInfo->_language - 1 );
			box.Detach();

			box.Attach(GetDlgItem(IDC_COMBO_GRADE));
			box.SetCurSel( pWebInfo->_grade - 1 );
			box.Detach();

			::SetWindowText( GetDlgItem(IDC_EDIT_NAME), pWebInfo->_strName );
			::SetWindowText( GetDlgItem(IDC_EDIT_HOMEPAGE), pWebInfo->_strWebUrl );
			::SetWindowText( GetDlgItem(IDC_EDIT_REMARK), pWebInfo->_strRemark );

			CheckDlgButton(IDC_CHECK_USE, pWebInfo->_used == 1 ? TRUE : FALSE );
			CheckDlgButton(IDC_CHECK_DEMO, pWebInfo->_demoUsed == 1 ? TRUE : FALSE );

		}
	}

	void SaveInfo(  CWebInfo* pWebInfo, BOOL bAdd )
	{
		if ( !pWebInfo )
			return;

		CComboBox box;
		box.Attach(GetDlgItem(IDC_COMBO_LANGUAGE));
		int nCur =  box.GetCurSel();
		box.Detach();

		pWebInfo->_language = nCur + 1;

		
		box.Attach(GetDlgItem(IDC_COMBO_GRADE));
		nCur =  box.GetCurSel();
		box.Detach();

		pWebInfo->_grade = nCur + 1;

		getWindowText( GetDlgItem(IDC_EDIT_NAME), pWebInfo->_strName );
		getWindowText( GetDlgItem(IDC_EDIT_HOMEPAGE), pWebInfo->_strWebUrl );
		getWindowText( GetDlgItem(IDC_EDIT_REMARK), pWebInfo->_strRemark );

		if ( IsDlgButtonChecked( IDC_CHECK_USE ) )
			pWebInfo->_used = 1;
		else
			pWebInfo->_used = 0;

		if ( IsDlgButtonChecked( IDC_CHECK_DEMO ) )
			pWebInfo->_demoUsed = 1;
		else
			pWebInfo->_demoUsed = 0;

		// 需要保存数据库
		CString strSql;	

		if ( bAdd )
			strSql.Format( _T("select * from weblist ")); //
		else
			strSql.Format( _T("select * from weblist where id = %d "), pWebInfo->_id ); //
		
		CAdoRecordSet* pRs = new CAdoRecordSet(_pDb);

		if( pRs->Open((LPCTSTR) strSql ) )
		{

			if ( bAdd )
			{
				pRs->AddNew();
				pRs->PutCollect( _T("id"), pWebInfo->_id );
			}
			pRs->PutCollect( _T("name"), pWebInfo->_strName );
			pRs->PutCollect( _T("url"), pWebInfo->_strWebUrl );
			pRs->PutCollect( _T("grade"), pWebInfo->_grade );
			pRs->PutCollect( _T("user"), pWebInfo->_strUser );
			pRs->PutCollect( _T("pass"), pWebInfo->_strPass );
			pRs->PutCollect( _T("language"), pWebInfo->_language );
			pRs->PutCollect( _T("used"), pWebInfo->_used );
			pRs->PutCollect( _T("demoused"), pWebInfo->_demoUsed );
			pRs->PutCollect( _T("remark"), pWebInfo->_strRemark );

			pRs->Update();
			
			
		}
		
		if ( pRs )
			delete pRs;

		pRs = NULL;

	}

public:

	CWebInfo* _pWebInfo;

	CWebInfoDlg*	_pWebInfoDlg;
};