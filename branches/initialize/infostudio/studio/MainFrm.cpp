
#include "stdafx.h"

#include <algorithm>

#include <atlctrls.h>
#include <atldlgs.h>
#include <atlframe.h>
#include <atlctrlw.h>
#include <atlctrlx.h> // CWaitCursor

#include <boost/static_assert.hpp>

#include "base/logging.h"

#include "data/basedata.h"
#include "data/mutabledata.h"
#include "engine/infoengine.h"

#include "resource.h"

#include "ypage.h"
// #include "ulist.h"
#include "proplist/PropertyList.h"

#include "leftv.h"
#include "mainfrm.h"
#include "pubpage.h"

namespace {
    struct string_pair{
        wchar_t * key;
        wchar_t * value;
    };
}


LRESULT CStudioView::OnButton(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
    frame_->ActiveChildView((CMainFrame::CV_TYPE)(wID - IDC_BUTTON1));
    return 0;
}

LRESULT CMainFrame::OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
    HWND hWndCmdBar = m_CmdBar.Create(m_hWnd, rcDefault, NULL, ATL_SIMPLE_CMDBAR_PANE_STYLE);
    // attach menu
    m_CmdBar.AttachMenu(GetMenu());
    // load command bar images
    m_CmdBar.LoadImages(IDR_MAINFRAME);
    // remove old menu
    SetMenu(NULL);

    // BTNS_SHOWTEXT must be used with the TBSTYLE_LIST style and the TBSTYLE_EX_MIXEDBUTTONS extended style
    // HWND hWndToolBar = CreateSimpleToolBarCtrl(m_hWnd, IDR_MAINFRAME, FALSE
    //    , ATL_SIMPLE_TOOLBAR_PANE_STYLE | TBSTYLE_LIST);

    HWND hWndToolBar = ::CreateWindowEx(0, TOOLBARCLASSNAME, NULL, ATL_SIMPLE_TOOLBAR_PANE_STYLE | TBSTYLE_LIST
        , 0,0,100,38,
        m_hWnd, (HMENU)ATL_IDW_TOOLBAR, ATL::_AtlBaseModule.GetModuleInstance(), NULL);

    CreateSimpleReBar(ATL_SIMPLE_REBAR_NOBORDER_STYLE);
    AddSimpleReBarBand(hWndCmdBar);
    AddSimpleReBarBand(hWndToolBar, NULL, TRUE);

    CreateSimpleStatusBar();

    m_MeshBar = hWndToolBar;

    LPCTSTR string_pool[] = {
        L"保存", 
        L"立即注册", L"注册所选网站", 0, L"手工注册",
        L"信息发布", L"删除", L"编辑发布信息", 0, L"开始发布", L"停止发布"
    };

    TBBUTTON arr[] = {
        {0, ID_TOOL_SAVE,           TBSTATE_ENABLED | TBSTATE_HIDDEN, TBSTYLE_BUTTON | TBSTYLE_AUTOSIZE | BTNS_SHOWTEXT, {}, 0, 0},
        
        {0, ID_TOOL_REGISTER_ALL,   TBSTATE_ENABLED | TBSTATE_HIDDEN, TBSTYLE_BUTTON | TBSTYLE_AUTOSIZE | BTNS_SHOWTEXT, {}, 0, 0},
        {0, ID_TOOL_REGISTER_SEL,   TBSTATE_ENABLED | TBSTATE_HIDDEN, TBSTYLE_BUTTON | TBSTYLE_AUTOSIZE | BTNS_SHOWTEXT, {}, 0, 0},
        {0, 0,                      TBSTATE_ENABLED | TBSTATE_HIDDEN,  TBSTYLE_SEP, {}, 0, 0},
        {0, ID_TOOL_REGISTER_CUSTOM, TBSTATE_ENABLED | TBSTATE_HIDDEN, TBSTYLE_BUTTON | TBSTYLE_AUTOSIZE | BTNS_SHOWTEXT, {}, 0, 0},

        {0, ID_TOOL_PUBLISH_NEW,    TBSTATE_ENABLED | TBSTATE_HIDDEN, TBSTYLE_BUTTON | TBSTYLE_AUTOSIZE | BTNS_SHOWTEXT, {}, 0, 0},
        {0, ID_TOOL_PUBLISH_DEL,    TBSTATE_ENABLED | TBSTATE_HIDDEN, TBSTYLE_BUTTON | TBSTYLE_AUTOSIZE | BTNS_SHOWTEXT, {}, 0, 0},
        {0, ID_TOOL_PUBLISH_EDIT,   TBSTATE_ENABLED | TBSTATE_HIDDEN, TBSTYLE_BUTTON | TBSTYLE_AUTOSIZE | BTNS_SHOWTEXT, {}, 0, 0},
        {0, 0,                      TBSTATE_ENABLED | TBSTATE_HIDDEN, TBSTYLE_SEP, {}, 0, 0},
        {0, ID_TOOL_PUBLISH_START,  TBSTATE_ENABLED | TBSTATE_HIDDEN, TBSTYLE_BUTTON | TBSTYLE_AUTOSIZE | BTNS_SHOWTEXT, {}, 0, 0},
        {0, ID_TOOL_PUBLISH_STOP,   TBSTATE_ENABLED | TBSTATE_HIDDEN, TBSTYLE_BUTTON | TBSTYLE_AUTOSIZE | BTNS_SHOWTEXT, {}, 0, 0},
    };

    BOOST_STATIC_ASSERT(ARRAYSIZE(string_pool) == ARRAYSIZE(arr));

    for(int i=0; i<ARRAYSIZE(arr); ++i)
    {
        if(string_pool[i])
            arr[i].iString = m_MeshBar.AddStrings(string_pool[i]);
    }

    m_MeshBar.SetExtendedStyle(TBSTYLE_EX_MIXEDBUTTONS);
    m_MeshBar.SetButtonStructSize(); // sizeof TBBUTTON

    m_MeshBar.AddButtons(ARRAYSIZE(arr), arr);

    BOOL rt = m_MeshBar.SetBitmapSize(CSize(24, 24));
    ASSERT(rt);
    rt = m_MeshBar.SetButtonSize(CSize(50, 26));
    ASSERT(rt);

    // 保存
    // 立即注册, 注册所选网站, 手工注册, [搜索,类似msn过滤性质]
    // 信息发布[支持任务属性] ,  删除  编辑发布信息,  开始发布 停止发布

    // Bug: CContainedWindowT::RegisterWndSuperclass 会注册一个 
    // CS_HREDRAW | CS_VREDRAW 的窗口，导致闪烁

     WNDCLASSEX wc = { 0 };
     wc.cbSize = sizeof(WNDCLASSEX);
     ::GetClassInfoEx(_AtlBaseModule.GetModuleInstance(), _T("SysTabControl32"), &wc);
 
     wc.lpszClassName = _T("ATL:SysTabControl32");
     wc.lpfnWndProc = CContainedWindowT<CTabCtrl>::StartWindowProc;
     wc.hInstance = _Module.GetModuleInstance();
     wc.style = CS_DBLCLKS;
     wc.hbrBackground = (HBRUSH)COLOR_WINDOW;
     ATOM atom = AtlWinModuleRegisterClassEx(&_AtlWinModule, &wc);
     ATLASSERT(atom);

    m_hWndClient = m_tab.Create(m_hWnd, rcDefault, NULL, WS_CHILD 
        | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN, 0, IDC_TAB);

    if (m_hWndClient)
        m_tab.SetFont(AtlGetDefaultGuiFont());

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

    // 
    InitCrank();

    return 0;
}

