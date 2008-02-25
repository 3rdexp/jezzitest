
#include <sstream>
#include "coreinfo.h"

bool UserInfo::ready() const
{
    const wchar_t* inside_key[] = {
        L"user", 
        L"psw",
        L"ques",
        L"answer",

        L"name",
        L"web",
        L"desc",
        L"kw",

        L"city",
        // L"web",
        L"contract", // ÁªÏµÈË
        L"email",
        L"area-code",
        L"phone",
        L"ext",
        L"fax",
        L"zip",
        L"province",
        L"city",
        L"address",
        L"title",
        L"mobile",
        L"qq"
    };

    for (int i=0; i<ARRAYSIZE(inside_key); ++i)
    {
        if (find(inside_key[i]) == end())
            return false;
    }

    return true;
}

void UserInfo::account_index(int index)
{
    ASSERT(index < name_.size());
    ASSERT(passwd_.size() == name_.size());
    insert(name_[index], passwd_[index]);
}

