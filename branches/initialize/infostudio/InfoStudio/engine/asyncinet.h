// AsyncInet.h :
//

#ifndef ASSERT
// #include <cassert>
  #define ASSERT ATLASSERT
#endif


// #include "asyncnotify.h"
struct AsyncNotify
{
	virtual ~AsyncNotify(){}
	virtual void Notify(){}
};

struct EventNotify : public AsyncNotify
{
	EventNotify() 
	{
		_hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	}
	~EventNotify()
	{
		CloseHandle(_hEvent);
	}
	void Notify()
	{
		SetEvent(_hEvent);
	}
	HANDLE _hEvent;
};


#include <wininet.h>
#pragma comment(lib, "wininet")

#include "urlhlp.h"


// Bug:InternetReadFileEx 的参数 dwContext 是在InternetStatusCallback中的 dwContext
// http://groups.google.com/group/microsoft.public.inetsdk.programming.wininet/browse_thread/thread/d6dadafac4c44f19/b4767d533e289e85?lnk=st&q=InternetReadFileEx&rnum=5&hl=zh-CN#b4767d533e289e85

// Sample: Using HttpSendRequestEx for Large POST Requests
// http://support.microsoft.com/kb/q177188/

// "InternetReadFileEx Solution Found!" -- 可能是正确的。
// http://groups.google.com/group/microsoft.public.inetsdk.programming.wininet/browse_thread/thread/5ef305762a8d792b/5906457ca81bd8cb?lnk=st&q=InternetReadFileEx&rnum=12&hl=zh-CN#5906457ca81bd8cb

// http://groups.google.com/groups?q=InternetReadFileEx&start=10&hl=zh-CN&ie=UTF-8&

// KB165800 How to Use URL Moniker to POST Data


// 输出信息级别
// 3 全部包括所有的callback
// 2 Response Received
// 1 url, timeout dump
#define VERBOSE_LEVEL 3

#define DBG

#ifdef DBG
std::string GetStatusText(DWORD dwStatus)
{
	std::map<DWORD, std::string> status_map;
#define Assign(s, v) status_map.insert( std::make_pair(v, std::string(#s)) );
	Assign( INTERNET_STATUS_RESOLVING_NAME,			 10)
		Assign( INTERNET_STATUS_NAME_RESOLVED,           11)
		Assign( INTERNET_STATUS_CONNECTING_TO_SERVER,    20)
		Assign( INTERNET_STATUS_CONNECTED_TO_SERVER,     21)
		Assign( INTERNET_STATUS_SENDING_REQUEST,         30)
		Assign( INTERNET_STATUS_REQUEST_SENT,            31)
		Assign( INTERNET_STATUS_RECEIVING_RESPONSE,      40)
		Assign( INTERNET_STATUS_RESPONSE_RECEIVED,       41)
		Assign( INTERNET_STATUS_CTL_RESPONSE_RECEIVED,   42)
		Assign( INTERNET_STATUS_PREFETCH,                43)
		Assign( INTERNET_STATUS_CLOSING_CONNECTION,      50)
		Assign( INTERNET_STATUS_CONNECTION_CLOSED,       51)
		Assign( INTERNET_STATUS_HANDLE_CREATED,          60)
		Assign( INTERNET_STATUS_HANDLE_CLOSING,          70)
		Assign( INTERNET_STATUS_DETECTING_PROXY,         80)
		Assign( INTERNET_STATUS_REQUEST_COMPLETE,        100)
		Assign( INTERNET_STATUS_REDIRECT,                110)
		Assign( INTERNET_STATUS_INTERMEDIATE_RESPONSE,   120)
		Assign( INTERNET_STATUS_USER_INPUT_REQUIRED,     140)
		Assign( INTERNET_STATUS_STATE_CHANGE,            200)
		Assign( INTERNET_STATUS_COOKIE_SENT,             320)
		Assign( INTERNET_STATUS_COOKIE_RECEIVED,         321)
		Assign( INTERNET_STATUS_PRIVACY_IMPACTED,        324)
		Assign( INTERNET_STATUS_P3P_HEADER,              325)
		Assign( INTERNET_STATUS_P3P_POLICYREF,           326)
		Assign( INTERNET_STATUS_COOKIE_HISTORY,          327)
#undef Assign
		return status_map[dwStatus];
}
#endif





enum RequestState 
{
	STATE_OPEN,
	STATE_HANDLE_CREATE,
	STATE_READ_REQUEST, // 调用了 InternetReadFileEx,但没有完成
	STATE_READ, // 调用 InternetReadFileEx 没有更多的内容读取了
	STATE_CLOSE,
	STATE_ERROR,
};

