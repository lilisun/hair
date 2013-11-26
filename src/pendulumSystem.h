#ifndef PENDULUMSYSTEM_H
#define PENDULUMSYSTEM_H

#include "extra.h"
#include <vector>

#include "particleSystem.h"

class PendulumSystem: public ParticleSystem
{
public:
	PendulumSystem(int numParticles);
	
	vector<Vector3f> evalF(vector<Vector3f> state);
	
	void draw();

	bool drawSprings;
	void showSprings();

	Vector3f getParticlePosition(vector<Vector3f> state, int x);

	Vector3f getParticleVelocity(vector<Vector3f> state, int x);

	float mass;
    Vector3f gravity;
    float drag_const;
    float spring_const;
    float rest_len;

    vector<vector<Vector3f>> springs;


	void moveBack() {
		return;}
	void moveForward() {
		return;}
};

#endif
