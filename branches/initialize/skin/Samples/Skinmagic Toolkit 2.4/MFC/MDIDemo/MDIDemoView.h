// MDIDemoView.h : interface of the CMDIDemoView class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_MDIDEMOVIEW_H__8C8BD3B5_E312_4840_AF67_2013DAF13DF5__INCLUDED_)
#define AFX_MDIDEMOVIEW_H__8C8BD3B5_E312_4840_AF67_2013DAF13DF5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CMDIDemoView : public CView
{
protected: // create from serialization only
	CMDIDemoView();
	DECLARE_DYNCREATE(CMDIDemoView)

// Attributes
public:
	CMDIDemoDoc* GetDocument();

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMDIDemoView)
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
	virtual ~CMDIDemoView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CMDIDemoView)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in MDIDemoView.cpp
inline CMDIDemoDoc* CMDIDemoView::GetDocument()
   { return (CMDIDemoDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MDIDEMOVIEW_H__8C8BD3B5_E312_4840_AF67_2013DAF13DF5__INCLUDED_)
