/*===================================================*\
|                                                     |
|  EASY-SIZE Macros                                   |
|                                                     |
|  Copyright (c) 2001 - Marc Richarme                 |
|      devix@devix.cjb.net                            |
|      http://devix.cjb.net                           |
|                                                     |
|  License:                                           |
|                                                     |
|  You may use this code in any commersial or non-    |
|  commersial application, and you may redistribute   |
|  this file (and even modify it if you wish) as      |
|  long as you keep this notice untouched in any      |
|  version you redistribute.                          |
|                                                     |
|  Usage:                                             |
|                                                     |
|  - Insert 'DECLARE_EASYSIZE' somewhere in your      |
|    class declaration                                |
|  - Insert an easysize map in the beginning of your  |
|    class implementation (see documentation) and     |
|    outside of any function.                         |
|  - Insert 'INIT_EASYSIZE;' in your                  |
|    OnInitDialog handler.                            |
|  - Insert 'UPDATE_EASYSIZE' in your OnSize handler  |
|  - Optional: Insert 'EASYSIZE_MINSIZE(mx,my);' in   |
|    your OnSizing handler if you want to specify     |
|    a minimum size for your dialog                   |
|                                                     |
|        Check http://devix.cjb.net for the           |
|              docs and new versions                  |
|                                                     |
|  Last modification :								  |
|  14/03/2003 by Ali (ayachali@yahoo.fr)			  |
|  the control options are stored in an array insted  |
|  of duplicating the body of the RepositionControls  |
|  function, that reduce too mutch the code inserted  |
|  by the macros. the size of the RepositionControls  |
|  function is always the same !					  |
\*===================================================*/


#ifndef __EASYSIZE_H_
#define __EASYSIZE_H_
#define ES_BORDER 0xffffffff
#define ES_KEEPSIZE 0xfffffffe
#define ES_HCENTER 0x00000001
#define ES_VCENTER 0x00000002

#define DECLARE_EASYSIZE \
private:\
struct _ctrlmap{UINT id, l, t, r, b, o, es_l, es_t, es_r, es_b;};\
static _ctrlmap ctrlmap[];\
void __ES__RepositionControls(BOOL bInit);\
void __ES__CalcBottomRight(CWnd *pThis, BOOL bBottom, int &bottomright, int &topleft, UINT id, UINT br, int es_br, CRect &rect, int clientbottomright);\
// #define DECLARE_EASYSIZE

#define INIT_EASYSIZE __ES__RepositionControls(TRUE); __ES__RepositionControls(FALSE);
#define UPDATE_EASYSIZE if(GetWindow(GW_CHILD)!=NULL) __ES__RepositionControls(FALSE);

#define EASYSIZE_MINSIZE(mx,my,s,r)\
{\
	if(r->right-r->left < mx)\
	{\
		if((s == WMSZ_BOTTOMLEFT)||(s == WMSZ_LEFT)||(s == WMSZ_TOPLEFT))\
			r->left = r->right-mx;\
		else r->right = r->left+mx;\
	}\
	if(r->bottom-r->top < my)\
	{\
		if((s == WMSZ_TOP)||(s == WMSZ_TOPLEFT)||(s == WMSZ_TOPRIGHT))\
			r->top = r->bottom-my;\
		else r->bottom = r->top+my;\
	}\
} // #define EASYSIZE_MINSIZE(mx,my,s,r)

