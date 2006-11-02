// PushLikeDemoDlg.cpp : implementation file
//

#include "stdafx.h"
#include "PushLikeDemo.h"
#include "PushLikeDemoDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPushLikeDemoDlg dialog

CPushLikeDemoDlg::CPushLikeDemoDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CPushLikeDemoDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CPushLikeDemoDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CPushLikeDemoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPushLikeDemoDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CPushLikeDemoDlg, CDialog)
	//{{AFX_MSG_MAP(CPushLikeDemoDlg)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON1, OnButton1)
	ON_BN_CLICKED(IDC_BUTTON2, OnButton2)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPushLikeDemoDlg message handlers

BOOL CPushLikeDemoDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	// TODO: Add extra initialization here
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CPushLikeDemoDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CPushLikeDemoDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CPushLikeDemoDlg::OnButton1() 
{
	// TODO: Add your control notification handler code here
	CWnd* pWnd = GetDlgItem( IDC_CHECK3 );
	if( pWnd )
	{
		if( (pWnd->GetStyle()&BS_PUSHLIKE)!=BS_PUSHLIKE )
		{
			pWnd->ModifyStyle( 0 , BS_PUSHLIKE );
		}
		else
		{
			pWnd->ModifyStyle( BS_PUSHLIKE , 0 );
		}
		pWnd->Invalidate();
	}
}

void CPushLikeDemoDlg::OnButton2() 
{
	// TODO: Add your control notification handler code here
	CWnd* pWnd = GetDlgItem( IDC_CHECK3 );
	CButton* pRadio = (CButton*) GetDlgItem( IDC_RADIO5 );
	if( pWnd )
	{
		((CButton*)pWnd)->SetCheck( !((CButton*)pWnd)->GetCheck());
		pRadio->SetCheck( !pRadio->GetCheck() );
	}	
}
