#include "stdafx.h"

#include "validateDlg.h"
#include "searchconf.h"
#include "HttpDown.h"

#include "ado/AdoRecordSet.h"
#include "infomanage.h"

CInfoManage::CInfoManage(void)
{

}

CInfoManage::~CInfoManage(void)
{
	Release();
}

void CInfoManage::Init()
{
	//��ʼ���������ݿ������ȡ��Щ��Ϣ��

	_Stop				= FALSE;
	_Release			= FALSE;

	InitializeCriticalSection(&_CsNoValidateLst);
	InitializeCriticalSection(&_CsValidateLst);
	InitializeCriticalSection(&_CsFinishLst);
	
	InitDB();

}

void CInfoManage::InitDB()
{

	_maxID = 0;
	// ѡ����վ�б�

	CString strSql;			
	strSql = "select * from weblist  where used > 0 order by id"; //ֻ�ǰ�ʹ�õ�ѡ�����
	CAdoRecordSet* pRs = new CAdoRecordSet(_pDb);
	if( pRs->Open((LPCTSTR) strSql ) )
	{
		pRs->MoveFirst();
		while ( !pRs->IsEOF() )
		{
			CWebInfo* pWebInfo = createWebInfo( pRs );
			if ( pWebInfo )
			{
				_maxID = pWebInfo->_id > _maxID ? pWebInfo->_id : _maxID;
				_webInfoMap.insert ( std::make_pair( pWebInfo->_id, pWebInfo ) );
			}
			pRs->MoveNext();
		}	
	}

	if ( pRs )
		delete pRs;

	pRs = NULL;


	strSql = "select * from weblistinfo order by id,item "; //ֻ�ǰ�ʹ�õ�ѡ�����
	pRs = new CAdoRecordSet(_pDb);
	if( pRs->Open((LPCTSTR) strSql ) )
	{
		pRs->MoveFirst();
		while ( !pRs->IsEOF() )
		{
			InitWebInfo( pRs );	
			pRs->MoveNext();
		}	
	}

	if ( pRs )
		delete pRs;

	pRs = NULL;


	// ����ҵ�������
	strSql = "select * from category order by categorycode"; //ֻ�ǰ�ʹ�õ�ѡ�����
	pRs = new CAdoRecordSet(_pDb);
	if( pRs->Open((LPCTSTR) strSql ) )
	{
		pRs->MoveFirst();

		CategoryItem* pLastItem = NULL;
		
		while ( !pRs->IsEOF() )
		{
			InitCategoryInfo( pLastItem, pRs );	
			pRs->MoveNext();
		}	
	}

	if ( pRs )
		delete pRs;

	pRs = NULL;


}

