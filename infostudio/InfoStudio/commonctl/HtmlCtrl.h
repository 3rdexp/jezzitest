
#pragma once
#include "../web/webview.h"
#include "../commonctl/PictureExWnd.h"
class CHtmlCtrl:	public CWindowImpl<CHtmlCtrl>
{
public:
	// Constructor/Destructor
	CHtmlCtrl()
	{
		_pWebExternal = 0;
	}
	~CHtmlCtrl()
	{
		if(_pWebExternal)
		{
			_WebView.SetExternalDispatch(0);

			_pWebExternal->Release();

			_pWebExternal = 0;

			ATLTRACE("MainFrame::OnScriptQuit release external\n");
		}
		if (::IsWindow(_WebView.m_hWnd))
			_WebView.DestroyWindow();
	}

	BOOL PreTranslateMessage(MSG* pMsg)
	{
		if (_WebView.PreTranslateMessage(pMsg))
			return TRUE;

		return FALSE;
	}


	BEGIN_MSG_MAP(CHtmlCtrl)		
		MSG_WM_ERASEBKGND(OnEraseBkGnd)
		MESSAGE_HANDLER(WM_CREATE, OnCreate)
		MSG_WM_SIZE(OnSize)
	END_MSG_MAP()

	LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		DefWindowProc();

		RECT rc;		
		GetClientRect(&rc);

		_wndWaitting.Create(m_hWnd, rcDefault, NULL, WS_CHILD | WS_CLIPSIBLINGS | WS_CLIPCHILDREN, 1);
		_wndWaitting.ModifyStyleEx(WS_EX_DLGMODALFRAME | WS_EX_WINDOWEDGE, 0);
		WTL::CString strFile = GetExePath() + "loading.gif";
		_wndWaitting.Load(strFile);
		_wndWaitting.SetBkColor(RGB(255, 255, 255));
		// 把gif控件居中
		RECT rcGif;
		_wndWaitting.GetWindowRect(&rcGif);
		int nLeft = ( rc.right - rc.left - (rcGif.right - rcGif.left) ) / 2 + rc.left;		
		int nTop = ( rc.bottom - rc.top - (rcGif.bottom - rcGif.top) ) / 2 + rc.top;
		if (nTop > 200)
			nTop = 200;
		::SetWindowPos(_wndWaitting.m_hWnd, NULL, nLeft, nTop, 0, 0, SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOSIZE );


		_WebView.Create(m_hWnd, rcDefault, _T("about:blank"), WS_CHILD   
			| WS_HSCROLL | WS_VSCROLL  , 0 /*WS_EX_CLIENTEDGE*/); //WS_CLIPSIBLINGS | WS_CLIPCHILDREN | 

		_WebView.SetWaitHwnd(&_wndWaitting);

		_WebView.Init();

		// 3 External
		HRESULT hr = CDispObject<HostDisp>::CreateInstance(&_pWebExternal);
		if(SUCCEEDED(hr) && _pWebExternal)
		{
			_pWebExternal->SetWebView(&_WebView);
			_WebView.SetExternalDispatch(_pWebExternal);
		}
		
		if ( _strUrl.GetLength() > 0 )
			_WebView.Navigate( _strUrl );

		return 0;
	}

	LRESULT	OnEraseBkGnd(HDC hdc)
	{
		if ( ::IsWindowVisible(_WebView.m_hWnd) )
			return TRUE;
		CRect rect;
		GetClientRect(&rect);
		CMemoryDC memDC(hdc, rect);	
		CBrush brushBack;
		brushBack.CreateSolidBrush(RGB( 255, 255, 255));		
		FillRect(memDC, &rect, brushBack);
		return TRUE;
	}

	void OnSize(UINT wParam, CSize size)
	{
		if (::IsWindow(_WebView.m_hWnd))
		{
			::SetWindowPos(_WebView.m_hWnd, NULL, 0, 0, size.cx, size.cy, SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_NOMOVE);			
		}

		if (::IsWindow(_wndWaitting.m_hWnd))
		{
			// 把gif控件居中	
			RECT rcGif;
			_wndWaitting.GetWindowRect(&rcGif);
			int nLeft = ( size.cx - (rcGif.right - rcGif.left) ) / 2;
			int nTop = ( size.cy - (rcGif.bottom - rcGif.top) ) / 2 ;
			if (nTop > 200)
				nTop = 200;
			::SetWindowPos(_wndWaitting.m_hWnd, NULL, nLeft, nTop, 0, 0, SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOSIZE );
		}

	}
	
	HRESULT Navigate (LPCTSTR pszName)
	{
		return _WebView.Navigate( pszName );
	}

private:
	CWebView		_WebView;
	CString			_strUrl;
	CDispObject<HostDisp> * _pWebExternal;
	CPictureExWnd	_wndWaitting;
};
