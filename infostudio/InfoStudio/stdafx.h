// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__BBE4FEC4_A5F9_4C35_884F_1D2DAF636DD4__INCLUDED_)
#define AFX_STDAFX_H__BBE4FEC4_A5F9_4C35_884F_1D2DAF636DD4__INCLUDED_

// Change these values to use different versions
#define WINVER		0x0400
//#define _WIN32_WINNT	0x0400
#define _WIN32_IE	0x0400
#define _RICHEDIT_VER	0x0100

#include <atlbase.h>
#include <atlapp.h>

extern CAppModule _Module;

#include "ado/Ado.h"

#include <atlwin.h>
#include <atlcom.h>
#include <atlhost.h>
#include <atlctl.h>
#include <atlcrack.h>
#include <atlmisc.h>


#if (_ATL_VER >= 0x0700)
DECLARE_TRACE_CATEGORY(atlTraceLog);
DECLARE_TRACE_CATEGORY(atlTraceWarn);
DECLARE_TRACE_CATEGORY(atlTracePerf);
DECLARE_TRACE_CATEGORY(atlTraceError);
DECLARE_TRACE_CATEGORY(atlTraceWebHost);
DECLARE_TRACE_CATEGORY(atlTraceDispRef);

#ifdef _DEBUG
__declspec(selectany) ATL::CTraceCategory atlTraceLog(_T("atlTraceLog"));
__declspec(selectany) ATL::CTraceCategory atlTraceWarn(_T("atlTraceWarn"));
__declspec(selectany) ATL::CTraceCategory atlTracePerf(_T("atlTracePerf"));
__declspec(selectany) ATL::CTraceCategory atlTraceError(_T("atlTraceError"));
__declspec(selectany) ATL::CTraceCategory atlTraceWebHost(_T("atlTraceWebHost"));
__declspec(selectany) ATL::CTraceCategory atlTraceDispRef(_T("atlTraceDispRef"));
#endif
#else // !(_ATL_VER >= 0x0700)
enum myTraceFlags
{
	atlTraceLog = atlTraceUser,
	atlTraceWarn = atlTraceUser2,
	atlTracePerf = atlTraceUser3,
	atlTraceError = atlTraceUser4,
	atlTraceWebHost = 0x20000000,
	atlTraceDispRef = 0x40000000,
};
#endif

#ifndef ASSERT
#define ASSERT ATLASSERT
#endif

#ifndef TRACE
#define TRACE ATLTRACE
#endif

class CInfoManage;
extern CAdoConnection* _pDb;
extern CInfoManage*	_pInfoManage;


//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__BBE4FEC4_A5F9_4C35_884F_1D2DAF636DD4__INCLUDED_)
