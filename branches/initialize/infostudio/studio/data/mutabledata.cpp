
#include "base/base.h"

#include "data/sqlite/sqlite3x.hpp"
using namespace sqlite3x;

#include "mutabledata.h"
#include <sstream>

bool MutableData::Init(sqlite3x::sqlite3_connection & con)
{
    try {
    // user, site, action
    con.executenonquery(L"CREATE TABLE IF NOT EXISTS userinfo(key TEXT PRIMARY KEY, value TEXT)");
    con.executenonquery(L"CREATE TABLE IF NOT EXISTS site(sid INTEGER PRIMARY KEY, username TEXT"
        L", passwd TEXT, time INTEGER, laststate INTEGER)");

    con.executenonquery(L"CREATE TABLE IF NOT EXISTS action(aid INTEGER PRIMARY KEY AUTOINCREMENT"
        L", type INTEGER, sid INTEGER, paid INTEGER, entry TEXT, url TEXT, method INTEGER"
        L", charset INTEGER, vars TEXT, content TEXT, restype INTEGER, referrer TEXT, checked TEXT"
        L", timeout INTEGER);");

    // TODO: 设置 site.sid的起始值，保证和 base.db 不冲突

    // publish, result
    wchar_t * create_table_sqls[] = {
        // user
        L"CREATE TABLE IF NOT EXISTS userinfo(key TEXT PRIMARY KEY,value TEXT);",

        // site
        L"CREATE TABLE IF NOT EXISTS site(sid INTEGER PRIMARY KEY,username TEXT,passwd TEXT,time INTEGER,laststate INTEGER);",

        // action
        L"CREATE TABLE IF NOT EXISTS action(aid INTEGER PRIMARY KEY AUTOINCREMENT,type INTEGER,sid INTEGER \
            ,paid INTEGER,entry TEXT,url TEXT,method INTEGER,charset INTEGER,vars TEXT,content TEXT \
            ,restype INTEGER,referrer TEXT,checked TEXT,timeout INTEGER);",

        L"CREATE TABLE IF NOT EXISTS publish(pubid INTEGER PRIMARY KEY AUTOINCREMENT,title TEXT,keywords TEXT,content TEXT,expire INTEGER,frequency INTEGER,create INTEGER)",
        L"CREATE TABLE IF NOT EXISTS publish_once(poid INTEGER PRIMARY KEY AUTOINCREMENT,pubid INTEGER,start INTEGER,end INTEGER, name TEXT)",

        L"CREATE TABLE IF NOT EXISTS publish_site(pubid INTEGER,sid INTEGER)",

        L"CREATE TABLE IF NOT EXISTS result(rid INTEGER PRIMARY KEY AUTOINCREMENT,dataid INTEGER,sid INTEGER,atype INTEGER,time INTEGER,content TEXT)"
    };

    for(int i=0; i<ARRAYSIZE(create_table_sqls); ++i)
    {
        con.executenonquery(create_table_sqls[i]);
    }


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
            std::wstring key = rs.getstring16(0);
            std::wstring val = rs.getstring16(1);
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

bool UserInfo::ready() const
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
        L"contract", // 联系人
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
        if (find(inside_key[i]) == end())
            return false;
    }

    return true;
}

Site* MutableData::Add(const Site* si)
{
    ASSERT(si);
    Site *p = new Site;
    static_cast<Site&>(*p) = *si;
    sites_.insert(site_map::value_type(si->sid, p));
    return p;
}

Site* MutableData::Find(int sid) const
{
    site_map::const_iterator i = sites_.find(sid);
    if (i != sites_.end())
        return i->second;
    return 0;
}

std::vector<Publish> & MutableData::GetPublish()
{
    if (!pubread_)
    {
        try {
            sqlite3_command cmd_read(con_, L"select pid,title,keywords,content,expire,frequency from publish");

            sqlite3_reader rs = cmd_read.executereader();
            while (rs.read())
            {
                Publish pub;
                pub.pubid = rs.getint(0);
                pub.title = rs.getstring16(1);
                pub.keywords = rs.getstring16(2);
                pub.content = rs.getstring16(3);
                pub.expire = rs.getint(4);
                pub.frequency = rs.getint(5);                

                pubs_.push_back(pub);
            }

            pubread_ = true;
        } catch(database_error & e) {
            e;
            ASSERT(false && "read publish");
        }
    }

    return pubs_;
}