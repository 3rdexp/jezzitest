// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#pragma once

// Change these values to use different versions
#define WINVER		0x0501
// #define _WIN32_WINNT	0x0501 in proj setting

#define _WIN32_IE	0x0501  // for toolbar style BTNS_SHOWTEXT

#define _RICHEDIT_VER	0x0100
#define _WTL_NO_CSTRING

#include <atlbase.h>
#include <atlstr.h>
#include <atlapp.h>

extern CAppModule _Module;

#include <atlwin.h>
#include <atltypes.h>