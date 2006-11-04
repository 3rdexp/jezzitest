// mfc42_sbDoc.h : interface of the CMfc42_sbDoc class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_MFC42_SBDOC_H__B8BDB2A0_7652_423D_A69B_CCD137F9F0F2__INCLUDED_)
#define AFX_MFC42_SBDOC_H__B8BDB2A0_7652_423D_A69B_CCD137F9F0F2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CMfc42_sbDoc : public CDocument
{
protected: // create from serialization only
	CMfc42_sbDoc();
	DECLARE_DYNCREATE(CMfc42_sbDoc)

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMfc42_sbDoc)
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CMfc42_sbDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CMfc42_sbDoc)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MFC42_SBDOC_H__B8BDB2A0_7652_423D_A69B_CCD137F9F0F2__INCLUDED_)
