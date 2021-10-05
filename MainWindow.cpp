#include <windows.h>
#include <d2d1.h>
#include <WindowsX.h>

#include "CellGrid.h"
#include "BaseWindow.h"
#include "MainWindow.h"
#include "resource.h"

#define TIMER_ID 1

template <class T> void SafeRelease(T** ppT)
{
    if (*ppT)
    {
        (*ppT)->Release();
        *ppT = NULL;
    }
}

LRESULT MainWindow::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_CREATE:
    {
        if (FAILED(D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &pFactory)))
        {
            return -1;
        }

        SetCursor(hCursor);

        // TIMER
        SetTimer(m_hwnd, TIMER_ID, 100, (TIMERPROC)NULL);

        return 0;
    }
    case WM_SETCURSOR:
        if (LOWORD(lParam) == HTCLIENT)
        {
            SetCursor(hCursor);
        }
        return 0;
    case WM_DESTROY:
        KillTimer(m_hwnd, TIMER_ID);
        DiscardGraphicsResources();
        SafeRelease(&pFactory);
        PostQuitMessage(0);
        return 0;

    case WM_LBUTTONDOWN: {
        if (running == false)
        {
            int xPos = GET_X_LPARAM(lParam);
            int yPos = GET_Y_LPARAM(lParam);

            int gridX = (int)((xPos - (int)init_x) / cellLength);
            int gridY = (int)(yPos / cellLength);

            // Check bounds
            if (gridX < 0 || gridX > cellGrid.width || gridY < 0 || gridY > cellGrid.height) return 0;

            // apply change to grid
            cellGrid.cells[gridX][gridY] = !cellGrid.cells[gridX][gridY];
        }
        return 0;
    }

    case WM_KEYDOWN:
    {
        if (GetAsyncKeyState(0x52)) // R key
        {
            cellGrid.randomize();
            return 0;
        }
        if (GetAsyncKeyState(0x47)) // G key
        {
            drawGrid = !drawGrid;
            return 0;
        }
        if (GetAsyncKeyState(0x43)) // C key
        {
            cellGrid.clear();
            return 0;
        }
        if (GetAsyncKeyState(VK_ESCAPE))
        {
            SendMessage(m_hwnd, WM_CLOSE, NULL, NULL);
            return 0;
        }
        if (GetAsyncKeyState(VK_RIGHT))
        {
            cellGrid.update();
            OnPaint();
            return 0;
        }
        if (GetKeyState(VK_SPACE))
        {
            running = !running;
            return 0;
        }
        return 0;
    }

    case WM_TIMER:
        switch (wParam)
        {
        case TIMER_ID:
            if (running)
            {
                cellGrid.update();
            }
        }

    case WM_PAINT:
        OnPaint();
        return 0;

    case WM_SIZE:
        Resize();
        return 0;

    case WM_COMMAND:
        switch (LOWORD(wParam))
        {
        case ID_FILE_EXIT:
            SendMessage(m_hwnd, WM_CLOSE, NULL, NULL);
            return 0;
        }
    }
    return DefWindowProc(m_hwnd, uMsg, wParam, lParam);
}

HRESULT MainWindow::CreateGraphicsResources()
{
    HRESULT hr = S_OK;
    if (pRenderTarget == NULL)
    {
        RECT rc;
        GetClientRect(m_hwnd, &rc);

        D2D1_SIZE_U size = D2D1::SizeU(rc.right, rc.bottom);

        hr = pFactory->CreateHwndRenderTarget(D2D1::RenderTargetProperties(), D2D1::HwndRenderTargetProperties(m_hwnd, size), &pRenderTarget);

        if (SUCCEEDED(hr))
        {
            const D2D1_COLOR_F color = D2D1::ColorF(1.0f, 1.0f, 0);
            hr = pRenderTarget->CreateSolidColorBrush(color, &pBrush);

            if (SUCCEEDED(hr))
            {
                CalculateLayout();
            }
        }
    }

    // Create array of rectangles
    if (rectangles == NULL)
    {
        rectangles = new D2D1_RECT_F * [cellGrid.width];
        for (int i = 0; i < cellGrid.width; i++)
        {
            rectangles[i] = new D2D1_RECT_F[cellGrid.height];
        }
    }

    return hr;
}

