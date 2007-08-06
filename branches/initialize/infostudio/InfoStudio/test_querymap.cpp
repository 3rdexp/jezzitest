
#include "querymap.h"

void test_querymap()
{
    Dictionary site_dict;
    {
        VariableMap fv; // fields - value

        fv.insert(VariableMap::value_type(L"工业", L"0"));
        fv.insert(VariableMap::value_type(L"农业", L"1"));
        fv.insert(VariableMap::value_type(L"商业", L"2"));
        site_dict.Insert(L"industry", fv);
        fv.clear();

        // fv.insert(VariableMap::value_type(L"仙桃市", L"101"));
        fv.insert(VariableMap::value_type(L"仙桃", L"101"));
        fv.insert(VariableMap::value_type(L"武汉", L"201"));
        site_dict.Insert(L"aread", fv);
    }

    Dictionary domain_dict;
    {
        VariableMap fv;
        fv.insert(VariableMap::value_type(L"仙桃", L"仙桃*;大仙桃"));
        fv.insert(VariableMap::value_type(L"武汉", L"武汉市"));
        domain_dict.Add(L"area", fv);
    }

    VariableMap userinfo;    
    userinfo.insert(VariableMap::value_type(L"name", L"中华民国"));
    userinfo.insert(VariableMap::value_type(L"age", L"38"));
    userinfo.insert(VariableMap::value_type(L"area", L"仙桃"));
    userinfo.insert(VariableMap::value_type(L"industry", L"工业"));

    // name={name}&age={age}&address={area}&ind={industry}
    // name=中华民国&age=38&address=101&ind=工业

//  type    |    userinfo
// ---------|----------------------
// {name}   | userinfo[name]         |
// {area}   | userinfo[area] = 仙桃  |  site_dict[area, 仙桃] == null 
//    domain_dict.Find(area, 仙桃) = 仙桃*;大仙桃
//    for_each(仙桃*;大仙桃)
//       site_dict.Find(仙桃*)

    QueryMap qm(L"name={name}&age={age}&address={area}&ind={industry}");
    qm.Apply(site_dict, SC_ANSI);
}