//weblistinfo table
void CInfoManage::InitWebInfo( CAdoRecordSet* pRs )
{

	int nID;
	CString	strUrl;
	int nType;
	CString	strHeader;
	CString strContent;
	int Posttype;
	CString strValidateurl;
	CString strSuccessRet;
	int Basetype;
	int nItem;
	CString strPostUrl;
	int		loginBase;
	CString strResultUrl;
	CString strPubViewUrl;

	pRs->GetCollect(_T("id"), nID);
	pRs->GetCollect(_T("item"), nItem);
	pRs->GetCollect(_T("url"), strUrl);
	pRs->GetCollect(_T("posturl"), strPostUrl);
	pRs->GetCollect(_T("type"), nType);
	pRs->GetCollect(_T("header"), strHeader);
	pRs->GetCollect(_T("content"), strContent);
	pRs->GetCollect(_T("posttype"), Posttype);
	pRs->GetCollect(_T("validateurl"), strValidateurl);
	pRs->GetCollect(_T("successRet"), strSuccessRet);
	pRs->GetCollect(_T("basetype"), Basetype);
	pRs->GetCollect(_T("loginBase"), loginBase);
	pRs->GetCollect(_T("resulturl"), strResultUrl);
	pRs->GetCollect(_T("pubviewurl"), strPubViewUrl);


	//�ҵ��Ѿ���ʼ����WebInfo;
	std::map<int, CWebInfo*>::iterator it = _webInfoMap.find( nID );
	if( it != _webInfoMap.end() )
	{
		CWebInfo* pWebInfo = it->second;
		//���ҵ�webRegisterCollect����

		webRegisterCollect* pWebRegisterCollect = NULL;

		std::map<int, webRegisterCollect*>::iterator it = pWebInfo->_webRegisterCollectMap.find( nType );
		if( it == pWebInfo->_webRegisterCollectMap.end() )
		{
			//û���ҵ�����ô�ʹ���һ��
			pWebRegisterCollect = new webRegisterCollect();
			pWebInfo->_webRegisterCollectMap.insert( std::make_pair( nType, pWebRegisterCollect ) );

		}
		else
		{
			pWebRegisterCollect = it->second;
		}

		webRegister* pWebRegister = new webRegister();
		pWebRegister->_id = nID;
		pWebRegister->_item = nItem;
		pWebRegister->_url = strUrl;
		pWebRegister->_posturl = strPostUrl;
		pWebRegister->_type = nType;

		//��ȡheard��content
		pWebRegister->_strHead = strHeader;
		pWebRegister->_strPost = strContent;

		pWebRegister->_httptype = Posttype;
		pWebRegister->_validateUrl = strValidateurl;
		pWebRegister->_validateCode = "";
		pWebRegister->_success	= strSuccessRet;
		
		pWebRegister->_base = Basetype;
		pWebRegister->_loginBase = loginBase;
		pWebRegister->_strResultUrl = strResultUrl;
		pWebRegister->_strPubViewUrl= strPubViewUrl;
		
		pWebRegisterCollect->_webRegisterLst.push_back( pWebRegister );
	}

}


CWebInfo* CInfoManage::createWebInfo( CAdoRecordSet* pRs )
{
	CWebInfo* pWebInfo = new CWebInfo();

	int nID;
	CString	strName;
	CString	strUrl;
	CString strType;
	int		grade;
	CString	strUser;
	CString	strPass;
	CString registrTime;
	CString	state;
	CString	used;
	CString	remark;
	int		nUsed;
	int		nDemoUsed;
	int		nLanguage;
	
	pRs->GetCollect(_T("id"), nID);
	pRs->GetCollect(_T("name"), strName);
	pRs->GetCollect(_T("url"), strUrl);
	pRs->GetCollect(_T("type"), strType);
	pRs->GetCollect(_T("grade"), grade);
	pRs->GetCollect(_T("user"), strUser);
	pRs->GetCollect(_T("pass"), strPass);
	pRs->GetCollect(_T("registertime"), registrTime);
	pRs->GetCollect(_T("state"), state);
	pRs->GetCollect(_T("remark"), remark);

	pRs->GetCollect(_T("language"), nLanguage);
	pRs->GetCollect(_T("used"), nUsed);
	pRs->GetCollect(_T("demoused"), nDemoUsed);
	
	pWebInfo->_id = nID;
	pWebInfo->_strName = strName;
	pWebInfo->_strWebUrl = strUrl;
	pWebInfo->_strType = strType;
	pWebInfo->_grade = grade;
	pWebInfo->_strUser = strUser;
	pWebInfo->_strPass = strPass;
	pWebInfo->_strRegisterTime = registrTime;
	pWebInfo->_strState = state;
	pWebInfo->_strRemark = remark;

	pWebInfo->_language = nLanguage;
	pWebInfo->_used = nUsed;
	pWebInfo->_demoUsed = nDemoUsed;

	return pWebInfo;
}

CWebInfo* CInfoManage::createNewWebInfo( )
{
	
	CWebInfo* pWebInfo = new CWebInfo();
	_maxID ++;

	pWebInfo->_id = _maxID;

	_webInfoMap.insert( std::make_pair( pWebInfo->_id, pWebInfo ));

	return pWebInfo;
}


