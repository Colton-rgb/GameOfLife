#include <windows.h>
#include <CommCtrl.h>

#include "Toolbar.h"

HIMAGELIST g_hImageList = NULL;
HINSTANCE g_hInst = NULL;

HWND CreateToolbar(HWND hWndParent)
{
    g_hInst = GetModuleHandle(NULL);

    // Declare and initialize local constants.
    const int ImageListID = 0;
    const int numButtons = 5;
    const int bitmapSize = 16;

    const DWORD buttonStyles = BTNS_AUTOSIZE;

    // Create the toolbar.
    HWND hWndToolbar = CreateWindowEx(0, TOOLBARCLASSNAME, NULL, WS_CHILD | TBSTYLE_WRAPABLE, 0, 0, 0, 0, hWndParent, NULL, g_hInst, NULL);

    if (hWndToolbar == NULL)
    {
        return NULL;
    }

    // Create the image list.
    // Dimensions of individual bitmaps. // Ensures transparent background.
    g_hImageList = ImageList_Create(bitmapSize, bitmapSize, ILC_COLOR16 | ILC_MASK, numButtons, 0);

    // Set the image list.
    SendMessage(hWndToolbar, TB_SETIMAGELIST, (WPARAM)ImageListID, (LPARAM)g_hImageList);

    // Load the button images.
    SendMessage(hWndToolbar, TB_LOADIMAGES, (WPARAM)IDB_STD_SMALL_COLOR, (LPARAM)HINST_COMMCTRL);

    // Initialize button info.
    // IDM_NEW, IDM_OPEN, and IDM_SAVE are application-defined command constants.

    TBBUTTON tbButtons[numButtons] =
    {
        { MAKELONG(STD_REDOW, ImageListID),  ID_TBRUN,  TBSTATE_ENABLED, buttonStyles, {0}, 0, (INT_PTR)L"Run" },
        { MAKELONG(STD_PRINTPRE, ImageListID),  ID_TBGRID, TBSTATE_ENABLED, buttonStyles, {0}, 0, (INT_PTR)L"Grid"},
        { MAKELONG(STD_PROPERTIES, ImageListID), ID_TBEDIT, TBSTATE_ENABLED, buttonStyles, {0}, 0, (INT_PTR)L"Edit"},
        { MAKELONG(STD_FILEOPEN, ImageListID), ID_TBOPEN, TBSTATE_ENABLED, buttonStyles, {0}, 0, (INT_PTR)L"Open"},
        { MAKELONG(STD_FILESAVE, ImageListID), ID_TBSAVE, TBSTATE_ENABLED, buttonStyles, {0}, 0, (INT_PTR)L"Save"}
    };

    // Add buttons.
    SendMessage(hWndToolbar, TB_BUTTONSTRUCTSIZE, (WPARAM)sizeof(TBBUTTON), 0);
    SendMessage(hWndToolbar, TB_ADDBUTTONS, (WPARAM)numButtons, (LPARAM)&tbButtons);

    // Resize the toolbar, and then show it.
    SendMessage(hWndToolbar, TB_AUTOSIZE, 0, 0);
    ShowWindow(hWndToolbar, TRUE);

    return hWndToolbar;
}