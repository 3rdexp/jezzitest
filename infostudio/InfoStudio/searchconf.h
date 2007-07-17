#if !defined(AFX_SEARCHCONF_H__0017283F_4EE8_46E9_AA4F_1E84B2CC08F6__INCLUDED_)
#define AFX_SEARCHCONF_H__0017283F_4EE8_46E9_AA4F_1E84B2CC08F6__INCLUDED_

enum HttpMethod
{
	HTTP_NULL	= -1,
	HTTP_POST	= 0,
	HTTP_GET    = 1,
};


enum
{
	//HTTP_THREAD_DEFAULT = 3,
	//ANALYSE_THREAD_DEFAULT = 1,

	NOVALIDATE_THREAD_DEFAULT = 3,
	VALIDATE_THREAD_DEFAULT = 1,
};

enum 
{
	POST_NULL = -1,
	POST_LOGIN = 0,
	POST_REGISTER = 1,
	POST_PRODUCE = 2,
	POST_BBS = 3,
	POST_SETTING = 4,
	POST_COUNT = 5,
};

enum
{
	RESULT_START	= 0,
	RESULT_VALIDATE = 1,
	RESULT_FAILED	= 2,
	RESULT_SUCCESS	= 3,
};

enum
{
	LOGINBASE_NOUSED	= 0,
	LOGINBASE_USED		= 1,
};


//
struct workInfo
{
	int			_type;
	//std::vector< int >	_workList;
	int			_nId;
	int			_result;// 处理结果
};

#endif // !defined(AFX_SEARCHCONF_H__0017283F_4EE8_46E9_AA4F_1E84B2CC08F6__INCLUDED_)