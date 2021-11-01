#include <windows.h>
#include <d2d1.h>
#include <WindowsX.h>
#include <CommCtrl.h>
#include <Shobjidl.h>

#include "CellGrid.h"
#include "BaseWindow.h"
#include "GridWindow.h"
#include "Toolbar.h"
#include "EditGridWindow.h"

#define TIMER_ID 1

// Safe Releases some of the variables created for Direct 2D, found in the windows documentation
template <class T>
void SafeRelease(T** ppT)
{
    if (*ppT)
    {
        (*ppT)->Release();
        *ppT = NULL;
    }
}

// Very long function that handles all of the messages that the window will recieve from Windows
LRESULT GridWindow::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_CREATE:
    {
        if (FAILED(D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &pFactory)))
        {
            return -1;
        }

        // Sets cursors permanently
        SetCursor(hCursor);

        // Creation of the timer that will redraw the screen
        SetTimer(m_hwnd, TIMER_ID, 75, (TIMERPROC)NULL);

        hToolbar = CreateToolbar(m_hwnd);

        return 0;
    }

    case WM_SETCURSOR:
    {
        if (LOWORD(lParam) == HTCLIENT)
        {
            SetCursor(hCursor);
        }
    } return 0;

    case WM_DESTROY:
    {
        KillTimer(m_hwnd, TIMER_ID);
        DiscardGraphicsResources();
        SafeRelease(&pFactory);
        PostQuitMessage(0);
    } return 0;

    case WM_LBUTTONDOWN:
    {
        if (running == false)
        {
            int xPos = GET_X_LPARAM(lParam);
            int yPos = GET_Y_LPARAM(lParam);

            int gridX = (int)((xPos - (int)init_x) / cellLength);
            int gridY = (int)((yPos - (int)init_y) / cellLength);

            // Check bounds
            if (gridX < 0 || gridX > cellGrid.width - 1 || gridY < 0 || gridY > cellGrid.height - 1) return 0;

            // apply change to grid
            cellGrid.cells[gridX][gridY] = !cellGrid.cells[gridX][gridY];
        }
    } return 0;

    case WM_KEYDOWN:
    {
        return HandleKeyboardInput();
    } return 0;

    case WM_COMMAND:
    {
        return HandleCommonControls(wParam);
    } return 0;

    case WM_TIMER:
    {
        switch (wParam)
        {
        case TIMER_ID:
            if (running)
            {
                cellGrid.update();
            }
        }
        SendMessage(m_hwnd, WM_PAINT, 0, 0);
    } return 0;

    case WM_PAINT:
    {
        OnPaint();
    } return 0;

    case WM_SIZE:
    {
        Resize();

        // Resize the toolbar
        SendMessage(hToolbar, TB_AUTOSIZE, 0, 0);
    } return 0;

    } // End of switch

    // Let windows handle all of the messages I don't care about
    return DefWindowProc(m_hwnd, uMsg, wParam, lParam);
}

LRESULT GridWindow::HandleKeyboardInput()
{
    if (GetAsyncKeyState('R')) // R key
    {
        cellGrid.randomize();
        return 0;
    }

    if (GetAsyncKeyState('G')) // G key
    {
        drawGrid = !drawGrid;
        if (drawGrid)
            SendMessage(hToolbar, TB_MARKBUTTON, ID_TBGRID, MAKELONG(1, 0));
        else
            SendMessage(hToolbar, TB_MARKBUTTON, ID_TBGRID, 0);
        return 0;
    }

    if (GetAsyncKeyState('C')) // C key
    {
        cellGrid.clear();
        return 0;
    }

    if (GetAsyncKeyState('S')) // S key
    {
        cellGrid.save("quick_save.txt");
        return 0;
    }

    if (GetAsyncKeyState('L')) // L key
    {
        load_grid("quick_save.txt");
        SendMessage(Window(), WM_SIZE, 0, 0);
        return 0;
    }

    if (GetAsyncKeyState(VK_ESCAPE)) // ESC key
    {
        SendMessage(m_hwnd, WM_CLOSE, NULL, NULL);
        return 0;
    }

    if (GetAsyncKeyState(VK_RIGHT)) // Right Arrow Key
    {
        cellGrid.update();
        OnPaint();
        return 0;
    }

    if (GetKeyState(VK_SPACE)) // Space key
    {
        running = !running;
        if (running)
            SendMessage(hToolbar, TB_MARKBUTTON, ID_TBRUN, MAKELONG(1, 0));
        else
            SendMessage(hToolbar, TB_MARKBUTTON, ID_TBRUN, 0);
        return 0;
    }

    return 0;
}


