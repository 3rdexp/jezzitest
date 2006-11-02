#if !defined(AFX_SKINSPLITTER_H__C4B0EEA7_E15A_42E1_8176_C974C1A7C3F8__INCLUDED_)
#define AFX_SKINSPLITTER_H__C4B0EEA7_E15A_42E1_8176_C974C1A7C3F8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SkinSplitter.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSkinSplitter window

class CSkinSplitter : public CSplitterWnd
{
// Construction
public:
	CSkinSplitter();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSkinSplitter)
	//}}AFX_VIRTUAL
protected:
	HSKIN  m_hSkin;

protected:
	virtual void OnDrawSplitter(CDC* pDC, ESplitType nType, const CRect& rect);
	virtual void OnInvertTracker(const CRect& rect);

// Implementation
public:
	virtual ~CSkinSplitter();

	// Generated message map functions
protected:
	afx_msg LRESULT OnSkinDataChanged( WPARAM wParam, LPARAM  lParam);
	//{{AFX_MSG(CSkinSplitter)
	afx_msg void OnParentNotify(UINT message, LPARAM lParam);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SKINSPLITTER_H__C4B0EEA7_E15A_42E1_8176_C974C1A7C3F8__INCLUDED_)
