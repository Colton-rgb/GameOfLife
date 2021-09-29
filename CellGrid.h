class CellGrid
{
	bool** cells;
	int width;
	int height;

public:

	CellGrid(int width, int height);

	void update();
};