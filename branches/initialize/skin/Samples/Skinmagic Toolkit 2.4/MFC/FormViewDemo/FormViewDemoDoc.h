// FormViewDemoDoc.h : interface of the CFormViewDemoDoc class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_FORMVIEWDEMODOC_H__3D3A1C4E_F05B_41BE_8D95_B5199CFD8D82__INCLUDED_)
#define AFX_FORMVIEWDEMODOC_H__3D3A1C4E_F05B_41BE_8D95_B5199CFD8D82__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CFormViewDemoDoc : public CDocument
{
protected: // create from serialization only
	CFormViewDemoDoc();
	DECLARE_DYNCREATE(CFormViewDemoDoc)

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFormViewDemoDoc)
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CFormViewDemoDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CFormViewDemoDoc)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FORMVIEWDEMODOC_H__3D3A1C4E_F05B_41BE_8D95_B5199CFD8D82__INCLUDED_)
