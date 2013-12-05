#ifndef PENDULUMSYSTEM_H
#define PENDULUMSYSTEM_H

#include "extra.h"
#include <vector>

#include "particleSystem.h"
#include "grid.h"

class PendulumSystem: public ParticleSystem
{
public:
	PendulumSystem(int numParticles, int howManyStrands);
	
	vector<Vector3f> evalF(vector<Vector3f> state);

	void draw();

	bool drawSprings;
	void showSprings();
    bool drawGhostParticles;
    void showGhostParticles();
    bool drawCylinders;
    void showCylinders();
    bool hasForce;
    void addForce();

	Vector3f getParticlePosition(vector<Vector3f> state, int x);
	Vector3f getParticleVelocity(vector<Vector3f> state, int x);

	Vector3f addSpringForces(vector<Vector3f> state, vector<Vector4f> spr, Vector3f current_position);

	float mass;
    Vector3f gravity;
    float drag_const;
    float spring_const;
    float less_stiff_spring_const;
    float rest_len;
    float strand_offset;

    vector<Vector3f> ghost_particles;
    vector<vector<Vector4f>> edge_springs;
    vector<vector<Vector4f>> ghost_edge_springs;
    vector<vector<Vector4f>> bend_springs;
    vector<vector<Vector4f>> ghost_bend_springs;
    vector<vector<Vector4f>> torsion_springs;

    void drawSpring(vector<vector<Vector4f>> springs);

    Grid grid;

    int numStrands;
    int numStrandParticles;
	void initializeStrand();

	void moveBack() {
		return;}
	void moveForward() {
		return;}
private:
	int numHairParticles;
	int numGhostParticles;
};

#endif
