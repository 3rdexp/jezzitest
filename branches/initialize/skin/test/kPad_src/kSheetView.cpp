/*
 Copyright (c) 2000 
 Author: Konstantin Boukreev 
 E-mail: konstantin@mail.primorye.ru 

 Created: 27.06.2000 15:03:17
 Version: 1.0.0

*/

#include "stdafx.h"
#include "kSheetView.h"
#include "kHyperLink.h"

const TCHAR* RTF_EXT = _T(".rtf");

///////////////////////////////////////////////////////////
// kSheetView

#ifdef _RICHEDIT_USE_TOM
DISPID kSheetView::m_dispidUndo = -1;
#endif //_RICHEDIT_USE_TOM

kSheetView::kSheetView()
{
	memset(m_path, 0, sizeof m_path);
	memset(m_name, 0, sizeof m_name);	
	
	m_bSpace = false;
	m_iRequestWidth = 0;	
	m_nRow = m_nCol = 0;
	m_TabCount = 4;

	m_RightMargin = m_LeftMargin = 0;
	m_UndoSize = 100;
	m_MaxBuffer = 32767;

	m_bWordWrap = true;
	m_clrTextColor = GetSysColor(COLOR_WINDOWTEXT);
	m_clrBackColor = GetSysColor(COLOR_WINDOW);
	m_clrHighlightColor = GetSysColor(COLOR_WINDOW);
}

kSheetView::~kSheetView()
{		
}

