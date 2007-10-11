
#include <windows.h>
#include "base/base.h"
#include "base/unicode.h"
#include "base/timer.hpp"

#include <list>
#include <string>
#include <fstream>
#include <iostream>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>

#include "data/sqlite/sqlite3x.hpp"
using namespace sqlite3x;

#include "basedata.h"
#include <sstream>

namespace {

struct indrel{
    int id, pid;
    std::wstring vid;
};

bool LoadIndrel(sqlite3_connection & con, std::list<indrel> & irlist)
{
    sqlite3_command cmd(con, L"SELECT * FROM ind_rel WHERE vid NOT NULL");
    sqlite3_reader reader = cmd.executereader();

    while(reader.read()) {
        indrel rel;
        rel.id = reader.getint(0);
        int pid = reader.getint(1);
        rel.vid = reader.getstring16(2);
        irlist.push_back(rel);
    }
    return true;
}

struct save_f
{
    save_f(sqlite3_connection & con) : con_(con) {}
    sqlite3_connection & con_;
    void operator()(const indrel & r)
    {
        using namespace std;
        using namespace boost;
        
        if (!r.vid.empty())
        {
            vector<wstring> v;
            split(v, r.vid, is_any_of(L"#"));
            ASSERT(!v.empty());

            sqlite3_command cmd_save(con_, L"insert into ind_rel (id, pid, vid) values (?,?,?);");
            sqlite3_command cmd_find(con_, L"select id from ind where vid = ?");

            for(vector<wstring>::const_iterator i=v.begin(); i != v.end(); ++i)
            {
                int vid = lexical_cast<int>(*i);

                cmd_find.bind(1, r.id);
                int ind_id = cmd_find.executeint();
                
                cmd_save.bind(1, ind_id);
                cmd_save.bind(2, vid);
                cmd_save.bind(3, r.id);
                cmd_save.executenonquery();
            }
        }
    }
};

bool SaveIndrel(sqlite3_connection & con, const std::list<indrel> & irlist)
{
    sqlite3_transaction trans(con);
    for_each(irlist.begin(), irlist.end(), save_f(con));
    trans.commit();
    return true;
}
}

