#ifndef __MUTABLE_DATA_H__
#define __MUTABLE_DATA_H__

#include "engine/coreinfo.h"

namespace sqlite3x 
{
    class sqlite3_connection;
}

class MutableData
{
public:
    MutableData(sqlite3x::sqlite3_connection &con) : ready_(false)
        , con_(con)
    {}

    bool Init(sqlite3x::sqlite3_connection &);

    // -----------------------------------------------
    // UserInfo
    bool CheckReady() const { return userinfo_.ready(); }
    UserInfo & GetUserInfo() { return userinfo_; }

    bool SaveUserInfo();

    // -----------------------------------------------
    // Site & Action ...
    // ����һ�� Site�����������ã�
    Site* Add(const SiteInfo* si);
    Site* Find(int sid) const;

    // ִ��ĳ��action�󣬱�����
    // д�����ݿ�
    Action* Add(const ActionInfo* ai, const std::wstring & result);

    // -----------------------------------------------
    // Publish Task

    // Execute

private:
    typedef std::map<int, Site*> site_map;
    site_map sites_;
    UserInfo userinfo_;
    bool ready_;
    sqlite3x::sqlite3_connection & con_;
};

#endif // __MUTABLE_DATA_H__
