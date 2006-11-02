#if !defined(AFX_VIDEOWND_H__7B36F131_BB84_4A17_B8D5_8DA46ADE53D6__INCLUDED_)
#define AFX_VIDEOWND_H__7B36F131_BB84_4A17_B8D5_8DA46ADE53D6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// VideoWnd.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CVideoWnd dialog

class CVideoWnd : public CDialog
{
// Construction
public:
	CVideoWnd(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CVideoWnd)
	enum { IDD = IDD_VIDEO };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CVideoWnd)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CVideoWnd)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnClose();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VIDEOWND_H__7B36F131_BB84_4A17_B8D5_8DA46ADE53D6__INCLUDED_)
