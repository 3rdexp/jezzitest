// ButtonsDlg.h : header file
//

#if !defined(AFX_BUTTONSDLG_H__A29F81C7_B995_4E82_94FB_4D38768FEFE0__INCLUDED_)
#define AFX_BUTTONSDLG_H__A29F81C7_B995_4E82_94FB_4D38768FEFE0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "easysize.h"
/////////////////////////////////////////////////////////////////////////////
// CButtonsDlg dialog

class CButtonsDlg : public CDialog
{
DECLARE_EASYSIZE
// Construction
public:
	CButtonsDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CButtonsDlg)
	enum { IDD = IDD_BUTTONS_DIALOG };
	CListBox	m_myListBox;
	CButton	m_Btn2;
	CButton	m_Btn1;
	CButton	m_Btn3;
	//}}AFX_DATA
	CBitmap bmp;
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CButtonsDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;
	CFont font;
	// Generated message map functions
	//{{AFX_MSG(CButtonsDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnButton1();
	afx_msg void OnButton3();
	afx_msg void OnButton2();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnButton4();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BUTTONSDLG_H__A29F81C7_B995_4E82_94FB_4D38768FEFE0__INCLUDED_)
