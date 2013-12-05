
#include "pendulumSystem.h"
#include "grid.h"
#include <iostream>

#include <math.h>

PendulumSystem::PendulumSystem(int numParticles, int howManyStrands):ParticleSystem(numParticles)
{
	//force constants
	mass = 0.02f;
	gravity = Vector3f(0.0f, -1.0f, 0.0f);
	drag_const = 0.01f;
	spring_const = 5.0f;
	less_stiff_spring_const = 1.0f;
	rest_len = 0.5f;

	//hair constants
	strand_offset = 0.2f;
	numStrands = howManyStrands;
	numHairParticles = numParticles;
	numGhostParticles = numParticles - 1;
	m_numParticles = (numHairParticles + numGhostParticles) * numStrands;
	numStrandParticles=numHairParticles+numGhostParticles;
	int rowLength=3; //for arranging hair in a grid
	float colOffset=strand_offset;

	//display options
	drawSprings = false;
    drawGhostParticles=false;
    drawCylinders=false;
    hasForce=false;

    float cellSize=0.17f;
    int grid_size = max((numParticles+howManyStrands)*rest_len, howManyStrands*strand_offset);
    this->grid = Grid(grid_size, grid_size, grid_size, cellSize);

	for (int k = 1; k <= numStrands; k++){
		//for making a grid of hairs
		int colNum=(k-1)/rowLength;
		float yPos=-1*((k-1)%rowLength)*strand_offset;
		float zPos=colNum*cellSize;

		for (int i = 0; i < numHairParticles; i++) {
			// position vector
			Vector3f pos=Vector3f((i*rest_len), yPos, zPos);
			m_vVecState.push_back(pos);
			grid.addParticle(pos, m_vVecState.size()-1);

			// velocity vector
			m_vVecState.push_back(Vector3f(0,0,0));

			// SPRINGS
			int currentParticle=(k-1)*numStrandParticles+i;
			// edge springs
			vector<Vector4f> edge;
			if ((i-1) > -1) { //if it's not the first particle, put a spring to the previous one
				edge.push_back(Vector4f(currentParticle,currentParticle-1, spring_const, rest_len)); // took out multiplying spring_const*i?
			}

			if ((i+1) < numHairParticles) { //if it's not the last particle, put a spring to the next one
				edge.push_back(Vector4f(currentParticle,currentParticle+1, spring_const, rest_len)); // same here
			}

			edge_springs.push_back(edge);

			// bending springs
			vector<Vector4f> bend;
			if ((i-2) > -1) {
				bend.push_back(Vector4f(currentParticle,currentParticle-2, less_stiff_spring_const, 2*rest_len));
			}
			if ((i+2) < numHairParticles) {
				bend.push_back(Vector4f(currentParticle,currentParticle+2, less_stiff_spring_const, 2*rest_len));
			}

			bend_springs.push_back(bend);

			// torsion springs
			vector<Vector4f> torsion;
			float hypotenuse = sqrt(pow((sqrt(3.0f)/2.0f)*rest_len, 2.0f) + pow(1.5f*rest_len, 2.0f));

			// 2nd value is index of ghost particle that hair particle i should have an edge with
			if ((i+1) < numGhostParticles) {
				torsion.push_back(Vector4f(currentParticle,currentParticle+1 + numHairParticles, less_stiff_spring_const, hypotenuse));
			}
			else {
				torsion.push_back(Vector4f(currentParticle,currentParticle-2 + numHairParticles, less_stiff_spring_const, hypotenuse));
			}

			torsion_springs.push_back(torsion);
		}

		// GHOST PARTICLES (to form triangles)
		for (int j = 0; j < numGhostParticles; j++) {
			// position vector
			m_vVecState.push_back(Vector3f((j+0.5)*rest_len, yPos, zPos+0.2f));

			// velocity vector
			m_vVecState.push_back(Vector3f(0,0,0));

			// edge springs
			int currentParticle=(k-1)*numStrandParticles+j+numHairParticles;
			vector<Vector4f> ghost_edge;
			// 2nd value is index of hair particle that ghost particle i should have an edge with
			ghost_edge.push_back(Vector4f(currentParticle,currentParticle-numHairParticles, spring_const*5, 0.2f));
			ghost_edge.push_back(Vector4f(currentParticle,currentParticle-numHairParticles+1, spring_const*5, 0.2f));

			ghost_edge_springs.push_back(ghost_edge);

			// bending springs
			vector<Vector4f> ghost_bend;
			// 2nd value is index of ghost particle that ghost particle i should have an edge with
			if ((j-1) > -1) {
				ghost_bend.push_back(Vector4f(currentParticle,currentParticle-1, less_stiff_spring_const, rest_len));
			}
			if ((j+1) < numGhostParticles) {
				ghost_bend.push_back(Vector4f(currentParticle,currentParticle+1, less_stiff_spring_const, rest_len));
			}

			ghost_bend_springs.push_back(ghost_bend);
		}

	}

	//cout << grid.printGrid() << endl;

}

