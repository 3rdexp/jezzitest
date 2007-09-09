
#include "stdafx.h"

#include <atlframe.h>
#include <atlctrls.h>
#include <atldlgs.h>
#include <atlctrlw.h>
#include <atlsplit.h>

#include "proplist/PropertyList.h"
#include "data/basedata.h"
#include "data/mutabledata.h"

#include "resource.h"
#include "ypage.h"
#include "leftv.h"
#include "mainfrm.h"

LRESULT CStudioView::OnButton(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
    frame_->ActiveChildView((CMainFrame::CV_TYPE)(wID - IDC_BUTTON1));
    return 0;
}

LRESULT CMainFrame::OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
    // create command bar window
    HWND hWndCmdBar = m_CmdBar.Create(m_hWnd, rcDefault, NULL, ATL_SIMPLE_CMDBAR_PANE_STYLE);
    // attach menu
    m_CmdBar.AttachMenu(GetMenu());
    // load command bar images
    m_CmdBar.LoadImages(IDR_MAINFRAME);
    // remove old menu
    SetMenu(NULL);

    HWND hWndToolBar = CreateSimpleToolBarCtrl(m_hWnd, IDR_MAINFRAME, FALSE, ATL_SIMPLE_TOOLBAR_PANE_STYLE);

    CreateSimpleReBar(ATL_SIMPLE_REBAR_NOBORDER_STYLE);
    AddSimpleReBarBand(hWndCmdBar);
    AddSimpleReBarBand(hWndToolBar, NULL, TRUE);

    CreateSimpleStatusBar();

    m_hWndClient = m_wndSplitter.Create(m_hWnd, rcDefault, NULL, WS_CHILD 
        | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN);
    
    m_studio.Create(m_wndSplitter, rcDefault, WS_CHILD | WS_VISIBLE 
        | WS_CLIPSIBLINGS | WS_CLIPCHILDREN);

    m_wndSplitter.m_cxyMin = 170;

    m_wndSplitter.SetSplitterPane(SPLIT_PANE_LEFT, m_studio, false);

//    RECT rect;
//    GetClientRect(&rect);
    m_wndSplitter.SetSplitterPos(170);
    InitViews();

    UpdateLayout();
   

    UIAddToolBar(hWndToolBar);
    UISetCheck(ID_VIEW_TOOLBAR, 1);
    UISetCheck(ID_VIEW_STATUS_BAR, 1);

    // register object for message filtering and idle updates
    CMessageLoop* pLoop = _Module.GetMessageLoop();
    ATLASSERT(pLoop != NULL);
    pLoop->AddMessageFilter(this);
    pLoop->AddIdleHandler(this);

    return 0;
}

void CMainFrame::InitViews()
{
    ActiveChildView(CV_USERINFO);
}

template<class T>
class ChildViewT : public T
    , public ChildViewBase
{
public:
    virtual BOOL PreTranslateMessage(MSG* pMsg) 
    {
        return FALSE;
    }
    virtual HWND GetHWND()
    {
        return m_hWnd;
    }
};

ChildViewBase * CMainFrame::CreateChildView(CV_TYPE type)
{
    if (type == CV_USERINFO)
    {
        ChildViewT<CPropertyListCtrl> * pv = new ChildViewT<CPropertyListCtrl>();
        HWND h = pv->Create(m_wndSplitter, rcDefault, NULL, WS_CHILD | WS_VISIBLE
            | WS_CLIPSIBLINGS | WS_CLIPCHILDREN, WS_EX_CLIENTEDGE);
        ATLASSERT(h);
        pv->SetExtendedListStyle(PLS_EX_CATEGORIZED);

        pv->SetColumnWidth(100);

        HPROPERTY hAppearance = pv->AddItem( PropCreateCategory(_T("Appearance"), 1234) );
        HPROPERTY hName = pv->AddItem( PropCreateSimple(_T("Name"), _T("bjarke")) );
        pv->AddItem( PropCreateSimple(_T("X"), 123L) );
        pv->AddItem( PropCreateSimple(_T("Y"), 456L) );

        pv->AddItem( PropCreateCategory(_T("Behaviour")) );
        pv->AddItem( PropCreateSimple(_T("Enabled"), false) );
        pv->AddItem( PropCreateFileName(_T("Picture"), _T("C:\\Temp\\Test.bmp")) );
        pv->AddItem( PropCreateCheckButton(_T("Checked"), true) );
        pv->AddItem( PropCreateCheckButton(_T("Checked"), false) );

        LPCTSTR list[] = { _T("0 - Red"), _T("1 - Blue"), _T("2 - Green"), _T("3 - Yellow"), NULL };
        pv->AddItem( PropCreateList(_T("Colors"), list, 2) );

        // Testing control methods
        HPROPERTY hFind = pv->FindProperty(_T("Name")); 
        hFind = pv->FindProperty(1234); 
        pv->SetItemEnabled( hName, FALSE );
        pv->SetItemData(hName, 666);
        LPARAM lParam = pv->GetItemData(hName); 
        ATLASSERT(lParam == 666);

        TCHAR szText[32];
        pv->GetItemName(hName, szText, 32);
        // 

        CComVariant v(L"Form1");
        pv->SetItemValue(hName, &v);
        pv->CollapseItem(hAppearance);
        pv->ExpandItem(hAppearance);
        return pv;
    }
    else if(CV_YELLOWPAGE == type)
    {
        SubYellowPage * pv = new SubYellowPage(bd_);
        HWND h = pv->Create(m_wndSplitter, rcDefault, NULL, WS_CHILD | WS_VISIBLE
            | WS_CLIPSIBLINGS | WS_CLIPCHILDREN, WS_EX_CLIENTEDGE);
        ATLASSERT(h);
        return pv;
    }
    return 0;
}

ChildViewBase * CMainFrame::ActiveChildView(CV_TYPE type)
{
    ATLASSERT(type < MAX_CHILD);
    if (children_[type] == 0)
        children_[type] = CreateChildView(type);

    if (children_[type])
    {
        // hide before
        if (current_)
        {
            ::ShowWindow(current_->GetHWND(), SW_HIDE);
            current_ = 0;
        }

        current_ = children_[type];
        if (current_)
        {
            ::ShowWindow(current_->GetHWND(), SW_SHOW);
            m_wndSplitter.SetSplitterPane(SPLIT_PANE_RIGHT, current_->GetHWND(), true);
        }
    }
    return 0;
}
