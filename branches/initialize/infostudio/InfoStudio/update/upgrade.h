#pragma once

#include "verinfo.h"
#include <vector>
#include <string>
#include <sys\stat.h>
#include "../util.h"

bool GetBackupPackInfo(const TCHAR * filename, std::string & ver)
{
	TCHAR szFileName[1024];
	GetModuleFileName(NULL, szFileName, 1024);
	TCHAR * chEnd = StrRChr(szFileName,NULL, _T('\\'));
	if(chEnd)
	{
		*(chEnd+1) = 0;
		lstrcat(szFileName, filename);
		struct _stat st;
		std::string ACfgFile = szFileName;
		if(_stat(ACfgFile.c_str(), &st) != 0)
		{
			return false;
		}
		return GetFileVer(szFileName, ver);
	}
	return false;
}

bool CompareVer(std::string strSetupVer, std::string strVer)
{
	vector<std::string> setupVerlist;
	vector<std::string> Verlist;
	str_split(setupVerlist, strSetupVer.c_str(), _T("."));
	str_split(Verlist, strVer.c_str(), _T("."));

	int nCount = max(setupVerlist.size() , Verlist.size());
	for (int i =0; i < nCount; i++)
	{
		int nSetupVer = 0;
		int nVer = 0;
		if ((int)setupVerlist.size() > i)
		{
			nSetupVer = _ttoi(setupVerlist[i].c_str());
		}

		if ((int)Verlist.size() > i)
		{
			nVer = _ttoi(Verlist[i].c_str());
		}
		if (nSetupVer > nVer)
			return true;
		else if (nSetupVer < nVer)
			return false;
	}
	return false;

}
/*
bool GetClientVer (std::string & ver)
{
	TCHAR szFileName[1024];
	GetModuleFileName(NULL, szFileName, 1024);	
	return GetFileVer(szFileName, ver);
}
*/
bool CheckSetupVersion()
{
	std::string strSetupVer;
	std::string strVer;	
	strSetupVer = _T("");
	strVer = _T("");		
	BOOL bRet = GetBackupPackInfo(_T("data\\setup.exe"), strSetupVer);
	if (!bRet)
		return false;

	GetClientVer(strVer);	
	bRet = CompareVer(strSetupVer, strVer);
	if (bRet)
	{
		//升级
		int nBack = MessageBox(NULL, "客户端已有新版本,是否升级?", MSGTITLE, MB_YESNO|MB_ICONQUESTION);
		if(nBack != IDYES )
			return false;
		else
			return true;
	}
	return false;
}

BOOL updateClient(LPCSTR updateurl)
{
	TCHAR cmd[MAX_PATH];
	WTL::CString strExe = GetExePath();	
	sprintf(cmd, "\"%s\\bgupdate.exe\" /m0%s",strExe.GetBuffer(strExe.GetLength()), updateurl);			
	return createProcess(cmd, FALSE,FALSE);

}