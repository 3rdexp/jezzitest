#ifndef __SKIN__SCHEME_H__
#define __SKIN__SCHEME_H__

// namespace Windows {
#include <windows.h>
// } // Windows

#include <hash_map>
#include <map>
#include <string>
#include <boost/variant.hpp>
#include <boost/shared_ptr.hpp>

namespace Skin{

struct area_t : public RECT
{
    int hstep;
    int vstep;

    int Width() { return right - left; }
    int Height() { return bottom - top; }
};

typedef COLORREF color_t;

struct font_t : public LOGFONT
{
};

// 
class scheme_data
{
public:
    typedef unsigned long key_type;
    typedef boost::variant<area_t, color_t, font_t> value_type;
    typedef std::hash_multimap<key_type, value_type> map_type;

    template<typename T>
        bool add(key_type k, const T& val)
    {
        map_type::iterator ret = 
            _map.insert(map_type::value_type(k, value_type()));
        ret->second = val;
        return true;
    }

    template<typename T>
        bool get(key_type k, T & val) const
    {
        std::pair<map_type::const_iterator, map_type::const_iterator> ret = 
            _map.equal_range(k);
        for (map_type::const_iterator i = ret.first; i != ret.second; ++i)
        {
            if (const T* p = boost::get<T>(&(i->second)) )
            {
                val = *p;
                return true;
            }
        }
        return false;
    }

#ifndef _DEBUG
private:
#endif
    map_type _map;
};

class CCacheDC;

// value in cfg file class_id
// value in hash     class_id << 26
// value in SkinMgr::InitControls

inline scheme_data::key_type make_key(int iClassId, int iPartId, int iState, int iProp = 0)
{
    ASSERT(iClassId < 1<<6);
    
    return iClassId<<26 | iPartId<<20 | iState<<16 | iProp;
}

class scheme_holder
{
public:
    typedef std::map<std::string, scheme_data> map_type;

    const scheme_data* get(const std::string & name) const
    {
        map_type::const_iterator i = _map.find(name);
        if (i != _map.end())
            return &(i->second);
        return 0;
    }

    scheme_data & insert(const std::string & name)
    {
        return _map[name];
    }

    boost::shared_ptr<CCacheDC> get_cache()
    {
        return _spCache;
    }

    bool parse_scheme(const std::string & fname);

#ifndef _DEBUG
private:
#endif
    map_type _map;
    std::string _filename;
    boost::shared_ptr<CCacheDC> _spCache;
};

} // Skin

#endif //__SKIN__SCHEME_H__