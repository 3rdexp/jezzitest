#pragma once

#include <hash_map>
#include <crtdbg.h>
#include <atlcrack.h>
#include <boost/shared_ptr.hpp>
#include <atlwin.h>
// ------------------------------------------------------
// We use UxTheme's Part, State Define
#include <tmschema.h>
// ms-help://MS.MSDNQTR.2004JAN.1033/shellcc/platform/commctls/userex/topics/partsandstates.htm
// ------------------------------------------------------

#include "../skinmsg.h"
#include "../libcoolsb/coolscroll.h"

namespace Skin {

using ATL::_ATL_MSG;
// see ../readme.txt

// XXXPolicy SkinControlImpl ��ģ�����
// ��������α���ʼ��
// Concept: 
//   bool Install(....); 
//   bool UnInstall(...)
//   WNDPROC GetDefaultProc()

//
class HookPolicy
{
public:
	HookPolicy() : _defaultproc(0) {}
	bool Install(HWND hWnd, HINSTANCE hInst, LPCSTR szClassName, WNDPROC proc)
	{
#pragma warning(disable : 4311 4312)
		WNDPROC oldproc = (WNDPROC)SetWindowLongPtr(hWnd, GWLP_WNDPROC, (DWORD)proc);
#pragma warning(default: 4311 4312)
		if (oldproc)
			_defaultproc = oldproc;
		return _defaultproc!=0;
	}

	bool Uninstall(HWND hWnd, HINSTANCE hInst, LPCSTR szClassName)
	{
#pragma warning(disable : 4311 4312)
		WNDPROC proc = (WNDPROC)SetWindowLongPtr(hWnd, GWLP_WNDPROC, (DWORD)GetDefaultProc());
#pragma warning(default: 4311 4312)
		if (proc)
			_defaultproc = 0;
		// ASSERT( proc == (WNDPROC)GetControlProc() );
		return proc!=0;
	}
	WNDPROC GetDefaultProc()
	{
		return _defaultproc;
	}
private:
	WNDPROC _defaultproc;
};

// ���ڷ�ϵͳwindow
class RegisterPolicy
{
public:
    RegisterPolicy() : _defaultproc(0) {}
    bool Install(HWND hWnd, HINSTANCE hInst, LPCSTR szClassName, WNDPROC proc)
    {
        WNDCLASS wc = {0};
        
        if (!GetClassInfo(hInst, szClassName, &wc))
        {
            // ��ע��Ĵ���
            wc.hInstance = hInst;
            // TODO: wc.style
            wc.lpszClassName = szClassName;
            wc.hbrBackground = (HBRUSH)(COLOR_WINDOW+1); // TODO: right?
        }
        
        WNDPROC temp = wc.lpfnWndProc;
        wc.lpfnWndProc = proc;

        ATOM atom = RegisterClass(&wc);
        if( atom )
            _defaultproc = temp;
        ASSERT(atom);
        return atom != 0;
    }

    bool Uninstall(HWND hWnd, HINSTANCE hInst, LPCSTR szClassName)
    {
        // һ���������ע��Ĵ��ڣ��õ��û�ʧ��
        BOOL r = UnregisterClass(szClassName, hInst);
        ASSERT(r);
        if (r)
            _defaultproc = 0;
        return r == TRUE;
    }
    WNDPROC GetDefaultProc()
    {
        return _defaultproc;
    }
private:
    WNDPROC _defaultproc;
};

// ϵͳ��window, �� button, edit ��
class ClassPolicy
{
public:
    ClassPolicy() : _defaultproc(0) {}
    bool Install(HWND hWnd, HINSTANCE hInst, LPCSTR szClassName, WNDPROC proc)
    {
        HWND hCreateWnd = ::CreateWindowEx(0, szClassName, 0, WS_DISABLED, 0, 0, 0, 0,
            NULL, // parent Wnd
            0, // hMenu
            hInst, 0 );
#pragma warning(disable : 4311 4312)
        WNDPROC oldproc = (WNDPROC)SetClassLongPtr(hCreateWnd, GCLP_WNDPROC, (DWORD)proc);
#pragma warning(default: 4311 4312)
        if (oldproc)
            _defaultproc = oldproc;
        ::DestroyWindow(hCreateWnd);
        return _defaultproc!=0;
    }