LRESULT CMainFrame::OnKeyUp(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
    return 0;
}

void CMainFrame::InitViews()
{
    LPTSTR arr[] = {_T("[公司信息"), _T("黄页登录"), _T("信息发布")};

    TCITEM tie; 
    tie.mask = TCIF_TEXT; // TODO: TCIF_IMAGE
    tie.iImage = -1;

    for (int i=0; i<ARRAYSIZE(arr); ++i)
    {
        tie.pszText = arr[i];
        m_tab.InsertItem(i, &tie);
    }

    // TODO: load from config
    ActiveChildView(CV_USERINFO);
}

ChildViewBase * CMainFrame::CreateChildView(CV_TYPE type)
{
    CRect rc;
    m_tab.GetClientRect(&rc);
    m_tab.AdjustRect(FALSE, &rc);

    if (type == CV_USERINFO)
    {
        // SubUserInfo * pv = new SubUserInfo(md_);
        ChildViewT<CPropertyListCtrl> * pv = new ChildViewT<CPropertyListCtrl>();
        HWND h = pv->Create(m_tab, rc, NULL, WS_CHILD | WS_VISIBLE
            | WS_CLIPSIBLINGS | WS_CLIPCHILDREN, 0); // WS_EX_CLIENTEDGE);
        ATLASSERT(h);
        pv->SetExtendedListStyle(PLS_EX_CATEGORIZED);

        pv->SetColumnWidth(100);

        if (md_)
        {
            UserInfo & u = md_->GetUserInfo();
            {
                static string_pair arr[] = {
                    {L"user", L"登录用户名"},
                    {L"psw", L"密码"},
                    {L"ques", L"忘记密码提示问题"},
                    {L"answer", L"回答"} };

                    HPROPERTY hAppearance = pv->AddItem( PropCreateCategory(_T("帐户信息"), 0) );
                    for(int i=0; i<ARRAYSIZE(arr); ++i)
                    {
                        pv->AddItem(PropCreateSimple(arr[i].value, u[arr[i].key].c_str(), (LPARAM)arr[i].key));
                    }
            }

            {
                static string_pair arr[] = {
                    {L"name", L"名称"},
                    {L"web", L"网站"},
                    {L"desc", L"描述"},
                    {L"kw", L"关键字"} };

                    HPROPERTY hAppearance = pv->AddItem( PropCreateCategory(_T("登录信息"), 0) );
                    for(int i=0; i<ARRAYSIZE(arr); ++i)
                    {
                        pv->AddItem(PropCreateSimple(arr[i].value, u[arr[i].key].c_str(), (LPARAM)arr[i].key));
                    }
            }

            {
                static string_pair arr[] = {
                    {L"contract", L"联系人"},
                    {L"email", L"Email"},
                    {L"area-code", L"电话区号"},
                    {L"phone", L"电话号码"},
                    {L"ext", L"分机"},
                    {L"fax", L"传真"},
                    {L"zip", L"邮编"},
                    {L"province", L"省"},
                    {L"city", L"城市"},
                    {L"address", L"地址"},
                    {L"title", L"职务"},
                    {L"mobile", L"移动电话"},
                    {L"qq", L"QQ"} };

                    HPROPERTY hAppearance = pv->AddItem( PropCreateCategory(_T("联系信息"), 0) );
                    for(int i=0; i<ARRAYSIZE(arr); ++i)
                    {
                        pv->AddItem(PropCreateSimple(arr[i].value, u[arr[i].key].c_str(), (LPARAM)arr[i].key));
                    }
            }
        }

#if 0
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
#endif
        return pv;
    }
    else if(CV_YELLOWPAGE == type)
    {
        SubYellowPage * pv = new SubYellowPage(bd_);
        HWND h = pv->Create(m_tab, rc, NULL, WS_CHILD | WS_VISIBLE
            | WS_CLIPSIBLINGS | WS_CLIPCHILDREN, 0);
        ATLASSERT(h);
        return pv;
    }
    else if(CV_PUBLISH == type)
    {
        SubPublishPage * pv = new SubPublishPage(md_);
        HWND h = pv->Create(m_tab, rc, NULL, WS_CHILD | WS_VISIBLE
            | WS_CLIPSIBLINGS | WS_CLIPCHILDREN, 0);
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

            for (int i=ID_TOOL_FIRST; i<ID_TOOL_LAST; ++i)
                m_MeshBar.SetState(i, TBSTATE_HIDDEN);
        }

        current_ = children_[type];
        if (current_)
        {
            UpdateChildView();

            ::ShowWindow(current_->GetHWND(), SW_SHOW);

            // set special buttons in toolbar
            if (CV_USERINFO == type)
            {
                m_MeshBar.SetState(ID_TOOL_SAVE, TBSTATE_ENABLED);
            }
            else if(CV_YELLOWPAGE == type)
            {
                m_MeshBar.SetState(ID_TOOL_REGISTER_ALL, TBSTATE_ENABLED);
                m_MeshBar.SetState(ID_TOOL_REGISTER_SEL, TBSTATE_ENABLED);
                m_MeshBar.SetState(ID_TOOL_REGISTER_CUSTOM, TBSTATE_ENABLED);
            }
            else if(CV_PUBLISH == type)
            {
                m_MeshBar.SetState(ID_TOOL_PUBLISH_NEW, TBSTATE_ENABLED);
                m_MeshBar.SetState(ID_TOOL_PUBLISH_DEL, TBSTATE_ENABLED);
                m_MeshBar.SetState(ID_TOOL_PUBLISH_EDIT, TBSTATE_ENABLED);
                m_MeshBar.SetState(ID_TOOL_PUBLISH_START, TBSTATE_ENABLED);
                m_MeshBar.SetState(ID_TOOL_PUBLISH_STOP, TBSTATE_ENABLED);
            }
        }
    }
    return 0;
}

