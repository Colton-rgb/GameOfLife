#include <windows.h>
#include <string.h>

#include <d2d1.h>
#include "EditGridWindow.h"
#include "BaseWindow.h"
#include "GridWindow.h"

#define ID_BOK 1
#define ID_EWIDTH 2
#define ID_EHEIGHT 3

struct EditGridWinInfo
{
    HWND parent;
    HWND widthField;
    HWND heightField;
    CellGrid* pCellGrid;
};

HWND CreateEditGridWindow(CellGrid *pCellgrid, HWND parent)
{
    HINSTANCE hInstance = GetModuleHandle(0);
    const wchar_t editClassName[] = L"Input Test Class";

    WNDCLASS inWndClass = { };
    inWndClass.lpfnWndProc = EditGridPrc;
    inWndClass.hInstance = hInstance;
    inWndClass.lpszClassName = editClassName;

    RegisterClass(&inWndClass);

    EditGridWinInfo* info = new (std::nothrow) EditGridWinInfo;

    if (info == NULL)
    {
        return 0;
    }


    HWND hEditWind = CreateWindowEx(0, editClassName, L"Edit Grid", WS_OVERLAPPED ^ WS_SYSMENU, 200, 200, 220, 155, 0, 0, hInstance, info);

    CreateWindowEx(0, L"STATIC", L"Resize Grid", WS_VISIBLE | WS_CHILD | SS_LEFT, 55, 10, 80, 18, hEditWind, 0, hInstance, 0);

    CreateWindowEx(0, L"STATIC", L"WIDTH: ", WS_VISIBLE | WS_CHILD | SS_LEFT, 10, 35, 65, 18, hEditWind, 0, hInstance, 0);

    HWND hWidthField = CreateWindowEx(0, L"EDIT", 0, WS_BORDER | WS_CHILD | WS_VISIBLE | ES_LEFT | ES_NUMBER, 75, 35, 100, 18, hEditWind, (HMENU) ID_EWIDTH, hInstance, 0);

    CreateWindowEx(0, L"STATIC", L"HEIGHT: ", WS_VISIBLE | WS_CHILD | SS_LEFT, 10, 55, 65, 18, hEditWind, 0, hInstance, 0);

    HWND hHeightField = CreateWindowEx(0, L"EDIT", 0, WS_BORDER | WS_CHILD | WS_VISIBLE | ES_LEFT | ES_NUMBER, 75, 55, 100, 18, hEditWind, (HMENU) ID_EHEIGHT, hInstance, 0);

    info->parent = parent;
    info->heightField = hHeightField;
    info->widthField = hHeightField;
    info->pCellGrid = pCellgrid;

    HWND hwndButton = CreateWindowEx(0, L"BUTTON", L"OK", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON, 55, 85, 75, 25, hEditWind, (HMENU) ID_BOK, hInstance, 0);

    ShowWindow(hEditWind, 1);

    return hEditWind;
}

LRESULT CALLBACK EditGridPrc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    EditGridWinInfo* pInfo = 0;

    if(uMsg == WM_CREATE)
    {
        CREATESTRUCT* pCreateStruct = (CREATESTRUCT*)lParam;
        pInfo = (EditGridWinInfo*)pCreateStruct->lpCreateParams;
        SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)pInfo);
    }
    else
    {
        pInfo = (EditGridWinInfo*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
    }

    switch (uMsg)
    {
    case WM_DESTROY:
        //delete pInfo;
        return 0;
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);
        FillRect(hdc, &ps.rcPaint, (HBRUSH)(COLOR_WINDOW + 1));

        EndPaint(hwnd, &ps);
    }
    case WM_COMMAND:
    {
        //SendMessage(hwnd, WM_CLOSE, 0, 0);
        switch (LOWORD(wParam))
        {
        case ID_BOK:
            /*LPWSTR buffer = new wchar_t[GetWindowTextLength(pInfo->heightField)];
            GetDlgItemText(hwnd, ID_EHEIGHT, buffer, GetWindowTextLength(pInfo->heightField)+1);
            MessageBox(hwnd, buffer, L"test", 0);*/

            int inputWidth = 0;
            BOOL bTranslateWidth = TRUE;
            inputWidth = GetDlgItemInt(hwnd, ID_EWIDTH, &bTranslateWidth, false);
            
            int inputHeight = 0;
            BOOL bTranslateHeight = 0;
            inputHeight = GetDlgItemInt(hwnd, ID_EHEIGHT, &bTranslateHeight, false);

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
                GridWindow* pParent = (GridWindow*)GetWindowLongPtr(pInfo->parent, GWLP_USERDATA);

                // Resize the rectangles to fit the new cell grid
                pParent->ResizeRectangles(inputWidth, inputHeight, oldWidth);

                // Repaint the parent window
                SendMessage(pInfo->parent, WM_SIZE, 0, 0);
            }

            SendMessage(hwnd, WM_CLOSE, 0, 0);
        }
    }
    return 0;

    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}