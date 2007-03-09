// CSubclassWnd is a generic class for hooking another window's messages.

#include "stdafx.h"
#include "Subclass.h"

#pragma warning(disable : 4311)
#pragma warning(disable : 4312)

class CSubclassWndMap : private CMapPtrToPtr {
public:
	CSubclassWndMap();
	~CSubclassWndMap();
	static CSubclassWndMap& GetHookMap();
	void Add(HWND hwnd, CSubclassWnd* pSubclassWnd);
	void Remove(CSubclassWnd* pSubclassWnd);
	void RemoveAll(HWND hwnd);
	CSubclassWnd* Lookup(HWND hwnd);
};

#define	theHookMap	(CSubclassWndMap::GetHookMap())


CSubclassWnd::CSubclassWnd()
{
	m_pNext = NULL;
	m_pOldWndProc = NULL;	
	m_hWnd  = NULL;
}

CSubclassWnd::~CSubclassWnd()
{
	if (m_hWnd) 
		HookWindow((HWND)NULL);		// unhook window
}

BOOL CSubclassWnd::HookWindow(HWND hwnd)
{
	ASSERT(this);
	if (hwnd) 
	{
		// Hook the window
		ASSERT(m_hWnd==NULL);
		ASSERT(::IsWindow(hwnd));
		theHookMap.Add(hwnd, this);			// Add to map of hooks

	}
	else if (m_hWnd) 
	{
		// Unhook the window
		theHookMap.Remove(this);				// Remove from map
		m_pOldWndProc = NULL;
	}
	m_hWnd = hwnd;
	return TRUE;
}

LRESULT CSubclassWnd::WindowProc(UINT msg, WPARAM wp, LPARAM lp)
{
	ASSERT(m_pOldWndProc);
	return m_pNext ? m_pNext->WindowProc(msg, wp, lp) :	
	::CallWindowProc(m_pOldWndProc, m_hWnd, msg, wp, lp);
}





LRESULT CALLBACK
HookWndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp)
{
#ifdef _USRDLL
	// If this is a DLL, need to set up MFC state
	//AFX_MANAGE_STATE(AfxGetStaticModuleState());
#endif

	// Set up MFC message state just in case anyone wants it
	// This is just like AfxCallWindowProc, but we can't use that because
	// a CSubclassWnd is not a CWnd.
	//
	//MSG& curMsg = AfxGetThreadState()->m_lastSentMsg;
	//MSG  oldMsg = curMsg;   // save for nesting
	//curMsg.hwnd		= hwnd;
	//curMsg.message = msg;
	//curMsg.wParam  = wp;
	//curMsg.lParam  = lp;

	// Get hook object for this window. Get from hook map
	CSubclassWnd* pSubclassWnd = theHookMap.Lookup(hwnd);
	ASSERT(pSubclassWnd);

	LRESULT lr;
	if (msg==WM_NCDESTROY) {
		// Window is being destroyed: unhook all hooks (for this window)
		// and pass msg to orginal window proc
		//
		WNDPROC wndproc = pSubclassWnd->m_pOldWndProc;
		theHookMap.RemoveAll(hwnd);
		lr = ::CallWindowProc(wndproc, hwnd, msg, wp, lp);

	} else {
		// pass to msg hook
		lr = pSubclassWnd->WindowProc(msg, wp, lp);
	}
	//curMsg = oldMsg;			// pop state
	return lr;
}

////////////////////////////////////////////////////////////////
// CSubclassWndMap implementation
//
CSubclassWndMap::CSubclassWndMap()
{
}

CSubclassWndMap::~CSubclassWndMap()
{
}

CSubclassWndMap& CSubclassWndMap::GetHookMap()
{
	static CSubclassWndMap theMap;
	return theMap;
}

void CSubclassWndMap::Add(HWND hwnd, CSubclassWnd* pSubclassWnd)
{
	ASSERT(hwnd && ::IsWindow(hwnd));

	// Add to front of list
	pSubclassWnd->m_pNext = Lookup(hwnd);
	SetAt(hwnd, pSubclassWnd);

	if (pSubclassWnd->m_pNext==NULL) {
		// If this is the first hook added, subclass the window
		pSubclassWnd->m_pOldWndProc = 
			(WNDPROC)SetWindowLong(hwnd, GWL_WNDPROC, (DWORD)HookWndProc);

	} else {
		// just copy wndproc from next hook
		pSubclassWnd->m_pOldWndProc = pSubclassWnd->m_pNext->m_pOldWndProc;
	}
	ASSERT(pSubclassWnd->m_pOldWndProc);
}

// Remove hook from map
void CSubclassWndMap::Remove(CSubclassWnd* pUnHook)
{
	HWND hwnd = pUnHook->m_hWnd;
	ASSERT(hwnd && ::IsWindow(hwnd));

	CSubclassWnd* pHook = Lookup(hwnd);
	ASSERT(pHook);
	if (pHook==pUnHook) {
		// hook to remove is the one in the hash table: replace w/next
		if (pHook->m_pNext)
			SetAt(hwnd, pHook->m_pNext);
		else {
			// This is the last hook for this window: restore wnd proc
			RemoveKey(hwnd);
			SetWindowLong(hwnd, GWL_WNDPROC, (DWORD)pHook->m_pOldWndProc);
		}
	} else {
		// Hook to remove is in the middle: just remove from linked list
		while (pHook->m_pNext!=pUnHook)
			pHook = pHook->m_pNext;
		ASSERT(pHook && pHook->m_pNext==pUnHook);
		pHook->m_pNext = pUnHook->m_pNext;
	}
}

// Remove all the hooks for a window
void CSubclassWndMap::RemoveAll(HWND hwnd)
{
	CSubclassWnd* pSubclassWnd;
	while ((pSubclassWnd = Lookup(hwnd))!=NULL)
		pSubclassWnd->HookWindow((HWND)NULL);	// (unhook)
}

// Find first hook associate with window
CSubclassWnd* CSubclassWndMap::Lookup(HWND hwnd)
{
	CSubclassWnd* pFound = NULL;
	if (!CMapPtrToPtr::Lookup(hwnd, (void*&)pFound))
		return NULL;
	//ASSERT_KINDOF(CSubclassWnd, pFound);
	return pFound;
}

