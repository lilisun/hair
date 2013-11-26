#include "ClothSystem.h"
#include <iostream>

//TODO: Initialize here
ClothSystem::ClothSystem(int s)
{
	mass = 0.05f;
    gravity = Vector3f(0.0f, -1.0f, 0.0f);
    drag_const = 0.01f;
    spring_const = 4.0f;
    rest_len = 0.5f;

	size = s;
	m_numParticles = size * size;

	move_back = false;
	move_forward = false;
	drawSprings = false;

	// initialize state


	// row is x, col is y
	for (int row=0; row < size; row++) {
		for (int col=0; col < size; col++) {
			m_vVecState.push_back(Vector3f(row*rest_len, -col*rest_len, 0));
			m_vVecState.push_back(Vector3f(0,0,0));

			// structural springs
			vector<Vector3f> structural;
			vector<Vector2f> structural_points;
            structural_points.push_back(Vector2f(row-1, col));
            structural_points.push_back(Vector2f(row+1, col));
            structural_points.push_back(Vector2f(row, col-1));
            structural_points.push_back(Vector2f(row, col+1));
			for (int i=0; i < structural_points.size(); i++) {
				Vector2f point = structural_points[i];

				if (isPointValid(point)) {
					structural.push_back(Vector3f(indexOfParticle(point[0], point[1]), spring_const, rest_len));
				}
			}

			structural_springs.push_back(structural);

			// shear springs
			vector<Vector3f> shear;
			vector<Vector2f> shear_points;
            shear_points.push_back(Vector2f(row-1, col-1));
            shear_points.push_back(Vector2f(row-1, col+1));
            shear_points.push_back(Vector2f(row+1, col-1));
            shear_points.push_back(Vector2f(row+1, col+1));
			float hypotenuse = sqrt(2*pow(rest_len, 2.0f));
			for (int i=0; i < shear_points.size(); i++) {
				Vector2f point = shear_points[i];

				if (isPointValid(point)) {
					shear.push_back(Vector3f(indexOfParticle(point[0], point[1]), spring_const, hypotenuse));
				}
			}

			shear_springs.push_back(shear);

			// flex springs
			vector<Vector3f> flex;
			vector<Vector2f> flex_points;
            flex_points.push_back(Vector2f(row-2, col));
            flex_points.push_back(Vector2f(row+2, col));
            flex_points.push_back(Vector2f(row, col-2));
            flex_points.push_back(Vector2f(row, col+2));
			for (int i=0; i < flex_points.size(); i++) {
				Vector2f point = flex_points[i];

				if (isPointValid(point)) {
					flex.push_back(Vector3f(indexOfParticle(point[0], point[1]), spring_const, 2*rest_len));
				}
			}

			flex_springs.push_back(flex);
		}
	}
}

// TODO: implement evalF
// for a given state, evaluate f(X,t)
vector<Vector3f> ClothSystem::evalF(vector<Vector3f> state)
{
	vector<Vector3f> f;

	for (int row=0; row < size; row++) {
		for (int col=0; col < size; col++) {
			Vector3f current_position = state[2*indexOfParticle(row, col)];
			Vector3f current_velocity = state[2*indexOfParticle(row, col) + 1];

			Vector3f force;
			if (row == 0 && col == 0) {
				force = Vector3f(0,0,0);
			}

			else if (row == (size-1) && col == 0) {
				force = Vector3f(0,0,0);
			}

			else {
				force = (mass*gravity) - drag_const*current_velocity;

				force += addSpringForces(state, structural_springs[indexOfParticle(row, col)],
					current_position);
				force += addSpringForces(state, shear_springs[indexOfParticle(row, col)],
					current_position);
				force += addSpringForces(state, flex_springs[indexOfParticle(row, col)],
					current_position);

				if (move_back) {
					force += Vector3f(0, 0, -1.0f);
					move_back = false;
				}

				if(move_forward) {
					force += Vector3f(0, 0, 1.0f);
					move_forward = false;
				}

			}

			f.push_back(current_velocity);
			f.push_back(force/mass);

			// f.push_back(Vector3f(0,0,0));
			// f.push_back(Vector3f(0,0,0));

		}
	}

	return f;
}

///TODO: render the system (ie draw the particles)
void ClothSystem::draw()
{
	for (int i = 0; i < m_numParticles; i++) {
		Vector3f pos = m_vVecState[2*i] ;//  position of particle i. YOUR CODE HERE
		glPushMatrix();
		glTranslatef(pos[0], pos[1], pos[2] );
		glutSolidSphere(0.075f,10.0f,10.0f);
		glPopMatrix();
	}

	if (drawSprings) {
		for(int i=0; i < structural_springs.size(); i++) {
	    	vector<Vector3f> s = structural_springs[i];

			for (int j=0; j < s.size(); j++) {
				Vector3f pos1 = m_vVecState[2*i];
				Vector3f pos2 = m_vVecState[2*s[j][0]];

				glDisable(GL_LIGHTING);
				glBegin(GL_LINES);
				glColor3f(1.0,1.0,1.0);
				glVertex3f(pos1[0], pos1[1], pos1[2]);
				glVertex3f(pos2[0], pos2[1], pos2[2]);
				glEnd();
				glEnable(GL_LIGHTING);
			}
		}

		for(int i=0; i < shear_springs.size(); i++) {
	    	vector<Vector3f> s = shear_springs[i];

			for (int j=0; j < s.size(); j++) {
				Vector3f pos1 = m_vVecState[2*i];
				Vector3f pos2 = m_vVecState[2*s[j][0]];

				glDisable(GL_LIGHTING);
				glBegin(GL_LINES);
				glColor3f(1.0,1.0,1.0);
				glVertex3f(pos1[0], pos1[1], pos1[2]);
				glVertex3f(pos2[0], pos2[1], pos2[2]);
				glEnd();
				glEnable(GL_LIGHTING);
			}
		}

		for(int i=0; i < flex_springs.size(); i++) {
	    	vector<Vector3f> s = flex_springs[i];

			for (int j=0; j < s.size(); j++) {
				Vector3f pos1 = m_vVecState[2*i];
				Vector3f pos2 = m_vVecState[2*s[j][0]];

				glDisable(GL_LIGHTING);
				glBegin(GL_LINES);
				glColor3f(1.0,1.0,1.0);
				glVertex3f(pos1[0], pos1[1], pos1[2]);
				glVertex3f(pos2[0], pos2[1], pos2[2]);
				glEnd();
				glEnable(GL_LIGHTING);
			}
		}
	}
}


void ClothSystem::showSprings() {
	if (drawSprings) {
		drawSprings = false;
	}

	else {
		drawSprings = true;
	}
}

int ClothSystem::indexOfParticle(int row, int col) {
	return row*size + col;
}

bool ClothSystem::isPointValid(Vector2f point) {
	int row = point[0];
	int col = point[1];

	if (row < 0 || row >= size) {
		return false;
	}

	else if (col < 0 || col >= size) {
		return false;
	}

	else {
		return true;
	}
}

Vector3f ClothSystem::addSpringForces(vector<Vector3f> state, vector<Vector3f> spr,
	Vector3f current_position) {
	Vector3f force;
	for (int i=0; i < spr.size(); i++) {
		Vector3f disp = current_position - state[2*spr[i][0]];
		force += -spr[i][1]*(disp.abs() - spr[i][2])*disp/disp.abs();
	}

	return force;
}

void ClothSystem::moveBack() {
	move_back = true;
}

void ClothSystem::moveForward() {
	move_forward = true;
}