// MFCDialogDlg.cpp : implementation file
//

#include "stdafx.h"
#include "MFCDialog.h"
#include "MFCDialogDlg.h"
#include "..\\About\\AboutDlg.h"
#include "..\\About\\HyperLink.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMFCDialogDlg dialog

CMFCDialogDlg::CMFCDialogDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CMFCDialogDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CMFCDialogDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CMFCDialogDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMFCDialogDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CMFCDialogDlg, CDialog)
	//{{AFX_MSG_MAP(CMFCDialogDlg)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_PAGEDLG, OnPagedlg)
	ON_BN_CLICKED(IDC_PRINTERDLG, OnPrinterdlg)
	ON_BN_CLICKED(IDC_FONTDLG, OnFontdlg)
	ON_BN_CLICKED(IDC_COLORDLG, OnColordlg)
	ON_BN_CLICKED(IDC_OPENFILE, OnOpenfile)
	ON_BN_CLICKED(IDC_ABOUT, OnAbout)
	ON_BN_CLICKED(IDC_MSGBOX, OnMsgbox)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMFCDialogDlg message handlers

BOOL CMFCDialogDlg::OnInitDialog()
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

void CMFCDialogDlg::OnPaint() 
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
HCURSOR CMFCDialogDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CMFCDialogDlg::OnPagedlg() 
{
	// TODO: Add your control notification handler code here
   CPageSetupDialog dlg;
   dlg.m_psd.Flags |= PSD_SHOWHELP;
   dlg.DoModal();	
}

void CMFCDialogDlg::OnPrinterdlg() 
{
	// TODO: Add your control notification handler code here
   CPrintDialog dlg( FALSE );
   dlg.m_pd.Flags |= PD_SHOWHELP;
   dlg.DoModal();		
}

void CMFCDialogDlg::OnFontdlg() 
{
	CFontDialog dlg;
    dlg.m_cf.Flags |= CF_SHOWHELP;
    dlg.DoModal();	
}

void CMFCDialogDlg::OnColordlg() 
{
	CColorDialog dlg;    
    dlg.m_cc.Flags |= CC_SHOWHELP;
	if ( dlg.DoModal() == IDOK )
	{
		HINSTANCE _hLibSkinLoad = LoadLibrary("skin.dll");	
		ATLASSERT(_hLibSkinLoad);
		if (_hLibSkinLoad)
		{
			typedef LRESULT (WINAPI *ChangeCurrentSchemeColorT)(COLORREF clr);
			ChangeCurrentSchemeColorT pf = (ChangeCurrentSchemeColorT)GetProcAddress(_hLibSkinLoad, "ChangeCurrentSchemeColor");
			ASSERT(pf);
			if(pf)
			{
				pf( dlg.GetColor() );
			}
		}
	}
	
}

void CMFCDialogDlg::OnOpenfile() 
{
	CFileDialog dlg( TRUE );
    dlg.m_ofn.Flags |= OFN_SHOWHELP;
    dlg.DoModal();	
}

void CMFCDialogDlg::OnAbout() 
{
	// TODO: Add your control notification handler code here
	CAboutDlg dlg;
	dlg.DoModal();
}

void CMFCDialogDlg::OnMsgbox() 
{
	// TODO: Add your control notification handler code here
	CHyperLink Link;
	DWORD dwRet = AfxMessageBox( IDS_DEMOSTRING , MB_YESNO|MB_ICONINFORMATION );
	if( dwRet == IDYES )
	{
		Link.GotoURL( "http://www.appspeed.com", SW_SHOW);
	}
}
