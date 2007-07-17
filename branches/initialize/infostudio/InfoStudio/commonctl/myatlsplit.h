#ifndef MY_SPLITEX__H__
#define MY_SPLITEX__H__

#pragma once

#include <atlsplit.h>
#include "../condef.h"
/////////////////////////////////////////////////////////////////////////////
// Forward declarations

template <class T, bool t_bVertical = true> class CMySplitterImpl;
template <class T, bool t_bVertical = true, class TBase = CWindow, class TWinTraits = CControlWinTraits> class CMySplitterWindowImpl;
template <bool t_bVertical = true> class CMySplitterWindowT;


/////////////////////////////////////////////////////////////////////////////
// Splitter extended styles
#define SPLIT_LEFTALIGNED  0x00000008

template <class T, bool t_bVertical = true>
class CMySplitterImpl : 
	public CSplitterImpl<T, t_bVertical>
{
public:
	int  m_xySplitterPosSave;
	bool m_isPosMiddle;
	int  m_cxyMinLeftPos;
	int  m_cxyMinRightPos;

	CMySplitterImpl()
		: m_isPosMiddle(false)
		, m_cxyMinLeftPos(0)
		, m_cxyMinRightPos(0)
	{
		m_cxySplitBar = 3;
	}

// Attributes
	void SetSplitterRect(LPRECT lpRect = NULL, bool bUpdate = true)
	{
		if(lpRect == NULL)
		{
			T* pT = static_cast<T*>(this);
			pT->GetClientRect(&m_rcSplitter);
		}
		else
		{
			m_rcSplitter = *lpRect;
		}

		if(IsProportional())
			UpdateProportionalPos();
		else if(IsRightAligned())
			UpdateRightAlignPos();
		else if(IsLeftAligned())
			UpdateLeftAlignPos();

		if(bUpdate)
			UpdateSplitterLayout();
	}

	bool SetSplitterPos(int xyPos = -1, bool bUpdate = true)
	{
		if(xyPos == -1)		// -1 == middle
		{
			m_xySplitterPosSave = m_xySplitterPos;
			m_isPosMiddle = true;

			if(t_bVertical)
				xyPos = (m_rcSplitter.right - m_rcSplitter.left) / 2;
			else
				xyPos = (m_rcSplitter.bottom - m_rcSplitter.top) / 2;
		} else
			m_isPosMiddle = false;

		// Adjust if out of valid range
		int cxyMax = 0;
		if(t_bVertical)
			cxyMax = m_rcSplitter.right - m_rcSplitter.left;
		else
			cxyMax = m_rcSplitter.bottom - m_rcSplitter.top;

		if(xyPos < m_cxyMinLeftPos)
			xyPos = m_cxyMinLeftPos;
		else if(xyPos > (cxyMax - m_cxyMinRightPos)){
			xyPos = cxyMax - m_cxyMinRightPos;
		}

		if(xyPos < m_cxyMin + m_cxyBarEdge)
			xyPos = m_cxyMin;
		else if(xyPos > (cxyMax - m_cxySplitBar - m_cxyBarEdge - m_cxyMin))
			xyPos = cxyMax - m_cxySplitBar - m_cxyBarEdge - m_cxyMin;

		// Set new position and update if requested
		bool bRet = (m_xySplitterPos != xyPos);
		m_xySplitterPos = xyPos;

		if(IsProportional())
			StoreProportionalPos();
		else if(IsRightAligned())
			StoreRightAlignPos();
		else if(IsLeftAligned())
			StoreLeftAlignPos();

		if(bUpdate && bRet)
			UpdateSplitterLayout();

		return bRet;
	}

	void DrawSplitterBar(CDCHandle dc)
	{
		RECT rect;
		if(GetSplitterBarRect(&rect))
		{
			RECT rt;
			T* pT = static_cast<T*>(this);
			pT->GetClientRect(&rt);
			dc.FillSolidRect(&rt, RGB(255, 255, 255));			
			if (t_bVertical)
			{
				//垂直
				//left
				CRect rcChild;
				rcChild.left = 0;
				rcChild.top = 0;
				rcChild.right = rect.left;
				rcChild.bottom = rt.bottom;
				CBrush brBorder;
				brBorder.CreateSolidBrush( g_OutBorder ); 
				dc.FrameRect(rcChild, (HBRUSH)brBorder);
				brBorder.DeleteObject();

				brBorder.CreateSolidBrush( g_InBorder ); 
				rcChild.DeflateRect(1, 1, 1, 1);
				dc.FrameRect(rcChild, (HBRUSH)brBorder);
				brBorder.DeleteObject();

				/*
				//right
				rcChild.left = rect.right;
				rcChild.top = 0;
				rcChild.right = rt.right;
				rcChild.bottom = rt.bottom;
				brBorder.CreateSolidBrush( g_OutBorder ); 
				dc.FrameRect(rcChild, brBorder);
				brBorder.DeleteObject();

				brBorder.CreateSolidBrush( g_InBorder ); 
				rcChild.DeflateRect(1, 1, 1, 1);
				dc.FrameRect(rcChild, brBorder);
				brBorder.DeleteObject();
				*/
			}
			else
			{
				//水平
				//top
				CRect rcChild;
				rcChild.left = 0;
				rcChild.top = 0;
				rcChild.right = rt.right;
				rcChild.bottom = rect.top;
				CBrush brBorder;
				brBorder.CreateSolidBrush( g_OutBorder ); 
				dc.FrameRect(rcChild, (HBRUSH)brBorder);
				brBorder.DeleteObject();

				brBorder.CreateSolidBrush( g_InBorder ); 
				rcChild.DeflateRect(1, 1, 1, 1);
				dc.FrameRect(rcChild, (HBRUSH)brBorder);
				brBorder.DeleteObject();

				//bottom
				rcChild.left = 0;
				rcChild.top = rect.bottom;
				rcChild.right = rt.right;
				rcChild.bottom = rt.bottom;
				brBorder.CreateSolidBrush( g_OutBorder ); 
				dc.FrameRect(rcChild, brBorder);
				brBorder.DeleteObject();

				brBorder.CreateSolidBrush( g_InBorder ); 
				rcChild.DeflateRect(1, 1, 1, 1);
				dc.FrameRect(rcChild, brBorder);
				brBorder.DeleteObject();

			}
			//dc.FillSolidRect(&rect, RGB(255, 0, 0));			
		}
	}

// Message map and handlers
	typedef CMySplitterImpl< T, t_bVertical> thisClass;
	BEGIN_MSG_MAP(thisClass)
		MESSAGE_HANDLER(WM_CREATE, OnCreate)
		MESSAGE_HANDLER(WM_PAINT, OnPaint)
		MESSAGE_HANDLER(WM_PRINTCLIENT, OnPaint)
		if(IsInteractive())
		{
			MESSAGE_HANDLER(WM_SETCURSOR, OnSetCursor)
			MESSAGE_HANDLER(WM_MOUSEMOVE, OnMouseMove)
			MESSAGE_HANDLER(WM_LBUTTONDOWN, OnLButtonDown)
			MESSAGE_HANDLER(WM_LBUTTONUP, OnLButtonUp)
			MESSAGE_HANDLER(WM_LBUTTONDBLCLK, OnLButtonDoubleClick)
		}
		MESSAGE_HANDLER(WM_SETFOCUS, OnSetFocus)
		MESSAGE_HANDLER(WM_MOUSEACTIVATE, OnMouseActivate)
		MESSAGE_HANDLER(WM_SETTINGCHANGE, OnSettingChange)
	END_MSG_MAP()

	LRESULT OnMouseMove(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		T* pT = static_cast<T*>(this);
		int xPos = GET_X_LPARAM(lParam);
		int yPos = GET_Y_LPARAM(lParam);
		if((wParam & MK_LBUTTON) && ::GetCapture() == pT->m_hWnd)
		{
			int xyNewSplitPos = 0;
			if(t_bVertical)
				xyNewSplitPos = xPos - m_rcSplitter.left - m_cxyDragOffset;
			else
				xyNewSplitPos = yPos - m_rcSplitter.top - m_cxyDragOffset;

			if(xyNewSplitPos == -1)	// avoid -1, that means middle
				xyNewSplitPos = -2;

			if(m_xySplitterPos != xyNewSplitPos)
			{
				if(m_bFullDrag)
				{
					if(pT->SetSplitterPos(xyNewSplitPos, true))
						pT->UpdateWindow();
				}
				else
				{
					DrawGhostBar();
					pT->SetSplitterPos(xyNewSplitPos, false);
					DrawGhostBar();
				}
			}
		}
		else		// not dragging, just set cursor
		{
			if(IsOverSplitterBar(xPos, yPos))
				::SetCursor(m_hCursor);
			bHandled = FALSE;
		}

		return 0;
	}

	LRESULT OnLButtonUp(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
	{
		if(!m_bFullDrag)
		{
			DrawGhostBar();
			T* pT = static_cast<T*>(this);
			UpdateSplitterLayout();
			pT->UpdateWindow();
		}
		::ReleaseCapture();
		bHandled = FALSE;
		return 1;
	}

	LRESULT OnPaint(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		T* pT = static_cast<T*>(this);
		// try setting position if not set
		if(m_nSinglePane == SPLIT_PANE_NONE && m_xySplitterPos == -1)
			pT->SetSplitterPos();
		// do painting
		CDCHandle dc;
		CPaintDC* dcPaint = NULL;
		if(wParam)
			dc = (HDC)wParam;
		else {
			dcPaint = new CPaintDC(pT->m_hWnd);
			dc = dcPaint->m_hDC;
		}
		pT->DrawSplitter(dc.m_hDC);
		delete dcPaint;
		return 0;
	}

	LRESULT OnLButtonDoubleClick(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		T* pT = static_cast<T*>(this);
		if(m_isPosMiddle) {
			pT->SetSplitterPos(m_xySplitterPosSave);
		} else
			pT->SetSplitterPos();	// middle
		return 0;
	}

	void UpdatePaneWndPos(HWND pane, const RECT* rect)
	{
		T* pT = static_cast<T*>(this);

		if(::GetParent(pane) == pT->m_hWnd) {
			::SetWindowPos(pane, NULL, rect->left, rect->top, 
				rect->right - rect->left, rect->bottom - rect->top, SWP_NOZORDER);
		} else {
			ATLASSERT(::GetParent(pane) == ::GetParent(pT->m_hWnd));

			RECT rc = *rect;
			pT->ClientToScreen(&rc);
			CWindow parent = pT->GetParent();
			parent.ScreenToClient(&rc);
			::SetWindowPos(pane, NULL, rc.left, rc.top, 
				rc.right - rc.left, rc.bottom - rc.top, SWP_NOZORDER);
		}
	}

// Implementation - internal helpers
	void UpdateSplitterLayout()
	{
		if(m_nSinglePane == SPLIT_PANE_NONE && m_xySplitterPos == -1)
			return;

		T* pT = static_cast<T*>(this);
		RECT rect = { 0, 0, 0, 0 };
		if(m_nSinglePane == SPLIT_PANE_NONE)
		{
			if(GetSplitterBarRect(&rect))
				pT->InvalidateRect(&rect);

			for(int nPane = 0; nPane < m_nPanesCount; nPane++)
			{
				if(GetSplitterPaneRect(nPane, &rect))
				{
					if(m_hWndPane[nPane] != NULL)
					{
						// 要绘制边框.所以缩小两个象素
						RECT rcOrg = rect;
						if (!t_bVertical || nPane != 1)
							InflateRect(&rect, -2, -2);	
						UpdatePaneWndPos(m_hWndPane[nPane], &rect);
						//::SetWindowPos(m_hWndPane[nPane], NULL, rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top, SWP_NOZORDER);
						pT->InvalidateRect(&rcOrg);
					}
					else
						pT->InvalidateRect(&rect);
				}
			}
		}
		else
		{
			if(GetSplitterPaneRect(m_nSinglePane, &rect))
			{
				if(m_hWndPane[m_nSinglePane] != NULL)
				{
					
					UpdatePaneWndPos(m_hWndPane[m_nSinglePane], &rect);
				}
					//::SetWindowPos(m_hWndPane[m_nSinglePane], NULL, rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top, SWP_NOZORDER);
				else
					pT->InvalidateRect(&rect);
			}
		}
	}

	void DrawGhostBar()
	{
		RECT rect = { 0, 0, 0, 0 };
		if(GetSplitterBarRect(&rect))
		{
			// invert the brush pattern (looks just like frame window sizing)
			T* pT = static_cast<T*>(this);
			CWindowDC dc(pT->m_hWnd);
			CBrush brush = CDCHandle::GetHalftoneBrush();
			if(brush.m_hBrush != NULL)
			{
				CBrushHandle brushOld = dc.SelectBrush(brush);
				dc.PatBlt(rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top, PATINVERT);
				dc.SelectBrush(brushOld);
			}
		}
	}


	bool IsLeftAligned() const
	{
		return (m_dwExtendedStyle & SPLIT_LEFTALIGNED) != 0;
	}

	void StoreLeftAlignPos()
	{
		int cxyTotal = t_bVertical ? (m_rcSplitter.left - m_rcSplitter.right) : (m_rcSplitter.bottom - m_rcSplitter.top);
		if(cxyTotal > 0)
			m_nProportionalPos = cxyTotal - m_xySplitterPos;
		else
			m_nProportionalPos = 0;
	}

	void UpdateLeftAlignPos()
	{
		int cxyTotal = t_bVertical ? (m_rcSplitter.left - m_rcSplitter.right) : (m_rcSplitter.bottom - m_rcSplitter.top);
		if(cxyTotal > 0)
		{
			T* pT = static_cast<T*>(this);
			pT->SetSplitterPos(cxyTotal - m_nProportionalPos, false);
		}
	}
};

/////////////////////////////////////////////////////////////////////////////
// CMySplitterWindowImpl - Implements a splitter window

template <class T, bool t_bVertical = true, class TBase = CWindow, class TWinTraits = CControlWinTraits>
class ATL_NO_VTABLE CMySplitterWindowImpl : public CWindowImpl< T, TBase, TWinTraits >, public CMySplitterImpl< T , t_bVertical >
{
public:
	DECLARE_WND_CLASS_EX(NULL, CS_DBLCLKS, COLOR_WINDOW)

	BOOL SubclassWindow(HWND hWnd)
	{
/*
		BOOL ret = CWindowImpl< T, TBase, TWinTraits >::SubclassWindow(hWnd);
		if(ret) {
			GetSystemSettings(true);
			SetSplitterRect();
		}
		return ret;
*/
		CWindow wnd(hWnd);
		CWindow parent(wnd.GetParent());
		RECT    rc;
		DWORD   style, exstyle;
		UINT    id;

		wnd.GetWindowRect(&rc);
		parent.ScreenToClient(&rc);
		style   = wnd.GetStyle();
		exstyle = wnd.GetExStyle();
		id      = wnd.GetDlgCtrlID();

		wnd.DestroyWindow();
		Create(parent, rc, NULL, style, exstyle, id);
		//::SetWindowPos(m_hWndPane[0], m_hWnd, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
		//::SetWindowPos(m_hWndPane[1], m_hWnd, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
		return TRUE;
	}

	typedef CMySplitterWindowImpl< T , t_bVertical, TBase, TWinTraits >	thisClass;
	typedef CMySplitterImpl< T , t_bVertical >				baseClass;
	BEGIN_MSG_MAP(thisClass)
		MESSAGE_HANDLER(WM_ERASEBKGND, OnEraseBackground)
		MESSAGE_HANDLER(WM_SIZE, OnSize)
		CHAIN_MSG_MAP(baseClass)
		FORWARD_NOTIFICATIONS()
		REFLECT_NOTIFICATIONS()
	END_MSG_MAP()

	LRESULT OnEraseBackground(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		// handled, no background painting needed
		return 1;
	}

	LRESULT OnSize(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& bHandled)
	{
		if(wParam != SIZE_MINIMIZED)
			SetSplitterRect();

		bHandled = FALSE;
		return 1;
	}
};


/////////////////////////////////////////////////////////////////////////////
// CSplitterWindow - Implements a splitter window to be used as is

template <bool t_bVertical = true>
class CMySplitterWindowT : public CMySplitterWindowImpl<CMySplitterWindowT<t_bVertical>, t_bVertical>
{
public:
	DECLARE_WND_CLASS_EX(_T("MySplitterWindowClass"), CS_DBLCLKS, COLOR_WINDOW)
};

typedef CMySplitterWindowT<true>	CMySplitterWindow;
typedef CMySplitterWindowT<false>	CMyHorSplitterWindow;

#endif // MY_SPLITEX__H__
