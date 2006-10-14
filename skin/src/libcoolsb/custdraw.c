#include <windows.h>
#include <commctrl.h>
#include "coolscroll.h"
#include "../skinitf.h"

UINT CALLBACK CoolSB_DrawProc(HDC hdc, UINT uCmdId, UINT uButflags, RECT *rect);

extern BOOL fCustomDraw  ;
//extern HDC hdcSkin;

HPEN hpen, oldpen;
HPEN whitepen;
HFONT hfont;
HENHMETAFILE hemf=0;

typedef struct 
{
	int x, y;
	int width, height;
} CustomDrawTable;

//
//	Define a set of structures which describe
//	where-abouts the source "textures" are in the
//	custom draw bitmap. We need to know x, y, width and height
//	for each scrollbar segment.
//

CustomDrawTable cdt_horz_normal[] = 
{
	{ 0,  0,  15, 15 },	//left arrow  NORMAL
	{ 0,  15, 15, 15 }, //right arrow NORMAL
	{ 0,  35, 1,  15 }, //page left   NORMAL
	{ 0,  35, 1,  15 }, //page right  NORMAL
	
	{ -1, -1, -1, -1 },	//padding

	{ 14, 35,  4,  15 }, //horz thumb (left)
	{ 18, 35, 8, 15 }, //horz thumb (middle)
	{ 26, 35, 4, 15 }, //horz thumb (right)
};

CustomDrawTable cdt_horz_hot[] = 
{
	{ 15, 0,  15, 15 }, //left arrow  ACTIVE
	{ 15, 15, 15, 15 }, //right arrow ACTIVE
	{ 6,  35, 1,  15 }, //page left   ACTIVE
	{ 6,  35, 1,  15 }, //page right  ACTIVE

	{ -1, -1, -1, -1 },	//padding

	{ 14, 35,  4,  15 }, //horz thumb (left)
	{ 18, 35, 8, 15 }, //horz thumb (middle)
	{ 26, 35, 4, 15 }, //horz thumb (right)
};

CustomDrawTable cdt_horz_active[] = 
{
	{ 30, 0,  15, 15 }, //left arrow  ACTIVE
	{ 30, 15, 15, 15 }, //right arrow ACTIVE
	{ 4,  35, 1,  15 }, //page left   ACTIVE
	{ 4,  35, 1,  15 }, //page right  ACTIVE

	{ -1, -1, -1, -1 },	//padding

	{ 14, 35,  4,  15 }, //horz thumb (left)
	{ 18, 35, 8, 15 }, //horz thumb (middle)
	{ 26, 35, 4, 15 }, //horz thumb (right)
};


// vertical
CustomDrawTable cdt_vert_normal[] = 
{
	{ 60, 0,  15, 15 }, //up arrow   NORMAL
	{ 60, 15, 15, 15 }, //down arrow NORMAL
	{ 60, 35, 15, 1 }, //page up	 NORMAL
	{ 60, 35, 15, 1 }, //page down  NORMAL

	{ -1, -1, -1, -1 },	//padding

	{ 80, 35,  15, 4  }, //vert thumb (left)
	{ 80, 39,  15, 8  }, //vert thumb (middle)
	{ 80, 47,  15, 4  }, //vert thumb (right)
};

CustomDrawTable cdt_vert_hot[] = 
{
	{ 75, 0,  15, 15 }, //up arrow   ACTIVE
	{ 75, 15, 15, 15 }, //down arrow ACTIVE
	{ 60, 41, 15, 1  }, //page up	 ACTIVE
	{ 60, 41, 15, 1  }, //page down  ACTIVE

	{ -1, -1, -1, -1 },	//padding

	{ 80, 35,  15, 4  }, //vert thumb (left)
	{ 80, 39,  15, 8  }, //vert thumb (middle)
	{ 80, 47,  15, 4  }, //vert thumb (right)
};

