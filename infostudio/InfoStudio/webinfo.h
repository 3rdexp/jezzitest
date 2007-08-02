#pragma once

#include <map>
#include <vector>
#include <list>
#include <string>


#if 0

/*
加载 UserInfo

加载要访问的网站 SiteSpecial

加载该网站的 ActionData

执行 Get/Post
*/

typedef std::map<std::wstring, std::wstring> VariableMap;

struct UserInfo
{
public:


private:
    VariableMap vars;
};


enum SiteActionType
{
    SAT_UTILITY,
    SAT_REGISTER,   // 
    SAT_LOGIN,
    SAT_PUBLISH,
    SAT_CHECK,  // 检查发布结果
};

enum SiteCharset
{
    SC_ANSI,
    SC_UTF8,
};

// for a=b&c={name}&e*= translate
// usage:
// VarTemplate vt(L"a=b&c={name}&e*=");
// string s = vt.Apply(UserInfo.vars, SC_ANSI, UrlQueryEscape());
// xxx.Post(s);

class VarTemplate
{
public:
    VarTemplate(const wstring & text);
    template<class Encoding>
    std::string Apply(const VariableMap & vars, SiteCharset charset, Encoding & e);
    // 
};

// 访问一个url，需要访问的数据
// 
struct ActionData
{
    SiteActionType type;
    std::string url;
    HttpVerb verb;  // HV_GET / HV_POST
    SiteCharset charset;
    std::string referrer;

    ActionData * front; // 前置任务

    // 
    time_t time;
};

struct Industry
{
    Industry * parent;
    deque<Industry> children;

    int id;
    std::wsting cname;
    std::wstirng ename;
};

class SiteSpecial
{
public:
    vector<ActionData*> find(SiteActionType type);

private:
    list<ActionData> actions_;

    // site info
    std::string homepage_;
    std::wstring name_;

    std::wstring username_;
    std::wstring passwd_;
};

#endif // #if 0

// ? 行业需要模糊匹配，能在数据结构中直接表示否？
// TODO:  std::vector< CategoryItem* > => std::vector<CategoryItem>

//行业分类
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
	int			_httptype;		//post还是get方式提交	
	CString		_validateUrl;
	CString		_validateCode;
	CString		_success;
	int			_type;			// 0 表示登录 1 表示注册 2 产品发布 3 供求发布 4 论坛发布 5 设置
	int			_base;
	BOOL		_utf8;			// 是否是UTF8编码
	int			_loginBase;		// 0  不需要登录 1 需要登录
	CString		_strResultUrl;
	CString		_strPubViewUrl;
};

struct webSearchItem
{
	int		_type;	//类型
	int		_item;	//要搜索的网站类型
	int		_base;  //是否已经把依赖的网站搜索出来
	int		_loginbase;// 0 没有使用过  1 使用过了
	
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

	int		_nType;				// 表示是那种类型的  0 表示登录 1 表示注册 2 产品发布 3 供求发布 4 论坛发布 5 设置
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

	// 基础信息
	int			_id;
	CString		_strName;
	CString		_strWebUrl;
	CString		_strType;	//属于那个类别，比如五金
	int			_grade;
	CString		_strUser;
	CString		_strPass;
	CString		_strRegisterTime;
	CString		_strState;
	BOOL		_success;
	int			_language;// 1 简体 2 英文 3 繁体
	int			_used;	// 1可用 0 不可用
	int			_demoUsed; // 1可用 0 不可用
	CString		_strRemark;

	// 0 表示登录 1 表示注册 2 产品发布 3 供求发布 4 论坛发布
	// 登录信息

	int			_result;// 信息发布的结果

	std::map<int, webRegisterCollect*>	_webRegisterCollectMap;

	std::vector < webSearchItem >		_referLst; //需要提交的列表  在发布信息的时候，需要把当前的操作添加到这个列表中 比如是黄页登录等。

};