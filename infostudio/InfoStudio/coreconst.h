#ifndef __CORECONSTANT_H__
#define __CORECONSTANT_H__

#pragma once

enum ActionType
{
    AT_UTILITY,     
    AT_REGISTER,    // 
    AT_PUBLISH,
    AT_RESULT,       // 检查发布结果
};

enum ActionResponseType
{
    ART_VERIFY_IMAGE,
    ART_CHECK_RESULT,
    ART_NONE,
};

enum SiteCharset
{
    SC_ANSI,
    SC_UTF8,
};



#endif // __CORECONSTANT_H__
