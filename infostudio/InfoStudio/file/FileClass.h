#ifndef	_FILE_CLASS_H_
#define	_FILE_CLASS_H_

#include <windows.h>
#include <string>
using namespace std;

class	CFileClass
{
private:
	HANDLE		m_hFile;
	string		m_sBuffer;		//缓冲区
public:
	enum
	{
		modeCreate		= 0x1,
		modeRead		= 0x2,
		modeWrite		= 0x4,
		shareDenyNone	= 0x8,
	};
	enum
	{
		begin			= FILE_BEGIN,
		end				= FILE_END,
		current			= FILE_CURRENT,
	};

	bool	IsOpen()
	{
		return m_hFile==NULL? false:true;
	}

	CFileClass()
	{
		m_hFile = NULL;
	};

	~CFileClass()	
	{
		Close();
	};

	void	SeekToEnd()
	{
		if(m_hFile)
		{
			SetFilePointer(m_hFile,0,NULL,FILE_END);
		}
	};

	void	Close()
	{
		if(m_hFile)
			CloseHandle(m_hFile);
		m_hFile = NULL;
	};

	void	SetEnd()
	{
		if(m_hFile)
			SetEndOfFile(m_hFile);
	}

	DWORD	GetPosition()	
	{
		if(m_hFile)
			return SetFilePointer(m_hFile,0,NULL,FILE_CURRENT);
		else
			return 0;
	};

	DWORD	GetLength()	
	{
		DWORD	dwRet		= 0;
		DWORD	dwCurrent	= 0;

		if(m_hFile)
		{
			dwCurrent	= GetPosition();
			dwRet		= SetFilePointer(m_hFile,0,NULL,FILE_END);
			SetFilePointer(m_hFile,dwCurrent,NULL,FILE_BEGIN);
			return dwRet;
		}
		else
			return 0;
	}

	DWORD	Read(void *pBuf,int bufSize) 
	{
		DWORD	dwRead	= 0;
		if(m_hFile)
			ReadFile(m_hFile,pBuf,bufSize,&dwRead,NULL);
		return dwRead;
	};

	BOOL	FlushBuffers()
	{
		if(m_hFile)
			return FlushFileBuffers(m_hFile);
		else 
			return FALSE;
	}

	DWORD	Write(void *pData,int size) 
	{
		DWORD	dwWrite	= 0;
		DWORD	dwError	= 0;

		if(m_hFile)
			if(!WriteFile(m_hFile,pData,size,&dwWrite,NULL))
				dwError = GetLastError();
		return dwWrite;
	};

	DWORD	Seek(int	dwSize,int mode)
	{
		if(m_hFile)
			return SetFilePointer(m_hFile,dwSize,NULL,mode);
		return 0;
	};

	string	GetLine()
	{
		if(!m_hFile)
			return "";
		
		string				sRet			;
		string::size_type	nPos			;
		DWORD				dwRead			= 0;
		DWORD				dwStart			= 0;
		char				szData[512*1024]= "";	//

		//如果缓冲区空了或者缓冲区没有找到回车换行则从文件中读取数据
LOOP:		while((m_sBuffer.empty() || (nPos = m_sBuffer.find("\n",dwStart))==string::npos))
		{
			if(dwRead!=0)
				dwStart+=dwRead;
			//如果读文件到末尾则返回
			if(!ReadFile(m_hFile,szData,sizeof(szData),&dwRead,NULL))
			{
				if(!m_sBuffer.empty())
				{
					sRet = m_sBuffer;
					m_sBuffer.erase();
				}
				return sRet;
			}
			else if(dwRead==0)
				return sRet;
			m_sBuffer.append(szData,dwRead);
		}
		sRet.append(m_sBuffer.data(),nPos);
		m_sBuffer.erase(0,nPos+2);
		if(sRet.empty())
			goto LOOP;
		return sRet;
	};

	bool	Open(LPCSTR lpFile,int mode)
	{
		DWORD	amode	= 0;
		DWORD	cmode	= 0;

		if((mode & modeRead))
			amode  = GENERIC_READ;
		if(mode & modeWrite) 
			amode |=GENERIC_WRITE;

		if(mode & modeCreate) 
		{
			amode = GENERIC_READ | GENERIC_WRITE;
			cmode = CREATE_ALWAYS;
		}
		else
			cmode = OPEN_EXISTING;

		m_hFile = CreateFile(lpFile,amode,FILE_SHARE_READ,NULL,cmode,FILE_ATTRIBUTE_NORMAL,NULL);
		if(m_hFile!=INVALID_HANDLE_VALUE)
			return TRUE;
		m_hFile = NULL;
		return FALSE;
	};

	bool	Open(const string& sfile,int mode)
	{
		return Open(sfile.c_str(),mode);
	};
};

#endif	_FILE_CLASS_H_