#include "pendulumSystem.h"

#include <math.h>
#include <string>
#include <sstream>
#include <iostream>

using namespace std;

Grid::Grid(int xDim, int yDim, int zDim) {
	this->x = xDim;
	this->y = yDim;
	this->z = zDim;

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

vector<int> Grid::findCell(Vector3f position) {
	int x = floor(position.x()) + this->x;
	int y = floor(position.y()) + this->y;
	int z = floor(position.z()) + this->z;

	return g[x][y][z];
}

void Grid::addParticle(Vector3f position, int index) {
	vector<int> cell = findCell(position);
	cell.push_back(index);
}

int Grid::numParticlesInCell(Vector3f position) {
	vector<int> cell = findCell(position);
	return cell.size();
}

string Grid::printGrid(){
	string gridString="";
	for (int i=0; i < 2*this->x; i++) {
		for (int j=0; j < 2*this->y; j++) {
			for (int k=0; k < 2*this->z; k++) {
				gridString += " " ;
				for (int m=0; m < g[i][j][k].size(); m++){
					stringstream ss;//create a stringstream
   					ss << this->g[i][j][k][m];//add number to the stream
					gridString += ","+ ss.str();
				}
			}
			gridString += "\n";
		}
		gridString += "\n \n";
	}
	return gridString;
}