void kSheetView::SetOption(kOption& option, int nFlag)
{
	#ifdef _RICHEDIT_USE_TOM
	// stop undo		
	CComDispatchDriver dd(m_pTom);
	if (dd.p != 0)
	{
		CComVariant varSuspend(-9999995); 
		HRESULT hr = dd.Invoke1(m_dispidUndo, &varSuspend, 0);
		ATLASSERT(SUCCEEDED(hr));
	}
	#endif //_RICHEDIT_USE_TOM

	CHARFORMAT2 cf;
	memset(&cf, 0, sizeof CHARFORMAT2);
	cf.cbSize = sizeof CHARFORMAT2;	

	if (nFlag & kOption::opFontName)
	{			
		cf.dwMask |= CFM_FACE;		
		lstrcpy(cf.szFaceName, option.szFontName); 
	}
	if (nFlag & kOption::opFontSize)
	{		 
		cf.dwMask |= CFM_SIZE;
		cf.yHeight = option.FontSize * 20;
	}
	if (nFlag & kOption::opFontWeight)
	{		 
		cf.dwMask |= CFM_WEIGHT;
		cf.wWeight = (WORD)option.FontWeight;
	}
	if (nFlag & kOption::opCharset)
	{		 
		cf.dwMask |= CFM_CHARSET;
		cf.bCharSet = (BYTE)option.Charset;
	}	
	if (nFlag & kOption::opTextColor)
	{
		m_clrTextColor = option.TextColor;
		COLORREF clr = 0;
		OleTranslateColor(m_clrTextColor, 0, &clr); 
		cf.dwMask |= CFM_COLOR; 		
		cf.crTextColor = clr;	
	}
	if (nFlag & kOption::opBackColor)
	{
		m_clrBackColor = option.BackColor; 
		COLORREF clr = 0;
		OleTranslateColor(m_clrBackColor, 0, &clr); 
		SetBackgroundColor(clr);
	}
	if (nFlag & kOption::opLeftMargin)
	{
		SendMessage(EM_SETMARGINS, EC_LEFTMARGIN, MAKELONG(option.LeftMargin, 0));
		m_LeftMargin = option.LeftMargin;
	}
	if (nFlag & kOption::opRightMargin)
	{			
		SendMessage(EM_SETMARGINS, EC_RIGHTMARGIN, MAKELONG(0, option.RightMargin));
		m_RightMargin = option.RightMargin;
	}
	if (nFlag & kOption::opTabCount)		
	{		
		m_TabCount = option.TabCount; 

		CFont font;
		if (!font.CreatePointFont(option.FontSize * 10, option.szFontName))	
			font = (HFONT)::GetStockObject(DEFAULT_GUI_FONT);	

		CClientDC dc(m_hWnd);
		HFONT hOldFont = dc.SelectFont(font);
	
		TEXTMETRIC tm;
		dc.GetTextMetrics(&tm);
		int nLogPix = dc.GetDeviceCaps(LOGPIXELSX);
		dc.SelectFont(hOldFont);
	
		int cxTab = ::MulDiv(tm.tmAveCharWidth * m_TabCount, 1440, nLogPix);	// 1440 twips = 1 inch
		if(cxTab != -1)
		{
			PARAFORMAT pf;
			pf.cbSize = sizeof PARAFORMAT;
			pf.dwMask = PFM_TABSTOPS;
			pf.cTabCount = MAX_TAB_STOPS;
			for(int i = 0; i < MAX_TAB_STOPS; i++)
				pf.rgxTabs[i] = (i + 1) * cxTab;

			SetSel(0, -1);
			SetParaFormat(pf);
			SetSel(-1, 0);
		}
	}	
	if (nFlag & kOption::opMaxBuffer)
	{		
		LimitText(option.MaxBuffer);
		m_MaxBuffer = option.MaxBuffer;
	}
	if (nFlag & kOption::opAutoURL)
	{		
		SetAutoURLDetect(option.AutoURL != 0);	
		long nEvents = GetEventMask();
		if (option.AutoURL != 0)
			nEvents |= ENM_LINK;
		else
			nEvents &= ~ENM_LINK;
		SetEventMask(nEvents);
	}
	if (nFlag & kOption::opWordWrap)
	{		
		m_bWordWrap = option.WordWrap != 0;
		SetTargetDevice(NULL, m_bWordWrap ? 0 : 1);
	}
	if (nFlag & kOption::opUndoSize)
	{		
		SetUndoLimit(option.UndoSize);
		m_UndoSize = option.UndoSize;
	}
		
	if (cf.dwMask)
		SendMessage(EM_SETCHARFORMAT, 0, (LPARAM)&cf); //default

	if (GetType(m_name) == SF_TEXT)
		SetModify(FALSE);

	#ifdef _RICHEDIT_USE_TOM	
	// start undo
	if (dd.p != 0)
	{
		CComVariant varResume(-9999994); 
		HRESULT hr = dd.Invoke1(m_dispidUndo, &varResume, 0);
		ATLASSERT(SUCCEEDED(hr));
	}
	#endif //_RICHEDIT_USE_TOM
	
}

void kSheetView::GetOption(kOption& option, int nFlag)
{	
	CHARFORMAT2 cf;					
	cf.cbSize = sizeof CHARFORMAT2;
	cf.dwMask = CFM_FACE | CFM_SIZE |  CFM_WEIGHT | CFM_CHARSET | CFM_SIZE | CFM_COLOR; //| CFM_BACKCOLOR 
	SendMessage(EM_GETCHARFORMAT, 0, (LPARAM)&cf); //get default format

	if (nFlag & kOption::opFontName)
	{				
		lstrcpy(option.szFontName, cf.szFaceName); 
	}
	if (nFlag & kOption::opFontSize)
	{		 	
		option.FontSize = cf.yHeight / 20;
	}
	if (nFlag & kOption::opFontWeight)
	{		 	
		option.FontWeight = cf.wWeight;
	}
	if (nFlag & kOption::opCharset)
	{		 	
		option.Charset = cf.bCharSet;
	}	
	if (nFlag & kOption::opTextColor)
	{
		option.TextColor = cf.crTextColor;	
	}
	if (nFlag & kOption::opBackColor)
	{	
		option.BackColor = m_clrBackColor; //cf.crBackColor; 		
	}
	if (nFlag & kOption::opLeftMargin)
	{	
		option.LeftMargin = m_LeftMargin;
	}
	if (nFlag & kOption::opRightMargin)
	{				
		option.RightMargin = m_RightMargin;
	}
	if (nFlag & kOption::opTabCount)		
	{		
		option.TabCount = m_TabCount; 
	}	
	if (nFlag & kOption::opMaxBuffer)
	{			
		option.MaxBuffer = m_MaxBuffer;
	}
	if (nFlag & kOption::opAutoURL)
	{		
		option.AutoURL = GetAutoURLDetect();			
	}
	if (nFlag & kOption::opWordWrap)
	{		
		option.WordWrap = m_bWordWrap ? -1 : 0;		
	}
	if (nFlag & kOption::opUndoSize)
	{			
		option.UndoSize = m_UndoSize;
	}
}
void kSheetView::InitNew()
{
	static int i = 0;
	TCHAR dir[_MAX_DIR] = {0};
	GetCurrentDirectory(_MAX_PATH, dir);
	
	TCHAR path[MAX_PATH];
	do {
		wsprintf(m_name, _T("New%d%s"), ++i, RTF_EXT);	
		wsprintf((TCHAR*)path, _T("%s\\%s"), dir, m_name);
	} while (PathExists(path));	
}

