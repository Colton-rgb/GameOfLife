#pragma once
#include <string>

class MainWindow : public BaseWindow<MainWindow>
{
    CellGrid cellGrid = CellGrid(150, 75);

    D2D1_SIZE_F size;
    float cellLength;
    int init_x;

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
    MainWindow() : pFactory(NULL), pRenderTarget(NULL), pBrush(NULL) { }

    PCWSTR  ClassName() const { return L"Main Window Class"; }
    LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
};