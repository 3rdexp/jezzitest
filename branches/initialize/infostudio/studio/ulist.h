
#ifndef __USERINFO_LIST_H__
#define __USERINFO_LIST_H__

#include <atlframe.h>
#include <atlctrls.h>

#include "childv.h"
#include "proplist/PropertyList.h"

class SubUserInfo : public ChildViewT<CPropertyListCtrl>
{
public:
    BEGIN_MSG_MAP(SubUserInfo)
        MESSAGE_HANDLER(WM_CREATE, OnCreate)
    END_MSG_MAP()

    LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
};

#endif // __USERINFO_LIST_H__
