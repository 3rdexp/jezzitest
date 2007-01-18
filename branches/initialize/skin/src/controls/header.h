#pragma once

#include "../base/skinctrl.h"

namespace Skin {
	template<class BaseT = WTL::CHeaderCtrl>
	struct SkinHeaderCtrl : public SkinControlImpl<SkinHeaderCtrl, BaseT>
	{
		//enum { class_id = HEADER };

		SkinHeaderCtrl()
		{
			m_bInItem   = FALSE;
			m_nItem		= -1;
			m_bCapture  = FALSE;
			//m_bSort     = FALSE;
			m_nSortItem = -1;
			m_isAscSort = TRUE;
			_classid		= HEADER; 
		}

		void OnFirstMessage()
		{
			int i = 0;
		}

		typedef SkinHeaderCtrl<BaseT> this_type;
		typedef SkinControlImpl<SkinHeaderCtrl, BaseT> base_type;

		BEGIN_MSG_MAP(this_type)
			MESSAGE_HANDLER(WM_ERASEBKGND, OnEraseBkgnd)
			MESSAGE_HANDLER(WM_PAINT,OnPaint)	
			MESSAGE_HANDLER(WM_PRINTCLIENT, OnPrintClient)
			MESSAGE_HANDLER(WM_LBUTTONDOWN,OnLbuttonDown)
			MESSAGE_HANDLER(WM_LBUTTONUP,OnLbuttonUp)
			MESSAGE_HANDLER(WM_LBUTTONDBLCLK,OnDbClick)
			MESSAGE_HANDLER(WM_MOUSEMOVE,OnMouseMove)
			MESSAGE_HANDLER(WM_MOUSELEAVE,OnMouseLeave)
		END_MSG_MAP()

