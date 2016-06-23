#pragma once
#include "IController.h"

class Ship;

struct BoidInfo
{
	float cohesion = 0;
	float separation = 1;
	float alignment = 0;
};

class BoidController : public IController
{
	std::vector<std::pair<BoidInfo, Ship*>> boids;
	std::vector<XMFLOAT3> Waypoints;
	int destWaypoint = 0;
	float radiusOfCaring = 0;
	XMFLOAT3 desiredForward;
public:
	BoidController(Ship* _slave, std::vector<Ship*>& _ships);
	~BoidController();
	void Update(float _dt);
	bool AddWaypoint(XMFLOAT3 _waypoint);
};