void CInfoManage::InitCategoryInfo( CategoryItem* &pLastItem, CAdoRecordSet* pRs ) 
{
	//

	CString		strID;
	CString		strCName;
	CString		strEName;


	pRs->GetCollect(_T("CategoryCode"), strID);
	pRs->GetCollect(_T("CName"), strCName);
	pRs->GetCollect(_T("EName"), strEName);
	
	int nLength = strID.GetLength();


	CategoryItem* pItem = new CategoryItem();
	pItem->_strID = strID;
	pItem->_pParentItem = NULL;
	pItem->_strCName = strCName;
	pItem->_strEName = strEName;

	if (  nLength == 2 )
	{
		
		//���ڵ�
		_categoryLst.push_back( pItem );
		pLastItem = pItem;

		return;
	}
	else
	{
		if ( !pLastItem )
			return ;

		int nLastLength = pLastItem->_strID.GetLength();
		if ( nLength >= nLastLength )
		{
			// ���ӽڵ�������ֵ�
			if ( nLastLength == nLength )
			{
				pLastItem->_pParentItem->_childItemLst.push_back( pItem );
				pItem->_pParentItem = pLastItem->_pParentItem;
				pLastItem = pItem;
				return;
			}
			else
			{
				pLastItem->_childItemLst.push_back( pItem );
				pItem->_pParentItem = pLastItem;
				pLastItem = pItem;
			}
		}
		else
		{
			// 
			while ( nLength != nLastLength )
			{
				pLastItem = pLastItem->_pParentItem;
				nLastLength = pLastItem->_strID.GetLength();
			}

			// ���������ڵ�Ĺ�ϵ���ֵ��ˡ�
			pLastItem->_pParentItem->_childItemLst.push_back( pItem );
			pItem->_pParentItem = pLastItem->_pParentItem;
			pLastItem = pItem;
			
			return;

		}
	}

}
void CInfoManage::Release()
{
	if ( _Release )
		return;


	_Release = TRUE;
	_Stop    = TRUE;

	ReleaseNoValidateThread();
	ReleaseValidateThread();

	DeleteCriticalSection(&_CsNoValidateLst);
	DeleteCriticalSection(&_CsValidateLst);
	DeleteCriticalSection(&_CsFinishLst);

	for( std::map<int, CWebInfo*>::iterator it = _webInfoMap.begin(); it!= _webInfoMap.end(); it++ )
	{
		delete it->second;
	}

	_webInfoMap.clear();


	for ( int i = 0; i < _categoryLst.size(); i++ )
	{
		CategoryItem*	pItem = _categoryLst[i];
		delete pItem;
	}

	_categoryLst.clear();

}

BOOL CInfoManage::InitNoValidateThread()
{
	_NoValidateHandleList.clear();
	for (int i = 0; i < NOVALIDATE_THREAD_DEFAULT; i++ )
	{
		DWORD tid;		
		HANDLE hThread = CreateThread(NULL, 0, NoValidateThread, this, 0, &tid);	
		_NoValidateHandleList.push_back( hThread );
	}
	return TRUE;
}

void CInfoManage::ReleaseNoValidateThread()
{
	for (int i = 0; i < (int)_NoValidateHandleList.size(); i++)
	{
		WaitForSingleObject(_NoValidateHandleList[i], 1000 * 30);
	}
}


DWORD CInfoManage::NoValidateThread(LPVOID pv)
{
	CInfoManage * pThis = (CInfoManage*) pv;
	pThis->WatchNoValidateThread();
	return 0;
}


