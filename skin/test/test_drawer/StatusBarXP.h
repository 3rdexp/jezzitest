// StatusBarXP.h: interface for the CStatusBarXP class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_STATUSBARXP_H__51192FF1_886B_4ACD_9159_952746D3BBB4__INCLUDED_)
#define AFX_STATUSBARXP_H__51192FF1_886B_4ACD_9159_952746D3BBB4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <atlctrls.h>
#include <atlctrlx.h>
#include <atlmisc.h>

class CStatusBarXP : public CMultiPaneStatusBarCtrl
{
	typedef CStatusBarXP thisClass;
	typedef CMultiPaneStatusBarCtrl baseClass;
public:
	DECLARE_WND_CLASS(_T("CStatusBarXP"))

	BEGIN_MSG_MAP( thisClass)
	MESSAGE_HANDLER( WM_PAINT, OnPaint)
	CHAIN_MSG_MAP( baseClass)
	END_MSG_MAP()

	BOOL SubclassWindow(HWND hWnd);
	void SetPaneWidths(int* arrWidths, int nPanes);	

protected:
	LRESULT OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
};

#endif // !defined(AFX_STATUSBARXP_H__51192FF1_886B_4ACD_9159_952746D3BBB4__INCLUDED_)
