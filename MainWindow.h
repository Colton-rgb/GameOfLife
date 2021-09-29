#pragma once
#include <string>

class MainWindow : public BaseWindow<MainWindow>
{
    CellGrid cellGrid = CellGrid(24, 24);

    ID2D1Factory          *pFactory;
    ID2D1HwndRenderTarget *pRenderTarget;
    ID2D1SolidColorBrush  *pBrush;
    D2D1_RECT_F          **rectangles;

    void    CalculateLayout();
    HRESULT CreateGraphicsResources();
    void    DiscardGraphicsResources();
    void    OnPaint();
    void    Resize();

    void DrawCellGrid();

public:
    MainWindow() : pFactory(NULL), pRenderTarget(NULL), pBrush(NULL)
    {
        int a = 16;
        wchar_t buffer[256];
        wsprintfW(buffer, L"%d", cellGrid.getLiveNeighbors(0,0));
        MessageBoxW(nullptr, buffer, buffer, MB_OK);
        OutputDebugString(buffer);
    }

    PCWSTR  ClassName() const { return L"Main Window Class"; }
    LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
};