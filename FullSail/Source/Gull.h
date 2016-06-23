#pragma once
#include "Entity3D.h"
#include "Ship.h"

class Gull :
	public Entity3D
{
private:
	float maxSpeed;
	float maxTurn;
	float cohesion;
	float separation;
	float alignment;
	float scohesion;
	float sseparation;
	float salignment;
public:
	Gull(XMFLOAT4X4& _transform);
	~Gull();

	void Update(float _dt, std::vector<Gull*>& boids, std::vector<Ship*>& ships);

	void FlyForward(float _dt, float _speed);
	void Turn(float _dt, float _speed);
};