    bool Uninstall(HWND hWnd, HINSTANCE hInst, LPCSTR szClassName)
    {
        HWND hCreateWnd = ::CreateWindowEx(0, szClassName, 0, WS_DISABLED, 0, 0, 0, 0,
            NULL, // parent Wnd
            0, // hMenu
            hInst, 0 );
        #pragma warning(disable : 4311 4312)
        WNDPROC proc = (WNDPROC)SetClassLongPtr(hCreateWnd, GCLP_WNDPROC, (DWORD)GetDefaultProc());
        #pragma warning(default: 4311 4312)
        if (proc)
            _defaultproc = 0;
        ::DestroyWindow(hCreateWnd);
        // ASSERT( proc == (WNDPROC)GetControlProc() );
        return proc!=0;
    }
    WNDPROC GetDefaultProc()
    {
        return _defaultproc;
    }
private:
    WNDPROC _defaultproc;
};

/*----------------------------------------------------------------------------//

ControlT: ʵ�ָ�window����Ϣ�����ģ�����
    Concept:
        static LPCSTR ControlT::GetWndClassName();
        class_id = 1; // ControlT::class_id

BaseT: ������ CWindow, CButton, CXXXButton
    Concept:
        HWND m_hWnd;


TODO: 
�ⲿ������Ҫ���� InstallPolicy���ı��Լ��������ڵ�����
  ��ȻҲ��hack
�����DrawXXX


\\----------------------------------------------------------------------------*/

template<class ControlT, class BaseT, class InstallPolicy = ClassPolicy>
class SkinControlImpl : public BaseT
{
	typedef HWND handle;

    typedef BaseT base_type;
	typedef SkinControlImpl<ControlT, BaseT, InstallPolicy> this_type;
    typedef ControlT derived_type;
	
    typedef std::hash_map<handle, boost::shared_ptr<ControlT> > handle_map;

	typedef std::hash_map<handle, handle> hook_map;

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
        CRect rc = GetSchemeRect(iPartId, iStateId);
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

	BOOL DrawText(HDC hdc, int iPartId, int iState, LPCSTR szText, DWORD dwTextFlags, DWORD dwTextFlags2, const RECT *pRect)
	{
		return _scheme->DrawText(hdc, _classid, iPartId, iState, szText, dwTextFlags, dwTextFlags2, pRect);
	}

#if 0
    BOOL Draw(HDC hdc, int iPartId, int iStateId, 
        long dx, long dy, long dcx, long dcy, long sx, long sy, long scx, long scy, 
        DWORD dwRop = SRCCOPY)
    {
    }
    STDMETHOD_(BOOL, TransparentDraw)(HDC hdc, int iClassId, int iPartId, int iStateId, 
        long dx, long dy, long dcx = 0, long dcy = 0) PURE;
    STDMETHOD_(BOOL, TransparentDraw)(HDC hdc, int iClassId, int iPartId, int iStateId, 
        long dx, long dy, long dcx, long dcy,long sx, long sy, long scx, long scy) PURE;
    STDMETHOD_(BOOL, TransparentDraw)(HDC hdc, int iClassId, int iPartId, int iStateId, 
        const RECT* lprc) PURE;
    
#endif

public:

	static bool Install(HINSTANCE hInst)
    {
        // derived_type:: �����������õ��� CWindow����������
        return _installer.Install(0, hInst, derived_type::GetWndClassName(), 
            GetControlProc());
    }
    static bool Uninstall(HINSTANCE hInst)
    {
        return _installer.Uninstall(0, hInst, derived_type::GetWndClassName());
    }


	//����hook��֧��
	static bool InstallHook( HWND hWnd )
	{
		// derived_type:: �����������õ��� CWindow����������
		//�ж�һ��,�Ƿ��Ѿ�hook��
		hook_map::const_iterator it = _hook_maps.lower_bound(hWnd);
		if (it == _hook_maps.end()) // ��һ�δ���֮
		{
			_hook_maps.insert( std::make_pair(hWnd, hWnd) );
			return _installer.Install( hWnd, 0, "",  GetControlProc() );
		}
		else
			return false;
	}

