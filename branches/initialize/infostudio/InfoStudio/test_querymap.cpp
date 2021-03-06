
#include "querymap.h"

void test_querymap()
{
    Dictionary site_dict;
    {
        VariableMap fv; // fields - value

        fv.insert(VariableMap::value_type(L"��ҵ", L"0"));
        fv.insert(VariableMap::value_type(L"ũҵ", L"1"));
        fv.insert(VariableMap::value_type(L"��ҵ", L"2"));
        site_dict.Insert(L"industry", fv);
        fv.clear();

        // fv.insert(VariableMap::value_type(L"������", L"101"));
        fv.insert(VariableMap::value_type(L"����", L"101"));
        fv.insert(VariableMap::value_type(L"�人", L"201"));
        site_dict.Insert(L"area", fv);
    }

    Dictionary domain_dict;
    {
        VariableMap fv;
        fv.insert(VariableMap::value_type(L"����", L"����*;������"));
        fv.insert(VariableMap::value_type(L"�人", L"�人��"));
        domain_dict.Insert(L"area", fv);
    }

    VariableMap userinfo;    
    userinfo.insert(VariableMap::value_type(L"name", L"�л����"));
    userinfo.insert(VariableMap::value_type(L"age", L"38"));
    userinfo.insert(VariableMap::value_type(L"area", L"����"));
    userinfo.insert(VariableMap::value_type(L"industry", L"��ҵ"));

    // name={name}&age={age}&address={area}&ind={industry}
    // name=�л����&age=38&address=101&ind=��ҵ

//  type    |    userinfo
// ---------|----------------------
// {name}   | userinfo[name]         |
// {area}   | userinfo[area] = ����  |  site_dict[area, ����] == null 
//    domain_dict.Find(area, ����) = ����*;������
//    for_each(����*;������)
//       site_dict.Find(����*)

    QueryMap qm(L"name={name}&age={age}&address={area}&ind={industry}");
    std::string s = qm.Apply(userinfo, site_dict, SC_ANSI);
    std::string s8 = qm.Apply(userinfo, site_dict, SC_UTF8);
}

