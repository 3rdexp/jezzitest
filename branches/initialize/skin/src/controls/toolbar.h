#pragma once

#include "../base/skinctrl.h"
#include "../base/skinhookbase.h"

#include <atlctrls.h>
#include <atlwin.h>

namespace Skin {

#define DROPDOWN(item) ((WORD)-item)

#define IS_STDBTN(button)        (((button).fsStyle&(TBSTYLE_SEP|TBSTYLE_CHECKGROUP))==TBSTYLE_BUTTON)
#define IS_CHECKBTN(button)      (((button).fsStyle&(TBSTYLE_SEP|TBSTYLE_CHECKGROUP))==TBSTYLE_CHECK)
#define IS_GROUPBTN(button)      (((button).fsStyle&(TBSTYLE_SEP|TBSTYLE_CHECKGROUP))==TBSTYLE_CHECKGROUP)
#define IS_DROPDOWN(button)      (((button).fsStyle&TBSTYLE_EX_DRAWDDARROWS)==TBSTYLE_EX_DRAWDDARROWS)
#define IS_SEPARATOR(button)     (((button).fsStyle&TBSTYLE_SEP)&&((button).idCommand==0))
#define IS_CONTROL(button)       (((button).fsStyle&TBSTYLE_SEP)&&((button).idCommand!=0))
#define IS_CHECKED(button)       (((button).fsState&TBSTATE_CHECKED)==TBSTATE_CHECKED)
#define IS_ENABLED(button)       (((button).fsState&TBSTATE_ENABLED)==TBSTATE_ENABLED)
#define IS_INDETERMINATE(button) (((button).fsState&TBSTATE_INDETERMINATE)==TBSTATE_INDETERMINATE)
#define IS_PRESSED(button)       (((button).fsState&TBSTATE_ENABLED)==TBSTATE_PRESSED)
#define IS_VISIBLE(button)       (((button).fsState&TBSTATE_HIDDEN)==0)
#define IS_WRAP(button)          (((button).fsState&TBSTATE_WRAP)==TBSTATE_WRAP)
#define IS_DISABLED(button)      (((button).fsState&TBSTATE_ENABLED)!=TBSTATE_ENABLED)
#define TOOLBAR_HasDropDownArrows(exStyle) ((exStyle & TBSTYLE_EX_DRAWDDARROWS) ? TRUE : FALSE)

#define KEYDOWN(Key) ((GetAsyncKeyState(Key) & 0x8000)!=0)


#define DEFPAD_CX 7
#define DEFPAD_CY 6

// 好像是设置下面的值的, 需要加上?? #define TB_UNKWN460              (WM_USER+96)
#define DEFLISTGAP 4

	/* vertical padding used in list mode when image is present */
#define LISTPAD_CY 9

	class SkinToolBarCtrl : public CSkinHookImpl<SkinToolBarCtrl>

