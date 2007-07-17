#pragma once

#include <atlctrlx.h>
#include "Resource.h"
#include "webCollect.h"
#include "webinfoDlg.h"

class CBackLoginDlg : public CDialogImpl<CBackLoginDlg>
{
public:

	CBackLoginDlg()
	{
		_pWebInfoDlg	= NULL;
	}

	~CBackLoginDlg()
	{
		_listView.Detach();
		_pWebRegister	= NULL;
		_pWebInfo		= NULL;
	}

	enum {IDD = IDD_BACKLOGINDLG};

	BEGIN_MSG_MAP(CBackLoginDlg)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		COMMAND_ID_HANDLER(IDOK, OnOk)
		COMMAND_ID_HANDLER(IDCANCEL, OnCancel)		

		MSG_WM_SIZE(OnSize)

		COMMAND_ID_HANDLER(IDC_BUTTON_WEB, OnWeb)		

		NOTIFY_HANDLER( IDC_LIST, NM_CLICK, OnClickList)

		COMMAND_ID_HANDLER(IDC_BUTTON_ADD, OnAdd)
		COMMAND_ID_HANDLER(IDC_BUTTON_EDIT, OnEdit)

		//COMMAND_HANDLER(IDC_EDIT_PARAM, EN_CHANGE, OnEnChangeEditParam)
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

	LRESULT OnWeb(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		CWebCollectDlg dlg;
		getWindowText( GetDlgItem(IDC_EDIT_POSTURL), dlg._strUrl );
		dlg.DoModal();
		return 0;
	}
	
	LRESULT OnAdd(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		//保存当前的设置
		CWebCollectDlg dlg;
		dlg._pWebInfo	= _pWebInfo;
		dlg._type		= _type;
		dlg.DoModal();
		if ( dlg._pRegister )
		{
			ShowWebRegisterInfo( dlg._pRegister );
		}
		return 0;
	}

	LRESULT OnEdit(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		if ( !_pWebInfo || !_pWebRegister )
		{
			MessageBox("没有可编辑的数据", MSGTITLE, MB_OK);
			return 0;
		}
		else
		{
			CWebCollectDlg dlg;
			dlg._pWebInfo	= _pWebInfo;
			dlg._pRegister	= _pWebRegister;
			dlg._type		= _type;
			dlg.DoModal();
			if ( dlg._pRegister )
			{
				ShowWebRegisterInfo( dlg._pRegister );
			}
		}
		return 0;
	}
	
	void OnSize(UINT wParam, CSize size)
	{
		DefWindowProc();
		int cx = size.cx;
		int cy = size.cy;
	}

	LRESULT OnClickList(int idCtrl, LPNMHDR pnmh, BOOL& bHandled)
	{
		int nSelect = _listView.SendMessage(LVM_GETNEXTITEM,
			-1,LVNI_FOCUSED); // return item selected

		if ( nSelect != -1 )
		{
			webRegister* pWebRegister = (webRegister*)_listView.GetItemData( nSelect );
			ShowWebRegisterInfo( pWebRegister );
		}
		return 0;
	}

	//funciton
	void Init()
	{
		_listView.Attach( GetDlgItem (IDC_LIST) );
		_listView.SetExtendedListViewStyle( LVS_SHOWSELALWAYS|LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_CHECKBOXES );

		for (int i = 0; i< sizeof(WEBINFOFIELD) / sizeof (WEBINFOFIELD[0]); i++)
		{
			_listView.InsertColumn(i, WEBINFOFIELD[i]);
			_listView.SetColumnWidth(i, WEBINFOFIELDSIZE[i]);
		}

	}

