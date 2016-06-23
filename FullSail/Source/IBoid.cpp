#include "pch.h"
#include "IBoid.h"
#include "Math.h"
#include "Entity3D.h"
#include "CSteeringManger.h"
#include "Squad.h"

#define WAYPOINT_DISTANCE_LIMIT 7.5f

Boid::Boid()
{
	m_pSquad = nullptr;
	m_bGuidance = false;
	m_vtTarget = VectorZero;
	m_vtAveragePosition = VectorZero;
	m_vtAverageForward = VectorZero;

	m_fStartingLeaderCohesionWeight = m_fLeaderCohesionWeight = 10;
	m_fStartingLeaderAlignmentWeight = m_fLeaderAlignmentWeight = 10;
	m_fStartingLeaderSeperationWeight = m_fLeaderSeperationWeight = 10;

	m_fStartingSquadCohesionWeight = m_fSquadCohesionWeight = 1.0f;
	m_fStartingSquadAlignmentWeight = m_fSquadAlignmentWeight = 5;
	m_fStartingSquadSeperationWeight = m_fSquadSeperationWeight = 4;

	m_fStartingPursuitWeight = m_fPursuitWeight = 10;
	m_fStartingEvadeWeight = m_fEvadeWeight = 10;
	m_fStartingFleeWeight = m_fFleeWeight = 10;
	m_fStartingSeekWeight = m_fSeekWeight = 5;

	m_fStartingFlockRadius = m_fFlockRadius = 4;
	m_fStartingGlobalSeperation = m_fGlobalSeperationWieght = 10;
	//notifying the SteeringManager about me
	fsEventManager::GetInstance()->FireEvent(fsEvents::AddBoid, &EventArgs1<Boid*>(this));
	m_vSteeringFunctions.push_back(&CSteeringManger::Seek);
}

bool Boid::AtWaypoint()
{
	float distance = sqrMagnitudeOf(m_vtTarget - m_pOwner->GetPosition());

	if (distance < WAYPOINT_DISTANCE_LIMIT * WAYPOINT_DISTANCE_LIMIT)
	{
		return true;
	}

	return false;
}

void Boid::EnableGuidance(bool guide)
{
	m_bGuidance = guide;
}

void Boid::CalculateAverages()
{
	m_vtAveragePosition = VectorZero;
	m_vtAverageForward = VectorZero;

	if (m_vSquad == nullptr)
	{
		return;
	}

	//getting a vector
	std::vector<Boid*>& Squad = (*m_vSquad);

	//gettign the max
	float MaxCount = (float)Squad.size();

	if (MaxCount == 1)
	{
		m_vtAveragePosition = m_pOwner->GetPosition();
		m_vtAverageForward = m_pOwner->GetForward();
		return;
	}

	//find the averages
	for each (Boid* boid in Squad)
	{
		//Do not want get an owner with nullptr
		if (boid->m_pOwner == nullptr || boid->m_bGuidance == false)
		{
			MaxCount -= 1;
			continue;
		}

		//Owner
		Entity3D* object = boid->m_pOwner;

		//find the sum of the position and forward


		m_vtAveragePosition += object->GetPosition();


		m_vtAverageForward += object->GetForward();
	}

	//Averaging the Position and Forward
	m_vtAveragePosition = m_vtAveragePosition / MaxCount;
	m_vtAverageForward = m_vtAverageForward / MaxCount;
}

void Boid::SquadCommands()
{
	//if guidance is enabled 
	if (m_bGuidance == true)
	{
		//check to see if you should go to the nextWaypoint
		if (m_vSquad->empty() == true)
		{
			FS_PRINT_OUTPUT("THIS IS GOING TO CRASH SOON");
			return;
		}

		if (m_pSquad)
		{
			m_pSquad->AdvanceToNextPoint(this);
		}
	}

}

DirectX::XMFLOAT3 Boid::Update()
{
	//early out
	if (m_pOwner == nullptr)
	{
		return VectorZero;
	}

	SquadCommands();
	//Creating the Averages
	CalculateAverages();

	//Getting the result of all of the steering functions
	XMFLOAT3&  myforward = RunSteeringFunctions();

	return myforward;
}

DirectX::XMFLOAT3 Boid::RunSteeringFunctions()
{
	//return value
	XMFLOAT3 SteeringResult = VectorZero;

	//find the size of the vector
	unsigned int maxFunctions = (unsigned int)m_vSteeringFunctions.size();

	CSteeringManger& Steering = CSteeringManger::SteeringManger();

	//looping through all of the functions
	for (unsigned int i = 0; i < maxFunctions; i++)
	{
		SteeringResult += (Steering.*m_vSteeringFunctions[i])(this);
	}

	return SteeringResult;
}

DirectX::XMFLOAT3 Boid::FindingAveragePosition()
{
	return DirectX::XMFLOAT3();
}

#pragma region Accessor & Mutator

std::vector<SteeringFunction>& Boid::GetSteeringVector()
{
	return m_vSteeringFunctions;
}

