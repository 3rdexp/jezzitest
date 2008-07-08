// mfopen.cpp: implementation of the mfopen class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "mfopen.h"

#include "findfile.h"
#include "kbplay.h"

std::vector<std::string> MultiOpenFile(bool filter)
{
	std::vector<std::string> ret;

	CComPtr<IFileFormat> spff;
	HRESULT hr = GetFileFormat(&spff);
	if(FAILED(hr) || !spff && filter)
		return ret;
	
	CFileDialog fdlg(TRUE, NULL, NULL, OFN_FILEMUSTEXIST | OFN_EXPLORER | OFN_ALLOWMULTISELECT 
		| OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, spff->GetFilterString());

	int structsize=0; 
	DWORD dwVersion,dwWindowsMajorVersion,dwWindowsMinorVersion; 
	//���Ŀǰ�Ĳ���ϵͳ��GetVersion�����÷����MSDN 
	dwVersion = GetVersion(); 
	dwWindowsMajorVersion = (DWORD)(LOBYTE(LOWORD(dwVersion))); 
	dwWindowsMinorVersion = (DWORD)(HIBYTE(LOWORD(dwVersion)));// ������еĲ���ϵͳ��Windows NT/2000 
	if (dwVersion < 0x80000000)
		structsize =88;//��ʾ�µ��ļ��Ի��� 
	else //���еĲ���ϵͳWindows 95/98 
		structsize =76;//��ʾ�ϵ��ļ��Ի��� 		�� 

	fdlg.m_ofn.lStructSize=structsize; 

	const int filename_buffer = 0x1000;

	std::vector<char> selected_files(filename_buffer, '\0');

	fdlg.m_ofn.lpstrFile = selected_files.begin();
	fdlg.m_ofn.nMaxFile = filename_buffer;

	if(IDOK == fdlg.DoModal())
	{
		std::string path = selected_files.begin();
		int start = fdlg.m_ofn.nFileOffset;

		// CAUTION: û���жϻ���ļ������ķ���
		for(std::vector<char>::const_iterator it=selected_files.begin() + start; it<selected_files.end();)
		{
			std::string file = path;
			
			if(!path.empty() && path[path.size()-1] != '\\')
				file += '\\';

			file += it;

			it += std::string(it).size() + 1;

			// cout << file << endl;
			if(filter && spff)
			{
				if( spff->Check(file.c_str()) )
					ret.push_back(file);
			}
			else
				ret.push_back(file);

			if(!*it)
				break;
		}

		// ����ֻ��һ���ļ������
		if(ret.size() == 1)
			ret[0] = path;
	}
	return ret;
}

template<class ConT>
static LPSTR StringArrayCombine(ConT const & cont)
{
	int nLen = cont.size() * MAX_PATH;
	LPSTR p = (LPSTR)GlobalAlloc(GPTR, nLen);
	if(!p)
		return 0;

	LPSTR pret = p;

	for(ConT::const_iterator it=cont.begin(); it!=cont.end(); ++it)
	{
		lstrcpyn(p, (*it).c_str(), nLen);
		nLen -= (*it).size() + 1;
		p += (*it).size() + 1;

		*p = '\0';
	}
	return pret;
}

DWORD WINAPI OpenMediaFiles(LPSTR *apath)
{
	ASSERT(apath);
	*apath = 0;

	std::vector<std::string> files = MultiOpenFile();
	int nLen = files.size() * MAX_PATH;
	LPSTR p = (LPSTR)GlobalAlloc(GPTR, nLen);
	if(!p)
		return 0;

	*apath = p;

	for(std::vector<std::string>::const_iterator it=files.begin(); it!=files.end(); ++it)
	{
		lstrcpyn(p, (*it).c_str(), nLen);
		nLen -= (*it).size() + 1;
		p += (*it).size() + 1;

		*p = '\0';
	}
	return files.size();
}

//////////////////////////////////////////////////////////////////////////
//
class MyFolderDialog : public CFolderDialogImpl<MyFolderDialog>
{
public:
	MyFolderDialog(HWND hWndParent = NULL, LPCTSTR lpstrTitle = NULL, UINT uFlags = BIF_RETURNONLYFSDIRS)
		: CFolderDialogImpl<MyFolderDialog>(hWndParent, lpstrTitle, uFlags)
	{
	}

	void OnInitialized()
	{
		SetSelection(m_lpstrInitialFolder);
	}
};

std::vector<std::string> OpenMediaDirImpl(bool filter)
{
	std::vector<std::string> ret;

	CComPtr<IFileFormat> spff;
	HRESULT hr = GetFileFormat(&spff);

	MyFolderDialog ddlg(NULL, "ѡ�����ý���ļ���Ŀ¼", BIF_RETURNONLYFSDIRS | BIF_NEWDIALOGSTYLE | BIF_NONEWFOLDERBUTTON);
	// TODO: ��ʼĿ¼��ַ���ҵĵ���
	// ddlg.SetInitialFolder();
	ddlg.SetInitialFolder("e:\\m\\");
	if(IDOK == ddlg.DoModal())
	{
		// ddlg.GetFolderPath();
		std::string path = ddlg.GetFolderPath();
		path += "\\*.*";

		CWinFindFile ff;
		if( ff.FindFile(path.c_str()) )
		{
			while(ff.m_bFound)
			{
				if( !ff.IsDirectory() && !ff.IsDots() )
				{
					std::string file = ff.GetFilePath();
					if(filter && spff)
					{
						if(spff->Check(file.c_str()))
							ret.push_back(file);
					}
				}

				ff.FindNextFile();
			}
		}
	}
	return ret;
}

DWORD WINAPI OpenMediaDirectory(LPSTR *apath, BOOL filter)
{
	ASSERT(apath);
	*apath = 0;

	std::vector<std::string> files = OpenMediaDirImpl(filter);
	if(files.size())
	{
		*apath = StringArrayCombine(files);
		return files.size();
	}
	return FALSE;
}