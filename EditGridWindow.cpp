#include <windows.h>
#include <string.h>

#include "EditGridWindow.h"
#include "CellGrid.h"

#define ID_BOK 1

struct EditGridWinInfo
{
    HWND widthField;
    HWND heightField;
};

HWND CreateEditGridWindow(CellGrid cg, HWND parent)
{
    HINSTANCE hInstance = GetModuleHandle(0);
    const wchar_t editClassName[] = L"Input Test Class";

    WNDCLASS inWndClass = { };
    inWndClass.lpfnWndProc = EditGridPrc;
    inWndClass.hInstance = hInstance;
    inWndClass.lpszClassName = editClassName;

    RegisterClass(&inWndClass);

    HWND hEditWind = CreateWindowEx(0, editClassName, L"Edit Grid", WS_OVERLAPPED ^ WS_SYSMENU, 200, 200, 220, 155, 0, 0, hInstance, 0);

    CreateWindowEx(0, L"STATIC", L"Resize Grid", WS_VISIBLE | WS_CHILD | SS_LEFT, 55, 10, 80, 18, hEditWind, 0, hInstance, 0);

    HWND width = CreateWindowEx(0, L"STATIC", L"WIDTH: ", WS_VISIBLE | WS_CHILD | SS_LEFT, 10, 35, 65, 18, hEditWind, 0, hInstance, 0);

    CreateWindowEx(0, L"EDIT", 0, WS_BORDER | WS_CHILD | WS_VISIBLE | ES_LEFT, 75, 35, 100, 18, hEditWind, 0, hInstance, 0);

    HWND height = CreateWindowEx(0, L"STATIC", L"HEIGHT: ", WS_VISIBLE | WS_CHILD | SS_LEFT, 10, 55, 65, 18, hEditWind, 0, hInstance, 0);

    CreateWindowEx(0, L"EDIT", 0, WS_BORDER | WS_CHILD | WS_VISIBLE | ES_LEFT, 75, 55, 100, 18, hEditWind, 0, hInstance, 0);

    EditGridWinInfo* info = new EditGridWinInfo;
    info->heightField = height;
    info->widthField = width;

    HWND hwndButton = CreateWindowEx(0, L"BUTTON", L"OK", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON, 55, 85, 75, 25, hEditWind, (HMENU) ID_BOK, hInstance, &info);

    ShowWindow(hEditWind, 1);

    return hEditWind;
}

LRESULT CALLBACK EditGridPrc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    EditGridWinInfo* pInfo = 0;

    if(uMsg == WM_NCCREATE)
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
        delete pInfo;
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
            SendMessage(hwnd, WM_CLOSE, 0, 0);
        }
    }
    return 0;

    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}