	void ReadInfo( CWebInfo* pWebInfo )
	{
		if ( !pWebInfo )
			return;

		_pWebInfo = pWebInfo;
		// 先把当前的列表读取出来吧。

		_listView.DeleteAllItems();

		// 把登录的列表读取出来
		std::map<int, webRegisterCollect*>::iterator it = pWebInfo->_webRegisterCollectMap.find( _type );
		if( it != pWebInfo->_webRegisterCollectMap.end() )
		{
			//
			webRegisterCollect* pWebRegisterCollect = it->second;
			for ( int i = 0 ; i < pWebRegisterCollect->_webRegisterLst.size(); i++ )
			{
				webRegister* pWebRegister = pWebRegisterCollect->_webRegisterLst[i];

				CString strField;
				strField.Format("%d", pWebRegister->_id);

				LVITEM lvitem;			
				lvitem.mask = LVIF_TEXT | LVIF_IMAGE;
				lvitem.iItem = i;
				lvitem.iSubItem = 0;				
				lvitem.iImage = 0;	
				lvitem.pszText = (LPSTR)strField.GetBuffer(strField.GetLength());
				int iPos = _listView.InsertItem(&lvitem);

				_listView.SetItemData(iPos, (DWORD_PTR) pWebRegister );

				strField = pWebRegister->_url;
				lvitem.mask = LVIF_TEXT;
				lvitem.iItem = iPos;		
				lvitem.iSubItem = 1;
				lvitem.pszText = (LPSTR)strField.GetBuffer(strField.GetLength());
				_listView.SetItem(&lvitem);

				strField = pWebRegister->_posturl;
				lvitem.mask = LVIF_TEXT;
				lvitem.iItem = iPos;		
				lvitem.iSubItem = 2;
				lvitem.pszText = (LPSTR)strField.GetBuffer(strField.GetLength());
				_listView.SetItem(&lvitem);


				strField = pWebRegister->_success;
				lvitem.mask = LVIF_TEXT;
				lvitem.iItem = iPos;		
				lvitem.iSubItem = 3;
				lvitem.pszText = (LPSTR)strField.GetBuffer(strField.GetLength());
				_listView.SetItem(&lvitem);

			}	
		}
		
	}

	void ShowWebRegisterInfo( webRegister* pWebRegister )
	{
		_pWebRegister = pWebRegister;
		//显示每个搜索的单元信息
		::SetWindowText( GetDlgItem(IDC_EDIT_WEB), pWebRegister->_url );
		::SetWindowText( GetDlgItem(IDC_EDIT_POSTURL), pWebRegister->_posturl );
		::SetWindowText( GetDlgItem(IDC_EDIT_VALIDATE), pWebRegister->_success );
		::SetWindowText( GetDlgItem(IDC_EDIT_VALIDATEURL), pWebRegister->_validateUrl);
		::SetWindowText( GetDlgItem(IDC_EDIT_USER), _pWebInfo->_strUser);
		::SetWindowText( GetDlgItem(IDC_EDIT_PASS), _pWebInfo->_strPass);

		CString strInfo = "";
		for( std::map<CString, CString>::iterator it = pWebRegister->_headMap.begin(); it!= pWebRegister->_headMap.end(); it++ )
		{
			strInfo += it->first;
			strInfo += "=";
			strInfo += it->second;
			strInfo += "&";
		}
		if ( strInfo != "" )
			strInfo = strInfo.Left( strInfo.GetLength() );
		::SetWindowText( GetDlgItem(IDC_EDIT_HEADER), strInfo );


		strInfo = "";
		for( std::map<CString, CString>::iterator it = pWebRegister->_postMap.begin(); it!= pWebRegister->_postMap.end(); it++ )
		{
			strInfo += it->first;
			strInfo += "=";
			strInfo += it->second;
			strInfo += "&";
		}
		if ( strInfo != "" )
			strInfo = strInfo.Left( strInfo.GetLength() );
		::SetWindowText( GetDlgItem(IDC_EDIT_PARAM), strInfo );

	}

	void SetType( int nType )
	{
		_type = nType;
	}

public:
	CWebInfo*			_pWebInfo; 
	CSortListViewCtrl	_listView;
	webRegister*		_pWebRegister;
	int					_type;// 登录 注册 等不同的类型
	CWebInfoDlg*		_pWebInfoDlg;
	//LRESULT OnEnChangeEditParam(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
};