// Another long function that handles all of the common control messages
LRESULT GridWindow::HandleCommonControls(WPARAM wParam)
{
    switch (LOWORD(wParam))
    {
    case ID_TBRUN:
    {
        running = !running;
        if (running)
            SendMessage(hToolbar, TB_MARKBUTTON, ID_TBRUN, MAKELONG(1, 0));
        else
            SendMessage(hToolbar, TB_MARKBUTTON, ID_TBRUN, 0);
        return 0;
    }

    case ID_TBGRID:
    {
        drawGrid = !drawGrid;
        if (drawGrid)
            SendMessage(hToolbar, TB_MARKBUTTON, ID_TBGRID, MAKELONG(1, 0));
        else
            SendMessage(hToolbar, TB_MARKBUTTON, ID_TBGRID, 0);
    } return 0;

    case ID_TBEDIT:
    {
        CreateEditGridWindow(&cellGrid, m_hwnd);
    } return 0;

    case ID_TBDELETE:
    {
        cellGrid.clear();
    } return 0;

    case ID_TBRANDOMIZE:
    {
        cellGrid.randomize();
    } return 0;

    case ID_TBSAVE:
    {
        // Creating the pop up window to select a file is very long-winded

        IFileSaveDialog* pFileSave;

        // Create the FileSaveDialog object.
        HRESULT hr = CoCreateInstance(CLSID_FileSaveDialog, NULL, CLSCTX_ALL,
            IID_IFileSaveDialog, reinterpret_cast<void**>(&pFileSave));

        // Add txt filter
        COMDLG_FILTERSPEC testFilter = { };
        testFilter.pszName = L"Text Document";
        testFilter.pszSpec = L"*.txt";
        pFileSave->SetFileTypes(1, &testFilter);

        if (SUCCEEDED(hr))
        {
            hr = pFileSave->Show(NULL);

            // Get the file name from the dialog box.
            if (SUCCEEDED(hr))
            {
                IShellItem* pItem;
                hr = pFileSave->GetResult(&pItem);
                if (SUCCEEDED(hr))
                {
                    PWSTR pszFilePath;
                    hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath);
                    if (SUCCEEDED(hr))
                    {

                        std::wstring ws(pszFilePath);
                        std::string filePath(ws.begin(), ws.end());

                        cellGrid.save(filePath);

                        // MessageBoxW(NULL, pszFilePath, L"File Path", MB_OK);
                        CoTaskMemFree(pszFilePath);
                    }
                    pItem->Release();
                }
            }
            pFileSave->Release();
        }
    } return 0;

    case ID_TBOPEN:
    {
        IFileSaveDialog* pFileOpen;

        // Create the FileOpenDialog object.
        HRESULT hr = CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_ALL,
            IID_IFileOpenDialog, reinterpret_cast<void**>(&pFileOpen));

        if (SUCCEEDED(hr))
        {
            hr = pFileOpen->Show(NULL);

            // Get the file name from the dialog box.
            if (SUCCEEDED(hr))
            {
                IShellItem* pItem;
                hr = pFileOpen->GetResult(&pItem);
                if (SUCCEEDED(hr))
                {
                    PWSTR pszFilePath;
                    hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath);

                    if (SUCCEEDED(hr))
                    {

                        std::wstring ws(pszFilePath);
                        std::string filePath(ws.begin(), ws.end());

                        load_grid(filePath);

                        SendMessage(Window(), WM_SIZE, 0, 0);

                        CoTaskMemFree(pszFilePath);
                    }
                    pItem->Release();
                }
            }
            pFileOpen->Release();
        }
    } return 0;
    } // End of switch (wParam)
    return 0;
}

