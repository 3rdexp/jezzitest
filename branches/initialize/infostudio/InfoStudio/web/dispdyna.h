#ifndef __DISPATCH_DYNAIMPL_H__
#define __DISPATCH_DYNAIMPL_H__

#pragma once

/*
template< class TImpl, class TDisp = IDispatch >
class ATL_NO_VTABLE IDynaDispImpl : public TDisp

template< class TImpl, class TDisp = IDispatch >
class ATL_NO_VTABLE DynaDispObject : public IDynaDispImpl< TImpl, TDisp >

template <class Base>
class CDispObject : public Base
*/

#define CheckPointer(p, e_val) if( 0==(p) ) return (e_val);

#include <string>
#include <map>

#ifdef _DEBUG
#include <typeinfo>
#endif

template< class T >
struct _ATL_DISPATCH_ENTRY
{
	OLECHAR *szName;
	DISPID dispid;
	UINT wFlags;
	VARTYPE vtReturn;
	UINT nArgs;
	VARTYPE vtArgs[10];
	void (__stdcall T::*pfn)();
};

#define BEGIN_DISPATCH_MAP(x) void InitDispMap(){
#define DISP_METHOD0(func, vtRet) \
	DispEntryAdd( OLESTR(#func), DISPID_UNKNOWN, DISPATCH_METHOD, vtRet, (DispCallFunction)##func, 0 );

#define DISP_METHOD1(func, vtRet, vtArg) \
	DispEntryAdd( OLESTR(#func), DISPID_UNKNOWN, DISPATCH_METHOD, vtRet, (DispCallFunction)##func, 1, vtArg );

#define DISP_METHOD2(func, vtRet, vtArg1, vtArg2) \
	DispEntryAdd( OLESTR(#func), DISPID_UNKNOWN, DISPATCH_METHOD, vtRet, (DispCallFunction)##func, 2, vtArg1, vtArg2 );

#define DISP_METHOD3(func, vtRet, vtArg1, vtArg2, vtArg3) \
	DispEntryAdd( OLESTR(#func), DISPID_UNKNOWN, DISPATCH_METHOD, vtRet, (DispCallFunction)##func, 3, vtArg1, vtArg2, vtArg3 );

#define DISP_METHOD9(func, vtRet, vtArg1, vtArg2, vtArg3, vtArg4, vtArg5, vtArg6, vtArg7, vtArg8, vtArg9) \
	DispEntryAdd( OLESTR(#func), DISPID_UNKNOWN, DISPATCH_METHOD, vtRet, (DispCallFunction)##func, 9, vtArg1, vtArg2, vtArg3, vtArg4, vtArg5, vtArg6, vtArg7, vtArg8, vtArg9 );

#define DISP_PROPGET(func, vtRet) \
	DispEntryAdd( OLESTR(#func), DISPID_UNKNOWN, DISPATCH_PROPERTYGET, vtRet, (DispCallFunction)get_##func, 0 );

#define DISP_PROPPUT(func, vtArg1) \
	DispEntryAdd(OLESTR(#func), DISPID_UNKNOWN, DISPATCH_PROPERTYPUT, VT_NULL, (DispCallFunction)put_##func, 1, vtArg1);

#define DISP_PROPGET1(func, vtRet, vtArg) \
	DispEntryAdd( OLESTR(#func), DISPID_UNKNOWN, DISPATCH_PROPERTYGET, vtRet, (DispCallFunction)get_##func, 1, vtArg );

#define DISP_CALL0(szName, dispid, wFlags, vtReturn, pfn) \
	DispEntryAdd(OLESTR(#szName), dispid, wFlags, vtReturn, (DispCallFunction)pfn, 0);

#define DISP_CALL1(szName, dispid, wFlags, vtReturn, pfn, vtarg1) \
	DispEntryAdd(szName, dispid, wFlags, vtReturn, (DispCallFunction)pfn, 1, vtarg1);

#define	DISP_CALL2(szName, dispid, wFlags, vtReturn, pfn, vtarg1, vtarg2) \
	DispEntryAdd(szName, dispid, wFlags, vtReturn, (DispCallFunction)pfn, 2, vtarg1, vtarg2);

#define	DISP_CALL3(szName, dispid, wFlags, vtReturn, pfn, vtarg1, vtarg2, vtarg3) \
	DispEntryAdd(szName, dispid, wFlags, vtReturn, (DispCallFunction)pfn, 3, vtarg1, vtarg2, vtarg3);

#define	DISP_CALL4(szName, dispid, wFlags, vtReturn, pfn, vtarg1, vtarg2, vtarg3, vtarg4) \
	DispEntryAdd(szName, dispid, wFlags, vtReturn, (DispCallFunction)pfn, 4, vtarg1, vtarg2, vtarg3, vtarg4);

#define	DISP_CALL5(szName, dispid, wFlags, vtReturn, pfn, vtarg1, vtarg2, vtarg3, vtarg4, vtarg5) \
	DispEntryAdd(szName, dispid, wFlags, vtReturn, (DispCallFunction)pfn, 5, vtarg1, vtarg2, vtarg3, vtarg4, vtarg5);

#define	DISP_CALL6(szName, dispid, wFlags, vtReturn, pfn, vtarg1, vtarg2, vtarg3, vtarg4, vtarg5, vtarg6) \
	DispEntryAdd(szName, dispid, wFlags, vtReturn, (DispCallFunction)pfn, 6, vtarg1, vtarg2, vtarg3, vtarg4, vtarg5, vtarg6);

#define	DISP_CALL7(szName, dispid, wFlags, vtReturn, pfn, vtarg1, vtarg2, vtarg3, vtarg4, vtarg5, vtarg6, vtarg7) \
	DispEntryAdd(szName, dispid, wFlags, vtReturn, (DispCallFunction)pfn, 7, vtarg1, vtarg2, vtarg3, vtarg4, vtarg5, vtarg6, vtarg7);

#define	DISP_CALL8(szName, dispid, wFlags, vtReturn, pfn, vtarg1, vtarg2, vtarg3, vtarg4, vtarg5, vtarg6, vtarg7, vtarg8) \
	DispEntryAdd(szName, dispid, wFlags, vtReturn, (DispCallFunction)pfn, 7, vtarg1, vtarg2, vtarg3, vtarg4, vtarg5, vtarg6, vtarg7, vtarg8);

#define	DISP_CALL9(szName, dispid, wFlags, vtReturn, pfn, vtarg1, vtarg2, vtarg3, vtarg4, vtarg5, vtarg6, vtarg7, vtarg8, vtarg9) \
	DispEntryAdd(szName, dispid, wFlags, vtReturn, (DispCallFunction)pfn, 9, vtarg1, vtarg2, vtarg3, vtarg4, vtarg5, vtarg6, vtarg7, vtarg8, vtarg9);

#define	DISP_CALL10(szName, dispid, wFlags, vtReturn, pfn, vtarg1, vtarg2, vtarg3, vtarg4, vtarg5, vtarg6, vtarg7, vtarg8, vtarg9, vtarg10) \
	DispEntryAdd(szName, dispid, wFlags, vtReturn, (DispCallFunction)pfn, 10, vtarg1, vtarg2, vtarg3, vtarg4, vtarg5, vtarg6, vtarg7, vtarg8, vtarg9, vtarg10);

#define END_DISPATCH_MAP() }

class AutoPerf
{
public:
	AutoPerf(const char *p) :_name(p)
	{
		QueryPerformanceCounter(&val);
	}
	~AutoPerf()
	{
		LARGE_INTEGER val2;
		QueryPerformanceCounter(&val2);
		LARGE_INTEGER freq;
		QueryPerformanceFrequency(&freq);
		ATLTRACE2(atlTracePerf, 0, "AutoPerf:%s %d\n", _name.c_str(), (val2.LowPart - val.LowPart)/freq.LowPart * 1000);
	}
	LARGE_INTEGER val;
	std::string _name;
};

// 要求：
// 1 GetIDsOfNames, Invoke 中查找速度
// 2 能适用于小对象
template< class TImpl, class TDisp = IDispatch >
class ATL_NO_VTABLE IDynaDispImpl : public TDisp
{
public:
	STDMETHOD(GetTypeInfoCount)(UINT* pctinfo)
	{
		ATLTRACE2(atlTraceWarn, 1, "Warn: GetTypeInfoCount\n");
		if(pctinfo)
			*pctinfo = 0; 
		return S_OK;
	}

	STDMETHOD(GetTypeInfo)(UINT /*itinfo*/, LCID /*lcid*/, ITypeInfo** /*pptinfo*/)
	{
		ATLTRACE2(atlTraceWarn, 1, "Warn: GetTypeInfo\n");
		return E_NOTIMPL;
	}
	STDMETHOD(GetIDsOfNames)(REFIID /*riid*/, LPOLESTR* rgszNames, UINT cNames, 
				LCID /*lcid*/, DISPID* rgdispid)
	{
#if 0
		char sz[100];
		wsprintfA(sz, "GetIDsOfNames %S", rgszNames[0]);
		AutoPerf aperf(sz);
#endif
		HRESULT hr = S_OK;
		for( UINT i=0; i<cNames; i++ ) 
		{
			rgdispid[i] = Name2Dispid( rgszNames[i] );
			if( DISPID_UNKNOWN == rgdispid[i] )
			{
				ATLTRACE2(atlTraceWarn, 1, "Warn: GetIDsOfNames name=%S return DISPID_UNKNOWN\n", rgszNames[i]);
			}
#ifdef _DEBUG
			ATLTRACE("__ %S %d\n", rgszNames[i], rgdispid[i] &~Unknown_Dispid_Mask );
#endif
		}
		return S_OK;
	}
	STDMETHOD(Invoke)(DISPID dispidMember, REFIID riid,
			LCID lcid, WORD wFlags, DISPPARAMS* pdispparams, VARIANT* pvarResult,
			EXCEPINFO* pexcepinfo, UINT* puArgErr)
	{
		if( (DISPATCH_PROPERTYPUT!=wFlags) && (pdispparams->cNamedArgs>0) )
		{
			ATLTRACE2(atlTraceError, 1, "Error: Invoke parameters error, dispidMember=%d\n", dispidMember);
			return DISP_E_NONAMEDARGS;
		}
		
		if( DispidExists(dispidMember) )
		{
#if 0
		char sz[100];
		wsprintfA(sz, "Invoke %d", dispidMember);
		AutoPerf aperf(sz);
#endif
#if 0
			if(dispidMember<100)
			{
				ATLTRACE("__ call %d\n", dispidMember);
			}
#endif
			DispatchEntry * pde = EntryFromDispid(dispidMember);

			if( pdispparams->cArgs != pde->nArgs )
			{
				ATLTRACE2(atlTraceError, 0, "Error: Invoke dispid=%d param count=%d error, right count=%d\n", dispidMember, pdispparams->cArgs, pde->nArgs);
				return DISP_E_BADPARAMCOUNT;
			}

			int nArgs = pde->nArgs;
			VARIANTARG** ppVarArgs = nArgs ? (VARIANTARG**)_alloca(sizeof(VARIANTARG*)*nArgs) : NULL;
			// VARIANTARG* pVarArgs = nArgs ? (VARIANTARG*)_alloca(sizeof(VARIANTARG)*nArgs) : NULL;
			int i;
			for( i=0; i<nArgs; i++ ) 
			{
				ppVarArgs[i] = &pdispparams->rgvarg[nArgs - i - 1];
			}
			TImpl *pT = static_cast<TImpl*>(this);
			CComStdCallThunk<TImpl> thunk;
			thunk.Init(pde->pfn, pT);

			CComVariant tmpResult;
			if( pvarResult==NULL ) 
				pvarResult = &tmpResult;

			HRESULT hr = ::DispCallFunc(
			   &thunk,
			   0,
			   CC_STDCALL,
			   pde->vtReturn,
			   nArgs,
			   pde->vtArgs,
			   ppVarArgs,
			   pvarResult);
			if (FAILED(hr))
			{
				ATLTRACE2(atlTraceError, 0, "Error: Invoke DispCallFunc error hr=0x%x\n", hr);
			}

			// for( i=0; i<nArgs; i++ ) 
			//	::VariantClear(&pVarArgs[i]);
			return hr;
		}
		TImpl *pT = static_cast<TImpl*>(this);
		return pT->IDispatch_Invoke(dispidMember, riid, lcid, wFlags, pdispparams, pvarResult, pexcepinfo, puArgErr);
		// return DISP_E_MEMBERNOTFOUND;
	}
public:
	typedef _ATL_DISPATCH_ENTRY<TImpl> DispatchEntry;
	typedef void (__stdcall TImpl::*DispCallFunction)();
protected:
	IDynaDispImpl()
	{
		TImpl* pT = static_cast<TImpl*>(this);
		pT->InitDispMap();
	}
	virtual ~IDynaDispImpl() {}

	int EntrySize() const { return _entries.size(); }
	DISPID Name2Dispid(LPOLESTR rgszNames) const
	{
		NameDispid::const_iterator it = _names.find( rgszNames );
		if( it!=_names.end() )
			return it->second;
		return DISPID_UNKNOWN;
	}
	BOOL DispidExists(DISPID dispid)
	{
		DispidEntry::const_iterator it = _entries.find(dispid);
		return it != _entries.end();
	}
	DispatchEntry * EntryFromDispid(DISPID dispid)
	{
		DispidEntry::iterator it = _entries.find(dispid);
		if( it != _entries.end() )
			return &it->second;
		
		ATLTRACE2(atlTraceError, 0, "Error: Invoke function dispid=%d not exists, in IDynaDispImpl::EntryFromDispid\n", dispid);
		return 0;
	}
	enum
	{
		Unknown_Dispid_Mask = 0x0f000000,
	};
	
	BOOL DispEntryAdd(const DispatchEntry & de)
	{
		// 1 对没有DISPID的分配一个 Unknown_Dispid_Mask
		DISPID dispid = de.dispid;
		if( DISPID_UNKNOWN == dispid )
		{
			dispid = Unknown_Dispid_Mask | (1 + _entries.size());
		}

		bool f = _entries.insert(  DispidEntry::value_type(dispid, de) ).second;
		ATLASSERT(f);
		
		if( de.szName )
		{	
			ATLASSERT( DISPID_UNKNOWN == Name2Dispid(de.szName) );
			
			return _names.insert(
				NameDispid::value_type(de.szName, dispid)
				).second == true;
		}
		return true;
	}
	
	BOOL DispEntryAdd(OLECHAR* szName, DISPID dispid, UINT wFlags, VARTYPE vtRet, 
			DispCallFunction pfn, int nArgs, ...)
	{
		va_list ArgList;
		va_start(ArgList, nArgs);

		DispatchEntry de;
		ZeroMemory(&de, sizeof(DispatchEntry));
		de.szName = szName;
		de.dispid = dispid;
		de.wFlags = wFlags;
		de.vtReturn = vtRet;
		de.nArgs = nArgs;
		de.pfn = pfn;

		for(int i=0; i<nArgs; i++)
		{
			VARTYPE vt = va_arg(ArgList, VARTYPE);
			de.vtArgs[i] = vt;
		}
		va_end(ArgList);
		return DispEntryAdd(de);
	}	
	
	void DispEntryRemoveAll()
	{
		_names.clear();
		_entries.clear();
	}
private:
	typedef std::map<std::wstring, DISPID> NameDispid;
	NameDispid _names;

	typedef std::map<DISPID, DispatchEntry> DispidEntry;
	DispidEntry _entries;
};


template< class TImpl, class TDisp = IDispatch >
class ATL_NO_VTABLE DynaDispObject  
				: public IDynaDispImpl< TImpl, TDisp >
{
public:
#if 0
	DynaDispObject() : m_dwRef(0) {}
	void FinalRelease() {}
	ULONG InternalAddRef()
	{
		return ++m_dwRef;
	}
	ULONG InternalRelease()
	{
		return --m_dwRef;
	}
	void SetVoid(void* pv) {}
	void InternalFinalConstructAddRef() {}
	void InternalFinalConstructRelease()
	{
		ATLASSERT(m_dwRef == 0);
	}
	HRESULT FinalConstruct()
	{
		return S_OK;
	}
#endif
	virtual HRESULT _InternalQueryInterface(REFIID riid, void** ppvObj)
	{
		CheckPointer(ppvObj, E_POINTER);
		if( ATL::InlineIsEqualUnknown(riid) )
		{
			*ppvObj = static_cast<IUnknown*>(this);
			AddRef();
			return S_OK;
		}
		else if( InlineIsEqualGUID(IID_IDispatch, riid) )
		{
			*ppvObj = static_cast<IDispatch*>(this);
			AddRef();
			return S_OK;
		}
		// TODO: compile check, if TDisp == IDispatch
		else if( InlineIsEqualGUID(__uuidof(TDisp), riid) )
		{
			*ppvObj = static_cast<TDisp*>(this);
			AddRef();
			return S_OK;
		}
		ATLTRACE2(atlTraceWarn, 0, "Warn:QueryInterface riid=%S return E_NOINTERFACE, %s\n", CComBSTR(riid), typeid(this).name());
		return E_NOINTERFACE;
	}

	virtual HRESULT IDispatch_Invoke(DISPID dispidMember, REFIID /*riid*/,
			LCID /*lcid*/, WORD wFlags, DISPPARAMS* pdispparams, VARIANT* pvarResult,
			EXCEPINFO* /*pexcepinfo*/, UINT* /*puArgErr*/)
	{
//		ATLTRACE2( atlTraceWarn, 0, "Warn:IDispatch_Invoke dispid=%d return MEMBER NOT FOUND\n", dispidMember);
//		if( dispidMember == DISPID_HTMLWINDOWEVENTS2_ONLOAD )
//			__asm int 3;
//		if( (int)dispidMember < 0 )
//			ATLTRACE2( atlTraceLog, 1, "Log:IDispatch_Invoke: %08x\n", dispidMember);
//		else
//			ATLTRACE2( atlTraceLog, 1, "Log:IDispatch_Invoke: %d\n", dispidMember);
		return DISP_E_MEMBERNOTFOUND;
	}
protected:
//	DWORD m_dwRef;
};

#if 0

  #define CDispObject CComObject

#else
// just like CComObject
template <class Base>
class CDispObject : public Base
{
public:
	typedef Base _BaseClass;
	CDispObject() : m_dwRef(0) {}
	~CDispObject() {  m_dwRef = 1L; }
	STDMETHOD_(ULONG, AddRef)() 
	{
#ifdef _DEBUG
		++m_dwRef;
		TraceRef("AddRef");
		return m_dwRef;
#endif
		return ++m_dwRef;
	}
	STDMETHOD_(ULONG, Release)()
	{
		DWORD dw = -- m_dwRef;
#ifdef _DEBUG
		TraceRef("Release");
#endif
		if( dw == 0 )
			delete this;
		return dw;
	}
	
	STDMETHOD(QueryInterface)(REFIID iid, void ** ppvObject)
	{return _InternalQueryInterface(iid, ppvObject);}
	template <class Q>
	HRESULT STDMETHODCALLTYPE QueryInterface(Q** pp)
	{
		return QueryInterface(__uuidof(Q), (void**)pp);
	}

	static HRESULT WINAPI CreateInstance(CDispObject<Base>** pp)
	{
#if 0
		AutoPerf aperf("CreateInstance ");
#endif		CheckPointer(pp, E_POINTER);
		HRESULT hRes = E_OUTOFMEMORY;
		CDispObject<Base>* p = 0;
		ATLTRY(p = new CDispObject<Base>())
		if (p != NULL)
		{
			hRes = S_OK;
			p->AddRef();
			if (hRes != S_OK)
			{
				delete p;
				p = NULL;
			}
		}
		*pp = p;
		return hRes;
	}
private:
	DWORD m_dwRef;
#ifdef _DEBUG
	void TraceRef(const char * desc)
	{
		// ATLTRACE2( atlTraceDispRef, 1, "RefCount: %s %d %s\n", typeid(this).name(), m_dwRef, desc );
	}
#endif
};
#endif

#endif //__DISPATCH_DYNAIMPL_H__