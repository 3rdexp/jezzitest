
#include <iostream>
#include <sstream>
#include <string>
#include <list>
#include <map>
#include <vector>
#include <cassert>

#include <boost/lexical_cast.hpp>

#include "base/unicode.h"

#include "sqlite/sqlite3x.hpp"

/*

CREATE TABLE IF NOT EXISTS userinfo(name TEXT PRIMARY KEY, value TEXT, cate INTEGER);

action
 aid IINTEGER PRIMARY KEY AUTOINCREMENT, type INTEGER, sid INTEGER, paid INTEGER DEFAULT 0, 
 url TEXT, method INTEGER DEFAULT 0, form_encoding TEXT, charset INTEGER, vars TEXT, restype INTEGER, 
 referrer TEXT DEFAULT NULL, checked TEXT DEFAULT NULL, timeout INTEGER DEFAULT 0


site
 sid INTEGER PRIMARY KEY AUTOINCREMENT, name TEXT, homepage TEXT DEFAULT NULL

relation industry site
 iid INTEGER, sid INTEGER

industry
 id INTEGER PRIMARY KEY AUTOINCREMENT, name TEXT, ename TEXT

industry_relation
 id INTEGER, pid INTEGER

site_industry_relation
 sid INTEGER, id INTEGER

// CREATE TABLE industry (id INTEGER PRIMARY KEY, name TEXT, ename TEXT, pid INTEGER)

sql-statement ::= 	CREATE [UNIQUE] INDEX [IF NOT EXISTS] [database-name .] index-name
ON table-name ( column-name [, column-name]* )
column-name ::= 	name [ COLLATE collation-name] [ ASC | DESC ]


conflict-clause ::= 	ON CONFLICT conflict-algorithm
conflict-algorithm ::= 	ROLLBACK | ABORT | FAIL | IGNORE | REPLACE
*/
using namespace sqlite3x;
using namespace std;

struct Industry
{
    Industry() 
        : parent(0), id(0) {}
    Industry(const wstring & n) 
        : parent(0), name(n), id(0) {}

    std::wstring name, ename;
    int id;

    Industry * parent;
    typedef std::list<Industry> list_type;
    list_type children;

    bool insert(int pid, const Industry & ind)
    {
        assert(pid != 0);
        Industry * parent_ind = find(pid);
        if (parent_ind)
            return parent_ind->insert(ind);

        return false;
    }

    bool insert(const Industry & ind)
    {
        assert(!name.empty());
        children.push_back(ind);
        list_type::iterator i = children.end();
        Industry & r = *(--i);
        r.parent = this;
        id_map.insert(id_map_type::value_type(ind.id, &r));
        return true;
    }

    Industry * find(int id) const
    {
        id_map_type::const_iterator i = id_map.find(id);
        if (i != id_map.end())
            return i->second;

        for (list_type::const_iterator j=children.begin(); j != children.end();
            ++j)
        {
            Industry * p = j->find(id);
            if (p)
                return p;
        }
        return 0;
    }

    // 模糊匹配多个行业？
    //    std::list<Industry*> find(const std::wstring & name) const
private:
    typedef std::map<int, Industry*> id_map_type;
    id_map_type id_map;;
};

Industry indroot(L"root");;

bool LoadIndustry(sqlite3_connection & con, Industry & ic)
{
    sqlite3_command cmd(con, "select id, name from industry order by id;");
    sqlite3_reader reader = cmd.executereader();

    list<Industry> all;
    while(reader.read()) {
        Industry ind;
        ind.id = reader.getint(0);
        ind.name = string2w(reader.getstring(1));
        
        all.push_back(ind);
    }

    for (list<Industry>::const_iterator i = all.begin(); i != all.end(); ++i)
    {
        if (i->id < 1000)
            indroot.insert(*i);
        else
        {
            std::string s = boost::lexical_cast<string>(i->id);
            s = s.substr(0, 2);
            indroot.insert(boost::lexical_cast<int>(s), *i);
        }
    }

    return true;
}

bool LoadIndustry2(sqlite3_connection & con, Industry & ic)
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


// base1.db => industry.db
void convert_industry()
{
    sqlite3_connection con("base1.db");

    con.executenonquery("CREATE TABLE IF NOT EXISTS industry(id INTEGER PRIMARY KEY AUTOINCREMENT, name TEXT, ename TEXT)");
    con.executenonquery("CREATE TABLE IF NOT EXISTS industry_relation(id INTEGER, pid INTEGER)");

    LoadIndustry(con, indroot);
    con.close();

    DumpIndustry(L"", indroot);
    OutputDebugString(oss.str().c_str());

    con.open(L"ind.db");
    con.executenonquery(L"CREATE TABLE IF NOT EXISTS industry(id INTEGER PRIMARY KEY AUTOINCREMENT, name TEXT, ename TEXT)");
    con.executenonquery(L"CREATE TABLE IF NOT EXISTS industry_relation(id INTEGER, pid INTEGER)");    
    SaveIndustry(con, indroot);
}

int main(int, char**)
{    
    try{
    sqlite3_connection con(L"ind.db");

    LoadIndustry2(con, indroot);
    con.close();

    DumpIndustry(L"", indroot);
    OutputDebugString(oss.str().c_str());    

    } catch(database_error & e) {
        cout << "database_error:" << e.what();
    }

#if 0
    {
        sqlite3_transaction trans(con);
        sqlite3_command cmd(con, "insert into userinfo values(?,?);");
        cmd.bind(2, "foobar", 6);

        for(int i = 0; i < 1000; ++i) {
            cmd.bind(1, i);
            cmd.executenonquery();
        }
        trans.commit();
    }

    {
        sqlite3_command cmd(con, "select * from userinfo;");
        sqlite3_reader reader = cmd.executereader();

        while(reader.read()) {
            cout << reader.getcolname(0) << ": " << reader.getstring(0) << endl;
        }
    }
#endif
    return 0;
}

