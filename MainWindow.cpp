#include <windows.h>
#include <d2d1.h>


#include "CellGrid.h"
#include "BaseWindow.h"
#include "MainWindow.h"

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
        if (FAILED(D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &pFactory)))
        {
            return -1;
        }

        // TIMER
        SetTimer(m_hwnd, TIMER_ID, 150, (TIMERPROC)NULL);

        return 0;

    case WM_DESTROY:
        KillTimer(m_hwnd, TIMER_ID);
        DiscardGraphicsResources();
        SafeRelease(&pFactory);
        PostQuitMessage(0);
        return 0;

    case WM_KEYDOWN:
    {
        if (GetKeyState(0x52))
        {
            cellGrid.randomize();
        }
        if (!GetKeyState(VK_SPACE))
        {
            return 0;
        }
        cellGrid.update();
    }

    case WM_TIMER:
        switch (wParam)
        {
        case TIMER_ID:
            cellGrid.update();
        }

    case WM_PAINT:
        OnPaint();
        return 0;

    case WM_SIZE:
        Resize();
        return 0;
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
            rectangles[i][j] = D2D1::RectF(
                init_x + cellLength * i,              // Left
                0 + cellLength * j,                   // Top
                init_x + cellLength + cellLength * i, //Right
                cellLength + cellLength * j           // Bottom
            );

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
    //// Draw Grid
    //pBrush->SetColor(D2D1::ColorF(1.0f, 1.0f, 1.0f));
    //for (int i = 0; i < cellGrid.width + 1; i++)
    //{
    //    pRenderTarget->DrawLine(D2D1::Point2F(init_x + cellLength * i, 0), D2D1::Point2F(init_x + cellLength * i, size.height), pBrush, 1);
    //}

    //for (int j = 0; j < cellGrid.height + 1; j++)
    //{
    //    pRenderTarget->DrawLine(D2D1::Point2F(init_x, cellLength * j), D2D1::Point2F(init_x + cellLength * (cellGrid.width), cellLength * j), pBrush, 1);
    //}
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