#pragma once

#include <windows.h>

#define ID_TBRUN 1
#define ID_TBGRID 2
#define ID_TBOPEN 3
#define ID_TBSAVE 4
#define ID_TBEDIT 5
#define ID_TBDELETE 6
#define ID_TBRANDOMIZE 7

HWND CreateToolbar(HWND hWndParent);