/*
 Copyright (c) 2000 
 Author: Konstantin Boukreev 
 E-mail: konstantin@mail.primorye.ru 

 Created: 11.07.2000 17:58:47
 Version: 1.0.0

*/

#ifndef _Option_35b51ee4_f087_4e98_bd99_169456593b64
#define _Option_35b51ee4_f087_4e98_bd99_169456593b64

#if _MSC_VER > 1000 
#pragma once
#endif // _MSC_VER > 1000

struct kOption
{
	enum {
		opNothing		= 0x0000, 
		opFontName 		= 0x0001, 
		opFontSize		= 0x0002, 
		opFontWeight	= 0x0004, 
		opCharset		= 0x0008, 
		opTextColor		= 0x0010,
		opBackColor		= 0x0020,
		opLeftMargin	= 0x0040,
		opRightMargin	= 0x0080,
		opTabCount		= 0x0100,
		opUndoSize		= 0x0200,
		opMaxBuffer		= 0x0400,
		opAutoURL		= 0x0800,
		opWordWrap		= 0x1000,
		opAll				= 0xffff };
	
	TCHAR szFontName[LF_FACESIZE];
	DWORD FontSize;
	DWORD FontWeight;
	DWORD Charset;
	DWORD TextColor;
	DWORD BackColor;	
	DWORD LeftMargin;
	DWORD RightMargin;
	DWORD TabCount;
	DWORD UndoSize;
	DWORD MaxBuffer;
	DWORD AutoURL;	
	DWORD WordWrap;	
	
	kOption();
	
	bool Load(HKEY);
	bool Save(HKEY);
};

inline kOption::kOption()
{	
	TextColor = GetSysColor(COLOR_WINDOWTEXT);
	BackColor = GetSysColor(COLOR_WINDOW);

	UndoSize = 32;
	AutoURL = TRUE;	
	WordWrap = TRUE;
	LeftMargin = 20;
	RightMargin = 0;
	MaxBuffer = 0x400000;
	TabCount = 4;
	
	lstrcpy(szFontName, _T("Lucida Sans Unicode"));
	FontSize = 10;	
	FontWeight = FW_NORMAL;
	Charset = ANSI_CHARSET;
}

inline bool kOption::
Load(HKEY hKeyParent)
{
//	TCHAR sKey[MAX_PATH];
//	wsprintf(sKey, _T("%s\\%s"), kPadRegKey, _T("Option"));

	CRegKey key;
	if (ERROR_SUCCESS != key.Open(hKeyParent, _T("Option")))
		return false;
	
	key.QueryValue(TextColor,	_T("TextColor"));
	key.QueryValue(BackColor,	_T("BackColor"));
	key.QueryValue(UndoSize,	_T("UndoSize"));
	key.QueryValue(AutoURL,		_T("AutoURL"));
	key.QueryValue(WordWrap,	_T("WordWrap"));
	key.QueryValue(LeftMargin,	_T("LeftMargin"));
	key.QueryValue(RightMargin,_T("RightMargin"));	
	key.QueryValue(MaxBuffer,	_T("MaxBuffer"));
	key.QueryValue(TabCount,	_T("TabCount"));

	DWORD dw = LF_FACESIZE;
	key.QueryValue(szFontName,	_T("FontName"), &dw);
	key.QueryValue(FontSize,	_T("FontSize"));
	key.QueryValue(FontWeight,	_T("FontWeight"));
	key.QueryValue(Charset,		_T("Charset"));

	return true;
}

inline bool kOption::
Save(HKEY hKeyParent)
{
	CRegKey key;
	if (ERROR_SUCCESS != key.Create(hKeyParent, _T("Option")))
		return false;

	key.SetValue(TextColor,		_T("TextColor"));
	key.SetValue(BackColor,		_T("BackColor"));
	key.SetValue(UndoSize,		_T("UndoSize"));
	key.SetValue(AutoURL,		_T("AutoURL"));
	key.SetValue(WordWrap,		_T("WordWrap"));
	key.SetValue(LeftMargin,	_T("LeftMargin"));
	key.SetValue(RightMargin,	_T("RightMargin"));	
	key.SetValue(MaxBuffer,		_T("MaxBuffer"));
	key.SetValue(TabCount,		_T("TabCount"));	
	key.SetValue(szFontName,	_T("FontName"));	
	key.SetValue(FontSize,		_T("FontSize"));
	key.SetValue(FontWeight,	_T("FontWeight"));
	key.SetValue(Charset,		_T("Charset"));

	return true;
}

#endif //_Option_35b51ee4_f087_4e98_bd99_169456593b64

