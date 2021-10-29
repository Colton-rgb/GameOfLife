#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <iostream>
#include <fstream>

using namespace std;

#include "CellGrid.h"

// Constructor
CellGrid::CellGrid(int width, int height)
{
	this->width = width;
	this->height = height;

	generate(width, height);
}

// Fills the cells and buffer arrays with 0's, the array sizes are newWidth and newHeight
void CellGrid::generate(int newWidth, int newHeight)
{
	if (cells && buffer)
	{
		for (int i = 0; i < width; i++)
		{
			delete[] cells[i];
			delete[] buffer[i];
		}
		delete[] cells;
		delete[] buffer;
	}

	width = newWidth;
	height = newHeight;

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


// Apply the rules of Conway's Game of Life to the array of booleans
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
			int iCount = getLiveNeighbors(i, j);
			if (buffer[i][j] == true)
			{
				if (iCount < 2 || iCount > 3)
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
				if (iCount == 3)
				{
					cells[i][j] = true;
				}
			}
		}
	}
}

// Utility for update(), returns the number of true booleans adjacent any given row and column
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

// Fills the cell array with random 1's or 0's
void CellGrid::randomize() {
	for (int i = 0; i < width; i++)
	{
		for (int j = 0; j < height; j++)
		{
			cells[i][j] = rand() & 1;
		}
	}
}

// Fills cell array with 0's
void CellGrid::clear()
{
	for (int i = 0; i < width; i++)
	{
		for (int j = 0; j < height; j++)
		{
			cells[i][j] = 0;
		}
	}
}

// Save the current cell array to a file with the first two int's being the width and height
void CellGrid::save(std::string file)
{
	ofstream outdata;
	outdata.open(file);

	outdata << width << " " << height << '\n';

	for (int i = 0; i < width; i++)
	{
		for (int j = 0; j < height; j++)
		{
			outdata << cells[i][j] << ' ';
		}
		outdata << '\n';
	}

	outdata.close();
}

// Load a given file from a string filepath into the cell array, first two int's being the width and height
void CellGrid::load(std::string file)
{
	ifstream indata;
	indata.open(file);

	int newWidth = 0;
	int newHeight = 0;

	indata >> newWidth;
	indata >> newHeight;

	generate(newWidth, newHeight);

	for (int i = 0; i < width; i++)
	{
		for (int j = 0; j < height; j++)
		{
			indata >> cells[i][j];
		}
	}

	indata.close();
}