// TransparentDemoDlg.h : header file
//

#if !defined(AFX_TRANSPARENTDEMODLG_H__FA117D76_C072_4A2D_BE1A_5374B6792C26__INCLUDED_)
#define AFX_TRANSPARENTDEMODLG_H__FA117D76_C072_4A2D_BE1A_5374B6792C26__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CTransparentDemoDlg dialog

class CTransparentDemoDlg : public CDialog
{
// Construction
public:
	CTransparentDemoDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CTransparentDemoDlg)
	enum { IDD = IDD_TRANSPARENTDEMO_DIALOG };
	CAnimateCtrl	m_ctrlAnimate;
	CTabCtrl	m_TabCtrl;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTransparentDemoDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	afx_msg void OnContextMenu(CWnd*, CPoint point);
	HIMAGELIST m_hMenuImage;
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CTransparentDemoDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnFileExit();
	afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TRANSPARENTDEMODLG_H__FA117D76_C072_4A2D_BE1A_5374B6792C26__INCLUDED_)
