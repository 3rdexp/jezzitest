// mfc42_srDoc.cpp : implementation of the CMfc42_srDoc class
//

#include "stdafx.h"
#include "mfc42_sr.h"

#include "mfc42_srDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMfc42_srDoc

IMPLEMENT_DYNCREATE(CMfc42_srDoc, CDocument)

BEGIN_MESSAGE_MAP(CMfc42_srDoc, CDocument)
	//{{AFX_MSG_MAP(CMfc42_srDoc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMfc42_srDoc construction/destruction

CMfc42_srDoc::CMfc42_srDoc()
{
	// TODO: add one-time construction code here

}

CMfc42_srDoc::~CMfc42_srDoc()
{
}

BOOL CMfc42_srDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}



/////////////////////////////////////////////////////////////////////////////
// CMfc42_srDoc serialization

void CMfc42_srDoc::Serialize(CArchive& ar)
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
// CMfc42_srDoc diagnostics

#ifdef _DEBUG
void CMfc42_srDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CMfc42_srDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMfc42_srDoc commands
