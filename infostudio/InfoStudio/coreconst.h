#ifndef __CORECONSTANT_H__
#define __CORECONSTANT_H__

#pragma once

enum ActionType
{
    AT_UTILITY,     
    AT_REGISTER,    // 
    AT_LOGIN,
    AT_PUBLISH,
    AT_CHECK,       // 检查发布结果
};

enum SiteCharset
{
    SC_ANSI,
    SC_UTF8,
};

#endif // __CORECONSTANT_H__
