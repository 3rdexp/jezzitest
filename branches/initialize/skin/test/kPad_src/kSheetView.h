/*
 Copyright (c) 2000 
 Author: Konstantin Boukreev 
 E-mail: konstantin@mail.primorye.ru 

 Created: 27.06.2000 14:56:26
 Version: 1.0.0

*/

#ifndef _kSheetView_fb173eaf_47d9_4a39_976c_a22ff9f15745
#define _kSheetView_fb173eaf_47d9_4a39_976c_a22ff9f15745

#if _MSC_VER > 1000 
#pragma once
#endif // _MSC_VER > 1000

#include "resource.h"
#include <atlctrls.h>
#include <atldlgs.h>
#include "kColorDlg.h"
#include "FindReplace.h"
#include "common.h"
#include "option.h"

class kSheetView : 
	public CWindowImpl<kSheetView, CRichEditCtrl>,
	public CRichEditCommands<kSheetView>
{	
	TCHAR m_path[_MAX_PATH];
	TCHAR m_name[_MAX_FNAME];

	bool m_bWordWrap;
	bool m_bSpace;

	COLORREF m_clrTextColor;
	COLORREF m_clrBackColor;
	COLORREF m_clrHighlightColor;

	int m_nRow;
	int m_nCol;
	int m_iRequestWidth;
	
	DWORD m_TabCount;
	DWORD m_LeftMargin;
	DWORD m_RightMargin;	
	DWORD m_UndoSize;
	DWORD m_MaxBuffer;

	#ifdef _RICHEDIT_USE_TOM
	CComPtr<IDispatch> m_pTom; //ITextDocument, TOM model
	static DISPID m_dispidUndo;
	#endif //_RICHEDIT_USE_TOM
	
public:
	kSheetView();
	~kSheetView();

	DECLARE_WND_SUPERCLASS(NULL, CRichEditCtrl::GetWndClassName())

	BEGIN_MSG_MAP(kSheetView)
		MESSAGE_HANDLER(WM_CREATE, OnCreate)
		MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
		MESSAGE_HANDLER(WM_KEYDOWN, OnKey)
		MESSAGE_HANDLER(WM_KEYUP, OnKey)
		MESSAGE_HANDLER(WM_LBUTTONDOWN, OnKey)
		MESSAGE_HANDLER(WM_INPUTLANGCHANGE, OnRefreshKeyboardLayout)
		COMMAND_ID_HANDLER(ID_EDIT_WORD_WRAP, OnEditWordWrap)
		COMMAND_ID_HANDLER(ID_FORMAT_FONT, OnFormatFont)
		COMMAND_ID_HANDLER(ID_FORMAT_COLOR, OnFormatColor)
		COMMAND_ID_HANDLER(ID_FORMAT_HIGHLIGHT, OnFormatColor)
		COMMAND_ID_HANDLER(ID_FORMAT_BGRN, OnFormatColor)
		COMMAND_ID_HANDLER(ID_FORMAT_BOLD, OnFormatBold)
		COMMAND_ID_HANDLER(ID_FORMAT_ITALIC, OnFormatItalic)
		COMMAND_ID_HANDLER(ID_FORMAT_UNDERLINE, OnFormatUnderline)
		COMMAND_ID_HANDLER(ID_FORMAT_CNT, OnFormatAlignCnt)
		COMMAND_ID_HANDLER(ID_FORMAT_LEFT, OnFormatAlignLeft)
		COMMAND_ID_HANDLER(ID_FORMAT_RIGHT, OnFormatAlignRight)		
		NOTIFY_CODE_HANDLER(TBN_DROPDOWN, OnToolbarDropDown)
		NOTIFY_CODE_HANDLER(EN_REQUESTRESIZE, OnRequestResize)		
		CHAIN_MSG_MAP_ALT(CRichEditCommands<kSheetView>, 1)		
	END_MSG_MAP()

// window message
private:
	LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);		
	LRESULT OnKey(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);	
	LRESULT OnEditWordWrap(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnFormatFont(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnToolbarDropDown(int idCtrl, LPNMHDR pnmh, BOOL& bHandled);
	LRESULT OnFormatColor(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);	
	LRESULT OnFormatBold(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnFormatItalic(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnFormatUnderline(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnFormatAlignCnt(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnFormatAlignLeft(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnFormatAlignRight(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);	
	LRESULT OnRefreshKeyboardLayout (UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnRequestResize(int idCtrl, LPNMHDR pnmh, BOOL& bHandled);
				
// kSheetView intreface
public:
	const TCHAR* Name() const;
	const TCHAR* Path() const;	
	bool IsDirty() const;
	bool IsWordWrap() const;	
	void InitNew();
	bool Load(const TCHAR* path, UINT uiFlag = 0);	
	bool Save(const TCHAR* path = 0, UINT uiFlag = 0);				
	void OnEnLink (ENLINK*);
	bool DoFindNext(FindReplace* fr, BOOL bNext = TRUE);
	bool DoReplaceCurrent(FindReplace* fr);
	bool DoReplaceAll(FindReplace* fr);
	void SetCharFormatEx(CHARFORMAT2& cf, DWORD dwMask);
	void GetCharFormatEx(CHARFORMAT2& cf, DWORD dwMask);
	bool IsEffect (DWORD, DWORD);	
	DWORD GetAlign ();
	void GetCurrentRowCol (int& nRow, int &nCol, bool bUpdate = false);
	void UpdateCurrentRowCol();	
	COLORREF GetBackgroundColor () const;
	int GetRequestWidth() const;	

	BOOL PreTranslateMessage(MSG* pMsg);

	void SetOption(kOption&, int nFlag = kOption::opAll);
	void GetOption(kOption&, int nFlag = kOption::opAll);

private:		
	void SetNamePath(const TCHAR* path);		
	bool PathExists (const TCHAR* path) const;
	int  GetType(const TCHAR* sPath) const;
	bool DoLoad(const TCHAR* sPath, UINT uiFlag = 0);
	bool DoSave(const TCHAR* sPath, UINT uiFlag = 0);
	void InvertEffect (DWORD, DWORD);
	void SetAlignment (WORD);
	void GetFormatColor (CHARFORMAT2& cf, OLE_COLOR& clr, DWORD iItem);
	void SetColor (OLE_COLOR, OLE_COLOR, DWORD iItem);
	
// rich edit callback
private:
	static DWORD CALLBACK StreamWriteCallback(DWORD dwCookie, LPBYTE pbBuff, LONG cb, LONG FAR *pcb);	
	static DWORD CALLBACK StreamReadCallback(DWORD dwCookie, LPBYTE pbBuff, LONG cb, LONG FAR *pcb);	

public:
	static const TCHAR* RFind (const TCHAR* str, const TCHAR ch);
	static const TCHAR* RFind (const TCHAR* str, const TCHAR* ss);

};

////////////////////////////////////////////
// inline

inline void kSheetView::
UpdateCurrentRowCol()
{
	long nStartPos, nEndPos;
	GetSel(nStartPos, nEndPos);
	m_nRow = LineFromChar(nEndPos);
	m_nCol = 0;
	int nChar = nEndPos - LineIndex();
	if(nChar > 0)
	{
		LPTSTR lpstrLine = (LPTSTR)_alloca(max(2, (nChar + 1) * sizeof(TCHAR)));	// min = WORD for length
		nChar = GetLine(m_nRow, lpstrLine, nChar);
		for(int i = 0; i < nChar; i++)
		{
			if(lpstrLine[i] == _T('\t'))
				m_nCol = ((m_nCol / m_TabCount) + 1) * m_TabCount;
			else
				m_nCol++;
		}
	}

	m_nRow++; m_nCol++;
}

inline void kSheetView::
GetCurrentRowCol (int& nRow, int &nCol, bool bUpdate)
{
	if (bUpdate)
		UpdateCurrentRowCol();
	nRow =  m_nRow;
	nCol =  m_nCol;		
}

inline COLORREF kSheetView::
GetBackgroundColor () const 
{
	COLORREF clr;
	OleTranslateColor(m_clrBackColor, 0, &clr); 
	return clr;
}

inline bool kSheetView::
IsWordWrap() const 
{
	return m_bWordWrap;
}

inline int kSheetView::
GetRequestWidth() const 
{
	return m_iRequestWidth;
}

inline const TCHAR* kSheetView::
Name() const
{
	return m_name;
}

inline const TCHAR* kSheetView::
Path() const
{
	return m_path;
}

inline bool  kSheetView::
IsDirty() const
{
	return GetModify() != 0;
}

inline BOOL kSheetView::
PreTranslateMessage(MSG* /*pMsg*/)
{
	return FALSE;
}

inline bool kSheetView::
IsEffect (DWORD dwMask, DWORD dwEffect)
{
	CHARFORMAT2 cf;			
	GetCharFormatEx(cf, dwMask);	

	return (cf.dwEffects & dwEffect) != 0;
}

inline DWORD kSheetView::
GetAlign ()
{
	PARAFORMAT pf;
	pf.cbSize = sizeof PARAFORMAT;
	pf.dwMask = PFM_ALIGNMENT;	
	GetParaFormat(pf);

	return pf.wAlignment;
}

inline void kSheetView::
 GetCharFormatEx(CHARFORMAT2& cf, DWORD dwMask)
{	
	cf.dwMask |= dwMask;
	cf.cbSize = sizeof CHARFORMAT2;
	SendMessage(EM_GETCHARFORMAT, 1, (LPARAM)&cf);
}

#endif //_kSheetView_fb173eaf_47d9_4a39_976c_a22ff9f15745

