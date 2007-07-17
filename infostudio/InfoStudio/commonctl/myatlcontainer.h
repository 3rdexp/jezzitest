#ifndef MY_CONTAINER_CTRL__H__
#define MY_CONTAINER_CTRL__H__

#pragma once

typedef CWinTraits<WS_CHILD, WS_EX_TRANSPARENT|WS_EX_NOPARENTNOTIFY>					CContainWinTraits;

template <class T, class TBase = CWindow, class TWinTraits = CControlWinTraits>
class ATL_NO_VTABLE CMyContainerWindowImpl : public CWindowImpl< T, TBase, TWinTraits >
{
public:
	enum {ANIMATE_TIMEOUT = 10};

	DECLARE_WND_CLASS_EX(_T("NormalWndClass"), CS_DBLCLKS, COLOR_WINDOW)

	CWindow m_client;
	HWND SetClient(HWND client, int animateStep = 0/*pixel*/)
	{
		ATLASSERT(m_hWnd != NULL);

		if(m_client.m_hWnd == client)
			return client;

		T* pT = static_cast<T*>(this);

		::ShowWindow(m_client, SW_HIDE);
		if(animateStep > 0)
			pT->AnimateClient(TRUE, animateStep);

		HWND oldClient = m_client;
		m_client = client;

		if(::IsWindow(client)) {
			ATLASSERT(::GetParent(client) == m_hWnd 
				|| ::GetParent(client) == ::GetParent(m_hWnd));

			RECT rcClient;
			GetClientRect(&rcClient);
			RECT rcw;
			m_client.GetWindowRect(&rcw);
			if(rcClient.right - rcClient.left != rcw.right-rcw.left || rcClient.bottom - rcClient.top != rcw.bottom - rcw.top)
				pT->UpdateLayout(rcClient.right, rcClient.bottom);
			// pT->UpdateLayout(rcClient.right, rcClient.bottom);



			if(animateStep > 0) {
				ShowWindow(SW_SHOW);
				::ShowWindow(client, SW_HIDE);
				pT->AnimateClient(FALSE, animateStep);
			}
			::ShowWindow(client, SW_SHOWNORMAL);

			if(::GetParent(client) != m_hWnd)
			{
				ShowWindow(SW_HIDE);
			}

		} else {
			ShowWindow(SW_SHOWNORMAL);
		}

		return oldClient;
	}

	HWND GetClient() const
	{
		return m_client;
	}
	BEGIN_MSG_MAP(CMyContainerWindowImpl)
		MESSAGE_HANDLER(WM_SIZE, OnSize)
		MESSAGE_HANDLER(WM_SETFOCUS, OnSetFocus)
		MESSAGE_HANDLER(WM_ERASEBKGND, OnEraseBkGnd)
		MESSAGE_HANDLER(WM_PAINT, OnPaint)
		MESSAGE_HANDLER(WM_PRINTCLIENT, OnPaint)
		FORWARD_NOTIFICATIONS()
	END_MSG_MAP()

	LRESULT OnSize(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& /*bHandled*/)
	{
		T* pT = static_cast<T*>(this);
		pT->UpdateLayout(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
//		m_client.RedrawWindow(NULL, NULL, FALSE);
		return 0;
	}

	LRESULT OnSetFocus(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		if(m_client.m_hWnd != NULL)
			m_client.SetFocus();
		return 0;
	}

	LRESULT OnEraseBkGnd(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		return 1;	// no background needed
	}

	LRESULT OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		//inline void _cdecl MyAtlTrace(LPCSTR lpszFormat, ...);
// 		OutputDebugString("LRESULT OnPaint");
//		CPaintDC *paintDC = NULL;
		CDCHandle dc;
//		m_client.RedrawWindow();
		CPaintDC	paintDC(m_hWnd);
		dc = paintDC.m_hDC;
//		if(wParam){
//			dc = (HDC)wParam;
//	}
//		else {
//			paintDC = new CPaintDC(m_hWnd);
//			dc = paintDC->m_hDC;
//		}

		if(m_client.m_hWnd == NULL)	{// no client window
			T* pT = static_cast<T*>(this);
			pT->DrawPane(dc.m_hDC);
		}
//		::DefWindowProc(m_client, uMsg, wParam, lParam);
		return 0;
	}

