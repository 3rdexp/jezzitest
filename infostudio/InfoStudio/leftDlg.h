#pragma once

#include "Resource.h"

class CLeftDlg : public CDialogImpl<CLeftDlg>
{
public:

	CLeftDlg()
	{
		
	}

	~CLeftDlg()
	{
		
	}
	enum {IDD = IDD_LEFTDLG};

	BEGIN_MSG_MAP(CLeftDlg)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		COMMAND_ID_HANDLER(IDOK, OnOk)
		COMMAND_ID_HANDLER(IDCANCEL, OnCancel)		

		MSG_WM_SIZE(OnSize)
		NOTIFY_HANDLER( IDC_TREE, TVN_SELCHANGED, OnSelChanged )

		COMMAND_ID_HANDLER(IDC_BUTTON1, OnButton1)		
		COMMAND_ID_HANDLER(IDC_BUTTON2, OnButton2)		
		

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

	LRESULT OnButton1(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{

		return 0;
	}

	LRESULT OnButton2(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{

		return 0;
	}

	void OnSize(UINT wParam, CSize size)
	{
		DefWindowProc();
		int cx = size.cx;
		int cy = size.cy;

		if (_tree.m_hWnd)
			_tree.SetWindowPos(NULL, 0, 0, cx , cy, SWP_NOACTIVATE | SWP_NOZORDER | SWP_NOMOVE);

	}


	LRESULT OnSelChanged(int idCtrl, LPNMHDR pnmh, BOOL& bHandled)
	{
		LPNMTREEVIEW nmh = (LPNMTREEVIEW)pnmh;
		TVITEM& item = nmh->itemNew;
	
		return 0;
	}

	//funciton
	void Init()
	{
		_tree.Attach( GetDlgItem( IDC_TREE ) );
		CRect rc;
		GetClientRect( &rc );
		_tree.SetWindowPos(NULL, 0, 0, rc.Width() , rc.Height(), SWP_NOACTIVATE | SWP_NOZORDER );

		//添加信息
		HTREEITEM hitem = NULL;
		TV_INSERTSTRUCT tvis = {0};
		tvis.item.mask = TVIF_PARAM | TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE;
		tvis.hParent = TVI_ROOT;
		tvis.hInsertAfter = TVI_LAST;
		tvis.item.iImage = 0;
		tvis.item.iSelectedImage = 0;
		
		tvis.item.pszText = "企业信息";
		hitem = _tree.InsertItem(&tvis);
		
		tvis.item.pszText = "信息发布";
		hitem = _tree.InsertItem(&tvis);
		
		tvis.hParent = hitem;
		tvis.item.pszText = "黄页登录";
		_tree.InsertItem(&tvis);

		tvis.item.pszText = "信息发布";
		_tree.InsertItem(&tvis);

		tvis.item.pszText = "论坛群发";
		_tree.InsertItem(&tvis);

		tvis.item.pszText = "引擎登录";
		_tree.InsertItem(&tvis);

		tvis.hParent = TVI_ROOT;
		tvis.item.pszText = "信息搜索";
		hitem = _tree.InsertItem(&tvis);

	}

public:
	
	CTreeViewCtrl	_tree;
};