Entity3D * Boid::GetOwner()
{
	return m_pOwner;
}

const DirectX::XMFLOAT3& Boid::GetTarget()
{
	return m_vtTarget;
}

const Boid * Boid::GetLeader()
{
	return m_pLeader;
}

float & Boid::GetLeaderAlignment()
{
	return m_fLeaderAlignmentWeight;
}

float & Boid::GetLeaderCohesion()
{
	return m_fLeaderCohesionWeight;
}

DirectX::XMFLOAT3 & Boid::GetAverageForward()
{
	return m_vtAverageForward;
}

DirectX::XMFLOAT3 & Boid::GetAveragePosition()
{
	return m_vtAveragePosition;
}

void Boid::SetTargetBoid(Boid * newTarget)
{
	m_pBoidTarget = newTarget;
}

Boid * Boid::GetTargetBoid()
{
	return m_pBoidTarget;
}

bool & Boid::GetGuidance()
{
	return m_bGuidance;
}

float & Boid::GetFlockRadius()
{
	return m_fFlockRadius;
}

float & Boid::GetMaxCombinedRadius()
{
	return m_fMaxCombinedRadius;
}

float* Boid::GetMaxSpeed()
{
	return m_pMaxSpeed;
}

float & Boid::GetLeaderSeperation()
{
	return m_fLeaderSeperationWeight;
}

float & Boid::GetSquadAlignment()
{
	return m_fSquadAlignmentWeight;
}

float & Boid::GetSquadCohesion()
{
	return m_fSquadCohesionWeight;
}

float & Boid::GetSquadSeperation()
{
	return m_fSquadSeperationWeight;
}

float & Boid::GetPursuitWeight()
{
	return m_fPursuitWeight;
}

float & Boid::GetEvadeWeight()
{
	return m_fEvadeWeight;
}

float & Boid::GetFleeWeight()
{
	return m_fFleeWeight;
}

float & Boid::GetSeekWeight()
{
	return m_fSeekWeight;
}

float & Boid::GetGlobalSeperation()
{
	return m_fGlobalSeperationWieght;
}

void Boid::ResetWeights()
{
	m_fLeaderCohesionWeight = m_fStartingLeaderCohesionWeight;
	m_fLeaderAlignmentWeight = m_fStartingLeaderAlignmentWeight;
	m_fLeaderSeperationWeight = m_fStartingLeaderSeperationWeight;

	m_fSquadCohesionWeight = m_fStartingSquadCohesionWeight;
	m_fSquadAlignmentWeight = m_fStartingSquadAlignmentWeight;
	m_fSquadSeperationWeight = m_fStartingSquadSeperationWeight;

	m_fPursuitWeight = m_fStartingPursuitWeight;
	m_fEvadeWeight = m_fStartingEvadeWeight;
	m_fFleeWeight = m_fStartingFleeWeight;
	m_fSeekWeight = m_fStartingSeekWeight;

	m_fFlockRadius = m_fStartingFlockRadius;
	m_fMaxCombinedRadius = m_fStartingMaxCombinedRadius;
	m_fGlobalSeperationWieght = m_fStartingGlobalSeperation;
}

DirectX::XMFLOAT3 Boid::GetPosition()
{
	return m_pOwner->GetPosition();
}

DirectX::XMFLOAT3 Boid::GetRight()
{
	return m_pOwner->GetRight();
}

DirectX::XMFLOAT3 Boid::GetForward()
{
	return m_pOwner->GetForward();
}

void Boid::SetOwner(Entity3D * newOwner)
{
	if (newOwner == nullptr || m_pOwner != nullptr)
	{
		return;
	}

	//Setting the owner
	m_pOwner = newOwner;
}

void Boid::SetTarget(DirectX::XMFLOAT3 newTarget)
{
	//Setting the target
	m_vtTarget = newTarget;
}

void Boid::SetSquad(Squad* squadmates)
{
	m_pSquad = squadmates;
	m_vSquad = &m_pSquad->GetSquad();
	//check to see if you should go to the nextWaypoint
	if (m_vSquad->empty() == true)
	{
		FS_PRINT_OUTPUT("THIS IS GOING TO CRASH SOON");
	}
	if (m_pSquad->GetWaypoints().empty() == false)
	{
		SetTarget(m_pSquad->GetWaypoints()[0]);
	}
	m_bGuidance = true;
}

void Boid::SetLeader(Boid * newLeader)
{
	if (newLeader == nullptr)
	{
		return;
	}

	//Setting the leader
	m_pLeader = newLeader;
}

#pragma endregion

Boid::~Boid()
{
	if (m_pSquad)
	{
		m_pSquad->RemoveSquadMate(this);
		m_pSquad = nullptr;
		m_vSquad = nullptr;
	}

	m_vSteeringFunctions.clear();

	fsEventManager::GetInstance()->FireEvent(fsEvents::DeleteBoid, &EventArgs1<Boid*>(this));
}