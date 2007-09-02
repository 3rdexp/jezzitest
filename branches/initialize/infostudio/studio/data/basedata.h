#ifndef __BASEDATA_H__
#define __BASEDATA_H__


#include "engine/industry.h"
#include "engine/coreinfo.h"

namespace sqlite3x 
{
    class sqlite3_connection;
}

class BaseData
{
public:
    Industry & GetIndustry()
    {
        return indroot_;
    }

    const SiteInfo * FindSite(int sid) const
    {
        site_map::const_iterator i = allsite_.find(sid);
        if (i != allsite_.end())
            return &(i->second);
        return 0;
    }

    bool Init(sqlite3x::sqlite3_connection &);

private:
    Industry indroot_;
    typedef std::map<int, SiteInfo> site_map;
    site_map allsite_;
};

#endif // __BASEDATA_H__
