
#pragma once

#include <string>
#include <vector>
#include <sstream>
#include <fstream>

CString GetExePath();
BOOL getWindowText(HWND hWnd, CString& text);
CString Convert(CString str, int sourceCodepage, int targetCodepage);
//HRESULT GetMetaCurrentScheme(ISkinScheme** pScheme);
//HINSTANCE getSkinHanld();

BOOL OpenUrl(LPCTSTR szUrl);
void minimizeMemory();
CString getTempFilePath();
std::string	GetFileMd5Info(std::string sFile);
int    str_split(std::vector<std::string>& dest, const char* str, const char* delimiter, bool includeEmpty = false);
//BOOL InitMail();
//BOOL SendMail(HWND hWndParent, CString strMailto, CString strTitle, CString strContext);
//void ReleaseMail();
bool isFileExist(const TCHAR *file);

bool isDirectoryExist(const TCHAR *dir);
bool createDirectory(const TCHAR *dir);
bool createProcess(LPCTSTR cmd, BOOL bShow = TRUE, BOOL bUtilEnd = FALSE);

bool CanCreateFile(const std::string &sFile);
std::string	GetUniqueFileName(const std::string &sFile);
bool GetFileVer(const TCHAR * filename, CString &ver);
bool GetClientVer (std::string & ver);
BOOL ShellOpenFile(LPCTSTR Filename);

CString HexToBin(CString string);
CString BinToHex(CString BinString);
int BinToInt(CString string);
WCHAR* UTF_8ToUnicode(char *ustart);
char * UnicodeToGB2312(unsigned short uData);
char * TranslateUTF_8ToGB(char *xmlStream, int len);
CString getEmailAddress(CString strMail);
CString OnConversionURL(CString sURL,CString str_fafURL);