void CInfoManage::WatchNoValidateThread()
{
	while ( !_Release )	
	{
		EnterCriticalSection( &_CsNoValidateLst );
		if ( _noValidateLst.size() > 0 && !_Stop )
		{
			ATLTRACE(_T("��ʼ�������� \r\n"));

			std::vector< CWebInfo* >::iterator  info = _noValidateLst.begin() + _noValidateLst.size() - 1;
			// simple to do use and erase

			//1 
			CWebInfo* pWebInfo = _noValidateLst[ _noValidateLst.size() - 1 ];


			//2 �������б�����ɾ��
			_noValidateLst.erase(info);

			//3 �˳�������
			LeaveCriticalSection(&_CsNoValidateLst);

			//4 ����
			PostWebInfo( pWebInfo );

		}
		else
			LeaveCriticalSection(&_CsNoValidateLst);

		Sleep(1);
	}
	return;
}



BOOL CInfoManage::InitValidateThread()
{
	_ValidateHandleList.clear();
	for (int i = 0; i < VALIDATE_THREAD_DEFAULT; i++ )
	{
		DWORD tid;		
		HANDLE hThread = CreateThread(NULL, 0, ValidateThread, this, 0, &tid);	
		_ValidateHandleList.push_back( hThread );
	}
	return TRUE;
}

void CInfoManage::ReleaseValidateThread()
{
	for (int i = 0; i < (int)_ValidateHandleList.size(); i++)
	{
		WaitForSingleObject(_ValidateHandleList[i], 1000 * 30);
	}
}


DWORD CInfoManage::ValidateThread(LPVOID pv)
{
	CInfoManage * pThis = (CInfoManage*) pv;
	pThis->WatchValidateThread();
	return 0;
}


void CInfoManage::WatchValidateThread()
{
	while ( !_Release )	
	{
		EnterCriticalSection( &_CsValidateLst );
		if ( _ValidateLst.size() > 0 && !_Stop )
		{
			ATLTRACE(_T("��ʼ�������� \r\n"));

			std::vector< CWebInfo* >::iterator  info = _ValidateLst.begin() + _ValidateLst.size() - 1;
			// simple to do use and erase

			//1 
			CWebInfo* pWebInfo = _ValidateLst[ _ValidateLst.size() - 1 ];


			//2 �������б�����ɾ��
			_noValidateLst.erase(info);

			//3 �˳�������
			LeaveCriticalSection(&_CsNoValidateLst);

			//4 ����
			PostWebInfo( pWebInfo );

		}
		else
			LeaveCriticalSection(&_CsNoValidateLst);

		Sleep(1);
	}
	return;
}


