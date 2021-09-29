class CellGrid
{

public:
	bool** cells;
	int width;
	int height;
	CellGrid(int width, int height);

	void update();
};