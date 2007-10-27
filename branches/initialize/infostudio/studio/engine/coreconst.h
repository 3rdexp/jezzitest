#ifndef __CORECONSTANT_H__
#define __CORECONSTANT_H__

#pragma once

enum ActionType
{
    AT_UTILITY,     
    AT_REGISTER,    // 
    AT_PUBLISH,
    AT_PUBRESULT,       // 检查发布结果
};

enum CheckType
{
    CT_EXISTS,      // 结果中必须含有，否则失败
    CT_EXCLUDE,     // 不包含
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
