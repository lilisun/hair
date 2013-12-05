#include "pendulumSystem.h"

#include <math.h>
#include <string>
#include <sstream>
#include <iostream>

using namespace std;

Grid::Grid(int xDim, int yDim, int zDim, float len) {
	this->x = xDim;
	this->y = yDim;
	this->z = zDim;
	this->len = len;

	int x_total = ceil(2*x/len);
	int y_total = ceil(2*y/len);

	for (int i=0; i < x_total; i++) {
		vector<vector<vector<int>>> temp;
		for (int j=0; j < y_total; j++) {
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

vector<int>* Grid::findCell(Vector3f position) {
	int x = floor(position.x()/len) + this->x/len;
	int y = floor(position.y()/len) + this->y/len;
	int z = floor(position.z()) + this->z;
	return &g[x][y][z];
}

vector<int> Grid::getCell(Vector3f position) {
	int x = floor(position.x()/len) + this->x/len;
	int y = floor(position.y()/len) + this->y/len;
	int z = floor(position.z()) + this->z;

	return g[x][y][z];
}

void Grid::addParticle(Vector3f position, int index) {
	vector<int>* cell = findCell(position);
	if (cell->front() == -1) {
		cell->clear();
		nonEmpty.push_back(cell);
	}

	cell->push_back(index);
}

int Grid::numParticlesInCell(Vector3f position) {
	vector<int> cell = getCell(position);
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

void Grid::drawGrid() {
	glLineWidth(2);
	glBegin(GL_LINES);
	for (float i=0; i < 2*x; i+=len) {
		float index = i - this->x;
		glVertex3f(index,-this->y, 0);
		glVertex3f(index,this->y, 0);
	}

	for (float j=0; j < 2*y; j+=len) {
		float index = j - this->y;
		glVertex3f(-this->x, index, 0);
		glVertex3f(this->x, index, 0);
	}
	glEnd();
}

void Grid::reset() {
	int x_total = ceil(2*x/len);
	int y_total = ceil(2*y/len);

	for (int i=0; i < nonEmpty.size(); i++) {
		vector<int>* cell = nonEmpty[i];
		cell->clear();
		cell->push_back(-1);
	}

	nonEmpty.clear();
}
