#include <windows.h>

#include "CellGrid.h"

HWND CreateEditGridWindow(CellGrid cg)
{
    const wchar_t editClassName[] = L"Input Test Class";

    WNDCLASS inWndClass = { };
    inWndClass.lpfnWndProc = EditGridPrc;
    inWndClass.hInstance = GetModuleHandle(0);
    inWndClass.lpszClassName = editClassName;

    RegisterClass(&inWndClass);

    HWND hEditWind = CreateWindowEx(0, editClassName, L"Edit Grid", WS_OVERLAPPED ^ WS_SYSMENU, 200, 200, 230, 125, 0, 0, hInstance, 0);

    CreateWindowEx(0, L"STATIC", L"Resize Grid", WS_VISIBLE | WS_CHILD | SS_LEFT, 10, 10, 80, 18, hEditWind, 0, hInstance, 0);

    CreateWindowEx(0, L"STATIC", L"WIDTH: ", WS_VISIBLE | WS_CHILD | SS_LEFT, 10, 35, 65, 18, hEditWind, 0, hInstance, 0);

    CreateWindowEx(0, L"EDIT", 0, WS_BORDER | WS_CHILD | WS_VISIBLE | ES_LEFT, 75, 35, 100, 18, hEditWind, 0, hInstance, 0);

    CreateWindowEx(0, L"STATIC", L"HEIGHT: ", WS_VISIBLE | WS_CHILD | SS_LEFT, 10, 55, 65, 18, hEditWind, 0, hInstance, 0);

    CreateWindowEx(0, L"EDIT", 0, WS_BORDER | WS_CHILD | WS_VISIBLE | ES_LEFT, 75, 55, 100, 18, hEditWind, 0, hInstance, 0);

    ShowWindow(hEditWind, 1);
}

LRESULT CALLBACK EditGridPrc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_DESTROY:
        return 0;

    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);



        FillRect(hdc, &ps.rcPaint, (HBRUSH)(COLOR_WINDOW + 1));

        EndPaint(hwnd, &ps);
    }
    return 0;

    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}