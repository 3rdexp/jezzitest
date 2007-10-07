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
    typedef std::map<int, Industry> children_type;
    children_type children;

    Industry * insert(int pid, const Industry & ind)
    {
        assert(pid != 0);
        Industry * parent_ind = find(pid);
        if (parent_ind)
            return parent_ind->insert(ind);

        return 0;
    }

    Industry * insert(const Industry & ind)
    {
        assert(!ind.name.empty());
        assert(!ind.id == 0);
        std::pair<children_type::iterator, bool> ret = children.insert(children_type::value_type(ind.id, ind));
        return &(ret.first->second);
    }

    Industry * find(int id) const
    {
        if (this->id == id) 
            return const_cast<Industry*>(this);

        children_type::const_iterator i = children.find(id);
        if (i != children.end())
            return const_cast<Industry*>(&(i->second));

        for (children_type::const_iterator j=children.begin(); j != children.end();
            ++j)
        {
            Industry * p = j->second.find(id);
            if (p)
                return p;
        }
        return 0;
    }

    void swap(Industry & ind)
    {
        children.swap(ind.children);
        std::swap(name, ind.name);
        std::swap(ename, ind.ename);
        std::swap(parent, ind.parent);
        std::swap(id, ind.id);
    }
};


#endif // __INDUSTRY_H__