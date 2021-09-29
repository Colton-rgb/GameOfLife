#include <string.h>

#include "CellGrid.h"

CellGrid::CellGrid(int width, int height)
{
	this->width =  width;
	this->height = height;

	cells = new bool* [width];

	for (int i = 0; i < width; i++)
	{
		cells[i] = new bool[height];
	}

	//Debug thing
	cells[0][0] = true;

	for (int i = 0; i < width; i++)
	{
		for (int j = 0; j < height; j++)
		{
			cells[i][j] = 0;
		}
	}
	cells[8][8] = true;
}
