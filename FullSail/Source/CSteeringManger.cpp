#include "pch.h"
#include "CSteeringManger.h"
#include "IBoid.h"
#include "Math.h"
#include "Squad.h"

#define MAX_SQUAD_SIZE 4

CSteeringManger CSteeringManger::m_cInstance;

CSteeringManger::CSteeringManger()
{
	//Registering for events

	//Adding Boids
	fsEventManager::GetInstance()->RegisterClient(fsEvents::AddSquadmate, this, &CSteeringManger::AddToSquad);
	fsEventManager::GetInstance()->RegisterClient(fsEvents::FollowLeader, this, &CSteeringManger::AddFollower);

	//Adding Squad
	fsEventManager::GetInstance()->RegisterClient(fsEvents::AddSquad, this, &CSteeringManger::AddSquad);

	//Removing Squad
	fsEventManager::GetInstance()->RegisterClient(fsEvents::RemoveSquad, this, &CSteeringManger::RemoveSquad);

	//Removing Boids
	fsEventManager::GetInstance()->RegisterClient(fsEvents::StopFollowing, this, &CSteeringManger::StopFollowing);
	fsEventManager::GetInstance()->RegisterClient(fsEvents::LeaveSquad, this, &CSteeringManger::RemoveSquadMate);

	//Global awareness of each boid
	fsEventManager::GetInstance()->RegisterClient(fsEvents::AddBoid, this, &CSteeringManger::AddBoid);
	fsEventManager::GetInstance()->RegisterClient(fsEvents::DeleteBoid, this, &CSteeringManger::DeleteBoid);

}

CSteeringManger::~CSteeringManger()
{
	m_mpLeader.clear();
	m_vWaypointContainer.clear();
	m_mpSquadron.clear();
	m_vKnownBoids.clear();

	//Adding Boids
	fsEventManager::GetInstance()->UnRegisterClient(fsEvents::AddSquadmate, this, &CSteeringManger::AddToSquad);
	fsEventManager::GetInstance()->UnRegisterClient(fsEvents::FollowLeader, this, &CSteeringManger::AddFollower);

	//Removing Boids
	fsEventManager::GetInstance()->UnRegisterClient(fsEvents::StopFollowing, this, &CSteeringManger::StopFollowing);
	fsEventManager::GetInstance()->UnRegisterClient(fsEvents::LeaveSquad, this, &CSteeringManger::RemoveSquadMate);

	//Global awareness of each boid
	fsEventManager::GetInstance()->UnRegisterClient(fsEvents::AddBoid, this, &CSteeringManger::AddBoid);
	fsEventManager::GetInstance()->UnRegisterClient(fsEvents::DeleteBoid, this, &CSteeringManger::DeleteBoid);

	//Adding Squad
	fsEventManager::GetInstance()->UnRegisterClient(fsEvents::AddSquad, this, &CSteeringManger::AddSquad);

	//Removing Squad
	fsEventManager::GetInstance()->UnRegisterClient(fsEvents::RemoveSquad, this, &CSteeringManger::RemoveSquad);


}

CSteeringManger& CSteeringManger::SteeringManger()
{
	return m_cInstance;
}

Squad* CSteeringManger::GetSquad(Squad & finder)
{
	//finding the end point
	std::map<unsigned int, Squad>::iterator end = m_mpSquadron.end();
	//find the start point
	std::map<unsigned int, Squad>::iterator SquadIter = m_mpSquadron.begin();

	//loop until you get to the endpoint
	for (; SquadIter != end; SquadIter++)
	{
		//if they are equal 
		if (SquadIter->second == finder)
		{
			//return the address correct squad
			return &SquadIter->second;
		}
	}

	return nullptr;
}

//************************EventHandlers**************************//

void CSteeringManger::AddToSquad(const EventArgs1<Boid**>& newEvent)
{
	//Looping through the map of vectors
	for (unsigned int index = 0; ; index++)
	{

		Squad& SteeringSquad = m_mpSquadron[index];

		//Grabbing the references to vector of boids
		std::vector<Boid*>& vecBoid = SteeringSquad.GetSquad();

		//Check to see if there is room
		if (vecBoid.size() < MAX_SQUAD_SIZE)
		{
			SteeringSquad.AddSquadmate(*newEvent.m_Data);
			//break out of the loop
			break;
		}
	}
}

