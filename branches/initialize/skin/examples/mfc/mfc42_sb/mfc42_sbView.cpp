// mfc42_sbView.cpp : implementation of the CMfc42_sbView class
//

#include "stdafx.h"
#include "mfc42_sb.h"

#include "mfc42_sbDoc.h"
#include "mfc42_sbView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMfc42_sbView

IMPLEMENT_DYNCREATE(CMfc42_sbView, CView)

BEGIN_MESSAGE_MAP(CMfc42_sbView, CView)
	//{{AFX_MSG_MAP(CMfc42_sbView)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CView::OnFilePrintPreview)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMfc42_sbView construction/destruction

CMfc42_sbView::CMfc42_sbView()
{
	// TODO: add construction code here

}

CMfc42_sbView::~CMfc42_sbView()
{
}

BOOL CMfc42_sbView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CMfc42_sbView drawing

void CMfc42_sbView::OnDraw(CDC* pDC)
{
	CMfc42_sbDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	// TODO: add draw code for native data here
}

/////////////////////////////////////////////////////////////////////////////
// CMfc42_sbView printing

BOOL CMfc42_sbView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CMfc42_sbView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CMfc42_sbView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}

/////////////////////////////////////////////////////////////////////////////
// CMfc42_sbView diagnostics

#ifdef _DEBUG
void CMfc42_sbView::AssertValid() const
{
	CView::AssertValid();
}

void CMfc42_sbView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CMfc42_sbDoc* CMfc42_sbView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CMfc42_sbDoc)));
	return (CMfc42_sbDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMfc42_sbView message handlers
