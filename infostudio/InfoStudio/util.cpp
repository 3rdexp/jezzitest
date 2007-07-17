#include "stdafx.h"
#include "util.h"
#include <string>
#include "./md5/md5sum.h"
#include "./file/FileClass.h"
#include "./update/verinfo.h"

CString GetExePath()
{
	TCHAR szBuffer[MAX_PATH];
	::GetModuleFileName(NULL, szBuffer, sizeof(szBuffer) / sizeof(TCHAR));

	CString strPath(szBuffer);
	int nFind = strPath.ReverseFind('\\');
	if(nFind < 0)
	{
		return "";
	}

	return strPath.Left(nFind + 1);
}

CString Convert(CString str, int sourceCodepage, int targetCodepage)
{
	int len=str.GetLength();

	/*
	char* pChar = TranslateUTF_8ToGB( str.GetBuffer(len), len );
	
	CString rt = "";

	if ( pChar )
	{
		len = strlen(pChar);
		rt.Format("%s",pChar);
		std::string strTemp = pChar;
		rt = strTemp.c_str();
		delete [] pChar;
	}
	*/

	int unicodeLen = MultiByteToWideChar(sourceCodepage,0,str,-1,NULL,0);

	wchar_t* pUnicode;
	pUnicode=new wchar_t[unicodeLen+1];

	memset(pUnicode,0,(unicodeLen+1)*sizeof(wchar_t));


	MultiByteToWideChar(sourceCodepage,0,str,-1,(LPWSTR)pUnicode,unicodeLen);

	BYTE * pTargetData = NULL;
	int targetLen = WideCharToMultiByte(targetCodepage,0,(LPWSTR)pUnicode,-1,(char *)pTargetData,0,NULL,NULL);

	pTargetData = new BYTE[targetLen+1];
	memset(pTargetData,0,targetLen+1);

	WideCharToMultiByte(targetCodepage,0,(LPWSTR)pUnicode,-1,(char *)pTargetData,targetLen,NULL,NULL);

	CString rt;
	std::string strTemp = (char *)pTargetData;
	//rt.Format("%s",pTargetData);
	rt = strTemp.c_str();

	delete pUnicode;
	delete pTargetData;
	
	/*
	static BOOL bStat = FALSE;

	if ( !bStat )
	{
		bStat = TRUE;
		std::ofstream* g_logfile = new std::ofstream;
		g_logfile->open("c:\\tidy\\in.html", std::ios::out|std::ios::app);
		*g_logfile << str.GetBuffer(str.GetLength()) << std::endl;
		g_logfile->close();
		delete g_logfile;

		g_logfile = new std::ofstream;
		g_logfile->open("c:\\tidy\\out.html", std::ios::out|std::ios::app);
		*g_logfile << rt.GetBuffer(rt.GetLength()) << std::endl;
		g_logfile->close();
		delete g_logfile;

		len = rt.GetLength();

	}
	*/
	return rt;

}


CString HexToBin(CString string)//将16进制数转换成2进制
{
	if( string == "0") return "0000";
	if( string == "1") return "0001";
	if( string == "2") return "0010";
	if( string == "3") return "0011";
	if( string == "4") return "0100";
	if( string == "5") return "0101";
	if( string == "6") return "0110";
	if( string == "7") return "0111";
	if( string == "8") return "1000";
	if( string == "9") return "1001";
	if( string == "a") return "1010";
	if( string == "b") return "1011";
	if( string == "c") return "1100";
	if( string == "d") return "1101";
	if( string == "e") return "1110";
	if( string == "f") return "1111";
	return "";
}


CString BinToHex(CString BinString)//将2进制数转换成16进制
{
	if( BinString == "0000") return "0";
	if( BinString == "0001") return "1";
	if( BinString == "0010") return "2";
	if( BinString == "0011") return "3";
	if( BinString == "0100") return "4";
	if( BinString == "0101") return "5";
	if( BinString == "0110") return "6";
	if( BinString == "0111") return "7";
	if( BinString == "1000") return "8";
	if( BinString == "1001") return "9";
	if( BinString == "1010") return "a";
	if( BinString == "1011") return "b";
	if( BinString == "1100") return "c";
	if( BinString == "1101") return "d";
	if( BinString == "1110") return "e";
	if( BinString == "1111") return "f";
	return "";
}

