#ifndef __QUERYMAP_H__
#define __QUERYMAP_H__

#pragma once

#include <map>
#include <string>

#include "coreconst.h"

typedef std::map<std::wstring, std::wstring> VariableMap;

class Dictionary
{
public:
    const VariableMap & Find(const std::wstring & domain) const
    {
        // return dict_[domain];
        DictMap::const_iterator i = dict_.find(domain);
        if (i != dict_.end())
            return i->second;

        static VariableMap vm;
        return vm;
    }

    void Insert(const std::wstring & domain, const VariableMap & vm)
    {
        dict_[domain] = vm;
    }
private:
    // domain ==> dictionay
    typedef std::map<std::wstring, VariableMap> DictMap;
    DictMap dict_;
};

//////////////////////////////////////////////////////////////////////////
// for url query
//
// for a=b&c={name}&e*= translate
// usage:
// QueryMap vt(L"a=b&c={name}&e*=");
// std::string s = vt.Apply(UserInfo.vars, SC_ANSI, UrlQueryEscape());
// xxx.Post(s);

class QueryMap
{
public:
    QueryMap(const std::wstring & text) : text_(text) {}

    // TODO: template<class Encoding>
    std::string Apply(const Dictionary & dict, SiteCharset charset);

private:
    std::wstring text_;
};


#endif // __QUERYMAP_H__
