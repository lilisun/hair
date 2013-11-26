#ifndef CLOTHSYSTEM_H
#define CLOTHSYSTEM_H

#include "extra.h"
#include <vector>

#include "particleSystem.h"

class ClothSystem: public ParticleSystem
{
///ADD MORE FUNCTION AND FIELDS HERE
public:
	ClothSystem(int size);
	int size;
	int num_particles;

	vector<Vector3f> evalF(vector<Vector3f> state);
	
	void draw();

	bool drawSprings;
	void showSprings();

	float mass;
    Vector3f gravity;
    float drag_const;
    float spring_const;
    float rest_len;

    int indexOfParticle(int row, int col);
	bool isPointValid(Vector2f point);
	Vector3f addSpringForces(vector<Vector3f> state, vector<Vector3f> spr, Vector3f current_position);

    vector<vector<Vector3f>> structural_springs;
    vector<vector<Vector3f>> shear_springs;
    vector<vector<Vector3f>> flex_springs;

    bool move_back;
    bool move_forward;

    void moveBack();
    void moveForward();
private:

};


#endif
