#ifndef WINDOWSHOOK__H__
#define WINDOWSHOOK__H__

#include "AtlAux2.h"

template <class T, int idHook = WH_GETMESSAGE>
class CWindowsHookImpl : public CAuxThunk<CWindowsHookImpl>
{
public:
	CWindowsHookImpl(): m_hhook(NULL)
	{
		InitThunk((TMFP)hookProc, this);
	}

	~CWindowsHookImpl()
	{
		unhook();
	}

	BOOL hook()
	{
		if(!m_hhook)
			m_hhook = SetWindowsHookEx(idHook, (HOOKPROC)GetThunk(), NULL, GetCurrentThreadId());

		return (m_hhook ? TRUE : FALSE);
	}

	void unhook()
	{
		if(m_hhook)
			UnhookWindowsHookEx(m_hhook);
	}

	BOOL hookGetMessage(int nCode, WPARAM wParam, MSG* pMsg)
	{
		//not handled
		return FALSE;
	}

	LRESULT hookProc(int nCode, WPARAM wParam, LPARAM lParam);

protected:	
	HHOOK m_hhook;

	template <int v>
	struct Int2Type
	{
		enum { value = v };
	};

	LRESULT realHookProc(int nCode, WPARAM wParam, LPARAM lParam, Int2Type<WH_GETMESSAGE>)
	{
		if(nCode >= 0) {
			T* pT = static_cast<T*>(this);
			MSG* pMsg = (MSG*)lParam;
			if(pT->hookGetMessage(nCode, wParam, pMsg))
				ZeroMemory(pMsg, sizeof(MSG));
		}
		return CallNextHookEx(m_hhook, nCode, wParam, lParam);
	}
};

template <class T, int idHook>
LRESULT CWindowsHookImpl<T, idHook>::hookProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	return realHookProc(nCode, wParam, lParam, CWindowsHookImpl<T, idHook>::Int2Type<idHook>());
}

#endif //WINDOWSHOOK__H__
