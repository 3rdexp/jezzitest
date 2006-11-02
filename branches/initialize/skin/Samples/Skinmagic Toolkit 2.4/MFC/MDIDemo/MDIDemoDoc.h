// MDIDemoDoc.h : interface of the CMDIDemoDoc class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_MDIDEMODOC_H__4449CB13_2340_4CDB_AEFA_A98EE672FC6F__INCLUDED_)
#define AFX_MDIDEMODOC_H__4449CB13_2340_4CDB_AEFA_A98EE672FC6F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CMDIDemoDoc : public CDocument
{
protected: // create from serialization only
	CMDIDemoDoc();
	DECLARE_DYNCREATE(CMDIDemoDoc)

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMDIDemoDoc)
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CMDIDemoDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CMDIDemoDoc)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MDIDEMODOC_H__4449CB13_2340_4CDB_AEFA_A98EE672FC6F__INCLUDED_)