	static bool UnInstallHook( HWND hWnd )
	{
		return _installer.Uninstall( hWnd, 0, "" );
	}

	WNDPROC getDefaultProc()
	{
		return _installer.GetDefaultProc();
	}


	ATL::CWindow GetChildWnd(LPCTSTR szClass)
	{
		ATL::CWindow wndChild = GetWindow(GW_CHILD); 

		while ( wndChild.m_hWnd )    
		{
			if (IsClass(wndChild.m_hWnd, szClass))
				return wndChild;

			wndChild = wndChild.GetWindow(GW_HWNDNEXT);
		}

		return ATL::CWindow( NULL );
	}

	BOOL IsClass(HWND hWnd, LPCTSTR szClass)
	{
		if (hWnd)
		{
			char szWndClass[128] = "";

			::GetClassName(hWnd, szWndClass, 127);
			return (lstrcmpi(szClass, szWndClass) == 0);
		}

		return FALSE;
	}


protected:
    SkinControlImpl() 
        : _enable(true) 
        , m_pCurrentMsg(0)
#ifdef LOOP_DEBUG 
        , indent(0)
#endif  
    {}

    // BaseT û�� virtual ~BaseT()
    // �ܽ�������� new derived; delete base*
    virtual ~SkinControlImpl() 
    {
        // TRACE("~ %p\n", this);
    } 

public: // ��Ҫ��ģ�����������
	static WNDPROC GetDefaultProc() { return _installer.GetDefaultProc(); }

	// �� ATL::CWindowImplBaseT::GetWindowProc ����
	// �������Ϊ GetWindowProc ������warning(some BaseT)
	static WNDPROC GetControlProc() { return (WNDPROC)ControlProc; }

	const _ATL_MSG* GetCurrentMessage() const
	{
		return m_pCurrentMsg;
	}

	// "handled" management for cracked handlers
	BOOL IsMsgHandled() const
	{
        _ASSERTE(_CrtCheckMemory( ));
		const _ATL_MSG* pMsg = GetCurrentMessage();
		ATLASSERT(pMsg != NULL);
		ATLASSERT(pMsg->cbSize >= sizeof(_ATL_MSG));
		return pMsg->bHandled;
	}
	void SetMsgHandled(BOOL bHandled)
	{
        _ASSERTE(_CrtCheckMemory( ));
		_ATL_MSG* pMsg = (_ATL_MSG*)GetCurrentMessage(); // override const
		ATLASSERT(pMsg != NULL);
		ATLASSERT(pMsg->cbSize >= sizeof(_ATL_MSG));
		pMsg->bHandled = bHandled;
	}

	LRESULT DefWindowProc()
	{
		const _ATL_MSG* pMsg = GetCurrentMessage();
		LRESULT lRes = 0;
		if (pMsg != NULL)
			lRes = DefWindowProc(pMsg->message, pMsg->wParam, pMsg->lParam);
		return lRes;
	}

	LRESULT DefWindowProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
        WNDPROC dw = GetDefaultProc();
        if (!dw)
            dw = ::DefWindowProc;
		return ::CallWindowProc(dw, m_hWnd, uMsg, wParam, lParam);
	}

	HRESULT InstallScrollBar()
	{
		BOOL f = InitializeCoolSB(m_hWnd);
		ATLASSERT( f );
		if( f )
		{
			RECT rc;
			BOOL fScroll = FALSE;
			if ( _scheme )
				fScroll = _scheme->GetRect(SCROLLBAR, SBP_ARROWBTN, 1, &rc);

			int nWidth = 0 ;
			int nHeight = 0;

			if ( fScroll )
			{
				nWidth = rc.right - rc.left;
				nHeight = rc.bottom - rc.top;
			}

			f = CoolSB_SetStyle(m_hWnd, SB_BOTH, CSBS_NORMAL);
			ATLASSERT( f );
			f = CoolSB_SetSize(m_hWnd, SB_BOTH, nWidth > 0 ? nWidth : 15, nHeight > 0 ? nHeight : 15);
			ATLASSERT( f );
			f = CoolSB_SetMinThumbSize(m_hWnd, SB_BOTH, nWidth);
			ATLASSERT( f );
		}
		return S_OK;
	}

