#pragma once

#include <map>
#include <vector>
#include <string>


#if 0
struct UserInfo
{
public:
    typedef std::map<std::wstring, std::wstring> VarMap;    
    VarMap vars;
};


enum SiteActionType
{
    SAT_REGISTER,   // 
    SAT_LOGIN,
    SAT_PUBLISH,
    SAT_CHECK,
};

enum SiteCharset
{
    SC_ANSI,
    SC_UTF8,
};

// for a=b&c={name}&e*= translate
// usage:
// VarTemplate vt(L"a=b&c={name}&e*=");
// wstring s = vt.Apply(UserInfo.vars);
// xxx.Post(s);

class VarTemplate
{
public:
    VarTemplate(const wstring & text);

    typedef std::map<std::wstring, std::wstring> VarMap;

    std::wstring Apply(const VarMap & vars);
};

// ???
struct SiteAction
{
    SiteActionType type;
    std::wstring url;
    SiteCharset charset;
    std::wstring referrer;
};

class SiteInfo
{
public:

private:
    vector<SiteAction> actions_; ???
};

#endif // #if 0

// ? ��ҵ��Ҫģ��ƥ�䣬�������ݽṹ��ֱ�ӱ�ʾ��
// TODO:  std::vector< CategoryItem* > => std::vector<CategoryItem>

//��ҵ����
class CategoryItem
{
public:
	CategoryItem()
	{
		_strID = "";
		_strCName = "";
		_strEName = "";
	}

	~CategoryItem()
	{

		for ( int i = 0; i < _childItemLst.size(); i++ )
		{
			CategoryItem*	pItem = _childItemLst[i];
			delete pItem;
		}

		_childItemLst.clear();

	}

	CString		_strID;

	CString	_strCName;
	CString	_strEName;

	CategoryItem*	_pParentItem;

	std::vector< CategoryItem* >	_childItemLst;
};


struct webRegister
{
	int			_id;
	int			_item;
	CString		_url;
	CString		_posturl;
	std::map<CString, CString>	_headMap;
	std::map<CString, CString>	_postMap;
	int			_httptype;		//post����get��ʽ�ύ	
	CString		_validateUrl;
	CString		_validateCode;
	CString		_success;
	int			_type;			// 0 ��ʾ��¼ 1 ��ʾע�� 2 ��Ʒ���� 3 ���󷢲� 4 ��̳���� 5 ����
	int			_base;
	BOOL		_utf8;			// �Ƿ���UTF8����
	int			_loginBase;		// 0  ����Ҫ��¼ 1 ��Ҫ��¼
	CString		_strResultUrl;
	CString		_strPubViewUrl;
};

struct webSearchItem
{
	int		_type;	//����
	int		_item;	//Ҫ��������վ����
	int		_base;  //�Ƿ��Ѿ�����������վ��������
	int		_loginbase;// 0 û��ʹ�ù�  1 ʹ�ù���
	
};

class webRegisterCollect
{
public:
	webRegisterCollect()
	{

	}

	~webRegisterCollect()
	{
		for ( int i = 0; i < _webRegisterLst.size(); i++ )
		{
			delete  _webRegisterLst[i];
		}
	}

	int		_nType;				// ��ʾ���������͵�  0 ��ʾ��¼ 1 ��ʾע�� 2 ��Ʒ���� 3 ���󷢲� 4 ��̳���� 5 ����
	std::vector< webRegister* >	_webRegisterLst;
};





class CWebInfo
{
public:
	CWebInfo()
	{
		_language	= 1;
		_grade	= 5;
	}

	~CWebInfo()
	{
		Release();
	}

	void Release()
	{
		for( std::map<int, webRegisterCollect*>::iterator it = _webRegisterCollectMap.begin(); it!= _webRegisterCollectMap.end(); it++ )
		{
			delete it->second;
		}
	}
	void Init();

	int GetMaxItem( int nType )
	{
		int nMax = 0;
		std::map<int, webRegisterCollect*>::iterator it = _webRegisterCollectMap.find( nType );
		if( it != _webRegisterCollectMap.end() )
		{
			webRegisterCollect* pWebRegisterCollect = it->second;
			for ( int i = 0 ; i < pWebRegisterCollect->_webRegisterLst.size(); i++ )
			{
				if ( pWebRegisterCollect->_webRegisterLst[i]->_item > nMax )
					nMax = pWebRegisterCollect->_webRegisterLst[i]->_item;
			}
		}
		
		return nMax;
	}

	void AddWebRegister( webRegister* pWebRegister )
	{
		std::map<int, webRegisterCollect*>::iterator it = _webRegisterCollectMap.find( pWebRegister->_type );

		webRegisterCollect* pWebRegisterCollect	= NULL;

		if( it == _webRegisterCollectMap.end() )
		{
			pWebRegisterCollect = new webRegisterCollect();
			pWebRegisterCollect->_nType = pWebRegister->_type;
			_webRegisterCollectMap.insert( std::make_pair( pWebRegisterCollect->_nType, pWebRegisterCollect) );
		}
		pWebRegisterCollect->_webRegisterLst.push_back( pWebRegister );
	}

public:

	// ������Ϣ
	int			_id;
	CString		_strName;
	CString		_strWebUrl;
	CString		_strType;	//�����Ǹ���𣬱������
	int			_grade;
	CString		_strUser;
	CString		_strPass;
	CString		_strRegisterTime;
	CString		_strState;
	BOOL		_success;
	int			_language;// 1 ���� 2 Ӣ�� 3 ����
	int			_used;	// 1���� 0 ������
	int			_demoUsed; // 1���� 0 ������
	CString		_strRemark;

	// 0 ��ʾ��¼ 1 ��ʾע�� 2 ��Ʒ���� 3 ���󷢲� 4 ��̳����
	// ��¼��Ϣ

	int			_result;// ��Ϣ�����Ľ��

	std::map<int, webRegisterCollect*>	_webRegisterCollectMap;

	std::vector < webSearchItem >		_referLst; //��Ҫ�ύ���б�  �ڷ�����Ϣ��ʱ����Ҫ�ѵ�ǰ�Ĳ�����ӵ�����б��� �����ǻ�ҳ��¼�ȡ�

};