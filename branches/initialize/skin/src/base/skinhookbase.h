#pragma once

#include "SkinCtrl.h"

namespace Skin {

	class CSkinHookBase
	{
	public:
		virtual bool Install( HWND hWnd ) = 0;

		int _classid;
	};

	template <class T, class TBase = ATL::CWindow>
	class CSkinHookBaseImpl : public CSkinHookBase, public ATL::CWindowImpl<T, TBase>
	{
	public:
		void OnFinalMessage(HWND)
		{
			ATLTRACE("Delete hook automatically\n");
			T* pT = static_cast<T*>(this);
			pT->Finalize();
			delete pT;
		}

		bool Install( HWND hWnd )
		{
			SubclassWindow( hWnd );
			T* pT = static_cast<T*>(this);
			pT->Initialize( );
			return true;
		}

		////////////////////////////////////////////////////////////////////
		// overrides, we don't need virtual member since we use the ATL way
		void Initialize() {}; //instance initialize
		void Finalize() {};	//instance finalize
	};

	template < class T >
	class CSkinHookImpl : public CSkinHookBaseImpl<T>
	{
	public:

		CSkinHookImpl()
		{

		}
	
		void Initialize()
		{
			Init();
		}

		// Draw Helper, Scheme function proxy
	public:
		int GetSchemeWidth(int iPartId, int iStateId = 0)
		{
			return GetSchemeSize(iPartId, iStateId).cx;
		}

		int GetSchemeHeight(int iPartId, int iStateId = 0)
		{
			return GetSchemeSize(iPartId, iStateId).cy;
		}

		SIZE GetSchemeSize(int iPartId, int iStateId = 0)
		{
			WTL::CRect rc = GetSchemeRect(iPartId, iStateId);
			return rc.Size();
		}

		RECT GetSchemeRect(int iPartId, int iStateId = 0)
		{
			RECT rc;
			BOOL f = _scheme->GetRect(_classid, iPartId, iStateId, &rc);
			ASSERT(f);
			return rc;
		}

		COLORREF GetSchemeColor(int iPartId, int iStateId, int iPropId)
		{
			// GetColor(int iClassId, int iPartId, int iStateId, int iPropId, COLORREF *pColor)
			COLORREF ret;
			BOOL f = _scheme->GetColor(_classid, iPartId, iStateId, iPropId, &ret);
			ASSERT(f);
			return ret;
		}

		HRGN GetSchemeRegion(int iPartId, int iStateId)
		{
			HRGN rgn = 0;
			BOOL f = _scheme->GetRegion(_classid, iPartId, iStateId, 0, &rgn);
			ASSERT(f);
			return rgn;
		}

		BOOL Draw(HDC hdc, int iPartId, int iState, long dx, long dy, long dcx = 0, 
			long dcy = 0, DWORD dwRop = SRCCOPY)
		{
			return _scheme->Draw(hdc, _classid, iPartId, iState, dx, dy, 
				dcx, dcy, dwRop);
		}

	private:
		// 得到缺省的 ISkinScheme，SkinControl对象创建后的第一个函数调用
		void Init()
		{
			CComPtr<ISkinMgr> mgr;
			if (S_OK == GetSkinMgr(&mgr) && mgr)
				mgr->GetCurentScheme(&_scheme);
		}

	protected:
		CComPtr<ISkinScheme> _scheme;
		
	};
}; // namespace 