bool kSheetView::Load(const TCHAR* sPath, UINT uiFlag)
{
	if (DoLoad(sPath, uiFlag)) 
	{
		SetNamePath(sPath);
		ATLASSERT(lstrlen(m_path));
		SetModify(FALSE);
		return true;
	}
	return false;	
}

bool kSheetView::Save(const TCHAR* sArgPath, UINT uiFlag)
{	
	const TCHAR* sPath = sArgPath ? sArgPath : m_path;

	if (DoSave(sPath, uiFlag))
	{
		if (sArgPath)
			SetNamePath(sArgPath);
		SetModify(FALSE);
		return true;
	}
	return false;	
}

void kSheetView::OnEnLink (ENLINK *plnk)
{
	if (WM_LBUTTONDOWN == plnk->msg)
	{		
		SetSel(plnk->chrg); 
		int cb = plnk->chrg.cpMax - plnk->chrg.cpMin + 1;
		TCHAR* url = 0;
		if (cb > 1024)
			url = new TCHAR[cb];
		else
			url = (TCHAR *) alloca(cb * sizeof TCHAR);
		
		GetSelText(url);	
		SetSel(-1, 0);

		// navigate				
		kHyperLink::Open(url, kHyperLink::OPEN, m_hWnd); 

		if (cb > 1024)
			delete [] url;
	}
}

void kSheetView::SetCharFormatEx(CHARFORMAT2& cf, DWORD dwMask)
{
	cf.cbSize = sizeof CHARFORMAT2;
	cf.dwMask |= dwMask;
	
	CHARRANGE cr;
	GetSel(cr);
	if (cr.cpMax != cr.cpMin)
		SendMessage(EM_SETCHARFORMAT, SCF_SELECTION, (LPARAM)&cf);	
	else
		SendMessage(EM_SETCHARFORMAT, SCF_SELECTION | SCF_WORD, (LPARAM)&cf);		
}

////////////////////////////////////
// private

void kSheetView::SetNamePath(const TCHAR* path)
{	
	lstrcpy(m_path, path);
	const TCHAR* name = RFind(m_path, _T('\\'));
	if (name) lstrcpy(m_name, ++name);
}

bool kSheetView::DoLoad(const TCHAR* sPath, UINT uiFlag)
{
	HANDLE hFile = ::CreateFile(sPath, GENERIC_READ, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN, 0);
	if(INVALID_HANDLE_VALUE == hFile) return false;

	EDITSTREAM es;
	es.dwCookie = (DWORD)hFile;
	es.dwError = 0;
	es.pfnCallback = StreamReadCallback;	
	
	DWORD dwSizeLow = GetFileSize(hFile, 0);	
	CProgressBarCtrl* progress = g_Context.m_frame->GetProgressBar();	
	ATLASSERT(progress->IsWindow());
	progress->SetRange32(0, dwSizeLow);
	progress->SetPos(0);
	
	StreamIn(GetType(sPath) | uiFlag, es);
	::CloseHandle(hFile);
	return !es.dwError;	
}

