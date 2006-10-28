
#include <atlbase.h>
#include <atlwin.h>

#ifndef Assert
#define Assert ATLASSERT
#endif

// --------------------------------------------------------------------------
// This looks at each item in the Active window's menu and any other menu 
// windows, until it finds one that has an hSubMenu that matches the hMenu 
// we are trying to find. It then returns the ID of that thing (1..n) and
// fills in the window handle of the owner, and whether that window is a top
// level window or a popup menu.
// --------------------------------------------------------------------------
long FindItemIDThatOwnsThisMenu (HMENU hMenuOwned,HWND* phwndOwner,
                                 BOOL* pfPopup,BOOL *pfSysMenu)
{
    GUITHREADINFO	GuiThreadInfo;
    MENUBARINFO     mbi;
    HWND            hwndMenu;
    HMENU           hMenu;
    int             cItems;
    int             i;

    if (IsBadWritePtr(phwndOwner,sizeof(HWND*)) || 
        IsBadWritePtr (pfPopup,sizeof(BOOL*))   ||
        IsBadWritePtr (pfSysMenu,sizeof(BOOL*)))
        return 0;

    *pfPopup = FALSE;
    *pfSysMenu = FALSE;
    *phwndOwner = NULL;

    GetGUIThreadInfo (NULL,&GuiThreadInfo);

    // check if it is from the sys menu first
    GetMenuBarInfo(GuiThreadInfo.hwndActive, OBJID_SYSMENU, 0, &mbi);
    hMenu = mbi.hMenu;

    if (hMenu)
    {
        if (GetSubMenu(hMenu,0) == hMenuOwned)
        {
            *pfSysMenu = TRUE;
            *pfPopup = FALSE;
            *phwndOwner = GuiThreadInfo.hwndActive;
            return (1);
        }
    }

    // if not from the sys menu, check the window's menu bar
    hMenu = GetMenu (GuiThreadInfo.hwndActive);
    if (hMenu)
    {
        cItems = GetMenuItemCount (hMenu);
        for (i=0;i<cItems;i++)
        {
#ifdef _DEBUG
            if (!hMenu)
            {
                //DBPRINTF ("null hmenu at 5\r\n");
                Assert (hMenu);
            }
#endif

            if (GetSubMenu(hMenu,i) == hMenuOwned)
            {
                *pfPopup = FALSE;
                *phwndOwner = GuiThreadInfo.hwndActive;
                return (i+1);
            }
        }
    }
    // Okay, it doesn't belong to the active window's menu bar, maybe
    // it belongs to a submenu of that...
    hwndMenu = FindWindow (TEXT("#32768"),NULL);
    while (hwndMenu)
    {
        hMenu = (HMENU)SendMessage (hwndMenu,MN_GETHMENU,0,0);
        if (hMenu)
        {
            cItems = GetMenuItemCount (hMenu);
            for (i=0;i<cItems;i++)
            {
#ifdef _DEBUG
                if (!hMenu)
                {
                    //DBPRINTF ("null hmenu at 6\r\n");
                    Assert (hMenu);
                }
#endif

                if (GetSubMenu(hMenu,i) == hMenuOwned)
                {
                    *pfPopup = TRUE;
                    *phwndOwner = hwndMenu;
                    return (i+1);
                }
            }
        }
        hwndMenu = FindWindowEx (NULL,hwndMenu,TEXT("#32768"),NULL);
    } // end while hwndMenu

    // if we still haven't returned, then this menu is either a context
    // menu, or belongs to the start button
    return 0;
}
