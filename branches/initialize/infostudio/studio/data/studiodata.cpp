#include "data/studiodata.h"
#include "base/logging.h"
#include "base/unicode.h"


/***********************************************************************


    ------------          ------------                     ------------
    | userinfo |          |   site   |                     |  action  |
    |----------|          |----------|                     |----------|
    |  key     |          | sid      |                     | aid      |
    |  value   |          | username |                     | type     | AT_UTILITY/AT_REGISTER/AT_PUBLISH/AT_PUBRESULT
    |__________|          | passwd   |                     | sid      | 
                          | time     |                     | paid     |
                          | laststate| 上次状态            | ......   | 
                          |__________|                     | ......   | 
                                                           |__________|



    ------------          -----------------
    | publish  |          | publish_once  |                ------------
    |----------|          |---------------|                |  result  |
    | pupbid   |          | poid          |                |----------|
    | title    |          | pubid         |                | rid      |
    | keywords |          | start         | time           | aid      |
    | content  |          | end           | time           | data     | 
    | expire   |          | name          |                |          |
    | frequency|          |               |                | time     | 
    | [create] |          |_______________|                | content  |
    | ......   |                                           |          | 
    | ......   |                                           |__________|
    | ......   |          ----------------     
    |__________|          | publish_site | 
                          |--------------| 
                          |  pubid       |              site_rel
                          |  sid         |              ind
                          |______________|              ind_rel

cached，一旦缓存一种数据，就需要提供对应的增删改方法：
userinfo

sid => site*



**************************************************************************/
using namespace sqlite3x;

bool LoadIndustry(sqlite3_connection & con, Industry & ic)
{
    // 一次把树全部读进来
#ifdef NDEBUG
    sqlite3_command cmd(con, L"SELECT ind.id, ind_rel.pid, name, ename FROM ind "
        L"LEFT JOIN ind_rel ON (ind.id = ind_rel.id) order by pid");
#else
    sqlite3_command cmd(con, L"SELECT ind.id, ind_rel.pid, name, ename FROM ind "
        L"LEFT JOIN ind_rel ON (ind.id = ind_rel.id) where ind.id<100 order by pid");
#endif
    sqlite3_reader reader = cmd.executereader();

    // ptimer pt, ptall;

    int pid_cur = -1;
    Industry * ind_cur = 0;

    while(reader.read()) {
        Industry ind;
        ind.id = reader.getint(0);

        int pid = reader.getint(1);
        ind.name = reader.getstring16(2);

        //        std::wostringstream woss;
        //        woss << ind.id;
        //        OutputDebugString(woss.str().c_str());

        ind.ename = reader.getstring16(3);

        if (ind_cur && ind_cur->id == pid)
        {
            ind_cur->insert(ind);
        }
        else
        {
            if (pid)
                ic.insert(pid, ind);
            else
                ic.insert(ind);
            ind_cur = ic.find(pid);
            ASSERT(ind_cur);
        }
    }

    // Dump(ptall, "all");

    return true;
}

bool StudioData::LoadSite(sqlite3_connection & con)
{
    // set<int>
    {
        sqlite3_command cmd(con, L"SELECT sid, name, homepage FROM site;");
        sqlite3_reader reader = cmd.executereader();

        while(reader.read()) {
            Site site;
            site.sid = reader.getint(0);
            site.name = reader.getstring16(1);
            site.homepage = reader.getstring16(2);
            
            allsite_.insert(site);
        }
    }

    // map<Industry.id, vector<sid> >
    {
        sqlite3_command cmd(con, L"SELECT sid, cid FROM site_rel;");
        sqlite3_reader reader = cmd.executereader();

        while(reader.read()) {
            int sid = reader.getint(0);
            int cid = reader.getint(1);

            sid_coll & coll = siterel_[cid];
            coll.push_back(sid);
        }
    }

    return true;
}

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

        // TODO: 设置 site.sid的起始值，保证和 base.db 不冲突

        // publish, result
        wchar_t * create_table_sqls[] = {
            // user
            L"CREATE TABLE IF NOT EXISTS userinfo(key TEXT PRIMARY KEY,value TEXT);",

            // site
            L"CREATE TABLE IF NOT EXISTS site(sid INTEGER PRIMARY KEY,username TEXT \
             ,passwd TEXT,time INTEGER,laststate INTEGER);",

             // action
            L"CREATE TABLE IF NOT EXISTS action(aid INTEGER PRIMARY KEY AUTOINCREMENT \
              ,type INTEGER,sid INTEGER,paid INTEGER,entry TEXT,url TEXT,method INTEGER \
              ,charset INTEGER,vars TEXT,content TEXT,restype INTEGER,referrer TEXT \
              ,checked TEXT,timeout INTEGER);",

            L"CREATE TABLE IF NOT EXISTS publish(pubid INTEGER PRIMARY KEY AUTOINCREMENT \
               ,title TEXT,keywords TEXT,content TEXT,expire INTEGER,frequency INTEGER,[create] INTEGER)",
            L"CREATE TABLE IF NOT EXISTS publish_once(poid INTEGER PRIMARY KEY AUTOINCREMENT \
                ,pubid INTEGER,start INTEGER,end INTEGER, name TEXT)",

            L"CREATE TABLE IF NOT EXISTS publish_site(pubid INTEGER,sid INTEGER)",

            L"CREATE TABLE IF NOT EXISTS result(rid INTEGER PRIMARY KEY AUTOINCREMENT,dataid INTEGER \
                ,sid INTEGER,atype INTEGER,time INTEGER,content TEXT)"
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

    bool ret = false;;

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

        ret = LoadIndustry(con_, indroot_);

        if (ret)
            ret = LoadSite(con_);
    } catch(database_error & e) {
        e;
        ASSERT(false && "read userinfo");
        LOG(LS_ERROR) << "read userinfo error" << e.what();
        return false;
    }

    return ret;
}

void StudioData::Close() {
    // save
    // close
}

std::vector<Site *> StudioData::FindSite(int indid) { 
    std::vector<Site *> ret;

    siterel_type::const_iterator i = siterel_.find(indid);
    if (i != siterel_.end())
    {
        const sid_coll & col = i->second;
        ret.reserve(col.size());

        for (sid_coll::const_iterator j = col.begin();
            j != col.end(); ++j)
        {
            Site* p = GetSite(*j);
            ASSERT(p);
            ret.push_back(p);
        }
    }
    return ret;
}

Site * StudioData::GetSite(int sid) {
    siteset::iterator i = allsite_.find(sid);
    if (i!= allsite_.end())
        return &*i;
    return 0;
}