#include "TimeStepper.h"
#include <iostream>

///TODO: implement Explicit Euler time integrator here
void ForwardEuler::takeStep(ParticleSystem* particleSystem, float stepSize)
{
	vector<Vector3f> new_state;
	vector<Vector3f> current_state = particleSystem->getState();
	vector<Vector3f> current_vel = particleSystem->evalF(current_state);
	for (int i = 0; i < current_state.size(); i++) {
		Vector3f s = current_state[i] + stepSize*current_vel[i];
		new_state.push_back(s);
	}

	particleSystem->setState(new_state);
}

///TODO: implement Trapzoidal rule here
void Trapzoidal::takeStep(ParticleSystem* particleSystem, float stepSize)
{
	vector<Vector3f> state1;
	vector<Vector3f> state0 = particleSystem->getState();
	vector<Vector3f> f0 = particleSystem->evalF(state0);

	for (int i = 0; i < state0.size(); i++) {
		Vector3f s = state0[i] + stepSize*f0[i];
		state1.push_back(s);
	}

	vector<Vector3f> f1 = particleSystem->evalF(state1);

	vector<Vector3f> new_state;
	for (int i=0; i < state0.size(); i++) {
		Vector3f s = state0[i] + stepSize/2 * (f0[i] + f1[i]);
		new_state.push_back(s);
	}

	particleSystem->setState(new_state);
}
