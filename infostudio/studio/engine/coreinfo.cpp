
#include <sstream>
#include "coreinfo.h"

int UserInfo::account_count() const
{
    if (ac_ == 0)
    {
        do {
            std::wostringstream oss;
            oss << L"user" << ac_;
            if (find(oss.str())  == end())
                break;
            ac_ ++;
        } while (true);
    }
    return ac_;
}