#pragma once

class MainWindow : public BaseWindow<MainWindow>
{
    HCURSOR hCursor = LoadCursor(NULL, IDC_ARROW);
    bool running = false;
    bool drawGrid = true;

    // CellGird Details
    CellGrid cellGrid = CellGrid(75, 50);
    D2D1_SIZE_F size = { 0 };
    float cellLength = NULL;
    float init_x = NULL;

    // Direct2D
    ID2D1Factory *pFactory = NULL;
    ID2D1HwndRenderTarget *pRenderTarget = NULL;
    ID2D1SolidColorBrush *pBrush = NULL;

    // Shapes
    D2D1_RECT_F **rectangles = NULL;
    D2D1_RECT_F rect_left = { 0 };
    D2D1_RECT_F rect_right = { 0 };

    void CalculateLayout();
    HRESULT CreateGraphicsResources();
    void DiscardGraphicsResources();
    void OnPaint();
    void Resize();
    void DrawCellGrid();

public:
    MainWindow() : pFactory(NULL), pRenderTarget(NULL), pBrush(NULL) { }
    PCWSTR  ClassName() const { return L"Main Window Class"; }
    LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
};