#include <windows.h>
#include <d2d1.h>


#include "CellGrid.h"
#include "BaseWindow.h"
#include "MainWindow.h"

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
        if (FAILED(D2D1CreateFactory(
            D2D1_FACTORY_TYPE_SINGLE_THREADED, &pFactory)))
        {
            return -1;  // Fail CreateWindowEx.
        }
        return 0;

    case WM_DESTROY:
        DiscardGraphicsResources();
        SafeRelease(&pFactory);
        PostQuitMessage(0);
        return 0;

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
        D2D1_SIZE_F size = pRenderTarget->GetSize();
        const float x = size.width / 2;
        const float y = size.height / 2;
        const float radius = min(x, y);
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
       // pRenderTarget->FillEllipse(ellipse, pBrush);

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
    D2D1_SIZE_F size = pRenderTarget->GetSize();
    float cellLength = size.height / cellGrid.height;

    for (int i = 0; i < cellGrid.width; i++)
    {
        for (int j = 0; j < cellGrid.height; j++)
        {
            rectangles[i][j] = D2D1::RectF(0 + cellLength*i, 0 + cellLength * j, cellLength + cellLength*i, cellLength+cellLength*j);
            if (cellGrid.cells[i][j] == false)
            {
                pBrush->SetColor(D2D1::ColorF(0, 0, 0));
            }
            else
            {
                pBrush->SetColor(D2D1::ColorF(1.0f, 1.0f, 1.0f));
            }
            pRenderTarget->FillRectangle(rectangles[i][j], pBrush);
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