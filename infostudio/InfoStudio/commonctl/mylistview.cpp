#include "StdAfx.h"
#include "mylistview.h"
#include "MainDlg.h"
#include "htmlDlg.h"

void CMyListView::ReadItem(int nItem)
{
	if ( nItem != -1 ) 
	{
		if (GetItemData(nItem) == _id)
			return;
		_id = GetItemData(nItem);
		//设置已读			
		//if (_type == SEARCHDATA_TYPE)
		SetReaded(nItem);
		//显示HTML
		if (_MainDlg)
			_MainDlg->ShowHtmlDetail(GetItemData(nItem), _type);
	}
}

void CMyListView::ShowItemDlg(int nItem)
{
	if ( nItem != -1 ) 
	{
		int nID = GetItemData(nItem) ;

		//显示HTML
		CHtmlDlg dlg;
		CString strUrl;
		if ( _type == SEARCHDATA_TYPE )
		{
			strUrl = GetExePath() + "html\\detail.html";
		}
		else
			strUrl = GetExePath() + "html\\edit.html";

		CString strParam;
		strParam.Format("?id=%d&type=%d", nID, _type);
		dlg._strUrl = strUrl + strParam;
		dlg.DoModal();
	}
}

LRESULT CMyListView::OnLButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	LRESULT lRet = DefWindowProc();
	POINT	pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
	UINT iFlags = 0;
	int nItem = HitTest(pt, &iFlags);
	ReadItem(nItem);
	
	return lRet;			
}

LRESULT CMyListView::OnLButtonDbClick(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	LRESULT lRet = DefWindowProc();
	POINT	pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
	UINT iFlags = 0;
	int nItem = HitTest(pt, &iFlags);
	if ( nItem != -1 ) 
	{
		ShowItemDlg( nItem );
	}
	return lRet;			
}


LRESULT CMyListView::OnItemChange (int idCtrl, LPNMHDR pnmh, BOOL& bHandled)
{
	NMLISTVIEW* nmh = (LPNMLISTVIEW)pnmh;
	if(nmh->iItem == -1)//|| m_iHotItem == nmh->iItem)
		return 0;
	int nItem  = nmh->iItem;
	if(nmh->uNewState&LVIS_SELECTED)
	{
		ReadItem(nItem);
	}
	return TRUE;
}

LRESULT CMyListView::OnDel(UINT uID, UINT uItem, HWND hwnd, BOOL& bHandled)
{
	if ( _type == SEARCHDATA_TYPE )
	{
		BOOL bHandled ;
		_MainDlg->OnDelCompany(0, 0, 0, bHandled);
	}
	else
	{
		BOOL bHandled ;
		_MainDlg->OnDelFriend(0, 0, 0, bHandled);
	}
	return 0;
}

LRESULT CMyListView::OnDelPage(UINT uID, UINT uItem, HWND hwnd, BOOL& bHandled)
{
	if ( _type == SEARCHDATA_TYPE )
	{
		CMyListView* pListView = _MainDlg->getWebListView();
		int nCount = pListView->GetItemCount();
		for ( int i = 0; i < nCount; i++ )
		{
			pListView->SetItemState(i, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);
		}

		BOOL bHandled ;
		_MainDlg->OnDelCompany(0, 0, 0, bHandled);
	}
	else
	{
		CMyListView* pListView = _MainDlg->getInfoListView();
		int nCount = pListView->GetItemCount();
		for ( int i = 0; i < nCount; i++ )
		{
			pListView->SetItemState(i, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);
		}

		BOOL bHandled ;
		_MainDlg->OnDelFriend(0, 0, 0, bHandled);
	}

	
	
	return 0;
}

LRESULT CMyListView::OnDelAll(UINT uID, UINT uItem, HWND hwnd, BOOL& bHandled)
{

	return 0;
}

