#include "test.h"

#include "../src/base/scheme.h"

using namespace boost;
using namespace std;
using namespace Skin;

void test_scheme_data()
{    
    scheme_data sd;

    progress_timer pt;
    const int c = 50000;
    int i = 0;
    while(i++<c)
    {
        int r = rand() % 1000;
        if (r%2)
        {
            area_t ain;
            ain.hstep = 123;
            sd.put(r, ain);
        }
        else
        {
            sd.put(r, r+1);
        }
    }

    cout << "insert: " << pt.elapsed() << endl;
    pt.restart();

    int got_count = 0;

    while(i--)
    {
        int r = rand() % 1000;
        if (r%2)
        {
            area_t aout;
            if( sd.get(r, aout) )
                got_count++;
        }
        else
        {
            color_t aout;
            if( sd.get(r, aout) )
                got_count++;
        }
    }
    cout << "get: " << pt.elapsed() << endl;
    cout << "got count: " << got_count << endl;
}

void test_scheme_holder()
{
   scheme_holder sh;
   scheme_data & sd = sh.insert("default");
   sd.put(1, 1);
}
