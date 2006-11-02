// MDIDemoDoc.cpp : implementation of the CMDIDemoDoc class
//

#include "stdafx.h"
#include "MDIDemo.h"

#include "MDIDemoDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMDIDemoDoc

IMPLEMENT_DYNCREATE(CMDIDemoDoc, CDocument)

BEGIN_MESSAGE_MAP(CMDIDemoDoc, CDocument)
	//{{AFX_MSG_MAP(CMDIDemoDoc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMDIDemoDoc construction/destruction

CMDIDemoDoc::CMDIDemoDoc()
{
	// TODO: add one-time construction code here

}

CMDIDemoDoc::~CMDIDemoDoc()
{
}

BOOL CMDIDemoDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}



/////////////////////////////////////////////////////////////////////////////
// CMDIDemoDoc serialization

void CMDIDemoDoc::Serialize(CArchive& ar)
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
// CMDIDemoDoc diagnostics

#ifdef _DEBUG
void CMDIDemoDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CMDIDemoDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMDIDemoDoc commands
