#include <string.h>
#include <stdlib.h>
#include <time.h> 

#include "CellGrid.h"

CellGrid::CellGrid(int width, int height)
{
	this->width =  width;
	this->height = height;

	cells = new bool* [width];
	buffer = new bool* [width];

	for (int i = 0; i < width; i++)
	{
		cells[i] = new bool[height];
		buffer[i] = new bool[height];

		for (int j = 0; j < height; j++)
		{
			buffer[i][j] = 0;
			cells[i][j] = 0;
		}
	}

}

void CellGrid::update()
{
	for (int i = 0; i < width; i++)
	{
		for (int j = 0; j < height; j++)
		{
			 buffer[i][j] = cells[i][j];
		}
	}

	for (int i = 0; i < width; i++)
	{
		for (int j = 0; j < height; j++)
		{
			int nCount = getLiveNeighbors(i, j);
			if (buffer[i][j] == true)
			{
				if (nCount < 2 || nCount > 3)
				{
					cells[i][j] = false;
				}
				else
				{
					cells[i][j] = true;
				}
			}
			else if(buffer[i][j] == false)
			{
				if (nCount == 3)
				{
					cells[i][j] = true;
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
		if (row + i < 0 || row + i > width - 1) continue;

		for (int j = -1; j <= 1; j++)
		{
			if ((col + j < 0) || (col + j > height - 1) || (i == 0 && j==0)) continue;

			if (buffer[row + i][col + j] == true)
			{
				cAlive++;
			}
		}
	}
	return cAlive;
}

void CellGrid::randomize() {
	for (int i = 0; i < width; i++)
	{
		for (int j = 0; j < height; j++)
		{
			cells[i][j] = rand() & 1;
		}
	}
}
