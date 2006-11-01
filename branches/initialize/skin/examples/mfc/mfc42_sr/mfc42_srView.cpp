// mfc42_srView.cpp : implementation of the CMfc42_srView class
//

#include "stdafx.h"
#include "mfc42_sr.h"

#include "mfc42_srDoc.h"
#include "mfc42_srView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMfc42_srView

IMPLEMENT_DYNCREATE(CMfc42_srView, CView)

BEGIN_MESSAGE_MAP(CMfc42_srView, CView)
	//{{AFX_MSG_MAP(CMfc42_srView)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CView::OnFilePrintPreview)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMfc42_srView construction/destruction

CMfc42_srView::CMfc42_srView()
{
	// TODO: add construction code here

}

CMfc42_srView::~CMfc42_srView()
{
}

BOOL CMfc42_srView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CMfc42_srView drawing

void CMfc42_srView::OnDraw(CDC* pDC)
{
	CMfc42_srDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	// TODO: add draw code for native data here
}

/////////////////////////////////////////////////////////////////////////////
// CMfc42_srView printing

BOOL CMfc42_srView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CMfc42_srView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CMfc42_srView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}

/////////////////////////////////////////////////////////////////////////////
// CMfc42_srView diagnostics

#ifdef _DEBUG
void CMfc42_srView::AssertValid() const
{
	CView::AssertValid();
}

void CMfc42_srView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CMfc42_srDoc* CMfc42_srView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CMfc42_srDoc)));
	return (CMfc42_srDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMfc42_srView message handlers