void CSteeringManger::AddSquad(const EventArgs1<Squad*>& newEvent)
{
	//Looping through the map of vectors
	for (unsigned int index = 0; ; index++)
	{
		Squad& SteeringSquad = m_mpSquadron[index];

		// this group has people dont add to this squad
		if (SteeringSquad.GetSquad().empty() == false)
		{
			continue;
		}

		//Grabbing the references to vector of boids
		std::vector<Boid*>& vecBoid = SteeringSquad.GetSquad();

		SteeringSquad = (*newEvent.m_Data);

		break;

	}
}

void CSteeringManger::RemoveSquad(const EventArgs1<Squad*>& newEvent)
{
	//Looping through the map of vectors
	for (unsigned int index = 0; ; index++)
	{
		Squad& SteeringSquad = m_mpSquadron[index];


		if (SteeringSquad == *newEvent.m_Data)
		{
			m_mpSquadron[index] = Squad();
		}

	}
}

void CSteeringManger::AddFollower(const EventArgs2<Boid*, Boid**>& newEvent)
{
	Boid*  Leader = newEvent.m_Data1;
	Boid*  Follower = *newEvent.m_Data2;

	//if one of these are null do not go in
	if (Follower == nullptr || Leader == nullptr)
	{
		return;
	}

	//setting the leader for the follower
	if (Follower->GetSquad() == nullptr)
	{
		Follower->SetSquad(&m_mpLeader[newEvent.m_Data1]);
		Follower->SetLeader(newEvent.m_Data1);

		//Adding the leader
		m_mpLeader[newEvent.m_Data1].AddSquadmate(*newEvent.m_Data2);
	}
}

void CSteeringManger::RemoveSquadMate(const EventArgs2<int, Boid*>& newEvent)
{
	//grabbing the info before hand
	Boid* RemoveBoid = newEvent.m_Data2;

	//Getting the vector from that container
	std::vector<Boid*>& squadHolder = m_mpSquadron[newEvent.m_Data1].GetSquad();

	//gettin amount of boids in that vector
	unsigned int squadCount = (unsigned int)squadHolder.size();

	//Looping through the squad
	for (unsigned int i = 0; i < squadCount; i++)
	{
		//to find the boid that wants to leave th squad
		if (squadHolder[i] == RemoveBoid)
		{
			//moving the iterator to the index of the removed SquadMate
			squadHolder.erase(squadHolder.begin() + i);
		}
	}
}

void CSteeringManger::StopFollowing(const EventArgs2<Boid*, Boid*>& newEvent)
{

	//grabbing the info before hand
	Boid* RemoveBoid = newEvent.m_Data2;

	//Getting the vector from that container
	std::vector<Boid*>& squadHolder = m_mpLeader[newEvent.m_Data1].GetSquad();

	//gettin amount of boids in that vector
	unsigned int squadCount = (unsigned int)squadHolder.size();

	//Looping through the squad
	for (unsigned int i = 0; i < squadCount; i++)
	{
		//to find the boid that wants to leave the squad
		if (squadHolder[i] == RemoveBoid)
		{
			//moving the iterator to the index of the removed SquadMate
			squadHolder.erase(squadHolder.begin() + i);
		}
	}
}

void CSteeringManger::AddBoid(const EventArgs1<Boid*>& newEvent)
{

	//Check if its nullptr
	if (newEvent.m_Data == nullptr)
	{
		return;
	}

	//adds the fucntion to the list of the objects
	newEvent.m_Data->GetSteeringVector().push_back(&CSteeringManger::GlobalSeperation);

	//adding it to the vector of boids
	m_vKnownBoids.push_back(newEvent.m_Data);

}