class AsyncRequest
{
public:
	AsyncRequest(LPCTSTR szUrl, AsyncNotify* pan = 0) : _hInet(0), _pszUrl(0), _lpBuffer(0)
		, _dwBufferLen(0), _dwReaded(0)
		, _pan(pan), _state(STATE_ERROR)
		, _fNameResolved(false), _fFinished(false)
	{
		_pszUrl = new TCHAR[lstrlen(szUrl) + 1];
		if(_pszUrl) lstrcpy(_pszUrl, szUrl);

		ZeroMemory(&_ib, sizeof(INTERNET_BUFFERS));
		_ib.dwStructSize = sizeof(INTERNET_BUFFERS);
#if defined(VERBOSE_LEVEL) && (VERBOSE_LEVEL > 0)
		std::cout << "AsyncRequest: " << szUrl << "\n";
#endif
	}
	~AsyncRequest()
	{
		if(_hInet) InternetCloseHandle(_hInet);
		delete [] _pszUrl;

		// TODO: 关闭问题。
		if(_lpBuffer)
			Free(_lpBuffer);
	}
	BOOL Redirect(LPCTSTR szUrl, DWORD dwUrlLength)
	{
		delete [] _pszUrl;
		_pszUrl = new TCHAR[dwUrlLength + 1];
		if(_pszUrl) 
			lstrcpyn(_pszUrl, szUrl, dwUrlLength + 1);

#if defined(VERBOSE_LEVEL) && (VERBOSE_LEVEL > 0)
		std::cout << "Redirect:" << szUrl << "\n";
#endif
		return TRUE;
	}
	BOOL NameResolved(LPCTSTR pszIpAddress)
	{
		_fNameResolved = true;
#if defined(VERBOSE_LEVEL) && (VERBOSE_LEVEL > 0)
		std::cout << "NameResolved:" << pszIpAddress << "\n";
#endif
		return TRUE;
	}

	BOOL ResponseReceived(DWORD dwBytes)
	{
#if defined(VERBOSE_LEVEL) && (VERBOSE_LEVEL > 1)
		std::cout << " ResponseReceived:" << dwBytes << " " << _pszUrl << "\n";
#endif
		return TRUE;
	}

	BOOL RequestComplete(DWORD dwResult, DWORD dwError)
	{
#if defined(VERBOSE_LEVEL) && (VERBOSE_LEVEL > 2)
		std::cout << "RequestComplete:" << (void*)dwResult << ", Error:" << dwError << ", State:" << _state << "\n";
#endif
		if(dwError != 0)
		{
			_state = STATE_ERROR;
			_dwError = dwError;
			Finished();
			return TRUE;
		}

		ASSERT(_state != STATE_ERROR);

		if(!_hInet)
			return TRUE;

		if(_state == STATE_READ)
		{
			ASSERT(false);
			Finished();
			return TRUE;
		}

		// TODO: split to function

#if 0   // 应该不能相信Content-Length的值
		char szContentLength[64];
		DWORD dwSize = 64;
		BOOL rt = HttpQueryInfo(_hInet, HTTP_QUERY_CONTENT_LENGTH, szContentLength, &dwSize, NULL);
		if(!rt)
		{
			szContentLength[0] = '\0';
			// TODO:没有Content-Length的情况。
			return FALSE;
		}

		long nLength = atol(szContentLength);

		// Not support file > 100M
		if(nLength > 100 * 1024 *1024)
			return FALSE;
#endif

		if(_state != STATE_READ_REQUEST)
		{
			ASSERT(_dwBufferLen == 0);
			ASSERT(_lpBuffer == 0);
		}

		BOOL rt = FALSE;

		do{
			DWORD buf_grow_size = 0x2000;

			if(_state == STATE_READ_REQUEST)
			{
				ASSERT(_ib.dwBufferLength <= buf_grow_size);
				ASSERT(_ib.dwBufferLength <= (_dwBufferLen - _dwReaded));
				_dwReaded += _ib.dwBufferLength; // CAUTION:
#if defined(VERBOSE_LEVEL) && (VERBOSE_LEVEL > 2)
				std::cout << "_ib.dwBufferLength 1: " << _ib.dwBufferLength << " TotalRead: " << _dwReaded << "\n";
#endif
				_state = STATE_OPEN;
			}

			LPBYTE p = Alloc(_dwBufferLen + buf_grow_size, _lpBuffer);
			ASSERT(p);
			if(!p) break;
			_lpBuffer = p;
			_dwBufferLen += buf_grow_size;

			ZeroMemory(&_ib, sizeof(INTERNET_BUFFERS));
			_ib.dwStructSize = sizeof(INTERNET_BUFFERS);

			_ib.lpvBuffer = _lpBuffer + _dwReaded;
			_ib.dwBufferLength = _dwBufferLen - _dwReaded;

			rt = InternetReadFileEx(_hInet, &_ib, IRF_ASYNC, (LPARAM)0);
			if(!rt)
				break;

#if defined(VERBOSE_LEVEL) && (VERBOSE_LEVEL > 2)
			std::cout << "_ib.dwBufferLength 2: " << _ib.dwBufferLength << " TotalRead: " << _dwReaded << "\n";
#endif
			_dwReaded += _ib.dwBufferLength; // CAUTION: 
			// if(_ib.dwBufferLength < buf_grow_size)
			if( 0 == _ib.dwBufferLength ) // 只有为0才能最终确定没有数据了，http://sf.net
				break;

		} while(_ib.dwBufferLength);

		if( !rt )
		{
			if( GetLastError()==ERROR_IO_PENDING )
			{
				_state = STATE_READ_REQUEST;
#if defined(VERBOSE_LEVEL) && (VERBOSE_LEVEL > 2)
				std::cout << "__ State: STATE_READ_REQUEST\n";
#endif
			}
			else
				_state = STATE_ERROR;
		}
		else
		{
			_state = STATE_READ;
			Finished();
		}

		return TRUE;
	}

