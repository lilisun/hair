#ifndef GRID_H
#define GRID_H

#include "extra.h"
#include <vector>
#include <string>
#include <sstream>
#include <iostream>

using namespace std;

class Grid
{
public:
	Grid(int xDim=0, int yDim=0, int zDim=0);
	int x;
	int y;
	int z;

	vector<int>* findCell(Vector3f position);
	void addParticle(Vector3f position, int index);
	int numParticlesInCell(Vector3f position);

	string printGrid();

private:
    vector<vector<vector<vector<int>>>> g;
};
#endif