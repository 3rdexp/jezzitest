
#include <vector>
#include <sstream>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/predicate.hpp>

#include "querymap.h"
#include "base/unicode.h"
#include "base/escape.h"

// std::wstring remove_bracket(const std::wstring & t) {}

std::string QueryMap::Expand(const VariableMap & vm, const Dictionary & dict
        , SiteCharset charset)
{
    using namespace std;

    // split to vector
    // find, replace...

    ostringstream ss;
    
    vector<wstring> v;
    boost::split(v, text_, boost::is_any_of(L"&"));
    
    vector<pair<wstring, wstring> > w;
    int i;
    for (i=0; i<v.size(); ++i)
    {
        vector<wstring> eq;
        boost::split(eq, v[i], boost::is_any_of(L"="));
        if (eq.size() == 2)
            w.push_back(make_pair(eq[0], eq[1]));
    }

    for (i=0; i<w.size(); ++i)
    {
        const wstring & key = w[i].first;
        wstring domain = w[i].second, val;
        
        // 1 find in vm, get value
        // 2 find in dict, get value's value
        if (boost::starts_with(domain, L"{")
            && boost::ends_with(domain, L"}") )
        {
            domain = domain.substr(1);
            domain.resize(domain.size() - 1);

            VariableMap::const_iterator i_vm = vm.find(domain);
            ASSERT(i_vm != vm.end());

            const VariableMap & vm_domain = dict.Find(domain);
            if (!vm_domain.empty())
            {
                VariableMap::const_iterator j_vm = vm_domain.find(i_vm->second);
                ASSERT(j_vm != vm_domain.end());
                if (j_vm != vm_domain.end())
                    val = j_vm->second;
            }
            else
                val = i_vm->second;
        }
        else
             val = domain;
    
        if (charset == SC_ANSI)
            ss << w2string(key) << "=" << UrlQueryEscape()(w2string(val)) << "&";
        else
            ss << string2utf8(key) << "=" << UrlQueryEscape()(string2utf8(val)) << "&";            
    }

    std::string ret = ss.str();

    // remove last &
    if (ret.size() > 1)
        ret.resize(ret.size() - 1);
    return ret;
}
