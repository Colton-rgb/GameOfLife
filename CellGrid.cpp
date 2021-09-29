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

}

void CellGrid::update()
{
	// Result array
	bool** result = new bool* [width];
	for (int i = 0; i < width; i++)
	{
		cells[i] = new bool[height];
	}

	for (int i = 0; i < width; i++)
	{
		for (int j = 0; j < height; j++)
		{
			int nCount = getLiveNeighbors(i, j);
			if (cells[i][j] == true)
			{
				if (nCount < 2 || nCount > 3)
				{
					result[i][j] == true;
				}
			}
			else
			{
				if (nCount == 3)
				{
					result[i][j] == true;
				}
			}
		}
	}
}

int CellGrid::getLiveNeighbors(int row, int col)
{
	int cAlive = 0;
	for (int i = -1; i <= 1; i++)
	{
		if (row + i < 0 || row + i > width) continue;
		for (int j = -1; j <= 1; j++)
		{
			if (col + j < 0 || col + j > height || (i == 0 && j==0)) continue;

			if (cells[row + i][col + j] == true)
			{
				cAlive++;
			}
		}
	}
	return cAlive;
}