	void RequestCalled()
	{
		_state = STATE_OPEN;
	}

	void HandleCreated(HINTERNET hInet)
	{
		_hInet = hInet;

		ASSERT(_state == STATE_OPEN);
		_state = STATE_HANDLE_CREATE;

		ASSERT(!_lpBuffer);
	}

	void HandleClosing()
	{
		ASSERT(_state == STATE_HANDLE_CREATE || _state == STATE_READ);
		_state = STATE_CLOSE;
		_hInet = 0;
	}

	BOOL QueryInfo(DWORD dwInfoLevel,LPVOID pBuffer,LPDWORD pdwBufferLength,LPDWORD pdwIndex=NULL) const
	{
		ATLASSERT(_hInet);
		return HttpQueryInfo(_hInet, dwInfoLevel, pBuffer, pdwBufferLength, pdwIndex);
	}
public:
	HINTERNET Handle() const { return _hInet; }
	LPCTSTR url() const { return _pszUrl; }
	bool IsNameResolved() const { return _fNameResolved; }
	bool IsFinished() const { return _fFinished; }
	LPBYTE BufferPtr() const { return _lpBuffer; }
	DWORD BufferLength() const { return _dwReaded; }

	LPBYTE ReleseBuffer()
	{
		LPBYTE rt = _lpBuffer;
		_lpBuffer = 0;
		return rt;
	}
	RequestState State() const { return _state; }
private:
	LPBYTE Alloc(DWORD dwBytes, LPBYTE lpMem = 0)
	{
		LPVOID p = HeapAlloc(GetProcessHeap(), 0, dwBytes);
		if(lpMem)
		{
			ASSERT(dwBytes > HeapSize(GetProcessHeap(), 0, lpMem));
			CopyMemory(p, lpMem, HeapSize(GetProcessHeap(), 0, lpMem));
			BOOL rt = HeapFree(GetProcessHeap(), 0, lpMem);
			ASSERT(rt);
		}
		return (LPBYTE)p;
	}

	BOOL Free(LPBYTE lpMem)
	{
		return HeapFree(GetProcessHeap(), 0, lpMem);
	}

	void Finished()
	{
		ASSERT(_dwBufferLen > _dwReaded);
		ASSERT(_state == STATE_READ || _state == STATE_ERROR);
#if defined(VERBOSE_LEVEL) && (VERBOSE_LEVEL > 1)
		std::cout << "Finished: " << _pszUrl << " Length:" << _dwBufferLen << " State:" << _state << "\n";
#endif
		if(_pan) 
		{
			_pan->Notify();
			_fFinished = true;
		}
	}
private:
	RequestState  _state;
	DWORD _dwError; // _state 为 STATE_ERROR时，为 INTERNET_STATUS_REQUEST_COMPLETE 返回的错误代码
	HINTERNET _hInet;
	LPTSTR _pszUrl;

