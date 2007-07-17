#ifndef __DISPATCH_HOST_H__
#define __DISPATCH_HOST_H__

#pragma once
#include <string>
#include <sstream>
#include <map>
#include <vector>
using namespace std;
// 通知代理窗口做最后的清理工作，DestroyWindow


#include "comhelp.h"
#include "dispdyna.h"
#include "webview.h"

// atlTraceWebHost
class HostDisp : public DynaDispObject<HostDisp>
{
public:
	BEGIN_DISPATCH_MAP(HostDisp)
		DISP_PROPGET(foo, VT_BSTR)		
		DISP_PROPGET1(test, VT_VARIANT, VT_VARIANT)	

	

	END_DISPATCH_MAP()

	enum{
		START_DOWN	= 0,
		FINISH_DOWN = 1
		};
	HostDisp()
	{
			
	}
	~HostDisp()
	{

	}



	VARIANT __stdcall get_test(VARIANT v)
	{
		ATLTRACE( "vt=%d, %S", v.vt, v.bstrVal );
		VARIANT vr;
		VariantInit(&vr);
		CComVariant v1(L"in test");
		v1.Detach(&vr);
		return vr;
	}

	BSTR __stdcall get_foo()
	{
		return CComBSTR(L"hello from get_foo").Detach();
	}


	void SetWebView(CWebView* pWebView)
	{
		_pWebView = pWebView;
	}

private:
	CWebView* _pWebView;
	

public:
	
};

#endif //__DISPATCH_HOST_H__