	void UpdateLayout(int cxWidth, int cyHeight)
	{
		ATLASSERT(::IsWindow(m_hWnd));
//|SWP_NOREDRAW|SWP_DEFERERASE|SWP_NOCOPYBITS
		if(m_client.m_hWnd != NULL) {
			if(m_client.GetParent() == m_hWnd)
				m_client.SetWindowPos(NULL, 0, 0, cxWidth, cyHeight, SWP_NOZORDER);
			else {
				RECT rc;
				GetWindowRect(&rc);
				CWindow parent = GetParent();
				parent.ScreenToClient(&rc);
				m_client.SetWindowPos(NULL, rc.left, rc.top, 
					rc.right - rc.left, rc.bottom - rc.top,
					SWP_NOZORDER);
			}
		}
	}

	void DrawPane(CDCHandle dc)
	{
		RECT rect;
		GetClientRect(&rect);
		dc.FillRect(&rect, (HBRUSH)LongToPtr(COLOR_APPWORKSPACE + 1));
	}

	void AnimateClient(BOOL isHide, int animateStep)
	{
		if(animateStep < 0 || !m_client.IsWindow())
			return;

		T* pT = static_cast<T*>(this);
		CClientDC dc(m_hWnd);
		CDC memDC;
		CBitmap bmpSelf, bmpClient;
		HBITMAP oldbmp = 0;
		RECT rcSelf, rcClient;

		memDC.CreateCompatibleDC(dc.m_hDC);

		GetClientRect(&rcSelf);
		m_client.GetWindowRect(&rcClient);
		ScreenToClient(&rcClient);

		bmpSelf.CreateCompatibleBitmap(dc.m_hDC, rcSelf.right - rcSelf.left, rcSelf.bottom - rcSelf.top);
		bmpClient.CreateCompatibleBitmap(dc.m_hDC, rcClient.right - rcClient.left, rcClient.bottom - rcClient.top);

		oldbmp = memDC.SelectBitmap(bmpSelf);
		pT->DrawPane(memDC.m_hDC);

		memDC.SelectBitmap(bmpClient);
		m_client.SendMessage(WM_PRINT, (WPARAM)memDC.m_hDC, PRF_NONCLIENT | PRF_CLIENT | PRF_ERASEBKGND | PRF_CHILDREN);

		if(isHide) {
			int xEnd = (rcClient.right + rcClient.left)/2;
			int yEnd = (rcClient.bottom + rcClient.top)/2;
			int x = rcClient.left + animateStep;
			int y = rcClient.top + animateStep;
			POINT pt;
			for(; x < xEnd && y < yEnd; x += animateStep, y += animateStep) {
				memDC.SelectBitmap(bmpSelf);
				dc.BitBlt(0, 0, rcSelf.right - rcSelf.left, rcSelf.bottom - rcSelf.top, memDC, 0, 0, SRCCOPY);
				memDC.SelectBitmap(bmpClient);

				pt.x = x;
				pt.y = y;
				ClientToScreen(&pt);
				m_client.ScreenToClient(&pt);
				dc.BitBlt(x, y, (xEnd-x)*2, (yEnd-y)*2, memDC, pt.x, pt.y, SRCCOPY);
				Sleep(ANIMATE_TIMEOUT);
			}
		} else {
			int xEnd = (rcClient.right + rcClient.left)/2;
			int yEnd = (rcClient.bottom + rcClient.top)/2;
			int x = xEnd - animateStep;
			int y = yEnd - animateStep;
			POINT pt;
			for(; x > rcClient.left && y > rcClient.top; x -= animateStep, y -= animateStep) {
				memDC.SelectBitmap(bmpSelf);
				dc.BitBlt(0, 0, rcSelf.right - rcSelf.left, rcSelf.bottom - rcSelf.top, memDC, 0, 0, SRCCOPY);
				memDC.SelectBitmap(bmpClient);

				pt.x = x;
				pt.y = y;
				ClientToScreen(&pt);
				m_client.ScreenToClient(&pt);
				dc.BitBlt(x, y, (xEnd-x)*2, (yEnd-y)*2, memDC, pt.x, pt.y, SRCCOPY);
				Sleep(ANIMATE_TIMEOUT);
			}
		}

		if(oldbmp)
			memDC.SelectBitmap(oldbmp);
	}
};

class CMyContainerWindow : public CMyContainerWindowImpl<CMyContainerWindow>
{
public:
	DECLARE_WND_CLASS_EX(_T("NormalWndClass"), CS_DBLCLKS, COLOR_WINDOW)// DECLARE_WND_CLASS_EX(_T("MyContainerWindowClass"), 0, -1)
};

#endif // MY_CONTAINER_CTRL__H__
