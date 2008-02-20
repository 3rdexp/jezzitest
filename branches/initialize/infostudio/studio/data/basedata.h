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
    BaseData(sqlite3x::sqlite3_connection & con) : con_(con) {}
    Industry & GetIndustry()
    {
        return indroot_;
    }

    Site * GetSite(int sid)
    {
        std::map<int, Site*>::iterator i = sidmap_.find(sid);
        if (i != sidmap_.end())
            return i->second;
        return 0;
    }

    std::vector<Site*> FindSite(int cid)
    {
        std::vector<Site*> ret;
        siterel_type::iterator i = siterel_.find(cid);
        if (i != siterel_.end())
        {
            sid_coll & col = i->second;
            for (sid_coll::iterator j = col.begin();
                j != col.end(); ++j)
            {
                Site* p = GetSite(*j);
                ASSERT(p);
                ret.push_back(p);
            }
        }
//        ASSERT(!ret.empty());
        return ret;
    }

    std::vector<Site*> AllSite()
    {
        std::vector<Site*> ret;
        
        for (std::list<Site>::iterator i = allsite_.begin();
            i != allsite_.end(); ++i)
            ret.push_back(&*i);

        return ret;
    }

    std::vector<Action> FindAction(int sid, ActionType t);

    bool Init(sqlite3x::sqlite3_connection &);

private:
    bool LoadSite(sqlite3x::sqlite3_connection & con);
private:
    Industry indroot_;

    // Industry and Site
    typedef std::vector<int> sid_coll;
    typedef std::map<int, sid_coll> siterel_type; // industry.id => site collection
    siterel_type siterel_;

    // all Site
    std::list<Site> allsite_;
    std::map<int, Site*> sidmap_;

    sqlite3x::sqlite3_connection & con_;
};

#endif // __BASEDATA_H__
