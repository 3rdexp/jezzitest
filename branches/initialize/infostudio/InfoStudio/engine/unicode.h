
#ifndef __UNICODE_H__
#define __UNICODE_H__

#pragma once

#include <windows.h>
#include <string>
#include "base.h"

BEGIN_ENGINE_NAMESPACE

inline std::string string2utf8(const std::string & text)
{
    // ansi => utf16 => utf8
    const int wbuf_len = MultiByteToWideChar(CP_ACP, 0, text.c_str()
        , text.size(), NULL, 0);
    wchar_t* wbuf = new wchar_t[wbuf_len];

    int cd = MultiByteToWideChar(CP_ACP, 0, text.c_str(), text.size()
        , wbuf, wbuf_len);
    ASSERT(cd == wbuf_len);


    const int u8_len = WideCharToMultiByte(CP_UTF8, 0, wbuf, wbuf_len
        , 0, 0, NULL, NULL);
    char * u8 = new char[u8_len + 1];
    cd = WideCharToMultiByte(CP_UTF8, 0, wbuf, wbuf_len
        , u8, u8_len, NULL, NULL);
    ASSERT(cd == u8_len);

    std::string ret(u8, u8 + u8_len);

    delete [] wbuf;
    delete [] u8;
    return ret;
}

inline std::string string2utf8(const std::wstring & text)
{
    // utf16 => utf8
    const int u8_len = WideCharToMultiByte(CP_UTF8, 0, text.c_str(), text.size()
        , 0, 0, NULL, NULL);
    char * u8 = new char[u8_len + 1];
    int cd = WideCharToMultiByte(CP_UTF8, 0, text.c_str(), text.size()
        , u8, u8_len, NULL, NULL);
    ASSERT(cd == u8_len);

    std::string ret(u8, u8 + u8_len);

    delete [] u8;
    return ret;
}

inline bool utf82string(const char* u8, int len, std::string& str)
{
    // u8 => utf16 => ansi
    const int wbuf_len = MultiByteToWideChar(CP_UTF8, 0, u8, len
        , NULL, 0);
    wchar_t* wbuf = new wchar_t[wbuf_len];

    int cd = MultiByteToWideChar(CP_UTF8, 0, u8, len
        , wbuf, wbuf_len);
    ASSERT(cd == wbuf_len);

    const int ansi_len = WideCharToMultiByte(CP_ACP, 0, wbuf, wbuf_len
        , 0, 0, NULL, NULL);
    char * ansi = new char[ansi_len + 1];
    cd = WideCharToMultiByte(CP_ACP, 0, wbuf, wbuf_len
        , ansi, ansi_len, NULL, NULL);
    ASSERT(cd == ansi_len);

    str.assign(ansi, ansi_len);
    delete [] wbuf;
    delete [] ansi;
    return true;
}

inline bool utf82string(const std::string & us, std::string& str)
{
    return utf82string(us.c_str(), us.size(), str);
}

inline bool utf82string(const char* u8, int len, std::wstring& str)
{
    const int wbuf_len = MultiByteToWideChar(CP_UTF8, 0, u8, len
        , NULL, 0);
    wchar_t* wbuf = new wchar_t[wbuf_len];

    int cd = MultiByteToWideChar(CP_UTF8, 0, u8, len
        , wbuf, wbuf_len);
    ASSERT(cd == wbuf_len);

    str.assign(wbuf, wbuf_len);
    delete [] wbuf;
    return true;
}

inline bool utf82string(const std::string & us, std::wstring& str)
{
    return utf82string(us.c_str(), us.size(), str);
}

inline std::string w2string(const std::wstring & text)
{
    // utf16 => ansi
    const int u8_len = WideCharToMultiByte(CP_ACP, 0, text.c_str(), text.size()
        , 0, 0, NULL, NULL);
    char * u8 = new char[u8_len + 1];
    int cd = WideCharToMultiByte(CP_ACP, 0, text.c_str(), text.size()
        , u8, u8_len, NULL, NULL);
    ASSERT(cd == u8_len);

    std::string ret(u8, u8 + u8_len);

    delete [] u8;
    return ret;
}

END_ENGINE_NAMESPACE

#endif // __UNICODE_H__

