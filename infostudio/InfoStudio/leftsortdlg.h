#pragma once

#include "Resource.h"

extern CInfoManage* _pInfoManage;

class CLeftSortDlg : public CDialogImpl<CLeftSortDlg>
{
public:

	CLeftSortDlg()
	{

	}

	~CLeftSortDlg()
	{

	}
	enum {IDD = IDD_LEFTSORTDLG};

	BEGIN_MSG_MAP(CLeftSortDlg)
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

		if (_tree.m_hWnd)
			_tree.SetWindowPos(NULL, 0, 0, cx , cy, SWP_NOACTIVATE | SWP_NOZORDER | SWP_NOMOVE);

	}

	//funciton
	void Init()
	{
		_tree.Attach( GetDlgItem( IDC_TREE ) );
		CRect rc;
		GetClientRect( &rc );
		_tree.SetWindowPos(NULL, 0, 0, rc.Width() , rc.Height(), SWP_NOACTIVATE | SWP_NOZORDER );

		for ( int i = 0 ; i < _pInfoManage->_categoryLst.size(); i++ )
		{
			
			CategoryItem*	pItem = _pInfoManage->_categoryLst[i];

			HTREEITEM hitem = NULL;
			TV_INSERTSTRUCT tvis = {0};
			tvis.item.mask = TVIF_PARAM | TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE;
			tvis.hParent = TVI_ROOT;
			tvis.hInsertAfter = TVI_LAST;
			tvis.item.iImage = 0;
			tvis.item.iSelectedImage = 0;
			tvis.item.lParam = (LPARAM) pItem;
			tvis.item.pszText = (LPSTR)pItem->_strCName.GetBuffer(pItem->_strCName.GetLength());
			hitem = _tree.InsertItem(&tvis);

			InsertItem( hitem, pItem );

		}

	}

	void InsertItem( HTREEITEM hitem, CategoryItem*	pItem )
	{
		
		for ( int i = 0 ; i < pItem->_childItemLst.size(); i++ )
		{
			CategoryItem*	pChildItem = pItem->_childItemLst[i];

			HTREEITEM hChilditem = NULL;
			TV_INSERTSTRUCT tvis = {0};
			tvis.item.mask = TVIF_PARAM | TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE;
			tvis.hParent = hitem;
			tvis.hInsertAfter = TVI_LAST;
			tvis.item.iImage = 0;
			tvis.item.iSelectedImage = 0;
			tvis.item.lParam = (LPARAM) pChildItem;
			tvis.item.pszText = (LPSTR)pChildItem->_strCName.GetBuffer(pItem->_strCName.GetLength());
			hChilditem = _tree.InsertItem(&tvis);

			InsertItem( hChilditem, pChildItem );

		}
		

	}

public:

	CTreeViewCtrl	_tree;
};