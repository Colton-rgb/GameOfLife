#pragma once
#include <windows.h>

#include "CellGrid.h"

HWND CreateEditGridWindow(CellGrid *pCellGrid, HWND parent);

LRESULT CALLBACK EditGridPrc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);