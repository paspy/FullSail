#include "pch.h"
#include "Squad.h"
#include "Math.h"
#include "IBoid.h"
#include "CSteeringManger.h"

Squad::Squad()
{
	m_nCurrentWaypointIndex = 0;
}

Squad::Squad(const Squad & other)
{
	*this = other;
}

Squad & Squad::operator=(const Squad & other)
{
	//if im looking at myslef
	if (&other == this)
	{
		//return myself
		return *this;
	}

	//assigning data to this variables
	m_nCurrentWaypointIndex = 0;
	m_pLeader = other.m_pLeader;
	m_vWaypoints = other.m_vWaypoints;
	m_vSquadmates.clear();
	AddSquad(static_cast<std::vector<Boid*>>(other.m_vSquadmates));

	return *this;
}

bool Squad::operator==(const Squad & other)
{

	const std::vector<Boid*>& otherSquad = other.m_vSquadmates;

	//if myself or the other Squad is empty
	if (m_vSquadmates.empty() || otherSquad.empty())
	{
		//we are not equal
		return false;
	}

	unsigned int MaxSquadMates = (UINT)otherSquad.size();

	//if we do not have the same number of boids
	if (MaxSquadMates != m_vSquadmates.size())
	{
		//we are not equal
		return false;
	}

	for (unsigned int i = 0; i < MaxSquadMates; i++)
	{
		//if one pointer doesnt point at the same boid 
		if (m_vSquadmates[i] != otherSquad[i])
		{
			//we are not equal
			return false;
		}
	}

	return true;
}

void Squad::SetLeader(Boid * newLeader)
{
	m_pLeader = newLeader;
}

Boid * Squad::GetLeader()
{
	return m_pLeader;
}

float Squad::GetRadius()
{
	float SquadRadius = 0;

	 unsigned int maxSize = (UINT)m_vSquadmates.size();
	 XMFLOAT3 AveragePosition  = m_vSquadmates[0]->GetAveragePosition();

	 //Getting the distance from the first position
	 SquadRadius = sqrDistanceBetween(AveragePosition,m_vSquadmates[0]->GetPosition());

	 for (unsigned int i = 1; i < maxSize; i++)
	 {
		 //the distance between the average position and the squadmate position
		 float newDistance = sqrDistanceBetween(AveragePosition, m_vSquadmates[i]->GetPosition());

		 //the newDistance is greater thena the Radius
		 if (SquadRadius < newDistance)
		 {
			 //set Radius equal to the distance
			 SquadRadius = newDistance;
		 }
	 }

	return SquadRadius;
}

DirectX::XMFLOAT3 Squad::GetPosition()
{
	return m_vSquadmates[0]->GetAveragePosition();
}

void Squad::AddSquadmate(Boid * newMate)
{
	m_vSquadmates.push_back(newMate);
	newMate->SetSquad(this);
	newMate->GetSteeringVector().push_back(&CSteeringManger::SquadronFlocking);
}

//updating the waypoint index
void Squad::UpdateWaypointIndex()
{
	//increment the index
		//mod it by the maxSize
	m_nCurrentWaypointIndex = (++m_nCurrentWaypointIndex % m_vWaypoints.size());

	fsEventManager::GetInstance()->FireEvent(fsEvents::BossTracking, &EventArgs1<Squad*>(this));
	if (m_nCurrentWaypointIndex < 0 || m_nCurrentWaypointIndex >= m_vWaypoints.size())
		m_nCurrentWaypointIndex = 0;
}

void Squad::AddSquad(std::vector<Boid*>& wholeSquad)
{
	UINT squadSize = (unsigned int)wholeSquad.size();

	for (unsigned int i = 0; i < squadSize; i++)
	{
		AddSquadmate(wholeSquad[i]);
	}
}

void Squad::RemoveSquadMate(Boid * squadmate)
{
	unsigned int  vectorSize = (UINT)m_vSquadmates.size();

	//loop through all boids
	for (unsigned int i = 0; i < vectorSize; i++)
	{
		//if I find one that matches the parameters
		if (m_vSquadmates[i] == squadmate)
		{
			//erase it
			m_vSquadmates.erase(m_vSquadmates.begin() + i);
			return;
		}
	}
}

