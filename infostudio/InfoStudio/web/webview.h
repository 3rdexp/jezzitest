// TestView.h : interface of the CWebView class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_TESTVIEW_H__03B19F97_CDD7_46DF_85E5_01CD1C47449F__INCLUDED_)
#define AFX_TESTVIEW_H__03B19F97_CDD7_46DF_85E5_01CD1C47449F__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "comhelp.h"

#include <exdispid.h> // Dispatch IDS for DWebBrowserEvents2.
#include <ActivScp.h>
#include <deque>
#include <vector>
#include <sstream>
#include <fstream>
#include <iostream>
#include "../commonctl/PictureExWnd.h"
#include "DocEventHandler.h"
#include "../messagedef.h"
// TODO: #include <mshtmdid.h> 

/*
//    E:\win2k\private\inet\mshtml\tools\include\exdisp.odl
// c:\nt\win2k\private\inet\mshtml\tools\include\exdisp.odl

msdn 
// Handling HTML Element Events 
// http://msdn.microsoft.com/library/default.asp?url=/workshop/browser/mshtml/tutorials/sink.asp
//

// NewWindow3
// http://msdn.microsoft.com/workshop/browser/webbrowser/reference/ifaces/dwebbrowserevents2/newwindow3.asp?frame=false

search 
C:\nt\win2k\private\shell\shdocvw
C:\nt\win2k\private\shell\inc
C:\nt\win2k\private\shell\iexplore
C:\nt\win2k\private\shell\shdisp
*/

//LPCWSTR ScriptDummyID = L"ScriptDiv";




// 记录各个元素
class ElementItem
{
public:
	

	ElementItem()
	{
		_Name = "";
		_Value = "";
		_Type = "";
	}

	CString		_Name;
	CString		_Value;
	CString		_Type;

};
// 记录各个Form的元素集合
class ElementCollect 
{
public:
	
	ElementCollect()
	{
		clear();
	}

	void clear()
	{
		_FormName = "";
		_FormUrl	= "";
		_vectElement.clear();
	}

	CString		_FormName;
	CString		_FormUrl;
	CString		_PostMethod;
	std::vector<ElementItem>	_vectElement;
};
class ATL_NO_VTABLE WebEvents
	: public DynaDispObject<WebEvents, DWebBrowserEvents2>
{
public:
	BEGIN_DISPATCH_MAP(WebEvents)
		DISP_CALL6(0, DISPID_NEWWINDOW, DISPATCH_METHOD, VT_NULL, NewWindow, VT_BSTR, VT_I4, VT_BSTR, VT_VARIANT|VT_BYREF, 
		VT_BSTR, VT_BYREF|VT_BOOL)
		DISP_CALL2(0, DISPID_NEWWINDOW2, DISPATCH_METHOD, VT_NULL, NewWindow2, VT_DISPATCH|VT_BYREF, VT_BYREF|VT_BOOL)
		DISP_CALL7(0, DISPID_BEFORENAVIGATE2, DISPATCH_METHOD, VT_NULL, BeforeNavigate2, VT_DISPATCH, VT_VARIANT|VT_BYREF, VT_VARIANT|VT_BYREF,
		VT_VARIANT|VT_BYREF, VT_VARIANT|VT_BYREF, VT_VARIANT|VT_BYREF, VT_BYREF|VT_BOOL)

		DISP_CALL2(0, DISPID_WINDOWCLOSING, DISPATCH_METHOD, VT_NULL, WindowClosing, VT_BOOL, VT_BYREF|VT_BOOL)

		DISP_CALL2(0, DISPID_PROGRESSCHANGE, DISPATCH_METHOD, VT_NULL, ProgressChange, VT_I4, VT_I4)
		DISP_CALL1(0, DISPID_STATUSTEXTCHANGE, DISPATCH_METHOD, VT_NULL, StatusTextChange, VT_BSTR)

		// DISPID_PROPERTYCHANGE
	END_DISPATCH_MAP()

	void __stdcall NewWindow(/*[in] */BSTR URL, /*[in] */long Flags, /*[in] */BSTR TargetFrameName, /*[in] */VARIANT * PostData, 
	/*[in] */BSTR Headers, /*[in,out] */VARIANT_BOOL * Processed)
	{
	}

	void __stdcall NewWindow2(/*[in] */IDispatch** pDisp, /*[in,out] */VARIANT_BOOL * Cancel)
	{
		//*Cancel = VARIANT_TRUE;
	}

	void __stdcall BeforeNavigate2(IDispatch *pDisp,
		VARIANT *url,
		VARIANT *Flags,
		VARIANT *TargetFrameName,
		VARIANT *PostData,
		VARIANT *Headers,
		/*[in,out]*/VARIANT_BOOL *Cancel)
	{
		// *Cancel = VARIANT_TRUE;
	}

	// IsChildWindow that specifies whether the window was created from script. Can be one of the following values
	void __stdcall WindowClosing(VARIANT_BOOL IsChildWindow, VARIANT_BOOL * Cancel)
	{
		*Cancel = VARIANT_TRUE;
	}


	void __stdcall ProgressChange(LONG Progress, long ProgressMax)
	{
		// ATLTRACE2( atlTraceLog, 2, "Log:ProgressChange progress=%d ProgressMax=%d\n", Progress, ProgressMax);
	}
	void __stdcall StatusTextChange(BSTR Text)
	{
		// if( Text ) ATLTRACE2( atlTraceLog, 2, "Log:StatusTextChange Text=%S\n", Text);
	}
};

