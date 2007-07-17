#pragma once

#include "Resource.h"
#include "web/disphost.h"
#include "web/dispdyna.h"
#include "web/webview.h"

class CValidateDlg : public CDialogImpl<CValidateDlg>
{
public:

	CValidateDlg()
	{
		_strUrl = "";
		_pWebExternal = 0;
	}

	~CValidateDlg()
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
	enum {IDD = IDD_VALIDATEDLG};

	BEGIN_MSG_MAP(CValidateDlg)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		COMMAND_ID_HANDLER(IDOK, OnOk)
		COMMAND_ID_HANDLER(IDCANCEL, OnCancel)				
	END_MSG_MAP()

	CWebView		_WebView;
	CString			_strUrl;	
	CString			_validateCode;
	CDispObject<HostDisp> * _pWebExternal;

	LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		Init();		
		return 0;
	}

	LRESULT OnOk(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		EndDialog(IDOK);
		return 0;
	}

	LRESULT OnCancel(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		EndDialog(IDCANCEL);
		return 0;
	}



	//funciton
	void Init()
	{
		CRect rc;
		::GetClientRect( GetDlgItem(IDC_WEB), &rc );

		POINT pt;
		pt.x = rc.left;
		pt.y = rc.top;
		::ClientToScreen( GetDlgItem(IDC_WEB), &pt );

		POINT ptClient;
		ptClient.x = ptClient.y = 0;
		ClientToScreen(  &ptClient );
		
		rc.OffsetRect( pt.x - ptClient.x, pt.y - ptClient.y );

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

		if ( _strUrl.GetLength() > 0 )
		{
			_WebView.Navigate( _strUrl );
			//_WebView.RunScript(L"init", _strID, _strType);
		}

		_validateCode = "";
	}

public:

};