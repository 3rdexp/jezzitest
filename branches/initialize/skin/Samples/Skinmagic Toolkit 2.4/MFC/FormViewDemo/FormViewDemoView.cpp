// FormViewDemoView.cpp : implementation of the CFormViewDemoView class
//

#include "stdafx.h"
#include "FormViewDemo.h"

#include "FormViewDemoDoc.h"
#include "FormViewDemoView.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CFormViewDemoView

IMPLEMENT_DYNCREATE(CFormViewDemoView, CFormView)

BEGIN_MESSAGE_MAP(CFormViewDemoView, CFormView)
	//{{AFX_MSG_MAP(CFormViewDemoView)
	ON_WM_CREATE()
	ON_WM_LBUTTONUP()
	ON_BN_CLICKED(IDC_BUTTON2, OnButton2)
	//}}AFX_MSG_MAP
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, CFormView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CFormView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CFormView::OnFilePrintPreview)
	
	//ON_NOTIFY_EX(NM_COOLSB_CUSTOMDRAW, 0, OnScrollCustomDraw)
	//ON_NOTIFY_EX(NM_COOLSB_CUSTOMDRAW, 0,  OnScrollCustomDraw)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFormViewDemoView construction/destruction

CFormViewDemoView::CFormViewDemoView()
	: CFormView(CFormViewDemoView::IDD)
{
	//{{AFX_DATA_INIT(CFormViewDemoView)
	//}}AFX_DATA_INIT
	// TODO: add construction code here

}

CFormViewDemoView::~CFormViewDemoView()
{
}

void CFormViewDemoView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFormViewDemoView)
	DDX_Control(pDX, IDC_LIST1, m_ctrlList);
	//}}AFX_DATA_MAP
}

BOOL CFormViewDemoView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CFormView::PreCreateWindow(cs);
}

void CFormViewDemoView::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();
	GetParentFrame()->RecalcLayout();
	ResizeParentToFit();
}

/////////////////////////////////////////////////////////////////////////////
// CFormViewDemoView printing

BOOL CFormViewDemoView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CFormViewDemoView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CFormViewDemoView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}

void CFormViewDemoView::OnPrint(CDC* pDC, CPrintInfo* /*pInfo*/)
{
	// TODO: add customized printing code here
}

HRESULT CFormViewDemoView::OnScrollCustomDraw(UINT wParam, NMHDR* lParam, LRESULT* bRet)
{
	return 0;
	//using namespace Skin;
	//return HandleSkinScrollCustomDraw(wParam, lParam);
}

BOOL CFormViewDemoView::PreTranslateMessage(MSG* pMsg)
{
	if ( pMsg->message == WM_NOTIFY )
	{
		LPARAM  lParam = pMsg->lParam;
		if ( NM_COOLSB_CUSTOMDRAW == ((LPNMHDR)lParam)->code )
		{
			LRESULT bRet;
			OnScrollCustomDraw(	pMsg->wParam, (LPNMHDR)pMsg->lParam, &bRet);
		}
		
	}
	return CFormView::PreTranslateMessage(pMsg);
}
/////////////////////////////////////////////////////////////////////////////
// CFormViewDemoView diagnostics

#ifdef _DEBUG
void CFormViewDemoView::AssertValid() const
{
	CFormView::AssertValid();
}

void CFormViewDemoView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}

CFormViewDemoDoc* CFormViewDemoView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CFormViewDemoDoc)));
	return (CFormViewDemoDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CFormViewDemoView message handlers

int CFormViewDemoView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CFormView::OnCreate(lpCreateStruct) == -1)
		return -1;
	//using namespace Skin;
	//InstallSkinScrollBar( m_hWnd );
	// TODO: Add your specialized creation code here

	return 0;
}

void CFormViewDemoView::OnLButtonUp(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default

	CFormView::OnLButtonUp(nFlags, point);
}

void CFormViewDemoView::OnButton2() 
{
	// TODO: Add your control notification handler code here

}

LRESULT CFormViewDemoView::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	// TODO: Add your specialized code here and/or call the base class
	
	if ( message == WM_NOTIFY )
	{
		if ( NM_COOLSB_CUSTOMDRAW == ((LPNMHDR)lParam)->code )
		{
			LRESULT bRet;
			return OnScrollCustomDraw(	wParam, (LPNMHDR)lParam, &bRet);
		}

	}
	return CFormView::DefWindowProc(message, wParam, lParam);
}

BOOL CFormViewDemoView::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult) 
{
	// TODO: Add your specialized code here and/or call the base class
	/*
	if ( NM_COOLSB_CUSTOMDRAW == ((LPNMHDR)lParam)->code )
	{
		LRESULT bRet;
		return OnScrollCustomDraw(	wParam, (LPNMHDR)lParam, &bRet);
	}
	*/
	return CFormView::OnNotify(wParam, lParam, pResult);
}

