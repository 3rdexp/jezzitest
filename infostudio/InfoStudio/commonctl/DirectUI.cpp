#include "StdAfx.h"
#include ".\directui.h"
#include "../condef.h"



//////////////////////////////////////////////////////////////////////////


CItemBase::CItemBase(void)
{
	_caption = "";
	_tooltip = "";
	_hide = FALSE;
	_enable = TRUE;
}
//////////////////////////////////////////////////////////////////////////



void CItemButtonBmp::Draw(HDC hdc, BOOL bSelect , BOOL bPress )
{
	if (_hide)
		return;

	CComPtr<ISkinScheme>	pSkinScheme;
	LRESULT hr = GetMetaCurrentScheme(&pSkinScheme);
	if (!SUCCEEDED(hr) || !pSkinScheme)
	{
		return ;
	}

	int nIndex = GetState2Img(bSelect, bPress);

	CRect rc;
	pSkinScheme->ExtractRect(_bmp[nIndex].c_str(), &rc);
	int nMode = SetStretchBltMode(hdc, HALFTONE );
	
	// 完全 BitBlt, 原大小
	if (rc.Width() == _rc.Width() && rc.Height() == _rc.Height())
	{
		pSkinScheme->TransparentDraw(hdc, _bmp[nIndex].c_str(), _rc.left, _rc.top);		
	}
	else //if (rc.Height() == rect.Height())// 高度一致时，分3次绘制
	{
		int step = rc.Width() / 3;
		pSkinScheme->TransparentDraw(hdc, _bmp[nIndex].c_str(), _rc.left, _rc.top, step, rc.Height(), 0, 0, step, rc.Height());
		pSkinScheme->Draw(hdc, _bmp[nIndex].c_str(), _rc.left + step, _rc.top, _rc.right - 2 * step, rc.Height(), step, 0, step, rc.Height());
		pSkinScheme->TransparentDraw(hdc, _bmp[nIndex].c_str(), _rc.right - step, _rc.top, step, rc.Height(), rc.Width() - step, 0, step, rc.Height());
	}

	SetStretchBltMode(hdc, nMode );
}

//////////////////////////////////////////////////////////////////////////

