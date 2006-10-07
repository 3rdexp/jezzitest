/*
 Copyright (c) 2000 
 Author: Konstantin Boukreev 
 E-mail: konstantin@mail.primorye.ru 

 Created: 26.06.2000 19:12:35
 Version: 1.0.0 

 Portions copyright Mihai Filimon (mihai@transart.ro). 
	from article in http://www.codeguru.com/controls/sheet_control.shtml
 Thanks a lot!
*/

#ifndef _kSheet_4b3991b4_c599_4fc1_89e7_62d134a53d5b
#define _kSheet_4b3991b4_c599_4fc1_89e7_62d134a53d5b

#if _MSC_VER > 1000 
#pragma once
#endif // _MSC_VER > 1000

#include "kSheetView.h"
#include "FindReplace.h"

class kSheetsCtrl : public CWindowImpl<kSheetsCtrl>
{
public:		
	enum {
		DELTA_ARROW = 5,
		SC_ONCHANGESHEET = WM_USER + 758
	};

private:
	CSimpleArray<kSheetView*> m_sheets;
	
	CContainedWindowT<CScrollBar> m_winArrow;
	CContainedWindowT<CScrollBar> m_winScroll;
	
	HCURSOR m_hCursor;
	
	CFont m_font;
	CFont m_bfont;

	RECT m_rcClient;
	RECT m_rcScroll;
	SIZE m_szArrow;
	
	bool m_bShowScrollBar;
	bool m_bTrack;

	int m_iActive;

	int m_xMoveBar;
	int m_xPages;

	FindReplace m_fr;
	FindDlg m_FindDlg;
	ReplaceDlg m_ReplaceDlg;
	
public:
	kSheetsCtrl();
	~kSheetsCtrl();

	DECLARE_WND_CLASS_EX(_T("kSheetsCtrl"), CS_HREDRAW | CS_VREDRAW, COLOR_BTNSHADOW)

	BEGIN_MSG_MAP(kSheetsCtrl)
		MESSAGE_HANDLER(WM_CREATE, OnCreate)
		MESSAGE_HANDLER(WM_DESTROY,	OnDestroy)
		MESSAGE_HANDLER(WM_ERASEBKGND, OnEraseBkgnd)
		MESSAGE_HANDLER(WM_PAINT, OnPaint)	
		MESSAGE_HANDLER(WM_SIZE, OnSize)
		MESSAGE_HANDLER(WM_MOUSEMOVE, OnMouseMove)
		MESSAGE_HANDLER(WM_LBUTTONDOWN, OnLButtonDown)
		MESSAGE_HANDLER(WM_LBUTTONUP, OnLButtonUp)	
		MESSAGE_HANDLER(WM_HSCROLL, OnHScroll)	
		MESSAGE_HANDLER(WM_INPUTLANGCHANGE, OnRefreshKeyboardLayout)
		
		MESSAGE_HANDLER(WM_DROPFILES, OnDropFiles)	

		NOTIFY_CODE_HANDLER(EN_DROPFILES, OnEnDropFiles)
		NOTIFY_CODE_HANDLER(EN_LINK, OnEnLink)
		NOTIFY_CODE_HANDLER(EN_REQUESTRESIZE, OnRequestResize)
		COMMAND_CODE_HANDLER(EN_HSCROLL, OnHScrollNotify)		

		COMMAND_ID_HANDLER(ID_EDIT_FIND, OnEditFind)
		COMMAND_ID_HANDLER(ID_EDIT_REPEAT, OnEditFindNext)
		COMMAND_ID_HANDLER(ID_EDIT_REPLACE, OnEditReplace)

		MESSAGE_HANDLER(FindReplace::GetFindReplaceMsg(), OnFindReplaceCmd)

	ALT_MSG_MAP(1) //left arrow
	ALT_MSG_MAP(2) //scrollbar		
	END_MSG_MAP()

// window message
protected:	
	LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);				
	LRESULT OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);	
	LRESULT OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);		
	LRESULT OnMouseMove(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);	
	LRESULT OnLButtonUp(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnLButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnHScroll(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);	
	LRESULT OnEraseBkgnd(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);	
	LRESULT OnHScrollNotify(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);		
	LRESULT OnRequestResize(int idCtrl, LPNMHDR pnmh, BOOL& bHandled);		
	LRESULT OnEnLink(int idCtrl, LPNMHDR pnmh, BOOL& bHandled);	
	LRESULT OnEditFind(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnEditFindNext(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnEditReplace(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT  OnFindReplaceCmd(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnEnDropFiles(int idCtrl, LPNMHDR pnmh, BOOL& bHandled);
	LRESULT OnDropFiles(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnRefreshKeyboardLayout (UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
	{
		::PostMessage(g_Context.m_frame->GetWindow(), uMsg, wParam, lParam);
		return 0;
	}

// 
public:
	BOOL PreTranslateMessage(MSG* pMsg);

// public intreface
public:
	kSheetView* Add();
	void Close(int);
	void Active(int);
	int Active() const;
	unsigned int Count() const;
	bool IsEmpty() const;
	kSheetView* GetAt(int i) const;
			
// utilites
private:
	void DrawSheets(HDC hdc, RECT&);
	void DrawMoveBar(HDC hdc, RECT&);
	void DrawCurve(HDC hdc, COLORREF clr, POINT* pts, int n, bool bMove);		
	void UpdateArrow ();
	void UpdateScrollBar();
	void OnNotify();
	bool HitMoverBar (int);	
	void RedrawNeed();	
	int HitTest(int, int);
	void SetMoveBar (int);	
	void ApplyLayout(int, int);	
	void DoDropFiles(HDROP);
};

///////////////////////////////////////////
// inline

inline LRESULT kSheetsCtrl::
OnEraseBkgnd(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
{
	if (IsEmpty()) {
		bHandled = FALSE;
		return 0;
	}
	return 1;
}

inline void kSheetsCtrl::
OnNotify()
{
	DWORD idCtrl = GetWindowLong(GWL_ID);
	NMHDR nmhdr = {m_hWnd, idCtrl, SC_ONCHANGESHEET};
	::SendMessage(GetParent(), WM_NOTIFY, idCtrl, (LPARAM)&nmhdr);
}

inline int kSheetsCtrl::
Active() const 
{
	return m_iActive;
}

inline unsigned int kSheetsCtrl::
Count() const 
{
	return m_sheets.GetSize();
}

inline bool kSheetsCtrl::
IsEmpty() const 
{
	return m_sheets.GetSize() == 0;
}

inline kSheetView* kSheetsCtrl::
GetAt(int i) const
{
	return m_sheets[i];
}

#endif //_kSheet_4b3991b4_c599_4fc1_89e7_62d134a53d5b

