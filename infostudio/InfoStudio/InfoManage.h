#pragma once

#include <map>
#include <vector>
#include <atlmisc.h>

#include "searchconf.h"
#include "webinfo.h"

class CInfoManage
{
public:
	CInfoManage(void);
	~CInfoManage(void);

	void Init();
	void Release();

	CWebInfo* createNewWebInfo( );
private:

	void InitDB();

	BOOL InitNoValidateThread();
	void ReleaseNoValidateThread();
	static DWORD WINAPI NoValidateThread(LPVOID pv);
	void WatchNoValidateThread();
	BOOL InitValidateThread();
	void ReleaseValidateThread();
	static DWORD WINAPI ValidateThread(LPVOID pv);
	void WatchValidateThread();

	void PostWebInfo( CWebInfo* pWebInfo );

	BOOL ProcessWebInfo( webRegister* pWebRegister, CWebInfo* pWebInfo );


	//数据库操作，初始化数据
	CWebInfo* createWebInfo( CAdoRecordSet* pRs );
	void InitWebInfo( CAdoRecordSet* pRs );
	void InitCategoryInfo( CategoryItem* &pLastItem, CAdoRecordSet* pRs );

	
public:

	//保存所有的网站信息
	std::map<int, CWebInfo*>	_webInfoMap;

	std::vector< CategoryItem* >	_categoryLst;

	// 当前的 weblist表里面的 id
	int						_maxID;

private:

	//保存注册信息
	BOOL					_Stop;
	BOOL					_Release;

	
	// 用两个队列，用一个队列来记录那些不需要弹出验证码的，用一个队列来继续需要弹出对话框的，
	// 整个系统只能黄页登录 ，信息发布，只能同时进行一项。
	std::vector< CWebInfo* > _noValidateLst;
	std::vector< CWebInfo* > _ValidateLst;
	
	std::vector< CWebInfo* > _finishLst;	//处理完的队列
	int						_type;	//表示当前是处理的黄页登录 ，信息发布还是什么操作。

	CRITICAL_SECTION			_CsNoValidateLst;
	CRITICAL_SECTION			_CsValidateLst;
	CRITICAL_SECTION			_CsFinishLst;

	std::vector<HANDLE>			_NoValidateHandleList;
	std::vector<HANDLE>			_ValidateHandleList;

	HWND						_hWnd;

};