int BinToInt(CString string)//2进制字符数据转换成10进制整型
{
	int len =0;
	int tempInt = 0;
	int strInt = 0;
	for(int i =0 ;i < string.GetLength() ;i ++)
	{
		tempInt = 1;
		strInt = (int)string.GetAt(i)-48;
		for(int k =0 ;k < 7-i ; k++)
		{
			tempInt = 2*tempInt;
		}
		len += tempInt*strInt;
	}
	return len;
}  


// UTF-8转换成GB2312先把UTF-8转换成Unicode.然后再把Unicode通过函数WideCharToMultiByte转换成GB2312 
WCHAR* UTF_8ToUnicode(char *ustart)  //把UTF-8转换成Unicode
{
	char char_one;
	char char_two;
	char char_three;
	int Hchar;
	int Lchar;
	char uchar[2];
	WCHAR *unicode;
	CString string_one;
	CString string_two;
	CString string_three;
	CString combiString;
	char_one = *ustart;
	char_two = *(ustart+1);
	char_three = *(ustart+2);
	string_one.Format("%x",char_one);
	string_two.Format("%x",char_two);
	string_three.Format("%x",char_three);
	string_three = string_three.Right(2);
	string_two = string_two.Right(2);
	string_one = string_one.Right(2);
	string_three = HexToBin(string_three.Left(1))+HexToBin(string_three.Right(1));
	string_two = HexToBin(string_two.Left(1))+HexToBin(string_two.Right(1));
	string_one = HexToBin(string_one.Left(1))+HexToBin(string_one.Right(1));
	combiString = string_one +string_two +string_three;
	combiString = combiString.Right(20);
	combiString.Delete(4,2);
	combiString.Delete(10,2);
	Hchar = BinToInt(combiString.Left(8));
	Lchar = BinToInt(combiString.Right(8));
	uchar[1] = (char)Hchar;
	uchar[0] = (char)Lchar;
	unicode = (WCHAR *)uchar;
	return unicode;
}


char * UnicodeToGB2312(unsigned short uData)  //把Unicode 转换成 GB2312
{
	char *buffer ;
	buffer = new char[sizeof(WCHAR)];
	WideCharToMultiByte(CP_ACP,NULL,&uData,1,buffer,sizeof(WCHAR),NULL,NULL);
	return buffer;
}  

char * TranslateUTF_8ToGB(char *xmlStream, int len)   //len 是xmlStream的长度 
{ 
	char * newCharBuffer = new char[len]; 
	int index =0; 
	int nCBIndex = 0; 
	while(index < len) 
	{ 
		if(xmlStream[index] > 0)     // 如果是GB2312的字符 
		{ 
			newCharBuffer[nCBIndex] = xmlStream[index];    //直接复制 
			index += 1;              //源字符串偏移量1 
			nCBIndex += 1;           //目标字符串偏移量1 
		} 
		else                 //如果是UTF-8的字符 
		{ 
			WCHAR * Wtemp =  UTF_8ToUnicode(xmlStream + index);  //先把UTF-8转成Unicode 
			char * Ctemp  = UnicodeToGB2312(*Wtemp);//再把Unicode 转成 GB2312 
			newCharBuffer[nCBIndex] = * Ctemp;         // 复制 
			newCharBuffer[nCBIndex + 1] = *(Ctemp + 1); 
			index += 3;     //源字符串偏移量3 
			nCBIndex += 2;   //目标字符串偏移量2  因为一个中文UTF-8占3个字节，GB2312占两个字节 
		} 
	} 
	newCharBuffer[nCBIndex] = 0; //结束符 
	return newCharBuffer;    
} 




BOOL getWindowText(HWND hWnd, CString& text)
{
	int nLen = MAX_PATH;//::GetWindowTextLength(hWnd);
	int nRetLen = ::GetWindowText(hWnd, text.GetBufferSetLength(nLen), nLen + 1);
	text.ReleaseBuffer();
	if(nRetLen < nLen)
		return FALSE;
	return TRUE;
}
/*
HRESULT GetMetaCurrentScheme(ISkinScheme** pScheme)
{
	if (!_hLibSkinLoad)
		return S_FALSE;

	typedef BOOL (WINAPI *GetCurrentScheme)(ISkinScheme** pScheme);
	GetCurrentScheme pf = (GetCurrentScheme)GetProcAddress(_hLibSkinLoad, "GetCurrentScheme");
	ATLASSERT(pf);
	if(pf)
	{
		pf(pScheme);
	}		
	return S_OK;
}
HINSTANCE getSkinHanld()
{
	if (_hLibSkinLoad)
		return _hLibSkinLoad;

	_hLibSkinLoad = LoadLibrary("skin.dll");
	return _hLibSkinLoad;
}

*/

