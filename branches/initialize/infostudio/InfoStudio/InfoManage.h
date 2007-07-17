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


	//���ݿ��������ʼ������
	CWebInfo* createWebInfo( CAdoRecordSet* pRs );
	void InitWebInfo( CAdoRecordSet* pRs );
	void InitCategoryInfo( CategoryItem* &pLastItem, CAdoRecordSet* pRs );

	
public:

	//�������е���վ��Ϣ
	std::map<int, CWebInfo*>	_webInfoMap;

	std::vector< CategoryItem* >	_categoryLst;

	// ��ǰ�� weblist������� id
	int						_maxID;

private:

	//����ע����Ϣ
	BOOL					_Stop;
	BOOL					_Release;

	
	// ���������У���һ����������¼��Щ����Ҫ������֤��ģ���һ��������������Ҫ�����Ի���ģ�
	// ����ϵͳֻ�ܻ�ҳ��¼ ����Ϣ������ֻ��ͬʱ����һ�
	std::vector< CWebInfo* > _noValidateLst;
	std::vector< CWebInfo* > _ValidateLst;
	
	std::vector< CWebInfo* > _finishLst;	//������Ķ���
	int						_type;	//��ʾ��ǰ�Ǵ���Ļ�ҳ��¼ ����Ϣ��������ʲô������

	CRITICAL_SECTION			_CsNoValidateLst;
	CRITICAL_SECTION			_CsValidateLst;
	CRITICAL_SECTION			_CsFinishLst;

	std::vector<HANDLE>			_NoValidateHandleList;
	std::vector<HANDLE>			_ValidateHandleList;

	HWND						_hWnd;

};
