// SkinSplitterDemoView.cpp : implementation of the CSkinSplitterDemoView class
//

#include "stdafx.h"
#include "SkinSplitterDemo.h"

#include "SkinSplitterDemoDoc.h"
#include "SkinSplitterDemoView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSkinSplitterDemoView

IMPLEMENT_DYNCREATE(CSkinSplitterDemoView, CListView)

BEGIN_MESSAGE_MAP(CSkinSplitterDemoView, CListView)
	//{{AFX_MSG_MAP(CSkinSplitterDemoView)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, CListView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CListView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CListView::OnFilePrintPreview)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSkinSplitterDemoView construction/destruction

CSkinSplitterDemoView::CSkinSplitterDemoView()
{
	// TODO: add construction code here

}

CSkinSplitterDemoView::~CSkinSplitterDemoView()
{
}

BOOL CSkinSplitterDemoView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CListView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CSkinSplitterDemoView drawing

void CSkinSplitterDemoView::OnDraw(CDC* pDC)
{
	CSkinSplitterDemoDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	CListCtrl& refCtrl = GetListCtrl();
	refCtrl.InsertItem(0, "Item!");
	// TODO: add draw code for native data here
}

void CSkinSplitterDemoView::OnInitialUpdate()
{
	CListView::OnInitialUpdate();


	// TODO: You may populate your ListView with items by directly accessing
	//  its list control through a call to GetListCtrl().
}

/////////////////////////////////////////////////////////////////////////////
// CSkinSplitterDemoView printing

BOOL CSkinSplitterDemoView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CSkinSplitterDemoView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CSkinSplitterDemoView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}

/////////////////////////////////////////////////////////////////////////////
// CSkinSplitterDemoView diagnostics

#ifdef _DEBUG
void CSkinSplitterDemoView::AssertValid() const
{
	CListView::AssertValid();
}

void CSkinSplitterDemoView::Dump(CDumpContext& dc) const
{
	CListView::Dump(dc);
}

CSkinSplitterDemoDoc* CSkinSplitterDemoView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CSkinSplitterDemoDoc)));
	return (CSkinSplitterDemoDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CSkinSplitterDemoView message handlers
void CSkinSplitterDemoView::OnStyleChanged(int nStyleType, LPSTYLESTRUCT lpStyleStruct)
{
	//TODO: add code to react to the user changing the view style of your window
}
