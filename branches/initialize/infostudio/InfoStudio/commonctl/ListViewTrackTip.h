#if !defined(AFX_LISTVIEWTRACKTIP_H__20030113_F7A2_CF82_8F7F_0080AD509054__INCLUDED_)
#define AFX_LISTVIEWTRACKTIP_H__20030113_F7A2_CF82_8F7F_0080AD509054__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#pragma once

/////////////////////////////////////////////////////////////////////////////
// ListViewTrackTip - Tracking ToolTip for a ListView control
//
// Written by Bjarke Viksoe (bjarke@viksoe.dk)
// Copyright (c) 2003 Bjarke Viksoe.
//
// Add a member to the parent window:
//    CListViewTrackTip m_Tip;
// Add the following macro to the parent's message map:
//    CHAIN_MSG_MAP_MEMBER( m_Tip )
// Attach the tracking tip to the ListView:
//    m_Tip.Attach(m_hWnd, m_ctrlList);
//
// This code may be used in compiled form in any way you desire. This
// file may be redistributed by any means PROVIDING it is 
// not sold for profit without the authors written consent, and 
// providing that this notice and the authors name is included. 
//
// This file is provided "as is" with no expressed or implied warranty.
// The author accepts no liability if it causes any damage to you or your
// computer whatsoever. It's free, so don't hassle me about it.
//
// Beware of bugs.
//

#ifndef __cplusplus
   #error WTL requires C++ compilation (use a .cpp suffix)
#endif

#ifndef __ATLCTRLS_H__
   #error ListViewTrackTip.h requires atlctrls.h to be included first
#endif


class CListViewTrackTip : public CMessageMap
{
public:
   enum 
   { 
      TIMER_ID = 438,
      X_OFFSET = 15,
      Y_OFFSET = 15,
   };

   CContainedWindowT<CListViewCtrl> m_ctrlList;
   CToolTipCtrl m_ctrlTip;
   CWindow m_wndOwner;
   TOOLINFO m_ti;
   int m_iCurItem;
   bool m_bTrackingMouseLeave;

   BEGIN_MSG_MAP(CListViewTrackTip)
      MESSAGE_HANDLER(WM_TIMER, OnTimer)
      NOTIFY_CODE_HANDLER(TTN_GETDISPINFO, OnGetDispInfo);
   ALT_MSG_MAP(1)
      MESSAGE_HANDLER(WM_MOUSEMOVE, OnMouseMove)
	  MESSAGE_HANDLER(WM_MOUSEWHEEL, OnMouseMove)
      MESSAGE_HANDLER(WM_MOUSELEAVE, OnMouseLeave)
   END_MSG_MAP()

public:
   CListViewTrackTip() :
      m_ctrlList(this, 1), m_iCurItem(-1), m_bTrackingMouseLeave(false)
   {
   }
   ~CListViewTrackTip()
   {
      if( m_ctrlTip.IsWindow() ) 
         /* scary */ m_ctrlTip.DestroyWindow();
      if( m_ctrlList.IsWindow() )
         /* scary */ m_ctrlList.UnsubclassWindow();
   }

   int GetCurItem()
   {
	   return m_iCurItem;
   }
   void Attach(HWND hwndOwner, HWND hwndList)
   {
      ATLASSERT(::IsWindow(hwndOwner));
      ATLASSERT(::IsWindow(hwndList));
      ATLASSERT(!m_ctrlList.IsWindow()); // Only attach once      

      m_wndOwner = hwndOwner;
      m_ctrlList.SubclassWindow(hwndList);

      // Create the tooltip
      DWORD dwStyle = WS_POPUP | TTS_NOPREFIX | TTS_ALWAYSTIP;
      m_ctrlTip.Create(NULL, CWindow::rcDefault, NULL, dwStyle, WS_EX_TOPMOST);
      ATLASSERT(m_ctrlTip.IsWindow());
      m_ctrlTip.SetWindowPos(HWND_TOPMOST, 0,0,0,0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);

      // Add the ListView as a ToolTip tool...
      ::ZeroMemory(&m_ti, sizeof(m_ti));
      m_ti.cbSize = sizeof(TOOLINFO);
      m_ti.uFlags = TTF_IDISHWND | TTF_TRACK | TTF_ABSOLUTE;
      m_ti.hwnd = hwndOwner;
      m_ti.hinst = _Module.GetResourceInstance();
      m_ti.uId = (UINT) (HWND) m_ctrlList;
      m_ti.lpszText = LPSTR_TEXTCALLBACK;
      m_ctrlTip.AddTool(&m_ti);
      m_ctrlTip.TrackActivate(&m_ti, FALSE);
   }