	DWORD _dwBufferLen, _dwReaded;
	LPBYTE _lpBuffer;

	INTERNET_BUFFERS _ib;

	AsyncNotify * _pan;

	bool _fNameResolved;
	bool _fFinished;
	friend class AsyncInet;
};


class AsyncInet
{
public:
	AsyncInet() : _hInet(0){}
	~AsyncInet()
	{
		BOOL f = Close();
		ASSERT(f);
	}

	DWORD GetLastError() { return ::GetLastError(); }
	BOOL GetLastErrorInfo(LPDWORD lpdwError, LPTSTR lpszBuffer, LPDWORD lpdwBufferLength)
	{
		return InternetGetLastResponseInfo(lpdwError, lpszBuffer, lpdwBufferLength);
	}

	BOOL Open(LPCTSTR lpszAgent = NULL, DWORD dwAccessType = INTERNET_OPEN_TYPE_PRECONFIG, 
		LPCTSTR lpszProxyName = NULL, LPCTSTR lpszProxyBypass = NULL,
		DWORD dwFlag = INTERNET_FLAG_ASYNC)
	{
		_hInet = InternetOpen(lpszAgent, dwAccessType, lpszProxyName, lpszProxyBypass, dwFlag);
		ASSERT(_hInet);
		if(_hInet && dwFlag & INTERNET_FLAG_ASYNC)
		{
			InternetSetStatusCallback(_hInet, InternetStatusCallback);
		}
		return _hInet != NULL;
	}
	BOOL Close()
	{
		return _hInet ? InternetCloseHandle(_hInet) : TRUE;
	}

	// we always request keep-alive, use an existing connection (FTP) and
	// passive mode transfers (also FTP)
	// lpBuffer: 下载后的内容的地址。
	BOOL Request(LPCSTR szUrl, LPBYTE * lpBuffer, LPDWORD lpdwBufferLen, LPCTSTR lpszHeaders = NULL, 
		DWORD dwFlags = INTERNET_FLAG_KEEP_CONNECTION | INTERNET_FLAG_EXISTING_CONNECT | INTERNET_FLAG_PASSIVE)
	{
		ASSERT(szUrl && lpBuffer && lpdwBufferLen);
		if(!lpBuffer || lpdwBufferLen || szUrl)
			return FALSE;

		*lpBuffer = 0;
		*lpdwBufferLen = 0;

		EventNotify en;

		AsyncRequest ar(szUrl, &en);
		HINTERNET hOpen = InternetOpenUrl(_hInet, szUrl, lpszHeaders, 0, dwFlags, (DWORD_PTR)&ar);
		if( !hOpen && (GetLastError() != ERROR_IO_PENDING) )
		{
			return FALSE;
		}
		ar.RequestCalled();

		WaitForSingleObject(en._hEvent, INFINITE);

		*lpBuffer = ar.ReleseBuffer();
		*lpdwBufferLen = ar.BufferLength();

		return TRUE;
	}

	AsyncRequest * Request(LPCTSTR szUrl, AsyncNotify * pan, LPCTSTR lpszHeaders = NULL, 
		DWORD dwFlags = INTERNET_FLAG_KEEP_CONNECTION | INTERNET_FLAG_EXISTING_CONNECT | INTERNET_FLAG_PASSIVE, 
		DWORD_PTR dwContext = 0)
	{
		ASSERT(szUrl);

		AsyncRequest * p = new AsyncRequest(szUrl, pan);
		HINTERNET hOpen = InternetOpenUrl(_hInet, szUrl, lpszHeaders, 0, dwFlags, (DWORD_PTR)p);
		if( !hOpen && (GetLastError() != ERROR_IO_PENDING) )
		{
			delete p;
			p = 0;
		}
		p->RequestCalled();
		return p;
	}
	AsyncRequest * RequestEx(LPCTSTR szUrl, AsyncNotify * pan, LPCTSTR lpszHeaders = NULL, 
		DWORD dwFlags = INTERNET_FLAG_KEEP_CONNECTION | INTERNET_FLAG_EXISTING_CONNECT | INTERNET_FLAG_PASSIVE,
		DWORD_PTR dwContext = 0)
	{
		ASSERT(_hInet);

		UrlComponet url;
		if(!url.Crack(szUrl))
			return 0;

		AsyncRequest * p = new AsyncRequest(szUrl, pan);

		HINTERNET hConn = InternetConnect( _hInet, url.lpszHostName, url.nPort, 
				url.lpszUserName, url.lpszPassword, url.nScheme, // INTERNET_SERVICE_HTTP )
				INTERNET_FLAG_PASSIVE, (DWORD_PTR)p);
		ASSERT(hConn);

		if(!hConn)
			return 0;

		LPCTSTR szAcceptType[] = { _T("*.*"), 0 };

		HINTERNET hReq = HttpOpenRequest(hConn, _T("HEAD"), url.lpszUrlPath, 
			NULL, // Version
			NULL, // Referrer
			szAcceptType,
			INTERNET_FLAG_NO_UI,
			(DWORD_PTR)p);
		ASSERT(hReq);

		HttpSendRequest(hReq, lpszHeaders, 0, NULL, 0);

		return p;
	}