LRESULT CMainFrame::OnTabSelChange(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)
{
    int i = m_tab.GetCurSel();
    ActiveChildView((CV_TYPE)i);
    return 0;
}

LRESULT CMainFrame::OnTabEraseBackground(int, WPARAM wParam, LPARAM, BOOL&)
{
    // TODO:
    CDCHandle dc((HDC)wParam);
    CRect rcc;
    GetClientRect(&rcc);
    dc.FillRect(&rcc, COLOR_WINDOW);
    // ATLTRACE("erase once %d %d\n", rcc.right, rcc.bottom);
    return 1;
}

LRESULT CMainFrame::OnUserInfoItemChanged(int /*idCtrl*/, LPNMHDR pnmh, BOOL& /*bHandled*/)
{
    NMPROPERTYITEM * nm = (NMPROPERTYITEM*)pnmh;
    ASSERT(nm && nm->prop);
    if (nm && nm->prop) 
    {
        wchar_t * key = (wchar_t*)(nm->prop->GetItemData());
        ASSERT(key);
        CComVariant v;
        if (nm->prop->GetValue(&v))
        {
            UserInfo & u = md_->GetUserInfo();
            u[key] = v.bstrVal;
        }
        else
            ASSERT(false);
    }
    return 0;
}

LRESULT CMainFrame::OnUserInfoSave(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
    // TODO:
    // md_->SaveUserInfo
    bool f = md_->SaveUserInfo();
    ASSERT(f);
    return 0;
}