   // Message handlers

   LRESULT OnMouseLeave(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
   {
      ATLASSERT(m_ctrlList.IsWindow());
      ATLASSERT(m_ctrlTip.IsWindow());
      m_bTrackingMouseLeave = false;
      m_wndOwner.KillTimer(TIMER_ID);
      m_ctrlTip.TrackActivate(&m_ti, FALSE);
      bHandled = FALSE;
      return 0;
   }

   LRESULT OnMouseMove(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& bHandled)
   {
      ATLASSERT(m_ctrlList.IsWindow());
      ATLASSERT(m_ctrlTip.IsWindow());
      bHandled = FALSE;

      m_wndOwner.KillTimer(TIMER_ID);

      if( !m_bTrackingMouseLeave ) {
         TRACKMOUSEEVENT tme = { 0 };
         tme.cbSize = sizeof(TRACKMOUSEEVENT);
         tme.dwFlags = TME_LEAVE;
         tme.hwndTrack = m_ctrlList;
         ::_TrackMouseEvent(&tme);
         m_bTrackingMouseLeave = true;
      }

      POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
      UINT iFlags = 0;
      int item = m_ctrlList.HitTest(pt, &iFlags);
	  int nWidth = m_ctrlList.GetColumnWidth(0);
      if( item == -1 || pt.x > nWidth) {
         // Not hovering over any list item? Turn ToolTip off.
         m_ctrlTip.TrackActivate(&m_ti, FALSE);
      }
      else {
         bool bTipVisible = m_ctrlTip.GetCurrentTool(NULL) != 0;
         if( !bTipVisible ) 
		 {

            // Start a timer. If it triggers, the ToolTip has hovered over
            // the same item for some time...
			m_ctrlList.ClientToScreen(&pt);
			m_ctrlTip.TrackPosition(pt.x + X_OFFSET, pt.y + Y_OFFSET);
            m_wndOwner.SetTimer(TIMER_ID, m_ctrlTip.GetDelayTime(TTDT_INITIAL));
            m_iCurItem = item;
         }

         else
		 {
            // Already visible! If we're hovering over a new
            // item, change tip text...

			m_ctrlList.ClientToScreen(&pt);
			m_ctrlTip.TrackPosition(pt.x + X_OFFSET, pt.y + Y_OFFSET);
            if( m_iCurItem != item ) 
			{
               m_iCurItem = item;
               m_ctrlTip.TrackActivate(&m_ti, FALSE);
			   m_wndOwner.SetTimer(TIMER_ID, m_ctrlTip.GetDelayTime(TTDT_INITIAL));
            }
            
         }
      }
      return 0;
   }
   LRESULT OnTimer(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& bHandled)
   {
      bHandled = FALSE;
      if( wParam != TIMER_ID ) return 0;
      m_ctrlTip.TrackActivate(&m_ti, TRUE);
      return 0;
   }
   LRESULT OnGetDispInfo(int /*idCtrl*/, LPNMHDR pnmh, BOOL& bHandled)
   {
      if( pnmh->hwndFrom != m_ctrlTip ) {
         bHandled = FALSE;
         return 0;
      }
	  
      //::wsprintf(lpnmtdi->szText, "Item %d", m_iCurItem);
      return 0;
   }
};


#endif // !defined(AFX_LISTVIEWTRACKTIP_H__20030113_F7A2_CF82_8F7F_0080AD509054__INCLUDED_)