	BEGIN_MSG_MAP(this_type)
		MESSAGE_HANDLER(WMS_ENABLE, OnEnable)
		MESSAGE_HANDLER(WMS_SETSTYLE, OnSetStyle)
		NOTIFY_CODE_HANDLER(NM_COOLSB_CUSTOMDRAW, OnScrollCustomDraw)
	END_MSG_MAP()

    // common skin message procedure
	LRESULT OnEnable(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		if( wParam != _enable )
		{
			_enable = wParam;
			Invalidate();
		}
		return 0;
	}

	LRESULT OnSetStyle(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		if( wParam != _classid )
		{
			_classid = wParam;
			Invalidate();
		}
		return 0;
	}

	LRESULT OnScrollCustomDraw( int wParam, LPNMHDR lParam, BOOL& bHandled)
	{
		return HandleCustomDraw(wParam, (NMCSBCUSTOMDRAW *)lParam);
	}
    // common skin message procedure end

private:
    // �õ�ȱʡ�� ISkinScheme��SkinControl���󴴽���ĵ�һ����������
    void Init()
    {
        CComPtr<ISkinMgr> mgr;
        if (S_OK == GetSkinMgr(&mgr) && mgr)
            mgr->GetCurentScheme(&_scheme);
    }

    static std::hash_map<HWND, boost::shared_ptr<ControlT> > _handle_maps;

	static std::hash_map<HWND, HWND> _hook_maps;