void CItemButtonTextIcon::Draw(HDC hdc, BOOL bSelect , BOOL bPress )
{
	if (_hide)
		return;

	CComPtr<ISkinScheme>	pSkinScheme;
	LRESULT hr = GetMetaCurrentScheme(&pSkinScheme);
	if (!SUCCEEDED(hr) || !pSkinScheme)
	{
		return ;
	}

	static const char* sz[4] = 
	{
		"metabutton_normal", "metabutton_focus", "metabutton_down", "metabutton_disable"
	};

	CRect rcClient = _rc;
	if (bPress)
		rcClient.InflateRect(-2, -2, 0, 0);
	else
		rcClient.InflateRect(-1, -1, -1, -1);		
	int nIndex = GetState2Img(bSelect, bPress);

	SetStretchBltMode( hdc, HALFTONE );

	CRect rc;
	pSkinScheme->ExtractRect(sz[nIndex], &rc);
	// 完全 BitBlt, 原大小
	CRect rect(_rc);
	if (rc.Width() == rect.Width() && rc.Height() == rect.Height())
	{
		pSkinScheme->TransparentDraw(hdc, sz[nIndex], _rc.left, _rc.top);		
	}

	else if (rc.Height() == rect.Height())// 高度一致时，分3次绘制
	{
		int step = rc.Width() / 3;
		pSkinScheme->TransparentDraw(hdc, sz[nIndex], rcClient.left, rcClient.top, step, rc.Height(), 0, 0, step, rc.Height());
		pSkinScheme->Draw(hdc, sz[nIndex], rcClient.left + step, rcClient.top, rect.Width() - 2 * step, rc.Height(), step, 0, step, rc.Height());
		pSkinScheme->TransparentDraw(hdc, sz[nIndex], rcClient.left + rect.Width() - step, rcClient.top, step, rc.Height(), rc.Width() - step, 0, step, rc.Height());
	}

	else // 分 9 段绘制, 高度自适应
	{
		const int step = rc.Height() / 3;
		// 左上角
		pSkinScheme->TransparentDraw(hdc, sz[nIndex], rcClient.left, rcClient.top, step, step, 0, 0, step, step);		
		// 上部中间
		pSkinScheme->Draw(hdc, sz[nIndex], rcClient.left + step, rcClient.top, rect.Width() - 2 * step, step, step, 0, rc.Width() - 2 * step, step);				
		// 上部右边
		pSkinScheme->TransparentDraw(hdc, sz[nIndex], rect.right - step, rcClient.top, step, step, rc.Width() - step, 0, step, step);						

		// 中间部分 左边
		pSkinScheme->Draw(hdc, sz[nIndex], rcClient.left, rcClient.top + step, step, rect.Height() - 2 * step, 0, step, step, rc.Height() - 2 * step);								

		// 中间部分 中部
		pSkinScheme->Draw(hdc, sz[nIndex], rcClient.left + step, rcClient.top + step, rect.Width() - 2 * step, rect.Height() - 2 * step, step, step, step, step);								

		// 中间部分 右边
		pSkinScheme->Draw(hdc, sz[nIndex], rect.right - step  , rcClient.top + step, step, rect.Height() - 2 * step, rc.Width() - step, step, step, step);										

		// 下面部分 左边
		pSkinScheme->TransparentDraw(hdc, sz[nIndex], rcClient.left, rect.bottom - step, step, step, 0, rc.Height() - step, step, step);

		// 下面部分 中间
		pSkinScheme->Draw(hdc, sz[nIndex], rcClient.left + step, rect.bottom - step, rect.Width() - 2 * step, step, step, rc.Height() - step, rc.Width() - 2 * step, step);														

		// 下面部分 右边
		pSkinScheme->TransparentDraw(hdc, sz[nIndex], rect.right - step , rect.bottom - step, step, step, rc.Width() - step, rc.Height() - step, step, step);

	}

	CRect rcCalText(rcClient);
	if (_caption.size() > 0)
	{
		CFont font;
		font.CreatePointFont(90, _T("宋体"), hdc);
		HFONT oldFont =  (HFONT)SelectObject(hdc, font);
		SetBkMode(hdc, TRANSPARENT);
		CRect rcText(rcClient);
		if (_icon.size() > 0)
		{
			if (_bTextLeft)
				rcText.right = rcText.right - _iconWidth - TEXTICONSPACE;
			else
				rcText.left = rcText.left + _iconWidth + TEXTICONSPACE;
		}
		SetTextColor(hdc, RGB(0, 0, 0));
		DrawText(hdc, _caption.c_str(), -1, &rcText, DT_CENTER | DT_VCENTER | DT_END_ELLIPSIS |DT_SINGLELINE);
		//DrawText(hdc, _caption.c_str(), -1, &rcCalText, DT_LEFT|DT_SINGLELINE|DT_TOP|DT_END_ELLIPSIS|DT_CALCRECT);
		SelectObject(hdc, oldFont);	
	}

	if (_icon.size() > 0)
	{
		int nIconState = GetState2Icon(bSelect, bPress);
		CRect rc;
		pSkinScheme->ExtractRect(_icon.c_str(), &rc);
		const int step = rc.Width() / 3;
		int nTop = (rcClient.Height() - rc.Height()) / 2 + rcClient.top;

		if (_bTextLeft)
			pSkinScheme->TransparentDraw(hdc, _icon.c_str(), rcCalText.right - _iconWidth - TEXTICONSPACE, nTop, step, rc.Height(), step * nIconState, 0, step, rc.Height());
		else
			pSkinScheme->TransparentDraw(hdc, _icon.c_str(), rcClient.left + TEXTICONSPACE , nTop, step, rc.Height(), step * nIconState, 0, step, rc.Height());
	}
}
//////////////////////////////////////////////////////////////////////////