// Create and fill in all of the data need to draw to the window
HRESULT GridWindow::CreateGraphicsResources()
{
    HRESULT hr = S_OK;
    if (pRenderTarget == NULL)
    {
        RECT rc;
        GetClientRect(m_hwnd, &rc);

        D2D1_SIZE_U sizeU = D2D1::SizeU(rc.right, rc.bottom);

        hr = pFactory->CreateHwndRenderTarget(D2D1::RenderTargetProperties(), D2D1::HwndRenderTargetProperties(m_hwnd, sizeU), &pRenderTarget);

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

// When the windows changes size, so does everything inside of it
void GridWindow::CalculateLayout()
{
    if (pRenderTarget != NULL)
    {
        size = pRenderTarget->GetSize();
        tbRect = { 0 };
        GetWindowRect(hToolbar, &tbRect);

        init_y = tbRect.bottom - tbRect.top;

        cellLength = (size.height - init_y) / cellGrid.height;

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

void GridWindow::OnPaint()
{
    HRESULT hr = CreateGraphicsResources();
    if (SUCCEEDED(hr))
    {
        PAINTSTRUCT ps;
        BeginPaint(m_hwnd, &ps);

        pRenderTarget->BeginDraw();

        pRenderTarget->Clear(D2D1::ColorF(D2D1::ColorF::DimGray));

        DrawCellGrid();

        pBrush->SetColor(D2D1::ColorF(0.15f, 0.15f, 0.15f));
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

void GridWindow::DrawCellGrid()
{
    for (int i = 0; i < cellGrid.width; i++)
    {
        for (int j = 0; j < cellGrid.height; j++)
        {
            // Create
            rectangles[i][j].left = init_x + cellLength * i;
            rectangles[i][j].top = init_y +cellLength * j;
            rectangles[i][j].right = init_x + cellLength + cellLength * i + 1;
            rectangles[i][j].bottom = init_y + cellLength + cellLength * j + 1;

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

    // Outline grid

    // D2D1::Point2F(init_x + cellLength * cellGrid.width, init_y + cellLength * cellGrid.height)
    pBrush->SetColor(D2D1::ColorF(0, 0.75f, 1.0f));

    pRenderTarget->DrawLine(D2D1::Point2F(init_x, init_y + 1), D2D1::Point2F(init_x * cellLength * cellGrid.width, init_y + 1), pBrush, 3);

    pRenderTarget->DrawLine(
        D2D1::Point2F(init_x, init_y + cellLength * cellGrid.height - 1),
        D2D1::Point2F(init_x + cellLength * cellGrid.width, init_y + cellLength * cellGrid.height - 1), pBrush,
        3);

    pRenderTarget->DrawLine(D2D1::Point2F(init_x, init_y), D2D1::Point2F(init_x, init_y * cellLength * cellGrid.height), pBrush, 3);

    pRenderTarget->DrawLine(
        D2D1::Point2F(init_x + cellLength * cellGrid.width, init_y),
        D2D1::Point2F(init_x + cellLength * cellGrid.width, init_y * cellLength * cellGrid.height), pBrush,
        3);

    // Draw Grid
    if (drawGrid)
    {
        pBrush->SetColor(D2D1::ColorF(0.50f, 0.50f, 0.50f));
        for (int i = 0; i < cellGrid.width + 1; i++)
        {
            pRenderTarget->DrawLine(D2D1::Point2F(init_x + cellLength * i, init_y), D2D1::Point2F(init_x + cellLength * i, init_y + size.height), pBrush);
        }

        for (int j = 0; j < cellGrid.height + 1; j++)
        {
            pRenderTarget->DrawLine(D2D1::Point2F(init_x, init_y + cellLength * j), D2D1::Point2F(init_x + cellLength * (cellGrid.width), init_y + cellLength * j), pBrush);
        }
    }
}

void GridWindow::Resize()
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

// Clears the memory from the old rectangles and fills in new memory with the new width and height
void GridWindow::ResizeRectangles(int width, int height, int oldWidth)
{
    for (int i = 0; i < oldWidth; i++)
    {
        delete[] rectangles[i];
    }
    delete[] rectangles;

    rectangles = new D2D1_RECT_F * [cellGrid.width];
    for (int i = 0; i < cellGrid.width; i++)
    {
        rectangles[i] = new D2D1_RECT_F[cellGrid.height];
    }
}

void GridWindow::load_grid(std::string file)
{
    int oldWidth = cellGrid.width;
    cellGrid.load(file);
    ResizeRectangles(cellGrid.width, cellGrid.height, oldWidth);
}

void GridWindow::DiscardGraphicsResources()
{
    SafeRelease(&pRenderTarget);
    SafeRelease(&pBrush);
}