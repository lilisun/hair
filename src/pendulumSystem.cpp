
#include "pendulumSystem.h"
#include <iostream>

#include <math.h>

PendulumSystem::PendulumSystem(int numParticles):ParticleSystem(numParticles)
{
	mass = 0.02f;
    gravity = Vector3f(0.0f, -1.0f, 0.0f);
	drag_const = 0.01f;
	spring_const = 5.0f;
	less_stiff_spring_const = 1.0f;
	rest_len = 0.5f;

	numStrands = 3;
	numHairParticles = numParticles;
	numGhostParticles = numParticles - 1;
	m_numParticles = (numParticles + numGhostParticles) * numStrands;
	drawSprings = false;
    drawGhostParticles=false;
    drawCylinders=false;
    
	for (int k = 1; k <= numStrands; k++){
		
		for (int i = 0; i < numHairParticles; i++) {
			// position vector
			if (i == 0) {
				m_vVecState.push_back(Vector3f(0,0,0));
				cout << m_vVecState.size() << " strand " << k << endl;
			}

			else {
				m_vVecState.push_back(Vector3f(i*rest_len, k * 0.2f, 0));
				cout << m_vVecState.size() << " strand " << k << " " << i*rest_len << " " << k* 0.2f << endl;
			}

			// velocity vector
			m_vVecState.push_back(Vector3f(0,0,0));

			// SPRINGS
			// edge springs
			vector<Vector3f> edge;
			if ((i-1) > -1) {
				edge.push_back(Vector3f(i*k-1, spring_const, rest_len)); // took out multiplying spring_const*i?
			}

			if ((i+1) < numHairParticles) {
				edge.push_back(Vector3f(i*k+1, spring_const, rest_len)); // same here
			}

			edge_springs.push_back(edge);

			// bending springs
			vector<Vector3f> bend;
			if ((i-2) > -1) {
				bend.push_back(Vector3f(i*k-2, less_stiff_spring_const, 2*rest_len));
			}
			if ((i+2) < numHairParticles) {
				bend.push_back(Vector3f(i*k+2, less_stiff_spring_const, 2*rest_len));
			}

			bend_springs.push_back(bend);

			// torsion springs
			vector<Vector3f> torsion;
			float hypotenuse = sqrt(pow((sqrt(3.0f)/2.0f)*rest_len, 2.0f) + pow(1.5f*rest_len, 2.0f));

			// first value is index of ghost particle that hair particle i should have an edge with
			if ((i+1) < numGhostParticles) {
				torsion.push_back(Vector3f(i*k+1 + numHairParticles, less_stiff_spring_const, hypotenuse));
			}
			else {
				torsion.push_back(Vector3f(i*k-2 + numHairParticles, less_stiff_spring_const, hypotenuse));
			}

			torsion_springs.push_back(torsion);
		}

		// GHOST PARTICLES (to form triangles)
		for (int j = 0; j < numGhostParticles; j++) {
			// position vector
			m_vVecState.push_back(Vector3f((j+0.5)*rest_len, 0.2f, k * 0.2f));

			// velocity vector
			m_vVecState.push_back(Vector3f(0,0,0));

			// edge springs
			vector<Vector3f> ghost_edge;
			// first value is index of hair particle that ghost particle i should have an edge with
			if (j == 0) {
				ghost_edge.push_back(Vector3f(0, spring_const*5, 0.2f));
				ghost_edge.push_back(Vector3f(j*k+1, spring_const*5, 0.2f));
			}
			else {
				ghost_edge.push_back(Vector3f(j*k+1, spring_const*5, 0.2f));
				ghost_edge.push_back(Vector3f(j*k, spring_const*5, 0.2f));
			}

			ghost_edge_springs.push_back(ghost_edge);

			// bending springs
			vector<Vector3f> ghost_bend;
			// first value is index of ghost particle that ghost particle i should have an edge with
			if ((j-1) > -1) {
				ghost_bend.push_back(Vector3f(j*k-1 + numHairParticles, less_stiff_spring_const, rest_len));
			}
			if ((j+1) < numGhostParticles) {
				ghost_bend.push_back(Vector3f(j*k+1 + numHairParticles, less_stiff_spring_const, rest_len));
			}

			ghost_bend_springs.push_back(ghost_bend);
		}

	}
}