bool kSheetView::DoSave(const TCHAR* sPath, UINT uiFlag)
{
	HANDLE hFile = ::CreateFile(sPath, GENERIC_WRITE, 0, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_ARCHIVE | FILE_FLAG_SEQUENTIAL_SCAN, 0);
	if(INVALID_HANDLE_VALUE == hFile) return false;

	EDITSTREAM es;
	es.dwCookie = (DWORD)hFile;
	es.dwError = 0;
	es.pfnCallback = StreamWriteCallback;
	
	// ! approximate size
	DWORD dwSize = GetTextLengthEx(GTL_NUMBYTES | GTL_CLOSE, CP_ACP); 
	
	CProgressBarCtrl* progress = g_Context.m_frame->GetProgressBar();	
	ATLASSERT(progress->IsWindow());
	progress->SetRange32(0, dwSize);
	progress->SetPos(0);

	StreamOut(GetType(sPath) | uiFlag, es);
	::CloseHandle(hFile);
	return !es.dwError;			
}

int  kSheetView::GetType(const TCHAR* sPath) const
{
	int r = SF_TEXT;
	const TCHAR* ext = RFind(sPath, _T('.'));
	if ( ext && (lstrcmpi(ext, RTF_EXT) == 0) )
		r = SF_RTF;
	return r;
}

const TCHAR* kSheetView::RFind (const TCHAR* str, const TCHAR ch)
{
	TCHAR* p = (TCHAR*)str;	
	while (*p++);
	while (--p != str)
		if(*p == ch) 
			return p;	
	return 0;
}

const TCHAR* kSheetView::RFind (const TCHAR* str, const TCHAR* ss)
{
	TCHAR* p = (TCHAR*)str;	
	while (*p++);
	while (--p != str) {
		const TCHAR* p1 = ss;
		while(*p1) { 
			if(*p1 == *p) {
				return p;
			}
			p1++;
		}
	}
	return 0;
}

bool kSheetView::PathExists (const TCHAR* path) const
{	
	WIN32_FIND_DATA fd;
	HANDLE hFind = FindFirstFile(path, &fd);
	if (INVALID_HANDLE_VALUE == hFind)
		return false;
	FindClose(hFind);
	return true;
}

DWORD CALLBACK kSheetView::StreamWriteCallback(DWORD dwCookie, LPBYTE pbBuff, LONG cb, LONG FAR *pcb)
{
	_ASSERTE(dwCookie != 0);
	_ASSERTE(pcb != NULL);

	DWORD dwRet = ::WriteFile((HANDLE)dwCookie, pbBuff, cb, (LPDWORD)pcb, NULL);	
	g_Context.m_frame->GetProgressBar()->OffsetPos(*pcb);	
	return !dwRet;
}

DWORD CALLBACK kSheetView::StreamReadCallback(DWORD dwCookie, LPBYTE pbBuff, LONG cb, LONG FAR *pcb)
{
	_ASSERTE(dwCookie != 0);
	_ASSERTE(pcb != NULL);

	DWORD dwRet = ::ReadFile((HANDLE)dwCookie, pbBuff, cb, (LPDWORD)pcb, NULL);
	g_Context.m_frame->GetProgressBar()->OffsetPos(*pcb);
	return !dwRet;
}

void kSheetView::InvertEffect (DWORD dwMask, DWORD dwEffect)
{
	CHARFORMAT2 cf;	
		
	GetCharFormatEx(cf, dwMask);	

	if (cf.dwEffects & dwEffect)
		cf.dwEffects &= ~dwEffect;
	else
		cf.dwEffects |= dwEffect;
	
	SetCharFormatEx(cf, dwMask);	
}

