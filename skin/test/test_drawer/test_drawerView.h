// test_drawerView.h : interface of the CTest_drawerView class
//
/////////////////////////////////////////////////////////////////////////////

#pragma once

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
	END_MSG_MAP()

// Handler prototypes (uncomment arguments if needed):
//	LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
//	LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
//	LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)
	CButton		_button;
	CButton		_check;
	LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		RECT rc;
		rc.left = 10;
		rc.top = 180;
		rc.right = 110;
		rc.bottom = 210;
		//_button.Create(m_hWnd, &rc, "this is ", WS_VISIBLE | WS_CHILD);
		
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

        pss->Draw(dc, 19, WP_CAPTION, 1, 10, 10, 300, 0);

        pss->Draw(dc, 19, WP_MINBUTTON, 1, 10, 40);

        pss->Draw(dc, 19, WP_MAXBUTTON, 1, 10, 60);

        pss->Draw(dc, 19, WP_RESTOREBUTTON, 1, 10, 80);

		pss->Draw(dc, 1, BP_PUSHBUTTON, 1, 10, 100, 100, 30);

		pss->Draw(dc, 1, BP_PUSHBUTTON, 2, 10, 140, 100, 30);
		return 0;
	}
};
