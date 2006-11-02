// MainFrm.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"
#include "MFCOutlookBar.h"

#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	//{{AFX_MSG_MAP(CMainFrame)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_OUTBAR_NOTIFY, OnOutbarNotify)
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // status line indicator
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};

/////////////////////////////////////////////////////////////////////////////
// CMainFrame construction/destruction

CMainFrame::CMainFrame()
{
	// TODO: add member initialization code here
	
}

CMainFrame::~CMainFrame()
{
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	SetWindowSkin( m_hWnd , "MainFrame" );

	if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
		| CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_wndToolBar.LoadToolBar(IDR_MAINFRAME))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}

	if (!m_wndStatusBar.Create(this) ||
		!m_wndStatusBar.SetIndicators(indicators,
		  sizeof(indicators)/sizeof(UINT)))
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}

	// TODO: Delete these three lines if you don't want the toolbar to
	//  be dockable
	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	EnableDocking(CBRS_ALIGN_ANY);
	DockControlBar(&m_wndToolBar);

	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	cs.style |= WS_CLIPCHILDREN;

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMainFrame message handlers


BOOL CMainFrame::OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext) 
{
	// TODO: Add your specialized code here and/or call the base class
	if (!wndSplitter.CreateStatic(this, 1, 2)) return false;

	RegisterSkinWindow(wndSplitter.m_hWnd , 0 );

	if (!wndSplitter.CreateView(0, 1, pContext->m_pNewViewClass, CSize(0,0), pContext)) return false;

	DWORD dwf = CGfxOutBarCtrl::fDragItems|CGfxOutBarCtrl::fEditGroups|CGfxOutBarCtrl::fEditItems|CGfxOutBarCtrl::fRemoveGroups|
				CGfxOutBarCtrl::fRemoveItems|CGfxOutBarCtrl::fAddGroups|CGfxOutBarCtrl::fAnimation;
//				|CGfxOutBarCtrl::fSelHighlight;


	wndBar.Create(WS_CHILD|WS_VISIBLE, CRect(0,0,0,0), &wndSplitter, wndSplitter.IdFromRowCol(0, 0), dwf);
	wndBar.SetOwner(this);

	imaLarge.Create(IDB_IMAGELIST, 32, 0, RGB(128,128,128));
	imaSmall.Create(IDB_SMALL_IMAGELIST, 16, 0, RGB(0,128,128));

	wndBar.SetImageList(&imaLarge, CGfxOutBarCtrl::fLargeIcon);
	wndBar.SetImageList(&imaSmall, CGfxOutBarCtrl::fSmallIcon);


	wndBar.SetAnimationTickCount(20);

	wndBar.SetAnimSelHighlight(200);

	wndBar.AddFolder("Folder 1", 0);
	wndBar.AddFolder("Folder 2", 1);
	wndBar.AddFolder("Folder 3", 2);
	wndBar.AddFolder("Folder 4", 3);
	wndBar.InsertItem(0, 0, "Item 1", 0, 0);
	wndBar.InsertItem(0, 1, "Item 2", 1, 0);
	wndBar.InsertItem(0, 2, "Item 3", 2, 0);
	wndBar.InsertItem(0, 3, "Item 4", 3, 0);
	wndBar.InsertItem(0, 4, "Item 5", 0, 0);
	wndBar.InsertItem(0, 5, "Item 6", 1, 0);
	wndBar.InsertItem(0, 6, "Item 7", 2, 0);
	wndBar.InsertItem(0, 7, "Item 8", 3, 0);
	wndBar.InsertItem(0, 8, "Item 9", 0, 0);
	wndBar.InsertItem(0, 9, "Item 10", 1, 0);
	wndBar.InsertItem(0, 10, "Item 11", 2, 0);
	wndBar.InsertItem(0, 11, "Item 12", 3, 0);
	wndBar.InsertItem(0, 12, "Item 13", 0, 0);
	wndBar.InsertItem(0, 13, "Item 14", 1, 0);
	wndBar.InsertItem(0, 14, "Item 15", 2, 0);
	wndBar.InsertItem(0, 15, "Item 16", 3, 0);

	wndBar.InsertItem(0, 1, "New Item 1", 3, 0);
	wndBar.InsertItem(0, 17, "Item 17", 3, 0);
	wndBar.InsertItem(2, 0, "Item 1", 0, 0);
	wndBar.InsertItem(2, 1, "Let's see what this Item will become actually 2", 1, 0);
	wndBar.InsertItem(2, 2, "Item 3", 2, 0);
	wndBar.InsertItem(2, 3, "Item 4", 3, 0);

	wndBar.SetSelFolder(0);

	CRect r;
	GetClientRect(&r);

	int w1 = r.Width()/5;
	int w2 = r.Width()/4;

	wndSplitter.SetColumnInfo( 0, w1, 0 );
	wndSplitter.SetColumnInfo( 1, w2, 0 );

	wndSplitter.RecalcLayout();

	return true;
	
	//return CFrameWnd::OnCreateClient(lpcs, pContext);
}

long CMainFrame::OnOutbarNotify(WPARAM wParam, LPARAM lParam)
{
	switch (wParam)
	{
		case NM_OB_ITEMCLICK:
		// cast the lParam to an integer to get the clicked item
			{
				int index = (int) lParam;
				CString cs, cs1;
				cs1 = wndBar.GetItemText(index);
				cs.Format("Clicked on %d - <%s>", (int)lParam, cs1);
				AfxMessageBox(cs);
			}
		return 0;

		case NM_OB_ONLABELENDEDIT:
		// cast the lParam to an OUTBAR_INFO * struct; it will contain info about the edited item
		// return 1 to do the change and 0 to cancel it
			{
				OUTBAR_INFO * pOI = (OUTBAR_INFO *) lParam;
				TRACE2("Editing item %d, new text:%s\n", pOI->index, pOI->cText);
			}
		return 1;

		case NM_OB_ONGROUPENDEDIT:
		// cast the lParam to an OUTBAR_INFO * struct; it will contain info about the edited folder
		// return 1 to do the change and 0 to cancel it
			{
				OUTBAR_INFO * pOI = (OUTBAR_INFO *) lParam;
				TRACE2("Editing folder %d, new text:%s\n", pOI->index, pOI->cText);
			}
		return 1;

		case NM_OB_DRAGITEM:
		// cast the lParam to an OUTBAR_INFO * struct; it will contain info about the dragged items
		// return 1 to do the change and 0 to cancel it
			{
				OUTBAR_INFO * pOI = (OUTBAR_INFO *) lParam;
				TRACE2("Drag item %d at position %d\n", pOI->iDragFrom, pOI->iDragTo);
			}
		return 1;
	}
	return 0;
}
