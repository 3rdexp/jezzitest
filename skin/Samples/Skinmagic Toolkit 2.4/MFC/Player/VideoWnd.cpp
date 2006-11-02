// VideoWnd.cpp : implementation file
//

#include "stdafx.h"
#include "Player.h"
#include "VideoWnd.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CVideoWnd dialog


CVideoWnd::CVideoWnd(CWnd* pParent /*=NULL*/)
	: CDialog(CVideoWnd::IDD, pParent)
{
	//{{AFX_DATA_INIT(CVideoWnd)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	Create( CVideoWnd::IDD , pParent );
}


void CVideoWnd::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CVideoWnd)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CVideoWnd, CDialog)
	//{{AFX_MSG_MAP(CVideoWnd)
	ON_WM_CREATE()
	ON_WM_CLOSE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CVideoWnd message handlers

int CVideoWnd::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	// TODO: Add your specialized creation code here
	SetSingleDialogSkin(m_hWnd ,"VideoWnd" );

	return 0;
}

void CVideoWnd::OnClose() 
{
	// TODO: Add your message handler code here and/or call default
	ShowWindow( SW_HIDE );
	//DestroyWindow();
}
