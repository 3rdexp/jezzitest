
#pragma once

#include <limits>
#include "base.h"
#undef max

BEGIN_ENGINE_NAMESPACE

// Escapes characters not in [0-9a-zA-Z.,_:*/~!()-] as %-prefixed hex.
// Space is encoded as a +.
struct UrlQueryUnEscape { std::string operator()(const std::string&) const; };

inline short hexvalue(char chIn) throw()
{
    unsigned char ch = (unsigned char)chIn;
    if (ch >= '0' && ch <= '9')
        return (short)(ch - '0');
    if (ch >= 'A' && ch <= 'F')
        return (short)(ch - 'A' + 10);
    if (ch >= 'a' && ch <= 'f')
        return (short)(ch - 'a' + 10);
    return -1;
}

// ftext=Your+Name&%B9%EE%D2%EC=%D6%D0+%CE%C4&fpassword=eeee&fradio=0&ftext2=long+value+%D6%D0%CE%C4&fcheckbox=on&fselect=1&fselect=3&fsubmit=OK
inline std::string UrlQueryUnEscape::operator()(const std::string& in) const 
{
    enum STATE { STATE_NUMBER1, STATE_NUMBER2, STATE_NORMAL };
    std::string ret;
    int num;
    std::string::size_type pos = 0;
    STATE state = STATE_NORMAL;
    while (pos < in.size())
    {
        char ch = in[pos++];
        if (STATE_NORMAL == state)
        {
            switch (ch)
            {
            case '+':
                ret += ' ';
                break;
            case '%':
                state = STATE_NUMBER1;
                break;
            default:
                ret += ch;
                break;
            }
        }
//        else if (STATE_PREFIX == state)
//        {            
//            state = STATE_NUMBER1;
//        }
        else if (STATE_NUMBER1 == state)
        {
            ASSERT(
                (ch >='0' && ch <= '9') || 
                (ch >='a' && ch <='f') ||
                (ch >='A' && ch <='F')
                );
            num = 16 * hexvalue(ch);
            state = STATE_NUMBER2;
        }
        else if (STATE_NUMBER2 == state)
        {
            ASSERT(
                (ch >='0' && ch <= '9') || 
                (ch >='a' && ch <='f') ||
                (ch >='A' && ch <='F')
                );
            num += hexvalue(ch);
            if (num <= std::numeric_limits<unsigned char>::max())
            {
                ret.push_back((char)num);
            }
            
            state = STATE_NORMAL;
            num = 0;
        }
    }
    return ret;
}

END_ENGINE_NAMESPACE

