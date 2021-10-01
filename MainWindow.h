#pragma once
#include <string>

class MainWindow : public BaseWindow<MainWindow>
{
    CellGrid cellGrid = CellGrid(75, 50);

    D2D1_SIZE_F size = { 0 };
    float cellLength = NULL;
    int init_x = NULL;

    ID2D1Factory          *pFactory = NULL;
    ID2D1HwndRenderTarget *pRenderTarget = NULL;
    ID2D1SolidColorBrush  *pBrush = NULL;
    D2D1_RECT_F          **rectangles = NULL;

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