class CDocHostUI 
	: public CComObjectRootEx<CComObjectThreadModel>
	, public IDispatchImpl<IDocHostUIHandlerDispatch, &IID_IDocHostUIHandlerDispatch, &GUID_NULL>
{
private:
public:
#if 1
	BEGIN_COM_MAP(CDocHostUI)
		COM_INTERFACE_ENTRY(IDispatch)
		COM_INTERFACE_ENTRY(IDocHostUIHandlerDispatch)
	END_COM_MAP()
#else
	// IUnknown
	STDMETHOD_(ULONG, AddRef)()
	{
		return 1;
	}
	STDMETHOD_(ULONG, Release)()
	{
		return 1;
	}
	STDMETHOD(QueryInterface)( /* [in] */ REFIID riid,
		/* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObj)
	{
		CheckPointer(ppvObj, E_POINTER);
		if( ATL::InlineIsEqualUnknown(riid) )
		{
			*ppvObj = static_cast<IUnknown*>(this);
			AddRef();
			return S_OK;
		}
		else if( ATL::InlineIsEqualGUID(IID_IDispatch, riid) )
		{
			*ppvObj = static_cast<IDispatch*>(this);
			AddRef();
			return S_OK;
		}
		// TODO: compile check, if TDisp == IDispatch
		else if( ATL::InlineIsEqualGUID(IID_IDocHostUIHandlerDispatch, riid) )
		{
			*ppvObj = static_cast<IDocHostUIHandlerDispatch*>(this);
			AddRef();
			return S_OK;
		}
#ifdef _DEBUG
		ATLTRACE2(atlTraceWarn, 0, "CDocHostUI:QueryInterface riid=%S return E_NOINTERFACE, %s\n", CComBSTR(riid), typeid(this).name());
#endif
		return E_NOINTERFACE;
	}
#endif

	// IDocHostUIHandlerDispatch
	// 所有没有实现的函数的行为参考了， ATLHost的实现
	STDMETHOD(ShowContextMenu)( 
		/* [in] */ DWORD dwID,
		/* [in] */ DWORD x,
		/* [in] */ DWORD y,
		/* [in] */ IUnknown __RPC_FAR *pcmdtReserved,
		/* [in] */ IDispatch __RPC_FAR *pDispatchObjectHit,
		/* [retval][out] */ HRESULT __RPC_FAR *dwRetVal)
	{
		*dwRetVal = S_OK; // disable

		CComQIPtr<IHTMLInputTextElement> input_text = pDispatchObjectHit;
		if( input_text )
		{
			*dwRetVal = S_FALSE; // enable ....
			return S_OK;
		}
		CComQIPtr<IHTMLTextAreaElement> text_area = pDispatchObjectHit;
		if( text_area )
		{
			*dwRetVal = S_FALSE;
			return S_OK;
		}
		return S_OK;
	}

	STDMETHOD(GetHostInfo)( 
		/* [out][in] */ DWORD __RPC_FAR *pdwFlags,
		/* [out][in] */ DWORD __RPC_FAR *pdwDoubleClick)
	{
		*pdwFlags = DOCHOSTUIFLAG_NO3DBORDER | DOCHOSTUIFLAG_THEME;
		*pdwDoubleClick = DOCHOSTUIDBLCLK_DEFAULT;
		return S_OK;
	}

	STDMETHOD(ShowUI)( 
		/* [in] */ DWORD dwID,
		/* [in] */ IUnknown __RPC_FAR *pActiveObject,
		/* [in] */ IUnknown __RPC_FAR *pCommandTarget,
		/* [in] */ IUnknown __RPC_FAR *pFrame,
		/* [in] */ IUnknown __RPC_FAR *pDoc,
		/* [retval][out] */ HRESULT __RPC_FAR *dwRetVal)
	{
		return S_OK;
	}

	STDMETHOD(HideUI)( void)
	{
		return S_OK;
	}

	STDMETHOD(UpdateUI)( void)
	{
		return S_OK;
	}

	STDMETHOD(EnableModeless)( 
		/* [in] */ VARIANT_BOOL fEnable)
	{
		return S_OK;
	}

	STDMETHOD(OnDocWindowActivate)( 
		/* [in] */ VARIANT_BOOL fActivate)
	{
		return S_OK;
	}

	STDMETHOD(OnFrameWindowActivate)( 
		/* [in] */ VARIANT_BOOL fActivate)
	{
		return S_OK;
	}

	STDMETHOD(ResizeBorder)( 
		/* [in] */ long left,
		/* [in] */ long top,
		/* [in] */ long right,
		/* [in] */ long bottom,
		/* [in] */ IUnknown __RPC_FAR *pUIWindow,
		/* [in] */ VARIANT_BOOL fFrameWindow)
	{
		ATLTRACE2(atlTraceLog, 1, "CDocHostUI::ResizeBorder %d %d\n", right-left, bottom-top);
		//	T * pT = static_cast<T*>(this);
		//	RECT rc = {left, top, right, bottom};
		//	pT->ResizeBorder(&rc, pUIWindow, fFrameWindow == VARIANT_TRUE ? TRUE : FALSE);
		if(_wnd.m_hWnd)
		{
			CWindow topw( _wnd.GetTopLevelWindow() );
			if( topw.m_hWnd )
			{
				CRect rcc;
				_wnd.GetClientRect(&rcc);

				int width = (right - left);
				int height = (bottom - top);

				if( rcc.Width() != width && rcc.Height() != height )
				{
					CRect rcw;
					topw.GetWindowRect(&rcw);
					topw.GetClientRect(&rcc);

					topw.SetWindowPos( NULL, 0, 0, 
						rcw.Width() - rcc.Width() + width, rcw.Height() - rcc.Height() + height,
						SWP_NOZORDER | SWP_NOMOVE );
				}
			}
		}
		return S_OK;
	}

	STDMETHOD(TranslateAccelerator)( 
		/* [in] */ DWORD hWnd,
		/* [in] */ DWORD nMessage,
		/* [in] */ DWORD wParam,
		/* [in] */ DWORD lParam,
		/* [in] */ BSTR bstrGuidCmdGroup,
		/* [in] */ DWORD nCmdID,
		/* [retval][out] */ HRESULT __RPC_FAR *dwRetVal)
	{
		// TODO:
		return S_FALSE;
	}

	STDMETHOD(GetOptionKeyPath)( 
		/* [out] */ BSTR __RPC_FAR *pbstrKey,
		/* [in] */ DWORD dw)
	{
		// TODO:
		return S_FALSE;
	}

	STDMETHOD(GetDropTarget)( 
		/* [in] */ IUnknown __RPC_FAR *pDropTarget,
		/* [out] */ IUnknown __RPC_FAR *__RPC_FAR *ppDropTarget)
	{
		return S_FALSE;
	}

	STDMETHOD(GetExternal)( 
		/* [out] */ IDispatch __RPC_FAR *__RPC_FAR *ppDispatch)
	{
		*ppDispatch = _spExternal;
		if(*ppDispatch)
			(*ppDispatch)->AddRef();
		return S_OK;
	}

	STDMETHOD(TranslateUrl)( 
		/* [in] */ DWORD dwTranslate,
		/* [in] */ BSTR bstrURLIn,
		/* [out] */ BSTR __RPC_FAR *pbstrURLOut)
	{
		ATLTRACE(L"TranslateUrl:%s\n", bstrURLIn);
		return S_FALSE;
	}

	STDMETHOD(FilterDataObject)( 
		/* [in] */ IUnknown __RPC_FAR *pDO,
		/* [out] */ IUnknown __RPC_FAR *__RPC_FAR *ppDORet)
	{
		return S_FALSE;
	}
public:
	CComPtr<IDispatch> _spExternal;
	CWindow _wnd;
};