CustomDrawTable cdt_vert_active[] = 
{
	{ 90, 0,  15, 15 }, //up arrow   ACTIVE
	{ 90, 15, 15, 15 }, //down arrow ACTIVE
	{ 60, 35, 15, 1  }, //page up	 ACTIVE
	{ 60, 35, 15, 1  }, //page down  ACTIVE

	{ -1, -1, -1, -1 },	//padding

	{ 80, 35,  15, 4  }, //vert thumb (left)
	{ 80, 39,  15, 8  }, //vert thumb (middle)
	{ 80, 47,  15, 4  }, //vert thumb (right)
};

CustomDrawTable cdt_box = { 35, 35, 15, 15 };



LRESULT WINAPI HandleCustomDraw(UINT ctrlid, NMCSBCUSTOMDRAW *nm)
{
	//using namespace Skin;

	RECT *rc;
	CustomDrawTable *cdt;
	UINT code = NM_CUSTOMDRAW;
	int temp = 0;
	int thumblen = 0;

	int nPart;
	int nState;
	CComPtr<ISkinScheme> pss;
	GetCurrentScheme(&pss);

	UNREFERENCED_PARAMETER(ctrlid);

	// inserted buttons do not use PREPAINT etc..
	if(nm->nBar == SB_INSBUT)
	{
		CoolSB_DrawProc(nm->hdc, nm->uItem, nm->uState, &nm->rect);
		return CDRF_SKIPDEFAULT;
	}

	if(!fCustomDraw) return CDRF_DODEFAULT;

	if(nm->dwDrawStage == CDDS_PREPAINT)
	{
		if(fCustomDraw)
			return CDRF_SKIPDEFAULT;
		else
			return CDRF_DODEFAULT;
	}

	if(nm->dwDrawStage == CDDS_POSTPAINT)
	{
		
	}

	//the sizing gripper in the bottom-right corner
	if(nm->nBar == SB_BOTH)	
	{
		RECT *rc = &nm->rect;
		if  ( pss )
		{
			pss->DrawBackground( nm->hdc, SCROLLBAR, SBP_SIZEBOX, SZB_RIGHTALIGN, rc);
		}
		/*
		StretchBlt(nm->hdc, rc->left, rc->top, rc->right-rc->left, rc->bottom-rc->top,
			hdcSkin, cdt_box.x, cdt_box.y, cdt_box.width, cdt_box.height, SRCCOPY);
		*/		
		return CDRF_SKIPDEFAULT;
	}
	else if(nm->nBar == SB_HORZ) //ˮƽ
	{
		rc = &nm->rect;
		if ( nm->uItem == HTSCROLL_LEFT || nm->uItem == HTSCROLL_RIGHT )
		{
			if(nm->uState == CDIS_DISABLED)	
				nState = nm->uItem == HTSCROLL_LEFT ? ABS_LEFTDISABLED : ABS_RIGHTDISABLED ;
			else if(nm->uState == CDIS_HOT)	
				nState = nm->uItem == HTSCROLL_LEFT ? ABS_LEFTHOT : ABS_RIGHTHOT ;
			else if(nm->uState == CDIS_SELECTED) 
				nState = nm->uItem == HTSCROLL_LEFT ? ABS_LEFTPRESSED : ABS_RIGHTPRESSED ;
			else				   
				nState = nm->uItem == HTSCROLL_LEFT ? ABS_LEFTNORMAL : ABS_RIGHTNORMAL ;

			nState = SBP_ARROWBTN;
			
			if  ( pss )
			{
				pss->DrawBackground( nm->hdc, SCROLLBAR, nPart, nState, rc);
			}
			return CDRF_SKIPDEFAULT;

		}
		else if ( nm->uItem == HTSCROLL_THUMB )
		{

			if(nm->uState == CDIS_DISABLED)	
				nState = SCRBS_DISABLED ;
			else if(nm->uState == CDIS_HOT)	
				nState = SCRBS_HOT ;
			else if(nm->uState == CDIS_SELECTED) 
				nState = SCRBS_PRESSED ;
			else				   
				nState = SCRBS_NORMAL ;

			// first draw thumb
			nState = SBP_THUMBBTNHORZ;
			if  ( pss )
			{
				pss->DrawBackground( nm->hdc, SCROLLBAR, nPart, nState, rc);
			}
			// second draw gripper
			
			RECT rcGripper;
			BOOL bRet = pss->GetRect( SCROLLBAR, SBP_GRIPPERHORZ, nState, &rcGripper);
			if ( bRet )
			{
				if ( rc->right - rc->left > rcGripper.right - rcGripper.left + 4 )
				{
					RECT rcClient = *rc;
					rcClient.left = rcClient.left + ( rc->right - rc->left - rcGripper.right + rcGripper.left ) / 2;
					rcClient.right = rcClient.left + rcGripper.right - rcGripper.left;
					if  ( pss )
					{
						pss->DrawBackground( nm->hdc, SCROLLBAR, SBP_GRIPPERHORZ, nState, rc);
					}
					
				}
			}
			return CDRF_SKIPDEFAULT;

		}
		else if ( nm->uItem == HTSCROLL_PAGELEFT )
		{
			nState = SBP_LOWERTRACKHORZ;
			if  ( pss )
			{
				pss->DrawBackground( nm->hdc, SCROLLBAR, nPart, 1, rc);
			}
			return CDRF_SKIPDEFAULT;
		}
		else if ( nm->uItem == HTSCROLL_PAGERIGHT )
		{
			nState = SBP_UPPERTRACKHORZ;
			if  ( pss )
			{
				pss->DrawBackground( nm->hdc, SCROLLBAR, nPart, 1, rc);
			}
			return CDRF_SKIPDEFAULT;
		}

	}
	else if(nm->nBar == SB_VERT)
	{
		rc = &nm->rect;
		if ( nm->uItem == HTSCROLL_UP || nm->uItem == HTSCROLL_DOWN )
		{
			if(nm->uState == CDIS_DISABLED)	
				nState = nm->uItem == HTSCROLL_UP ? ABS_UPDISABLED : ABS_DOWNDISABLED ;
			else if(nm->uState == CDIS_HOT)	
				nState = nm->uItem == HTSCROLL_UP ? ABS_UPHOT : ABS_DOWNHOT ;
			else if(nm->uState == CDIS_SELECTED) 
				nState = nm->uItem == HTSCROLL_UP ? ABS_UPPRESSED : ABS_DOWNRESSED ;
			else				   
				nState = nm->uItem == HTSCROLL_UP ? ABS_UPNORMAL : ABS_DOWNNORMAL ;

			nState = SBP_ARROWBTN;

			if  ( pss )
			{
				pss->DrawBackground( nm->hdc, SCROLLBAR, nPart, nState, rc);
			}
			return CDRF_SKIPDEFAULT;

		}
		else if ( nm->uItem == HTSCROLL_THUMB )
		{

			if(nm->uState == CDIS_DISABLED)	
				nState = SCRBS_DISABLED ;
			else if(nm->uState == CDIS_HOT)	
				nState = SCRBS_HOT ;
			else if(nm->uState == CDIS_SELECTED) 
				nState = SCRBS_PRESSED ;
			else				   
				nState = SCRBS_NORMAL ;

			// first draw thumb
			nState = SBP_THUMBBTNHORZ;
			if  ( pss )
			{
				pss->DrawBackground( nm->hdc, SCROLLBAR, nPart, nState, rc);
			}
			// second draw gripper

			RECT rcGripper;
			BOOL bRet = pss->GetRect( SCROLLBAR, SBP_GRIPPERHORZ, nState, &rcGripper);
			if ( bRet )
			{
				if ( rc->bottom - rc->top > rcGripper.bottom - rcGripper.top + 4 )
				{
					RECT rcClient = *rc;
					rcClient.top = rcClient.top + ( rc->bottom - rc->top - rcGripper.bottom + rcGripper.top ) / 2;
					rcClient.bottom = rcClient.top + rcGripper.bottom - rcGripper.top;
					if  ( pss )
					{
						pss->DrawBackground( nm->hdc, SCROLLBAR, SBP_GRIPPERVERT, nState, rc);
					}

				}
			}
			return CDRF_SKIPDEFAULT;

		}
		else if ( nm->uItem == HTSCROLL_PAGEGUP )
		{
			nState = SBP_LOWERTRACKVERT;
			if  ( pss )
			{
				pss->DrawBackground( nm->hdc, SCROLLBAR, nPart, 1, rc);
			}
			return CDRF_SKIPDEFAULT;
		}
		else if ( nm->uItem == HTSCROLL_PAGEGDOWN )
		{
			nState = SBP_UPPERTRACKVERT;
			if  ( pss )
			{
				pss->DrawBackground( nm->hdc, SCROLLBAR, nPart, 1, rc);
			}
			return CDRF_SKIPDEFAULT;
		}
	}
	//INSERTED BUTTONS are handled here...
	else if(nm->nBar == SB_INSBUT)
	{
		CoolSB_DrawProc(nm->hdc, nm->uItem, nm->uState, &nm->rect);
		return CDRF_SKIPDEFAULT;
	}
	else
	{
		return CDRF_DODEFAULT;
	}

	//normal bitmaps, use same code for HORZ and VERT
	//StretchBlt(nm->hdc, rc->left, rc->top, rc->right-rc->left, rc->bottom-rc->top,
	//	hdcSkin, cdt->x, cdt->y, cdt->width, cdt->height, SRCCOPY);

	return CDRF_SKIPDEFAULT;

}