BOOL OpenUrl(LPCTSTR szUrl)
{
	CComQIPtr<IWebBrowser2> m_pWebBrowser2;
	m_pWebBrowser2.CoCreateInstance(CLSID_InternetExplorer);
	HRESULT hr;
	hr = m_pWebBrowser2->put_StatusBar(VARIANT_TRUE);
	hr = m_pWebBrowser2->put_ToolBar(VARIANT_TRUE); 
	hr = m_pWebBrowser2->put_MenuBar(VARIANT_TRUE);
	hr = m_pWebBrowser2->put_Visible(VARIANT_TRUE);
	CComVariant v;
	CComBSTR url(szUrl);
	hr = m_pWebBrowser2->Navigate ( url, &v, &v, &v, &v );
	if (FAILED(hr))
	{
		std::string cmd = "IEXPLORE.EXE \"";
		cmd  += szUrl;
		cmd  += "\"";
		//createProcess(cmd.c_str());		
		ShellExecute(NULL, _T("open"), szUrl, NULL, NULL, 2);
		return TRUE;
	}	
	return TRUE;
}

void minimizeMemory()
{
	OSVERSIONINFOEX osvi;
	BOOL bOsVersionInfoEx;

	// Try calling GetVersionEx using the OSVERSIONINFOEX structure.
	// If that fails, try using the OSVERSIONINFO structure.
	ZeroMemory(&osvi, sizeof(OSVERSIONINFOEX));
	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);

	if( !(bOsVersionInfoEx = GetVersionEx ((OSVERSIONINFO *) &osvi)) ) {
		osvi.dwOSVersionInfoSize = sizeof (OSVERSIONINFO);
		if (! GetVersionEx ( (OSVERSIONINFO *) &osvi) )
			return;
	}

	if(osvi.dwPlatformId == VER_PLATFORM_WIN32_NT) {
		SetProcessWorkingSetSize(GetCurrentProcess(), -1, -1);
	}
}


CString getTempFilePath()
{
	char szTmpPath[1024]		= "";
	char szTempName[MAX_PATH];

	// get temp path
	GetTempPath(1024,   // length of the buffer
		szTmpPath);      // buffer for path 

	// Create a temporary file. 
	GetTempFileName(szTmpPath, // directory for temp files 
		"NEW",                    // temp file name prefix 
		0,                        // create unique name 
		szTempName);              // buffer for name 
	
	return szTempName;
}

std::string	GetFileMd5Info(std::string sFile)
{
	int			i				= 0;
	int			nCnt			= 0;
	std::string		sRet			;
	MD5Sum		md5sum			;
	CFileClass	FileObj			;
	DWORD		dwFileLen		= 0;
	BYTE		buf[64*1024]	= "";

	if(!(FileObj.Open(sFile,CFileClass::modeRead | CFileClass::modeWrite)))
	{
		return "";
	}

	dwFileLen = FileObj.GetLength();

	nCnt = dwFileLen/(64*1024);
	for(i=0;i<nCnt;i++)
	{
		FileObj.Read(buf,64*1024);
		md5sum.put((char *)buf,1024*64);
	}

	if(nCnt*1024*64!=dwFileLen)
	{
		dwFileLen-=(nCnt*1024*64);
		FileObj.Read(buf,dwFileLen);
		md5sum.put((char *)buf,dwFileLen);
	}

	sRet = md5sum.toString();
	FileObj.Close();
	return sRet	;
}

int    str_split(std::vector<std::string>& dest, const TCHAR* str, const TCHAR* delimiter, bool includeEmpty)
{
	const TCHAR *prev = str;
	const TCHAR *cur = str;
	const TCHAR *tmp;
	bool    found;
	std::string  txt;

	dest.clear();

	for(; *cur; cur++) {
		tmp = delimiter;
		found = false;
		while (*tmp) {
			if (*tmp == *cur) {
				found = true;
				break;
			}
			tmp++;
		}
		if(found) {
			if(prev < cur) {
				txt.assign(prev, cur - prev);
				dest.push_back(txt);
				prev = cur + 1;
			} else if(prev == cur) {
				prev = cur + 1;
				if(includeEmpty)
					dest.push_back(_T(""));
			}
		}
	}
	if(prev < cur) {
		txt.assign(prev, cur - prev);
		dest.push_back(txt);
	} else if(prev == cur && includeEmpty) {
		dest.push_back(_T(""));
	}
	return dest.size();
}

