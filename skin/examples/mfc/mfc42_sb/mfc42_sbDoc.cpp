// mfc42_sbDoc.cpp : implementation of the CMfc42_sbDoc class
//

#include "stdafx.h"
#include "mfc42_sb.h"

#include "mfc42_sbDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMfc42_sbDoc

IMPLEMENT_DYNCREATE(CMfc42_sbDoc, CDocument)

BEGIN_MESSAGE_MAP(CMfc42_sbDoc, CDocument)
	//{{AFX_MSG_MAP(CMfc42_sbDoc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMfc42_sbDoc construction/destruction

CMfc42_sbDoc::CMfc42_sbDoc()
{
	// TODO: add one-time construction code here

}

CMfc42_sbDoc::~CMfc42_sbDoc()
{
}

BOOL CMfc42_sbDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}



/////////////////////////////////////////////////////////////////////////////
// CMfc42_sbDoc serialization

void CMfc42_sbDoc::Serialize(CArchive& ar)
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
// CMfc42_sbDoc diagnostics

#ifdef _DEBUG
void CMfc42_sbDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CMfc42_sbDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMfc42_sbDoc commands