void Squad::RemoveSquadMate(unsigned int index)
{
	if (index >= m_vSquadmates.size())
	{
		return;
	}

	//Removing the boid from the squadmate vector
	m_vSquadmates.erase(m_vSquadmates.begin() + index);
}

void Squad::AddWaypoint(DirectX::XMFLOAT3 & newPoint)
{
	//adding waypoints
	m_vWaypoints.push_back(newPoint);

	//setting the current Waypoint
	if (m_vWaypoints.size() == 1)
	{
		m_nCurrentWaypointIndex = 0;


		unsigned int MaxSquad = (unsigned int)m_vSquadmates.size();

		//looping through all of the squads
		for (unsigned int i = 0; i < MaxSquad; i++)
		{
			//setting the target
			m_vSquadmates[i]->SetTarget(m_vWaypoints[0]);
		}
	}
}

void Squad::AddWaypoints(std::vector<DirectX::XMFLOAT3>& points)
{
	//Clear the vector
	m_vWaypoints.clear();

	//Assigning the waypoints to the new waypoints being passed in
	m_vWaypoints = points;

	//setting the currentWaypoint
	m_nCurrentWaypointIndex = 0;

	unsigned int MaxSquad = (unsigned int)m_vSquadmates.size();

	//looping through all of the squads
	for (unsigned int i = 0; i < MaxSquad; i++)
	{
		//setting the target
		m_vSquadmates[i]->SetTarget(m_vWaypoints[0]);
	}
}

void Squad::RemoveWaypoint(DirectX::XMFLOAT3 & waypoint)
{
	unsigned int  vectorSize = (UINT)m_vWaypoints.size();

	//loop through all wapoint
	for (unsigned int i = 0; i < vectorSize; i++)
	{
		//if i find one that matches the parameters
		if (m_vWaypoints[i] == waypoint)
		{
			//erase it
			m_vWaypoints.erase(m_vWaypoints.begin() + i);
		}
	}
}

void Squad::RemoveWaypoint(unsigned int index)
{
	if (index >= m_vWaypoints.size())
	{
		return;
	}

	//Removing the boid from the waypoint vector
	m_vWaypoints.erase(m_vWaypoints.begin() + index);
}

bool Squad::AdvanceToNextPoint(Boid* squadMate)
{
	unsigned int WaypointSize = (unsigned int)m_vWaypoints.size();

	//if I have no waypoints
	if (WaypointSize == 0)
	{
		return false;
	}

	unsigned int size = (UINT)m_vSquadmates.size();

	//loop through all of the squads
	for (unsigned int i = 0; i < size; i++)
	{
		//Check to find one boid that is not at the current waypoint
		if (m_vSquadmates[i]->AtWaypoint() == false)
		{
			squadMate->SetTarget(m_vWaypoints[m_nCurrentWaypointIndex]);
			//if so return false

			return false;
		}
	}
	//return true if all of the boids are at the waypoint
		//increment the index
	UpdateWaypointIndex();
	
	if (m_nCurrentWaypointIndex >= (int)WaypointSize)
	{
		return false;
	}

	squadMate->SetTarget(m_vWaypoints[m_nCurrentWaypointIndex]);
	return true;
}

std::vector<Boid*>& Squad::GetSquad()
{
	return m_vSquadmates;
}

std::vector<DirectX::XMFLOAT3>& Squad::GetWaypoints()
{
	return m_vWaypoints;
}

DirectX::XMFLOAT3 Squad::GetCurrentWaypoint()
{
	if (m_nCurrentWaypointIndex >= m_vWaypoints.size() || m_nCurrentWaypointIndex < 0)
		return DirectX::XMFLOAT3(0.0f, FLT_MAX, 0.0f);
	return m_vWaypoints[m_nCurrentWaypointIndex];
}

int Squad::GetCurrentWaypointIndex()
{
	return m_nCurrentWaypointIndex;
}

Squad::~Squad()
{

	//Containers
	m_vSquadmates.clear();
	m_vWaypoints.clear();

}
