// test_drawerView.h : interface of the CTest_drawerView class
//
/////////////////////////////////////////////////////////////////////////////

#pragma once
#include "DlgButton.h"

class CTest_drawerView : public CWindowImpl<CTest_drawerView>
{
public:
	DECLARE_WND_CLASS(NULL)

	BOOL PreTranslateMessage(MSG* pMsg)
	{
		pMsg;
		return FALSE;
	}

	BEGIN_MSG_MAP(CTest_drawerView)
		MESSAGE_HANDLER(WM_CREATE, OnCreate)
		MESSAGE_HANDLER(WM_PAINT, OnPaint)
		MESSAGE_HANDLER(WM_TIMER, OnTimer)
		COMMAND_ID_HANDLER(1000, OnButton)
        MSG_WM_CONTEXTMENU(OnContextMenu)
	END_MSG_MAP()

// Handler prototypes (uncomment arguments if needed):
//	LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
//	LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
//	LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)
	CButton		_button;
	CButton		_check;
	CFont font;
	LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		RECT rc;
		rc.left = 10;
		rc.top = 180;
		rc.right = 110;
		rc.bottom = 210;
		_button.Create(m_hWnd, &rc, "this is ", WS_VISIBLE | WS_CHILD, 0, 1000);
		
		LOGFONT Font;
		Font.lfHeight = 18; 
		Font.lfWidth = 0; 
		Font.lfEscapement = 0; 
		Font.lfOrientation = 0; 
		Font.lfWeight = 400; 
		Font.lfItalic = 0; 
		Font.lfUnderline = 0; 
		Font.lfStrikeOut = 0; 
		Font.lfCharSet = 0; 
		Font.lfOutPrecision = 3; 
		Font.lfClipPrecision = 2; 
		Font.lfQuality = 1; 
		Font.lfPitchAndFamily = 0; 
		strcpy( Font.lfFaceName , "New Times" ); 


		if( font.CreateFontIndirect( &Font ) )
		{
			_button.SetFont( font );
		}

		rc.top = 220;
		rc.bottom = 250;

		
		//_check.Create(m_hWnd, &rc, "this is ", WS_VISIBLE | WS_CHILD | BS_CHECKBOX);
		return 0;
	}
	
		
	LRESULT OnPaint(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
        using namespace Skin;
        // CComPtr<ISkinMgr> pm;
        // GetSkinMgr(&pm);

        CComPtr<ISkinScheme> pss;
        GetCurrentScheme(&pss);


		CPaintDC dc(m_hWnd);

		//TODO: Add your drawing code here
/*
        pss->Draw(dc, 19, WP_CAPTION, 1, 10, 10, 300, 0);

        pss->Draw(dc, 19, WP_MINBUTTON, 1, 10, 40);

        pss->Draw(dc, 19, WP_MAXBUTTON, 1, 10, 60);

        pss->Draw(dc, 19, WP_RESTOREBUTTON, 1, 10, 80);

		pss->Draw(dc, 1, BP_PUSHBUTTON, 1, 10, 100, 100, 30);

		pss->Draw(dc, 1, BP_PUSHBUTTON, 2, 10, 140, 100, 30);
*/
		return 0;
	}
	
	LRESULT OnTimer(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		return 0;
	}
	LRESULT OnButton(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
	{
		CDlgButton dlg;
		dlg.DoModal();
		return 0;
	}

    void OnContextMenu(HWND hWnd, CPoint point)
    {
        // CreatePopupMenu();
        CMenu m;
        m.LoadMenu(IDR_MAINFRAME);

        CMenu s = m.GetSubMenu(0);
        ATLTRACE("menu: %p\n", s.m_hMenu);
        s.TrackPopupMenu(TPM_LEFTBUTTON|TPM_LEFTALIGN, point.x, point.y, hWnd);
    }
};
