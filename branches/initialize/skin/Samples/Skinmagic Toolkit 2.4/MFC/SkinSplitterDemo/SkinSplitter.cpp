// SkinSplitter.cpp : implementation file
//

#include "stdafx.h"
#include "SkinSplitterDemo.h"
#include "SkinSplitter.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSkinSplitter

CSkinSplitter::CSkinSplitter()
{
	m_hSkin = NULL;
}

CSkinSplitter::~CSkinSplitter()
{
}


BEGIN_MESSAGE_MAP(CSkinSplitter, CSplitterWnd)
	//{{AFX_MSG_MAP(CSkinSplitter)
	ON_WM_PARENTNOTIFY()
	//}}AFX_MSG_MAP
	ON_MESSAGE( WM_SKINDATACHANGED , OnSkinDataChanged )
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CSkinSplitter message handlers
void CSkinSplitter::OnDrawSplitter(CDC* pDC, ESplitType nType, const CRect& rectArg)
{
	if (pDC == NULL)
	{
		RedrawWindow(rectArg, NULL, RDW_INVALIDATE|RDW_NOCHILDREN);
		return;
	}

	ASSERT_VALID(pDC);
	CRect rect = rectArg;
	switch (nType)
	{
	case splitBorder:
		break;

	case splitBar:
		{
			if( m_hSkin )
			{
				DrawSkinImageSection( m_hSkin, "SplitBox", pDC->m_hDC, &rect );
				return;
			}
		}
		break;
	}

	CSplitterWnd::OnDrawSplitter( pDC , nType , rect );
}

void CSkinSplitter::OnInvertTracker(const CRect& rect)
{
	CSplitterWnd::OnInvertTracker( rect );
}

LRESULT CSkinSplitter::OnSkinDataChanged(WPARAM wParam, LPARAM  lParam)
{
	if( m_hSkin ) CloseSkinData( m_hSkin );

	m_hSkin = OpenSkinData( _T("SkinSplitter") );
	if( m_hSkin )
	{
		int iSplitterBarSize = m_cxSplitter;
		if( GetSkinInt( m_hSkin , _T("BarSize") , &iSplitterBarSize ) )
		{
			//iSplitterBarSize = 10;
			m_cxSplitterGap = m_cySplitterGap = m_cxSplitter = m_cySplitter = iSplitterBarSize;
			//if( m_nRows>0 && m_nCols > 0 )
			//{
			//	RecalcLayout();
			//	RedrawWindow();
			//}
		}
	}
	return 1;
}

void CSkinSplitter::OnParentNotify(UINT message, LPARAM lParam) 
{
	CSplitterWnd::OnParentNotify(message, lParam);
	
	// TODO: Add your message handler code here
	
}
