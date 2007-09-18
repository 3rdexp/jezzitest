
#include "stdafx.h"

#include <boost/bind.hpp>

#include "data/mutabledata.h"
#include "ulist.h"

namespace {
struct string_pair{
    wchar_t * key;
    wchar_t * value;
};
}


LRESULT SubUserInfo::OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
{
    DefWindowProc();
    if (md_)
    {
        UserInfo & u = md_->GetUserInfo();
        {
            string_pair arr[] = {
                {L"user", L"登录用户名"},
                {L"psw", L"密码"},
                {L"ques", L"忘记密码提示问题"},
                {L"answer", L"回答"} };
        
            HPROPERTY hAppearance = AddItem( PropCreateCategory(_T("帐户信息"), 0) );
            for(int i=0; i<ARRAYSIZE(arr); ++i)
            {
                AddItem(PropCreateSimple(arr[i].value, u[arr[i].key].c_str()));
            }
        }

        {
            string_pair arr[] = {
                {L"name", L"名称"},
                {L"web", L"网站"},
                {L"desc", L"描述"},
                {L"kw", L"关键字"} };

            HPROPERTY hAppearance = AddItem( PropCreateCategory(_T("登录信息"), 0) );
            for(int i=0; i<ARRAYSIZE(arr); ++i)
            {
                AddItem(PropCreateSimple(arr[i].value, u[arr[i].key].c_str()));
            }
        }

        {
            string_pair arr[] = {
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

            HPROPERTY hAppearance = AddItem( PropCreateCategory(_T("联系信息"), 0) );
            for(int i=0; i<ARRAYSIZE(arr); ++i)
            {
                AddItem(PropCreateSimple(arr[i].value, u[arr[i].key].c_str()));
            }
        }
    }
    return 0;
}

LRESULT SubUserInfo::OnItemChanged(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)
{
    return 0;
}