/*
BOOL InitMail()
{
	_hMail = LoadLibrary("MAPI32.DLL");
	if (_hMail == NULL)	//加载动态库失败
	{
		return FALSE;
	}
	//获取发送邮件的函数地址
	(FARPROC&)_lpfnSendMail = GetProcAddress(_hMail, "MAPISendMail");	
	if ( _lpfnSendMail )
		return TRUE;
	else 
		return FALSE;
}

BOOL SendMail(HWND hWndParent, CString strMailto, CString strTitle, CString strContext)
{
	if ( !_lpfnSendMail )
		return FALSE;

	if (!hWndParent || !::IsWindow(hWndParent))
		return FALSE;

	//收件人结构信息
	MapiRecipDesc recip;
	memset(&recip,0,sizeof(MapiRecipDesc));
	recip.lpszAddress	= strMailto.GetBuffer(0);
	recip.ulRecipClass = MAPI_TO;

	//邮件结构信息
	MapiMessage message;
	memset(&message, 0, sizeof(message));
	message.nFileCount	= 0;				//文件个数
	message.lpFiles		= NULL;				//文件信息
	message.nRecipCount = 1;				//收件人个数
	message.lpRecips 	= &recip;					//收件人
	message.lpszSubject	= strTitle.GetBuffer(0);	//主题
	message.lpszNoteText= strContext.GetBuffer(0);	//正文内容

	//发送邮件
	int nError = _lpfnSendMail(0, (ULONG_PTR)hWndParent,
		&message, MAPI_LOGON_UI|MAPI_DIALOG, 0);

	if (nError != SUCCESS_SUCCESS && nError != MAPI_USER_ABORT 
		&& nError != MAPI_E_LOGIN_FAILURE)
	{
		//AfxMessageBox(AFX_IDP_FAILED_MAPI_SEND);
		MessageBox(NULL, "发送邮件失败", MSGTITLE, MB_OK);
		return FALSE;
	}
	return TRUE;
	
}

void ReleaseMail()
{
	if ( _hMail )
		FreeLibrary( _hMail );
}

*/

bool isFileExist(const TCHAR *file)
{
	WIN32_FIND_DATA data;
	HANDLE handle;

	if(!file)
		return false;

	handle = FindFirstFile(file, &data);
	if(handle == INVALID_HANDLE_VALUE)
		return false;

	FindClose(handle);
	return true;
}
bool isDirectoryExist(const TCHAR *dir)
{
	WIN32_FIND_DATA data;
	HANDLE handle;
	TCHAR *name;
	int  len;

	if(!dir)
		return false;

	len = lstrlen(dir);
	name = (TCHAR *)malloc((len + 5)*sizeof(TCHAR));
	lstrcpy(name, dir);
	if(name[len - 1] == _T('\\'))
		lstrcpy(name + len, _T("*.*"));
	else
		lstrcpy(name + len, _T("\\*.*"));

	handle = FindFirstFile(name, &data);
	free(name);

	if(handle == INVALID_HANDLE_VALUE) {
		return false;
	}
	FindClose(handle);
	return true;
}

bool createDirectory(const TCHAR *dir)
{
	if(isDirectoryExist(dir))
		return true;

	if(CreateDirectory(dir, NULL))
		return true;
	else {
		if(GetLastError() == ERROR_PATH_NOT_FOUND) {
			TCHAR path[MAX_PATH*2];
			lstrcpy(path, dir);
			TCHAR *p = path;
			while (*p) {
				if(*p == _T('\\') && *(p+1) != _T('\\')) {
					*p = 0;
					if(!isDirectoryExist(path)) {
						if(!CreateDirectory(path, NULL))
							return false;
					}
					*p = _T('\\');
				}
				p = CharNext(p);
			}
			if(!isDirectoryExist(path)) {
				if(!CreateDirectory(path, NULL))
					return false;
			}
			return true;
		}
		return false;
	}
}

bool createProcess(LPCTSTR cmd, BOOL bShow, BOOL bUtilEnd)
{
	PROCESS_INFORMATION pi;
	STARTUPINFO si = {0};
	si.cb = sizeof(si);
	si.dwFlags = STARTF_USESHOWWINDOW;
	if(bShow) {
		si.wShowWindow = SW_SHOWDEFAULT;
		//si.lpDesktop = "WinSta0\\Default";
	} else {
		si.wShowWindow = SW_HIDE;
	}

	if(CreateProcess(NULL, (LPTSTR)cmd, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi)) 
	{
		if(bUtilEnd) WaitForSingleObject(pi.hProcess, INFINITE);
		CloseHandle(pi.hThread);
		CloseHandle(pi.hProcess);
		return true;
	}
	return false;
}

