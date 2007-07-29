// HttpDown.cpp: implementation of the CHttpDown class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "HttpDown.h"

#include "util.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CHttpDown::CHttpDown()
{
	_pInfoManage	= NULL;
	// _buff			= "";
}

CHttpDown::~CHttpDown()
{
	
}

void CHttpDown::Init( CInfoManage* pInfoManage, webRegister& webReg  )
{

	_pInfoManage = pInfoManage;
	httpClient.ClearParam();
	httpClient.SetInternet(_T("Mozilla/4.0 (compatible; MSIE 6.0; Windows NT 5.1; SV1; .NET CLR 1.1.4322)"));

	for( PARAM_ITERATOR it = webReg._headMap.begin(); it!= webReg._headMap.end(); it++ )
	{
		if (it->second.GetLength() > 0)
			httpClient.SetHeader(it->first, it->second);
	}

	// Add user's custom headers
	//httpClient.SetHeader (_T ("Lyoul-Magic"), _T ("My Magic Header")) ;
	//httpClient.SetHeader (_T ("User-Magic"), _T ("User's Magic Header")) ;
	for( PARAM_ITERATOR it = webReg._postMap.begin(); it!= webReg._postMap.end(); it++ )
	{
		if (it->second.GetLength() > 0)
			httpClient.AddParam(it->first, it->second, CHttpClient::ParamNormal);
	}

	_Url = webReg._url;
	_method	= (HttpMethod) webReg._httptype;	
}

inline BOOL  CHttpDown::CheckOperationStat (CInfoManage* pInfoManage)// throw (DWORD)
{

	return FALSE;
}
void CHttpDown::StartDown()
{
	CHttpResponse*	pobjRes = NULL;	
	CHttpPostStat	objPostStat;
	const DWORD		cbStep = 1024;
	BOOL		bUtf =	FALSE;
	//TRACE("开始下载网页:%s\r\n", _Url.GetBuffer(_Url.GetLength()));
	TRACE("开始下载网页:%s\r\n", _Url);
	try
	{
		httpClient.SetUseUtf8 (static_cast<BOOL> (FALSE)) ;
	
		if (_method == HTTP_POST)
		{
			httpClient.BeginPost(_Url);
		}
		else if (_method == HTTP_GET)
		{
			pobjRes = httpClient.RequestGet(_Url);
		}

		
		while (pobjRes == NULL) 
		{
			if (CheckOperationStat(_pInfoManage))
			{
				goto _exit;
			}
			httpClient.Query(objPostStat);						
			pobjRes = httpClient.Proceed(cbStep);
		}

		CString		str ;
		str.Format (_T ("%u %s\r\n"), pobjRes->GetStatus (), pobjRes->GetStatusText ()) ;
		TRACE(str);
		
		static LPCTSTR		szHeaders[] = 
		{ _T ("Server"), _T ("Date"), _T ("X-Powered-By"), _T ("Content-Length"), _T ("Set-Cookie")
		, _T ("Expires"), _T ("Cache-control"), _T ("Connection"), _T ("Transfer-Encoding")
		, _T ("Content-Type"), _T ("") } ;

		LPCTSTR		szHeader ;

		for (size_t i = 0; i < sizeof (szHeaders) / sizeof (LPCTSTR); i++) 
		{
			if (CheckOperationStat(_pInfoManage))
			{
				goto _exit;
			}
			if ( szHeader = pobjRes->GetHeader (szHeaders[i]) ) 
			{
				str.Format (_T ("%s: %s\r\n"), szHeaders[i], szHeader) ;
				//TRACE(str);
				//if (_pHttpSearch)
				//	_pHttpSearch->AddBuffer(str);
			}
		}

		BOOL		bIsText = TRUE ;
		if ( szHeader = pobjRes->GetHeader (_T ("Content-Type")) )
		{
			//bIsText = (0 == ::_tcsncicmp (szHeader, _T ("text/"), 5)) ;
			CString strHeader = szHeader;
			if (strHeader.Find(_T("utf-8")) > 0)
				bUtf = TRUE;
		}
		DWORD		dwContSize ;
		if ( !pobjRes->GetContentLength (dwContSize) )
			dwContSize = 0 ;


		TRACE("Reading HTTP response ---> %d/%d\r\n", 0, dwContSize) ;

		const DWORD		cbBuff = 1024 * 10 ;
		BYTE			byBuff[cbBuff] ;
		DWORD			dwRead ;
		size_t			cbSize = 0 ;

		while ( dwRead = pobjRes->ReadContent (byBuff, cbBuff - 1) ) 
		{
			if (CheckOperationStat( _pInfoManage ))
			{
				goto _exit;
			}
				
			cbSize += dwRead ;
			TRACE("Reading HTTP response ---> %d/%d\r\n", cbSize, dwContSize) ;
			if ( bIsText ) 
			{
				byBuff[dwRead] = '\0' ;			
				str = reinterpret_cast<LPCSTR> (byBuff) ;
				int nStrLen = str.GetLength();
				while ( nStrLen < dwRead )
				{
					byBuff[nStrLen] = ' ';
					str = reinterpret_cast<LPCSTR> (byBuff) ;
					nStrLen = str.GetLength();
				}
				_buff += str;				
				//str.ReleaseBuffer();
				str = "";
			}
		}

		if ( !bIsText ) 
		{
			str.Format (_T ("%u bytes skipped..\r\n"), cbSize) ;
			TRACE(str);			
		}
		TRACE("-----Reading HTTP response Complete---> %d/%d\r\n", cbSize, dwContSize) ;
	}

	catch (DWORD & nOpStat) 
	{
		if (pobjRes)
			delete pobjRes ;
		pobjRes = NULL;
		httpClient.Cancel() ;
		TRACE("\r\n\r\nOperation canceled by user\r\n");
		// If the program is going to terminate,
		//if ( nOpStat & CCHttpClientDemoDlg::TERMINATING )
		//	return 0 ;

		// canceled by user
		//pobjDlg->SetTextMsg (_T ("\r\n\r\nOperation canceled by user\r\n")) ;
		//::PostMessage (pobjDlg->GetSafeHwnd (), WM_THREADFINISHED, 0, 0) ;
		//_pHttpSearch->StopSearch();
		return ;
	}
	catch (CHttpClient::Exception & ) 
	{
		if (pobjRes)
			delete pobjRes;
		pobjRes = NULL;
		httpClient.Cancel();
		//_pHttpSearch->StopSearch();
		return ;
	}
	
	
	if (pobjRes)
		delete pobjRes;
	pobjRes = NULL;

	if ( bUtf )
	{
        ATLASSERT(FALSE);
		// _buff = Convert(_buff, CP_UTF8, 936);
	}

	return;

_exit:
	if (pobjRes)
		delete pobjRes;
	pobjRes = NULL;
	httpClient.Cancel();
	//if (_pHttpSearch)
	//	_pHttpSearch->AddBuffer("", TRUE);
	return ;
	//_pHttpSearch->AnalyseDate();
}


BOOL CHttpDown::IsSuccess( CString strKey )
{
	if ( _buff.Find( strKey ) > 0 )
		return TRUE;
	else
		return FALSE;
}
