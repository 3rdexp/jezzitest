
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
                {L"user", L"��¼�û���"},
                {L"psw", L"����"},
                {L"ques", L"����������ʾ����"},
                {L"answer", L"�ش�"} };
        
            HPROPERTY hAppearance = AddItem( PropCreateCategory(_T("�ʻ���Ϣ"), 0) );
            for(int i=0; i<ARRAYSIZE(arr); ++i)
            {
                AddItem(PropCreateSimple(arr[i].value, u[arr[i].key].c_str()));
            }
        }

        {
            string_pair arr[] = {
                {L"name", L"����"},
                {L"web", L"��վ"},
                {L"desc", L"����"},
                {L"kw", L"�ؼ���"} };

            HPROPERTY hAppearance = AddItem( PropCreateCategory(_T("��¼��Ϣ"), 0) );
            for(int i=0; i<ARRAYSIZE(arr); ++i)
            {
                AddItem(PropCreateSimple(arr[i].value, u[arr[i].key].c_str()));
            }
        }

        {
            string_pair arr[] = {
                {L"contract", L"��ϵ��"},
                {L"email", L"Email"},
                {L"area-code", L"�绰����"},
                {L"phone", L"�绰����"},
                {L"ext", L"�ֻ�"},
                {L"fax", L"����"},
                {L"zip", L"�ʱ�"},
                {L"province", L"ʡ"},
                {L"city", L"����"},
                {L"address", L"��ַ"},
                {L"title", L"ְ��"},
                {L"mobile", L"�ƶ��绰"},
                {L"qq", L"QQ"} };

            HPROPERTY hAppearance = AddItem( PropCreateCategory(_T("��ϵ��Ϣ"), 0) );
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