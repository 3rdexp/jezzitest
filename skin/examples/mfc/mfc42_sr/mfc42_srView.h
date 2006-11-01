// mfc42_srView.h : interface of the CMfc42_srView class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_MFC42_SRVIEW_H__3EC20027_B3A8_4345_A5F2_0BB7530526CB__INCLUDED_)
#define AFX_MFC42_SRVIEW_H__3EC20027_B3A8_4345_A5F2_0BB7530526CB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CMfc42_srView : public CView
{
protected: // create from serialization only
	CMfc42_srView();
	DECLARE_DYNCREATE(CMfc42_srView)

// Attributes
public:
	CMfc42_srDoc* GetDocument();

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMfc42_srView)
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
	virtual ~CMfc42_srView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CMfc42_srView)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in mfc42_srView.cpp
inline CMfc42_srDoc* CMfc42_srView::GetDocument()
   { return (CMfc42_srDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MFC42_SRVIEW_H__3EC20027_B3A8_4345_A5F2_0BB7530526CB__INCLUDED_)