//判断是否可以创建文件
bool CanCreateFile(const std::string &sFile)
{
	if(sFile.empty())
		return FALSE;

	if(isDirectoryExist(sFile.c_str()))
		return FALSE;

	std::ifstream inFile(sFile.c_str());

	if(!inFile)
		return TRUE;
	else
		return FALSE;
}


//得到一个唯一的不存在文件名称
//得到唯一的文件名称--组合规则xxxxx(n).xxx
std::string	GetUniqueFileName(const std::string &sFile)
{
	int					nCount		= 0;
	std::string::size_type	dwPos		= std::string::npos;
	std::string::size_type	dwLeft		= std::string::npos;
	std::string::size_type	dwRight		= std::string::npos;
	std::string	sTempFile	;
	std::string	sFileName	;
	std::string	sFileType	;
	std::string sFilePath	;
	std::string sFileCount	;

	sTempFile = sFile;
	//得到原来的路径和文件名称
	dwPos = sTempFile.rfind(_T('\\'));
	if(dwPos!=std::string::npos)
	{
		sFilePath = sTempFile.substr(0,dwPos+1);
		sFileName = sTempFile.substr(dwPos+1);
	}
	else
	{
		sFileName = sTempFile;
		sFilePath = _T("");
	}

	//得到文件名称和类型
	sTempFile = sFileName;
	dwPos = sTempFile.find(_T('.'));
	if(dwPos!=std::string::npos)
	{
		sFileName = sTempFile.substr(0,dwPos);
		sFileType = sTempFile.substr(dwPos+1);
	}

	//得到文件的计数
	sTempFile = sFileName;
	dwLeft	  = sTempFile.find(_T("("));
	dwRight	  = sTempFile.rfind(_T(")"));

	if((dwLeft==0 && dwRight==sTempFile.size()-1) || (dwRight!=sTempFile.size()-1 && dwRight!=std::string::npos ))
	{
		dwLeft = std::string::npos;
		dwRight = std::string::npos;
	}

	if(dwLeft!=std::string::npos && dwRight!=std::string::npos)
	{
		sFileCount = sTempFile.substr(dwLeft+1,(dwRight-dwLeft)-1);
		if(dwLeft+1==dwRight)
		{
			sFileCount = _T("");
			sFileName  = sTempFile.substr(0,dwLeft);
		}
		else
		{
			//如果存在非数字之外的字符
			if(sFileCount.find_first_not_of(_T(" 0123456789"))!=std::string::npos)
			{
				sFileCount = _T("");
				sFileName  = sTempFile;
			}
			else
			{
				sFileName  = sTempFile.substr(0,dwLeft);
			}
		}
	}

	//重新组合文件名称
	while(TRUE)
	{
		std::ostringstream		sRetName	;
		sRetName << sFilePath;
		sRetName << sFileName;
		if(nCount!=0)
		{
			sRetName << _T("(");
			sRetName << nCount;
			sRetName << _T(")");
		}
		if(!sFileType.empty())
		{
			sRetName << _T(".");
			sRetName << sFileType;
		}

		//查询文件是否存在
		if(CanCreateFile(sRetName.str()))
			return sRetName.str();
		nCount++;
	}
}

bool GetFileVer(const TCHAR * filename, std::string &ver)
{
	CFileVersionInfo fvi;
	if( fvi.Open(filename) )
	{
		TCHAR szVer[ 512 ] = { 0 };
		::wsprintf( szVer, 
			_T( "%d.%d.%d.%d" ),
			fvi.GetFileVersionMajor(),  // Major version
			fvi.GetFileVersionMinor(),  // Minor version
			fvi.GetFileVersionBuild(),  // Build number
			fvi.GetFileVersionQFE()     // QFE
			);
		ver = szVer;
		fvi.Close();
		return true;
	}
	return false;
}

bool GetClientVer (std::string & ver)
{
	TCHAR szFileName[1024];
	GetModuleFileName(NULL, szFileName, 1024);	
	return GetFileVer(szFileName, ver);
}

