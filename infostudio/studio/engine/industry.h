#ifndef __INDUSTRY_H__
#define __INDUSTRY_H__

#include <string>
#include <list>
#include <map>
#include <vector>
#include <cassert>

struct Industry
{
    Industry() 
        : parent(0), id(0) {}
        Industry(const std::wstring & n) 
        : parent(0), name(n), id(0) {}

    std::wstring name, ename;
    int id;

    Industry * parent;
    typedef std::list<Industry> list_type;
    list_type children;

    bool insert(int pid, const Industry & ind)
    {
        assert(pid != 0);
        Industry * parent_ind = find(pid);
        if (parent_ind)
            return parent_ind->insert(ind);

        return false;
    }

    bool insert(const Industry & ind)
    {
        assert(!ind.name.empty());
        assert(!ind.id == 0);
        children.push_back(ind);
        list_type::iterator i = children.end();
        Industry & r = *(--i);
        r.parent = this;
        id_map.insert(id_map_type::value_type(ind.id, &r));
        return true;
    }

    Industry * find(int id) const
    {
        id_map_type::const_iterator i = id_map.find(id);
        if (i != id_map.end())
            return i->second;

        for (list_type::const_iterator j=children.begin(); j != children.end();
            ++j)
        {
            Industry * p = j->find(id);
            if (p)
                return p;
        }
        return 0;
    }

    // 模糊匹配多个行业？
    //    std::list<Industry*> find(const std::wstring & name) const
private:
    typedef std::map<int, Industry*> id_map_type;
    id_map_type id_map;;
};


#endif // __INDUSTRY_H__