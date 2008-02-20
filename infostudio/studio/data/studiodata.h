#ifndef __STUDIO_DATA_H__
#define __STUDIO_DATA_H__

#include <string>
#include "data/sqlite/sqlite3x.hpp"

class StudioData
{
public:
    bool Open(const std::wstring & basefn, const std::wstring & mutablefn)
    {
        try {
            basecon_.open(basefn.c_str());
            mutablecon_.open(mutablefn.c_str());
        } catch(sqlite3x::database_error &) {
            return false;
        }
        return true;
    }
    void CloseAll()
    {
        basecon_.close();
        mutablecon_.close();
    }
    void CloseBase()
    {
        basecon_.close();
    }

private:
#if 0
    Industry indroot_;

    // Industry and Site
    typedef std::vector<int> sid_coll;
    typedef std::map<int, sid_coll> siterel_type; // industry.id => site collection
    siterel_type siterel_;

    // all Site
    std::list<Site> allsite_;
    std::map<int, Site*> sidmap_;
#endif

private:
    sqlite3x::sqlite3_connection basecon_;
    sqlite3x::sqlite3_connection mutablecon_;
};

#endif // __STUDIO_DATA_H__