void CSteeringManger::DeleteBoid(const EventArgs1<Boid*>& newEvent)
{
	//Holding on to a reference for it
	Boid*  RemovalBoid = newEvent.m_Data;

	//Finding the size 
	unsigned int size = (unsigned int)m_vKnownBoids.size();

	// looping through all of the boids 
	for (unsigned int i = 0; i < size; i++)
	{
		// if the boid is equal to the subindexed Boid
		if (m_vKnownBoids[i] == RemovalBoid)
		{
			//erase it at tha index
			m_vKnownBoids.erase(m_vKnownBoids.begin() + i);
			break;
		}
	}
}

//************************Flocking******************************//

DirectX::XMFLOAT3 CSteeringManger::Cohesion(Boid * _boid)
{
	XMFLOAT3 Steering = _boid->GetAveragePosition() - _boid->GetPosition();

	float length = magnitudeOf(Steering);

	if (length > 1)
	{
		normalizeFloat3(Steering);
	}

	return Steering;
}

DirectX::XMFLOAT3 CSteeringManger::Seperation(Boid * _boid, const std::vector<Boid*>& boidHolder)
{
	XMFLOAT3 Steering = VectorZero;

	for each (Boid* indexBoid in boidHolder)
	{
		if (_boid == indexBoid)
		{
			continue;
		}

		if (_boid->GetTargetBoid() == indexBoid)
		{
			continue;
		}

		XMFLOAT3 toMyself = _boid->GetPosition() - indexBoid->GetPosition();
		float length = magnitudeOf(toMyself);

		const float& CombinedRadius = _boid->GetFlockRadius() + indexBoid->GetFlockRadius() + 2;

		if (length < CombinedRadius)
		{
			normalizeFloat3(toMyself);
			//toMyself = toMyself * ((CombinedRadius - length) / CombinedRadius);

			Steering += toMyself;
		}
	}

	//normalize if nessecary
	if (magnitudeOf(Steering) > 1)
	{
		normalizeFloat3(Steering);
	}

	return Steering;
}

DirectX::XMFLOAT3 CSteeringManger::SquadSeperation(Boid * _boid)
{
	XMFLOAT3 Steering = VectorZero;

	for (std::map<unsigned int, Squad>::iterator iter = m_mpSquadron.begin(); iter != m_mpSquadron.end(); iter++)
	{
		Squad& containedSquad = iter->second;

		if (*_boid->GetSquad() == containedSquad)
		{
			continue;
		}

		for each (Boid* indexBoid in containedSquad.GetSquad())
		{
			if (_boid == indexBoid)
			{
				continue;
			}

			XMFLOAT3 toMyself = _boid->GetPosition() - indexBoid->GetPosition();
			float length = magnitudeOf(toMyself);

			const float& CombinedRadius = _boid->GetFlockRadius() + indexBoid->GetFlockRadius();

			if (length < CombinedRadius)
			{
				normalizeFloat3(toMyself);
				toMyself = toMyself * ((CombinedRadius - length) / CombinedRadius);

				Steering += toMyself;
			}
		}

	}

	//normalize if nessecary
	if (magnitudeOf(Steering) > 1)
	{
		normalizeFloat3(Steering);
	}

	return Steering;
}

DirectX::XMFLOAT3 CSteeringManger::Alignment(Boid * _boid)
{

	if (_boid->GetMaxSpeed() == nullptr)
	{
		return VectorZero;
	}

	XMFLOAT3 Steering = _boid->GetAverageForward();
	//getting the reciprocal
	float ScalingSpeed = 1 / *_boid->GetMaxSpeed();
	//multiplying by the reciprocal
	Steering *= ScalingSpeed;

	if (magnitudeOf(Steering) > 1)
	{
		normalizeFloat3(Steering);
	}
	return Steering;
}

void CSteeringManger::SetUpSquadWaypoints(Squad & newSquad, unsigned int MaxWaypoints)
{
	unsigned int  maxWaypoints = (unsigned int)m_vWaypointContainer.size();

	unsigned int newIndex = rand() % maxWaypoints;
	newSquad.AddWaypoint(m_vWaypointContainer[newIndex]);

	for (unsigned int i = 1; i < MaxWaypoints; i++)
	{
		newIndex = rand() % maxWaypoints;

		if (m_vWaypointContainer[newIndex] == newSquad.GetWaypoints()[i - 1])
		{
			--i;
			continue;
		}

		// push back at that index
		newSquad.AddWaypoint(m_vWaypointContainer[newIndex]);
	}
}

