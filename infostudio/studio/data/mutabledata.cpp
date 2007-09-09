
#include "base/base.h"

#include "data/sqlite/sqlite3x.hpp"
using namespace sqlite3x;

#include "mutabledata.h"
#include <sstream>


void MutableData::Init(sqlite3x::sqlite3_connection & con)
{
    // read all
    // check ready_

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
}