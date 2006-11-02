// MDIDemoView.cpp : implementation of the CMDIDemoView class
//

#include "stdafx.h"
#include "MDIDemo.h"

#include "MDIDemoDoc.h"
#include "MDIDemoView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMDIDemoView

IMPLEMENT_DYNCREATE(CMDIDemoView, CView)

BEGIN_MESSAGE_MAP(CMDIDemoView, CView)
	//{{AFX_MSG_MAP(CMDIDemoView)
	//}}AFX_MSG_MAP
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CView::OnFilePrintPreview)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMDIDemoView construction/destruction

CMDIDemoView::CMDIDemoView()
{
	// TODO: add construction code here

}

CMDIDemoView::~CMDIDemoView()
{
}

BOOL CMDIDemoView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CMDIDemoView drawing

void CMDIDemoView::OnDraw(CDC* pDC)
{
	CMDIDemoDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	// TODO: add draw code for native data here
}

/////////////////////////////////////////////////////////////////////////////
// CMDIDemoView printing

BOOL CMDIDemoView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CMDIDemoView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CMDIDemoView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}

/////////////////////////////////////////////////////////////////////////////
// CMDIDemoView diagnostics

#ifdef _DEBUG
void CMDIDemoView::AssertValid() const
{
	CView::AssertValid();
}

void CMDIDemoView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CMDIDemoDoc* CMDIDemoView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CMDIDemoDoc)));
	return (CMDIDemoDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMDIDemoView message handlers