void kSheetView::SetAlignment (WORD dwAlign)
{
	PARAFORMAT pf;
	pf.cbSize = sizeof PARAFORMAT;
	pf.dwMask = PFM_ALIGNMENT;
	pf.wAlignment = dwAlign;
	SetParaFormat(pf);
}

void kSheetView::GetFormatColor (CHARFORMAT2& cf, OLE_COLOR& clr, DWORD iItem)
{
	cf.cbSize = sizeof CHARFORMAT2;

	if (ID_FORMAT_COLOR == iItem) 
	{
		cf.dwMask |= CFM_COLOR;
		SendMessage(EM_GETCHARFORMAT, 1, (LPARAM)&cf);	
		clr = cf.crTextColor;
	}
	else if (ID_FORMAT_HIGHLIGHT == iItem) 
	{
		cf.dwMask |= CFM_BACKCOLOR;
		SendMessage(EM_GETCHARFORMAT, 1, (LPARAM)&cf);	
		clr = cf.crBackColor;
	} 
	else 
	{
		clr = m_clrBackColor;
	}
}

void kSheetView::SetColor (OLE_COLOR clrNew, OLE_COLOR clrOld, DWORD iItem)
{
	if (clrNew != clrOld)
	{
		OleTranslateColor(clrNew, 0, &clrNew);

		CHARFORMAT2 cf;
		cf.cbSize = sizeof CHARFORMAT2;
		cf.dwEffects = 0;		

		if (ID_FORMAT_COLOR == iItem) 
		{
			cf.dwMask = CFM_COLOR;
			cf.crTextColor = clrNew;
			SendMessage(EM_SETCHARFORMAT, SCF_SELECTION | SCF_WORD, (LPARAM)&cf);	
			m_clrTextColor = clrNew;
		}
		else if (ID_FORMAT_HIGHLIGHT == iItem) 
		{
			cf.dwMask = CFM_BACKCOLOR;
			cf.crBackColor = clrNew;
			SendMessage(EM_SETCHARFORMAT, SCF_SELECTION | SCF_WORD, (LPARAM)&cf);	
			m_clrHighlightColor = clrNew;
		}		
		else 
		{
			SendMessage(EM_SETBKGNDCOLOR, 0, clrNew);
			m_clrBackColor = clrNew;
			::InvalidateRect(GetParent(), 0, TRUE);
		}
	}
}

/////////////////////////////////////////
// window message

LRESULT kSheetView::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
{
	LRESULT lRet = DefWindowProc(uMsg, wParam, lParam);
	DragAcceptFiles();
	SetEventMask(ENM_REQUESTRESIZE | ENM_SCROLL | ENM_DROPFILES);	
	SetModify(FALSE);

	#ifdef _RICHEDIT_USE_TOM
	CComPtr<IRichEditOle> pRichEdit;
	pRichEdit.Attach(GetOleInterface());		
	if (pRichEdit.p) {		
//		"8CC497C0-A1DF-11ce-8098-00AA0047BE5D"
		GUID iid = {0x8CC497C0, 0xA1DF, 0x11ce, {0x80, 0x98, 0x00, 0xAA, 0x00, 0x47, 0xBE, 0x5D}};		
		pRichEdit->QueryInterface(iid, (void**)&m_pTom); 
		ATLASSERT(m_pTom.p);
		if (-1 == m_dispidUndo)
		{
			wchar_t* names[] = {L"Undo"};
			DISPID dispids[] = {0};
			m_pTom->GetIDsOfNames(IID_NULL, names, 1, LOCALE_SYSTEM_DEFAULT, dispids);
			m_dispidUndo = dispids[0];
		}
	}
	#endif //_RICHEDIT_USE_TOM
	return lRet;
}

LRESULT kSheetView::OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
{
	bHandled = FALSE;
	DragAcceptFiles(FALSE);
	return 0;
}

