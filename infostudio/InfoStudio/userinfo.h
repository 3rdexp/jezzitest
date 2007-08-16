#pragma once
#include <string>
#include <vector>
#include <map>
class CUserItem
{
public:
	CUserItem()
	{

	}

	~CUserItem()
	{

	}

public:
	
	CString	_strName;			//�ֶ���,���繫˾��
	CString	_strValue;			//ʵ�ʵ�ֵ ���粩����˾
	CString	_strParamValue;		//�ύ����������ı�ʾ ������{companyname} 
	CString	_strByname;			//�û�¼�������,�����û�¼��,����company ����������û��޸�

};
class CUserInfo
{
public:
	CUserInfo()
	{

	}

	~CUserInfo()
	{

	}


	CString getParamValue( const CString& strByName )
	{
		for( std::map< CString, CUserItem >::iterator it = _userInfoMap.begin(); it!= _userInfoMap.end(); it++ )
		{
			CUserItem& Item = it->second;
			if ( Item._strByname == strByName )
				return Item._strParamValue;
		}

		return "";
	}
public:

	std::map< CString, CUserItem > _userInfoMap;

/*
	CString			_strCompany;
	CString			_strCompanyE;
	CString			_strCompanyShort;
	CString			_strCompanyShortE;
	CString			_strfinance;
	CString			_strkind;
	CString			_strmode;
	CString			_strachievement;
	CString			_stremployee;
	CString			_strcorporate;
	CString			_strcorporateE;
	CString			_strhomepage;
	CString			_strbuildup;
	CString			_stroperation;
	CString			_stroperationE;
	CString			_strstock;
	CString			_strstockE;
	CString			_strdesc;
	CString			_strdescE;
	CString			_strcalling;

	CString			_strlinkman;
	CString			_strsex;
	CString			_strduty;
	CString			_strdutyE;

	CString			_strlinkmanFirst;
	CString			_strlinkmanLast;
	CString			_strarea;
	CString			_strarea1;
	CString			_strarea2;
	CString			_strarea3;

	CString			_straddress;
	CString			_straddressE;
	CString			_strpostcode;
	CString			_strbranch;
	CString			_strbranchE;
	CString			_stremail;
	CString			_strcardID;
	CString			_stremailbackup;

	CString			_strmobile;
	CString			_strphone;

	CString			_strfax;
	CString			_strquestion;
	CString			_strquestionE;
	CString			_stranswer;
	CString			_stranswerE;
*/
};