// StatusBarXP.cpp: implementation of the CStatusBarXP class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "StatusBarXP.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
BOOL CStatusBarXP::SubclassWindow(HWND hWnd)
{
	BOOL bRes = baseClass::SubclassWindow( hWnd);

	return bRes;
}

LRESULT CStatusBarXP::OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	DefWindowProc();

	CClientDC cDC (m_hWnd);
	CPen pen;
	pen.CreatePen( PS_SOLID, 1, ::GetSysColor (COLOR_3DSHADOW));

	HPEN hOldPen = cDC.SelectPen( pen);

	CRect rcItem;

	HBRUSH hOldBrush = cDC.SelectBrush (AtlGetStockBrush(NULL_BRUSH));

	for ( int i = 0; i < m_nPanes; i++ )
	{
		GetRect (i, rcItem);
		cDC.Rectangle (rcItem); 
	}

	cDC.SelectBrush(hOldBrush);
	cDC.SelectPen( hOldPen);
	
	return 0;
}

// this workaround solves a bug in CMultiPaneStatusBarCtrl
// (in SetPanes() method) that limits the size of all panes
// after the default pane to a combined total of 100 pixels  
void CStatusBarXP::SetPaneWidths(int* arrWidths, int nPanes)
{
	ATLASSERT( m_nPanes == nPanes);
    // find the size of the borders
    int arrBorders[3];
    GetBorders(arrBorders);

    // calculate right edge of default pane (0)
    arrWidths[0] += arrBorders[2];
    for (int i = 1; i < nPanes; i++)
        arrWidths[0] += arrWidths[i];

    // calculate right edge of remaining panes (1 thru nPanes-1)
    for (int j = 1; j < nPanes; j++)
        arrWidths[j] += arrBorders[2] + arrWidths[j - 1];

    // set the pane widths
    SetParts(m_nPanes, arrWidths); 
}
