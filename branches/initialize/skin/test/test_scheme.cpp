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
            sd.add(r, ain);
        }
        else
        {
            sd.add(r, r+1);
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
   sd.add(1, 1);
}

void test_scheme_parse()
{
    CoInitialize(0);
    
    scheme_holder sh;
    sh.parse_scheme("..\\debug\\test.cfg");

    for(scheme_holder::map_type::iterator i=sh._map.begin(); i!=sh._map.end(); ++i)
    {
        cout << i->first << "\n";
        scheme_data & sd = i->second;
        for(scheme_data::map_type::iterator j=sd._map.begin(); j!=sd._map.end(); ++j)
        {
            unsigned key = j->first;
            cout << setw(8);
            cout << (key >> 26);
            cout << " " << ((key&0x3f00000) >> 20);
            cout << " " << ((key & 0xf0000) >> 16);
            cout << " " << (key & 0xFFFF);
            area_t * pa = get<area_t>(&(j->second));
            if (pa)
                cout << " " << pa->left << " " << pa->top << " " << pa->right << " " << pa->bottom;
            cout << '\n';
        }
    }
    
    CoUninitialize();
}
