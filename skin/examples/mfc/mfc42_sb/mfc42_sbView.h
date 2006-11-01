// mfc42_sbView.h : interface of the CMfc42_sbView class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_MFC42_SBVIEW_H__3A9A1BF6_6993_4956_BC94_479593F94F48__INCLUDED_)
#define AFX_MFC42_SBVIEW_H__3A9A1BF6_6993_4956_BC94_479593F94F48__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CMfc42_sbView : public CView
{
protected: // create from serialization only
	CMfc42_sbView();
	DECLARE_DYNCREATE(CMfc42_sbView)

// Attributes
public:
	CMfc42_sbDoc* GetDocument();

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMfc42_sbView)
	public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CMfc42_sbView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CMfc42_sbView)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in mfc42_sbView.cpp
inline CMfc42_sbDoc* CMfc42_sbView::GetDocument()
   { return (CMfc42_sbDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MFC42_SBVIEW_H__3A9A1BF6_6993_4956_BC94_479593F94F48__INCLUDED_)
