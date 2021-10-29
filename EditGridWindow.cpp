#include <windows.h>
#include <string.h>

#include <d2d1.h>
#include "EditGridWindow.h"
#include "BaseWindow.h"
#include "GridWindow.h"

// ID of the common controls
#define ID_BOK 1
#define ID_EWIDTH 2
#define ID_EHEIGHT 3

// Window state data
struct EditGridWinInfo
{
    HWND parent;
    HWND widthField;
    HWND heightField;
    CellGrid* pCellGrid;

    EditGridWinInfo()
    {
        parent = 0;
        widthField = 0;
        heightField = 0;
        pCellGrid = 0;
    }
};

HWND CreateEditGridWindow(CellGrid *pCellgrid, HWND parent)
{

    // GetModuleHandle(0) returns a handle to the instance that called the function
    HINSTANCE hInstance = GetModuleHandle(0);

    // Create a new window class for editing the cell grid
    const wchar_t editClassName[] = L"Edit Grid Class";

    // Fills out the WNDCLASS structure
    WNDCLASS inWndClass = { 0 };
    inWndClass.lpfnWndProc = EditGridPrc;
    inWndClass.hInstance = hInstance;
    inWndClass.lpszClassName = editClassName;

    RegisterClass(&inWndClass);

    // Create a pointer to State applications info on the heap that can be accessed later
    EditGridWinInfo* info = new (std::nothrow) EditGridWinInfo;
    if (info == NULL) return 0;

    // Passing info to the CreateWindow function allow for later use of the pointer for managing application state
    HWND hEditWnd = CreateWindowEx(0, editClassName, L"Edit Grid", WS_OVERLAPPED ^ WS_SYSMENU, 200, 200, 220, 155, 0, 0, hInstance, info);

    // Create all the common contorls and lay them out in the window
    CreateWindowEx(0, L"STATIC", L"Resize Grid", WS_VISIBLE | WS_CHILD | SS_LEFT, 55, 10, 80, 18, hEditWnd, 0, hInstance, 0);
    CreateWindowEx(0, L"STATIC", L"WIDTH: ", WS_VISIBLE | WS_CHILD | SS_LEFT, 10, 35, 65, 18, hEditWnd, 0, hInstance, 0);
    CreateWindowEx(0, L"STATIC", L"HEIGHT: ", WS_VISIBLE | WS_CHILD | SS_LEFT, 10, 55, 65, 18, hEditWnd, 0, hInstance, 0);

    HWND hWidthField = CreateWindowEx(0, L"EDIT", 0, WS_BORDER | WS_CHILD | WS_VISIBLE | ES_LEFT | ES_NUMBER, 75, 35, 100, 18, hEditWnd, (HMENU) ID_EWIDTH, hInstance, 0);
    HWND hHeightField = CreateWindowEx(0, L"EDIT", 0, WS_BORDER | WS_CHILD | WS_VISIBLE | ES_LEFT | ES_NUMBER, 75, 55, 100, 18, hEditWnd, (HMENU) ID_EHEIGHT, hInstance, 0);

    CreateWindowEx(0, L"BUTTON", L"OK", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON, 55, 85, 75, 25, hEditWnd, (HMENU)ID_BOK, hInstance, 0);

    // Fill out the info data structure
    info->parent = parent;
    info->heightField = hHeightField;
    info->widthField = hHeightField;
    info->pCellGrid = pCellgrid;

    ShowWindow(hEditWnd, 1);

    return hEditWnd;
}

LRESULT CALLBACK EditGridPrc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{

    EditGridWinInfo* pInfo = 0;

    if(uMsg == WM_CREATE)
    {
        // Extract the application state data from the pointer that was passed when the window was created
        CREATESTRUCT* pCreateStruct = (CREATESTRUCT*)lParam;
        pInfo = (EditGridWinInfo*)pCreateStruct->lpCreateParams;

        // Set window long pointer to the info so it can be easily grabbed later
        SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)pInfo);
    }
    else
    {
        // If the window was already created, grab the pointer from the window long pointer
        pInfo = (EditGridWinInfo*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
    }

    switch (uMsg)
    {
    case WM_DESTROY:
    {
        delete pInfo;
    } return 0;

    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);
        FillRect(hdc, &ps.rcPaint, (HBRUSH)(COLOR_WINDOW + 1));
        EndPaint(hwnd, &ps);
    }
    case WM_COMMAND:
    {
        switch (LOWORD(wParam))
        {
        case ID_BOK:

            // Retrieve the data from the input fields when the OK button is pressed
            int inputWidth = 0;
            BOOL bTranslateWidth = TRUE;
            inputWidth = GetDlgItemInt(hwnd, ID_EWIDTH, &bTranslateWidth, false);
            
            int inputHeight = 0;
            BOOL bTranslateHeight = 0;
            inputHeight = GetDlgItemInt(hwnd, ID_EHEIGHT, &bTranslateHeight, false);

            // Check if it was able to retrieve the integers
            if (bTranslateWidth == FALSE || bTranslateHeight == FALSE)
            {
                MessageBox(hwnd, L"Input values must be integers above 0.", L"Error", MB_ICONEXCLAMATION);
            }
            else
            {
                // Resize the cellGrid and get old width to pass to resize rectangles
                int oldWidth = pInfo->pCellGrid->width;
                pInfo->pCellGrid->generate(inputWidth, inputHeight);
                
                // Get a pointer to the CellGrid class in order to call its methods
                // This is a blind cast but I don't care because I know the pointer will always be a pointer to the CellGrid window
                GridWindow* pParent = (GridWindow*)GetWindowLongPtr(pInfo->parent, GWLP_USERDATA);

                // Resize the rectangles to fit the new cell grid
                pParent->ResizeRectangles(inputWidth, inputHeight, oldWidth);

                // Repaint the parent window
                SendMessage(pInfo->parent, WM_SIZE, 0, 0);
            }

            // Close the current EditGridWindow
            SendMessage(hwnd, WM_CLOSE, 0, 0);
        }
    }
    return 0;
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}