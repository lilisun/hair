#include "pendulumSystem.h"

#include <math.h>

Grid::Grid(int x, int y, int z) {
	this.x = x;
	this.y = y;
	this.z = z;

	for (int i=0; i < 2*x; i++) {
		vector<vector<vector<int>>> temp;
		for (int j=0; j < 2*y; j++) {
			vector<vector<int>> temp2;
			for (int k=0; k < 2*z; k++) {
				vector<int> cell;
				cell.push_back(-1);
				temp2.push_back(cell);
			}

			temp.push_back(temp2);
		}

		g.push_back(temp);
	}
}

Grid::findCell(Vector3f position) {
	int x = floor(position.x()) + this.x;
	int y = floor(position.y()) + this.y;
	int z = floor(position.z()) + this.z;

	return g[x][y][z];
}

Grid::addParticle(Vector3f position, int index) {
	vector<int> cell = findCell(position);
	cell.push_back(index);
}

Grid::numParticlesInCell(Vector3f position) {
	vector<int> cell = findCell(position);
	return cell.size();
}