void CItemTab::Draw(HDC hdc, BOOL bSelect , BOOL bPress )
{
	// 1 draw background
	// 2 draw AllItem
	if (_hide)
		return;

	CComPtr<ISkinScheme>	pSkinScheme;
	LRESULT hr = GetMetaCurrentScheme(&pSkinScheme);
	if (!SUCCEEDED(hr) || !pSkinScheme)
	{
		return ;
	}

	RECT rc;
	pSkinScheme->ExtractRect(_backImg.c_str(), &rc);
	pSkinScheme->Draw(hdc, _backImg.c_str(), _rc.left, _rc.top, _rc.Width(), _rc.Height(), 0, 0, rc.right - rc.left,
		rc.bottom - rc.top);

	
	
	SetBkMode(hdc, TRANSPARENT);
	SetTextColor(hdc, _clrNormal);
	HFONT oldFont =  (HFONT)SelectObject(hdc, _font);
	for (int i = 0; i < (int)_ListTab.size(); i++)
	{
		int nImage = 0;
		if (i == _selectID)
			nImage = 2;
		if (i == _mousemove)
			nImage ++;

		if (i != _selectID)
		{
			pSkinScheme->Draw(hdc, _img[nImage].c_str(), _ListTab[i]._rc.left, _ListTab[i]._rc.top);			
			DrawText(hdc, _ListTab[i]._name.c_str(), -1, &_ListTab[i]._rc, DT_CENTER | DT_VCENTER | DT_END_ELLIPSIS |DT_SINGLELINE);
		}
	}
	SelectObject(hdc, oldFont);	

	SetTextColor(hdc, _clrSelect);
	oldFont =  (HFONT)SelectObject(hdc, _fontSelect);
	
	int nImage = 2;	
	if (_selectID == _mousemove)
		nImage ++;
	pSkinScheme->Draw(hdc, _img[nImage].c_str(), _ListTab[_selectID]._selectrc.left, _ListTab[_selectID]._selectrc.top);
	DrawText(hdc, _ListTab[_selectID]._name.c_str(), -1, &_ListTab[_selectID]._selectrc, DT_CENTER | DT_VCENTER | DT_END_ELLIPSIS |DT_SINGLELINE);
	SelectObject(hdc, oldFont);	

}

int CItemTab::OnMouseMove(UINT nFlags, CPoint point)
{
	int nSelect = HitTestTab(point);
	HighlightItem(nSelect);	
	//_mousemove = nSelect;
	return _selectID;
}
int CItemTab::OnLButtonUp(UINT nFlags, CPoint point)
{
	int nSelect = HitTestTab(point);
	HighlightItem(nSelect);	
	_mousemove = nSelect;
	return _selectID;
}
int CItemTab::OnLButtonDown(UINT nFlags, CPoint point)
{
	int nSelect = HitTestTab(point);	
	//HighlightItem(nSelect);	
	if ( nSelect >= 0 )
	{
		HighlightSelectItem(nSelect);
		_selectID = nSelect;
		PostMessage(_hWnd, WM_COMMAND, _ListTab[nSelect]._id, 0);
	}			
	return _selectID;
}

