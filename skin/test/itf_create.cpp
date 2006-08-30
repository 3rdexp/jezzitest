

#include "stdafx.h"


#include "SkinMgr.h"

#define INITGUID
#include "Skiniid.h"
#include "../src/controls/sbutton.h"

#include <cassert>

CAppModule _Module;

bool test_iskinmgr()
{
	// 0 namespace
	using namespace Skin;

	// 1 can create
	CComPtr<ISkinMgr> mgr;
	GetSkinMgr(&mgr);
	assert(mgr);

	// 2 unique itf
	CComPtr<ISkinMgr> mgr2;
	GetSkinMgr(&mgr2);
	
	assert( mgr == mgr2 );

	// 3 skinscheme
	CComPtr<ISkinScheme> pss;
	mgr->GetCurentScheme(&pss);
	assert(pss);


	
	return false;
}

bool test_ctrl()
{
	using namespace Skin;

	typedef Int2Type<0, CButton> tt;
	int i = tt::value;
	void *p = new tt::type();

	typedef SkinControlImpl<SkinButton<CWindow> > ssbuton;
	ssbuton ss;
	ss.Install(0);
	return true;
}


int main()
{
	CoInitialize(0);
	test_iskinmgr();
	CoUninitialize();
	return 0;
}