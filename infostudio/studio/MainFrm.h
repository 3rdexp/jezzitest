// MainFrm.h : interface of the CMainFrame class
//
/////////////////////////////////////////////////////////////////////////////

#pragma once

#include "childv.h"
#include "proplist/PropertyItem.h"

class ChildViewBase;
class BaseData;
class MutableData;
class SiteTask;
class EngineCrank;

#define IDC_TAB     ATL_IDW_CLIENT

#define ID_TOOL_FIRST       200
#define ID_TOOL_SAVE            ID_TOOL_FIRST

#define ID_TOOL_REGISTER_ALL        ID_TOOL_FIRST + 10
#define ID_TOOL_REGISTER_SEL        ID_TOOL_FIRST + 11
#define ID_TOOL_REGISTER_CUSTOM     ID_TOOL_FIRST + 12

#define ID_TOOL_PUBLISH_NEW         ID_TOOL_FIRST + 20
#define ID_TOOL_PUBLISH_DEL         ID_TOOL_FIRST + 21
#define ID_TOOL_PUBLISH_EDIT        ID_TOOL_FIRST + 22
#define ID_TOOL_PUBLISH_START       ID_TOOL_FIRST + 23
#define ID_TOOL_PUBLISH_STOP        ID_TOOL_FIRST + 24

#define ID_TOOL_LAST                ID_TOOL_PUBLISH_STOP + 1



// #ifndef FORWARD_RANGE
#define FORWARD_COMMAND_RANGE(idFirst, idLast, toWnd) \
    if(uMsg == WM_COMMAND && LOWORD(wParam) >= idFirst  && LOWORD(wParam) <= idLast) \
    { \
		bHandled = TRUE; \
        lResult = ::SendMessage(toWnd, uMsg, wParam, lParam); \
		if(bHandled) \
			return TRUE; \
	}


class CTransTab : public CWindowImpl<CTransTab, CTabCtrl>
{
public:
    DECLARE_WND_SUPERCLASS(_T("TransflateTab"), WC_TABCONTROL)
    BEGIN_MSG_MAP(CTransTab)
        FORWARD_NOTIFICATIONS()
    END_MSG_MAP()

    CTransTab()
    {
#if 0
        ATL::CWndClassInfo & wci = GetWndClassInfo();
        ATLASSERT(wci.m_atom == 0);

        WNDCLASSEX wcorg = {sizeof(WNDCLASSEX)};
        GetClassInfoEx(NULL, wci.m_lpszOrigName, &wcorg);
        wci.pWndProc = wcorg.lpfnWndProc;
        m_pfnSuperWindowProc = wcorg.lpfnWndProc;

        wci.m_atom = AtlWinModuleRegisterClassExW(&_AtlWinModule, &wci.m_wc);
        ATLASSERT(wci.m_atom);
#endif
    }
};