void CItemTab::OnSize(UINT nType, int cx, int cy)
{
	CItemBase::OnSize(nType, cx, cy);


	CComPtr<ISkinScheme>	pSkinScheme;
	LRESULT hr = GetMetaCurrentScheme(&pSkinScheme);
	if (!SUCCEEDED(hr) || !pSkinScheme)
	{
		return ;
	}

	RECT rc;
	RECT rcSelect;
	int nTop[4];
	pSkinScheme->ExtractRect(_img[0].c_str(), &rc);
	pSkinScheme->ExtractRect(_img[2].c_str(), &rcSelect);

	nTop[0] = nTop[1] = _rc.bottom - rc.bottom + rc.top;
	nTop[2] = nTop[3] = _rc.bottom - rcSelect.bottom + rcSelect.top;
	int nLeft = _rc.left + _space;
	if (!_bLeft)
	{
		nLeft = _rc.right - (rcSelect.right - rcSelect.left)
			- (_ListTab.size() - 1) * (rc.right - rc.left) - _space;
	}

	for (int i = 0; i < (int)_ListTab.size(); i++)
	{
		int nImage = 0;
		if (i == _selectID)
			nImage = 2;
		if (i == _mousemove)
			nImage ++;
		
		_ListTab[i]._rc.left = nLeft;
		_ListTab[i]._rc.right = nLeft + rc.right - rc.left;
		_ListTab[i]._rc.top = nTop[0];
		_ListTab[i]._rc.bottom = _rc.bottom;

		_ListTab[i]._selectrc.left = nLeft;
		_ListTab[i]._selectrc.right = nLeft + rc.right - rc.left;
		_ListTab[i]._selectrc.top = nTop[2];
		_ListTab[i]._selectrc.bottom = _rc.bottom;

		nLeft  = nLeft + _itemspace + rc.right - rc.left;
	}


}

int CItemTab::HitTestTab(CPoint point)
{
	for( int i  = 0 ; i < (int)_ListTab.size(); i++)
	{
		if (_selectID == i)
		{
			if (_ListTab[i]._selectrc.PtInRect(point))
				return i;
		}
		else
			if (_ListTab[i]._rc.PtInRect(point))
				return i;
	}
	return -1;
}

BOOL CItemTab::HitTest(CPoint point)
{
	for( int i  = 0 ; i < (int)_ListTab.size(); i++)
	{
		if (_selectID == i)
		{
			if (_ListTab[i]._selectrc.PtInRect(point))
				return TRUE;
		}
		else
			if (_ListTab[i]._rc.PtInRect(point))
				return TRUE;
	}
	return FALSE;
}

void CItemTab::HighlightItem(int nSelect)
{
	int oldmouse = _mousemove;
	_mousemove = nSelect;
	if (nSelect != oldmouse)
	{
		if (oldmouse >= 0)	
		{
			if (oldmouse == _selectID)
				InvalidateRect(_hWnd, _ListTab[oldmouse]._selectrc, TRUE);
			else
				InvalidateRect(_hWnd, _ListTab[oldmouse]._rc, TRUE);
		}
		if (nSelect >= 0)		
		{
			if (nSelect == _selectID)
				InvalidateRect(_hWnd, _ListTab[nSelect]._selectrc, TRUE);
			else
				InvalidateRect(_hWnd, _ListTab[nSelect]._rc, TRUE);

		}
	}
}

void CItemTab::HighlightSelectItem(int nSelect)
{
	if (nSelect != _selectID)
	{
		if (_selectID >= 0)		
			InvalidateRect(_hWnd, _ListTab[_selectID]._selectrc, TRUE);

		if (nSelect >= 0)		
			InvalidateRect(_hWnd, _ListTab[nSelect]._selectrc, TRUE);
	}
}
//////////////////////////////////////////////////////////////////////////

CDirectUI::CDirectUI(void)
{
	_bPressed = FALSE;
	_hWnd = 0;
	_rc.SetRect(0, 0, 0, 0);
	_selectID = -1;
}

CDirectUI::~CDirectUI(void)
{

}


void CDirectUI::OnSize(UINT nType, int cx, int cy)
{
	for( _ItemMap_ITERATOR it = _ItemMap.begin(); it!= _ItemMap.end(); it++ )
	{
		CItemBase* pItem = it->second;
		// from _rcopposite to _rc;
		pItem->OnSize(nType, cx, cy);
	}
}

