#if !defined(AFX_BITMAPSLIDER_H__496DDCD4_3057_4DF5_9924_830BBCA017E6__INCLUDED_)
#define AFX_BITMAPSLIDER_H__496DDCD4_3057_4DF5_9924_830BBCA017E6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// bitmapslider.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CBitmapSlider window

class CBitmapSlider : public CStatic
{
// Construction
public:
	CBitmapSlider();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBitmapSlider)
	//}}AFX_VIRTUAL

// Implementation
public:
	int GetValue();
	
	void SetValue( int nValue );
	virtual ~CBitmapSlider();

	// Generated message map functions
protected:
	int m_nValue;
	//{{AFX_MSG(CBitmapSlider)
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BITMAPSLIDER_H__496DDCD4_3057_4DF5_9924_830BBCA017E6__INCLUDED_)
