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
    MutableData() : ready_(false) {}

    void Init(sqlite3x::sqlite3_connection &);

    bool CheckReady() const;
    UserInfo & GetUserInfo() { return userinfo_; }

    // access Site, Action

private:
    typedef std::map<int, Site> site_map;
    site_map allsite_;
    UserInfo userinfo_;
    bool ready_;
};

#endif // __MUTABLE_DATA_H__
