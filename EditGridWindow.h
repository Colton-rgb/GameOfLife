#pragma once
#include <windows.h>

#include "CellGrid.h"

HWND CreateEditGridWindow(CellGrid cg);

LRESULT CALLBACK EditGridPrc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);