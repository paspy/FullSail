#include "pch.h"
#include "Boid.h"
#include "Ship.h"


BoidController::BoidController(Ship* _slave, std::vector<Ship*>& _ships) : IController(_slave)
{
	for (auto&& ship : _ships)
	{
		if (ship != _slave)
			boids.push_back(std::pair<BoidInfo, Ship*>(BoidInfo(), ship));
	}
}


BoidController::~BoidController()
{
}

void BoidController::Update(float _dt)
{
	for (size_t i = 0; i < boids.size(); i++)
	{
		if (boids[i].second == nullptr)
		{
			boids.erase(boids.begin() + i);
			--i;
		}
	}
	if (destWaypoint > -1)
	{
		if (sqrDistanceBetween(Waypoints[destWaypoint], m_pOwner->GetPosition()) < 1.0f)
		{
			destWaypoint++;
			if (destWaypoint >= Waypoints.size())
				destWaypoint = 0;
		}

		desiredForward = Waypoints[destWaypoint] - m_pOwner->GetPosition();
		normalizeFloat3(desiredForward);
	}
	else
		desiredForward = VectorZero;

	XMFLOAT3 flockForward = VectorZero;
	size_t numCaredAbout = 0;
	for (auto&& boid : boids)
	{
		XMFLOAT3& pos = boid.second->GetPosition();
		if (sqrDistanceBetween(pos, m_pOwner->GetPosition()) < radiusOfCaring * radiusOfCaring)
		{
			BoidInfo& info = boid.first;
			XMFLOAT3& forward = boid.second->GetForward();
			flockForward += (pos * info.cohesion) - (pos * info.separation) + (forward * info.alignment);
			numCaredAbout++;
		}
	}
	if (numCaredAbout)
		flockForward /= (float)numCaredAbout;

	desiredForward += flockForward;
	m_pOwner->GetTransform().Rotate(desiredForward * _dt, true);
	m_pOwner->GetTransform().Translate(VectorZ * _dt, true);
}

bool BoidController::AddWaypoint(XMFLOAT3 _waypoint)
{
	Waypoints.push_back(_waypoint);
	return true;
}