void MainWindow::CalculateLayout()
{
    if (pRenderTarget != NULL)
    {
        size = pRenderTarget->GetSize();

        cellLength = size.height / cellGrid.height;

        init_x = (size.width - (cellLength * cellGrid.width)) / 2;

        rect_left.left = 0;
        rect_left.right = init_x;
        rect_left.top = 0;
        rect_left.bottom = size.height;

        rect_right.left = size.width - init_x;
        rect_right.right = size.width;
        rect_right.top = 0;
        rect_right.bottom = size.height;
    }
}

void MainWindow::OnPaint()
{
    HRESULT hr = CreateGraphicsResources();
    if (SUCCEEDED(hr))
    {
        PAINTSTRUCT ps;
        BeginPaint(m_hwnd, &ps);

        pRenderTarget->BeginDraw();

        pRenderTarget->Clear(D2D1::ColorF(D2D1::ColorF::DimGray));

        DrawCellGrid();

        pBrush->SetColor(D2D1::ColorF(0.25f, 0.25f, 0.25f));
        pRenderTarget->FillRectangle(rect_left, pBrush);
        pRenderTarget->FillRectangle(rect_right, pBrush);

        hr = pRenderTarget->EndDraw();
        if (FAILED(hr) || hr == D2DERR_RECREATE_TARGET)
        {
            DiscardGraphicsResources();
        }
        EndPaint(m_hwnd, &ps);
    }
}

void MainWindow::DrawCellGrid()
{

    for (int i = 0; i < cellGrid.width; i++)
    {
        for (int j = 0; j < cellGrid.height; j++)
        {
            // Create
            rectangles[i][j].left = init_x + cellLength * i;
            rectangles[i][j].top = cellLength * j;
            rectangles[i][j].right = init_x + cellLength + cellLength * i + 1;
            rectangles[i][j].bottom = cellLength + cellLength * j + 1;

            // Color
            if (cellGrid.cells[i][j] == false)
            {
                pBrush->SetColor(D2D1::ColorF(0, 0, 0));
            }
            else
            {
                pBrush->SetColor(D2D1::ColorF(1.0f, 1.0f, 1.0f));
            }

            // Draw
            pRenderTarget->FillRectangle(rectangles[i][j], pBrush);
        }
    }
    // Draw Grid
    if (drawGrid)
    {
        pBrush->SetColor(D2D1::ColorF(1.0f, 1.0f, 1.0f));
        for (int i = 0; i < cellGrid.width + 1; i++)
        {
            pRenderTarget->DrawLine(D2D1::Point2F(init_x + cellLength * i, 0), D2D1::Point2F(init_x + cellLength * i, size.height), pBrush);
        }

        for (int j = 0; j < cellGrid.height + 1; j++)
        {
            pRenderTarget->DrawLine(D2D1::Point2F(init_x, cellLength * j), D2D1::Point2F(init_x + cellLength * (cellGrid.width), cellLength * j), pBrush);
        }
    }
}

void MainWindow::Resize()
{
    if (pRenderTarget != NULL)
    {
        RECT rc;
        GetClientRect(m_hwnd, &rc);

        D2D1_SIZE_U size = D2D1::SizeU(rc.right, rc.bottom);

        pRenderTarget->Resize(size);
        CalculateLayout();
        InvalidateRect(m_hwnd, NULL, FALSE);
    }
}

void MainWindow::DiscardGraphicsResources()
{
    SafeRelease(&pRenderTarget);
    SafeRelease(&pBrush);
}