vector<Vector3f> PendulumSystem::evalF(vector<Vector3f> state)
{
	vector<Vector3f> f;

	vector<Vector3f> empty;

	for (int i=0; i < numHairParticles; i++) {
		Vector3f current_position = getParticlePosition(state, i);
		Vector3f current_velocity = getParticleVelocity(state, i);

		Vector3f force = Vector3f(0,0,0);
		if (i == 0) {
			force = Vector3f(0,0,0);
		}

		else {
			force = (mass*gravity) - drag_const*current_velocity;

			// edge springs
			force += addSpringForces(state, edge_springs[i], current_position);
			// bending springs
			force += addSpringForces(state, bend_springs[i], current_position);
			// torsion springs
			force += addSpringForces(state, torsion_springs[i],	current_position);
		}

		f.push_back(current_velocity);
		f.push_back(force/mass);

		empty.push_back(Vector3f(0,0,0));
		empty.push_back(Vector3f(0,0,0));
	}

	for (int j=0; j < numGhostParticles; j++) {
		int index = j + numHairParticles;
		Vector3f current_position = getParticlePosition(state, index);
		Vector3f current_velocity = getParticleVelocity(state, index);

		Vector3f force = Vector3f(0,0,0);

		force = (mass*gravity) - drag_const*current_velocity;

		// edge springs
		force += addSpringForces(state, ghost_edge_springs[j], current_position);
		// bending springs
		force += addSpringForces(state, ghost_bend_springs[j], current_position);

		f.push_back(current_velocity);
		f.push_back(force/mass);

		empty.push_back(Vector3f(0,0,0));
		empty.push_back(Vector3f(0,0,0));
	}

	//return f;
	return empty;
}

Vector3f PendulumSystem::getParticlePosition(vector<Vector3f> state, int x) {
	return state[2*x];
}

Vector3f PendulumSystem::getParticleVelocity(vector<Vector3f> state, int x) {
	return state[2*x + 1];
}

Vector3f PendulumSystem::addSpringForces(vector<Vector3f> state, vector<Vector3f> spr,
	Vector3f current_position) {
	Vector3f force;
	for (int i=0; i < spr.size(); i++) {
		Vector3f disp = current_position - state[2*spr[i][0]];
		force += -spr[i][1]*(disp.abs() - spr[i][2])*disp/disp.abs();
	}

	return force;
}

// render the system (ie draw the particles)
void PendulumSystem::draw()
{
	for (int k = 0; k < numStrands; k++){
		for (int i = 0; i < numHairParticles; i++) {
			Vector3f pos = m_vVecState[2*i+k*(numHairParticles+numGhostParticles)];//  position of particle i. YOUR CODE HERE
			//cout << "strand " << k << " " << 2*i+k*(numHairParticles+numGhostParticles) << " " << pos << endl;
			glPushMatrix();
			glTranslatef(pos[0], pos[1], pos[2] );
	        
	        float hairWidth=.05f;
	        
			glutSolidSphere(hairWidth,10.0f,10.0f);
	        
	        // drawing cylinders at each particle point
	        if (drawCylinders){
	             if (i+1<numHairParticles){
	                 Vector3f pos2 = m_vVecState[2*(i+1)];
	                 float angleRad = atanf((pos[0]-pos2[0])/(pos[1]-pos2[1]));
	                 float angleDeg = angleRad * 180 / 3.1415296;
	                 glRotatef(-angleDeg,0,0,1.0f);
	                 glRotatef(90.0f,1.0f,0.0f,0.0f); //switch y and z axis since cylinders draw on the z axis
	                 GLUquadricObj *quad= gluNewQuadric();
	                 gluCylinder(quad,hairWidth,hairWidth,0.5f,32,32);
	                 glPopMatrix();}

	        }
	        
			glPopMatrix();
		}
	}
    
    if (drawGhostParticles){
        for (int j = 0; j < numGhostParticles; j++) {
            int i = j + numHairParticles;
            Vector3f pos = m_vVecState[2*i] ;//  position of particle i. YOUR CODE HERE
            glPushMatrix();
            glTranslatef(pos[0], pos[1], pos[2] );
            glutSolidSphere(0.025f,10.0f,10.0f);
            
            glPopMatrix();
        }
    }
    

	if (drawSprings) {

		for(int i=0; i < torsion_springs.size(); i++) {
	    	vector<Vector3f> s = torsion_springs[i];
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

		for(int i=0; i < ghost_edge_springs.size(); i++) {
	    	vector<Vector3f> s = ghost_edge_springs[i];

			for (int j=0; j < s.size(); j++) {
				Vector3f pos1 = m_vVecState[2*(i + numHairParticles)];
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

void PendulumSystem::showSprings() {
	if (drawSprings) {
		drawSprings = false;
	}

	else {
		drawSprings = true;
	}
}

void PendulumSystem::showGhostParticles(){
    if (drawGhostParticles) {
		drawGhostParticles = false;
	}
    
	else {
		drawGhostParticles = true;
	}
    
}

void PendulumSystem::showCylinders(){
    if (drawCylinders) {
		drawCylinders = false;
	}
    
	else {
		drawCylinders = true;
	}
    
}