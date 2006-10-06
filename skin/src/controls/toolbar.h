#pragma once

#include "../base/skinctrl.h"

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

	#define KEYDOWN(Key) ((GetKeyState(Key)&0x8000)!=0)

	template<class BaseT = CToolBarCtrl>
	struct SkinToolBarCtrl : public SkinControlImpl<SkinToolBarCtrl, BaseT>
	{
		enum { class_id = TOOLBAR };

		SkinToolBarCtrl()
		{
			
		}
		typedef SkinToolBarCtrl<BaseT> this_type;
		typedef SkinControlImpl<SkinToolBarCtrl, BaseT> base_type;

		BEGIN_MSG_MAP(this_type)
			MESSAGE_HANDLER(WM_PAINT, OnPaint)	
			MESSAGE_HANDLER(WM_ERASEBKGND, OnEraseBkgnd)	
		END_MSG_MAP()

		LRESULT OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
		{
			CPaintDC dc(m_hWnd);
			TOOLBAR_Refresh(dc);
			return 0;
		}

		LRESULT OnEraseBkgnd(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
		{
			return 1;
		}
		
		void TOOLBAR_Refresh(HDC dc)
		{
			CRect rcClient;
			GetClientRect(&rcClient);
			CMemoryDC memdc(dc, rcClient);

			int nPart = TP_SEPARATORVERT + 1; //没有背景的.我加上一个
			int nState = TS_NORMAL;

			//draw background
			if( _scheme )
				_scheme->DrawBackground(memdc, class_id, nPart, nState, &rcClient, NULL );


			CPoint ptCursor;
			::GetCursorPos (&ptCursor);
			ScreenToClient (&ptCursor);

			CRect rcClip;
			GetClipBox (dc, rcClip);
			
			HIMAGELIST m_hImageList = (HIMAGELIST)DefWindowProc (TB_GETIMAGELIST, 0, 0);
			TBBUTTON tbbutton;
			int nCount = DefWindowProc (TB_BUTTONCOUNT, 0, 0);
			int nHotItem = GetHotItem();
			
			int OldMode = memdc.SetBkMode(TRANSPARENT);

			for ( int i = 0; i < nCount; i++ )
			{
				DefWindowProc (TB_GETBUTTON, i, (LPARAM)&tbbutton);

				if ( !IS_VISIBLE(tbbutton) )
				{
					continue;
				}
				CRect rcButton;
				DefWindowProc (TB_GETITEMRECT, i, (LPARAM)&rcButton);

				if ( !CRect().IntersectRect (rcClip, rcButton) )
				{
					continue;
				}
				bool bOver = nHotItem == i && IS_ENABLED(tbbutton);
				bool bPressed = false;

				

				if ( IS_INDETERMINATE(tbbutton) )
				{
					CPen pen;// (cDC, ::GetSysColor (COLOR_3DSHADOW));
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
				}
				else if ( bOver || IS_CHECKED(tbbutton) )
				{
					bPressed = KEYDOWN(VK_LBUTTON) && rcButton.PtInRect (ptCursor);

					if ( IS_DROPDOWN(tbbutton) && bPressed )
					{
						bPressed = ptCursor.x < rcButton.right-13;

						if ( bPressed )
						{
							rcButton.right -= 13;
						}
					}
					COLORREF crHighLight = ::GetSysColor (COLOR_HIGHLIGHT);
					CPen pen;// (cDC, crHighLight);
					pen.CreatePen (PS_SOLID, 1, crHighLight);
					HPEN oldPen= memdc.SelectPen(pen);
					//CPenDC pen (cDC, ::GetSysColor(COLOR_BTNSHADOW));
					CBrush brush ;//(cDC, bPressed||(bOver&&IS_CHECKED(tbbutton)) ? HLS_TRANSFORM (crHighLight, +50, -50) : (bOver ? HLS_TRANSFORM (crHighLight, +70, -57) : HLS_TRANSFORM (crHighLight, +80, -66)));
					brush.CreateSolidBrush(  crHighLight );
					HBRUSH oldBrush = memdc.SelectBrush(brush);
					memdc.FillSolidRect(rcButton, crHighLight);
					memdc.Draw3dRect(rcButton,crHighLight,crHighLight);
					//cDC.Draw3dRect (rcButton, ::GetSysColor (COLOR_BTNHILIGHT), ::GetSysColor(COLOR_BTNSHADOW));//Enable This to Get the classic look

					if ( IS_DROPDOWN(tbbutton) )
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
				if ( IS_SEPARATOR(tbbutton) )
				{
					CPen pen;// (cDC, crHighLight);
					pen.CreatePen (PS_SOLID, 1, ::GetSysColor (COLOR_BTNFACE));
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
					if ( IS_DROPDOWN(tbbutton) )
					{
						CPen pen;// (cDC, crHighLight);
						pen.CreatePen (PS_SOLID, 1, ::GetSysColor (COLOR_BTNTEXT));
						HPEN oldPen =  memdc.SelectPen(pen);
						//CPenDC pen (cDC, ( bOver && !IS_INDETERMINATE(tbbutton) ) ? RGB(0,0,0) : ::GetSysColor (IS_ENABLED(tbbutton) ? COLOR_BTNTEXT : COLOR_GRAYTEXT));

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
							//memdc.DrawState (CPoint (rcButton.left + ( bOver ? 4 : 3 ), rcButton.top + ( bOver ? 4 : 3 )), m_sizeImage, hIcon, DSS_MONO, CBrush (bOver ? (IS_INDETERMINATE(tbbutton) ? ::GetSysColor (COLOR_BTNFACE): ::GetSysColor (COLOR_HIGHLIGHT): ::GetSysColor (COLOR_BTNFACE)));
							//cDC.DrawState (CPoint (rcButton.left + 2, rcButton.top + 2), m_sizeImage, hIcon, DSS_MONO, CBrush (bOver ? (IS_INDETERMINATE(tbbutton) ? HLS_TRANSFORM (::GetSysColor (COLOR_3DFACE), -20, 0) : HLS_TRANSFORM (::GetSysColor (COLOR_HIGHLIGHT), +50, -66)) : HLS_TRANSFORM (::GetSysColor (COLOR_3DFACE), -27, 0)));
							DestroyIcon (hIcon);

						}
						if ( IS_DISABLED(tbbutton) )
						{
							memdc.FillSolidRect(rcButton, ::GetSysColor (COLOR_3DFACE));
							//CBrush icont(::GetSysColor(COLOR_BTNSHADOW));
							CBrush icont ;
							icont.CreateSolidBrush(  ::GetSysColor(COLOR_BTNSHADOW));

							HICON hIcon = ImageList_ExtractIcon (NULL, m_hImageList, tbbutton.iBitmap);
							//memdc.DrawState (CPoint (rcButton.left + ( bOver ? 4 : 3 ), rcButton.top + ( bOver ? 4 : 3 )), m_sizeImage, hIcon, DSS_MONO, icont/*CBrush (bOver ? (IS_INDETERMINATE(tbbutton) ? HLS_TRANSFORM (::GetSysColor (COLOR_3DFACE), -20, 0) : HLS_TRANSFORM (::GetSysColor (COLOR_HIGHLIGHT), +50, -66)) : HLS_TRANSFORM (::GetSysColor (COLOR_3DFACE), -27, 0))*/);
							DestroyIcon (hIcon);
						}
						if ( IS_ENABLED(tbbutton) )
						{
							::ImageList_Draw (m_hImageList, tbbutton.iBitmap, memdc,
								rcButton.left + ( (bOver && !bPressed) ? 2 : 3 ), rcButton.top + ( (bOver && !bPressed) ? 2 : 3 ), ILD_TRANSPARENT);
							/*::ImageList_Draw (m_hImageList, tbbutton.iBitmap, cDC.m_hDC,//Enable This to get the Non-shadowed buttons
							rcButton.left + 2, rcButton.top + 2, ILD_TRANSPARENT);*/
						}
						/*if (bPressed )//Enable this to get The Classic Style
						{
						COLORREF crHighLight = ::GetSysColor (COLOR_HIGHLIGHT);
						//cDC.Draw3dRect (rcButton, ::GetSysColor (COLOR_3DSHADOW), ::GetSysColor(COLOR_3DHIGHLIGHT));
						cDC.FillSolidRect(rcButton,HLS_TRANSFORM (crHighLight, +50, -57));
						}*/
					}
				}

				TCHAR szText[256];
				memset(szText, 0, 256);
				//GetString( i, szText, 256);
				GetButtonText( i, szText );
				memdc.DrawText (szText, -1, rcButton, DT_SINGLELINE|DT_LEFT|DT_VCENTER);
			}

	
			memdc.SetBkMode(OldMode);
		}

	};

}; // namespace 