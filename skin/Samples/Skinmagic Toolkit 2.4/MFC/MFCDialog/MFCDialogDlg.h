// MFCDialogDlg.h : header file
//

#if !defined(AFX_MFCDIALOGDLG_H__1B5E5FE7_526F_448C_A5A5_82E63B4B8828__INCLUDED_)
#define AFX_MFCDIALOGDLG_H__1B5E5FE7_526F_448C_A5A5_82E63B4B8828__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CMFCDialogDlg dialog

class CMFCDialogDlg : public CDialog
{
// Construction
public:
	CMFCDialogDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CMFCDialogDlg)
	enum { IDD = IDD_MFCDIALOG_DIALOG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMFCDialogDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CMFCDialogDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnPagedlg();
	afx_msg void OnPrinterdlg();
	afx_msg void OnFontdlg();
	afx_msg void OnColordlg();
	afx_msg void OnOpenfile();
	afx_msg void OnAbout();
	afx_msg void OnMsgbox();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MFCDIALOGDLG_H__1B5E5FE7_526F_448C_A5A5_82E63B4B8828__INCLUDED_)
