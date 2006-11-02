// ShapeWindowDemoDlg.h : header file
//

#if !defined(AFX_SHAPEWINDOWDEMODLG_H__5B4D4EA7_9BC5_4C4E_8988_E5A0ADF0A181__INCLUDED_)
#define AFX_SHAPEWINDOWDEMODLG_H__5B4D4EA7_9BC5_4C4E_8988_E5A0ADF0A181__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CShapeWindowDemoDlg dialog

class CShapeWindowDemoDlg : public CDialog
{
// Construction
public:
	CShapeWindowDemoDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CShapeWindowDemoDlg)
	enum { IDD = IDD_SHAPEWINDOWDEMO_DIALOG };
	CMonthCalCtrl	m_MonthCalCtrl;
	CButton	m_Button2;
	CButton	m_Button1;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CShapeWindowDemoDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	afx_msg void OnContextMenu(CWnd*, CPoint point);
	HICON m_hIcon;
	DWORD m_nPos;

	// Generated message map functions
	//{{AFX_MSG(CShapeWindowDemoDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnButton1();
	afx_msg void OnButton2();
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnItem1();
	afx_msg void OnItem2();
	afx_msg void OnItem9();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SHAPEWINDOWDEMODLG_H__5B4D4EA7_9BC5_4C4E_8988_E5A0ADF0A181__INCLUDED_)