//////////////////////////////////////////////////////////////////////////
//
bool LoadIndustry(sqlite3_connection & con, Industry & ic)
{
    // 一次把树全部读进来
    sqlite3_command cmd(con, L"SELECT ind.id, ind_rel.pid, name, ename FROM ind_rel LEFT JOIN ind ON (ind.id = ind_rel.id) order by pid limit 15000");
    sqlite3_reader reader = cmd.executereader();

    ptimer pt, ptall;

    int pid_cur = -1;
    Industry * ind_cur = 0;

    while(reader.read()) {
        Industry ind;
        ind.id = reader.getint(0);
        int pid = reader.getint(1);
        ind.name = reader.getstring16(2);
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

    Dump(ptall, "all");

    return true;
}

int SaveIndustryInter(sqlite3_connection & con, int pid, const Industry & ind)
{
    sqlite3_command cmd(con, L"insert into ind (name, ename) values (?,?);");
    cmd.bind(1, ind.name);
    cmd.bind(2, ind.ename);
    cmd.executenonquery();
    int id = con.executeint(L"select last_insert_rowid();");
    if (pid !=0)
    {
        sqlite3_command cmdr(con, L"insert into ind_rel values (?,?);");
        cmdr.bind(1, id);
        cmdr.bind(2, pid);
        cmdr.executenonquery();
    }

    for (Industry::children_type::const_iterator i=ind.children.begin(); i != ind.children.end(); ++i)
        SaveIndustryInter(con, id, i->second);

    return id;
}

bool SaveIndustry(sqlite3_connection & con, const Industry & ind)
{
    sqlite3_transaction trans(con);

    for (Industry::children_type::const_iterator i=ind.children.begin(); i != ind.children.end(); ++i)
        SaveIndustryInter(con, 0, i->second);

    trans.commit();
    return true;
}

void DumpIndustry(std::wostringstream &oss, const std::wstring & prefix, const Industry & ind)
{
    oss << prefix << ind.name << std::endl;
    for (Industry::children_type::const_iterator i=ind.children.begin(); i != ind.children.end(); ++i)
    {
        DumpIndustry(oss, prefix + prefix, i->second);
    }
}

//////////////////////////////////////////////////////////////////////////
// 牵扯数据结构太多，不作为类成员函数参数太多
bool BaseData::LoadSite(sqlite3_connection & con)
{
    // list<SiteInfo> map<sid, SiteInfo*>
    {
        sqlite3_command cmd(con, L"SELECT sid, name, homepage FROM site;");
        sqlite3_reader reader = cmd.executereader();

        while(reader.read()) {
            SiteInfo site;
            site.sid = reader.getint(0);
            site.name = reader.getstring16(1);
            site.homepage = reader.getstring16(2);

            allsite_.push_back(site);
            SiteInfo & ref = allsite_.back();
            sidmap_.insert(std::map<int, SiteInfo*>::value_type(site.sid, &ref));
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

bool BaseData::Init(sqlite3x::sqlite3_connection & con)
{
    try {
#if 0
        std::ifstream inf("data\\tools\\export_site_rel_1.txt");
        int sid;
        std::string cate;

        sqlite3_command cmd_find(con, "select id from ind where name=?");
        sqlite3_command cmd_save(con, "insert into site_rel values(?, ?)");

        sqlite3_transaction trans(con);

        std::string precate;
        int precid = 0;

        while (!inf.eof())
        {
            inf >> sid;
            inf >> cate;

            try {
                int cid = precid;
                if (precate != cate)
                {
                    cmd_find.bind(1, string2w(cate));
                    cid = cmd_find.executeint();

                    precate = cate;
                    precid = cid;
                }

                cmd_save.bind(1, sid);
                cmd_save.bind(2, cid);
                cmd_save.executenonquery();
            } catch(database_error & e) {
                std::cerr << sid << " " << cate << " err:" << e.what() << std::endl;
            }
        }
        trans.commit();

        ExitProcess(0);
#endif

        bool f = LoadIndustry(con, indroot_);
        ASSERT(f);

//        std::wostringstream oss;
//        DumpIndustry(oss, L"_", indroot_);
//        OutputDebugString(oss.str().c_str());

        f = LoadSite(con);
        ASSERT(f);
    }
    catch(database_error & e) {
        e.what();
        ASSERT(false);
        return false;
    }
    return true;
}


std::vector<ActionInfo> BaseData::FindAction(int sid, ActionType t)
{
    std::vector<ActionInfo> ret;
    // TODO: order by aid and return all dependent action
    sqlite3_command cmd(con_, L"SELECT aid, paid, url, method, charset, vars"
        L", restype, referrer, checked, timeout, type FROM action WHERE sid=? order by aid");
    cmd.bind(1, sid);
    sqlite3_reader reader = cmd.executereader();

    while(reader.read()) {
        ActionInfo a;
        a.aid = reader.getint(0);
        int paid = reader.getint(1);
        ASSERT(paid == 0);
        a.url = reader.getstring16(2);
        a.method = (HttpVerb)reader.getint(3);
        ASSERT(a.method == HV_GET || a.method == HV_POST);
        a.charset = (SiteCharset)reader.getint(4);
        ASSERT(a.charset == SC_ANSI || a.charset == SC_UTF8);
        a.vars = reader.getstring16(5);

        a.restype = (ActionResponseType)reader.getint(6);
        ASSERT(a.restype >= ART_VERIFY_IMAGE && a.restype <= ART_NONE);
        a.referrer = reader.getstring16(7);
        a.timeout = reader.getint(8);
        a.type = (ActionType)reader.getint(9);
        a.content_type = L"application/x-www-form-urlencoded"; // TODO:

        if (a.referrer.empty())
            a.referrer = a.url; // use action.entry ?

        ret.push_back(a);
    }
    return ret;
}

