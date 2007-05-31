#ifndef __TAGPATH_H__
#define __TAGPATH_H__

#include <string>
#include <deque>

#include "parser/range.h"

namespace kong {

//////////////////////////////////////////////////////////////////////////
//
class Section
{
public:
    virtual ~Section() {}
    virtual bool Test(const range_t & name, const attrarray_t & attr) const;
};

// TagSection
// AttrSection


class TagSection : public Section
{
public:
  std::string name_;
};

//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////
class Expresssion
{
public:
private:
    std::deque<Section> secs_;
};

typedef std::pair<Expresssion, Expresssion> ExpressoinPair;



//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////
class DataSnatch;

class YYY
{
    DataSnatch snatch_;
    bool care_data_;
    bool care_tag_;
    ExpressoinPair expr_;

    bool Test(name, attr);
    void ProcessTagBegin(name, attr);
    void ProcessTagEnd(name);
    void ProcessData(data);
};

class Dispatcher
{
    std::list<YYY*> all_;
    std::list<YYY*> matched_;
    ProcessTagBegin(name, attr)
    {
        // span/a/table/tr         br

        //
        {
            for(i in matched_)
                i->ProcessTagBegin(name, attr);
        }
        
        // new pattern
        {
            for (i in all_)
            {
                if (i->Test(name, attr))
                {
                    move i into matched_;
                }
            }
        }
    }

    ProcessTagEnd(name)
    {
        //
        {
            for(i in matched_)
                i->ProcessTagEnd(name);
        }

        // new pattern
        {
            for (i in all_)
            {
                if (i->Test(name, attr))
                {
                    move i into matched_;
                }
            }
        }
    }
    ProcessData(data);


    //////////////////////////////////
    XXX x_;
    void tag_begin(name, attr)
    {
        x_.Process(name, attr);
    }
    void tag_end(name)
    {


    }
};

} // kong

#endif // __TAGPATH_H__
