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

public:
    sqlite3x::sqlite3_connection basecon_;
    sqlite3x::sqlite3_connection mutablecon_;
};

#endif // __STUDIO_DATA_H__