LRESULT CMainFrame::OnRegisterAll(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
    ASSERT(bd_);
    CWaitCursor wc;
    {
        std::vector<SiteInfo*> v = bd_->AllSite();
        Register(v);
    }
    return 0;
}

LRESULT CMainFrame::OnRegisterSel(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
    ASSERT(current_);
    SubYellowPage * yp = dynamic_cast<SubYellowPage*>(current_);
    ATLASSERT(yp);
    CWaitCursor wc;
    {
        std::vector<SiteInfo*> v;
        yp->GetSelectedSite(v);
        Register(v);
    }
    return 0;
}

LRESULT CMainFrame::OnPublishNew(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
    PublishInfoDialog<IDD_PUBLISH_INFO1> dlg;
    if (IDOK == dlg.DoModal())
    {
        Publish & pub = dlg.pub_;
        // TODO: Save 
    }
    return 0;
}

//////////////////////////////////////////////////////////////////////////

void CMainFrame::Register(Site & site)
{
    std::vector<Action> va = bd_->FindAction(site.sid, AT_REGISTER);
    if (!va.empty())
    {
        for(std::vector<Action>::const_iterator i=va.begin(); 
            i != va.end(); ++i)
            site.Add(CreateAction(*i));
        LOG(LS_VERBOSE) << "add to crank, site:" << site.sid << " action size:" << va.size();
        crank_.Add(site);
        LOG(LS_VERBOSE) << "add to crank, site:" << site.sid << " return.";
    }
    else
    {
        ATLTRACE("empty action site:%S\n", site.name.c_str());
    }
}

void CMainFrame::Register(std::vector<SiteInfo*> & vec)
{
    LOG(LS_VERBOSE) << "start register " << vec.size() << " sites";
    for(std::vector<SiteInfo*>::const_iterator i = vec.begin(); i != vec.end(); ++i)
    {
        const SiteInfo * si = *i;
        Site *site = md_->Add(si);
        Register(*site);
    }
}

void CMainFrame::InitCrank()
{
    ASSERT(md_);
    crank_.SigStateChange.connect(this, &CMainFrame::StateChange);
    crank_.SigVerifyCode.connect(this, &CMainFrame::VerifyCode);
}

void CMainFrame::StateChange(SiteTask * task, int state)
{
    LOG(LS_VERBOSE) << "site:" << task->GetSite().sid << " state:" << state;
}

void CMainFrame::VerifyCode(SiteTask* task, const std::wstring & imgfn)
{
    LOG(LS_VERBOSE) << "site:" << task->GetSite().sid << " want VerifyCode file:" << w2string(imgfn);
}