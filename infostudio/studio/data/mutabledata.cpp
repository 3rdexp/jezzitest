
#include "base/base.h"

#include "data/sqlite/sqlite3x.hpp"
using namespace sqlite3x;

#include "mutabledata.h"
#include <sstream>


void MutableData::Init(sqlite3x::sqlite3_connection & con)
{
    // CREATE TABLE IF NOT EXISTS userinfo(key TEXT PRIMARY KEY, value TEXT)

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
        ASSERT(false && "read userinfo");
    }

    ready_ = CheckReady();
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