	// Post
	// Head
private:
	void static CALLBACK InternetStatusCallback(HINTERNET hInternet, DWORD_PTR dwContext, DWORD dwInternetStatus,
		LPVOID lpvStatusInformation, DWORD dwStatusInformationLength)
	{
		AsyncRequest * p = (AsyncRequest*)dwContext;
#if defined(VERBOSE_LEVEL) && (VERBOSE_LEVEL > 2)
		std::cout << (void*)p->Handle() << " [" << GetCurrentThreadId() << "] :" 
			<< dwInternetStatus << " " << GetStatusText(dwInternetStatus) << " State:" << p->_state << "\n";
#endif
		if(!p) return;

		if(INTERNET_STATUS_HANDLE_CREATED == dwInternetStatus )
		{
			p->HandleCreated( (HINTERNET)((INTERNET_ASYNC_RESULT*)lpvStatusInformation)->dwResult );
		}
		else if( INTERNET_STATUS_REDIRECT == dwInternetStatus )
		{
			p->Redirect((LPCTSTR)lpvStatusInformation, dwStatusInformationLength);
		}
		else if( INTERNET_STATUS_RESPONSE_RECEIVED == dwInternetStatus )
		{
			p->ResponseReceived(*(DWORD *)lpvStatusInformation);
		}
		else if( INTERNET_STATUS_REQUEST_COMPLETE == dwInternetStatus )
		{
			p->RequestComplete(
				((INTERNET_ASYNC_RESULT*)lpvStatusInformation)->dwResult, 
				((INTERNET_ASYNC_RESULT*)lpvStatusInformation)->dwError
				);
		}
		else if(INTERNET_STATUS_NAME_RESOLVED == dwInternetStatus)
		{
			p->NameResolved((LPCTSTR)lpvStatusInformation);
		}
		//		else if(INTERNET_STATUS_HANDLE_CLOSING == dwInternetStatus)
		//		{
		//			p->HandleClosing();
		//		}
	}
private:
	HINTERNET _hInet;
};


std::string url2file(const char * url, LPCSTR target_dir)
{
	std::string s(url);
	s = s.substr(s.find("://") + 3);
	s.insert(0, target_dir);

	std::string::size_type pos = s.find_first_of("/?=");
	while( pos != std::string::npos )
	{
		s.replace(pos, 1, "_");

		pos = s.find_first_of("/?=");
	}
	return s;
}