vector<Vector3f> PendulumSystem::evalF(vector<Vector3f> state)
{
	// reinitialize grid
	grid.reset();
	for (int k=0; k<numStrands;k++) {
	 	for (int i=0; i<numHairParticles; i++) {			
	 		int currentIndex = i+k*numStrandParticles;
	 		Vector3f current_position = getParticlePosition(state, currentIndex);
	 		grid.addParticle(current_position, currentIndex);
	 	}
	}

	vector<Vector3f> f;
	vector<Vector3f> empty;
	for (int k=0; k<numStrands; k++){
		for (int i=0; i < numHairParticles; i++) {
			int currentIndex = i+k*numStrandParticles;
			Vector3f current_position = getParticlePosition(state, currentIndex);
			Vector3f current_velocity = getParticleVelocity(state, currentIndex);

			Vector3f force = Vector3f(0,0,0);
			if (i == 0) {
				force = Vector3f(0,0,0);
			}

			else {
				force = (mass*gravity) - drag_const*current_velocity;
				int currentSpringIndex=i+k*numHairParticles;
				// edge springs
				force += addSpringForces(state, edge_springs[currentSpringIndex], current_position);
				// bending springs
				force += addSpringForces(state, bend_springs[currentSpringIndex], current_position);
				// torsion springs

				force += addSpringForces(state, torsion_springs[currentSpringIndex], current_position);

				//check for collisions in same cell
				if (grid.numParticlesInCell(current_position) > 1) {
					// iterate through other particles in cell
					vector<int> cell = grid.getCell(current_position);
					for (int p=0; p < cell.size(); p++) {
						int new_index = cell[p];

						// add force to repel particles
						if (new_index != currentIndex && current_velocity.abs() != 0) {
							Vector3f diff = current_position - getParticlePosition(state, new_index);
							Vector3f dir = current_velocity.normalized();
							dir.negate(); // negate unit direction of velocity
							srand (time(NULL));
							float zOffset = 0.05 * ((float)rand()/RAND_MAX) + 1; // either .05 or -.05
							//dir = dir * Vector3f(1.0f,1.0f,1.0f+zOffset); //try to push it in another direction so it's not bound in 1 plane
							Vector3f repel = dir * diff.abs()* 1.0f;
							force += repel;
						}
					}
				}
			}

			f.push_back(current_velocity);
			f.push_back(force/mass);

			empty.push_back(Vector3f(0,0,0));
			empty.push_back(Vector3f(0,0,0)); // 0 force
		}

		for (int j=0; j < numGhostParticles; j++) {
			int currentIndex = j+numHairParticles+k*numStrandParticles;
			Vector3f current_position = getParticlePosition(state, currentIndex);
			Vector3f current_velocity = getParticleVelocity(state, currentIndex);

			Vector3f force = Vector3f(0,0,0);

			force = (mass*gravity) - drag_const*current_velocity;

			int currentSpringIndex=j + k*numGhostParticles;
			// edge springs
			force += addSpringForces(state, ghost_edge_springs[currentSpringIndex], current_position);
			// bending springs
			force += addSpringForces(state, ghost_bend_springs[currentSpringIndex], current_position);

			f.push_back(current_velocity);
			f.push_back(force/mass);
			
			empty.push_back(Vector3f(0,0,0));
			empty.push_back(Vector3f(0,0,0)); // 0 force

		}
	}

	if (hasForce){
		return f;
	}else{
		return empty;
	}
}

Vector3f PendulumSystem::getParticlePosition(vector<Vector3f> state, int x) {
	return state[2*x];
}

Vector3f PendulumSystem::getParticleVelocity(vector<Vector3f> state, int x) {
	return state[2*x + 1];
}

