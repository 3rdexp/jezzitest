// PlayerDlg.h : header file
//

#if !defined(AFX_PLAYERDLG_H__A0B0A0F1_F11C_47CF_9F75_79392FDA8C5C__INCLUDED_)
#define AFX_PLAYERDLG_H__A0B0A0F1_F11C_47CF_9F75_79392FDA8C5C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "VideoWnd.h"
#include "BitmapSlider.h"
/////////////////////////////////////////////////////////////////////////////
// CPlayerDlg dialog

class CPlayerDlg : public CDialog
{
// Construction
public:
	CPlayerDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CPlayerDlg)
	enum { IDD = IDD_PLAYER_DIALOG };
	CBitmapSlider	m_bmpSlider;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPlayerDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	CVideoWnd* m_pVideo;
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CPlayerDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnDestroy();
	afx_msg void OnDemo();
	afx_msg void OnVoiceUp();
	afx_msg void OnVoiceDown();
	afx_msg void OnClose();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PLAYERDLG_H__A0B0A0F1_F11C_47CF_9F75_79392FDA8C5C__INCLUDED_)
