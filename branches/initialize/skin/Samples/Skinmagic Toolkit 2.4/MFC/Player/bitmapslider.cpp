// bitmapslider.cpp : implementation file
//

#include "stdafx.h"
#include "Player.h"
#include "bitmapslider.h"
#include "memorydc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CBitmapSlider

CBitmapSlider::CBitmapSlider()
{
	m_nValue = 0;
}

CBitmapSlider::~CBitmapSlider()
{
}


BEGIN_MESSAGE_MAP(CBitmapSlider, CStatic)
	//{{AFX_MSG_MAP(CBitmapSlider)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_WM_LBUTTONDOWN()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBitmapSlider message handlers

void CBitmapSlider::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	CRect rc;
	GetClientRect( rc );

	CMemDC mdc( &dc , rc ); 
	// TODO: Add your message handler code here
	HSKIN hSkin = OpenSkinData( _T("SkinBmpSlider") );
	if( hSkin )
	{
		DrawSkinImageRect( hSkin , _T("Slider") , mdc.GetSafeHdc() , rc );
		
		/////////////////////////////
		int h = rc.bottom - rc.top;
		h = (int)((h * m_nValue ) / 100.00 );
		rc.top = rc.bottom - h;
		dc.BitBlt( rc.left , rc.top , rc.right-rc.left , h , &mdc , rc.left , rc.top , SRCCOPY );
		CloseSkinData( hSkin );
	}	
	// Do not call CStatic::OnPaint() for painting messages
}

BOOL CBitmapSlider::OnEraseBkgnd(CDC* pDC) 
{
	// TODO: Add your message handler code here and/or call default
	CRect rc;
	GetClientRect( rc );
	HSKIN hSkin = OpenSkinData( _T("SkinBmpSlider") );
	if( hSkin )
	{
		DrawSkinImageRect( hSkin , _T("Background") , pDC->GetSafeHdc() , rc );
		CloseSkinData( hSkin );
		return TRUE;
	}
	return CStatic::OnEraseBkgnd(pDC);
}

void CBitmapSlider::SetValue(int nValue)
{
	if( nValue >= 100 ) nValue = 100;
	if( nValue <= 0 ) nValue = 0;
	m_nValue = nValue;
	Invalidate();
}

void CBitmapSlider::OnLButtonDown(UINT nFlags, CPoint point) 
{
	CRect rc;
	GetClientRect( rc );
	int h = rc.Height();
	int h1 = rc.bottom - point.y;
	m_nValue = ( (h1*100) / h );
	CStatic::OnLButtonDown(nFlags, point);
	Invalidate();
}

int CBitmapSlider::GetValue()
{
	return m_nValue;
}
