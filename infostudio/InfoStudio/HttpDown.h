// HttpDown.h: interface for the CHttpDown class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_HTTPDOWN_H__1B0F58A6_BC0D_4B41_8C73_EDDA8BA27B8F__INCLUDED_)
#define AFX_HTTPDOWN_H__1B0F58A6_BC0D_4B41_8C73_EDDA8BA27B8F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "infomanage.h"

#include "httpupload\\LyoulHttpClient.h"
#include "httpupload\\LyoulErrMgmtFunc.h"

using namespace Lyoul;

class CHttpDown  
{
public:
	CHttpDown();
	virtual ~CHttpDown();
	void Init( CInfoManage* pInfoManage, webRegister& webReg );
	void StartDown();
	BOOL IsSuccess( CString strKey );
	static inline BOOL CheckOperationStat ( CInfoManage* pInfoManage );// throw (DWORD);

	typedef std::map<CString, CString>::iterator PARAM_ITERATOR;	

private:

	CHttpClient		httpClient;
	CInfoManage*	_pInfoManage;
	CString			_Url;
	HttpMethod		_method;
	CString			_buff;
};

#endif // !defined(AFX_HTTPDOWN_H__1B0F58A6_BC0D_4B41_8C73_EDDA8BA27B8F__INCLUDED_)
