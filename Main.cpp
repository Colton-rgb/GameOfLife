#ifndef UNICODE
#define UNICODE
#endif 

#include <windows.h>
#include <d2d1.h>
#include <Shobjidl.h>

#pragma comment(lib, "d2d1")
#pragma comment(lib, "Comctl32")

#include "BaseWindow.h"
#include "CellGrid.h"
#include "GridWindow.h"

LRESULT CALLBACK testWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
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

// Must be using windows subsystem
int WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE, _In_ PWSTR pCmdLine, _In_ int nCmdShow)
{

    // COM
    if (CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE) != 0)
    {
        return 0;
    }

    GridWindow win;

    if (!win.Create(L"Game of Life", WS_OVERLAPPEDWINDOW| WS_CLIPCHILDREN))
    {
        return 0;
    }

    ShowWindow(win.Window(), nCmdShow);

    const wchar_t editClassName[] = L"Input Test Class";

    WNDCLASS inWndClass = { };
    inWndClass.lpfnWndProc = testWndProc;
    inWndClass.hInstance = hInstance;
    inWndClass.lpszClassName = editClassName;

    RegisterClass(&inWndClass);

    HWND hEditWind = CreateWindowEx(0, editClassName, L"Edit Window", WS_OVERLAPPEDWINDOW ^ WS_THICKFRAME, CW_USEDEFAULT, CW_USEDEFAULT, 300, 200, win.Window(), 0, hInstance, 0);
    CreateWindow(L"EDIT", 0, WS_BORDER | WS_CHILD | WS_VISIBLE, 56, 100, 50, 18, hEditWind, 0, hInstance, 0);

    ShowWindow(hEditWind, nCmdShow);

    // Run the message loop
    MSG msg = { };
    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    CoUninitialize();
    return 0;
}