class CMainFrame : public CFrameWindowImpl<CMainFrame>, public CUpdateUI<CMainFrame>,
		public CMessageFilter, public CIdleHandler
        , public sigslot::has_slots<sigslot::multi_threaded_local>
{
public:
	DECLARE_FRAME_WND_CLASS(NULL, IDR_MAINFRAME)

    // UI stuff
    CCommandBarCtrl m_CmdBar;
    CToolBarCtrl m_MeshBar;

    CTransTab m_tab;

    ChildViewBase * current_;

    enum CV_TYPE { CV_USERINFO, CV_YELLOWPAGE, CV_PUBLISH };
    enum { MAX_CHILD = 3 };

    ChildViewBase * children_[MAX_CHILD];

    CMainFrame(EngineCrank & crank) : crank_(crank)
        , current_(0)
        , bd_(0), md_(0)
    {
        std::fill_n(children_, size_t(MAX_CHILD), (ChildViewBase*)0);
    }

	virtual BOOL PreTranslateMessage(MSG* pMsg)
	{
		if(CFrameWindowImpl<CMainFrame>::PreTranslateMessage(pMsg))
			return TRUE;

        if (current_)
	        return current_->PreTranslateMessage(pMsg);

        return FALSE;
	}

	virtual BOOL OnIdle()
	{
		UIUpdateToolBar();
		return FALSE;
	}

	BEGIN_UPDATE_UI_MAP(CMainFrame)
		UPDATE_ELEMENT(ID_VIEW_TOOLBAR, UPDUI_MENUPOPUP)
		UPDATE_ELEMENT(ID_VIEW_STATUS_BAR, UPDUI_MENUPOPUP)
	END_UPDATE_UI_MAP()

	BEGIN_MSG_MAP(CMainFrame)
        CHAIN_MSG_MAP(CUpdateUI<CMainFrame>)
        CHAIN_MSG_MAP(CFrameWindowImpl<CMainFrame>)

		COMMAND_ID_HANDLER(ID_APP_EXIT, OnFileExit)
		COMMAND_ID_HANDLER(ID_FILE_NEW, OnFileNew)
		COMMAND_ID_HANDLER(ID_VIEW_TOOLBAR, OnViewToolBar)
		COMMAND_ID_HANDLER(ID_VIEW_STATUS_BAR, OnViewStatusBar)
		COMMAND_ID_HANDLER(ID_APP_ABOUT, OnAppAbout)

        COMMAND_ID_HANDLER(ID_TOOL_SAVE, OnUserInfoSave)
        COMMAND_ID_HANDLER(ID_TOOL_REGISTER_ALL, OnRegisterAll)
        COMMAND_ID_HANDLER(ID_TOOL_REGISTER_SEL, OnRegisterSel)
        // COMMAND_ID_HANDLER(ID_TOOL_REGISTER_ALL, OnRegisterAll)

        // TODO: foward to subpage
        // COMMAND_ID_HANDLER(ID_TOOL_PUBLISH_NEW, OnPublishNew)
        FORWARD_COMMAND_RANGE(ID_TOOL_PUBLISH_NEW, ID_TOOL_PUBLISH_STOP, current_->GetHWND())
#if 0
        if(uMsg == WM_COMMAND && LOWORD(wParam) >= ID_TOOL_PUBLISH_NEW  && LOWORD(wParam) <= ID_TOOL_PUBLISH_STOP)
        {
		    bHandled = TRUE;
            lResult = ::SendMessage(current_->GetHWND(), uMsg, wParam, lParam);
		    if(bHandled)
			    return TRUE;
	    }
#endif

        MESSAGE_HANDLER(WM_CREATE, OnCreate)
        MESSAGE_HANDLER(WM_KEYUP, OnKeyUp)


        NOTIFY_HANDLER(IDC_TAB, TCN_SELCHANGE, OnTabSelChange)

        NOTIFY_CODE_HANDLER(PIN_ITEMCHANGED, OnUserInfoItemChanged)
        REFLECT_NOTIFICATIONS()
	END_MSG_MAP()

    void UpdateLayout(BOOL bResizeBars = TRUE)
    {
        CFrameWindowImpl<CMainFrame>::UpdateLayout(bResizeBars);

        UpdateChildView();
    }

    void UpdateChildView()
    {
        if (m_tab)
        {
            CRect rc;
            m_tab.GetClientRect(&rc);
            m_tab.AdjustRect(FALSE, &rc);

#if 0
            HDWP hdwp = BeginDeferWindowPos(2);
            DeferWindowPos(hdwp,
                current_->GetHWND(), HWND_TOP, rc.left, rc.top,
                rc.right - rc.left, rc.bottom - rc.top, 0
                );
            EndDeferWindowPos(hdwp);
#else
            CWindow w(current_->GetHWND());
            w.SetWindowPos(NULL, &rc, SWP_NOZORDER | SWP_NOACTIVATE);
#endif
        }
    }

    ChildViewBase * CreateChildView(CV_TYPE);
    ChildViewBase * ActiveChildView(CV_TYPE);

    void InitData(BaseData * bd, MutableData * md)
    {
        bd_ = bd;
        md_ = md;
    }

    // TaskNotify
    void StateChange(SiteTask * task, int state);
    void VerifyCode(SiteTask* task, const std::wstring & imgfn);

// Handler prototypes (uncomment arguments if needed):
//	LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
//	LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
//	LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)

private:
	LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
    LRESULT OnKeyUp(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);

    LRESULT OnTabSelChange(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/);

    LRESULT OnTabEraseBackground(int, WPARAM, LPARAM, BOOL&);

    // sub UserInfo
    LRESULT OnUserInfoItemChanged(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/);
    LRESULT OnUserInfoSave(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);

    // sub Yello Page
    LRESULT OnRegisterAll(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
    LRESULT OnRegisterSel(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);

    // sub Publish Page
    LRESULT OnPublishNew(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);


	LRESULT OnFileExit(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		PostMessage(WM_CLOSE);
		return 0;
	}

	LRESULT OnFileNew(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		// TODO: add code to initialize document
		return 0;
	}

	LRESULT OnViewToolBar(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		static BOOL bVisible = TRUE;	// initially visible
		bVisible = !bVisible;
		CReBarCtrl rebar = m_hWndToolBar;
		int nBandIndex = rebar.IdToIndex(ATL_IDW_BAND_FIRST + 1);	// toolbar is 2nd added band
		rebar.ShowBand(nBandIndex, bVisible);
		UISetCheck(ID_VIEW_TOOLBAR, bVisible);
		UpdateLayout();
		return 0;
	}

	LRESULT OnViewStatusBar(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		BOOL bVisible = !::IsWindowVisible(m_hWndStatusBar);
		::ShowWindow(m_hWndStatusBar, bVisible ? SW_SHOWNOACTIVATE : SW_HIDE);
		UISetCheck(ID_VIEW_STATUS_BAR, bVisible);
		UpdateLayout();
		return 0;
	}

	LRESULT OnAppAbout(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		// CAboutDlg dlg;
		// dlg.DoModal();
		return 0;
	}

    void InitViews();

    void InitCrank();
    void Register(std::vector<SiteInfo*> &);
    void Register(Site &);

    //////////////////////////////////////////////////////////////////////////
    // database stuff
    BaseData * bd_;
    MutableData * md_;
private:
    EngineCrank & crank_;
};
