// PushLikeDemoDlg.h : header file
//

#if !defined(AFX_PUSHLIKEDEMODLG_H__715FC98F_5900_46F9_B243_3A706DBDD43C__INCLUDED_)
#define AFX_PUSHLIKEDEMODLG_H__715FC98F_5900_46F9_B243_3A706DBDD43C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CPushLikeDemoDlg dialog

class CPushLikeDemoDlg : public CDialog
{
// Construction
public:
	CPushLikeDemoDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CPushLikeDemoDlg)
	enum { IDD = IDD_PUSHLIKEDEMO_DIALOG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPushLikeDemoDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CPushLikeDemoDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnButton1();
	afx_msg void OnButton2();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PUSHLIKEDEMODLG_H__715FC98F_5900_46F9_B243_3A706DBDD43C__INCLUDED_)
