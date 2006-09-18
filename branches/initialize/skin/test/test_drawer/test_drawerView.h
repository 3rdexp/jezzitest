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
		MESSAGE_HANDLER(WM_PAINT, OnPaint)
	END_MSG_MAP()

// Handler prototypes (uncomment arguments if needed):
//	LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
//	LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
//	LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)

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

		return 0;
	}
};
