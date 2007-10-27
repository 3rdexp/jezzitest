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
    // domain ==> dictionary
    typedef std::map<std::wstring, VariableMap> DictMap;
    DictMap dict_;
};

//////////////////////////////////////////////////////////////////////////
// for url query
//
// for a=b&c={name}&e*= translate
// usage:
// text: a=b&c={name}&d={e}

class QueryMap
{
public:
    QueryMap(const std::wstring & text) : text_(text) {}

    // TODO: Encoding
    std::string Expand(const VariableMap & vm, const Dictionary & dict
        , SiteCharset charset);

private:
    std::wstring text_;
};


#endif // __QUERYMAP_H__
