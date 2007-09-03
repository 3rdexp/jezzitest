#ifndef __BASEDATA_H__
#define __BASEDATA_H__


#include "engine/industry.h"
#include "engine/coreinfo.h"

#include <list>
#include <map>
#include <vector>

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
        all_site_type::const_iterator i = allsite_.find(sid);
        if (i != allsite_.end())
            return &(i->second);
        return 0;
    }

    bool Init(sqlite3x::sqlite3_connection &);

private:
    Industry indroot_;

    // Industry and SiteInfo
    typedef std::vector<int> sid_coll;
    typedef std::map<int, sid_coll> site_rel_type; // industry.id => site list
    site_rel_type siterel_;

    // all SiteInfo
    typedef std::list<SiteInfo> all_site_type;
    all_site_type allsite_;
    typedef std::map<int, SiteInfo*> sid_map_type;
    sid_map_type sidmap_;
};

#endif // __BASEDATA_H__
