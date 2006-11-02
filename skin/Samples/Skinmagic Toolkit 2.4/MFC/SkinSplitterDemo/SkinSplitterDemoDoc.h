// SkinSplitterDemoDoc.h : interface of the CSkinSplitterDemoDoc class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_SKINSPLITTERDEMODOC_H__5985DC34_4EB5_4AD9_B670_CDD26A10F098__INCLUDED_)
#define AFX_SKINSPLITTERDEMODOC_H__5985DC34_4EB5_4AD9_B670_CDD26A10F098__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CSkinSplitterDemoDoc : public CDocument
{
protected: // create from serialization only
	CSkinSplitterDemoDoc();
	DECLARE_DYNCREATE(CSkinSplitterDemoDoc)

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSkinSplitterDemoDoc)
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CSkinSplitterDemoDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CSkinSplitterDemoDoc)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SKINSPLITTERDEMODOC_H__5985DC34_4EB5_4AD9_B670_CDD26A10F098__INCLUDED_)
