// MFCControlsDlg.h : header file
//

#if !defined(AFX_MFCCONTROLSDLG_H__2E6AF410_778C_403A_82F1_9DF2330C6B68__INCLUDED_)
#define AFX_MFCCONTROLSDLG_H__2E6AF410_778C_403A_82F1_9DF2330C6B68__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "afxcmn.h"
/////////////////////////////////////////////////////////////////////////////
// CMFCControlsDlg dialog

class CMFCControlsDlg : public CDialog 
{
// Construction
public:
	CMFCControlsDlg(CWnd* pParent = NULL);	// standard constructor
// Dialog Data
	//{{AFX_DATA(CMFCControlsDlg)
	enum { IDD = IDD_MFCCONTROLS_DIALOG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

	CListBox		m_List;
	CScrollBar		m_HorzScroll;
	CScrollBar		m_VertScroll;
	CComboBox		m_Combo;
	CButton			m_PausePlayButton;
	CProgressCtrl	m_Progress;
	CTabCtrl		m_Tab;
	CString			m_Edit;

	BOOL m_bPlaying;
	BOOL m_bComboDropped;
	CSliderCtrl m_Slider1;
    CSliderCtrl m_Slider2;
    CSliderCtrl m_Slider3;
    CSliderCtrl m_Slider4;
    CSliderCtrl m_Slider5;
    CSliderCtrl m_Slider6;

	HANDLE _hScoreThread;
	HANDLE _hWait;

	void ScoreThreadProc ();
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMFCControlsDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CMFCControlsDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnPauseplay();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnSelchangeTab1(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSkinmagic();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MFCCONTROLSDLG_H__2E6AF410_778C_403A_82F1_9DF2330C6B68__INCLUDED_)
