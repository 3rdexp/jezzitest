
#ifndef __PUBPAGE_H__
#define __PUBPAGE_H__

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

    BEGIN_MSG_MAP(PublishTree)
    END_MSG_MAP()

    PublishTree tree_;
    PublishList list_;
    PublishInfo info_;
};

#endif // __PUBPAGE_H__