void Test_Download(AsyncInet & inet, LPCTSTR url, LPCSTR target_dir)
{
#if 0
	std::string s(url);
	s = s.substr(s.find("://") + 3);
	s.insert(0, target_dir);

	//s.find_fir('/', '_');
	std::string::size_type pos = s.find_first_of('/');
	while( pos != std::string::npos )
	{
		s.replace(pos, 1, "_");

		pos = s.find_first_of('/');
	}

	char * p = 0;
	DWORD len;
	if(inet.Request(url, (LPVOID*)&p, &len))
	{
		ASSERT( !IsBadWritePtr(p, len) );

		HANDLE h = CreateFile(s.c_str(), GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		ASSERT(h != INVALID_HANDLE_VALUE);
		DWORD dwWrited = 0;
		WriteFile(h, p, len, &dwWrited, NULL);
		CloseHandle(h);

		HeapFree(GetProcessHeap(), 0, p);
	}
#if defined(VERBOSE_LEVEL) && (VERBOSE_LEVEL > 0)
	else
	{
		Beep(2000, 100);
		std::cout << url << " Error:" << GetLastError() << "\n";
	}
#endif
#endif
}

void Test_Sync(LPCTSTR* urls, int urlcount, AsyncInet & inet)
{
	for(int i=0; i<urlcount; i++)
	{
		LPBYTE pBuffer = 0;
		DWORD dwLength = 0;
		if( inet.Request(urls[i], &pBuffer, &dwLength) )
		{
			std::string file_name = url2file(urls[i], "d:\\t\\");

			file_name += ".htm";

			HANDLE h = CreateFile(file_name.c_str(), GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
			ASSERT(h != INVALID_HANDLE_VALUE);
			DWORD dwWrited = 0;
			WriteFile(h, pBuffer, dwLength, &dwWrited, NULL);
			CloseHandle(h);
		}
	}
}

struct AsyncResult{
	AsyncRequest * par;
} gAsyncResult[20];


void Test_Async(LPCTSTR* urls, const int urlcount, AsyncInet & inet)
{
	EventNotify* pen = new EventNotify;
	int i;
	for(i=0; i<urlcount; i++)
	{
		gAsyncResult[i].par = inet.Request(urls[i], pen);
		ASSERT(gAsyncResult[i].par);
	}

	int n = 0;
	while(n<urlcount)
	{
		if(WAIT_OBJECT_0 == WaitForSingleObject(pen->_hEvent, 1000 * 6))
		{
			n++;
			Beep(1000, 5);
		}
#if defined(VERBOSE_LEVEL) && (VERBOSE_LEVEL > 0)
		else
		{
			for(int k=0; k<urlcount; k++)
				std::cout << std::setw(2) << k 
				<< " f:" << gAsyncResult[k].par->IsFinished()
				<< " s:" << gAsyncResult[k].par->State() 
				<< " l:" << std::setw(10) << gAsyncResult[k].par->BufferLength()
				<< " r:" << gAsyncResult[k].par->IsNameResolved() 
				<< " " << ( gAsyncResult[k].par->url() ? gAsyncResult[k].par->url() : "(null)")
				<< "\n";

			std::cout << "\n";
		}
#endif
	}

	for(i=0; i<urlcount; i++)
	{
		std::string file_name = url2file(urls[i], "d:\\t\\");

		file_name += ".htm";

		HANDLE h = CreateFile(file_name.c_str(), GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		ASSERT(h != INVALID_HANDLE_VALUE);
		DWORD dwWrited = 0;
		if(gAsyncResult[i].par->BufferPtr())
			WriteFile(h, gAsyncResult[i].par->BufferPtr(), gAsyncResult[i].par->BufferLength(), &dwWrited, NULL);
		CloseHandle(h);

		delete gAsyncResult[i].par;
	}

	Beep(1000, 100);
}

void Test_1()
{
	std::cout << "GetCurrentThreadId:" << GetCurrentThreadId() << std::endl;
	{
		AsyncInet inet;	
		inet.Open();

		LPCTSTR urls[] = 
		{
			"http://musicdata.dudu.com/search.php?id=mf7w0fxzIVfTFFa9",
				"http://www.codeproject.com",
				"http://news.sohu.com",
				"http://news.163.com",
				"http://www.google.com/intl/zh-CN/about.html",
				"http://www.google.com", // redirect
				"http://www.google.com/images/google_80wht.gif",
				"http://pack.google.com",
				"http://163.com",
				"http://ken.com",
				"http://bug/bugzilla",
				"http://news.sina.com.cn",
				"http://download.kuho.com/kuho/kuho.exe",
				"http://labs.google.com/",
				"http://reader.google.com/",
				"http://sourceforge.net/projects/libtorrent/",
				"http://sf.net/",
				"http://www.boost.org/",
		};

		// Test_Sync(urls, sizeof(urls)/sizeof(*urls), inet);

		Test_Async(urls, sizeof(urls)/sizeof(*urls), inet);

		std::cout << "Input command(quit for exit): \n";
		do{
			std::string str_input;
			std::cin >> str_input;
			if(str_input == "quit") break;
		}while(true);
	}
}

void Test_Ex()
{
	AsyncInet inet;
	inet.Open();

	EventNotify en;
	AsyncRequest * par = inet.RequestEx("http://www.google.com", &en);
	// AsyncRequest * par = inet.RequestEx("http://localhost/post.html", &en);

	// http://localhost/post.html

	WaitForSingleObject(en._hEvent, INFINITE);

	TCHAR szHeaders[0x1000];
	DWORD len = 0x1000;
	DWORD index = 0;
	BOOL rt = FALSE;
	do{
		len = 0x1000;
		rt = par->QueryInfo(HTTP_QUERY_RAW_HEADERS_CRLF, szHeaders, &len, &index);
		index++;
	} while(rt);

	// ERROR_HTTP_HEADER_NOT_FOUND 

	__asm int 3;
}

