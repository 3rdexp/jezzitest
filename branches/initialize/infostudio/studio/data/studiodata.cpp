#include "data/studiodata.h"
#include "base/logging.h"
#include "base/unicode.h"

bool StudioData::Open(const std::wstring & filename) {
    // 尽可能的把所有数据载入内存
    // Industry, Site

    using namespace sqlite3x;

    try {
        con_.open(filename.c_str());
    } catch(database_error & e) {
        // TODO: return error detail
        LOG(LS_ERROR) << "open " << w2string(filename) << e.what();
        return false;
    }

    try {
        // TODO: 快速的检查，确定是否执行如此多的 create
        // 或者不执行？

        // user, site, action
        con_.executenonquery(L"CREATE TABLE IF NOT EXISTS userinfo(key TEXT PRIMARY KEY, value TEXT)");
        con_.executenonquery(L"CREATE TABLE IF NOT EXISTS site(sid INTEGER PRIMARY KEY, username TEXT"
            L", passwd TEXT, time INTEGER, laststate INTEGER)");

        con_.executenonquery(L"CREATE TABLE IF NOT EXISTS action(aid INTEGER PRIMARY KEY AUTOINCREMENT"
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
            con_.executenonquery(create_table_sqls[i]);
        }

    } catch(database_error & e) {
        e;
        ASSERT(false && "init tables");
        LOG(LS_ERROR) << "init tables error" << e.what();
        return false;
    }

    // read all
    try {
        sqlite3_command cmd_read(con_, L"select key, value from userinfo");

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
        LOG(LS_ERROR) << "read userinfo error" << e.what();
        return false;
    }

    return true;
}

void StudioData::Close() {

}