Vector3f PendulumSystem::addSpringForces(vector<Vector3f> state, vector<Vector4f> spr,
	Vector3f current_position) {
	Vector3f force;
	for (int i=0; i < spr.size(); i++) {
		Vector3f other_end = getParticlePosition(state, spr[i][1]);
		Vector3f disp = current_position - other_end;
		float k = spr[i][2];
		float rest = spr[i][3];
		force += -k*(disp.abs() - rest)*disp/disp.abs();
	}
	return force;
}

// render the system (ie draw the particles)
void PendulumSystem::draw()
{
	//grid.drawGrid();
	for (int k = 0; k < numStrands; k++){
		for (int i = 0; i < numHairParticles; i++) {
			int currentIndex=2*i+k*2*(numStrandParticles);
			Vector3f pos = m_vVecState[currentIndex];
			glPushMatrix();
			glTranslatef(pos[0], pos[1], pos[2] );
	        
	        float hairWidth=.03f;
	        
			glutSolidSphere(hairWidth,10.0f,10.0f);
	        
	        // drawing cylinders at each particle point
	        //this is ugly code i'm sorry lol
	        if (drawCylinders){
	             if (i+1<numHairParticles){
	                 Vector3f pos2 = m_vVecState[currentIndex+2];
	                 float distance = sqrt( pow(pos[0]-pos2[0],2.0f) + pow(pos[1]-pos2[1],2.0f) + pow(pos[2]-pos2[2],2.0f));
	                 //rotate in xy plane
	                 float opp=pos[0]-pos2[0];
	                 float adj=pos[1]-pos2[1];
	                 float angleRad = atanf(opp/adj);
	                 float angleDeg = angleRad * 180 / 3.1415296;
	                 if (opp>0 && adj < 0){ //4th quad, add 270
	                 	angleDeg += 270;
	                 }else if (opp<0 && adj < 0){ //3rd quad, add 180
	                 	angleDeg += 180;
	                 }
	                 glRotatef(-angleDeg,0,0,1.0f);

	                 //rotate in yz plane
	                 if (abs(pos[2]-pos2[2])>hairWidth/4.0f){
	                 	//cout << "point "<<endl;
	                 	//pos.print();
	                 	//pos2.print();
		                 opp=pos[2]-pos2[2];
		                 adj=pos[1]-pos2[1];
		                 angleRad = atanf(opp/adj);
		                 angleDeg = angleRad * 180 / 3.1415926;
		                 if (opp>0 && adj < 0){ //4th quad, add 270
		                 	angleDeg += 270;
		                 	//cout << "4th quad " << angleDeg << endl;
		                 }else if (opp<0 && adj < 0){ //3rd quad, add 180
		                 	angleDeg += 90;
		                 	//cout << "3rd quad " << angleDeg << endl;
		                 }
		                 glRotatef(angleDeg,1.0f,0,0);
	             		}	
	                 glRotatef(90.0f,1.0f,0.0f,0.0f); //switch y and z axis since cylinders draw on the z axis
	                 GLUquadricObj *quad= gluNewQuadric();
	                 gluCylinder(quad,hairWidth,hairWidth,distance,32,32);
	                 glPopMatrix();}

	        }
	        
			glPopMatrix();
		}
	}
    
    if (drawGhostParticles){
    	for (int k=0; k<numStrands; k++){
    		for (int j = 0; j < numGhostParticles; j++) {
	            int i = j + numHairParticles;
	            int currentIndex= 2*(j+numHairParticles) + k*2*(numStrandParticles);
	            Vector3f pos = m_vVecState[currentIndex] ;
	            glPushMatrix();
	            glTranslatef(pos[0], pos[1], pos[2] );
	            glutSolidSphere(0.025f,10.0f,10.0f);
	            
	            glPopMatrix();
	        }
    	}

    }
    

	if (drawSprings) {
		drawSpring(torsion_springs);
		drawSpring(ghost_edge_springs);
		drawSpring(ghost_bend_springs);
		drawSpring(edge_springs);
		drawSpring(bend_springs);

	}
}

void PendulumSystem::drawSpring(vector<vector<Vector4f>> springs){
	for(int i=0; i < springs.size(); i++) {
    	vector<Vector4f> s = springs[i];
		for (int j=0; j < s.size(); j++) {
			Vector3f pos1 = m_vVecState[2*s[j][0]];
			Vector3f pos2 = m_vVecState[2*s[j][1]];

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

void PendulumSystem::addForce(){
    if (hasForce) {
		hasForce = false;
	}
    
	else {
		hasForce = true;
	}
    
}
