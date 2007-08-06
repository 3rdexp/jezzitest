
#include <vector>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/predicate.hpp>

#include "querymap.h"
#include "engine/unicode.h"
#include "engine/escape.h"

// std::wstring remove_bracket(const std::wstring & t) {}

std::string QueryMap::Apply(const VariableMap & dict, const VariableMap & vars
                       , SiteCharset charset)
{
    using namespace std;

    // split to vector
    // find value in dict

    wstring ret;
    
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
        wstring key = w[i].first, val = w[i].second;
        
        if (boost::starts_with(val, L"{")
            && boost::ends_with(val, L"}") )
        {
            val = val.substr(1);
            val.resize(val.size() - 1);

            VariableMap::const_iterator i_dict = dict.find(val);
            ASSERT(i_dict != dict.end());
            if (i_dict != dict.end())
            {
                VariableMap::const_iterator i_vars = vars.find(i_dict->second);
                ASSERT(i_vars != vars.end());
                ret += key.append(L"=").append(i_vars->second);
            }
        }
        else
        {
            ret += key.append(L"=").append(val);
        }

        ret += L"&";
    }

    return "";
}