int CDirectUI::OnMouseMove(UINT nFlags, CPoint point)
{
	for( _ItemMap_ITERATOR it = _ItemMap.begin(); it!= _ItemMap.end(); it++ )
	{
		CItemBase* pItem = it->second;
		pItem->OnMouseMove(nFlags, point);
	}	
	int nSelect = HitTest(point);
	HighlightItem(nSelect);
	//_selectID = nSelect;
	return _selectID;
}
int CDirectUI::OnLButtonUp(UINT nFlags, CPoint point)
{
	for( _ItemMap_ITERATOR it = _ItemMap.begin(); it!= _ItemMap.end(); it++ )
	{
		CItemBase* pItem = it->second;
		pItem->OnLButtonUp(nFlags, point);
	}
	_bPressed = FALSE;
	int nSelect = HitTest(point);
	HighlightItem(nSelect, TRUE);
	//_selectID = nSelect;
	if (_selectID >= 0)
		PostMessage(_hWnd, WM_COMMAND, _selectID, 0);
	return _selectID;
}
int CDirectUI::OnLButtonDown(UINT nFlags, CPoint point)
{
	for( _ItemMap_ITERATOR it = _ItemMap.begin(); it!= _ItemMap.end(); it++ )
	{
		CItemBase* pItem = it->second;
		pItem->OnLButtonDown(nFlags, point);
	}
	_bPressed = TRUE;
	int nSelect = HitTest(point);
	HighlightItem(nSelect, TRUE);
	//_selectID = nSelect;
	return nSelect;
}

//draw
void CDirectUI::DrawItem(HDC hdc, CItemBase* pItem)
{
	BOOL bSelect = FALSE;
	BOOL bPress = _bPressed;
	if (_selectID == pItem->_id)
		bSelect = TRUE;
	else
		bPress = FALSE;
	pItem->Draw(hdc, bSelect, bPress);
}
void CDirectUI::DrawAllItem(HDC hdc)
{
	for( _ItemMap_ITERATOR it = _ItemMap.begin(); it!= _ItemMap.end(); it++ )
	{
		CItemBase* pItem = it->second;
		DrawItem(hdc, pItem);
	}
}

// function

CItemBase* CDirectUI::getItem(int nID)
{
	_ItemMap_ITERATOR it = _ItemMap.find(nID);
	if( it != _ItemMap.end() )
		return it->second;
	else
		return NULL;
}
void CDirectUI::ClearAll()
{
	for( _ItemMap_ITERATOR it = _ItemMap.begin(); it!= _ItemMap.end(); it++ )
	{
		delete it->second;
	}
	_ItemMap.clear();
}
int CDirectUI::HitTest(CPoint point)
{
	for( _ItemMap_ITERATOR it = _ItemMap.begin(); it!= _ItemMap.end(); it++ )
	{
		CItemBase* pItem = it->second;
		if (pItem->HitTest(point))
			return pItem->_id;
	}
	return -1;
}
void CDirectUI::SetParent(HWND hWnd)
{
	_hWnd = hWnd;
}

void CDirectUI::HighlightItem(int nSelect, BOOL bForce )
{
	int oldSelect = _selectID;
	_selectID = nSelect;
	if (nSelect != oldSelect || bForce)
	{
		CItemBase* pOldItem = getItem(oldSelect);
		if (pOldItem)
			InvalidateRect(_hWnd, pOldItem->_rc, TRUE);

		CItemBase* pItem = getItem(nSelect);
		if (pItem)
			InvalidateRect(_hWnd, pItem->_rc, TRUE);
	}
}
void CDirectUI::CreateButton(int nId, const char* szCaption, CRect rc, BOOL bShow, const char* szIcon )
{
	CItemButtonTextIcon* pButton = new CItemButtonTextIcon();
	pButton->_id = nId;
	pButton->_icon = szIcon;
	pButton->_rcopposite = rc;
	pButton->_caption = szCaption;
	pButton->_hide = !bShow;
	_ItemMap.insert(std::make_pair(pButton->_id, pButton));
}

void CDirectUI::Show(int nID, BOOL bShow, BOOL bInvalidate)
{
	CItemBase* pItem = getItem(nID);
	if (pItem)
	{
		pItem->_hide = !bShow;
		InvalidateRect(_hWnd, pItem->_rc, TRUE);
	}
}

