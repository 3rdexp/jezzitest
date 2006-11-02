#ifndef __ABOUTDLG_H__
#define __ABOUTDLG_H__

#include "hyperlink.h"	// Added by ClassView
#define IDD_ABOUTBOX  100
#define IDC_HOMEPAGE  5000

class CAboutDlg : public CDialog
{
public:
	CHyperLink m_homepage;
	CAboutDlg();
	
	// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA
	
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL
	
	// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#endif
