#pragma once

#include <hash_map>
#include <crtdbg.h>
#include <atlcrack.h>
// ------------------------------------------------------
// We use UxTheme's Part, State Define
#include <tmschema.h>

// BEGIN_TM_CLASS_PARTS(WINDOW)
#define WP_MENUBAR 38
// ------------------------------------------------------

#include "../skinmsg.h"
#include "../libcoolsb/coolscroll.h"

namespace Skin {

// TODO: place TypeHolder/Int2Type to anywhere
template<typename T>
struct TypeHolder
{
	typedef T type;
};

template<unsigned int Val, typename T>
struct Int2Type
{
	enum { value = Val };
	typedef T type;
};

// see ../readme.txt

// XXXPolicy SkinControlImpl 的模版参数
// 窗口类如何被初始化
// Concept: 
//   bool Install(....); 
//   bool UnInstall(...)
//   WNDPROC GetDefaultProc()

// 用于非系统window
class RegisterPolicy
{
public:
    RegisterPolicy() : _defaultproc(0) {}
    bool Install(HINSTANCE hInst, LPCSTR szClassName, WNDPROC proc)
    {
        WNDCLASS wc = {0};
        
        if (!GetClassInfo(NULL, szClassName, &wc))
        {
            // 新注册的窗口
            wc.hInstance = hInst;
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

    bool Uninstall(HINSTANCE hInst, LPCSTR szClassName)
    {
        // 一般如果还有注册的窗口，该调用会失败
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

// 系统的window, 如 button, edit 等
class ClassPolicy
{
public:
    ClassPolicy() : _defaultproc(0) {}
    bool Install(HINSTANCE hInst, LPCSTR szClassName, WNDPROC proc)
    {
        HWND hWnd = ::CreateWindowEx(0, szClassName, 0, WS_DISABLED, 0, 0, 0, 0,
            NULL, // parent Wnd
            0, // hMenu
            hInst, 0 );
#pragma warning(disable : 4311 4312)
        WNDPROC oldproc = (WNDPROC)SetClassLongPtr(hWnd, GCLP_WNDPROC, (DWORD)proc);
#pragma warning(default: 4311 4312)
        if (oldproc)
            _defaultproc = oldproc;
        ::DestroyWindow(hWnd);
        return _defaultproc!=0;
    }

    bool Uninstall(HINSTANCE hInst, LPCSTR szClassName)
    {
        HWND hWnd = ::CreateWindowEx(0, szClassName, 0, WS_DISABLED, 0, 0, 0, 0,
            NULL, // parent Wnd
            0, // hMenu
            hInst, 0 );
        #pragma warning(disable : 4311 4312)
        WNDPROC proc = (WNDPROC)SetClassLongPtr(hWnd, GCLP_WNDPROC, (DWORD)GetDefaultProc());
        #pragma warning(default: 4311 4312)
        if (proc)
            _defaultproc = 0;
        ::DestroyWindow(hWnd);
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

ControlT: 实现该window的消息处理的模版参数
    Concept:
        static LPCSTR ControlT::GetWndClassName();
        class_id = 1; // ControlT::class_id

BaseT: 可以是 CWindow, CButton, CXXXButton
    Concept:
        HWND m_hWnd;


TODO: 
外部可能需要访问 InstallPolicy，改变自己创建窗口的属性
  当然也可hack
更多的DrawXXX


\\----------------------------------------------------------------------------*/
template<class ControlT, class BaseT, class InstallPolicy = ClassPolicy>
class SkinControlImpl : public BaseT
{
	typedef HWND handle;
	typedef ControlT derived_type;
	typedef SkinControlImpl<ControlT, BaseT, InstallPolicy> this_type;
	typedef BaseT base_type;
	typedef typename std::hash_map<handle, derived_type *> handle_map;

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
        BOOL f = _scheme->GetRect(ControlT::class_id, iPartId, iStateId, &rc);
        ASSERT(f);
        return rc;
    }

    COLORREF GetSchemeColor(int iPartId, int iStateId, int iPropId)
    {
        // GetColor(int iClassId, int iPartId, int iStateId, int iPropId, COLORREF *pColor)
        COLORREF ret;
        BOOL f = _scheme->GetColor(ControlT::class_id, iPartId, iStateId, iPropId, &ret);
        ASSERT(f);
        return ret;
    }

    BOOL Draw(HDC hdc, int iPartId, int iState, long dx, long dy, long dcx = 0, 
        long dcy = 0, DWORD dwRop = SRCCOPY)
    {
        return _scheme->Draw(hdc, ControlT::class_id, iPartId, iState, dx, dy, 
            dcx, dcy, dwRop);
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

    ... GetColor ...
#endif

public:
	static bool Install(HINSTANCE hInst)
    {
        // derived_type:: 可能真正调用的是 CWindow，必须限制
        return _installer.Install(hInst, derived_type::GetWndClassName(), 
            GetControlProc());
    }
    static bool Uninstall(HINSTANCE hInst)
    {
        return _installer.Uninstall(hInst, derived_type::GetWndClassName());
    }

protected:
    SkinControlImpl() 
        : _enable(true) 
        , m_pCurrentMsg(0)
    {}

    // BaseT 没有 virtual ~BaseT()
    // 能解决问题吗？ new derived; delete base*
    virtual ~SkinControlImpl() {} 

public: // 需要被模版派生类访问
	static WNDPROC GetDefaultProc() { return _installer.GetDefaultProc(); }

	// 和 ATL::CWindowImplBaseT::GetWindowProc 类似
	// 如果命名为 GetWindowProc 可能有warning(some BaseT)
	static WNDPROC GetControlProc() { return (WNDPROC)ControlProc; }

	const _ATL_MSG* GetCurrentMessage() const
	{
		return m_pCurrentMsg;
	}

	// "handled" management for cracked handlers
	BOOL IsMsgHandled() const
	{
		const _ATL_MSG* pMsg = GetCurrentMessage();
		ATLASSERT(pMsg != NULL);
		ATLASSERT(pMsg->cbSize >= sizeof(_ATL_MSG));
		return pMsg->bHandled;
	}
	void SetMsgHandled(BOOL bHandled)
	{
		_ATL_MSG* pMsg = (_ATL_MSG*)GetCurrentMessage(); // override const
		ATLASSERT(pMsg != NULL);
		ATLASSERT(pMsg->cbSize >= sizeof(_ATL_MSG));
		pMsg->bHandled = bHandled;
	}

	LRESULT DefWindowProc()
	{
		const _ATL_MSG* pMsg = m_pCurrentMsg;
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
		NOTIFY_CODE_HANDLER(NM_COOLSB_CUSTOMDRAW, OnScrollCustomDraw)
	END_MSG_MAP()

    // common skin message procdure
	LRESULT OnEnable(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		if( wParam != _enable )
		{
			_enable = wParam;
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
    // 得到缺省的 ISkinScheme，SkinControl对象创建后的第一个函数调用
    void Init()
    {
        CComPtr<ISkinMgr> mgr;
        if (S_OK == GetSkinMgr(&mgr) && mgr)
            mgr->GetCurentScheme(&_scheme);
    }


	static handle_map _handle_maps;
    static InstallPolicy _installer;
public:
	static LRESULT ControlProc(handle hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
        // ATLTRACE("HWND: 0x%08x msg: %08x\n", hWnd, uMsg);
		_ASSERTE( _CrtCheckMemory( ) );

		BOOL bRet = FALSE;
		LRESULT lRes = 0;

		_ATL_MSG msg(hWnd, uMsg, wParam, lParam);

		handle_map::const_iterator it = _handle_maps.lower_bound(hWnd);
		if (it == _handle_maps.end()) // 第一次创建之
		{
			// TODO: function this
			derived_type * p = new derived_type;
			p->m_hWnd = hWnd;
			it = _handle_maps.insert( std::make_pair(hWnd, p) ).first;

			p->Init();
		}

		if (it->second)
		{
            if (uMsg == WM_SYSCOMMAND)
            {
                // DebugBreak();
            }

            // 
            // ProcessWindowMessage 可能不是virtual的。需要调用每个类型的ProcessWindowMessage
            // 调用顺序是 SkinButton, SkinControlImpl, BaseT
            // BaseT 可以是 CWindowImpl 类型的类
            // 
			// call SkinButton
			derived_type * p = it->second;
			p->m_pCurrentMsg = &msg;

			if (p->_enable || uMsg == WMS_ENABLE)
			{
                // TRACE("1 %p %d, %p::%p\n", hWnd, uMsg, p, p->ProcessWindowMessage);
                bRet = p->derived_type::ProcessWindowMessage(hWnd, uMsg, wParam, lParam, lRes, 0);
                _ASSERTE(_CrtCheckMemory( ));

				if (!bRet)
				{
					// call SkinControlImpl
					this_type * pd = static_cast<this_type *>(p);
					ASSERT(pd);
                    // TRACE("2 %p %d, %p::%p\n", hWnd, uMsg, pd, pd->ProcessWindowMessage);
					// ATTENTION: this_type:: (maybe ProcessWindowMessage not virtual)
					bRet = pd->this_type::ProcessWindowMessage(hWnd, uMsg, 
                        wParam, lParam, lRes, 0);
                    _ASSERTE(_CrtCheckMemory( ));
				}
#if 0
                // ATTENTION: 设计时打算最后在这里直接调用 BaseT::ProcessWindowMessage 
                // 省略了 CHAIN_MSG_MAP ，但是导致SkinFramexxx的消息执行会有两次
                // SkinFrame 必须加上 CHAIN_MSG_MAP
				if (!bRet)
				{
                    // 使用了vc7的特性，如果BaseT==CWindowImpl等，需要再调用其处理过程
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

			if (uMsg == WM_NCDESTROY) //最后一个消息
			{
				// TODO: function this
				handle_map::iterator rr = _handle_maps.lower_bound(hWnd);
				ASSERT(rr != _handle_maps.end());

				rr->second->m_hWnd = 0; // for ~CWindowImplRoot ASSERT

				_ASSERTE( _CrtCheckMemory( ) );
				delete rr->second;
				_ASSERTE( _CrtCheckMemory( ) );
				_handle_maps.erase(rr);
			}
		}

		if (!bRet)
        {
            WNDPROC dw = GetDefaultProc();
            if (!dw)
                dw = ::DefWindowProc;

            if (uMsg == WM_NCLBUTTONUP || uMsg == WM_SYSCOMMAND)
            {
                TRACE("trace before CallWindowProc::%08x ret=%d\n", uMsg, lRes);
            }
            
            lRes = ::CallWindowProc(dw, hWnd, uMsg, wParam, lParam);
            
            if (uMsg == WM_NCLBUTTONUP || uMsg == WM_SYSCOMMAND)
            {
                TRACE("trace after CallWindowProc::%08x ret=%d\n", uMsg, lRes);
            }
        }

		_ASSERTE( _CrtCheckMemory( ) );
		return lRes;
	}
    private:

    // TODO: find out the way to unload dll from process memory
    // 如果还有hooked的窗口没有销毁
    // 实际没有啥用处，还是会导致崩溃
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
protected:
	const _ATL_MSG * m_pCurrentMsg;
	CComPtr<ISkinScheme> _scheme;

	unsigned _enable : 1;
};

template<class ControlT, class BaseT, class InstallPolicy>
InstallPolicy SkinControlImpl<ControlT, BaseT, InstallPolicy>::_installer;

template<class ControlT, class BaseT, class InstallPolicy>
std::hash_map<HWND, typename SkinControlImpl<ControlT, BaseT, InstallPolicy>::derived_type * > 
SkinControlImpl<ControlT, BaseT, InstallPolicy>::_handle_maps;

} // namespace Skin