LRESULT kSheetView::OnKey(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
{
	LRESULT lRet = DefWindowProc(uMsg, wParam, lParam);
			
	UpdateCurrentRowCol();

	if ((VK_SPACE == wParam) || (VK_RETURN == wParam)) 
	{
		if (!m_bSpace) 
		{
			SendMessage(EM_STOPGROUPTYPING);
			m_bSpace = true;
		}
	}
	else
	{
		if (m_bSpace) 
		{
			SendMessage(EM_STOPGROUPTYPING);
			m_bSpace = false;
		}
	}

	return lRet;
}

LRESULT kSheetView::OnEditWordWrap(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{	
	m_bWordWrap = !m_bWordWrap;
	int nLine = m_bWordWrap ? 0 : 1;
	SetTargetDevice(NULL, nLine);
	return 0;
}

LRESULT kSheetView::OnFormatFont(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	CHARFORMAT cf;	
	GetSelectionCharFormat(cf);		
	CRichEditFontDialog dlg(cf, CF_SCREENFONTS | CF_EFFECTS, 0, m_hWnd);
	
	if (dlg.DoModal() == IDOK)
	{
		dlg.GetCharFormat(cf);
		SetSelectionCharFormat(cf);	
	}
	return 0;
}

LRESULT kSheetView::OnFormatColor(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& /*bHandled*/)
{	
	ATLASSERT((wID == ID_FORMAT_COLOR) || (wID == ID_FORMAT_BGRN) || (wID == ID_FORMAT_HIGHLIGHT));

	OLE_COLOR clrNew, clrOld;
	CHARFORMAT2 cf;	
	cf.dwMask = CFM_SIZE;		
	GetFormatColor(cf, clrOld, wID);
	
	if (hWndCtl != 0)
	{
		if (wID == ID_FORMAT_COLOR)
			clrNew = m_clrTextColor;
		else if (wID == ID_FORMAT_HIGHLIGHT)
			clrNew = m_clrHighlightColor;
		else
			clrNew = m_clrBackColor;			
	}
	else
	{			
		if (wNotifyCode == 1)
		{		
			// accel
			if (GetFocus() != m_hWnd) 
				return 0;
		
			POINT pt;
			::GetCaretPos(&pt);			
			CClientDC dc(m_hWnd);			
			int nLogPix = dc.GetDeviceCaps(LOGPIXELSX);	
			pt.y += ::MulDiv(cf.yHeight, nLogPix, 1440);
		 			
			MapWindowPoints(HWND_DESKTOP, &pt, 1);		
			
			kColorDlg dlg;
			if (dlg.Pick(m_hWnd, pt.x, pt.y, clrOld, &clrNew) != S_OK) 
				return 0;
		}
		else 
		{
			// menu 			
			CColorDialog dlg(clrOld);
			if (dlg.DoModal() != IDOK)			
				return 0;
			clrNew = dlg.m_cc.rgbResult; 
		}		
	}

	SetColor(clrNew, clrOld, wID);
	return 0;
}

LRESULT kSheetView::OnToolbarDropDown(int /*idCtrl*/, LPNMHDR pnmh, BOOL& /*bHandled*/)
{	
	NMTOOLBAR* ptb = (NMTOOLBAR *) pnmh;
	ATLASSERT((ptb->iItem == ID_FORMAT_COLOR) || (ptb->iItem == ID_FORMAT_BGRN) || (ptb->iItem == ID_FORMAT_HIGHLIGHT));
	
	OLE_COLOR clrOld, clrNew; 
	POINT pt; RECT rc; 
//	int iIndex = -1; 
	CHARFORMAT2 cf;
		
	GetFormatColor(cf, clrOld, ptb->iItem);

	CToolBarCtrl tbar(pnmh->hwndFrom);	
	BOOL b = tbar.GetItemRect(tbar.CommandToIndex(ptb->iItem), &rc);	
	b;
	ATLASSERT(b);	
	pt.x = rc.left; pt.y = rc.bottom;
	tbar.MapWindowPoints(HWND_DESKTOP, &pt, 1);

	kColorDlg dlg;	
	if (dlg.Pick(m_hWnd, pt.x, pt.y, clrOld, &clrNew) != S_OK) 
		return 0;

	SetColor(clrNew, clrOld, ptb->iItem);
	return TBDDRET_DEFAULT;
}

LRESULT kSheetView::OnFormatBold(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	InvertEffect(CFM_BOLD, CFE_BOLD);	
	return 0;
}

LRESULT kSheetView::OnFormatItalic(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	InvertEffect(CFM_ITALIC, CFE_ITALIC);	
	return 0;
}

LRESULT kSheetView::OnFormatUnderline(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	InvertEffect(CFM_UNDERLINE, CFE_UNDERLINE);
	return 0;
}

LRESULT kSheetView::OnFormatAlignCnt(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	SetAlignment(PFA_CENTER);
	return 0;
}

LRESULT kSheetView::OnFormatAlignLeft(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	SetAlignment(PFA_LEFT);
	return 0;
}

LRESULT kSheetView::OnFormatAlignRight(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	SetAlignment(PFA_RIGHT);
	return 0;
}

LRESULT kSheetView::OnRefreshKeyboardLayout (UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
{	
	::PostMessage(g_Context.m_frame->GetWindow(), uMsg, wParam, lParam);
	return 0;
}

LRESULT kSheetView::OnRequestResize(int /*idCtrl*/, LPNMHDR pnmh, BOOL& /*bHandled*/)
{
	REQRESIZE *prs = (REQRESIZE *)pnmh;		
	ATLASSERT(prs->nmhdr.hwndFrom == m_hWnd);
	m_iRequestWidth = prs->rc.right - prs->rc.left;	
	return 0;
}

////////////////////////////////////
// find & replace function

bool kSheetView::DoFindNext(FindReplace* fr, BOOL bNext)
{
	ATLASSERT(fr != 0);
		
	CHARRANGE chrg;
	GetSel(chrg);

	FINDTEXTEX ft;
	
	if (fr->SearchDown()) 
	{
		ft.chrg.cpMin = bNext ? chrg.cpMax : chrg.cpMin;
		ft.chrg.cpMax = -1;
	} 
	else
	{
		ft.chrg.cpMin = chrg.cpMin;
		ft.chrg.cpMax = -1;
	}

	ft.lpstrText = fr->m_szFindWhat;

	if(FindText(fr->m_dwFlags, ft) == -1) 
		return false;
	
	SetSel(ft.chrgText);
	return true;
}

bool kSheetView::DoReplaceCurrent(FindReplace* fr)
{
	CHARRANGE chrg;
	GetSel(chrg);

	if(chrg.cpMin != chrg.cpMax)
	{
		USES_CONVERSION;
		LPSTR lpstrTextA = (LPSTR)_alloca(chrg.cpMax - chrg.cpMin + 2);
		GetSelText(lpstrTextA);
		LPTSTR lpstrText = A2T(lpstrTextA);
		int nRet;
		if(fr->MatchCase())
			nRet = lstrcmp(lpstrText, fr->m_szFindWhat);
		else
			nRet = lstrcmpi(lpstrText, fr->m_szFindWhat);
		if(nRet == 0)
			ReplaceSel(fr->m_szReplaceWith, TRUE);
	}

	return DoFindNext(fr);	
}

bool kSheetView::DoReplaceAll(FindReplace* fr)
{
	HCURSOR hOldCursor = 0;
		
	if(!DoFindNext(fr, FALSE))
		return false;
		
	SetRedraw(FALSE);
	
	hOldCursor = ::SetCursor(::LoadCursor(NULL, IDC_WAIT));

	do {
		ReplaceSel(fr->m_szReplaceWith, TRUE);
	} while(DoFindNext(fr, FALSE));
	
	SetRedraw(TRUE);
	Invalidate();
	UpdateWindow();

	if(hOldCursor != 0)
		::SetCursor(hOldCursor);

	return true;
}

