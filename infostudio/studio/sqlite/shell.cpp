
#include <iostream>
#include <fstream>
#include <string>

#include "../base/unicode.h"
#include "../data/sqlite/sqlite3x.hpp"

using namespace std;
using namespace sqlite3x;

int main(int argc, char*argv[])
{
    std::string line;
    try {
        sqlite3_connection con(argv[1]);

        ifstream inf(argv[2]);        
        cout << "open " << argv[2] << endl;
        if (inf.is_open()) 
        {
            sqlite3_transaction trans(con);

            while (getline(inf, line))
            {
                std::wstring wline = string2w(line);
                con.executenonquery(wline);
            }

            trans.commit();
        }
        else
        {
            cout << "run:" << argv[2] << endl;
            con.executenonquery(argv[2]);
        }

    } catch(database_error & e) {
        cout << "error: " << e.what() << "\nsql:" << line << endl;
    }

    
    return 0;
}