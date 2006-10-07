// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__717ED902_72CB_4E5C_8D0B_7AF66C7E4B65__INCLUDED_)
#define AFX_STDAFX_H__717ED902_72CB_4E5C_8D0B_7AF66C7E4B65__INCLUDED_

// Change these values to use different versions
//#define WINVER		0x0400
//#define _WIN32_IE	0x0400
//#define _RICHEDIT_VER	0x0100

#define WINVER		0x0500
#define _WIN32_WINNT	0x0500
#define _WIN32_IE	0x0501
#define _RICHEDIT_VER	0x0200

#define _WTL_NO_CSTRING

#include <atlbase.h>
#include <atlapp.h>

extern CAppModule _Module;

#include <atlwin.h>

const TCHAR* const kPadRegKey = _T("Software\\Konstantin Boukreev\\kPad");

//#define _RICHEDIT_USE_TOM

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__717ED902_72CB_4E5C_8D0B_7AF66C7E4B65__INCLUDED_)