void CDirectUI::InitSearchDlg()
{
	CItemButtonBmp* pLog = new CItemButtonBmp();
	pLog->_id = DIRECTUI_SORT;
	pLog->_bmp[0] = "sorticon_normal";
	pLog->_bmp[1] = "sorticon_select";
	pLog->_bmp[2] = "sorticon_select";
	pLog->_bmp[3] = "sorticon_select";

	pLog->_rcopposite.left = -26;
	pLog->_rcopposite.right = -4;
	pLog->_rcopposite.top = 95;
	pLog->_rcopposite.bottom = 117;
	
	pLog->_enable = TRUE;
	_ItemMap.insert(std::make_pair(pLog->_id, pLog));

	CRect rc;
	GetClientRect(_hWnd, &rc);
	OnSize(0, rc.Width(), rc.Height());

}
void CDirectUI::Init()
{
	// 1 tab
	CItemTab* pTab = new CItemTab();
	pTab->_id = DIRECTUI_TAB;
	pTab->_backImg = "tabback";
	pTab->_bLeft = FALSE;
	pTab->SetParent(_hWnd);
	pTab->_img[0] = "tab_normal";
	pTab->_img[1] = "tab_mousemove";
	pTab->_img[2] = "selecttab_normal";
	pTab->_img[3] = "selecttab_mousemove";
	pTab->_space = 200;
	pTab->_itemspace = 2;
	pTab->_rc.left = 0;
	pTab->_rc.right = 0;
	pTab->_rc.top = 0;
	pTab->_rc.bottom = 45;
	pTab->_rcopposite = pTab->_rc;
	pTab->AddItem(DIRECTUI_TAB_SEARCH, "客户搜索");
	pTab->AddItem(DIRECTUI_TAB_MANAGE, "客户管理");
	pTab->AddItem(DIRECTUI_TAB_HELP, "帮助");	
	_ItemMap.insert(std::make_pair(pTab->_id, pTab));

	CItemButtonBmp* pLog = new CItemButtonBmp();
	pLog->_id = DIRECTUI_LOG;
	pLog->_bmp[0] = "log";
	pLog->_bmp[1] = "log";
	pLog->_bmp[2] = "log";
	pLog->_bmp[3] = "log";

	pLog->_rcopposite.left = 24;
	pLog->_rcopposite.right = 230;
	pLog->_rcopposite.top = 5;
	pLog->_rcopposite.bottom = 42;

	_ItemMap.insert(std::make_pair(pLog->_id, pLog));

	// 2 toolbar
	CItemButtonBmp* pToolBarBK = new CItemButtonBmp();
	pToolBarBK->_id = DIRECTUI_BACK;
	pToolBarBK->_bmp[0] = "toolbarback";
	pToolBarBK->_bmp[1] = "toolbarback";
	pToolBarBK->_bmp[2] = "toolbarback";
	pToolBarBK->_bmp[3] = "toolbarback";

	pToolBarBK->_rcopposite.left = 4;
	pToolBarBK->_rcopposite.right = -4;
	pToolBarBK->_rcopposite.top = 47;
	pToolBarBK->_rcopposite.bottom = 78;

	_ItemMap.insert(std::make_pair(pToolBarBK->_id, pToolBarBK));

	// tab1 
	CreateButton(DIRECTUI_DELETE_SEARCH, "删除搜索历史", CRect(9, 49, 119, 75));
	CreateButton(DIRECTUI_DELETE_COMPANY, "删除搜索结果", CRect(121, 49, 231, 75));
	CreateButton(DIRECTUI_ADD_FAVOUR, "添加为我的客户", CRect(233, 49, 343,75));
	CreateButton(DIRECTUI_EXPORT, "导出到EXCEL", CRect(345, 49, 435, 75));	
	//CreateButton(DIRECTUI_RESTART, "继续搜索", CRect(457, 49, 547, 75));	
	CreateButton(DIRECTUI_REPEAT, "数据整理", CRect(457, 49, 547, 75));	
	CreateButton(DIRECTUI_UNITE, "合并数据", CRect(549, 49, 639, 75));	
	//CreateButton(DIRECTUI_LOCAL, "搜索", CRect(-48, 49, -10, 75));	

	/*
	CreateButton(DIRECTUI_FIRST, "", CRect(-240, 49, -214, 75), TRUE, "firsticon");	
	CreateButton(DIRECTUI_PREVIOUS, "", CRect(-212, 49, -186, 75),TRUE, "previousicon");	
	CreateButton(DIRECTUI_NEXT, "", CRect(-184, 49, -158, 75), TRUE, "nexticon");	
	CreateButton(DIRECTUI_LAST, "", CRect(-156, 49, -130, 75), TRUE, "lasticon");	
	*/

	//tab2
	CreateButton(DIRECTUI_GROUP_ADD, "添加分类", CRect(9, 49, 119, 75), FALSE);
	CreateButton(DIRECTUI_GROUP_DEL, "删除分类", CRect(121, 49, 231, 75), FALSE);
	CreateButton(DIRECTUI_GROUP_EDIT, "修改分类", CRect(233, 49, 343,75), FALSE);
	CreateButton(DIRECTUI_EXPORTFRIEND, "导出到EXCEL", CRect(345, 49, 455, 75), FALSE);	
	CreateButton(DIRECTUI_DEL_FRIEND, "删除客户", CRect(457, 49, 567, 75), FALSE);
	CreateButton(DIRECTUI_MOVETO_FRIEND, "移动客户到...", CRect(569, 49, 679, 75), FALSE);

	// 2 status bar
	CItemButtonBmp* StatusBK = new CItemButtonBmp();
	StatusBK->_id = DIRECTUI_STATUS;
	StatusBK->_bmp[0] = "toolbarback";
	StatusBK->_bmp[1] = "toolbarback";
	StatusBK->_bmp[2] = "toolbarback";
	StatusBK->_bmp[3] = "toolbarback";

	StatusBK->_rcopposite.left = 4;
	StatusBK->_rcopposite.right = -20;
	StatusBK->_rcopposite.top = -33;
	StatusBK->_rcopposite.bottom = -2;
	StatusBK->_enable = FALSE;
	_ItemMap.insert(std::make_pair(StatusBK->_id, StatusBK));

	CItemButtonBmp* StatusRight = new CItemButtonBmp();
	StatusRight->_id = DIRECTUI_STATUS_RIGHT;
	StatusRight->_bmp[0] = "statusright";
	StatusRight->_bmp[1] = "statusright";
	StatusRight->_bmp[2] = "statusright";
	StatusRight->_bmp[3] = "statusright";

	StatusRight->_rcopposite.left = -20;
	StatusRight->_rcopposite.right = -4;
	StatusRight->_rcopposite.top = -17;
	StatusRight->_rcopposite.bottom = -1;
	_ItemMap.insert(std::make_pair(StatusRight->_id, StatusRight));


	/*
	CreateButton(DIRECTUI_REGISTER, "注册", CRect(9, -31, 88, -5));	
	CreateButton(DIRECTUI_SETTING, "设置", CRect(90, -31, 169, -5));
	CreateButton(DIRECTUI_LOGIN, "登陆", CRect(171, -31, 250,-5));
	*/
	
	//CreateButton(DIRECTUI_REGISTER, "注册", CRect(-268, -31, -189, -5));	
	//CreateButton(DIRECTUI_SETTING, "设置", CRect(-187, -31, -107, -5));
	//CreateButton(DIRECTUI_LOGIN, "登陆", CRect(-105, -31, -25, -5));

	CreateButton(DIRECTUI_REGISTER, "关于", CRect(-105, -31, -25, -5));	
	CreateButton(DIRECTUI_UPGRADE, "升级", CRect(-187, -31, -107, -5));
	
	

	CRect rc;
	GetClientRect(_hWnd, &rc);
	OnSize(0, rc.Width(), rc.Height());
}