		LRESULT OnEraseBkgnd(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
		{
			bHandled = FALSE;
			return 1;
		}	

		LRESULT OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
		{
			WTL::CPaintDC dc(m_hWnd);
			DoPaint(dc);
			return 0;
		}

		LRESULT OnPrintClient(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
		{
			DoPaint( (HDC) wParam );
			return 0;
		}
		
		LRESULT OnLbuttonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
		{
			ATLTRACE("CMyListViewImpl<T>::OnHeadLbuttonDown");
			POINT			pt		= { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
			HD_HITTESTINFO	testInfo= {0};

			bHandled = FALSE;
			BOOL bSort = FALSE;
			if(::GetWindowLong(m_hWnd, GWL_STYLE) & HDS_BUTTONS)
			{
				bSort = TRUE;
			}
			if(!bSort)
				return 0;
			testInfo.pt = pt;
			if(SendMessage(m_hWnd, HDM_HITTEST, NULL, (LPARAM)&testInfo)==-1)
				return 0;
			//如果没有处于分割线上面
			if(testInfo.flags != HHT_ONDIVIDER)
			{
				RECT	Itemrc;
				m_bCapture = TRUE;
				m_bInItem  = TRUE;
				m_nItem	   = testInfo.iItem;

				if(m_nSortItem == m_nItem)
					m_isAscSort = !m_isAscSort;
				else
					m_isAscSort = TRUE;
		
				RECT	rc;
				if(m_nSortItem != m_nItem)
				{
					//更新
					GetItemRect(m_nSortItem,&rc);
					InvalidateRect(&rc);
					m_nSortItem = m_nItem;
				}
				//更新
				GetItemRect(testInfo.iItem,&Itemrc);
				InvalidateRect(&Itemrc,FALSE);
				UpdateWindow();
			}
			bHandled = FALSE;
			return 0;

		}

		LRESULT OnLbuttonUp(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
		{
			if(m_bCapture)
			{
				RECT	Itemrc;

				//更新
				GetItemRect(m_nItem,&Itemrc);
				InvalidateRect(&Itemrc,FALSE);

				m_bInItem  = FALSE;
				m_bCapture = FALSE;
				m_nItem	   = -1;

				UpdateWindow();
			}
			bHandled = FALSE;
			return 0;
		}

		LRESULT OnDbClick(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
		{
			POINT			pt		= { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
			HD_HITTESTINFO	testInfo= {0};

			testInfo.pt = pt;
			if( SendMessage(m_hWnd, HDM_HITTEST, NULL, (LPARAM)&testInfo) == -1 )
				return 0;
			else
				if(testInfo.flags == HHT_ONDIVIDER)
					return 0;
				else
					bHandled = FALSE;
			return 0;
		}

		LRESULT OnMouseMove(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
		{
			POINT			pt		= { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
			BOOL			bInItem	= m_bInItem;

			bHandled = FALSE;
			//if(!m_bCapture)
			//	return 0;
			{
				HD_HITTESTINFO	testInfo= {0};
				m_bInItem = FALSE;
				testInfo.pt = pt;
				if( SendMessage(m_hWnd, HDM_HITTEST, NULL,(LPARAM)&testInfo) == -1 )
					return 0;
				if ( m_bInItem )
				{
					if( testInfo.iItem != m_nItem )
					{
						int nOld = m_nItem;
						m_nItem = testInfo.iItem;
						RECT	Itemrc;
						//更新
						GetItemRect(nOld,&Itemrc);
						InvalidateRect(&Itemrc,FALSE);
					}
				}
				else
					m_nItem = testInfo.iItem;

				m_bInItem = TRUE;

				/*
				if(testInfo.iItem!=m_nItem)
				{
					m_bInItem = FALSE;
				}
				else
				*/
				
			}

			//if(m_bInItem != bInItem)
			{
				RECT	Itemrc;

				//更新
				GetItemRect(m_nItem,&Itemrc);
				InvalidateRect(&Itemrc,FALSE);
				//UpdateWindow();
			}
			TRACKMOUSEEVENT tme;
			tme.cbSize = sizeof( tme );
			tme.dwFlags = TME_LEAVE;
			tme.hwndTrack = m_hWnd;
			TrackMouseEvent( &tme );
			return 0;
		}

		LRESULT OnMouseLeave(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
		{
			if ( m_bInItem )
			{
				m_bInItem = FALSE;
				int nOld = m_nItem;
				m_nItem = -1;
				RECT	Itemrc;
				//更新
				GetItemRect(nOld,&Itemrc);
				InvalidateRect(&Itemrc,FALSE);
			}
			return 0;
		}
		LRESULT DoPaint(HDC dc)
		{
			WTL::CRect rc;
			GetClientRect(&rc);
			WTL::CMemoryDC memdc(dc, rc);

			int nPart;
			int nState;
			nPart = HP_HEADERITEM;
			nState = HIS_NORMAL;

			if(_scheme && _scheme->IsThemeBackgroundPartiallyTransparent(_classid, nPart, nState))
				_scheme->DrawParentBackground(m_hWnd, memdc, &rc);

			// draw background
			if ( _scheme )
				_scheme->DrawBackground(memdc, _classid, nPart, nState, &rc, NULL);

			int nCount = GetItemCount();
			for ( int i = 0; i < nCount; i++)
			{

				TCHAR	szText[ 256 ] = {0};
				HD_ITEM hditem;
				RECT	rect;

				hditem.mask = HDI_TEXT | HDI_FORMAT | HDI_IMAGE | HDI_LPARAM | HDI_WIDTH;
				hditem.pszText = szText;
				hditem.cchTextMax = 255;

				GetItem(i, &hditem);
				GetItemRect(i,&rect);
				
				DoPaintHeadItem((WTL::CDCHandle)(HDC)memdc,rect,i,hditem);
			}

			return 0;
		}

		void DoPaintHeadItem(WTL::CDCHandle memdc,RECT rc,int i,HD_ITEM &hditem)
		{
			int nPart;
			int nState;
			RECT rcClient = rc;
			// draw left
			nPart = HP_HEADERITEMLEFT;
			if ( m_bCapture )
			{
				if ( m_bInItem && i == m_nItem )
					nState = HILS_PRESSED;
				else
					nState = HILS_NORMAL;
			}
			else
			{
				if ( m_bInItem && i == m_nItem )
					nState = HILS_HOT;
				else
					nState = HILS_NORMAL;
			}
			
			int nLeftWidth = GetSchemeWidth(nPart, nState);
			rcClient.right = rc.left + nLeftWidth;

			if ( _scheme )
				_scheme->DrawBackground(memdc, _classid, nPart, nState, &rcClient, NULL);

			// draw right

			nPart = HP_HEADERITEMRIGHT;
			if ( m_bCapture )
			{
				if ( m_bInItem && i == m_nItem )
					nState = HIRS_PRESSED;
				else
					nState = HIRS_NORMAL;
			}
			else
			{
				if ( m_bInItem && i == m_nItem )
					nState = HIRS_HOT;
				else
					nState = HIRS_NORMAL;
			}

			int nRightWidth = GetSchemeWidth(nPart, nState);
			rcClient = rc;
			rcClient.left = rc.right - nRightWidth;
			if ( _scheme )
				_scheme->DrawBackground(memdc, _classid, nPart, nState, &rcClient, NULL);

			// draw center
			nPart = HP_HEADERITEM;
			if ( m_bCapture )
			{
				if ( m_bInItem && i == m_nItem )
					nState = HIS_PRESSED;
				else
					nState = HIS_NORMAL;
			}
			else
			{
				if ( m_bInItem && i == m_nItem )
					nState = HIS_HOT;
				else
					nState = HIS_NORMAL;
			}

			rcClient = rc;
			rcClient.left = rc.left + nLeftWidth;
			rcClient.right = rc.right - nRightWidth;
			if ( _scheme )
				_scheme->DrawBackground(memdc, _classid, nPart, nState, &rcClient, NULL);

			//draw imagelist
			WTL::CImageList pImageList = GetImageList();
			if ( !pImageList.IsNull() )
			{
				int margin = GetBitmapMargin();
				RECT rcImage = rcClient;
				rcImage.left += margin;
				IMAGEINFO* pImageInfo = NULL;
				BOOL bGetInfo = pImageList.GetImageInfo( i, pImageInfo );
				if ( bGetInfo && pImageInfo )
				{
					rcImage.top = rcImage.top + (rcImage.bottom - rcImage.top - (pImageInfo->rcImage.bottom - pImageInfo->rcImage.top) ) / 2;
					pImageList.Draw(memdc, i, rcImage.left, rcImage.top, ILD_TRANSPARENT);
					rcClient.left = rcClient.left + margin + pImageInfo->rcImage.right - pImageInfo->rcImage.left;
				}
			}

			// draw arrow
			BOOL bItemSort = FALSE;
			BOOL bSort = FALSE;
			if(::GetWindowLong(m_hWnd, GWL_STYLE) & HDS_BUTTONS)
			{
				bSort = TRUE;
			}

			int nArrowWidth = 0;
			int nArrowHeight = 0;
			RECT rcText = { 0 };
			if( bSort && m_nSortItem==i )
			{
				bItemSort = TRUE;
				nArrowWidth = GetSchemeWidth(HP_HEADERSORTARROW, m_isAscSort ? HSAS_SORTEDUP : HSAS_SORTEDDOWN);
				nArrowHeight = GetSchemeHeight(HP_HEADERSORTARROW, m_isAscSort ? HSAS_SORTEDUP : HSAS_SORTEDDOWN);

				rcClient.right -= nArrowWidth;

				memdc.DrawText(hditem.pszText, -1, &rcText, DT_SINGLELINE | DT_CALCRECT);
			}
			// draw text
			memdc.SetBkMode(TRANSPARENT);
		
			HFONT hOldFont = memdc.SelectFont(GetCtrlFont(m_hWnd));
			
			UINT uFormat = DT_SINGLELINE  | DT_END_ELLIPSIS |DT_VCENTER ;

			if( hditem.fmt & HDF_CENTER)
			{
				uFormat |= DT_CENTER;
			}
			else if( hditem.fmt & HDF_RIGHT)
			{
				uFormat |= DT_RIGHT;
				if ( bItemSort )
					rcClient.right -= GetBitmapMargin();
			}
			else
			{
				uFormat |= DT_LEFT;
			}

			if( m_bCapture && m_nItem==i && m_bInItem)
			{
				//被选中,字体往下移动  是否有必要?
				rcClient.top += 1;
			}
			// nPart 和 nState使用HP_HEADERITEM的属性来设置
			if (_scheme)
				_scheme->DrawText(memdc, _classid, nPart, nState, hditem.pszText, uFormat, 0, &rcClient);

			memdc.SelectFont(hOldFont);

			// draw arrow
			if ( bItemSort )
			{
				RECT rcArrow = rcClient;

				if( hditem.fmt & HDF_CENTER)
				{
					rcArrow.left = rcClient.right - ( rcClient.right - rcClient.left - rcText.right + rcText.left ) / 2;
					if ( rcArrow.left < 0 )
						rcArrow.left = 0;
					rcArrow.right = rcArrow.left + nArrowWidth;
				}
				else if( hditem.fmt & HDF_RIGHT)
				{
					rcArrow.left = rcClient.right;
					rcArrow.right = rcArrow.left + nArrowWidth;
				}
				else
				{
					rcArrow.left = rcClient.left + rcText.right - rcText.left + GetBitmapMargin();
					if ( rcArrow.left < 0 )
						rcArrow.left = 0;
					rcArrow.right = rcArrow.left + nArrowWidth;
				}
				rcArrow.top = rc.top + ( rc.bottom - rc.top - nArrowHeight ) / 2;
				if ( rcArrow.top < 0 )
					rcArrow.top = 0;
				rcArrow.bottom = rcArrow.top + nArrowHeight;
				
				nPart = HP_HEADERSORTARROW;
				nState = m_isAscSort ? HSAS_SORTEDUP : HSAS_SORTEDDOWN;

				if (_scheme)
					_scheme->TransparentDraw(memdc, _classid, nPart, nState, rcArrow.left, rcArrow.top);

			}
		}

		static HFONT GetCtrlFont(HWND hwnd)
		{
			HFONT hFont;
			//如果sendmessage得到的是NULL,则使用的是系统字体
			if ( (hFont = (HFONT)::SendMessage(hwnd,WM_GETFONT,0,0))==NULL)
				hFont = (HFONT)::GetStockObject(SYSTEM_FONT);

			return hFont;
		}
	private:

	BOOL	m_bInItem ;
	int		m_nItem	;
	BOOL	m_bCapture ;
	BOOL	m_bSort    ;
	int		m_nSortItem ;
	BOOL	m_isAscSort ;

	};

}; // namespace 
