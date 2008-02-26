#ifndef __STUDIO_DATA_H__
#define __STUDIO_DATA_H__

#include <string>
#include <set>

#include "data/sqlite/sqlite3x.hpp"

#include "engine/industry.h"
#include "engine/coreinfo.h"

class StudioData {
public:
    ~StudioData() { Close(); }
    bool Open(const std::wstring & filename);
    void Close();

    sqlite3x::sqlite3_connection & GetConnection();

    Industry & GetIndustry() { return indroot_; }
    Site * GetSite(int sid);
    std::vector<Site *> FindSite(int indid) { return std::vector<Site *>(); }

    // action/site
    
private:
    struct orderby_sid : public std::binary_function<Site, Site, bool>
    {
        bool operator()(const Site & _Left, const Site & _Right) const
        {	// apply operator< to operands
            return (_Left.sid < _Right.sid);
        }
    };


    // 在内存中保存的项： site, industry
    std::set<Site, orderby_sid> allsite_;
    Industry indroot_;

public:
    UserInfo & GetUserInfo() { return userinfo_; }
    bool UserInfoReady() const { return userinfo_.ready(); }

    bool SaveUserInfo() { return true; }

private:
    UserInfo userinfo_;

    sqlite3x::sqlite3_connection con_;
};

#endif // __STUDIO_DATA_H__
