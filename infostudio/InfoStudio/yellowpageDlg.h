#pragma once

#include <atlctrlx.h>
#include "Resource.h"
#include "condef.h"
#include "webinfo.h"
#include "infomanage.h"

extern CInfoManage* _pInfoManage;

class CYellowPageDlg : public CDialogImpl<CYellowPageDlg>
{
public:

	CYellowPageDlg()
	{

	}

	~CYellowPageDlg()
	{

	}
	enum {IDD = IDD_YELLOWPAGEDLG};

	BEGIN_MSG_MAP( CYellowPageDlg )
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		COMMAND_ID_HANDLER(IDOK, OnOk)
		COMMAND_ID_HANDLER(IDCANCEL, OnCancel)		

		MSG_WM_SIZE(OnSize)

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
		InitData();
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

		if (_listView.m_hWnd)
			_listView.SetWindowPos(NULL, 0, 0, cx , cy, SWP_NOACTIVATE | SWP_NOZORDER | SWP_NOMOVE);

	}

	//funciton
	void Init()
	{
		_listView.Create(m_hWnd,rcDefault,NULL,
			WS_CHILD|WS_VISIBLE|LVS_REPORT|LVS_SHOWSELALWAYS | WS_VSCROLL | WS_HSCROLL);		

		_listView.SetExtendedListViewStyle( LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_CHECKBOXES );

		for (int i = 0; i< sizeof(LOGINFIELD) / sizeof (LOGINFIELD[0]); i++)
		{
			_listView.InsertColumn(i, LOGINFIELD[i]);
			_listView.SetColumnWidth(i, LOGINFIELDSIZE[i]);
		}
	}

	void InitData()
	{
		int nCount = 0;
		if ( !_pInfoManage )
			return ;

		for( std::map<int, CWebInfo*>::iterator it = _pInfoManage->_webInfoMap.begin(); it!= _pInfoManage->_webInfoMap.end(); it++ )
		{
			CWebInfo* pWebInfo = it->second;

			CString strField;
			strField.Format( "%d", pWebInfo->_id );

			LVITEM lvitem;			
			lvitem.mask = LVIF_TEXT | LVIF_IMAGE;
			lvitem.iItem = nCount;
			lvitem.iSubItem = 0;				
			lvitem.iImage = 0;	
			lvitem.pszText = (LPSTR)strField.GetBuffer(strField.GetLength());
			int iPos = _listView.InsertItem(&lvitem);

			_listView.SetItemData(iPos, (DWORD_PTR) pWebInfo );

			strField = pWebInfo->_strName;
			lvitem.mask = LVIF_TEXT;
			lvitem.iItem = iPos;		
			lvitem.iSubItem = 1;
			lvitem.pszText = (LPSTR)strField.GetBuffer(strField.GetLength());
			_listView.SetItem(&lvitem);

			strField = pWebInfo->_strWebUrl;
			lvitem.mask = LVIF_TEXT;
			lvitem.iItem = iPos;		
			lvitem.iSubItem = 2;
			lvitem.pszText = (LPSTR)strField.GetBuffer(strField.GetLength());
			_listView.SetItem(&lvitem);

			strField = pWebInfo->_strType;
			lvitem.mask = LVIF_TEXT;
			lvitem.iItem = iPos;		
			lvitem.iSubItem = 3;
			lvitem.pszText = (LPSTR)strField.GetBuffer(strField.GetLength());
			_listView.SetItem(&lvitem);

			strField.Format( "%d", pWebInfo->_grade ) ;
			lvitem.mask = LVIF_TEXT;
			lvitem.iItem = iPos;		
			lvitem.iSubItem = 4;
			lvitem.pszText = (LPSTR)strField.GetBuffer(strField.GetLength());
			_listView.SetItem(&lvitem);

			strField = pWebInfo->_strUser;
			lvitem.mask = LVIF_TEXT;
			lvitem.iItem = iPos;		
			lvitem.iSubItem = 5;
			lvitem.pszText = (LPSTR)strField.GetBuffer(strField.GetLength());
			_listView.SetItem(&lvitem);

			strField = pWebInfo->_strRegisterTime;
			lvitem.mask = LVIF_TEXT;
			lvitem.iItem = iPos;		
			lvitem.iSubItem = 6;
			lvitem.pszText = (LPSTR)strField.GetBuffer(strField.GetLength());
			_listView.SetItem(&lvitem);

			strField = pWebInfo->_strState;
			lvitem.mask = LVIF_TEXT;
			lvitem.iItem = iPos;		
			lvitem.iSubItem = 7;
			lvitem.pszText = (LPSTR)strField.GetBuffer(strField.GetLength());
			_listView.SetItem(&lvitem);

			nCount++;
		}

	}

public:

	CSortListViewCtrl	_listView;
	

};