#ifndef __K_HANDLER_H__
#define __K_HANDLER_H__

#include <iostream>
#include <iterator>
#include <iomanip>

#include <boost/algorithm/string.hpp>

class dump_handler // : public tagparse_handler
{
public:
    dump_handler() : _indent(0) {}
    bool start_document()
    {
        std::cout << "start\n";
        return true;
    }
    bool start_element(const crange& name, const attr_array& arr)
    {
        indent(std::cout, max(_indent ++, 0));
        std::cout << '<' << name;

        if (arr.size())
        {
            for(size_t i=0; i<arr.size(); ++i)
            {
                std::cout << " " << arr[i].first << "=" << arr[i].second << "";
            }
        }
        std::cout << ">\n";
        return true;
    }
    bool characters(const crange& text)
    {
        indent(std::cout, max(_indent, 0));
        for (crange::const_iterator i = text.begin(); i != text.end(); ++i)
        {
            if (*i != '\r' && *i != '\n')
            {
                std::cout << *i;
            }
        }
        std::cout << "\n";
        return true;
    }
    bool end_element(const crange& name)
    {
        indent(std::cout, max(--_indent, 0));
        std::cout << "</" << name << ">\n";
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

private:
    void indent(std::ostream& os, int c, char to_fill = ' ')
    {
        std::string s(c, to_fill);
        os << s;
    }

    int _indent;
};


class empty_handler// : public tagparse_handler
{
public:
    bool start_document()
    {
        return true;
    }
    bool start_element(const crange& name, const attr_array&)
    {
        return true;
    }
    bool characters(const crange& text)
    {
        return true;
    }
    bool end_element(const crange& name)
    {
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
};


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

class content_extract // : public tagparse_handler
{
public:
    content_extract(std::ostream & os = std::cout) : os_(os), care_data_(false)
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
                filter_crlf(os_, i->second);
                os_ << '\n';
            }
            else
            {
                i = find_attribute(arr, "NAME");

                assert((arr.size() && i != arr.end()) || !arr.size());
                for (attr_array::const_iterator i=arr.begin(); i!=arr.end(); ++i)
                {
                    os_ << i->first << "=" << i->second << "\n";
                }
            }
        }
        return true;
    }
    bool characters(const crange& text)
    {
        if (care_data_)
        {
            os_ << "  ";
            filter_crlf(os_, text) << '\n';
        }
        return true;
    }
    bool end_element(const crange& name)
    {
        if (care_data_)
            care_data_ = false;
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

    std::ostream & os_;
    bool care_data_;
};

#endif // __K_HANDLER_H__
