// MFCOutlookBarDoc.cpp : implementation of the CMFCOutlookBarDoc class
//

#include "stdafx.h"
#include "MFCOutlookBar.h"

#include "MFCOutlookBarDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMFCOutlookBarDoc

IMPLEMENT_DYNCREATE(CMFCOutlookBarDoc, CDocument)

BEGIN_MESSAGE_MAP(CMFCOutlookBarDoc, CDocument)
	//{{AFX_MSG_MAP(CMFCOutlookBarDoc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMFCOutlookBarDoc construction/destruction

CMFCOutlookBarDoc::CMFCOutlookBarDoc()
{
	// TODO: add one-time construction code here

}

CMFCOutlookBarDoc::~CMFCOutlookBarDoc()
{
}

BOOL CMFCOutlookBarDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}



/////////////////////////////////////////////////////////////////////////////
// CMFCOutlookBarDoc serialization

void CMFCOutlookBarDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}
}

/////////////////////////////////////////////////////////////////////////////
// CMFCOutlookBarDoc diagnostics

#ifdef _DEBUG
void CMFCOutlookBarDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CMFCOutlookBarDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMFCOutlookBarDoc commands