DirectX::XMFLOAT3 CSteeringManger::GlobalSeperation(Boid * _boid)
{

	float3  seperation = VectorZero;

	float& weight = _boid->GetGlobalSeperation();

	if (weight <= 0)
	{
		return seperation;
	}

	seperation += Seperation(_boid, m_vKnownBoids) * weight;

	return seperation;
}

DirectX::XMFLOAT3 CSteeringManger::FollowingFlocking(Boid * _boid)
{
	XMFLOAT3& FlockingVector = VectorZero;

	float weight;
	//Flocking 
	//checking  cohesion
	weight = _boid->GetLeaderCohesion();

	if (weight > 0)
	{
		FlockingVector += Cohesion(_boid) * weight;
	}

	//checking alignment
	weight = _boid->GetLeaderAlignment();
	if (weight > 0)
	{
		FlockingVector += Alignment(_boid) * weight;
	}

	//checking seperation
	weight = _boid->GetLeaderSeperation();

	if (weight > 0)
	{
		FlockingVector += Seperation(_boid, *_boid->GetSquadVector()) * weight;
	}

	return FlockingVector;
}

DirectX::XMFLOAT3 CSteeringManger::SquadronFlocking(Boid * _boid)
{
	XMFLOAT3& FlockingVector = VectorZero;


	float weight;
	//FLocking 
	//checking  cohesion
	weight = _boid->GetSquadCohesion();

	if (weight > 0)
	{
		FlockingVector += Cohesion(_boid) * weight;
	}

	//checking alignment
	weight = _boid->GetSquadAlignment();
	if (weight > 0)
	{
		FlockingVector += Alignment(_boid) * weight;
	}

	//checking seperation
	weight = _boid->GetSquadSeperation();

	if (weight > 0)
	{
		FlockingVector += SquadSeperation(_boid) * weight;
	}

	return FlockingVector;
}

DirectX::XMFLOAT3 CSteeringManger::Pursuit(Boid * _boid)
{
	XMFLOAT3 steering = VectorZero;



	return steering * _boid->GetPursuitWeight();
}

DirectX::XMFLOAT3 CSteeringManger::Evade(Boid * _boid)
{
	XMFLOAT3 steering = VectorZero;



	return steering * _boid->GetEvadeWeight();
}

DirectX::XMFLOAT3 CSteeringManger::Seek(Boid * _boid)
{
	XMFLOAT3 steering = VectorZero;
	//seekweight is 0 
	if (_boid->GetSeekWeight() == 0)
	{
		//return vector 0
		return steering;
	}

	steering = _boid->GetTarget() - _boid->GetPosition();

	normalizeFloat3(steering);

	return steering * _boid->GetSeekWeight();
}

void CSteeringManger::SetWaypoint(const std::vector<DirectX::XMFLOAT3>& waypoints)
{
	m_vWaypointContainer = waypoints;
}

void CSteeringManger::AddWaypoint(const DirectX::XMFLOAT3 & waypoint)
{
	m_vWaypointContainer.push_back(waypoint);
}

bool CSteeringManger::RemoveBoid(Boid * _boid)
{
	const unsigned int& size = (unsigned int)m_vKnownBoids.size();

	for (unsigned int i = 0; i < size; i++)
	{
		if (m_vKnownBoids[i] == _boid)
		{
			m_vKnownBoids.erase(m_vKnownBoids.begin() + i);
			return true;
		}
	}

	return false;
}

bool CSteeringManger::AddBoid(Boid * _boid)
{
	//checking for the KnownBoid
	for each (Boid* indexBoid in m_vKnownBoids)
	{
		if (_boid == indexBoid)
		{
			return false;
		}

	}
	m_vKnownBoids.push_back(_boid);
	return true;
}