void DrawTab(HDC hdcEMF, int x, int tabwidth, int tabheight, int xslope, BOOL active)
{
	POINT pts[4];

	pts[0].x = x + 0;
	pts[0].y = 0;
	pts[1].x = x + xslope;
	pts[1].y = tabheight;
	pts[2].x = x + tabwidth - xslope;
	pts[2].y = tabheight;
	pts[3].x = x + tabwidth;
	pts[3].y = 0;

	if(active)
		SelectObject(hdcEMF, GetStockObject(WHITE_BRUSH));
	else
		SelectObject(hdcEMF, GetSysColorBrush(COLOR_3DFACE));

	Polygon(hdcEMF, pts, 4);

	oldpen = SelectObject(hdcEMF, hpen);
	
	MoveToEx(hdcEMF, pts[1].x+1, pts[1].y, 0);
	LineTo(hdcEMF, pts[2].x, pts[2].y);
	
	if(active)
		SelectObject(hdcEMF, whitepen);

	MoveToEx(hdcEMF, pts[3].x - 1, pts[3].y, 0);
	LineTo(hdcEMF, pts[0].x, pts[0].y);

	
	SelectObject(hdcEMF, oldpen);

}

//
//	Draw a series of "tabs" into a meta-file,
//	which we will use to custom-draw one of the inserted
//  scrollbar buttons
//
void InitMetaFile(void)
{
	HDC hdcEMF;
	RECT rect;
	int totalwidth = 120;
	int width = 110, height = GetSystemMetrics(SM_CYHSCROLL);
	LOGFONT lf;

	POINT pts[4];

	int tabwidth = 40, tabxslope = 5;

	pts[0].x = 0;
	pts[0].y = 0;
	pts[1].x = tabxslope;
	pts[1].y = height - 1;
	pts[2].x = tabwidth - tabxslope;
	pts[2].y = height - 1;
	pts[3].x = tabwidth;
	pts[3].y = 0;

	hpen = CreatePen(PS_SOLID,0,GetSysColor(COLOR_3DSHADOW));
	whitepen = CreatePen(PS_INSIDEFRAME,0,RGB(0xff,0xff,0xff));

	SetRect(&rect, 0, 0, totalwidth, height+1);
	
	hdcEMF = CreateEnhMetaFile(NULL, NULL, NULL, NULL);
	
	ZeroMemory(&lf, sizeof(lf));
	lf.lfHeight = -MulDiv(7, GetDeviceCaps(hdcEMF, LOGPIXELSY), 72);
	lf.lfPitchAndFamily = DEFAULT_PITCH;
	lf.lfCharSet = ANSI_CHARSET;
	lstrcpy(lf.lfFaceName, "Arial");//Small fonts");
	hfont = CreateFontIndirect(&lf);

	pts[0].x = 0;
	pts[0].y = 0;
	pts[1].x = tabxslope;
	pts[1].y = height - 1;
	pts[2].x = tabwidth - tabxslope;
	pts[2].y = height - 1;
	pts[3].x = tabwidth;
	pts[3].y = 0;
	
	FillRect  (hdcEMF, &rect, GetSysColorBrush(COLOR_3DFACE));//GetStockObject(WHITE_BRUSH);

	//fit as many lines in as space permits

	SelectObject(hdcEMF, GetSysColorBrush(COLOR_3DFACE));

	DrawTab(hdcEMF, width-tabwidth, tabwidth, height - 1, tabxslope, FALSE);
	DrawTab(hdcEMF, width-tabwidth-tabwidth+tabxslope, tabwidth, height - 1, tabxslope, FALSE);
	DrawTab(hdcEMF, 0, tabwidth, height - 1, tabxslope, TRUE);
	
	SelectObject(hdcEMF, hpen);
	MoveToEx(hdcEMF, 110, 0, 0);
	LineTo(hdcEMF, totalwidth, 0);


	SelectObject(hdcEMF, hfont);
	SetBkMode(hdcEMF, TRANSPARENT);
	TextOut(hdcEMF, 10,1, "Build", 5);

	TextOut(hdcEMF, 42,1, "Debug", 5);
	TextOut(hdcEMF, 78,1, "Result", 6);

	SelectObject(hdcEMF, oldpen);
	DeleteObject(hpen);
	DeleteObject(whitepen);
	hemf  = CloseEnhMetaFile(hdcEMF);
}

