#pragma once

#include "../base/skinctrl.h"


namespace Skin {

#if 0

/*
 * From HANDTABL.C
 */
/*
 * Static information about each handle type.
 */
typedef void (*FnDestroyUserObject)(void *);

typedef struct tagHANDLETYPEINFO {
#if DBG
    LPCSTR              szObjectType;
    UINT                uSize;
#endif
    FnDestroyUserObject fnDestroy;
    DWORD               dwAllocTag;
    BYTE                bObjectCreateFlags;
} HANDLETYPEINFO, *PHANDLETYPEINFO;

/*
 * The following is the header of all objects managed in the handle list.
 * (allocated as part of the object for easy access).  All object
 * headers must start with the members of a HEAD structure.
 */
typedef struct _HEAD {
    HANDLE h;
    DWORD cLockObj;
} HEAD, *PHEAD;

/*
 * sizeof(THROBJHEAD) must be equal to sizeof(PROCOBJHEAD)
 * This is to make sure that DESKHEAD fields are always at the same offset.
 */
typedef struct _THROBJHEAD {
    HEAD;
    PTHREADINFO pti;
} THROBJHEAD, *PTHROBJHEAD;

typedef struct _PROCOBJHEAD {
    HEAD;
    DWORD hTaskWow;
} PROCOBJHEAD, *PPROCOBJHEAD;

typedef struct _PROCMARKHEAD {
    PROCOBJHEAD;
    struct tagPROCESSINFO *ppi;
} PROCMARKHEAD, *PPROCMARKHEAD;

typedef struct _DESKHEAD {
    PDESKTOP rpdesk;
    PBYTE pSelf;
} DESKHEAD, *PDESKHEAD;

/*
 * This type is for HM casting only. Use THRDESKHEAD or PROCDESKHEAD instead.
 */
typedef struct _DESKOBJHEAD {
    HEAD;
    PVOID pOwner;
    DESKHEAD;
} DESKOBJHEAD, *PDESKOBJHEAD;

typedef struct _THRDESKHEAD {
    THROBJHEAD;
    DESKHEAD;
} THRDESKHEAD, *PTHRDESKHEAD;

typedef struct _PROCDESKHEAD {
    PROCOBJHEAD;
    DESKHEAD;
} PROCDESKHEAD, *PPROCDESKHEAD;



#define HANDLEF_DESTROY        0x01
#define HANDLEF_INDESTROY      0x02
#define HANDLEF_INWAITFORDEATH 0x04
#define HANDLEF_FINALDESTROY   0x08
#define HANDLEF_MARKED_OK      0x10
#define HANDLEF_GRANTED        0x20
#define HANDLEF_VALID          0x3F

/*
 * The following is a handle table entry.
 *
 * Note that by keeping a pointer to the owning entity (process or
 * thread), cleanup will touch only those objects that belong to
 * the entity being destroyed.  This helps keep the working set
 * size down.  Look at DestroyProcessesObjects() for an example.
 */
typedef struct _HANDLEENTRY {
    PHEAD   phead;                  /* pointer to the real object */
    PVOID   pOwner;                 /* pointer to owning entity (pti or ppi) */
    BYTE    bType;                  /* type of object */
    BYTE    bFlags;                 /* flags - like destroy flag */
    WORD    wUniq;                  /* uniqueness count */

#if DBG
    PLR     plr;                    /* lock record pointer */
#endif // DBG

} HANDLEENTRY, *PHE;

typedef struct tagWND {          // wnd
    THRDESKHEAD   head;

    WW;         // WOW-USER common fields. Defined in wowuserp.h
                //  The presence of "state" at the start of this structure is assumed
                //  by the STATEOFFSET macro.

    struct tagWND *spwndNext;    // Handle to the next window
    struct tagWND *spwndParent;  // Backpointer to the parent window.
    struct tagWND *spwndChild;   // Handle to child
    struct tagWND *spwndOwner;   // Popup window owner field

    RECT          rcWindow;     // Window outer rectangle
    RECT          rcClient;     // Client rectangle

    WNDPROC_PWND lpfnWndProc;   // Can be WOW address or standard address

    PCLS          pcls;         // Pointer to window class
    int           cbwndExtra;   // Extra bytes in window

    HRGN          hrgnUpdate;   // Accumulated paint region

    struct tagWND *spwndLastActive; // Last active in owner/ownee list
    PPROPLIST     ppropList;    // Pointer to property list
    PSBINFO        pSBInfo;   // Words used for scrolling

    struct tagMENU *spmenuSys;  // Handle to system menu
    struct tagMENU *spmenu;     // Menu handle or ID
    LARGE_UNICODE_STRING strName;

    ULONG_PTR      dwUserData;   // Reserved for random application data
    HRGN          hrgnClip;     // Clipping region for this window

    HIMC          hImc;         // Associated input context handle

} WND;


/*
 * Menu Structure
 */
typedef struct tagMENU {
    PROCDESKHEAD    head;
    DWORD           fFlags;         /* Menu Flags */
    int             iItem;          /* Contains the position of the selected
                                       item in the menu. -1 if no selection */
    UINT            cAlloced;       // Number of items that can fit in rgItems
    UINT            cItems;         /* Number of items in rgItems */

    DWORD           cxMenu;
    DWORD           cyMenu;
    DWORD           cxTextAlign;    /* Text align offset for popups*/
    struct tagWND *spwndNotify;     /* The owner hwnd of this menu */
    PITEM           rgItems;        /* The list of items in this menu */
    DWORD           dwContextHelpId;// Context help Id for the whole menu
    DWORD           cyMax;          /* max menu height after which menu scrolls */
    DWORD           dwMenuData;     /* app-supplied menu data */

    HBRUSH          hbrBack;        // background brush for menu
    int             iTop;           // Scroll top
    int             iMaxTop;        // Scroll MaxTop
    DWORD           dwArrowsOn:2;   // Scroll flags
} MENU, *PMENU;
#endif

template<class BaseT = CWindow>
class SkinMenu : public SkinControlImpl<SkinMenu, BaseT>
{
public:
    enum { class_id = MENU };
    static LPCTSTR GetWndClassName()
    {
        return _T("#32768");
    }

