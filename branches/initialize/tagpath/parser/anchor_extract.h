#ifndef __ANCHOR_EXTRACT_H__
#define __ANCHOR_EXTRACT_H__

#include <iostream>
#include <iterator>
#include <iomanip>

#include <boost/algorithm/string.hpp>

#include "tagsax.h"

namespace {
attr_array::const_iterator find_attribute(const attr_array& arr, const std::string& key_name)
{
    attr_array::const_iterator i = arr.begin();
    for (; i!=arr.end(); ++i)
    {
        if (boost::iequals(i->first, key_name))
            break;
    }
    return i;
}

template<typename T>    
std::ostream& filter_crlf(std::ostream& os_, T& t)
{
    for (typename T::const_iterator i = t.begin(); i != t.end(); ++i)
    {
        if (*i != '\r' && *i != '\n')
        { 
            os_ << *i;
        }
    }
    return os_;
}
}

struct anchor
{
    std::string title;
    std::string url;
};

template<class Container>
class anchor_extract
{
public:
    anchor_extract(Container & cont) : care_data_(false)
        , inserter_(cont)
    {
    }

    bool start_document()
    {
        return true;
    }
    bool start_element(const crange& name, const attr_array& arr)
    {
        if (!name.empty() && boost::iequals(name, "A"))
        {
            care_data_ = true;

            // read href
            attr_array::const_iterator i = find_attribute(arr, "HREF");

            if (i != arr.end())
            {
                current_anchor_.url = i->second.to_string();
                // filter_crlf(os_, i->second);
                // os_ << '\n';
            }
#if 0
            else
            {
                i = find_attribute(arr, "NAME");

                assert((arr.size() && i != arr.end()) || !arr.size());
                for (attr_array::const_iterator i=arr.begin(); i!=arr.end(); ++i)
                {
                    os_ << i->first << "=" << i->second << "\n";
                }
            }
#endif
        }
        return true;
    }
    bool characters(const crange& text)
    {
        if (care_data_)
        {
            current_anchor_.title = text.to_string();
            // os_ << "  ";
            // filter_crlf(os_, text) << '\n';
        }
        return true;
    }
    bool end_element(const crange& name)
    {
        if (care_data_)
        {
            care_data_ = false;
            *inserter_ = current_anchor_;
        }
        return true;
    }
    bool entities(const crange& text)
    {
        return true;
    }
    bool script(const crange& text, const attr_array&)
    {
        return true;
    }
    bool end_document()
    {
        return true;
    }

    std::back_insert_iterator<Container> inserter_;
    anchor current_anchor_;
    bool care_data_;
};

#endif // __ANCHOR_EXTRACT_H__
