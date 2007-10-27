#ifndef __CORECONSTANT_H__
#define __CORECONSTANT_H__

#pragma once

enum ActionType
{
    AT_UTILITY,     
    AT_REGISTER,    // 
    AT_PUBLISH,
    AT_PUBRESULT,       // ��鷢�����
};

enum CheckType
{
    CT_EXISTS,      // ����б��뺬�У�����ʧ��
    CT_EXCLUDE,     // ������
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