    static InstallPolicy _installer;

public:
	static LRESULT ControlProc(handle hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
        if ((uMsg < WM_MOUSEFIRST || uMsg > WM_MOUSELAST)
                    && uMsg != WM_NCHITTEST && uMsg != WM_SETCURSOR)
        //    ATLTRACE("%04x SkinCtrl\n", uMsg);
		_ASSERTE( _CrtCheckMemory( ) );

		BOOL bRet = FALSE;
		LRESULT lRes = 0;
        boost::shared_ptr<derived_type> safeptr;

		_ATL_MSG msg(hWnd, uMsg, wParam, lParam);
		
		handle_map::const_iterator it = _handle_maps.lower_bound(hWnd);
		if (it == _handle_maps.end()) // ��һ�δ���֮
		{
			// TODO: function this
			derived_type * p = new derived_type;
			p->m_hWnd = hWnd;
			it = _handle_maps.insert( std::make_pair(hWnd, p) ).first;

            // TRACE("n ew: %p\n", p);

			p->Init();

			p->OnFirstMessage();
		}

		if (it->second)
		{
            // 
            // ProcessWindowMessage ���ܲ���virtual�ġ���Ҫ����ÿ�����͵�ProcessWindowMessage
            // ����˳���� SkinButton, SkinControlImpl, BaseT
            // BaseT ������ CWindowImpl ���͵���
            // 
			// call SkinButton
			safeptr = it->second;

            // safeptr->m_pCurrentMsg[safeptr->depth_++] = &msg;
            // ������������ˣ�����ĸ���ȷ
            const _ATL_MSG* pOldMsg = safeptr->m_pCurrentMsg;
            safeptr->m_pCurrentMsg = &msg;
			if ( uMsg == WM_NCPAINT )
			{
				TRACE("WM_NCPAINT  %p %d\n", hWnd, uMsg);
			}
			if ( ( safeptr->_enable || uMsg == WMS_ENABLE ) && uMsg != WM_NCDESTROY )
			{
                bRet = safeptr->derived_type::ProcessWindowMessage(hWnd, uMsg, wParam, lParam, lRes, 0);
                _ASSERTE(_CrtCheckMemory( ));

				if (!bRet)
				{
					// call SkinControlImpl
					this_type * pd = static_cast<this_type *>(safeptr.get());
					ASSERT(pd);
                    // TRACE("2 %p %d, %p::%p\n", hWnd, uMsg, pd, pd->ProcessWindowMessage);
					// ATTENTION: this_type:: (maybe ProcessWindowMessage not virtual)
					bRet = pd->this_type::ProcessWindowMessage(hWnd, uMsg, 
                        wParam, lParam, lRes, 0);
                    _ASSERTE(_CrtCheckMemory( ));
				}
#if 0
                // ATTENTION: ���ʱ�������������ֱ�ӵ��� BaseT::ProcessWindowMessage 
                // ʡ���� CHAIN_MSG_MAP �����ǵ���SkinFramexxx����Ϣִ�л�������
                // SkinFrame ������� CHAIN_MSG_MAP
				if (!bRet)
				{
                    // ʹ����vc7�����ԣ����BaseT==CWindowImpl�ȣ���Ҫ�ٵ����䴦�����
					// call BaseT::
					__if_exists( base_type::ProcessWindowMessage )
					{
						base_type * pb = static_cast<base_type *>(p);
						ASSERT(pb);
                        TRACE("3 %p %d, %p::%p\n", hWnd, uMsg, pb, pb->ProcessWindowMessage);
						// ATTENTION: base_type:: (maybe ProcessWindowMessage not virtual)
						bRet = pb->base_type::ProcessWindowMessage(hWnd, uMsg, 
                            wParam, lParam, lRes, 0);
                        _ASSERTE(_CrtCheckMemory( ));
					}
				}
#endif
			}
			
			if ( uMsg != WM_NCDESTROY)
				safeptr->m_pCurrentMsg = pOldMsg;
		}

		if (!bRet)
        {

			if(uMsg != WM_NCDESTROY)
			{
				WNDPROC dw = GetDefaultProc();
				//if (!dw)
				//	dw = ::DefWindowProc;

				lRes = ::CallWindowProc(dw, hWnd, uMsg, wParam, lParam);
			}
			else
			{
				// unsubclass, if needed
				LONG_PTR pfnWndProc = ::GetWindowLongPtr(hWnd, GWLP_WNDPROC);
				
				WNDPROC dw = GetDefaultProc();
				//if (!dw)
				//	dw = ::DefWindowProc;

				lRes = ::CallWindowProc(dw, hWnd, uMsg, wParam, lParam);

				hook_map::const_iterator it = _hook_maps.lower_bound(hWnd);
				if (it != _hook_maps.end()) // ��һ�δ���֮
				{
					//������
					//if(GetDefaultProc() != ::DefWindowProc && ::GetWindowLongPtr(hWnd, GWLP_WNDPROC) == pfnWndProc)
						::SetWindowLongPtr(hWnd, GWLP_WNDPROC, (LONG_PTR)GetDefaultProc());
				}
				// mark window as destryed
				safeptr->m_hWnd = 0;

				_handle_maps.erase(hWnd);

				_hook_maps.erase(hWnd);
				
			}            
        }
		
		_ASSERTE( _CrtCheckMemory( ) );
		return lRes;
	}

	
#if 0
private:
    // TODO: find out the way to unload dll from process memory
    // �������hooked�Ĵ���û������
    // ʵ��û��ɶ�ô������ǻᵼ�±���
	static void Recover()
	{
		handle_map::iterator it = _handle_maps.begin();
		while( it != _handle_maps.end() )
		{
			ASSERT(it->first);

			LONG_PTR proc = ::SetWindowLongPtr(it->first, GWLP_WNDPROC, 
                (DWORD_PTR)GetDefaultProc());
			ASSERT(proc == (LONG_PTR)ControlProc);

			delete it->second;
			it = _handle_maps.erase(it);
		}
	}
#endif

protected:
	const _ATL_MSG * m_pCurrentMsg;
	CComPtr<ISkinScheme> _scheme;

	unsigned	_enable : 1;
	int			_classid;
#ifdef LOOP_DEBUG 
    int indent;
#endif    
};

template<class ControlT, class BaseT, class InstallPolicy>
InstallPolicy SkinControlImpl<ControlT, BaseT, InstallPolicy>::_installer;

template<class ControlT, class BaseT, class InstallPolicy>
std::hash_map<HWND, boost::shared_ptr<ControlT>  >
SkinControlImpl<ControlT, BaseT, InstallPolicy>::_handle_maps;

template<class ControlT, class BaseT, class InstallPolicy>
std::hash_map<HWND, HWND>
SkinControlImpl<ControlT, BaseT, InstallPolicy>::_hook_maps;


} // namespace Skin
