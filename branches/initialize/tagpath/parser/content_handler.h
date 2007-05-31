#ifndef __K_PAGE_CONTENT_H__
#define __K_PAGE_CONTENT_H__

#include <boost/algorithm/string.hpp>
#include <iostream>


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


// a { href / name, title }


class page_content
{
public:
    page_content(std::ostream & os = std::cout) : os_(os), care_data_(false), is_title_(false)
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

            os_ << "a {";

            // read href
            attr_array::const_iterator i = find_attribute(arr, "HREF");

            if (i != arr.end())
            {
                filter_crlf(os_, i->second);
                os_ << ", ";
            }
            else
            {
                i = find_attribute(arr, "NAME");

//                assert((arr.size() && i != arr.end()) || !arr.size());
//                for (attr_array::const_iterator i=arr.begin(); i!=arr.end(); ++i)
//                {
//                    std::cerr << i->first << "=" << i->second << "\n";
//                }
            }
        }

        if (!name.empty() && boost::iequals(name, "TITLE"))
        {
            is_title_ = true;
        }
        return true;
    }
    bool characters(const crange& text)
    {
        if (care_data_)
        {
            filter_crlf(os_, text);
            care_data_ = false;
            os_ << "}\n";
        }

        if (is_title_)
        {
            assert(title_.empty());
            title_.assign(text.begin(), text.end());
        }
    
        return true;
    }
    bool end_element(const crange& name)
    {
        if (is_title_)
            is_title_ = false;
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

    bool care_data_;
    bool is_title_;

private:
    std::ostream & os_;
    std::string title_;
};

#endif
