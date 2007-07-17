#pragma once

#include "Resource.h"
#include "web/disphost.h"
#include "web/dispdyna.h"
#include "web/webview.h"

class CInfoDlg : public CDialogImpl<CInfoDlg>
{
public:

	CInfoDlg()
	{
		_pWebExternal = 0;
	}

	~CInfoDlg()
	{
		if(_pWebExternal)
		{
			_WebView.SetExternalDispatch(0);

			_pWebExternal->Release();

			_pWebExternal = 0;

		}
		if (::IsWindow(_WebView.m_hWnd))
			_WebView.DestroyWindow();
	}

	CWebView		_WebView;
	CDispObject<HostDisp> * _pWebExternal;

	enum {IDD = IDD_INFODLG};

	BEGIN_MSG_MAP(CInfoDlg)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		COMMAND_ID_HANDLER(IDOK, OnOk)
		COMMAND_ID_HANDLER(IDCANCEL, OnCancel)		

		MSG_WM_SIZE(OnSize)
	END_MSG_MAP()

	BOOL PreTranslateMessage(MSG* pMsg)
	{
		if (pMsg->message == WM_KEYDOWN)
		{
			if(pMsg->wParam == VK_RETURN)
			{
				if(::IsDialogMessage(m_hWnd, pMsg))
					return TRUE;
			}				
		}	
		return FALSE;
	}

	LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		Init();		
		return 0;
	}

	LRESULT OnOk(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{

		return 0;
	}

	LRESULT OnCancel(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{

		return 0;
	}

	void OnSize(UINT wParam, CSize size)
	{
		DefWindowProc();
		int cx = size.cx;
		int cy = size.cy;

		if (_WebView.m_hWnd)
			_WebView.SetWindowPos(NULL, 0, 0, cx , cy, SWP_NOACTIVATE | SWP_NOZORDER | SWP_NOMOVE);

	}

	//funciton
	void Init()
	{
		CRect rc;
		GetClientRect( rc );

		_WebView.Create(m_hWnd, rc, _T("about:blank"), WS_CHILD | WS_VISIBLE  
			| WS_HSCROLL | WS_VSCROLL  , 0 /*WS_EX_CLIENTEDGE*/); //WS_CLIPSIBLINGS | WS_CLIPCHILDREN | 

		_WebView.Init();

		// 3 External
		HRESULT hr = CDispObject<HostDisp>::CreateInstance(&_pWebExternal);
		if(SUCCEEDED(hr) && _pWebExternal)
		{
			_pWebExternal->SetWebView(&_WebView);
			_WebView.SetExternalDispatch(_pWebExternal);
		}

	}

public:

};