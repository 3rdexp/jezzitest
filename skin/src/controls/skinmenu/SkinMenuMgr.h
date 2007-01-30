// SkinMenuMgr.h: interface for the CSkinMenuMgr class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SKINMENUMGR_H__05F14DDA_161A_4291_B43A_4F5064081EED__INCLUDED_)
#define AFX_SKINMENUMGR_H__05F14DDA_161A_4291_B43A_4F5064081EED__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "skinmenu.h"
#include "SkinGlobals.h"
#include "hookmgr.h"
#include <map>
#include <afxtempl.h>

namespace Skin {

#ifndef NO_SKIN_INI
class CSkinIniGlobalsFile;
#endif

// pure static class
class CSkinMenuMgr : protected CHookMgr<CSkinMenuMgr>
{
	friend class CHookMgr<CSkinMenuMgr>; // to allow access to protected c'tor

public:
	virtual ~CSkinMenuMgr();

	static BOOL Initialize(DWORD dwMenuStyle = SKMS_SIDEBAR | SKMS_FLAT, int nSBWidth = 10, BOOL bNotXP = TRUE);
	static BOOL InitColor();
	static void SetColor(int nColor, COLORREF color);
	static COLORREF GetColor(int nColor);
	static void ClearColors();

	//添加的换肤的处理再里面了
	void AddhWnd(HWND hWnd );
	void RemovehWnd( HWND hWnd );
	void UpdateUI();
	BOOL InitSkin( HWND hWnd );
	static void RefreshSkin();

#ifndef NO_SKIN_INI
	static BOOL LoadSkin(const CSkinIniGlobalsFile* pIniFile);
	static void UnloadSkin();
#endif

protected:
	CMap<HWND, HWND, CSkinMenu*, CSkinMenu*&> m_mapMenus;
	DWORD m_dwMenuStyle;
	CSkinGlobals m_skGlobals;
	int m_nSidebarWidth;
	CSkinMenu* m_pCurSkinMenu;
	HWND m_hCurContextWnd;
	HMENU m_hCurMenu;

	std::map< HWND , HWND > _hwnd_maps;
	typedef std::map<HWND, HWND >::iterator SKINHWND_ITERATOR;

protected:
	CSkinMenuMgr();

	// message handlers
	virtual void OnCallWndProc(const MSG& msg);
	virtual BOOL OnCbt(int nCode, WPARAM wParam, LPARAM lParam);

	CSkinMenu* GetSkinMenu(HWND hWnd);
	BOOL Skin(HWND hWnd); 
	BOOL Unskin(HWND hWnd);
	CSkinMenu* GetParentSkinMenu(HMENU hMenu);
	void Release();
};
}; // namespace 
#endif // !defined(AFX_SkinMenuMgr_H__05F14DDA_161A_4291_B43A_4F5064081EED__INCLUDED_)
