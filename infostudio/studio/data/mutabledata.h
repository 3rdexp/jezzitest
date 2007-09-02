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
    void Init(sqlite3x::sqlite3_connection &);

private:
    typedef std::map<int, Site> site_map;
    site_map allsite_;
    UserInfo userinfo_;
};

#endif // __MUTABLE_DATA_H__