    typedef SkinMenu<BaseT> this_type;
    typedef SkinControlImpl<SkinMenu, BaseT> base_type;

    BEGIN_MSG_MAP(this_type)
        MSG_WM_NCPAINT(OnNcPaint)
//        MSG_WM_PRINT
//        MSG_WM_PRINTCLIENT
//        MSG_WM_PAINT(OnPaint)
//        MSG_WM_KEYDOWN
//        MSG_WM_NCCALCSIZE
//        MSG_WM_WINDOWPOSCHANGING
//        MSG_WM_ERASEBKGND
//		MESSAGE_HANDLER(WM_DRAWITEM, OnDrawItem)
//		MESSAGE_HANDLER(WM_MEASUREITEM, OnMeasureItem)
        MSG_WM_CREATE(OnCreate)
        MSG_WM_NCCREATE(OnNcCreate)
    END_MSG_MAP()

    // TODO: 得到 HMENU !!!

    LRESULT OnNcCreate(LPCREATESTRUCT lpcs)
    {
        TRACE("nc lpcs->lpCreateParams : %x hmenu: %p, hwndParent: %p\n", lpcs->lpCreateParams, lpcs->hMenu, lpcs->hwndParent);

        if (lpcs->hMenu)
        {
            CMenuHandle mh(lpcs->hMenu);
            int c = mh.GetMenuItemCount();
            TRACE("  count: %d\n", c);
            for (int i=0; i<c; ++i)
            {
                char sz[32] = {0};
                mh.GetMenuString(i, sz, 32, MF_BYPOSITION);
                TRACE("   %d %s\n", i, sz);
            }
        }
        SetMsgHandled(FALSE);
        return 0;
    }

    LRESULT OnCreate(LPCREATESTRUCT lpcs)
    {
        TRACE("lpcs->lpCreateParams : %x hmenu: %p, hwndParent: %p\n", lpcs->lpCreateParams, lpcs->hMenu, lpcs->hwndParent);

        if (lpcs->hMenu)
        {
            CMenuHandle mh(lpcs->hMenu);
            int c = mh.GetMenuItemCount();
            TRACE("  count: %d\n", c);
            for (int i=0; i<c; ++i)
            {
                char sz[32] = {0};
                mh.GetMenuString(i, sz, 32, MF_BYPOSITION);
                TRACE("   %d %s\n", i, sz);
            }
        }
        SetMsgHandled(FALSE);
        return 0;
    }

	LRESULT OnDrawItem(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		return (LRESULT)TRUE;
	}

	LRESULT OnMeasureItem(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		return (LRESULT)TRUE;
	}

    void OnNcPaint(HRGN)
    {
        CRect rcw, rcc;
        GetWindowRect(&rcw);
        GetClientRect(&rcc);
        ClientToScreen(&rcc);
        rcc.OffsetRect(-rcw.left, -rcw.top);
        rcw.OffsetRect(-rcw.left, -rcw.top);

        CWindowDC dc(m_hWnd);

//        CRect rcclip;
//        dc.GetClipBox(&rcclip);

        // memory dc
        CDC dcMem;
        dcMem.CreateCompatibleDC(dc);
        ASSERT( dcMem );
        HBITMAP bmpMemBg = ::CreateCompatibleBitmap(dc, rcw.Width(), rcw.Height());
        ASSERT( bmpMemBg );
        HGDIOBJ pOldBmp = ::SelectObject(dcMem, bmpMemBg);
        ASSERT( pOldBmp );

        // dcMem.FillSolidRect(&rcw, 0x0000ff00);

        CPen pen;
        pen.CreatePen(PS_SOLID, 1, GetSchemeColor(0, 0, TMT_MENU));
        HPEN old_pen = dcMem.SelectPen(pen);
        dcMem.Rectangle(rcw);
        dcMem.SelectPen(old_pen);


        // draw back
        int n = dc.SaveDC();
        dc.ExcludeClipRect(&rcc);

        ::BitBlt(dc, 0, 0, rcw.Width(), rcw.Height(), dcMem, rcw.left, rcw.top, SRCCOPY);
        
        dc.RestoreDC(n);

        // memory dc
        ::SelectObject(dcMem, pOldBmp);
        ::DeleteObject(bmpMemBg);
        ::DeleteDC(dcMem);
    }

    void OnPaint(HDC)
    {
        //CPaintDC dc(m_hWnd);
        // warning C4312: “类型转换” : 从“LONG”转换到更大的“HMENU”
        #pragma warning(disable: 4312)
        HMENU h = (HMENU)GetWindowLong(0);
        #pragma warning(default: 4312)
        TRACE("longptr: %p m_hWnd:%p owner:%p\n", h, m_hWnd, ::GetWindow(m_hWnd, GW_OWNER));
		DefWindowProc();
    }
};

}; // namespace 