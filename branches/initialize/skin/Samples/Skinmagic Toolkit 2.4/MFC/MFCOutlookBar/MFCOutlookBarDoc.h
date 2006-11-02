// MFCOutlookBarDoc.h : interface of the CMFCOutlookBarDoc class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_MFCOUTLOOKBARDOC_H__54F32923_A020_421C_9A15_A1A5911A67D7__INCLUDED_)
#define AFX_MFCOUTLOOKBARDOC_H__54F32923_A020_421C_9A15_A1A5911A67D7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CMFCOutlookBarDoc : public CDocument
{
protected: // create from serialization only
	CMFCOutlookBarDoc();
	DECLARE_DYNCREATE(CMFCOutlookBarDoc)

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMFCOutlookBarDoc)
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CMFCOutlookBarDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CMFCOutlookBarDoc)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MFCOUTLOOKBARDOC_H__54F32923_A020_421C_9A15_A1A5911A67D7__INCLUDED_)