	//template<class BaseT = WTL::CToolBarCtrl>
	//struct SkinToolBarCtrl : public SkinControlImpl<SkinToolBarCtrl, BaseT>
	{
	public:
		enum { class_id = TOOLBAR };

		SkinToolBarCtrl()
		{
			m_iListGap	= DEFLISTGAP;
			m_bHorz		= TRUE;
		}

		~SkinToolBarCtrl()
		{
			//UnInstallHook( m_hWnd );
		}

		//typedef SkinToolBarCtrl<BaseT> this_type;
		//typedef SkinControlImpl<SkinToolBarCtrl, BaseT> base_type;

		BEGIN_MSG_MAP(SkinToolBarCtrl)
			MESSAGE_HANDLER(WM_PAINT, OnPaint)	
			MESSAGE_HANDLER(WM_NCPAINT, OnNcPaint)	
			MESSAGE_HANDLER(WM_ERASEBKGND, OnEraseBkgnd)	
		END_MSG_MAP()

		LRESULT OnNcPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
		{
			EraseNonClient();
			return 0;
		}

		LRESULT OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
		{
			//CToolBar* pToolbar = NULL;
			//pToolbar = (CToolBar*)CWnd::FromHandle( m_hWnd ); 
			//TRACE("toolbar count is %d \r\n", pToolbar->GetCount());
			if ( m_bHorz != IsHorz() )
			{
				LRESULT bRet = DefWindowProc();
				m_bHorz = IsHorz();
				Invalidate();
				return bRet;
			}
			WTL::CPaintDC dc(m_hWnd);
			TOOLBAR_Refresh(dc);
			return 0;
		}

		BOOL IsHorz()
		{
			CWindow hParent = GetParent();
			DWORD dwStyle = hParent.GetStyle();
			if (dwStyle & CBRS_ORIENT_VERT)
				return FALSE;
			return TRUE;

		}

		LRESULT OnEraseBkgnd(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
		{
			return 1;
			CRect rect;
			GetClientRect(&rect);
			CDCHandle dc = (HDC)wParam;
			//CMemoryDC dc ( (HDC)wParam, rect);
			dc.FillSolidRect (rect, RGB( 0, 0, 255));
			
		}

		void EraseNonClient()
		{
			// get window DC that is clipped to the non-client area
			WTL::CWindowDC dc( m_hWnd );
			CRect rectClient;
			GetClientRect(rectClient);
			CRect rectWindow;
			GetWindowRect(rectWindow);
			ScreenToClient(rectWindow);
			rectClient.OffsetRect(-rectWindow.left, -rectWindow.top);
			dc.ExcludeClipRect(rectClient);


			// draw borders in non-client area
			rectWindow.OffsetRect(-rectWindow.left, -rectWindow.top);
			dc.FillSolidRect (rectWindow, RGB( 0, 0, 255));

			// erase parts not drawn
			dc.IntersectClipRect(rectWindow);
			

			// draw gripper in non-client area
			DrawGripper(&dc, rectWindow);
		}


		void DrawGripper(WTL::CWindowDC* dc,CRect rcWin)
		{
			
			DWORD dwStyle = GetStyle();

			if (dwStyle & CBRS_FLOATING) return ;

			if (IsHorz())
			{

				rcWin.top += 6;
				rcWin.left += 4;
				rcWin.right =  rcWin.left + 3;
				rcWin.bottom -= 3;


				for (int i = 0; i < rcWin.Height(); i += 2)
				{
					CRect rcWindow;
					WTL::CBrush cb;
					cb.CreateSolidBrush(::GetSysColor(COLOR_BTNSHADOW));
					rcWindow = rcWin;
					rcWindow.top = rcWin.top + i;
					rcWindow.bottom = rcWindow.top + 1;
					dc->FillRect(rcWindow, cb);
				}

			}
			else
			{
				rcWin.top += 2;
				rcWin.left += 2;
				rcWin.right -= 2;
				rcWin.bottom = rcWin.top + 3;
				for (int i=0; i < rcWin.Width(); i += 2)
				{
					CRect rcWindow;
					WTL::CBrush cb;
					cb.CreateSolidBrush(::GetSysColor(COLOR_BTNSHADOW));
					rcWindow=rcWin;
					rcWindow.left = rcWindow.left + i;
					rcWindow.right = rcWindow.left + 1;
					dc->FillRect(rcWindow,cb);

				}

			}

		}

		BOOL IsDropDown (TBBUTTON& tbbutton)
		{
			DWORD dwExtendedStyle = DWORD(DefWindowProc(TB_GETEXTENDEDSTYLE, 0, 0));
			BOOL hasDropDownArrow = (TOOLBAR_HasDropDownArrows(dwExtendedStyle) &&
				(tbbutton.fsStyle & BTNS_DROPDOWN)) ||
				(tbbutton.fsStyle & BTNS_WHOLEDROPDOWN);

			return hasDropDownArrow;
		}
		
		static HFONT GetCtrlFont(HWND hwnd)
		{
			HFONT hFont;
			//如果sendmessage得到的是NULL,则使用的是系统字体
			if ( (hFont = (HFONT)::SendMessage(hwnd,WM_GETFONT,0,0))==NULL)
				hFont = (HFONT)::GetStockObject(SYSTEM_FONT);

			return hFont;
		}

		void TOOLBAR_Refresh(HDC dc)
		{
            //CToolBar * ptb = (CToolBar *)CWnd::FromHandle(m_hWnd);
			WTL::CToolBarCtrl toolbar;
			toolbar = m_hWnd;

			WTL::CRect rcClient;
			GetClientRect(&rcClient);
			WTL::CMemoryDC memdc(dc, rcClient);

			int nPart = TP_SEPARATORVERT + 1; //没有背景的.我加上一个
			int nState = TS_NORMAL;

			//draw background
			if( _scheme )
				_scheme->DrawBackground(memdc, class_id, nPart, nState, &rcClient, NULL );


			WTL::CPoint ptCursor;
			::GetCursorPos (&ptCursor);
			ScreenToClient (&ptCursor);

			WTL::CRect rcClip;
			GetClipBox (dc, rcClip);
			
			HIMAGELIST m_hImageList = (HIMAGELIST)DefWindowProc (TB_GETIMAGELIST, 0, 0);
			TBBUTTON tbbutton;
			LRESULT nCount = DefWindowProc (TB_BUTTONCOUNT, 0, 0);
			int nHotItem = toolbar.GetHotItem();
			
			int OldMode = memdc.SetBkMode(TRANSPARENT);
			HFONT hOldFont = memdc.SelectFont(GetCtrlFont(m_hWnd));

			for ( int i = 0; i < nCount; i++ )
			{
				toolbar.GetButton( i, &tbbutton );
				//DefWindowProc (TB_GETBUTTON, i, (LPARAM)&tbbutton);

				if ( !IS_VISIBLE(tbbutton) )
				{
					continue;
				}
				WTL::CRect rcButton;
#if 1
				BOOL r = toolbar.GetItemRect( i, rcButton );
				//TRACE("rcButton is %d,%d,%d,%d \r\n", rcButton.left, rcButton.top, rcButton.right, rcButton.bottom );
                ASSERT(r);
				// r = DefWindowProc (TB_GETITEMRECT, i, (LPARAM)&rcButton);
#else
                ptb->GetItemRect(i, &rcButton);
#endif

				if ( !WTL::CRect().IntersectRect (rcClip, rcButton) )
				{
					continue;
				}

				if ( nCount == 5 )
				{
					int n;
					n = 1;
				}
				bool bOver = nHotItem == i && IS_ENABLED(tbbutton);
				bool bPressed = false;

				nPart = 0; 
				nState = TS_NORMAL;
				// 没有处理toolbar是垂直的情况.
				if ( IS_SEPARATOR(tbbutton) )
				{
					if ( IS_WRAP(tbbutton) )
					{
						nPart = TP_SEPARATORVERT;
					}
					else
					{
						nPart = TP_SEPARATOR;
					}

					if ( !IsHorz() )
					{
						rcButton.top = rcButton.bottom - rcButton.right;	
					}

					int nSepWidth = GetSchemeWidth( nPart, nState );
					int nSepHeight = GetSchemeHeight( nPart, nState );
					WTL::CRect rcSep ;
					rcSep.left = rcButton.left + ( rcButton.Width() - nSepWidth ) / 2;
					rcSep.right = rcSep.left +  nSepWidth;
					rcSep.top = rcButton.top + ( rcButton.Height() - nSepHeight ) / 2;
					rcSep.bottom = rcSep.top +  nSepHeight;
					if (_scheme)
						_scheme->TransparentDraw(memdc, class_id, nPart, nState, rcSep.left < 0 ? 0 : rcSep.left, rcSep.top);
				}
				else if ( !IS_CONTROL(tbbutton) )
				{
					if ( !toolbar.IsButtonEnabled(tbbutton.idCommand) )
						nState = TS_DISABLED;
					else if ( toolbar.IsButtonChecked(tbbutton.idCommand) )
					{
						if ( toolbar.IsButtonHighlighted(tbbutton.idCommand) )
							nState = TS_HOTCHECKED;
						else
							nState = TS_CHECKED;
					}
					else if ( toolbar.IsButtonPressed(tbbutton.idCommand) )
					{
						nState = TS_PRESSED;
						bPressed = true;
					}
					else if ( toolbar.IsButtonHighlighted(tbbutton.idCommand) )
					{
						nState = TS_HOT;
						bOver = true;
					}
					else 
						nState = TS_NORMAL;

					nPart = TP_BUTTON;

					if (_scheme)
						_scheme->DrawBackground(memdc, class_id, nPart, nState, &rcButton, NULL );

					if ( IsDropDown(tbbutton) )
					{					
						nPart = TP_DROPDOWNBUTTON;
						int nArrowWidth = GetSchemeWidth( nPart, nState );
						int nArrowHeight = GetSchemeHeight( nPart, nState );
						WTL::CRect rcArrow ;
						rcArrow.left = rcButton.right - nArrowWidth;
						rcArrow.right = rcButton.right;
						rcArrow.top = rcButton.top + ( rcButton.Height() - nArrowHeight ) / 2;
						rcArrow.bottom = rcArrow.top +  nArrowHeight;
						if (_scheme)
							_scheme->TransparentDraw(memdc, class_id, nPart, nState, rcArrow.left, rcArrow.top);
					}



					/***********************************************************************
					* 		TOOLBAR_MeasureButton
					*
					* Calculates the width and height required for a button. Used in
					* TOOLBAR_CalcToolbar to set the all-button width and height and also for
					* the width of buttons that are autosized.
					*
					* Note that it would have been rather elegant to use one piece of code for
					* both the laying out of the toolbar and for controlling where button parts
					* are drawn, but the native control has inconsistencies between the two that
					* prevent this from being effectively. These inconsistencies can be seen as
					* artefacts where parts of the button appear outside of the bounding button
					* rectangle.
					*
					* There are several cases for the calculation of the button dimensions and
					* button part positioning:
					*
					* List
					* ====
					*
					* With Bitmap:
					*
					* +--------------------------------------------------------+ ^
					* |                    ^                     ^             | |
					* |                    | pad.cy / 2          | centred     | |
					* | pad.cx/2 + cxedge +--------------+     +------------+  | | DEFPAD_CY +
					* |<----------------->| nBitmapWidth |     | Text       |  | | max(nBitmapHeight, szText.cy)
					* |                   |<------------>|     |            |  | |
					* |                   +--------------+     +------------+  | |
					* |<-------------------------------------->|               | |
					* |  cxedge + iListGap + nBitmapWidth + 2  |<----------->  | |
					* |                                           szText.cx    | |
					* +--------------------------------------------------------+ -
					* <-------------------------------------------------------->
					*  2*cxedge + nBitmapWidth + iListGap + szText.cx + pad.cx
					*
					* Without Bitmap (I_IMAGENONE):
					*
					* +-----------------------------------+ ^
					* |                     ^             | |
					* |                     | centred     | | LISTPAD_CY +
					* |                   +------------+  | | szText.cy
					* |                   | Text       |  | |
					* |                   |            |  | |
					* |                   +------------+  | |
					* |<----------------->|               | |
					* |      cxedge       |<----------->  | |
					* |                      szText.cx    | |
					* +-----------------------------------+ -
					* <----------------------------------->
					*          szText.cx + pad.cx
					*
					* Without text:
					*
					* +--------------------------------------+ ^
					* |                       ^              | |
					* |                       | padding.cy/2 | | DEFPAD_CY +
					* |                     +------------+   | | nBitmapHeight
					* |                     | Bitmap     |   | |
					* |                     |            |   | |
					* |                     +------------+   | |
					* |<------------------->|                | |
					* | cxedge + iListGap/2 |<----------->   | |
					* |                       nBitmapWidth   | |
					* +--------------------------------------+ -
					* <-------------------------------------->
					*     2*cxedge + nBitmapWidth + iListGap
					*
					* Non-List
					* ========
					*
					* With bitmap:
					*
					* +-----------------------------------+ ^
					* |                     ^             | |
					* |                     | pad.cy / 2  | | nBitmapHeight +
					* |                     -             | | szText.cy +
					* |                   +------------+  | | DEFPAD_CY + 1
					* |    centred        |   Bitmap   |  | |
					* |<----------------->|            |  | |
					* |                   +------------+  | |
					* |                         ^         | |
					* |                       1 |         | |
					* |                         -         | |
					* |     centred     +---------------+ | |
					* |<--------------->|      Text     | | |
					* |                 +---------------+ | |
					* +-----------------------------------+ -
					* <----------------------------------->
					* pad.cx + max(nBitmapWidth, szText.cx)
					*
					* Without bitmaps (NULL imagelist or ImageList_GetImageCount() = 0):
					*
					* +---------------------------------------+ ^
					* |                     ^                 | |
					* |                     | 2 + pad.cy / 2  | |
					* |                     -                 | | szText.cy +
					* |    centred      +-----------------+   | | pad.cy + 2
					* |<--------------->|   Text          |   | |
					* |                 +-----------------+   | |
					* |                                       | |
					* +---------------------------------------+ -
					* <--------------------------------------->
					*          2*cxedge + pad.cx + szText.cx
					*
					* Without text:
					*   As for with bitmaps, but with szText.cx zero.
					*/
				
					// 绘制按钮bmp和text

					WTL::CImageList imgList;
					if ( tbbutton.iBitmap >= 0 )
					{
						if ( nState == TS_HOT )
							imgList = toolbar.GetHotImageList();
						else if ( nState == TS_DISABLED )
							imgList = toolbar.GetDisabledImageList();

						if ( imgList.IsNull() )
							imgList = toolbar.GetImageList();
					}

					TCHAR szText[256];
					memset(szText, 0, 256);
					//GetString( tbbutton.idCommand, szText, 256);
					toolbar.GetButtonText( tbbutton.idCommand, szText );
					
					POINT ptButton;
					SIZE sizePad;
					toolbar.GetPadding( &sizePad );
					SIZE szImg = { 0 };

					BOOL bHasBitmap = FALSE;
					if ( !imgList.IsNull() && tbbutton.iBitmap >= 0 && tbbutton.iBitmap < imgList.GetImageCount() )
					{
						bHasBitmap = TRUE;
						imgList.GetIconSize( szImg );
					}

					if ( GetStyle() & TBSTYLE_LIST )
					{
						if ( strlen(szText) > 0 )
						{
							ptButton.x = rcButton.left + sizePad.cx / 2 + GetSystemMetrics(SM_CXEDGE);
							ptButton.y = rcButton.top + sizePad.cy / 2;
						}
						else
						{
							ptButton.x =  rcButton.left + GetSystemMetrics(SM_CXEDGE) + m_iListGap / 2;
							ptButton.y  = rcButton.top + sizePad.cy / 2;
						}

						if ( bHasBitmap )
						{
							::ImageList_Draw (imgList, tbbutton.iBitmap, memdc,
								ptButton.x + ( (bOver && !bPressed) ? 0 : 1 ), ptButton.y + ( (bOver && !bPressed) ? 0 : 1 ), ILD_TRANSPARENT);
						}

						// draw Text
						if ( bHasBitmap )
						{
							rcButton.left = rcButton.left + GetSystemMetrics(SM_CXEDGE) + m_iListGap + szImg.cx + 2;
						}
						else
						{
							rcButton.left =  rcButton.left + GetSystemMetrics(SM_CXEDGE);
						}

						DWORD dwDTFlags = DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS;
						if (_scheme)
							_scheme->DrawText(memdc, class_id, nPart, nState, szText, dwDTFlags, 0, &rcButton);
					}
					else
					{
						int nArrowWidth = 0;
						if ( IsDropDown(tbbutton) )
						{
							nArrowWidth = GetSchemeWidth( TP_DROPDOWNBUTTON, TS_NORMAL );
						}
						ptButton.x = rcButton.left + ( rcButton.Width() - szImg.cx - nArrowWidth ) / 2;
						ptButton.y = rcButton.top + sizePad.cy / 2;
						
						if ( bHasBitmap )
						{
							::ImageList_Draw (imgList, tbbutton.iBitmap, memdc,
								ptButton.x + ( (bOver && !bPressed) ? 0 : 1 ), ptButton.y + ( (bOver && !bPressed) ? 0 : 1 ), ILD_TRANSPARENT);
						}

						// draw Text
						if ( bHasBitmap )
						{
							rcButton.top =  rcButton.top + sizePad.cy / 2 + szImg.cy + 1;
						}
						else
						{
							rcButton.top =  rcButton.top + 2 + sizePad.cy / 2;
						}

						DWORD dwDTFlags = DT_TOP | DT_CENTER | DT_END_ELLIPSIS;
						if (_scheme)
							_scheme->DrawText(memdc, class_id, nPart, nState, szText, dwDTFlags, 0, &rcButton);
					}

					//memdc.DrawText (szText, -1, rcButton, DT_SINGLELINE|DT_LEFT|DT_VCENTER);
				}
				
				continue; 
				//变灰色 不可点击
				if ( IS_INDETERMINATE(tbbutton) )
				{
					WTL::CPen pen;// (WTL::CDC, ::GetSysColor (COLOR_3DSHADOW));
					pen.CreatePen( PS_SOLID, 1, ::GetSysColor (COLOR_3DSHADOW));
					HPEN oldPen = memdc.SelectPen(pen);
					memdc.MoveTo (rcButton.left, rcButton.bottom);
					memdc.LineTo (rcButton.left, rcButton.top);
					memdc.LineTo (rcButton.right-1, rcButton.top);
					memdc.LineTo (rcButton.right-1, rcButton.bottom-1);
					memdc.LineTo (rcButton.left, rcButton.bottom-1);
					memdc.SelectPen(oldPen);
					pen.DeleteObject();
					bOver = true;
				}//鼠标移动.或者check
				else if ( bOver || IS_CHECKED(tbbutton) )
				{
					bPressed = KEYDOWN(MK_LBUTTON) && rcButton.PtInRect (ptCursor);
					if ( IsDropDown(tbbutton) && bPressed )
					{
						bPressed = ptCursor.x < rcButton.right-13;

						if ( bPressed )
						{
							rcButton.right -= 13;
						}
					}
					COLORREF crHighLight = ::GetSysColor (COLOR_HIGHLIGHT);
					WTL::CPen pen;// (WTL::CDC, crHighLight);
					pen.CreatePen (PS_SOLID, 1, crHighLight);
					HPEN oldPen= memdc.SelectPen(pen);
					//WTL::CPenDC pen (WTL::CDC, ::GetSysColor(COLOR_BTNSHADOW));
					WTL::CBrush brush ;//(WTL::CDC, bPressed||(bOver&&IS_CHECKED(tbbutton)) ? HLS_TRANSFORM (crHighLight, +50, -50) : (bOver ? HLS_TRANSFORM (crHighLight, +70, -57) : HLS_TRANSFORM (crHighLight, +80, -66)));
					brush.CreateSolidBrush(  crHighLight );
					HBRUSH oldBrush = memdc.SelectBrush(brush);
					memdc.FillSolidRect(rcButton, crHighLight);
					memdc.Draw3dRect(rcButton,crHighLight,crHighLight);
					//WTL::CDC.Draw3dRect (rcButton, ::GetSysColor (COLOR_BTNHILIGHT), ::GetSysColor(COLOR_BTNSHADOW));//Enable This to Get the classic look

					if ( IsDropDown(tbbutton) )
					{
						if ( bPressed )
						{
							int nLeft = rcButton.left;

							rcButton.left = rcButton.right-1;
							rcButton.right += 13;

							//brush.Color (HLS_TRANSFORM (crHighLight, +70, -66));
							memdc.Draw3dRect (rcButton, crHighLight, crHighLight);

							rcButton.left = nLeft;
						}
						else
						{
							memdc.MoveTo (rcButton.right-14, rcButton.top);
							memdc.LineTo (rcButton.right-14, rcButton.bottom);
						}
					}

					memdc.SelectBrush(oldBrush);
					memdc.SelectPen(oldPen);
				}
				//分隔条
				if ( IS_SEPARATOR(tbbutton) )
				{
					WTL::CPen pen;// (WTL::CDC, crHighLight);
					pen.CreatePen (PS_SOLID, 1, RGB( 255, 0 ,0 ) ) ; // ::GetSysColor (COLOR_BTNFACE));
					HPEN oldPen =  memdc.SelectPen(pen);
					if ( IS_WRAP(tbbutton) )
					{
						memdc.MoveTo (rcClient.left+2, rcButton.bottom-4);
						memdc.LineTo (rcClient.right-2, rcButton.bottom-4);
					}
					else
					{
						memdc.MoveTo ((rcButton.right+rcButton.left)/2-1, rcButton.top+2);
						memdc.LineTo ((rcButton.right+rcButton.left)/2-1, rcButton.bottom-2);
					}
					memdc.SelectPen(oldPen);
				}
				else if ( !IS_CONTROL(tbbutton) )
				{
					if ( IsDropDown(tbbutton) )
					{
						WTL::CPen pen;// (WTL::CDC, crHighLight);
						pen.CreatePen (PS_SOLID, 1, RGB( 255, 0, 0 ) ); //::GetSysColor (COLOR_BTNTEXT));
						HPEN oldPen =  memdc.SelectPen(pen);
						//WTL::CPenDC pen (WTL::CDC, ( bOver && !IS_INDETERMINATE(tbbutton) ) ? RGB(0,0,0) : ::GetSysColor (IS_ENABLED(tbbutton) ? COLOR_BTNTEXT : COLOR_GRAYTEXT));

						memdc.MoveTo (rcButton.right-9, (rcButton.top+rcButton.bottom)/2-1);
						memdc.LineTo (rcButton.right-4, (rcButton.top+rcButton.bottom)/2-1);
						memdc.MoveTo (rcButton.right-8, (rcButton.top+rcButton.bottom)/2);
						memdc.LineTo (rcButton.right-5, (rcButton.top+rcButton.bottom)/2);
						//memdc.SetPixel (rcButton.right-7, (rcButton.top+rcButton.bottom)/2+1, pen.Color());

						rcButton.right -= 14;

						memdc.SelectPen(oldPen);
					}
					if ( tbbutton.iBitmap >= 0 )
					{
						if ( !IS_ENABLED(tbbutton) || (bOver && !bPressed) )
						{
							COLORREF crHighLight = ::GetSysColor (COLOR_HIGHLIGHT);
							if (!bPressed)
							{
								memdc .FillSolidRect(rcButton, crHighLight);
							}
							if (bOver)
							{
								memdc.FillSolidRect(rcButton, crHighLight);
							}
							memdc.Draw3dRect (rcButton, crHighLight, crHighLight);
							HICON hIcon = ImageList_ExtractIcon (NULL, m_hImageList, tbbutton.iBitmap);
							//memdc.DrawState (WTL::CPoint (rcButton.left + ( bOver ? 4 : 3 ), rcButton.top + ( bOver ? 4 : 3 )), m_sizeImage, hIcon, DSS_MONO, WTL::CBrush (bOver ? (IS_INDETERMINATE(tbbutton) ? ::GetSysColor (COLOR_BTNFACE): ::GetSysColor (COLOR_HIGHLIGHT): ::GetSysColor (COLOR_BTNFACE)));
							//WTL::CDC.DrawState (WTL::CPoint (rcButton.left + 2, rcButton.top + 2), m_sizeImage, hIcon, DSS_MONO, WTL::CBrush (bOver ? (IS_INDETERMINATE(tbbutton) ? HLS_TRANSFORM (::GetSysColor (COLOR_3DFACE), -20, 0) : HLS_TRANSFORM (::GetSysColor (COLOR_HIGHLIGHT), +50, -66)) : HLS_TRANSFORM (::GetSysColor (COLOR_3DFACE), -27, 0)));
							DestroyIcon (hIcon);

						}
						if ( IS_DISABLED(tbbutton) )
						{
							memdc.FillSolidRect(rcButton, ::GetSysColor (COLOR_3DFACE));
							//WTL::CBrush icont(::GetSysColor(COLOR_BTNSHADOW));
							WTL::CBrush icont ;
							icont.CreateSolidBrush(  ::GetSysColor(COLOR_BTNSHADOW));

							HICON hIcon = ImageList_ExtractIcon (NULL, m_hImageList, tbbutton.iBitmap);
							//memdc.DrawState (WTL::CPoint (rcButton.left + ( bOver ? 4 : 3 ), rcButton.top + ( bOver ? 4 : 3 )), m_sizeImage, hIcon, DSS_MONO, icont/*WTL::CBrush (bOver ? (IS_INDETERMINATE(tbbutton) ? HLS_TRANSFORM (::GetSysColor (COLOR_3DFACE), -20, 0) : HLS_TRANSFORM (::GetSysColor (COLOR_HIGHLIGHT), +50, -66)) : HLS_TRANSFORM (::GetSysColor (COLOR_3DFACE), -27, 0))*/);
							DestroyIcon (hIcon);
						}
						if ( IS_ENABLED(tbbutton) )
						{
							::ImageList_Draw (m_hImageList, tbbutton.iBitmap, memdc,
								rcButton.left + ( (bOver && !bPressed) ? 2 : 3 ), rcButton.top + ( (bOver && !bPressed) ? 2 : 3 ), ILD_TRANSPARENT);
							/*::ImageList_Draw (m_hImageList, tbbutton.iBitmap, WTL::CDC.m_hDC,//Enable This to get the Non-shadowed buttons
							rcButton.left + 2, rcButton.top + 2, ILD_TRANSPARENT);*/
						}
						/*if (bPressed )//Enable this to get The Classic Style
						{
						COLORREF crHighLight = ::GetSysColor (COLOR_HIGHLIGHT);
						//WTL::CDC.Draw3dRect (rcButton, ::GetSysColor (COLOR_3DSHADOW), ::GetSysColor(COLOR_3DHIGHLIGHT));
						WTL::CDC.FillSolidRect(rcButton,HLS_TRANSFORM (crHighLight, +50, -57));
						}*/
					}
				}

				TCHAR szText[256];
				memset(szText, 0, 256);
				//GetString( i, szText, 256);
				toolbar.GetButtonText( i, szText );
				//if (_scheme)
				//	_scheme->DrawText(memdc, class_id, nPart, nState, szText, GetButtonTextFormat(lStyle), 0, &rc);
				memdc.DrawText (szText, -1, rcButton, DT_SINGLELINE|DT_LEFT|DT_VCENTER);
			}

	
			memdc.SetBkMode(OldMode);
			memdc.SelectFont(hOldFont);
			toolbar.Detach();
		}

	private:
		int		m_iListGap;
		BOOL	m_bHorz;
	};

}; // namespace 