void CInfoManage::PostWebInfo( CWebInfo* pWebInfo )
{
	//  ���ж���û�г�ʼ�������û�г�ʼ�����ͳ�ʼ��
	if ( pWebInfo->_referLst.size() == 0 )
	{

		std::map<int, webRegisterCollect*>::iterator Iter = pWebInfo->_webRegisterCollectMap.find( _type );
		if( Iter == pWebInfo->_webRegisterCollectMap.end() )
		{
			pWebInfo->_result = RESULT_FAILED;
			return;
		}

		webRegisterCollect* pWebRegisterCollect = Iter->second;

		if ( pWebRegisterCollect->_webRegisterLst.size() >  0 )
		{
			int nSize = pWebRegisterCollect->_webRegisterLst.size();

			webRegister* pWebAdd = pWebRegisterCollect->_webRegisterLst[nSize - 1];

			webSearchItem searchItem;
			
			searchItem._type = pWebAdd->_type;
			searchItem._item = pWebAdd->_item;
			if ( pWebRegisterCollect->_webRegisterLst.size() > 1 )
				searchItem._base = pWebRegisterCollect->_webRegisterLst[nSize - 2]->_item;
			else
				searchItem._base = -1;

			searchItem._loginbase = LOGINBASE_NOUSED;
			
			if ( pWebAdd->_validateUrl == "" )
			{
				EnterCriticalSection( &_CsNoValidateLst );

				pWebInfo->_referLst.push_back( searchItem );
				_noValidateLst.push_back( pWebInfo );

				LeaveCriticalSection(&_CsNoValidateLst);
			}
			else
			{
				EnterCriticalSection( &_CsValidateLst );

				pWebInfo->_referLst.push_back( searchItem );
				_ValidateLst.push_back( pWebInfo );

				LeaveCriticalSection(&_CsValidateLst);
			}
		}

		return;
	}

	
	// 
	// ��ȡ��ע����Ϣ
	if ( pWebInfo->_referLst.size() == 0 )
	{
		pWebInfo->_result = RESULT_FAILED;
		return;
	}

	int nType = pWebInfo->_referLst[ pWebInfo->_referLst.size() - 1 ]._type ;
	int nItem = pWebInfo->_referLst[ pWebInfo->_referLst.size() - 1 ]._item ;
	int nBase = pWebInfo->_referLst[ pWebInfo->_referLst.size() - 1 ]._base ;
	int nLogin = pWebInfo->_referLst[ pWebInfo->_referLst.size() - 1 ]._loginbase;

	//pWebInfo->_referLst.erase( pWebInfo->_referLst.end() - 1 );

	// ����ͷ��������Ҫ������
	std::map<int, webRegisterCollect*>::iterator Iter = pWebInfo->_webRegisterCollectMap.find( nType );
	if( Iter == pWebInfo->_webRegisterCollectMap.end() )
	{
		pWebInfo->_result = RESULT_FAILED;
		return;
	}

	webRegisterCollect* pWebRegisterCollect = Iter->second;
	webRegister*		pWebRegister = NULL;

	// �������ҵ�Ҫ�ύ�Ĳ����ļ�¼
	for ( int i = 0; i < pWebRegisterCollect->_webRegisterLst.size();i ++ )
	{
		if ( pWebRegisterCollect->_webRegisterLst[i]->_item == nItem )
		{
			pWebRegister = pWebRegisterCollect->_webRegisterLst[i];
			break;
		}
	}

	if ( !pWebRegister )
	{
		pWebInfo->_result = RESULT_FAILED;
		return;
	}

	// �ȿ�����������Ƿ���Ҫ�Ƚ��������Ĳ�����������Ϣ�����Ĳ�����������Ҫ�Ƚ����û���¼

	if ( pWebRegister->_loginBase == 1 && nLogin == LOGINBASE_NOUSED ) // ��ʾ��Ҫ��¼
	{
		//��Ҫ�Ƚ���һ�������Ĳ��ܽ�����ȥ ��������Ҫ�Ƚ�����һ���Ĳ�����

		// 0 ������һ�£��´ξͲ�����������Ƿ���Ҫ��¼��

		pWebInfo->_referLst[ pWebInfo->_referLst.size() - 1 ]._loginbase = LOGINBASE_USED;
		
		// 1 �ҵ���Ҫ���е���һ������Ϣ
		
		std::map<int, webRegisterCollect*>::iterator IterLogin = pWebInfo->_webRegisterCollectMap.find( POST_LOGIN );
		if( IterLogin == pWebInfo->_webRegisterCollectMap.end() )
		{
			pWebInfo->_result = RESULT_FAILED;
			return;
		}

		webRegisterCollect* pWebRegisterCollect = IterLogin->second;

		if ( pWebRegisterCollect->_webRegisterLst.size() >  0 )
		{
			int nSize = pWebRegisterCollect->_webRegisterLst.size();

			webRegister* pWebAdd = pWebRegisterCollect->_webRegisterLst[nSize - 1];

			webSearchItem searchItem;

			searchItem._type = pWebAdd->_type;
			searchItem._item = pWebAdd->_item;
			if ( pWebRegisterCollect->_webRegisterLst.size() > 1 )
				searchItem._base = pWebRegisterCollect->_webRegisterLst[nSize - 2]->_item;
			else
				searchItem._base = -1;

			searchItem._loginbase = LOGINBASE_NOUSED;

			if ( pWebAdd->_validateUrl == "" )
			{
				EnterCriticalSection( &_CsNoValidateLst );

				pWebInfo->_referLst.push_back( searchItem );
				_noValidateLst.push_back( pWebInfo );

				LeaveCriticalSection(&_CsNoValidateLst);
			}
			else
			{
				EnterCriticalSection( &_CsValidateLst );

				pWebInfo->_referLst.push_back( searchItem );
				_ValidateLst.push_back( pWebInfo );

				LeaveCriticalSection(&_CsValidateLst);
			}
		}
	}
	else if ( nBase > -1 ) // ��ʾ��Ҫ�ȷ���һ����վ
	{
		//��Ҫ�Ƚ���һ�������Ĳ��ܽ�����ȥ ��������Ҫ�Ƚ�����һ���Ĳ�����

		// 0 ������һ�£��´ξͲ�����������Ƿ���Ҫ��¼��

		pWebInfo->_referLst[ pWebInfo->_referLst.size() - 1 ]._base = -1;

			// 1 �ҵ���Ҫ���е���һ������Ϣ

		std::map<int, webRegisterCollect*>::iterator IterLogin = pWebInfo->_webRegisterCollectMap.find( nType );
		if( IterLogin == pWebInfo->_webRegisterCollectMap.end() )
		{
			pWebInfo->_result = RESULT_FAILED;
			return;
		}

		webRegisterCollect* pWebRegisterCollect = IterLogin->second;

		for  ( int i = 0; i <  pWebRegisterCollect->_webRegisterLst.size(); i++ )
		{

			webRegister* pWebAdd = pWebRegisterCollect->_webRegisterLst[ i ];

			if ( pWebAdd->_item == nItem )
			{

				webSearchItem searchItem;

				searchItem._type = pWebAdd->_type;
				searchItem._item = pWebAdd->_item;
				if ( i > 0 )
					searchItem._base = pWebRegisterCollect->_webRegisterLst[ i - 1 ]->_item;
				else
					searchItem._base = -1;

				searchItem._loginbase = LOGINBASE_NOUSED;

				if ( pWebAdd->_validateUrl == "" )
				{
					EnterCriticalSection( &_CsNoValidateLst );

					pWebInfo->_referLst.push_back( searchItem );
					_noValidateLst.push_back( pWebInfo );

					LeaveCriticalSection(&_CsNoValidateLst);
				}
				else
				{
					EnterCriticalSection( &_CsValidateLst );

					pWebInfo->_referLst.push_back( searchItem );
					_ValidateLst.push_back( pWebInfo );

					LeaveCriticalSection(&_CsValidateLst);
				}

			}

			
		}

	}
	else
	{
		//�����ⲽ�ύ�Ϳ�����
		ProcessWebInfo( pWebRegister, pWebInfo );
		// ��ӵ�����б���
	}
}



BOOL CInfoManage::ProcessWebInfo( webRegister* pWebRegister, CWebInfo* pWebInfo )
{
	// 1 �ж��Ƿ���Ҫ��֤��
	if ( pWebRegister->_validateUrl != "" )
	{
		// ��Ҫ����֤�룬 ����ֻ�ǵ���һ�����ڣ���ʾ��֤�룬���û����롣
		CValidateDlg dlg;
		dlg._strUrl = pWebRegister->_validateUrl ;
		if ( dlg.DoModal() != IDOK )
		{
			pWebInfo->_result = RESULT_FAILED;
			return FALSE;
		}
		pWebRegister->_validateCode = dlg._validateCode;

		//�ж�httpDown
	}

	// ��Ҫ��һЩ�û��ı����滻Ϊʵ�ʵ�
	CHttpDown	httpDown;
	httpDown.Init( this, *pWebRegister );
	httpDown.StartDown();
	if (  httpDown.IsSuccess( pWebRegister->_success ) )
	{
		pWebInfo->_result = RESULT_SUCCESS;
		return TRUE;
	}
	else
	{
		pWebInfo->_result = RESULT_FAILED;
		return FALSE;
	}
	//return TRUE;
}