enum ConnectType { ConnType_Advise, ConnType_Unadvise };


class CConnInfo
{
public:
	CConnInfo(IDispatch* pWBDisp) :
	  m_spWBDisp(pWBDisp),
		  m_dwDocumentCookie(0),
		  m_dwWindowCookie(0)
	  {
		  m_pDocHandler = NULL;
	  }

	  CComPtr<IDispatch> m_spWBDisp;
	  DWORD m_dwDocumentCookie;
	  DWORD m_dwWindowCookie;
	  CDispObject<CDocEventHandler>*	m_pDocHandler;
};
// @
class CWebView : public CWindowImpl<CWebView, CAxWindow>	
	, public IDispEventSimpleImpl<1, CWebView, &DIID_DWebBrowserEvents2>
{
public:
	static _ATL_FUNC_INFO m_status_text_change_info;
	static _ATL_FUNC_INFO m_new_window2_info;
	static _ATL_FUNC_INFO m_command_state_change_info;
	static _ATL_FUNC_INFO m_complete_navigate2_info;
	static _ATL_FUNC_INFO m_complete_down_info;
	static _ATL_FUNC_INFO m_before_navigate2_info;
	static _ATL_FUNC_INFO m_quit_info;
	static _ATL_FUNC_INFO m_naviaget_complete2_info;

#if 1
	DECLARE_WND_SUPERCLASS(NULL, CAxWindow::GetWndClassName())
#else
	static CWndClassInfo& GetWndClassInfo()
	{
		static CWndClassInfo wc =
		{
			{ sizeof(WNDCLASSEX), CS_DBLCLKS, StartWindowProc, \
				0, 0, NULL, NULL, NULL, NULL, NULL, NULL/*WndClassName*/, NULL }, \
				CAxWindow::GetWndClassName(), NULL, NULL, TRUE, 0, _T("") \
		};
		ATLTRACE("wc style:%d\b", wc.m_wc.style);
		return wc;
	}
#endif

	BOOL PreTranslateMessage(MSG* pMsg)
	{
		if((pMsg->message < WM_KEYFIRST || pMsg->message > WM_KEYLAST) &&
			(pMsg->message < WM_MOUSEFIRST || pMsg->message > WM_MOUSELAST))
			return FALSE;

		//if(pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_F5)
		//	return FALSE;

		// give HTML page a chance to translate this message
		return (BOOL)SendMessage(WM_FORWARDMSG, 0, (LPARAM)pMsg);
	}

	BEGIN_MSG_MAP(CWebView)
		// MSG_WM_MBUTTONDBLCLK
		// MESSAGE_HANDLER( WM_PARENTNOTIFY, OnParentNotify )
		MESSAGE_HANDLER( WM_DESTROY, OnDestroy)
	END_MSG_MAP()

	LRESULT OnDestroy(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		// TODO: WM_DESTROY 会被收到两次，是否有问题
#if 0
		if( _pExternal )
		{
			_pExternal->Release();
			_pExternal = 0;
		}

		if(_pHostUI)
		{
			_pHostUI->Release();
			_pHostUI = 0;
		}
#endif
		if(_pWebEvents)
		{
			CComPtr<IUnknown> spUnk;
			QueryControl(&spUnk);
			AtlUnadvise(spUnk, DIID_DWebBrowserEvents2, _dwWinSink);

			_pWebEvents->Release();
			_pWebEvents = 0;
		}

		_spHTMLDoc.Release();

		return 0;
	}

	LRESULT OnParentNotify(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		switch(LOWORD(wParam))
		{
		case WM_CREATE:
			break;
		case WM_DESTROY:
			// MessageBox( "Destroy OK", "", MB_OK );
			//				hWndChild = (HWND)lParam;
			break;
		default:
			//				hWndChild = GetDlgItem(HIWORD(wParam));
			break;
		}
		return 0;
	}


	BEGIN_SINK_MAP(CWebView)
		SINK_ENTRY_INFO(1, DIID_DWebBrowserEvents2, DISPID_DOCUMENTCOMPLETE, OnDocumentComplete, &m_complete_navigate2_info)	
		SINK_ENTRY_INFO(1, DIID_DWebBrowserEvents2, DISPID_BEFORENAVIGATE2, OnBeforeNavigate, &m_before_navigate2_info)	
		SINK_ENTRY_INFO(1, DIID_DWebBrowserEvents2, DISPID_NAVIGATECOMPLETE2, OnNavigateComplete2, &m_naviaget_complete2_info)	
		SINK_ENTRY_INFO(1, DIID_DWebBrowserEvents2, DISPID_ONQUIT, OnQuit, &m_quit_info)	
	END_SINK_MAP()	

	void __stdcall OnDocumentComplete(/*[in]*/ IDispatch* pDisp, 
	/*	[in] */ VARIANT* URL)
	{
		// 捕获事件
		if ( _bEnumElements )
		{
			CConnInfo* pConnInfo = new CConnInfo(pDisp);
			if (pConnInfo)
			{
				m_deqEventHandlers.push_back(pConnInfo);
				ManageEventHandlers(pConnInfo, ConnType_Advise);
			}
		}
		

		if(IsTopFrame(pDisp))
		{
			CComPtr<IHTMLDocument2> pDoc;
			if(GetHtmlDocument(&pDoc)) 
			{
				if ( _bEnumElements )
				{
					EnumForm( pDoc );	
					::PostMessage (_NotifyhWnd, WM_WEBFINISH, 0, 0);
				}
				CComBSTR burl;
				pDoc->get_URL(&burl);
				WTL::CString url(burl);
				
				_url = url;

				url.MakeLower();
				if(url.Find(_T("res://")) == 0
					|| url.Find(_T("file://")) == 0) 
				{
					ATLTRACE(_T("visit url fail:%s\n"), (LPCTSTR)url);
				}
				else 
				{
					if(url.Find(_T("http://")) == 0 && !IsBusy()) 	
					{
						ShowWindow(SW_SHOW);
						if (_pWndWaitting)
						{
							_pWndWaitting->UnLoad();
							_pWndWaitting->ShowWindow(SW_HIDE);								
						}																		
					}
				}
			}			
		}
	}



	void __stdcall OnBeforeNavigate(/*[in]*/ IDispatch* pDisp, /*[in]*/ VARIANT* URL, /*[in]*/ VARIANT* Flags, 
		/*[in]*/ VARIANT* TargetFrameName, /*[in]*/ VARIANT* PostData, /*[in]*/ VARIANT* Headers, 
		/*[out]*/ VARIANT_BOOL* Cancel)
	{
	
		/*
		CComBSTR bsTarget = V_BSTR(TargetFrameName);
		CComBSTR bsUrl = V_BSTR(URL);
		if (bsUrl)
		{
			if (wcsnicmp(bsUrl, L"javascript:", 11) != 0)
			{
				// not javascript
				// unadvise
				while (m_deqEventHandlers.size() > 0)
				{
					CConnInfo* pConnInfo = m_deqEventHandlers.front();
					ManageEventHandlers(pConnInfo, ConnType_Unadvise);
					delete pConnInfo;
					m_deqEventHandlers.pop_front();
				}
			}
			//else
			{
				// Allow javascript to open the window since the 
				// user initiated it by clicking on a link.
				//m_bUserClickedLink = TRUE;
			}
		}
		*/
		*Cancel =  VARIANT_FALSE;

	}

	void __stdcall OnQuit()
	{
		ReleaseConnInfo();
	}

	void __stdcall OnNavigateComplete2(/*[in]*/ IDispatch* pDisp, 
		/*	[in] */ VARIANT* URL)
	{
		::PostMessage( _NotifyhWnd, WM_CHANGEURL, 0, 0 );			
	};

	BOOL IsBusy()
	{
		CComPtr<IWebBrowser2> spBrowser;
		if( SUCCEEDED(QueryControl(&spBrowser)) )
		{
			VARIANT_BOOL bBusy;
			spBrowser->get_Busy(&bBusy);
			if (bBusy == VARIANT_TRUE)
				return TRUE;
			else
				return FALSE;
		}
		return FALSE;
	}

	CString GetLocationURL()
	{
		CComPtr<IWebBrowser2> spBrowser;
		if( SUCCEEDED(QueryControl(&spBrowser)) )
		{
			CComBSTR szResult;
			spBrowser->get_LocationURL(&szResult);
			return szResult;
		}
		return "";
	}



	BOOL IsTopFrame(IDispatch* pDisp)
	{
		CComPtr<IWebBrowser2> spBrowser;
		if( SUCCEEDED(QueryControl(&spBrowser)) )
		{
			return spBrowser.IsEqualObject(pDisp)?TRUE:FALSE;
		}
		return FALSE;

	}

	BOOL GetHtmlDocument(IHTMLDocument2** ppDoc)
	{
		CComPtr<IDispatch> pDisp;
		CComPtr<IWebBrowser2> spBrowser;
		if( SUCCEEDED(QueryControl(&spBrowser)) )
		{
			HRESULT hr = spBrowser->get_Document(&pDisp);
			if(SUCCEEDED(hr) && pDisp) {
				hr = pDisp->QueryInterface(IID_IHTMLDocument2, (void**)ppDoc);
			}
			return (SUCCEEDED(hr) && (*ppDoc))? TRUE : FALSE;
		}
		return FALSE;		
	}


	HRESULT Navigate(LPCTSTR pszName)
	{
		CString url = pszName;
		if( url.Find(_T("http://")) == 0 )
		{
			ShowWindow(SW_HIDE);
			if (_pWndWaitting)
			{
				_pWndWaitting->Draw();
				_pWndWaitting->ShowWindow(SW_SHOW);
			}
		}
		

		CComPtr<IWebBrowser2> spBrowser;
		if( SUCCEEDED(QueryControl(&spBrowser)) )
		{
			CComBSTR url(pszName);
			CComVariant empty;
			return spBrowser->Navigate(url, &empty, &empty, &empty, &empty);
		}
		return S_FALSE;
	}
#if 0 // for host MSHTML
	HRESULT Load(BSTR bsURL)
	{
		if( !bsURL )
			return E_FAIL;

		DWORD dwFlags = 0;

		URL_COMPONENTSW urlComponents;
		ZeroMemory((PVOID)&urlComponents, sizeof(URL_COMPONENTSW));
		urlComponents.dwStructSize = sizeof(URL_COMPONENTSW);

		if( InternetCrackUrlW( bsURL, 0, dwFlags, &urlComponents ) )
		{
			if( INTERNET_SCHEME_FILE == urlComponents.nScheme )
				return LoadURLFromFile( bsURL );
			else
				return LoadURLFromMoniker( bsURL );
		}
		ATLASSERT( false && "InternetCrackUrlW return false" && GetLastError() );
		return E_FAIL;
	}

	HRESULT LoadURLFromMoniker(BSTR bsURL)
	{
		if( !_spHTMLDoc )
			return S_FALSE;

		// 1 CreateURLMoniker
		CComPtr<IMoniker> spmk;
		HRESULT hr = CreateURLMoniker( 0, bsURL, &spmk );
		HRASSERT( hr );

		// 2 BindCtx
		CComPtr<IBindCtx> spBCtx;
		hr = CreateBindCtx( 0, &spBCtx );
		HRASSERT( hr );

		CComQIPtr<IPersistMoniker> spPMk = _spHTMLDoc;
		hr = spPMk->Load( FALSE, spmk, spBCtx, STGM_READ );
		HRASSERT( hr );
		return hr;
	}

	HRESULT LoadURLFromFile(BSTR bsURL)
	{
		CComQIPtr<IPersistFile> sppf = _spHTMLDoc;
		if( sppf )
			return sppf->Load( bsURL, 0 );
		return S_FALSE;
	}
#endif // for host MSHTML

	IHTMLDocument2* getHTMLDocument2()
	{
		// CAUTION: for different host type
		// Host 类型不同，得到 IHTMLDocument2 的方法也不一样
		CComPtr<IWebBrowser2> spBrowser;
		if( SUCCEEDED(QueryControl(&spBrowser)) )
		{
			CComPtr<IDispatch> spdoc;
			spBrowser->get_Document(&spdoc);

			CComQIPtr<IHTMLDocument2> spMSHTML = spdoc;

			if( spMSHTML )
				return spMSHTML.Detach();
		}
		else
		{
			CComPtr<IHTMLDocument2> spMSHTML;
			QueryControl( &spMSHTML );

			if( spMSHTML )
				return spMSHTML.Detach();
		}

		ATLASSERT( false && "getHTMLDocument2 Error" );
		return 0;
	}

	IHTMLWindow2* getHTMLWindow2()
	{
		if( _spHTMLDoc )
		{
			CComQIPtr<IHTMLWindow2> spwin;
			_spHTMLDoc->get_parentWindow(&spwin);
			if( spwin )
				return spwin.Detach();
		}
		ATLASSERT( false && "getHTMLWindow2 Error" );
		return 0;
	}


	// Handler prototypes (uncomment arguments if needed):
	//	LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	//	LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	//	LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)

public:

	void SetWaitHwnd(CPictureExWnd* pWnd)
	{
		_pWndWaitting = pWnd;
	}

	void Init()
	{
		InitBrowser();

		InitDisp();

		// TODO: release use it
		//InitWinSink();
	}

	void RefreshEnumForm()
	{
		//刷新当前的显示
		CComPtr<IHTMLDocument2> pDoc;
		if(GetHtmlDocument(&pDoc)) 
		{
			if ( _bEnumElements )
			{
				EnumForm( pDoc );	
			}
		}
	}

	/// #region 
	/// insert <DIV ID="ScriptDiv" style="display:none"></Div>
	IHTMLElement* InstallScriptDummy(IHTMLDocument2 * pdoc)
	{
		CComPtr<IHTMLElement> spembody;
		pdoc->get_body(&spembody);

		if( spembody )
		{
			CComBSTR str(L"<DIV ID=\"");
			LPCWSTR ScriptDummyID = L"ScriptDiv";
			str += ScriptDummyID;
			str += L"\" style=\"display:none\"></Div>";
			spembody->insertAdjacentHTML(L"beforeEnd", str);

			return GetScriptDummy(pdoc);
		}
		return 0;
	}

	IHTMLElement* GetScriptDummy(IHTMLDocument2 * pdoc)
	{
		CComQIPtr<IHTMLDocument3> spdoc3 = pdoc;
		if( spdoc3 )
		{
			CComPtr<IHTMLElement> spem;
			LPCWSTR ScriptDummyID = L"ScriptDiv";
			CComBSTR id(ScriptDummyID);
			spdoc3->getElementById(id, &spem);
			if( spem )
				return spem.Detach();
		}
		return 0;
	}

	HRESULT InsertScriptFile(IHTMLElement * pem, LPCWSTR scpUrl)
	{
		// CAUTION: &nbsp; is VERY important!!!!
		// &nbsp;<script defer="true" src="a.js"></script>
		CComBSTR html(L"<span>script in</span><script defer src=\"");
		html += scpUrl;
		html += L"\"></script>";
		return pem->insertAdjacentHTML(L"beforeEnd", html);
	}

	HRESULT RunScript(LPCWSTR scpFunc)
	{
		CComDispatchDriver spscp;
		HRESULT hr = _spHTMLDoc->get_Script(&spscp);
		if( SUCCEEDED(hr) && spscp )
		{
			DISPID id = DISPID_UNKNOWN;
			hr = spscp.GetIDOfName(scpFunc, &id);
			if( DISPID_UNKNOWN != id )
			{
				CComVariant ret;
				return spscp.Invoke0(id, &ret);
			}
		}
		return hr;
	}

	HRESULT RunScript(LPCWSTR scpFunc, CString& strArg1)
	{
		CComDispatchDriver spscp;
		HRESULT hr = _spHTMLDoc->get_Script(&spscp);
		if( SUCCEEDED(hr) && spscp )
		{
			DISPID id = DISPID_UNKNOWN;
			hr = spscp.GetIDOfName(scpFunc, &id);
			if( DISPID_UNKNOWN != id )
			{
				CComVariant ret;
				CComVariant arg1(strArg1);				
				return spscp.Invoke1(id, &arg1, &ret);
			}
		}
		return hr;
	}

	HRESULT RunScript(LPCWSTR scpFunc, CString& strArg1, CString& strArg2 )
	{
		CComDispatchDriver spscp;
		HRESULT hr = _spHTMLDoc->get_Script(&spscp);
		if( SUCCEEDED(hr) && spscp )
		{
			DISPID id = DISPID_UNKNOWN;
			hr = spscp.GetIDOfName(scpFunc, &id);
			if( DISPID_UNKNOWN != id )
			{
				CComVariant ret;
				CComVariant arg1(strArg1);
				CComVariant arg2(strArg2);
				return spscp.Invoke2(id, &arg1, &arg2, &ret);
			}
		}
		return hr;
	}
	/// #endregion

	// insert script into ScriptDummy
	// scpUrl : <script src="http://host.com/a.js"> or pack://dir/a.js or /d.js
	// scpFunc : after insert the js file, we run it! no parameters!
	HRESULT InsertScript(LPCWSTR scpUrl, LPCWSTR scpFunc)
	{
		ATLTRACE2(atlTraceLog, 1, "Log: InsertScript: %S, %S\n", scpUrl, scpFunc);
		// 
		CComPtr<IHTMLElement> spdummy = GetScriptDummy(_spHTMLDoc);
		if( !spdummy )
			spdummy = InstallScriptDummy(_spHTMLDoc);

		if( !spdummy )
		{
			ATLTRACE2(atlTraceError, 0, "Error: InsertScript, cannot get ScriptDummy, %S, %S\n", scpUrl, scpFunc);
			return S_FALSE;
		}

		HRESULT hr = InsertScriptFile(spdummy, scpUrl);
		if( FAILED(hr) )
		{
			ATLTRACE2(atlTraceError, 0, "Error: InsertScriptFile, %08X\n", hr);
			return hr;
		}

		hr = RunScript(scpFunc);
		if( FAILED(hr) )
		{
			ATLTRACE2(atlTraceError, 0, "Error: RunScript, %08X\n", hr);
			return hr;
		}

		return S_OK;
	}

	void InsertJS()
	{
		CComPtr<IHTMLElement> spembody;
		_spHTMLDoc->get_body(&spembody);

		CComQIPtr<IHTMLDOMNode> spndbody = spembody;
#if 0
		CComPtr<IHTMLElement> spemnew;
		_spHTMLDoc->createElement(L"script", &spemnew);
		spemnew->setAttribute(L"src", CComVariant(L"a.js"));

		CComQIPtr<IHTMLDOMNode> spndnew = spemnew;

		if( spndbody && spndnew )
		{
			CComPtr<IHTMLDOMNode> spndlast;
			spndbody->get_lastChild(&spndlast);

			CComPtr<IHTMLDOMNode> spndret;
			CComVariant vbefore(spndlast);
			spndbody->insertBefore(spndnew, vbefore, &spndret);

			CComQIPtr<IHTMLScriptElement> spjs = spndret;
		}
#else

		// not work
		spembody->insertAdjacentHTML(L"beforeEnd", L"&nbsp;<script defer=\"true\" src=\"a.js\"></script>");
		// spembody->insertAdjacentHTML(L"beforeEnd", L"<button>hello</button>");
#endif
	}

private:

	HRESULT InitDisp()
	{
#if 0
		ATLTRACE2(atlTraceLog, 1, "WebView: Set external & uihnadler\n");
		HRESULT hr = CComObject<CDocHostUI>::CreateInstance(&_pHostUI);
		if( SUCCEEDED(hr) && _pHostUI )
		{
			_pHostUI->AddRef();
			_pHostUI->_spExternal = _pExternal;
			_pHostUI->_wnd = m_hWnd;
			hr = SetExternalUIHandler(_pHostUI);
			HRASSERT(hr);
		}
#endif	
		_spHTMLDoc = getHTMLDocument2();
		ATLASSERT( _spHTMLDoc );
		return S_OK;
	}
	HRESULT InitWinSink()
	{
		HRESULT hr = CDispObject<WebEvents>::CreateInstance(&_pWebEvents);
		if( FAILED(hr) )
			return hr;

		CComPtr<IWebBrowser2> spUnk;
		QueryControl(&spUnk);
		if( spUnk )
		{
			hr = AtlAdvise(spUnk, _pWebEvents, DIID_DWebBrowserEvents2, &_dwWinSink);
			HRASSERT(hr);
			return hr;
		}
		return S_FALSE;
	}
	HRESULT InitBrowser()
	{

		CComPtr<IUnknown> spU;
		QueryControl(IID_IUnknown, (void**)&spU);
		if (spU != NULL)
			DispEventAdvise(spU);

		CComPtr<IWebBrowser2> spUnk;
		QueryControl(&spUnk);
		if( spUnk )
		{
			spUnk->put_RegisterAsDropTarget(VARIANT_FALSE);
		}
		CComPtr<IAxWinAmbientDispatch> spamdisp;
		QueryHost(&spamdisp);
		if( spamdisp )
		{
			// spamdisp->put_AllowContextMenu(VARIANT_FALSE);
			spamdisp->put_MessageReflect(VARIANT_FALSE);

			DWORD flags;
			spamdisp->get_DocHostFlags(&flags);
			spamdisp->put_DocHostFlags(flags | DOCHOSTUIFLAG_THEME);
		}
		return S_OK;
	}


	//
	// Set event handlers for HTMLDocumentEvents2 and HTMLWindowEvents2
	//
	HRESULT ManageEventHandlers(CConnInfo* pConnInfo, ConnectType eConnectType)
	{
		HRESULT hr = E_FAIL;

		CComQIPtr<IWebBrowser2, &IID_IWebBrowser2> spWB(pConnInfo->m_spWBDisp);
		if (spWB)
		{
			CComPtr<IDispatch> spDocDisp;
			hr = spWB->get_Document(&spDocDisp);
			if (SUCCEEDED(hr) && spDocDisp)
			{
				CComQIPtr<IHTMLDocument2, &IID_IHTMLDocument2> spDoc(spDocDisp);
				if (spDoc)
				{
					IHTMLWindow2* spWnd = NULL;
					hr = spDoc->get_parentWindow(&spWnd);
					if (SUCCEEDED(hr) && spWnd)
					{
						if (eConnectType == ConnType_Advise)
						{
							DWORD dwCookie = 0;

							CDispObject<CDocEventHandler>* pDocHandler = NULL;
							hr = CDispObject<CDocEventHandler>::CreateInstance (&pDocHandler);
							ATLASSERT(SUCCEEDED(hr));
							if (SUCCEEDED(hr) && pDocHandler)
							{
								pDocHandler->m_hNotifyWnd = _NotifyhWnd;
								if ( pConnInfo->m_pDocHandler )
									pConnInfo->m_pDocHandler->Release();

								pConnInfo->m_pDocHandler = pDocHandler;

								CComQIPtr<IDispatch> spDocHandlerDisp(pDocHandler);
								ATLASSERT(spDocHandlerDisp);

								ATLASSERT(pConnInfo->m_dwDocumentCookie == 0);
								dwCookie = 0;
								hr = AtlAdvise(spDoc, spDocHandlerDisp, DIID_HTMLDocumentEvents2, &dwCookie);
								ATLASSERT(SUCCEEDED(hr));
								if (SUCCEEDED(hr))
									pConnInfo->m_dwDocumentCookie = dwCookie;

								spDocHandlerDisp.Release();
								pDocHandler->Release();

							}

						}
						else
						{
							ATLASSERT(pConnInfo->m_dwDocumentCookie != 0);
							hr = AtlUnadvise(spWnd, DIID_HTMLWindowEvents2, pConnInfo->m_dwDocumentCookie);
							ATLASSERT(SUCCEEDED(hr) || hr == CONNECT_E_NOCONNECTION);
							pConnInfo->m_dwDocumentCookie = 0;

						}
					}
				}
			}
		}

		//ATLASSERT(SUCCEEDED(hr));
		return hr;
	}

	void EnumFrame( IHTMLDocument2 * pIHTMLDocument2 )
	{
		if ( !pIHTMLDocument2 )	return;

		HRESULT hr;

		CComPtr< IHTMLFramesCollection2 > spFramesCollection2;
		pIHTMLDocument2->get_frames( &spFramesCollection2 );	//取得框架frame的集合

		long nFrameCount=0;				//取得子框架个数
		hr = spFramesCollection2->get_length( &nFrameCount );
		if ( FAILED ( hr ) || 0 == nFrameCount )	return;

		for(long i=0; i<nFrameCount; i++)
		{
			CComVariant vDispWin2;		//取得子框架的自动化接口
			hr = spFramesCollection2->item( &CComVariant(i), &vDispWin2 );
			if ( FAILED ( hr ) )	continue;

			CComQIPtr< IHTMLWindow2 > spWin2 = vDispWin2.pdispVal;
			if( !spWin2 )	continue;	//取得子框架的 IHTMLWindow2 接口

			CComPtr < IHTMLDocument2 > spDoc2;
			spWin2->get_document( &spDoc2 );	//取得字框架的 IHTMLDocument2 接口

			EnumForm( spDoc2 );			//递归枚举当前子框架 IHTMLDocument2 上的表单form
		}
	}

	void EnumForm( IHTMLDocument2 * pIHTMLDocument2 )
	{
		_vectElements.clear();

		if( !pIHTMLDocument2 )	return;

		EnumFrame( pIHTMLDocument2 );	//递归枚举当前 IHTMLDocument2 上的子框架frame

		HRESULT hr;
		CComBSTR bstrTitle;
		pIHTMLDocument2->get_title( &bstrTitle );	//取得文档标题

		USES_CONVERSION;
		ostringstream strTrace;
		strTrace << _T("====================") << endl;
		strTrace << _T("开始枚举“") << OLE2CT( bstrTitle ) << _T("”的表单") << endl;
		strTrace << _T("==================== \r\n") << endl;

		//

		CComQIPtr< IHTMLElementCollection > spElementCollection;
		hr = pIHTMLDocument2->get_forms( &spElementCollection );	//取得表单集合
		if ( FAILED( hr ) )
		{
			strTrace << _T("获取表单的集合 IHTMLElementCollection 错误") << endl;
			return;
		}

		long nFormCount=0;				//取得表单数目
		hr = spElementCollection->get_length( &nFormCount );
		if ( FAILED( hr ) )
		{
			strTrace << _T("获取表单数目错误") << endl;
			return;
		}

		for(long i=0; i<nFormCount; i++)
		{
			
			ElementCollect	elments;

			IDispatch *pDisp = NULL;	//取得第 i 项表单
			hr = spElementCollection->item( CComVariant( i ), CComVariant(), &pDisp );
			if ( FAILED( hr ) )		continue;

			CComQIPtr< IHTMLFormElement > spFormElement = pDisp;
			pDisp->Release();

			CComBSTR bstrFormName;
			spFormElement->get_name( &bstrFormName );
			
			CComBSTR bstrFormAction;
			spFormElement->get_action( &bstrFormAction );

			CComBSTR bstrFormMethod;
			spFormElement->get_method( &bstrFormMethod );

			elments._FormName = OLE2T( bstrFormName ) ;
			elments._FormUrl = OLE2T( bstrFormAction ) ;
			elments._PostMethod = OLE2T( bstrFormMethod ) ;

			strTrace << _T("Form的名字：“") << elments._FormName << _T("”\r\n") << endl;
			strTrace << _T("Form的URL：“") << elments._FormUrl << _T("”\r\n") << endl;
		
			long nElemCount=0;			//取得表单中 域 的数目
			hr = spFormElement->get_length( &nElemCount );
			if ( FAILED( hr ) )		continue;

			for(long j=0; j<nElemCount; j++)
			{
				CComDispatchDriver spInputElement;	//取得第 j 项表单域
				hr = spFormElement->item( CComVariant( j ), CComVariant(), &spInputElement );
				if ( FAILED( hr ) )	continue;

				CComVariant vName,vVal,vType;		//取得表单域的 名，值，类型
				hr = spInputElement.GetPropertyByName( L"name", &vName );
				if( FAILED( hr ) )	continue;
				hr = spInputElement.GetPropertyByName( L"value", &vVal );
				if( FAILED( hr ) )	continue;
				hr = spInputElement.GetPropertyByName( L"type", &vType );
				if( FAILED( hr ) )	continue;

				LPCTSTR lpName = vName.bstrVal?
					OLE2CT( vName.bstrVal ) : _T("NULL");	//未知域名
				LPCTSTR lpVal  = vVal.bstrVal?
					OLE2CT( vVal.bstrVal  ) : _T("NULL");	//空值，未输入
				LPCTSTR lpType = vType.bstrVal?
					OLE2CT( vType.bstrVal ) : _T("NULL");	//未知类型

				strTrace << _T("[") << lpType << _T("] ");
				strTrace << lpName << _T(" = ") << lpVal << endl;

				ElementItem		element;
				element._Name = OLE2T( vName.bstrVal ) ;
				element._Value = OLE2T( vVal.bstrVal ) ;
				element._Type = OLE2T( vType.bstrVal ) ;

				elments._vectElement.push_back( element );

				
			}
			//想提交这个表单吗？删除下面语句的注释吧
			//pForm->submit();

			_vectElements.push_back( elments );
		}

		TRACE("%s", strTrace.str().c_str() );

		
	}
private:
	CComPtr<IHTMLDocument2> _spHTMLDoc;
	DWORD _dwWinSink;
	CDispObject<WebEvents> *_pWebEvents;
	// CDispObject<HostDisp> *_pExternal;
	// CComObject<CDocHostUI> *_pHostUI;

	//等待图片
	CPictureExWnd*			_pWndWaitting;

	std::deque<CConnInfo*>		m_deqEventHandlers;

	BOOL					_bEnumElements;
	HWND					_NotifyhWnd;

	

public:

	std::vector<ElementCollect>	_vectElements;
	WTL::CString			_url;

public:
	CWebView() : _pWebEvents(0)
		// , _pHostUI(0), _pExternal(0)
	{
		_pWndWaitting	= NULL;
		_bEnumElements	= FALSE;
		_NotifyhWnd		= NULL;
	}

	~CWebView()
	{
		ReleaseConnInfo();
	}

	void ReleaseConnInfo()
	{
		while (m_deqEventHandlers.size() > 0)
		{
			CConnInfo* pConnInfo = m_deqEventHandlers.front();
			ManageEventHandlers(pConnInfo, ConnType_Unadvise);

			if ( pConnInfo->m_pDocHandler )
				pConnInfo->m_pDocHandler->Release();

			delete pConnInfo;
			
			m_deqEventHandlers.pop_front();
		}		
		


	}
	void SetNotifyWnd( HWND hWnd )
	{
		_bEnumElements = TRUE;
		_NotifyhWnd = hWnd;
	}
};

