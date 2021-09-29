#include <d2d1.h>
#include <string.h>

#include "CellGrid.h"

CellGrid::CellGrid(int width, int height)
{
	this->width =  width;
	this->height = height;

	// Dynamically Allocate a 2D Array of bools
	cells = new bool* [width];
	for (int i = 0; i < width; i++)
	{
		cells[i] = new bool[height];
	}

	// Set all cells to 0
	memset(cells, 0, width * height * sizeof(*cells));

	//Debug thing
	cells[0][0] = true;
}
