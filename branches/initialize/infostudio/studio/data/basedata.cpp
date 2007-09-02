
#include "base/base.h"

#include "data/sqlite/sqlite3x.hpp"
using namespace sqlite3x;

#include "basedata.h"
#include <sstream>

//////////////////////////////////////////////////////////////////////////
//
bool LoadIndustry(sqlite3_connection & con, Industry & ic)
{
    // 一次把树全部读进来
    sqlite3_command cmd(con, L"SELECT industry.id, industry_relation.pid, name, ename FROM industry LEFT JOIN industry_relation ON (industry.id = industry_relation.id);");
    sqlite3_reader reader = cmd.executereader();

    while(reader.read()) {
        Industry ind;
        ind.id = reader.getint(0);
        int pid = reader.getint(1);
        ind.name = reader.getstring16(2);
        ind.ename = reader.getstring16(3);

        if (pid)
            ic.insert(pid, ind);
        else
            ic.insert(ind);
    }

    return true;
}

int SaveIndustryInter(sqlite3_connection & con, int pid, const Industry & ind)
{
    sqlite3_command cmd(con, L"insert into industry (name, ename) values (?,?);");
    cmd.bind(1, ind.name);
    cmd.bind(2, ind.ename);
    cmd.executenonquery();
    int id = con.executeint(L"select last_insert_rowid();");
    if (pid !=0)
    {
        sqlite3_command cmdr(con, L"insert into industry_relation values (?,?);");
        cmdr.bind(1, id);
        cmdr.bind(2, pid);
        cmdr.executenonquery();
    }

    for (Industry::list_type::const_iterator i=ind.children.begin(); i != ind.children.end(); ++i)
        SaveIndustryInter(con, id, *i);

    return id;
}

bool SaveIndustry(sqlite3_connection & con, const Industry & ind)
{
    sqlite3_transaction trans(con);

    for (Industry::list_type::const_iterator i=ind.children.begin(); i != ind.children.end(); ++i)
        SaveIndustryInter(con, 0, *i);

    trans.commit();
    return true;
}

void DumpIndustry(std::wstringstream &oss, const std::wstring & prefix, const Industry & ind)
{
    oss << prefix << ind.name << std::endl;
    for (Industry::list_type::const_iterator i=ind.children.begin(); i != ind.children.end(); ++i)
    {
        DumpIndustry(oss, prefix + L" ", *i);
    }
}

//////////////////////////////////////////////////////////////////////////
//
bool LoadSite(sqlite3_connection & con, std::map<int, SiteInfo> & site_map)
{
    return true;
}

bool BaseData::Init(sqlite3x::sqlite3_connection & con)
{
    try {
        bool f = LoadIndustry(con, indroot_);
        ASSERT(f);

        f = LoadSite(con, allsite_);
        ASSERT(f);
    }
    catch(database_error & e) {
        e.what();
        ASSERT(false);
        return false;
    }
    return true;
}
