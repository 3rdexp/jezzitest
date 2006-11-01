// mfc42_srDoc.h : interface of the CMfc42_srDoc class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_MFC42_SRDOC_H__091F4A3E_6415_4B8F_A732_3E94A6C9630B__INCLUDED_)
#define AFX_MFC42_SRDOC_H__091F4A3E_6415_4B8F_A732_3E94A6C9630B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CMfc42_srDoc : public CDocument
{
protected: // create from serialization only
	CMfc42_srDoc();
	DECLARE_DYNCREATE(CMfc42_srDoc)

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMfc42_srDoc)
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CMfc42_srDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CMfc42_srDoc)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MFC42_SRDOC_H__091F4A3E_6415_4B8F_A732_3E94A6C9630B__INCLUDED_)
