// MFCOutlookBarView.cpp : implementation of the CMFCOutlookBarView class
//

#include "stdafx.h"
#include "MFCOutlookBar.h"

#include "MFCOutlookBarDoc.h"
#include "MFCOutlookBarView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMFCOutlookBarView

IMPLEMENT_DYNCREATE(CMFCOutlookBarView, CView)

BEGIN_MESSAGE_MAP(CMFCOutlookBarView, CView)
	//{{AFX_MSG_MAP(CMFCOutlookBarView)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CView::OnFilePrintPreview)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMFCOutlookBarView construction/destruction

CMFCOutlookBarView::CMFCOutlookBarView()
{
	// TODO: add construction code here

}

CMFCOutlookBarView::~CMFCOutlookBarView()
{
}

BOOL CMFCOutlookBarView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CMFCOutlookBarView drawing

void CMFCOutlookBarView::OnDraw(CDC* pDC)
{
	CMFCOutlookBarDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	// TODO: add draw code for native data here
	CRect rc;
	GetClientRect(rc);
	pDC->DrawText(CString("TEST VIEW"), rc, DT_CENTER|DT_VCENTER|DT_SINGLELINE);

}

/////////////////////////////////////////////////////////////////////////////
// CMFCOutlookBarView printing

BOOL CMFCOutlookBarView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CMFCOutlookBarView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CMFCOutlookBarView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}

/////////////////////////////////////////////////////////////////////////////
// CMFCOutlookBarView diagnostics

#ifdef _DEBUG
void CMFCOutlookBarView::AssertValid() const
{
	CView::AssertValid();
}

void CMFCOutlookBarView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CMFCOutlookBarDoc* CMFCOutlookBarView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CMFCOutlookBarDoc)));
	return (CMFCOutlookBarDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMFCOutlookBarView message handlers