_declspec(selectany) _ATL_FUNC_INFO CWebView::m_status_text_change_info = 
{CC_STDCALL, VT_EMPTY, 1, {VT_BSTR}};
_declspec(selectany) _ATL_FUNC_INFO CWebView::m_new_window2_info = 
{CC_STDCALL, VT_EMPTY, 2, {VT_BYREF|VT_BOOL,VT_BYREF|VT_DISPATCH}}; 
_declspec(selectany) _ATL_FUNC_INFO CWebView::m_command_state_change_info = 
{CC_STDCALL, VT_EMPTY, 2, {VT_I4,VT_BOOL}};
_declspec(selectany) _ATL_FUNC_INFO CWebView::m_complete_navigate2_info = 
{CC_STDCALL, VT_EMPTY, 2, {VT_DISPATCH,VT_BYREF|VT_VARIANT}};
_declspec(selectany) _ATL_FUNC_INFO CWebView::m_before_navigate2_info = 
{CC_STDCALL, VT_EMPTY, 7,
{VT_DISPATCH,VT_BYREF|VT_VARIANT,VT_BYREF|VT_VARIANT,VT_BYREF|VT_VARIANT,VT_BYREF|VT_VARIANT,VT_BYREF|VT_VARIANT,VT_BYREF|VT_BOOL}};
_declspec(selectany) _ATL_FUNC_INFO CWebView::m_quit_info = 
{CC_STDCALL, VT_EMPTY, 0};
_declspec(selectany) _ATL_FUNC_INFO CWebView::m_complete_down_info = 
{CC_STDCALL, VT_EMPTY, 0};
_declspec(selectany) _ATL_FUNC_INFO CWebView::m_naviaget_complete2_info = 
{CC_STDCALL, VT_EMPTY, 2, {VT_DISPATCH, VT_BYREF|VT_VARIANT}}; 





/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TESTVIEW_H__03B19F97_CDD7_46DF_85E5_01CD1C47449F__INCLUDED_)
