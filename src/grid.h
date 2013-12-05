#ifndef GRID_H
#define GRID_H

#include "extra.h"
#include <vector>

class Grid
{
public:
	Grid(int x, int y, int z);
	int x;
	int y;
	int z;

	vector<int> findCell(Vector3f position);
	void addParticle(Vector3f position, int index);
	int numParticlesInCell(Vector3f position);

private:
    vector<vector<vector<vector<int>>>> g;
};
#endif