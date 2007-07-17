// mylistview.h : interface of the CMyListView class
//
/////////////////////////////////////////////////////////////////////////////

#pragma once

#include "util.h"
#include "searchconf.h"
#include "condef.h"
#include "ado/Ado.h"
#include <atlctrls.h>
#include <vector>
#include <map>
#include <atlctrlx.h>
#include "resource.h"

CONST	int MAX_SHOWCOUNT = 200;
typedef CWinTraits<
WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN |
LVS_REPORT | LVS_SHOWSELALWAYS  | LVS_EX_FULLROWSELECT , 0> CMyListViewCtrlWinTraits;

class CMyListView :
	public CSortListViewCtrlImpl<CMyListView, CListViewCtrl, CMyListViewCtrlWinTraits>	
{

protected:
	typedef CMyListView thisClass;
	typedef CSortListViewCtrlImpl<CMyListView, CListViewCtrl, CMyListViewCtrlWinTraits> baseClass;

public:

	CImageList _ImageList;
	int			_type;
	int			_id;
	//std::vector<CString>	_remarkLst; // todo 记录备注信息
public:
	DECLARE_WND_SUPERCLASS(_T("MyListView"), CListViewCtrl::GetWndClassName())	

	BOOL PreTranslateMessage(MSG* pMsg)
	{
	
		return FALSE;
	}

	CMyListView()
	{
		_type = SEARCHDATA_TYPE;
		_id = -1;
	}
	
	BEGIN_MSG_MAP(thisClass)
		MESSAGE_HANDLER(WM_CREATE, OnCreate)
		//REFLECTED_NOTIFY_CODE_HANDLER(LVN_ITEMCHANGED,OnItemChange) 
		MESSAGE_HANDLER(WM_LBUTTONDOWN, OnLButtonDown)	
		MESSAGE_HANDLER(WM_CONTEXTMENU, OnContextMenu)

		MESSAGE_HANDLER(WM_LBUTTONDBLCLK,OnLButtonDbClick)

		COMMAND_ID_HANDLER(ID_SEARCH_DEL, OnDel)

		
		COMMAND_ID_HANDLER(ID_SEARCH_DELPAGE, OnDelPage)

		COMMAND_ID_HANDLER(ID_COMPANY_DELPAGE, OnDelPage)
		COMMAND_ID_HANDLER(ID_COMPANY_DELALL, OnDelAll)

		COMMAND_RANGE_HANDLER(MENU_GROUP, MENU_GROUP_END, OnAddGroup)
		COMMAND_ID_HANDLER(ID_SEARCH_EXCEL, OnExcel)

		COMMAND_ID_HANDLER(ID_COMPANY_DEL, OnDel)		
		COMMAND_ID_HANDLER(ID_COMPANY_EXCEL, OnExcel)

		COMMAND_ID_HANDLER(ID_SEARCH_ALL, OnAll)		
		COMMAND_ID_HANDLER(ID_SEARCH_NULL, OnNull)		
		COMMAND_ID_HANDLER(ID_SEARCH_REVERSE, OnReverse)		

		CHAIN_MSG_MAP(baseClass)
		DEFAULT_REFLECTION_HANDLER()
	END_MSG_MAP()

	LRESULT OnLButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnItemChange(int idCtrl, LPNMHDR pnmh, BOOL& bHandled);
	LRESULT OnContextMenu(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnLButtonDbClick(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	LRESULT OnDel(UINT uID, UINT uItem, HWND hwnd, BOOL& bHandled);
	LRESULT OnDelPage(UINT uID, UINT uItem, HWND hwnd, BOOL& bHandled);
	LRESULT OnDelAll(UINT uID, UINT uItem, HWND hwnd, BOOL& bHandled);

	LRESULT OnAddGroup(UINT uID, UINT uItem, HWND hwnd, BOOL& bHandled);
	LRESULT OnExcel(UINT uID, UINT uItem, HWND hwnd, BOOL& bHandled);

	LRESULT OnAll(UINT uID, UINT uItem, HWND hwnd, BOOL& bHandled);
	LRESULT OnNull(UINT uID, UINT uItem, HWND hwnd, BOOL& bHandled);
	LRESULT OnReverse(UINT uID, UINT uItem, HWND hwnd, BOOL& bHandled);

	void ReadItem(int nItem);
	void ShowItemDlg(int nItem);
	LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		DefWindowProc();
		DWORD dwStyle = ::GetWindowLong(m_hWnd,GWL_STYLE);
		if (dwStyle & LVS_REPORT)
		{
			SetExtendedListViewStyle(LVS_EX_FULLROWSELECT,LVS_EX_FULLROWSELECT);
		}

		InitListView();
		return 0;
	}



	void ShowListViewInfo(CAdoRecordSet* pRes)
	{
		ASSERT(pRes);
		int nSearchID = -1;	
		CString strField[SEARCHRESULT_FIELD_COUNT];

		pRes->GetCollect("id", nSearchID);

		for (int i = 0; i < SEARCHRESULT_FIELD_COUNT; i++)
		{
			pRes->GetCollect(ENSEARCHRESULTFIELD[i], strField[i]);
			strField[i].Replace("\r\n", "");
		}

		int nCount = GetItemCount();	

		int nState = 0;
		pRes->GetCollect("state", nState);

		LVITEM lvitem;			
		lvitem.mask = LVIF_TEXT | LVIF_IMAGE;
		lvitem.iItem = nCount;
		lvitem.iSubItem = 0;				
		lvitem.iImage = nState;	
		lvitem.pszText = (LPSTR)strField[0].GetBuffer(strField[0].GetLength());
		int iPos = InsertItem(&lvitem);

		SetItemData(iPos, nSearchID);
		for (i = 0; i < SEARCHRESULT_FIELD_COUNT; i++)
		{
			if (strField[i].GetLength() == 0)
				strField[i] = "N/A";

			{
				LVITEM lvitem;		
				lvitem.mask = LVIF_TEXT;
				lvitem.iItem = iPos;		
				lvitem.iSubItem = i;
				if (i == 0)
				{
					lvitem.mask = LVIF_TEXT | LVIF_IMAGE;				
					lvitem.iImage = nState;				
				}
				strField[i].Replace(" ", "");
				
					
				lvitem.pszText = (LPSTR)strField[i].GetBuffer(strField[i].GetLength());
				SetItem(&lvitem);
			}
		}
	}

	void AddListView(int nID, int nState, std::vector<CString>& ListItem)
	{
		// 添加的搜索数据,超过
		int nCount = GetItemCount();	
		if ( nCount > MAX_SHOWCOUNT )
			DeleteAllItems();

		nCount = GetItemCount();	

		LVITEM lvitem;			
		lvitem.mask = LVIF_TEXT | LVIF_IMAGE;
		lvitem.iItem = nCount;
		lvitem.iSubItem = 0;				
		lvitem.iImage = nState;	
		lvitem.pszText = (LPSTR)ListItem[0].GetBuffer(ListItem[0].GetLength());
		int iPos = InsertItem(&lvitem);

		SetItemData(iPos, nID);
		EnsureVisible(iPos, FALSE);

		for (int i = 0; i < (int)ListItem.size(); i++)
		{
			if (ListItem[i].GetLength() > 0)
			{
				LVITEM lvitem;		
				lvitem.mask = LVIF_TEXT;
				lvitem.iItem = iPos;		
				lvitem.iSubItem = i;
				if (i == 0)
				{
					lvitem.mask = LVIF_TEXT | LVIF_IMAGE;				
					lvitem.iImage = nState;				
				}
				lvitem.pszText = (LPSTR)ListItem[i].GetBuffer(ListItem[i].GetLength());
				SetItem(&lvitem);
			}
		}
	}

	void UpdateListView(int nID, int nState, std :: map<CString, CString>& ListItem)
	{

		LVFINDINFO info;
		info.flags = LVFI_PARAM;
		info.lParam = (LPARAM)nID;
		int iPos = FindItem( &info, -1);
		if (iPos == -1)
		{
			// add it;
			int nCount = GetItemCount();	

			LVITEM lvitem;			
			lvitem.mask = LVIF_TEXT | LVIF_IMAGE;
			lvitem.iItem = nCount;
			lvitem.iSubItem = 0;				
			lvitem.iImage = nState;	
			lvitem.pszText = "";
			iPos = InsertItem(&lvitem);

			SetItemData(iPos, nID);

		}
		
		std :: map<CString, CString> :: iterator it;
		for (it = ListItem.begin(); it != ListItem.end(); it ++)
		{
			for (int i = 0; i < SEARCHRESULT_FIELD_COUNT; i++)
			{
				if (ENSEARCHRESULTFIELD[i].CompareNoCase(it->first) == 0)
				{
					LVITEM lvitem;		
					lvitem.mask = LVIF_TEXT;
					lvitem.iItem = iPos;		
					lvitem.iSubItem = i;
					if (i == 0)
					{
						lvitem.mask = LVIF_TEXT | LVIF_IMAGE;				
						lvitem.iImage = nState;				
					}
					lvitem.pszText = (LPSTR)it->second.GetBuffer(it->second.GetLength());
					SetItem(&lvitem);
				}
			}				
		}
		
	}
private:
	void InitListView()
	{

		DWORD dwStyle = ::GetWindowLong(m_hWnd,GWL_STYLE);
		if (dwStyle & LVS_REPORT)
		{
			SetExtendedListViewStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
		}


		for (int i = 0; i< SEARCHRESULT_FIELD_COUNT; i++)
		{
			InsertColumn(i, SEARCHRESULTFIELD[i]);
			SetColumnWidth(i, SEARCHRESULTFIELDSIZE[i]);
		}

		CComPtr<ISkinScheme>	pSkinScheme;
		LRESULT hr = GetMetaCurrentScheme(&pSkinScheme);
		if (!SUCCEEDED(hr) || !pSkinScheme)
		{
			return ;
		}

		_ImageList.Create(16, 16, ILC_MASK | ILC_COLOR24, 6, 6);

		CRect rc;
		HBITMAP hBmp = pSkinScheme->ExtractBitmap("listviewicon");

		_ImageList.Add(hBmp,RGB(255, 0, 255));

		SetImageList(_ImageList, LVSIL_SMALL);
	}

	void SetReaded(int nItem)
	{
		LVITEM lvitem;	
		lvitem.mask = LVIF_TEXT | LVIF_IMAGE;						
		lvitem.iItem = nItem;		
		lvitem.iSubItem = 0;	

		//GetItem(&lvitem);
		CString strText = "";
		GetItemText(nItem, 0, strText);
		lvitem.pszText = (LPSTR)strText.GetBuffer(strText.GetLength());
		//if (lvitem.iImage == SEARCHDATA_READED)
		//	return;

		CString strSql;
		if (_type == SEARCHDATA_TYPE)
			strSql.Format("update searchdata set State = 1 where ID  = %d", GetItemData(nItem));
		else
			strSql.Format("update friend set State = 1 where ID  = %d", GetItemData(nItem));
		_pDb->Execute(strSql);
		
		lvitem.iImage = SEARCHDATA_READED;
		SetItem(&lvitem);
	}
};
// Handler prototypes (uncomment arguments if needed):
//	LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
//	LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
//	LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)