#define BEGIN_EASYSIZE_MAP(theClass)\
void theClass::__ES__CalcBottomRight(CWnd *pThis, BOOL bBottom, int &bottomright, int &topleft, UINT id, UINT br, int es_br, CRect &rect, int clientbottomright)\
{\
	if(br==ES_BORDER)\
		bottomright = clientbottomright-es_br;\
	else\
	{\
		if(br==ES_KEEPSIZE)\
			bottomright = topleft+es_br;\
		else\
		{\
			CRect rect2;\
			pThis->GetDlgItem(br)->GetWindowRect(rect2);\
			pThis->ScreenToClient(rect2);\
			bottomright = (bBottom?rect2.top:rect2.left) - es_br;\
		}\
	}\
}\
void theClass::__ES__RepositionControls(BOOL bInit)\
{\
	CRect rect,rect2,client;\
	GetClientRect(client);\
	int i = 0;\
	CWnd* pWnd = NULL;\
	while(ctrlmap[i].id)\
	{\
		pWnd = GetDlgItem(ctrlmap[i].id);\
		if(pWnd)\
		{\
			if(bInit)\
			{\
				pWnd->GetWindowRect(rect);\
				ScreenToClient(rect);\
				if(ctrlmap[i].o & ES_HCENTER)\
					ctrlmap[i].es_l = rect.Width()/2;\
				else\
				{\
					if(ctrlmap[i].l==ES_BORDER)\
						ctrlmap[i].es_l = rect.left;\
					else if(ctrlmap[i].l==ES_KEEPSIZE)\
						ctrlmap[i].es_l = rect.Width();\
					else\
					{\
						GetDlgItem(ctrlmap[i].l)->GetWindowRect(rect2);\
						ScreenToClient(rect2);\
						ctrlmap[i].es_l = rect.left-rect2.right;\
					}\
				}\
				if(ctrlmap[i].o & ES_VCENTER)\
					ctrlmap[i].es_t = rect.Height()/2;\
				else\
				{\
					if(ctrlmap[i].t==ES_BORDER)\
						ctrlmap[i].es_t = rect.top;\
					else if(ctrlmap[i].t==ES_KEEPSIZE)\
						ctrlmap[i].es_t = rect.Height();\
					else\
					{\
						GetDlgItem(ctrlmap[i].t)->GetWindowRect(rect2);\
						ScreenToClient(rect2);\
						ctrlmap[i].es_t = rect.top-rect2.bottom;\
					}\
				}\
				if(ctrlmap[i].o & ES_HCENTER)\
					ctrlmap[i].es_r = rect.Width();\
				else\
				{\
					if(ctrlmap[i].r==ES_BORDER)\
						ctrlmap[i].es_r = client.right-rect.right;\
					else if(ctrlmap[i].r==ES_KEEPSIZE)\
						ctrlmap[i].es_r = rect.Width();\
					else\
					{\
						GetDlgItem(ctrlmap[i].r)->GetWindowRect(rect2);\
						ScreenToClient(rect2);\
						ctrlmap[i].es_r = rect2.left-rect.right;\
					}\
				}\
				if(ctrlmap[i].o & ES_VCENTER)\
					ctrlmap[i].es_b = rect.Height();\
				else\
				{\
					if(ctrlmap[i].b==ES_BORDER)\
						ctrlmap[i].es_b = client.bottom-rect.bottom;\
					else if(ctrlmap[i].b==ES_KEEPSIZE)\
						ctrlmap[i].es_b = rect.Height();\
					else\
					{\
						GetDlgItem(ctrlmap[i].b)->GetWindowRect(rect2);\
						ScreenToClient(rect2);\
						ctrlmap[i].es_b = rect2.top-rect.bottom;\
					}\
				}\
			}\
			else\
			{\
				int left,top,right,bottom;\
				BOOL bR = FALSE,bB = FALSE;\
				if(ctrlmap[i].o & ES_HCENTER)\
				{\
					int _a,_b;\
					if(ctrlmap[i].l==ES_BORDER)\
						_a = client.left;\
					else\
					{\
						GetDlgItem(ctrlmap[i].l)->GetWindowRect(rect2);\
						ScreenToClient(rect2);\
						_a = rect2.right;\
					}\
					if(ctrlmap[i].r==ES_BORDER)\
						_b = client.right;\
					else\
					{\
						GetDlgItem(ctrlmap[i].r)->GetWindowRect(rect2);\
						ScreenToClient(rect2);\
						_b = rect2.left;\
					}\
					left = _a+((_b-_a)/2-ctrlmap[i].es_l);\
					right = left + ctrlmap[i].es_r;\
				}\
				else\
				{\
					if(ctrlmap[i].l==ES_BORDER)\
						left = ctrlmap[i].es_l;\
					else if(ctrlmap[i].l==ES_KEEPSIZE)\
					{\
						__ES__CalcBottomRight(this,FALSE,right,left,ctrlmap[i].id,ctrlmap[i].r,ctrlmap[i].es_r,rect,client.right);\
						left = right-ctrlmap[i].es_l;\
					}\
					else\
					{\
						GetDlgItem(ctrlmap[i].l)->GetWindowRect(rect2);\
						ScreenToClient(rect2);\
						left = rect2.right + ctrlmap[i].es_l;\
					}\
					if(ctrlmap[i].l != ES_KEEPSIZE)\
						__ES__CalcBottomRight(this,FALSE,right,left,ctrlmap[i].id,ctrlmap[i].r,ctrlmap[i].es_r,rect,client.right);\
				}\
				if(ctrlmap[i].o & ES_VCENTER)\
				{\
					int _a,_b;\
					if(ctrlmap[i].t==ES_BORDER)\
						_a = client.top;\
					else\
					{\
						GetDlgItem(ctrlmap[i].t)->GetWindowRect(rect2);\
						ScreenToClient(rect2);\
						_a = rect2.bottom;\
					}\
					if(ctrlmap[i].b==ES_BORDER)\
						_b = client.bottom;\
					else\
					{\
						GetDlgItem(ctrlmap[i].b)->GetWindowRect(rect2);\
						ScreenToClient(rect2);\
						_b = rect2.top;\
					}\
					top = _a+((_b-_a)/2-ctrlmap[i].es_t);\
					bottom = top + ctrlmap[i].es_b;\
				}\
				else\
				{\
					if(ctrlmap[i].t==ES_BORDER)\
						top = ctrlmap[i].es_t;\
					else if(ctrlmap[i].t==ES_KEEPSIZE)\
					{ \
						__ES__CalcBottomRight(this,TRUE,bottom,top,ctrlmap[i].id,ctrlmap[i].b,ctrlmap[i].es_b,rect,client.bottom);\
						top = bottom-ctrlmap[i].es_t;\
					}\
					else\
					{\
						GetDlgItem(ctrlmap[i].t)->GetWindowRect(rect2);\
						ScreenToClient(rect2);\
						top = rect2.bottom + ctrlmap[i].es_t;\
					}\
					if(ctrlmap[i].t != ES_KEEPSIZE)\
						__ES__CalcBottomRight(this,TRUE,bottom,top,ctrlmap[i].id,ctrlmap[i].b,ctrlmap[i].es_b,rect,client.bottom);\
				}\
				pWnd->MoveWindow(left,top,right-left,bottom-top,FALSE);\
			}\
		}\
		i++;\
	}\
	Invalidate();\
	UpdateWindow();\
}\
theClass::_ctrlmap theClass::ctrlmap[] = \
{ //#define BEGIN_EASYSIZE_MAP

#define EASYSIZE(id,l,t,r,b,o)	id,l,t,r,b,o,0,0,0,0,

#define END_EASYSIZE_MAP \
	0,0,0,0,0,0,0,0,0,0 \
}; //#define END_EASYSIZE_MAP

#endif //__EASYSIZE_H_