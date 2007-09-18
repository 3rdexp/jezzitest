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

    const SiteInfo * GetSite(int sid) const
    {
        std::map<int, SiteInfo*>::const_iterator i = sidmap_.find(sid);
        if (i != sidmap_.end())
            return i->second;
        return 0;
    }

    std::vector<const SiteInfo*> FindSite(int cid) const
    {
        std::vector<const SiteInfo*> ret;
        siterel_type::const_iterator i = siterel_.find(cid);
        if (i != siterel_.end())
        {
            const sid_coll & col = i->second;
            for (sid_coll::const_iterator j = col.begin();
                j != col.end(); ++j)
            {
                const SiteInfo* p = GetSite(*j);
                ASSERT(p);
                ret.push_back(p);
            }
        }
//        ASSERT(!ret.empty());
        return ret;
    }

    std::vector<const SiteInfo*> AllSite() const
    {
        std::vector<const SiteInfo*> ret;
        
        for (std::list<SiteInfo>::const_iterator i = allsite_.begin();
            i != allsite_.end(); ++i)
            ret.push_back(&*i);

        return ret;
    }

    bool Init(sqlite3x::sqlite3_connection &);

private:
    bool LoadSite(sqlite3x::sqlite3_connection & con);
private:
    Industry indroot_;

    // Industry and SiteInfo
    typedef std::vector<int> sid_coll;
    typedef std::map<int, sid_coll> siterel_type; // industry.id => site collection
    siterel_type siterel_;

    // all SiteInfo
    std::list<SiteInfo> allsite_;
    std::map<int, SiteInfo*> sidmap_;
};

#endif // __BASEDATA_H__
