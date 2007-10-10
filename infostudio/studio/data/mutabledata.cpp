
#include "base/base.h"

#include "data/sqlite/sqlite3x.hpp"
using namespace sqlite3x;

#include "mutabledata.h"
#include <sstream>


bool MutableData::Init(sqlite3x::sqlite3_connection & con)
{
    try {
    // user
    con.executenonquery(L"CREATE TABLE IF NOT EXISTS userinfo(key TEXT PRIMARY KEY, value TEXT)");
    con.executenonquery(L"CREATE TABLE IF NOT EXISTS site(sid INTEGER PRIMARY KEY, username TEXT, passwd TEXT"
        L", time INTEGER, laststate INTEGER)");

    // site

    // action result

    } catch(database_error & e) {
        e;
        ASSERT(false && "init table");
        return false;
    }

    // read all
    // check ready_

    try {
        sqlite3_command cmd_read(con, L"select key, value from userinfo");

        sqlite3_reader rs = cmd_read.executereader();
        while (rs.read())
        {
            wstring key = rs.getstring16(0);
            wstring val = rs.getstring16(1);
            userinfo_.insert(key, val);
        }
    } catch(database_error & e) {
        e;
        ASSERT(false && "read userinfo");
        return false;
    }

    ready_ = CheckReady();
    return true;
}

bool MutableData::SaveUserInfo()
{
    try {
        sqlite3x::sqlite3_transaction t(con_);
        sqlite3_command cmd_save(con_, L"REPLACE INTO userinfo(key, value) VALUES (?, ?);");

        for(VariableMap::const_iterator i = userinfo_.begin();
            i != userinfo_.end(); ++i)
        {
            cmd_save.bind(1, i->first);
            cmd_save.bind(2, i->second);

            cmd_save.executenonquery();
        }
        t.commit();
    } catch(database_error & e) {
        e;
        ASSERT(false && "save userinfo");
        return false;
    }
    return true;
}

bool MutableData::CheckReady() const
{
    const wchar_t* inside_key[] = {
        L"user", 
        L"psw",
        L"ques",
        L"answer",

        L"name",
        L"web",
        L"desc",
        L"kw",

        L"city",
        // L"web",
        L"contract", // ÁªÏµÈË
        L"email",
        L"area-code",
        L"phone",
        L"ext",
        L"fax",
        L"zip",
        L"province",
        L"city",
        L"address",
        L"title",
        L"mobile",
        L"qq"
    };
    
    for (int i=0; i<ARRAYSIZE(inside_key); ++i)
    {
        if (userinfo_.find(inside_key[i]) == userinfo_.end())
            return false;
    }

    return true;
}

Site* MutableData::Add(const SiteInfo* si)
{
    ASSERT(si);
    Site *p = new Site;
    static_cast<SiteInfo&>(*p) = *si;
    sites_.insert(site_map::value_type(si->sid, p));
    return p;
}

Site* MutableData::Find(int sid)
{
    site_map::iterator i = sites_.find(sid);
    if (i != sites_.end())
        return i->second;
    return 0;
}

Action* MutableData::Add(const ActionInfo* ai, const std::wstring & result)
{
    Action* p = new Action;
    static_cast<ActionInfo&>(*p) = *ai;
    p->result = result;
    // TODO: save to sqlite
    return p;
}
