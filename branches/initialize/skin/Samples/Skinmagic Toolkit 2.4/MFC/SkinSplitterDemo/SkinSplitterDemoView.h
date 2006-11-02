// SkinSplitterDemoView.h : interface of the CSkinSplitterDemoView class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_SKINSPLITTERDEMOVIEW_H__0946711A_6EDE_47A2_940F_C2A25F9C0AD1__INCLUDED_)
#define AFX_SKINSPLITTERDEMOVIEW_H__0946711A_6EDE_47A2_940F_C2A25F9C0AD1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CSkinSplitterDemoView : public CListView
{
protected: // create from serialization only
	CSkinSplitterDemoView();
	DECLARE_DYNCREATE(CSkinSplitterDemoView)

// Attributes
public:
	CSkinSplitterDemoDoc* GetDocument();

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSkinSplitterDemoView)
	public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	protected:
	virtual void OnInitialUpdate(); // called first time after construct
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CSkinSplitterDemoView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CSkinSplitterDemoView)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	afx_msg void OnStyleChanged(int nStyleType, LPSTYLESTRUCT lpStyleStruct);
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in SkinSplitterDemoView.cpp
inline CSkinSplitterDemoDoc* CSkinSplitterDemoView::GetDocument()
   { return (CSkinSplitterDemoDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SKINSPLITTERDEMOVIEW_H__0946711A_6EDE_47A2_940F_C2A25F9C0AD1__INCLUDED_)
