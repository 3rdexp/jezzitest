
#ifndef __USERINFO_LIST_H__
#define __USERINFO_LIST_H__

#include <atlframe.h>
#include <atlctrls.h>

#include "childv.h"
#include "proplist/PropertyList.h"

class MutableData;

class SubUserInfo : public ChildViewT<CPropertyListCtrl>
{
public:
    SubUserInfo(MutableData *md) : md_(md) {}
    
    BEGIN_MSG_MAP(SubUserInfo)
        MESSAGE_HANDLER(WM_CREATE, OnCreate)
        NOTIFY_CODE_HANDLER(PIN_ITEMCHANGED, OnItemChanged)
        CHAIN_MSG_MAP(ChildViewT<CPropertyListCtrl>)
    END_MSG_MAP()

    LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
    LRESULT OnItemChanged(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/);

private:
    MutableData * md_;
};

#endif // __USERINFO_LIST_H__