LRESULT CMyListView::OnAddGroup(UINT uID, UINT uItem, HWND hwnd, BOOL& bHandled)
{
	int nGroupId = uItem - MENU_GROUP;
	if ( _type == SEARCHDATA_TYPE )
		_MainDlg->AddFavour(nGroupId);
	else
		_MainDlg->MoveFriend(nGroupId);

	return 0;
}

LRESULT CMyListView::OnExcel(UINT uID, UINT uItem, HWND hwnd, BOOL& bHandled)
{
	if ( _type == SEARCHDATA_TYPE )
	{
		BOOL bHandled ;
		_MainDlg->OnExport(0, 0, 0, bHandled);
	}
	else
	{
		BOOL bHandled ;
		_MainDlg->OnExportFriend(0, 0, 0, bHandled);
	}
	return 0;
}

LRESULT CMyListView::OnContextMenu(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
	POINT point = pt;
	ScreenToClient(&point);		

	HMENU	hMenu			= NULL;
	HMENU	hPopMenu		= NULL;
	HMENU	hPopChildMenu	= NULL;

	hMenu = LoadMenu(_Module.GetResourceInstance(),MAKEINTRESOURCE(IDR_MENU_SEARCHLIST));
	hPopMenu = GetSubMenu(hMenu, _type);

	hPopChildMenu = GetSubMenu(hPopMenu, 7);
	if ( hPopChildMenu )
	{
		// 1 del the old menu
		while ( GetMenuItemCount(hPopChildMenu) > 0 )
			RemoveMenu(hPopChildMenu, 0, MF_BYPOSITION);
		// 2 insert gourp name 

		CString strSql;		
		strSql.Format("select * from groupinfo order by groupid desc");		
		CAdoRecordSet* pRs = new CAdoRecordSet(_pDb);
		if(pRs->Open((LPCTSTR) strSql ))
		{
			int nCount = 0;
			while(!pRs->IsEOF())
			{
				CString strName = "";
				int nID = -1;				
				pRs->GetCollect("groupid", nID);	
				pRs->GetCollect("groupname", strName);					
				
				// insert menu 
				MENUITEMINFO mii;
				mii.cbSize = sizeof mii;
				mii.fMask = MIIM_STATE | /*MIIM_STRING*/0x00000040 | MIIM_ID; 
				mii.fState = MFS_ENABLED;
				mii.wID = MENU_GROUP + nID;
				mii.dwTypeData = strName.GetBuffer(strName.GetLength());
				InsertMenuItem( hPopChildMenu, nCount, TRUE, &mii);
				pRs->MoveNext();
			}
			pRs->Close();
		}	
		delete pRs;
		pRs = NULL;
	}
	TrackPopupMenu(hPopMenu, TPM_LEFTALIGN, pt.x, pt.y, 0, m_hWnd, NULL);
	DestroyMenu(hMenu);
			
	return 0;
}

LRESULT CMyListView::OnAll(UINT uID, UINT uItem, HWND hwnd, BOOL& bHandled)
{
	for ( int i = 0; i < GetItemCount(); i ++ )
	{
		SetItemState(i, LVIS_SELECTED, LVIS_SELECTED);
	}
	return 0 ;
}

LRESULT CMyListView::OnNull(UINT uID, UINT uItem, HWND hwnd, BOOL& bHandled)
{
	int i = GetNextItem(-1,LVNI_SELECTED);
	while( i != -1 )
	{
		SetItemState( i, 0, LVIS_SELECTED );	
		i = GetNextItem( -1, LVNI_SELECTED );
	}
	return 0 ;
}

LRESULT CMyListView::OnReverse(UINT uID, UINT uItem, HWND hwnd, BOOL& bHandled)
{
	for ( int i = 0; i < GetItemCount(); i ++ )
	{
		if ( GetItemState( i, LVIS_SELECTED ) == LVIS_SELECTED )
		{	
			SetItemState( i, 0, LVIS_SELECTED );	
		}
		else
		{
			SetItemState( i, LVIS_SELECTED, LVIS_SELECTED );	
		}
	}
	return 0 ;
}