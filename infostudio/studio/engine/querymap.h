#ifndef __QUERYMAP_H__
#define __QUERYMAP_H__

#pragma once

#include <map>
#include <string>

#include "coreconst.h"

typedef std::multimap<std::wstring, std::wstring> VariableMap;

//////////////////////////////////////////////////////////////////////////
// domain   => map
// sex      => { male:1; female:2; }
// province => { 安徽:1001; 北京:1002; }

class Dictionary
{
public:
    const VariableMap & Find(const std::wstring & domain) const
    {
        DictMap::const_iterator i = dict_.find(domain);
        if (i != dict_.end())
            return i->second;

        static VariableMap empty;
        return empty;
    }

    void Insert(const std::wstring & domain, const VariableMap & vm)
    {
        dict_[domain] = vm;
    }
private:
    typedef std::map<std::wstring, VariableMap> DictMap;
    DictMap dict_;
};

//////////////////////////////////////////////////////////////////////////
// for url query
//
// for a=b&c={name}&e*= translate
// usage:
// QueryMap qm("a=b&c={name}&d={e}");
// qm.Expand

class QueryMap
{
public:
    QueryMap(const std::wstring & text) : text_(text) {}

    // vm: name => domain.key|value
    // dict: domain => [key, value]
    // TODO: Encoding
    std::string Expand(const VariableMap & vm, const Dictionary & dict
        , SiteCharset charset);

private:
    std::wstring text_;
    // TODO: 缓存 text_ split 结果
};


#endif // __QUERYMAP_H__
