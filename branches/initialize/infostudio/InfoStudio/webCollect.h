#pragma once

#include "Resource.h"
#include "web/disphost.h"
#include "web/dispdyna.h"
#include "web/webview.h"
#include "util.h"
#include "webinfo.h"
#include "condef.h"

class CWebCollectDlg : public CDialogImpl<CWebCollectDlg>
{
public:

	CWebCollectDlg()
	{
		_strUrl			= "";
		_pWebExternal	= 0;
		_pWebInfo		= NULL;
		_pRegister		= NULL;
	}

	~CWebCollectDlg()
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
	enum {IDD = IDD_COLLECTDLG};

	BEGIN_MSG_MAP(CWebCollectDlg)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		COMMAND_ID_HANDLER(IDOK, OnOk)
		COMMAND_ID_HANDLER(IDCANCEL, OnCancel)		

		COMMAND_ID_HANDLER(IDC_BUTTON_PARAM, OnParam)		
		COMMAND_ID_HANDLER(IDC_BUTTON_IMGURL, OnImgUrl)		

		COMMAND_ID_HANDLER(IDC_BUTTON_SAVEINFO, OnSaveRegister)
		COMMAND_ID_HANDLER(IDC_BUTTON_ADDINFO, OnAddRegister)

		MESSAGE_HANDLER(WM_WEBFINISH, OnWebFinish)
		MESSAGE_HANDLER(WM_CHANGEURL, OnChangeUrl)

		MESSAGE_HANDLER(WM_WEBGETCOMBOX, OnWebGetCombox)

		COMMAND_HANDLER(IDC_COMBO_FORM, CBN_SELCHANGE, OnFormSelChanged)

		
	END_MSG_MAP()

	CWebView		_WebView;
	WTL::CString			_strUrl;	
	CDispObject<HostDisp> * _pWebExternal;

	LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		Init();		
		return 0;
	}

	LRESULT OnOk(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		//EndDialog(IDOK);
		
		getWindowText(GetDlgItem(IDC_EDIT_URL), _strUrl);

		if ( _strUrl.GetLength() > 0 )
		{
			_WebView.Navigate( _strUrl );
			//_WebView.RunScript(L"init", _strID, _strType);
		}

		return 0;
	}

	LRESULT OnCancel(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		EndDialog(IDCANCEL);
		return 0;
	}

	LRESULT OnSaveRegister(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		SaveRegister();
		MessageBox("保存成功", MSGTITLE, MB_OK);
		return 0;
	}
	

	LRESULT OnAddRegister(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		_pRegister = NULL;
		SaveRegister();
		MessageBox("保存成功", MSGTITLE, MB_OK);
		return 0;
	}


	LRESULT OnParam(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		
		CComboBox box;
		int nSel = 0;
		box.Attach(GetDlgItem(IDC_COMBO_FORM));
		nSel = box.GetCurSel();
		if(nSel < 0)
			return 0;
	
		SaveForm( nSel );

		return 0;
	}
	
	LRESULT OnImgUrl(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		CString str;
		getWindowText( GetDlgItem( IDC_EDIT_FIELD ),  str );
		
		::SetWindowText( GetDlgItem(IDC_EDIT_VALIDATEURL), str );

		return 0;
	}	


	LRESULT OnWebFinish(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& )
	{
		CComboBox cb;
		cb.Attach( GetDlgItem(IDC_COMBO_FORM) );
		while ( cb.GetCount() > 0 )
			cb.DeleteString( 0 );

		for ( int i = 0; i < _WebView._vectElements.size(); i++ )
		{
			cb.AddString( _WebView._vectElements[i]._FormName);
		}

		if ( _WebView._vectElements.size() > 0 )
		{
			cb.SetCurSel( 0 );
			ListForm( 0 );
		}
		return 0;
	}

	LRESULT OnChangeUrl(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& )
	{
		CString strUrl;
		strUrl = _WebView.GetLocationURL();
		::SetWindowText( GetDlgItem(IDC_EDIT_URL), strUrl );
		return 0;
	}


	LRESULT OnWebGetCombox(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& )
	{
		CComboBox cb;
		cb.Attach( GetDlgItem(IDC_COMBO_LIST) );
		while ( cb.GetCount() > 0 )
			cb.DeleteString( 0 );

		std::vector<CString>*	pValueLst = (std::vector<CString>*)wParam;

		if ( pValueLst->size() > 0 )
		{
			::SetWindowText( GetDlgItem(IDC_EDIT_FIELD), (*pValueLst)[0] );
		}

		if ( pValueLst->size() > 1 )
		{
			for ( int i = 1; i < pValueLst->size(); i++ )
			{
				cb.AddString( (*pValueLst)[i] );
			}
			cb.SetCurSel( 0 );
		}
	
		return 0;
	}

	LRESULT OnFormSelChanged (WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
	{
		CComboBox box;
		int nSel = 0;
		box.Attach(GetDlgItem(IDC_COMBO_FORM));
		nSel = box.GetCurSel();
		if(nSel < 0)
			return 0;
		
		ListForm( nSel );
		return 0;
	}

	//funciton
	void Init()
	{
		CRect rc;
		::GetClientRect( GetDlgItem(IDC_IE), &rc );

		POINT pt;
		pt.x = rc.left;
		pt.y = rc.top;
		::ClientToScreen( GetDlgItem(IDC_IE), &pt );

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

		_WebView.SetNotifyWnd( m_hWnd );

		if ( _strUrl.GetLength() > 0 )
		{
			_WebView.Navigate( _strUrl );
			//_WebView.RunScript(L"init", _strID, _strType);
		}

		CComboBox box;
		box.Attach( GetDlgItem(IDC_COMBO_POSTMETHOD) );
		box.AddString( "POST" );
		box.AddString( "GET" );
		box.SetCurSel( 0 );
		box.Detach();

		box.Attach( GetDlgItem(IDC_COMBO_ENCODE) );
		box.AddString( "GB2312" );
		box.AddString( "UTF-8" );
		box.SetCurSel( 0 );
		box.Detach();

	}


	void ListForm( int nIndex )
	{
		CListBox	listbox;
		listbox.Attach(GetDlgItem(IDC_LIST_FORM));

		while ( listbox.GetCount() > 0 )
			listbox.DeleteString(0);

		if ( nIndex >= _WebView._vectElements.size() )
			return ;
		for ( int i = 0; i < _WebView._vectElements[nIndex]._vectElement.size(); i++ )
		{
			CString strItem;
			strItem = _WebView._vectElements[nIndex]._vectElement[i]._Name + "=" + _WebView._vectElements[nIndex]._vectElement[i]._Value;
			listbox.AddString( strItem );
		}
		
		CString strUrl = OnConversionURL( _WebView._url, _WebView._vectElements[nIndex]._FormUrl);
		
		::SetWindowText( GetDlgItem(IDC_EDIT_POSTURL), strUrl );

		CComboBox box;
		box.Attach( GetDlgItem(IDC_COMBO_POSTMETHOD) );

		if ( _WebView._vectElements[nIndex]._PostMethod == "post" )
			box.SetCurSel( 0 );
		else
			box.SetCurSel( 1 );

	}

	void SaveForm( int nIndex )
	{
		if ( nIndex >= _WebView._vectElements.size() )
			return ;

		CString strInfo = "";

		for ( int i = 0; i < _WebView._vectElements[nIndex]._vectElement.size(); i++ )
		{
			CString strItem;
			strItem = _WebView._vectElements[nIndex]._vectElement[i]._Name + "=" + _WebView._vectElements[nIndex]._vectElement[i]._Value;
			if ( i < _WebView._vectElements[nIndex]._vectElement.size() - 1 )
				strItem += "&";

			strInfo += strItem;
		}

		::SetWindowText( GetDlgItem(IDC_EDIT_PARAM), strInfo );
	}

	void SaveRegister()
	{
		//判断是新增还是修改
		if ( !_pRegister )
		{

			//表示是新增的。创建一个吧
			_pRegister = new webRegister();
			_pRegister->_id		= _pWebInfo->_id;
			_pRegister->_item	= _pWebInfo->GetMaxItem( _type ) + 1;
			_pRegister->_type	= _type;
			_pWebInfo->AddWebRegister( _pRegister );
		}

		if ( _pRegister )
		{
			getWindowText( GetDlgItem( IDC_EDIT_URL ), _pRegister->_url );
			getWindowText( GetDlgItem( IDC_EDIT_POSTURL ), _pRegister->_posturl );

			CString str = "";

			getWindowText( GetDlgItem( IDC_EDIT_PARAM ), str );	//以&分割 里面用=

			_pRegister->_postMap.clear();

			vector<std::string> paramList;
			str_split( paramList, str.GetBuffer( str.GetLength() ) , _T("&") );
			for ( int i = 0; i < paramList.size(); i++ )
			{
				if ( paramList[i] != "" )
				{
					vector<std::string> itemList;
					str_split( itemList, paramList[i].c_str() , _T("=") );	
					CString strLeft = itemList[0].c_str();
					CString strRight = "";
					if ( itemList.size() > 1 )
					{
						for ( int j = 1; j < itemList.size(); j++ )
						{
							strRight += itemList[j].c_str();
						}
					}

					_pRegister->_postMap.insert( std::make_pair( strLeft, strRight ) );

				}
			}


			
			CComboBox box;
			box.Attach( GetDlgItem(IDC_COMBO_POSTMETHOD) );
			_pRegister->_httptype = box.GetCurSel();
			box.Detach();

			
			box.Attach( GetDlgItem(IDC_COMBO_ENCODE) );
			_pRegister->_utf8 = box.GetCurSel() == 0 ? FALSE : TRUE;
			box.Detach();
			
			getWindowText( GetDlgItem( IDC_EDIT_SUCC ), _pRegister->_success );
			
			box.Detach();

		}

	}

public:

	CWebInfo*			_pWebInfo;
	webRegister*		_pRegister;
	int					_type;//是注册，登录，还是信息发布

};