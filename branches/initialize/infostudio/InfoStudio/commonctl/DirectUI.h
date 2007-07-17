#pragma once

#include <string>
#include <vector>
#include <map>
#include <atlmisc.h>


enum
{
	STATE_NORMAL = 0,
	STATE_MOUSEMOVE = 1,
	STATE_MOUSEDOWN = 2,
	STATE_DISABLE = 3,
};

class CItemBase
{
public:
	int			_id;
	std::string _caption;
	std::string _tooltip;		
	CRect		_rc;
	CRect		_rcopposite; //相对坐标
	BOOL		_hide;
	BOOL		_enable;
	virtual void Draw(HDC hdc, BOOL bSelect, BOOL bPress) = 0;

	CItemBase(void);
	
	virtual  ~CItemBase(void)
	{
		_caption = "";
		_tooltip = "";
		_hide = FALSE;
		_enable = TRUE;
	}
	virtual int  OnMouseMove(UINT nFlags, CPoint point)
	{
		return -1;
	}

	virtual int  OnLButtonUp(UINT nFlags, CPoint point)
	{
		return -1;
	}

	virtual int  OnLButtonDown(UINT nFlags, CPoint point)
	{
		return -1;
	}

	virtual void OnSize(UINT nType, int cx, int cy)
	{
		if (_rcopposite.left >= 0)
			_rc.left = _rcopposite.left;		
		else
			_rc.left = cx + _rcopposite.left;

		if (_rcopposite.right > 0)
			_rc.right = _rcopposite.right;
		else
			_rc.right = cx + _rcopposite.right;

		if (_rcopposite.top >= 0)
			_rc.top = _rcopposite.top;
		else
			_rc.top = cy + _rcopposite.top;

		if (_rcopposite.bottom > 0)
			_rc.bottom = _rcopposite.bottom;
		else
			_rc.bottom = cy + _rcopposite.bottom;	
	}

	virtual BOOL HitTest(CPoint point)
	{
		return !_hide&&_enable && _rc.PtInRect(point);
	}
	int GetState2Img(BOOL bSelect, BOOL bPress)
	{
		int nIndex = STATE_NORMAL;

		if (bSelect)
			nIndex = STATE_MOUSEMOVE;
		if (bPress && bSelect)
			nIndex = STATE_MOUSEDOWN;
		if (!_enable)
			nIndex = STATE_DISABLE;

		return nIndex;
	}

	int GetState2Icon(BOOL bSelect, BOOL bPress)
	{
		int nIndex = STATE_NORMAL;

		if (bSelect)
			nIndex = STATE_MOUSEMOVE;		
		if (!_enable)
			nIndex = STATE_DISABLE;

		return nIndex;
	}


};

// 显示指定的图片
class CItemButtonBmp : public CItemBase
{
public:
	std::string	_bmp[4];	
	void Draw(HDC hdc, BOOL bSelect = FALSE, BOOL bPress = FALSE);
	CItemButtonBmp()
	{
		_enable = FALSE;
		std::fill(_bmp, _bmp + 4, "");		
	}

	//virtual BOOL HitTest(CPoint point)
	//{
	//	return FALSE;
	//}
};

// 显示Button包括Text 和icon等

class CItemButtonTextIcon : public CItemBase
{
public:
	enum
	{
		TEXTICONSPACE = 5,
	};
	std::string _icon;
	BOOL		_bTextLeft;
	int			_iconWidth;	
	CItemButtonTextIcon()
	{
		_iconWidth = 16;
		_bTextLeft = FALSE; 
		_icon = "";
	};	
	void Draw(HDC hdc, BOOL bSelect = FALSE, BOOL bPress = FALSE);
};


class CItemTab : public CItemBase
{
public:

	class CItemTabItem 
	{
	public:
		int				_id;
		std::string		_name;
		CRect			_rc;
		CRect			_selectrc;
	};
	int							_selectID;
	int							_mousemove;
	std::vector<CItemTabItem>	_ListTab;
	std::string					_backImg;
	std::string					_img[4]; // unselect select unselect mousemove select mousemove;
	int							_space;
	BOOL						_bLeft;
	int							_itemspace;
	HWND						_hWnd;

	COLORREF					_clrNormal;
	COLORREF					_clrSelect;
	CFont						_font;
	CFont						_fontSelect;
	CItemTab()
	{	
		_selectID = 0;
		_space = 30;
		_bLeft = FALSE;
		_itemspace = 2;
		_mousemove = -1;
		_hWnd = 0;
		_clrNormal = RGB(0, 0, 0);
		_clrSelect = RGB(0, 0, 0);

		LOGFONT	logfont;
		::GetObject((HFONT)GetStockObject(DEFAULT_GUI_FONT),sizeof(LOGFONT),&logfont);		
		_font.CreateFontIndirect(&logfont);

		logfont.lfWeight  = FW_BOLD;
		_fontSelect.CreateFontIndirect(&logfont);
	}

	void SetParent(HWND hWnd)
	{
		_hWnd = hWnd;
	}
	void Draw(HDC hdc, BOOL bSelect = FALSE, BOOL bPress = FALSE);

	void AddItem(int nID, LPCSTR name)
	{
		CItemTabItem item;
		item._id = nID;
		item._name = name;
		_ListTab.push_back(item);
	}
	int HitTestTab (CPoint point);
	void HighlightItem (int nSelect);
	void HighlightSelectItem(int nSelect);
	int OnMouseMove(UINT nFlags, CPoint point);
	int OnLButtonUp(UINT nFlags, CPoint point);	
	int OnLButtonDown(UINT nFlags, CPoint point);
	void OnSize(UINT nType, int cx, int cy);
	BOOL HitTest(CPoint point);
};

class CDirectUI
{
public:
	CDirectUI(void);
	~CDirectUI(void);


	// message
	void OnSize(UINT nType, int cx, int cy);
	int OnMouseMove(UINT nFlags, CPoint point);
	int OnLButtonUp(UINT nFlags, CPoint point);
	int OnLButtonDown(UINT nFlags, CPoint point);

	//draw
	void DrawItem(HDC hdc, CItemBase* pItem);
	void DrawAllItem(HDC hdc);

	// function

	CItemBase* getItem(int nID);
	void ClearAll();
	int HitTest(CPoint point);
	void SetParent(HWND hWnd);

	void HighlightItem(int nSelect, BOOL bForce = FALSE);

	void Init();
	void InitSearchDlg();

	void CreateButton(int nId, const char* szCaption, CRect rc, BOOL bShow = TRUE, const char* szIcon = "");
	void Show(int nID, BOOL bShow, BOOL bInvalidate = TRUE);
private:
	int		_selectID;
	HWND	_hWnd;
	CRect	_rc;
	BOOL	_bPressed;

	std::map<int, CItemBase*> _ItemMap;	
	typedef std::map<int, CItemBase*>::iterator _ItemMap_ITERATOR;	
};