//
//	function for drawing the custom-draw inserted buttons
//	Called from the WM_NOTIFY handler (HandleCustomDraw)
//
UINT CALLBACK CoolSB_DrawProc(HDC hdc, UINT uCmdId, UINT uButflags, RECT *rect)
{
	RECT rc;
	POINT pt;
	HPEN hpen, hold;

	HBITMAP hbm, oldbm;
	HDC hdcmem;

	if(hemf == 0)
		InitMetaFile();

	SetRect(&rc, 0, 0, 120, rect->bottom-rect->top);

	hdcmem = CreateCompatibleDC(hdc);
	hbm = CreateCompatibleBitmap(hdc, rc.right, rc.bottom);
	oldbm = SelectObject(hdcmem, hbm);

	SetWindowOrgEx(hdc, -rect->left, -rect->top, &pt);
	PlayEnhMetaFile(hdcmem, hemf, &rc);
	BitBlt(hdc, 0, 0, rc.right, rc.bottom, hdcmem, 0, 0, SRCCOPY);
	
	SetRect(&rc, 120, 0, rect->right-rect->left, rect->bottom-rect->top);
	FillRect(hdc, &rc, GetSysColorBrush(COLOR_3DFACE));
	
	hpen = CreatePen(PS_SOLID, 0, GetSysColor(COLOR_3DSHADOW));
	hold = SelectObject(hdc, hpen);
	MoveToEx(hdc, 120, 0, 0);
	LineTo(hdc, rect->right-rect->left, 0);
	
	SetWindowOrgEx(hdc, pt.x, pt.y, 0);		

	
	SelectObject(hdc, hold);
	SelectObject(hdcmem, oldbm);
	DeleteObject(hbm);
	DeleteDC(hdcmem);
	DeleteObject(hpen);
	
	UNREFERENCED_PARAMETER(uButflags);
	UNREFERENCED_PARAMETER(uCmdId);
	return 0;
}

