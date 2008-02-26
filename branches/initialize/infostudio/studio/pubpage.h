
#ifndef __PUBPAGE_H__
#define __PUBPAGE_H__

#include <atlctrls.h>
#include <atlsplit.h>

#include "childv.h"

#include "pubtree.h"
#include "publist.h"
#include "pubinfo.h"

// 
class SubPublishPage : public CSplitterWindowImpl<SubPublishPage, true>
    , public ChildViewBase
{
public:
    DECLARE_WND_CLASS_EX(NULL, CS_DBLCLKS, COLOR_WINDOW)

    virtual BOOL PreTranslateMessage(MSG* pMsg) 
    {
        // TODO:
        return FALSE;
    }
    virtual HWND GetHWND()
    {
        return m_hWnd;
    }

    SubPublishPage(StudioData * d) : data_(d) 
        , tree_(this, 1), list_(this, 2)
    {}

    typedef CSplitterWindowImpl<SubPublishPage, true> _baseClass;

    BEGIN_MSG_MAP(SubPublishPage)
        COMMAND_ID_HANDLER(ID_TOOL_PUBLISH_NEW, OnPublishNew)

        // ×¢ÒâÏûÏ¢Ë³Ðò
        CHAIN_MSG_MAP(_baseClass)
        MESSAGE_HANDLER(WM_CREATE, OnCreate)
    ALT_MSG_MAP(1)
        ATLTRACE("%4x %d\n", uMsg, dwMsgMapID);
        MESSAGE_HANDLER(WM_CONTEXTMENU, OnTreeContextMenu)
        MESSAGE_HANDLER(WM_RBUTTONDOWN, OnTreeContextMenu)
    ALT_MSG_MAP(2)
        MESSAGE_HANDLER(WM_CONTEXTMENU, OnTreeContextMenu)
        MESSAGE_HANDLER(WM_RBUTTONDOWN, OnTreeContextMenu)
    END_MSG_MAP()

private:
    LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);

    LRESULT OnPublishNew(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);

    // tree
    LRESULT OnTreeContextMenu(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
//     LRESULT (UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
//     {
//         return 0;
//     }

#if 1
    CContainedWindowT<CTreeViewCtrl> tree_;
    CContainedWindowT<CListViewCtrl> list_;
#else
    PublishTree tree_;
    PublishResultList list_;
#endif
    PublishInfoDialog<> info_;

    CHorSplitterWindow rsplit_;

    StudioData * data_;
};

#endif // __PUBPAGE_H__