BOOL ShellOpenFile(LPCTSTR Filename)
{
	if(Filename == NULL || *Filename == 0)
		return FALSE;
	BOOL bSuccess;
	SHELLEXECUTEINFO ShExecInfo = {0};
	ShExecInfo.cbSize = sizeof(SHELLEXECUTEINFO);
	ShExecInfo.fMask = SEE_MASK_NOCLOSEPROCESS|SEE_MASK_FLAG_NO_UI;
	ShExecInfo.hwnd = NULL;
	ShExecInfo.lpVerb = _T("open");
	ShExecInfo.lpFile = Filename;
	ShExecInfo.lpParameters = NULL;
	ShExecInfo.lpDirectory = NULL;
	ShExecInfo.nShow = SW_SHOWDEFAULT;
	ShExecInfo.hInstApp = NULL;
	bSuccess = ShellExecuteEx(&ShExecInfo);
	if(!bSuccess)
	{
		ShExecInfo.fMask = NULL;
		ShExecInfo.lpVerb = NULL;
		return ShellExecuteEx(&ShExecInfo);
	}
	return TRUE;
}


CString getEmailAddress(CString strMail)
{
	static const CString strCon = "abcdefghijklmnopqrstuvwxyz_-.1234567890";
	int nLen = strMail.GetLength();
	if ( nLen < 4 )
		return "";

	// 1 取得@
	int nPos = strMail.Find("@");
	if ( nPos <= 0 )
		return "";

	CString strLeft = "";
	CString strRight = "";

	// 2 判断@前,后是否是abcdefghijklmnopqrstuvwxyz_-. 和数字
	for ( int i = nPos + 1; i < nLen; i ++ )
	{
		CString strTemp = strMail.Mid( i, 1);
		strTemp.MakeLower();
		int nFind = strCon.Find( strTemp );
		if ( nFind == -1 )
		{
			//没有找到
			break;
		}
		strRight += strTemp;
	}

	for (  i = nPos - 1; i >= 0 ; i -- )
	{
		CString strTemp = strMail.Mid( i, 1);
		strTemp.MakeLower();
		int nFind = strCon.Find( strTemp );
		if ( nFind == -1 )
		{
			//没有找到
			break;
		}
		strLeft = strTemp + strLeft;
	}

	CString strTemp = strLeft + "@" + strRight;
	return strTemp;

}



//====================================================
//
// ../表示向上一层
// /表示根目录下的
// XX.htm表示当前目录下的
//把URL转换成绝对地址
CString OnConversionURL(CString sURL,CString str_fafURL)
{
	if ( str_fafURL.Find("http://") >= 0 )
		return str_fafURL;

	if(sURL.Find("/",8)<0)
	{
		sURL +="/";
	}
	CString str_activeURL;
	int int_j = 0;
	int i=0;
	str_activeURL = str_fafURL;
	if(str_fafURL.Find("../",0)!=-1&&str_fafURL[0]!='/')
	{
		while( i<=str_fafURL.GetLength() )
		{
			if( str_fafURL[i] == '.' && str_fafURL[i+1] == '.' && str_fafURL[i+2] == '/' )
			{ int_j++;}
			i++;
		}
		if(str_fafURL[0]=='/')
		{
			str_fafURL.Delete(0,1);
		}
		str_fafURL.Replace("../","");
		i=0;
		int int_i=0;
		while( i <= sURL.GetLength() )
		{
			if( sURL[i]=='/' )
			{ 
				int_i++;
			}
			i++;
		}
		int_i -= int_j;

		if( int_i<3 )
		{
			int_i = 3;
		}

		int int_cour=0;
		for( i=0; i<=sURL.GetLength(); i++)
		{
			if( sURL[i]=='/' )
			{ 
				int_cour++;
			}
			if( int_cour==int_i )
			{			 
				sURL= sURL.Left(i+1);
				break;
			}
		}
		//容错处理
		if( sURL[sURL.GetLength()-1]!='/' )
		{	
			sURL +="/";
		}
		sURL += str_fafURL;
		return sURL;
	}
	else
	{
		if( str_fafURL[0] =='/' )
		{
			int int_b = 0 ;
			for( int a=0; int_b<3 && a<sURL.GetLength(); a++)
			{
				if( sURL[a]=='/' )
			 {
				 int_b++;
			 }
			 if( int_b==3 )
			 {
				 sURL = sURL.Left(a);
				 break;
			 }
			}
			sURL += str_fafURL;
		}
		else
		{
			for( int i = sURL.GetLength() ; i >  0 ; i -- )
			{
				if( sURL[i - 1] =='/' )
				{
					sURL = sURL.Left( i );
					break;
				}
			}
			sURL += str_fafURL;
		}
		return sURL;
	}
}