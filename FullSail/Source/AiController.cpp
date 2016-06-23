#include "pch.h"
#include "PlayerShip.h"
#include "Game.h"
#include "Colliders.h"
#include "AiController.h"
#include "GameplayScene.h"
#include "CollisionManager.h"
#include "SideCannonHandler.h"
#include "CSteeringManger.h"
#include "IBoid.h"
#include "Squad.h"

#if _DEBUG
#include <RendererController.h>
#endif

using namespace DirectX;

#define WAYPOINT_LIMIT 4
#define ALIGNMENT_LIMIT 2

#define DISTANCE_TO_TARGET 200.0f
#define WAYPOINT_DISTANCE_LIMIT 7.5f
#define FLEE_DISTANCE 64
#define DISTANCE_ALERT 500

//SETTING A STATIC POINTER
//NEVER KNEW you had to do THIS
//Transform*	  AiController::m_StaticTargetTransform = nullptr;
//Ship*		  AiController::m_StaticTarget = nullptr;


AiController::AiController(Ship* owner)
	:IController(owner)
	, m_vtAllBoids(nullptr)
	, m_pGameCollision(*((CGame*)CGame::GetApplication())->GetSceneManager()->GetCollisionManager())
{
	//turning on the collider
	CollisionSetup();

	//Alignment of the AI for whether what side its going to be on
	m_nAligment = owner->GetAlignment();

	m_vWaypoints.reserve(WAYPOINT_LIMIT);

	m_fPosition = VectorZero;

	//Holds the position of the new target
	toTarget = VectorZero;

	//float m_fTimer;
	CurrentWaypointIndex = 0;
	CurrentAttackWaypointIndex = 0;

	m_fWaypointDistance = 0;

	fsEventManager::GetInstance()->RegisterClient("Alert", this, &AiController::Alert);
	m_fEngageDistance = DISTANCE_TO_TARGET;
}

AiController::AiController(const AiController & other)
	: IController(other)
	, m_vtAllBoids(nullptr)
	, m_pGameCollision(*((CGame*)CGame::GetApplication())->GetGameplayScene()->GetSceneManager()->GetCollisionManager())
{
	//turning on the collider
	CollisionSetup();


	fsEventManager::GetInstance()->RegisterClient("Alert", this, &AiController::Alert);


	m_fPosition = VectorZero;

	//Holds the position of the new target
	toTarget = VectorZero;

	//float m_fTimer;
	CurrentWaypointIndex = 0;
	CurrentAttackWaypointIndex = 0;

	m_fWaypointDistance = 0;


	m_vWaypoints.reserve(WAYPOINT_LIMIT);

	//Movement
	m_fSlowDownRadius = other.m_fSlowDownRadius;


	//State Checking
	//Attack
	m_fAttackDistance = other.m_fAttackDistance;
	m_fDistanceSideAttack = other.m_fDistanceSideAttack;
	m_fDistanceStationary = other.m_fDistanceStationary;
	m_fEngageDistance = other.m_fEngageDistance;

	//Cannon Firing 
	m_fSideCannonAttackDistance = other.m_fSideCannonAttackDistance;
	m_fSideCannonAngleLimit = other.m_fSideCannonAngleLimit;
	m_fFrontCannonRange = other.m_fFrontCannonRange;


	//Flocking Values		 
	m_fFlockRadius = other.m_fFlockRadius;
	m_fMaxCombinedRadius = other.m_fMaxCombinedRadius;
	m_fCohesionWieight = other.m_fCohesionWieight;
	m_fAlignmentWieight = other.m_fAlignmentWieight;
	m_fSeperationWieight = other.m_fSeperationWieight;

	//*******************Timer********************//
	m_fAttackTargetFinderTimer = other.m_fAttackTargetFinderTimer;
	m_fMaxFinderTimer = other.m_fMaxFinderTimer;
	m_fBoundsTravelTimer = other.m_fBoundsTravelTimer;
	m_fMaxBoundsTime = other.m_fMaxBoundsTime;
	m_fDistanceAlert = other.m_fDistanceAlert;


}

AiController& AiController::operator=(const AiController& other)
{
	if (this == &other)
	{
		return *this;
	}
	IController::operator=(other);

	m_vtAllBoids = other.m_vtAllBoids;
	m_pGameCollision = other.m_pGameCollision;

	// turning on the collider
	CollisionSetup();

	fsEventManager::GetInstance()->RegisterClient("Alert", this, &AiController::Alert);

	m_fPosition = VectorZero;

	//Holds the position of the new target
	toTarget = VectorZero;

	//float m_fTimer;
	CurrentWaypointIndex = 0;
	CurrentAttackWaypointIndex = 0;

	m_fWaypointDistance = 0;


	m_vWaypoints.reserve(WAYPOINT_LIMIT);

	m_vtAllBoids = other.m_vtAllBoids;
	m_pGameCollision = other.m_pGameCollision;

	//Movement
	m_fSlowDownRadius = other.m_fSlowDownRadius;


	//State Checking
	//Attack
	m_fAttackDistance = other.m_fAttackDistance;
	m_fDistanceSideAttack = other.m_fDistanceSideAttack;
	m_fDistanceStationary = other.m_fDistanceStationary;
	m_fEngageDistance = other.m_fEngageDistance;

	//Cannon Firing 
	m_fSideCannonAttackDistance = other.m_fSideCannonAttackDistance;
	m_fSideCannonAngleLimit = other.m_fSideCannonAngleLimit;
	m_fFrontCannonRange = other.m_fFrontCannonRange;


	//Flocking Values		 
	m_fFlockRadius = other.m_fFlockRadius;
	m_fMaxCombinedRadius = other.m_fMaxCombinedRadius;
	m_fCohesionWieight = other.m_fCohesionWieight;
	m_fAlignmentWieight = other.m_fAlignmentWieight;
	m_fSeperationWieight = other.m_fSeperationWieight;

	//*******************Timer********************//
	m_fAttackTargetFinderTimer = other.m_fAttackTargetFinderTimer;
	m_fMaxFinderTimer = other.m_fMaxFinderTimer;
	m_fBoundsTravelTimer = other.m_fBoundsTravelTimer;
	m_fMaxBoundsTime = other.m_fMaxBoundsTime;
	m_fDistanceAlert = other.m_fDistanceAlert;


	return *this;
}

void AiController::CollisionSetup()
{
	XMFLOAT4X4 r15;
	XMStoreFloat4x4(&r15, XMMatrixRotationY(XMConvertToRadians(45)));
	XMFLOAT4X4 l15;
	XMStoreFloat4x4(&l15, XMMatrixRotationY(XMConvertToRadians(-45)));

	XMFLOAT3 vec[2] = { VectorZero, VectorZ * 10 };
	XMFLOAT3 vecR15[2] = { VectorZero, VectorZ * 7 * r15 };
	XMFLOAT3 vecL15[2] = { VectorZero, VectorZ * 7 * l15 };
	float yOff = -m_pOwner->GetPosition().y;
	TemplatedCollisionCallback<AiController>* CollisionFunction1 = new TemplatedCollisionCallback<AiController>(this, &AiController::SegementOnCollisionR);

	m_pRightAvoidanceSegement = new SegmentCollider(m_pGameCollision, m_pOwner->GetLocalMatrix(), m_pOwner, CollisionFunction1, vecR15, XMFLOAT3(.4f, yOff, -1.0f));


	TemplatedCollisionCallback<AiController>* CollisionFunction2 = new TemplatedCollisionCallback<AiController>(this, &AiController::SegementOnCollisionL);

	m_pLeftAvoidanceSegement = new SegmentCollider(m_pGameCollision, m_pOwner->GetLocalMatrix(), m_pOwner, CollisionFunction2, vecL15, XMFLOAT3(-.4f, yOff, -1.0f));


	TemplatedCollisionCallback<AiController>* CollisionFunction = new TemplatedCollisionCallback<AiController>(this, &AiController::SegementOnCollisionM);

	m_pAvoidanceSegment = new SegmentCollider(m_pGameCollision, m_pOwner->GetLocalMatrix(), m_pOwner, CollisionFunction, vec, XMFLOAT3(0, yOff, -.75f));


	vec[0] = VectorZero;
	vec[1] = VectorZero;

	TemplatedCollisionCallback<AiController>* AttackCollisionFunction = new TemplatedCollisionCallback<AiController>(this, &AiController::CannonSightCollision);

	m_pAttackSegment = new SegmentCollider(m_pGameCollision, m_pOwner->GetLocalMatrix(), m_pOwner, AttackCollisionFunction, vec, XMFLOAT3(0, .5f, 0.2f));

	m_pGameCollision.stopTracking(m_pAttackSegment);

	m_bOpenFire = false;
}

void AiController::StopColliders()
{
	CollisionManager& GameCollision = *((CGame*)CGame::GetApplication())->GetSceneManager()->GetCollisionManager();

	GameCollision.stopTracking(m_pRightAvoidanceSegement);
	GameCollision.stopTracking(m_pLeftAvoidanceSegement);
	GameCollision.stopTracking(m_pAvoidanceSegment);
	GameCollision.stopTracking(m_pAttackSegment);
}

void AiController::StartColliders()
{
	CollisionManager& GameCollision = *((CGame*)CGame::GetApplication())->GetSceneManager()->GetCollisionManager();

	GameCollision.trackThis(m_pRightAvoidanceSegement);
	GameCollision.trackThis(m_pLeftAvoidanceSegement);
	GameCollision.trackThis(m_pAvoidanceSegment);
	GameCollision.trackThis(m_pAttackSegment);
}

#pragma region //**********************Update Functions**************************//

void AiController::ComeBackIntoWorld()
{
	if (m_vWaypoints.size() == 0)
	{
		return;
	}

	if (fabs(m_pOwner->GetPosition().x) >= 400 || fabs(m_pOwner->GetPosition().y) >= 400)
	{
		setPosition(m_pOwner->GetLocalMatrix(), m_vWaypoints[CurrentWaypointIndex]);
	}
}

bool AiController::Regroup()
{
	if (m_eCurrentState == ATTACK || m_eCurrentState == ATTACK_STATIONARY)
	{
		return false;
	}

	//if it is null you have to squad to regroup with 
	if (m_pSteeringBoid->GetSquad() == nullptr)
	{
		return false;
	}

	//if you are alone you dont need to regroup
	if (m_pSteeringBoid->GetSquadVector()->size() <= 1)
	{
		return false;
	}


	//find the vector to the average distance
	XMFLOAT3 toAverage = m_pSteeringBoid->GetAveragePosition() - m_fPosition;
	float distance = sqrMagnitudeOf(toAverage);

	normalizeFloat3(toAverage);
	float dotResult = dotProduct(toAverage, m_pOwner->GetForward());
	//check to see if the distance it  greater that my limit
	if (distance > m_pSteeringBoid->GetMaxCombinedRadius())
	{
		if (dotResult < 0)
		{

			//if so tell the AI he should slow down and wait up
			return true;
		}

		float SquadCohesion = m_pSteeringBoid->GetMaxCombinedRadius();

		SquadCohesion = (distance / SquadCohesion);
		SquadCohesion *= 10;
		m_pSteeringBoid->SetSquadCohesion(m_pSteeringBoid->GetSquadCohesion() + SquadCohesion);
	}

	return false;
}

AiController::AI_States AiController::UpdateCurrentState()
{
	//turning off guidance 
		//the checks will turn it on
	m_pSteeringBoid->EnableGuidance(false);


	if (CheckAlertState())
	{
		m_pSteeringBoid->SetSquadAlignment(0);
		m_pSteeringBoid->SetSquadCohesion(0);

		if (m_eType == GOODS)
		{
			return SCATTER;
		}
		return ASSIST;
	}

	if (CheckFleeState())
	{
		return FLEE;
	}

	if (CheckAttackState())
	{
		m_pSteeringBoid->SetSquadAlignment(0);
		m_pSteeringBoid->SetSquadCohesion(0);
		return ATTACK;
	}

	if (CheckWaypointState())
	{
		toTarget = m_pSteeringBoid->GetTarget() - m_fPosition;
		return WAYPOINT;
	}

	m_pSteeringBoid->EnableGuidance(true);
	return WAIT;
}

void AiController::UpdateStartup()
{
	if (stillColliding && sqrDistanceBetween(playerShip->GetPosition(), GetPosition()) > (agroSphere->getRadius() * agroSphere->getRadius() + 15))
	{
		stillColliding = false;
	}

	//Reseting info
	m_nAccelerating = 0;
	m_fForward = VectorZero;

	m_pSteeringBoid->ResetWeights();
	//Getting my information
	m_fPosition = m_pOwner->GetPosition();
	//Finding the state i neeed to be in now
	m_eCurrentState = UpdateCurrentState();

}

void AiController::UpdateMovement(float dt)
{
	m_fForward = CalculateForward(dt);

	ComeBackIntoWorld();

	Movement(dt);

	if (m_nAccelerating == 1)
	{
		Accelerate(dt);
	}
	else
	{
		Decelerate(dt);
	}

}

XMFLOAT3 AiController::CollisionCorrection(float dt)
{
	if (m_bCollisionHappened == false && m_bLeftCollision == false && m_bRightCollision == false)
	{
		m_bSameTarget = false;
		return lastDirection = VectorZero;
	}

	m_pSteeringBoid->SetSquadCohesion(10);

	size_t numVerts = 0;


	XMFLOAT3 Vector2Target = toTarget;
	
	if (m_pSteeringBoid->GetSquad() && (m_eCurrentState != ATTACK || m_eCurrentState != ATTACK_STATIONARY || m_eCurrentState != FLEE))
	{
		Vector2Target = m_pSteeringBoid->GetSquad()->GetCurrentWaypoint() - m_fPosition;
	}

	//If my right Collision Segment is hit
	if (m_bRightCollision)
	{
		if (magnitudeOf(Vector2Target) < magnitudeOf(m_pRightAvoidanceSegement->getEnd() - m_pRightAvoidanceSegement->getStart()))
		{
			return VectorZero;
		}

		lastDirection = (m_pLeftAvoidanceSegement->getEnd() - m_pLeftAvoidanceSegement->getStart());

		if (magnitudeOf(lastDirection) > 1)
		{
			normalizeFloat3(lastDirection);

		}
	}

	//If my Left Collision Segment is hit
	else if (m_bLeftCollision)
	{
		if (magnitudeOf(Vector2Target) < magnitudeOf(m_pLeftAvoidanceSegement->getEnd() - m_pLeftAvoidanceSegement->getStart()))
		{
			return VectorZero;
		}
		lastDirection = (m_pRightAvoidanceSegement->getEnd() - m_pRightAvoidanceSegement->getStart());
		if (magnitudeOf(lastDirection) > 1)
		{
			normalizeFloat3(lastDirection);

		}
	}

	//Checking to see if the Collision manager had a collision happen
	else //if (m_bCollisionHappened && m_bLeftCollision == false && m_bRightCollision == false)
	{
		if (magnitudeOf(Vector2Target) < magnitudeOf(m_pAvoidanceSegment->getEnd() - m_pAvoidanceSegment->getStart()))
		{
			return VectorZero;
		}
		Decelerate(dt);
		static int side = 1;
		//Getting the right of my owner
		XMFLOAT3 & myRight = m_pOwner->GetRight();

		XMFLOAT3 targetVector = Vector2Target;

		if (magnitudeOf(targetVector) > 1)
		{
			normalizeFloat3(targetVector);
		}

		//the Dot bewteen Right and the target
		float dotresult = dotProduct(targetVector, myRight);

		//if the dot result  is less then 0
		if (m_bSameTarget == false)
		{
			if (dotresult < 0)
			{
				side = -1;
			}
			else
				side = 1;
		}
		myRight *= (float)side;

		//Getting my forward
		XMFLOAT3 &  myForward = m_pOwner->GetForward();

		//Applying my direction and the side I need to avoid
		XMFLOAT3 newDirection = myRight;

		//checking to see if the length of the vector is greater than 1
		if (magnitudeOf(newDirection) > 1)
		{
			//if so Normalize it
			normalizeFloat3(newDirection);
		}

		return newDirection;
	}



	return lastDirection;
}

void AiController::Movement(float dt)
{
	// im told to wait i will stop and wait
	if (m_eCurrentState == WAIT || m_eCurrentState == ATTACK_STATIONARY)
	{
		m_nAccelerating = -1;

		if (m_eCurrentState == ATTACK_STATIONARY && m_bOpenFire == false)
		{
			m_pOwner->SetMinSpeed(2);
		}
		else
		{
			m_pOwner->SetMinSpeed(0);
		}
		return;
	}

	if (m_bCollisionHappened || Regroup() || CorrectForwardHeading(m_fForward))
	{
		m_nAccelerating = -1;
		m_pOwner->SetMinSpeed(2);
		return;
	}

	float DistanceCheck = sqrMagnitudeOf(m_pSteeringBoid->GetTarget() - m_fPosition);

	float slowDown = m_fSlowDownRadius * m_fSlowDownRadius;

	if (DistanceCheck < slowDown)
	{
		m_nAccelerating = -1;
	}
	else
	{
		m_nAccelerating = 1;
	}

}

bool AiController::CorrectForwardHeading(XMFLOAT3& steering)
{
	XMFLOAT3 CorrectionVector = steering;

	if (magnitudeOf(steering) > 1)
	{
		normalizeFloat3(CorrectionVector);
	}


	//find the dot result of my forward and the new one
	float dotresult = dotProduct(CorrectionVector, m_pOwner->GetForward());

	// -1 for saying its behind me
	if (dotresult < 0)
	{
		dotresult = dotProduct(CorrectionVector, m_pOwner->GetRight());
		steering = m_pOwner->GetRight();
		if (dotresult < 0)
		{
			steering = -m_pOwner->GetRight();
		}
		return true;
	}

	return false;
}

void AiController::UpdateTimers(float dt)
{
	m_ePrevState = m_eCurrentState;
	m_bOpenFire = false;

	//Updating how long they have to find the target before waiting
	m_fAttackTargetFinderTimer -= dt;
	m_fBoundsTravelTimer -= dt;
}

void AiController::UpdateStates()
{
	if (m_eCurrentState == ATTACK || m_eCurrentState == ASSIST)
	{
		m_pSteeringBoid->SetGlobalSeperation(0.5f);

		if (CheckStationaryState())
		{
			//returns the heading perpendicular towards the target
			if (m_pOwner->GetSpeed() < 5)
			{
				m_eCurrentState = ATTACK_STATIONARY;
			}
		}

		FireCannons();
	}
}

XMFLOAT3 AiController::CalculateForward(float dt)
{
	XMFLOAT3 newHeading = VectorZero;
	//these is for when you are in Attack stationary position

	//Calculating the Collision correction is nessecary
	XMFLOAT3 correction = CollisionCorrection(dt);

	if (magnitudeOf(correction) > 1)
	{
		normalizeFloat3(correction);
	}

	if (m_eCurrentState == ATTACK_STATIONARY)
	{
		m_pSteeringBoid->SetSeekWeight(0);
		m_pSteeringBoid->SetGlobalSeperation(0.1f);
		XMFLOAT3 Steering = StationaryPositioning(toTarget);

		if (m_pOwner->GetSpeed() > 0.1f)
		{
			Steering += m_pSteeringBoid->Update();
		}


		if (magnitudeOf(Steering) > 1)
		{
			normalizeFloat3(Steering);
		}

		newHeading = Steering;

		if (correction.x || correction.z)
		{
			if (dotProduct(correction, newHeading) < 0)
			{
				Steering *= -1;
			}
			newHeading = (correction * 0.8f) + (Steering * 0.2f);
		}

		const XMFLOAT3& myForward = m_pOwner->GetForward();

		float AnglebetweenSteeringForward = XMConvertToDegrees(acosf(fabs(dotProduct(newHeading, myForward))));

		if (AnglebetweenSteeringForward < 28)
		{
			return myForward;
		}

		return newHeading;
	}

	XMFLOAT3 steering = VectorZero;

	//adding the steering behaviors together

	steering += m_pSteeringBoid->Update();


	if (magnitudeOf(steering) > 1)
	{
		normalizeFloat3(steering);
	}

	newHeading = steering;

	if (correction.x || correction.z)
	{
		newHeading = (correction * 0.4f) + (steering*0.6f);
	}

	return newHeading;
}

#pragma endregion 

void AiController::Update(float dt)
{
	//This holds all thought process the AI will have
	UpdateStartup();

	UpdateStates();

	//Call this to move your ship
	UpdateMovement(dt);

	m_bCollisionHappened = false;
	m_bRightCollision = false;
	m_bLeftCollision = false;

	//rotates in the direction of your newly calculated forward 
	FaceDirection(m_fForward, dt);
	IController::Update(dt);

	//Call this to update all 
	UpdateTimers(dt);
}

#pragma region//*******************State Check********************************//

bool AiController::CheckAttackState()
{

#pragma region Early Outs
	if (m_pTarget == nullptr)
	{
		return false;
	}

	if (m_pTarget->ToBeDeleted() == true)
	{
		ChaseTarget(nullptr);
		return false;
	}
#pragma endregion

	//Vector to the Target
	toTarget = positionOf(m_pTarget->GetWorldMatrix());

	//setting the destination I should travel to
	m_pSteeringBoid->SetTarget(toTarget);

	toTarget = toTarget - m_fPosition;

	//Distance check
	float Distancecheck = sqrMagnitudeOf(toTarget);

	if (Distancecheck <= m_fEngageDistance)
	{
		m_fAttackTargetFinderTimer = m_fMaxFinderTimer;
	}

	//Setting the attack target
	if (m_fAttackTargetFinderTimer > 0)
	{
		SetUpAttackSegment();
		m_pOwner->SetMinSpeed(0);
		return true;
	}

	ChaseTarget(nullptr);
	return false;
}

bool AiController::CheckWaypointState()
{

	if (m_pSteeringBoid)
	{
		Squad*  mySquad = m_pSteeringBoid->GetSquad();

		if (mySquad)
		{
			if (mySquad->GetWaypoints().empty() == true)
			{
				return false;
			}

			if (m_pSteeringBoid->AtWaypoint())
			{
				//because im at the waypoint and im waiting
				return false;
			}
			//enable guidance
			m_pSteeringBoid->EnableGuidance(true);
			//return true because im not at the waypoint yet
			return true;
		}
	}


#pragma region Without a boid or squad
	int numWaypoints = (int)m_vWaypoints.size();

	if (numWaypoints > 0)
	{
		float distance = sqrMagnitudeOf(m_vWaypoints[CurrentWaypointIndex] - m_fPosition);

		if (distance < WAYPOINT_DISTANCE_LIMIT * WAYPOINT_DISTANCE_LIMIT)
		{

			//increase the counter
			CurrentWaypointIndex++;
			//Check to see the index went out of scope 
			if (CurrentWaypointIndex >= numWaypoints)
			{
				//and resetting it
				CurrentWaypointIndex = 0;
			}
		}

		m_pSteeringBoid->SetTarget(m_vWaypoints[CurrentWaypointIndex]);
		m_pOwner->SetMinSpeed(1);
		return true;
	}

	return false;
#pragma endregion

}

bool AiController::CheckFleeState()
{
	bool result = EnemyNearBy();

	//if are fleeing and there are enemies around
	if (m_bFleeMode == true && result)
	{
		//check the timer
		if (m_fBoundsTravelTimer <= 0)
		{
			//Finds the farthest point
			FarthestWaypoint();

			//If the FleeMode becomes false 
			if (m_bFleeMode == false)
			{
				ChaseTarget(nullptr);
				//teleport and return false
				Teleport();
				return false;
			}
		}
		return true;
	}

	//if you are fleeing and you dont see a enemy in sight 
	if (m_bFleeMode == true && result == false)
	{
		ChaseTarget(nullptr);
		//teleport to the point will no player near by
		Teleport();
		return false;
	}

	//if I have low health and your fleeMode is false
	if ((m_pOwner->GetHealth() < m_nFleeHealth))
	{
		BeggingText();
		//Set the fleeing to happen once
		m_bFleeMode = true;
		//find the FurthestPoint
		FarthestWaypoint();
		//Yess I and Getting the mess away from people
		return true;
	}

	return false;
}

bool AiController::CheckAlertState()
{
	//Check to see if i hear anything

	if (m_bHeardAlert == false)
	{
		//if not return false
		return false;
	}

	//Getting the vector to the AlertLocation
	XMFLOAT3 toAlert = m_vtAlertLocation - m_fPosition;

	m_pSteeringBoid->SetTarget(m_vtAlertLocation);

	//Finding the distance of the vector
	float distance = sqrMagnitudeOf(toAlert);

	if (m_eType == GOODS)
	{
		//if im out side of the range I do not need to run away anymore 

		if (distance > DISTANCE_ALERT)
		{
			//Set the alert to false and 
			m_bHeardAlert = false;
			//and return false for the check
			return false;
		}

		FarthestWaypoint();
		return true;
	}

	if (m_eType == MILITARY)
	{

		if (m_pTarget == nullptr)
		{
			return false;
		}

		if (m_pTarget->ToBeDeleted() == true)
		{
			return false;
		}

		//if im at the location I dont need to travel there anymore
		if (distance <= 25)
		{
			//Getting the vector from myself to the target
			toAlert = m_pTarget->GetPosition() - m_fPosition;

			//Setting the destinaiton of the target
			m_pSteeringBoid->SetTarget(m_pTarget->GetPosition());

			//Getting the distance
			distance = sqrMagnitudeOf(toAlert);

			//see if the Target is further than my sight Range
			if (distance > DISTANCE_ALERT)
			{
				//if so set target to null
				ChaseTarget(nullptr);
				//Set the alert to false and 
				m_bHeardAlert = false;
				//and return false for the check
				return false;
			}

			//if the target is in sight go after them 
			ChaseTarget(m_pTarget);
			m_bHeardAlert = false;

		}

	}

	toTarget = toAlert;

	return true;
}

bool AiController::CheckStationaryState()
{
	//Distance check
	float Distancecheck = sqrMagnitudeOf(m_pTarget->GetPosition() - m_fPosition);

	if (m_ePrevState == ATTACK_STATIONARY && m_eCurrentState == ATTACK)
	{
		if (Distancecheck <= m_fDistanceStationary + 9)
		{
			return true;
		}
	}

	//Setting the attack target
	if (Distancecheck <= m_fDistanceStationary && m_pOwner->GetSpeed() < 5)
	{
		return true;
	}


	return false;
}

XMFLOAT3 AiController::StationaryPositioning(XMFLOAT3 toVector)
{
	XMFLOAT3 heading = VectorZero;

	heading = rightAxisOf(m_pOwner->GetWorldMatrix());

	float Xdir = 1;
	float Zdir = 1;

	XMFLOAT3 newDirection = toVector;

	normalizeFloat3(newDirection);

	//getting the direction of my forward
	XMFLOAT3& AxisVector = forwardAxisOf(m_pOwner->GetWorldMatrix());

	//Seeing what side of the boat they are on
	float dotresult = dotProduct(AxisVector, newDirection);

	if (dotresult < 0)
	{
		Zdir = -1;
	}

	AxisVector = heading;

	//Seeing what side of the boat they are on
	dotresult = dotProduct(AxisVector, newDirection);
	if (fabs(dotresult) > 0.9995f)
	{
		Xdir = 0;
	}
	else if (dotresult > FLT_EPSILON)
	{
		Xdir = -1;
	}

	heading *= Zdir * Xdir;

	normalizeFloat3(heading);

	return heading;
}

bool AiController::EnemyNearBy()
{
	if (!m_vtAllBoids)
	{
		return false;
	}
	//grabbing the size before hand
	unsigned int vectorSize = (UINT)m_vtAllBoids->size();

	XMFLOAT3 position = (*m_vtAllBoids)[0]->GetPosition();

	//looping to find the ships 
	for (unsigned int index = 1; index < vectorSize; index++)
	{
		Ship* _ship = (*m_vtAllBoids)[index];

		//Early out on myslef
		if (_ship == m_pOwner)
		{
			continue;
		}
		//Early out on all my Allies
		if (_ship->GetAlignment() == m_pOwner->GetAlignment())
		{
			continue;
		}
		//Getting the otherShip in game
		XMFLOAT3& newPos = _ship->GetPosition();

		//Finding the closest ship to me
		if (CloserObject(position, newPos))
		{
			position = newPos;
		}
	}

	//Checking to see if the distance between myself and another
	float distanceCheck = distanceBetween(position, m_fPosition);
	// are less then my flees distance
	if (distanceCheck < FLEE_DISTANCE)
	{
		return true;
	}

	return false;
}

void AiController::Teleport()
{
	//Getting the reference
	std::vector<XMFLOAT3>& Waypoints = ((CGame*)CGame::GetApplication())->GetGameplayScene()->GetWaypoints();

	if (!m_vtAllBoids)
	{
		return;
	}
	//grabbing the sizes before hand
	unsigned int vectorSize = (UINT)m_vtAllBoids->size();
	unsigned int waypointCount = (UINT)Waypoints.size();

	//This vecotr will hold the 
	XMFLOAT3 position = VectorZero;

	//For loop through all the waypoints
	bool found = false;
	//grabbing the position
	XMFLOAT3 WaypointPosition = Waypoints[0];
	for (unsigned int i = 1; i < waypointCount; i++, found = false)
	{
		XMFLOAT3&  newPoint = Waypoints[i];
		//Finding the first farthest waypoint
		if (CloserObject(newPoint, WaypointPosition))
		{
			if (sqrMagnitudeOf(newPoint - m_fPosition) > 100)
			{
				WaypointPosition = newPoint;
			}
		}

		//Check to see if there are any ships within range of that WaypointPosition

		//looping to find the ships 
		for (unsigned int index = 0; index < vectorSize; index++)
		{
			//Getting the current index
			Ship* _ship = (*m_vtAllBoids)[index];

			//Do not include my self
			if (_ship == m_pOwner)
			{
				continue;
			}

			//Make sure to not check for those on my side
			if (_ship->GetAlignment() == m_pOwner->GetAlignment())
			{
				continue;
			}

			// Getting the position of the other ship
			XMFLOAT3& newPos = _ship->GetPosition();


			//Check to see if its wwith in distance  
			float sqrDistance = sqrMagnitudeOf(newPos - WaypointPosition);


			if (sqrDistance < 100)
			{
				found = true;
				break;
			}
		}

		if (found == false)
		{
			position = WaypointPosition;
			break;
		}

	}


	//check to see if the Position is above the incorrect position
	if (position.y > -1)
	{
		//getting the vector to the new position
		m_pOwner->GetTransform().Translate(position - m_fPosition);

		//stop fleeing
		m_bFleeMode = false;
		//reset the Flee health to a new lower limit
		m_nFleeHealth = int(m_pOwner->GetHealth() * 0.5f);
	}


}

void AiController::FarthestWaypoint()
{

	//Need to apply the target to he boid
	//Also this needs to be applyed to the Ports and Out of bounds which ever is closer
	//assert(false);

	//if you are already out of bounds 
	if (fabs(m_fPosition.x) >= 350 || fabs(m_fPosition.z) >= 350)
	{
		//stop fleeing
		m_bFleeMode = false;
		//reset the Flee health to a new lower limit
		m_nFleeHealth = int(m_pOwner->GetHealth() * 0.5f);
		return;
	}

	//Setting the four points to my position
	XMFLOAT3 OutOFBoundsPoints[] = { m_fPosition,m_fPosition,m_fPosition,m_fPosition };

	//moving the points accordingly
	OutOFBoundsPoints[0].x -= 300;
	OutOFBoundsPoints[1].x += 300;
	OutOFBoundsPoints[2].z -= 300;
	OutOFBoundsPoints[3].z += 300;

	//will hold the variable for the closest Point
	XMFLOAT3 closestObject = OutOFBoundsPoints[0];

	//looping to find the ClosestPoint
	for (unsigned int index = 1; index < 4; index++)
	{
		//Grabbing the index of that Boundary point
		XMFLOAT3& BoundaryPoint = OutOFBoundsPoints[index];

		//if the boundaryPoint is closer than the closestObject
		if (CloserObject(closestObject, BoundaryPoint))
		{
			//Assign the ClosestPoint to the BoundaryPoint 
			closestObject = BoundaryPoint;
		}
	}

	//Setting the toTarget to the ClosestObject
	m_pSteeringBoid->SetTarget(closestObject);

	toTarget = closestObject - m_fPosition;
	//reseting the timer to the maxTime
	m_fBoundsTravelTimer = m_fMaxBoundsTime;
}

//Finding the closer object if its true Target2 is closer than Target1
bool  AiController::CloserObject(const XMFLOAT3& target1, const XMFLOAT3& target2)
{
	if (distanceBetween(target2, m_fPosition) < distanceBetween(target1, m_fPosition))
	{
		return true;
	}
	return false;
}

bool AiController::CloserObject(const XMFLOAT3 & target1, const XMFLOAT3 & target2, const XMFLOAT3 & position)
{
	if (distanceBetween(target2, position) < distanceBetween(target1, position))
	{
		return true;
	}
	return false;
}

XMFLOAT3 AiController::VectorToWaypoint()
{
	return m_vWaypoints[CurrentWaypointIndex] - m_fPosition;
}

#pragma endregion

//**********************Turning************************//
//This Fucntion will normalize the vector that is being passed

void AiController::FaceDirection(const XMFLOAT3 & _direction, const float& dt)
{
	if (_direction == VectorZero)
	{
		return;
	}

	//normalizing the direction being passed in
	XMFLOAT3 newDirection = _direction;
	if (magnitudeOf(newDirection) > 1)
	{
		normalizeFloat3(newDirection);
	}

	XMFLOAT3& AxisVector = m_pOwner->GetForward();

	if (magnitudeOf(AxisVector) > 1)
	{
		normalizeFloat3(AxisVector);
	}
	//find the dot product
	float dotresult = dotProduct(AxisVector, newDirection);

	//clamping the value between negative and positive 1
	float angle = 1 - dotresult;
	angle = clamp(angle, -1, 1);

	//Getting my right vector
	AxisVector = m_pOwner->GetRight();

	if (magnitudeOf(AxisVector) > 1)
	{
		normalizeFloat3(AxisVector);
	}

	//find the dot between my heading and the heading I am trying to travel to
	if (dotProduct(AxisVector, newDirection) < 0)
	{
		//negate the angle if its less than 0
		angle *= -1;
	}

	////rotating the transform of the 
	float m_RotationAngle = 0;

	if (fabs(angle) > FLT_EPSILON)
	{
		m_RotationAngle = MATH_PI * angle;
	}

	m_pOwner->TurnRadians(m_RotationAngle, dt);
}

void AiController::FireCannons()
{
	if (m_bOpenFire == false)
	{
		return;
	}

	XMFLOAT3& SegmentVector = m_pAttackSegment->getEnd() - m_pAttackSegment->getStart();

	//finding the the Sqrd Length of the my
	if (sqrMagnitudeOf(SegmentVector) > 100)
	{
		return;
	}

	//if the length is greater than that of 1
	if (magnitudeOf(SegmentVector) > 1)
	{
		//setting it to a unit length of one
		normalizeFloat3(SegmentVector);
	}

	//dotting the Segement with my forward
	float dotresult = dotProduct(SegmentVector, m_pOwner->GetForward());

	if (dotresult > m_fFrontCannonRange)
	{
		FireFrontCannons();
	}

	//Finding the result of the segment and my right
	dotresult = dotProduct(SegmentVector, m_pOwner->GetRight());

	//making sure that the value is InRange regaurdless of sign
	if (fabs(dotresult) < m_fSideCannonAngleLimit)
	{
		return;
	}


	//if the dot is less than the FLT_EPSLION
	if (dotresult < FLT_EPSILON)
	{
		//Fire Left
		FireSideCannons(LEFT);
	}
	else
	{
		//Fire Right
		FireSideCannons(RIGHT);
	}
}

void AiController::SetUpAttackSegment()
{
	//Putting the Target Position into out Local space
	XMFLOAT3 TargetPos_LocalToMe = InToLocalSpace(m_pTarget->GetPosition(), m_pOwner->GetLocalMatrix());

	//Setting our attack position endpoint
	m_pAttackSegment->SetEndPoint(TargetPos_LocalToMe);
}

#pragma region//**********************Collision Functions************************//

void AiController::ToggleRayCollider(bool turnOn, Collider* collider)
{
	//if turnOn is true
	if (turnOn == true)
	{
		//We will start tracking the collider from the manager
		m_pGameCollision.trackThis(collider);
	}
	else
	{
		//We will stop tracking the collider from the manager
		m_pGameCollision.stopTracking(collider);
	}

}

void AiController::AgrosphereOnCollision(Collider & other)
{
	Entity3D* CollidedObject = reinterpret_cast<Entity3D*>(other.getID());
	if (!CollidedObject || CollidedObject->GetType() == Entity3D::SHIP && other.isTrigger())
		return;

	if (CollidedObject->GetType() == Entity3D::SHIP)
	{
		Ship* CollidedShip = static_cast<Ship*>(CollidedObject);
		if (CollidedShip->GetAlignment() == Ship::eAlignment::PLAYER)
		{
			CGame*	game = (CGame*)CGame::GetApplication();
			//std::unordered_map<std::string, G2D::Entity2D*> HUD = game->GetGameplayScene()->GetHUDElements();
			if (m_pOwner->GetCurrentMaxSpeed() <= BOARDING_SPEED
				&& sqrDistanceBetween(CollidedShip->GetPosition(), m_pOwner->GetPosition()) < BOARDING_DIST * BOARDING_DIST
				&& m_pOwner->GetBoardingStage() == Ship::eBoardingState::NONE)
			{
				if (strcmp(m_pOwner->GetMeshName(), "BossShip.mesh") != 0)
				{
					//if (GameSetting::GetRef().GamePadIsConnected())
					//{
					//	HUD["BoardButtonXbox"]->Active(true);
					//}
					//else
					//{
					//	HUD["BoardButtonKeyboard"]->Active(true);
					//}
					game->GetGameplayScene()->ActivateBoardingPrompt();
				}
			}
			//else
			//{
			//	HUD["BoardButtonKeyboard"]->Active(false);
			//	HUD["BoardButtonXbox"]->Active(false);
			//}

			if (!stillColliding)
			{
				playerShip = CollidedShip;
				//float ratio = CollidedShip->GetCurrReputation() / CollidedShip->GetMaxReputation();
				float ratio = static_cast<PlayerShip*>(CollidedShip)->GetCurrReputation() / static_cast<PlayerShip*>(CollidedShip)->GetMaxReputation();
				bool shouldAgro = rand() % 100 < ratio * 100;
				if (shouldAgro)
					ChaseTarget(CollidedShip);
				stillColliding = true;
			}
		}
	}

}

void AiController::FleesphereOnCollision(Collider & other)
{

	Entity3D* CollidedObject = reinterpret_cast<Entity3D*>(other.getID());
	if (!CollidedObject || CollidedObject->GetType() == Entity3D::SHIP && other.isTrigger())
		return;

	if (CollidedObject->GetType() != Entity3D::SHIP)
	{
		return;
	}
	Ship* CollidedShip = static_cast<Ship*>(CollidedObject);
	if (CollidedShip->GetAlignment() == Ship::eAlignment::PLAYER)
	{
		CGame*	game = (CGame*)CGame::GetApplication();
		//std::unordered_map<std::string, G2D::Entity2D*> HUD = game->GetGameplayScene()->GetHUDElements();
		if (m_pOwner->GetCurrentMaxSpeed() <= BOARDING_SPEED
			&& sqrDistanceBetween(CollidedShip->GetPosition(), m_pOwner->GetPosition()) < BOARDING_DIST * BOARDING_DIST
			&& m_pOwner->GetBoardingStage() == Ship::eBoardingState::NONE)
		{
			//if (GameSetting::GetRef().GamePadIsConnected())
			//{
			//	HUD["BoardButtonXbox"]->Active(true);
			//}
			//else
			//{
			//	HUD["BoardButtonKeyboard"]->Active(true);
			//}
			game->GetGameplayScene()->ActivateBoardingPrompt();
		}
		//else
		//{
		//	HUD["BoardButtonKeyboard"]->Active(false);
		//	HUD["BoardButtonXbox"]->Active(false);
		//}


		playerShip = CollidedShip;

		if (!stillColliding)
		{


			//Calling the police in to help
			float ratio = static_cast<PlayerShip*>(CollidedShip)->GetCurrReputation();

			float radius = agroSphere->getRadius();

			float distance = distanceBetween(playerShip->GetPosition(), m_fPosition);

			//adding how far you are in the field of the sphere
			ratio = ratio + (radius - distance);

			//make it a ratio
			ratio /= (static_cast<PlayerShip*>(CollidedShip)->GetMaxReputation() + radius);

			//make it a chance to call the police
			bool shouldAgro = rand() % 100 < ratio * 100;

			if (shouldAgro)
			{
				fsEventManager::GetInstance()->FireEvent(fsEvents::AI_Help, &EventArgs1<Ship*>(playerShip));
			}
		}

		stillColliding = true;

	}

}

void AiController::SegementOnCollisionL(Collider& other)
{
	Entity3D* CollidedObject = reinterpret_cast<Entity3D*>(other.getID());
	if (CollidedObject->GetType() == Entity3D::LAND)
		m_bLeftCollision = true;
}

void AiController::SegementOnCollisionM(Collider& other)
{
	Entity3D* CollidedObject = reinterpret_cast<Entity3D*>(other.getID());
	if (CollidedObject->GetType() == Entity3D::LAND)
		m_bCollisionHappened = true;

	m_bSameTarget = false;
	if (m_pLastObject == CollidedObject && prevWaypoint == CurrentWaypointIndex)
	{
		m_bSameTarget = true;
		return;
	}
	prevWaypoint = CurrentWaypointIndex;

	m_pLastObject = CollidedObject;
}

void AiController::SegementOnCollisionR(Collider& other)
{
	Entity3D* CollidedObject = reinterpret_cast<Entity3D*>(other.getID());
	if (CollidedObject->GetType() == Entity3D::LAND)
		m_bRightCollision = true;
}

void AiController::CannonSightCollision(Collider & other)
{
	m_bOpenFire = true;

	//making sure that im not trying to shoot myself
	if (other.getID() == m_pOwner)
	{
		//m_bRightCollision = false;
		m_bOpenFire = false;
		return;
	}


	//changing the pointer to a entity
	Entity3D* CollidedObject = reinterpret_cast<Entity3D*>(other.getID());

	//checking to see if the entity is a ship or land
	if (CollidedObject->GetType() != Entity3D::SHIP)
	{
		m_bOpenFire = false;

		return;
	}

	//checking to see if the alignment is the same
	if (((Ship*)CollidedObject)->GetAlignment() == m_nAligment)
	{
		if (other.GetType() != Collider::ColliderType::Segment)
		{
			m_bOpenFire = false;
			return;
		}
	}


}

#pragma endregion

void AiController::FireFrontCannons()
{
	m_pOwner->LongNineFire();
}

void AiController::FireSideCannons(int side)
{
	//Firing the cannon on the side
	m_pOwner->GetSideCannonHandler()->Fire(Cannon::WeaponType::CANNON, side);
}

#pragma region//**********************Utility************************//

bool AiController::AddWaypoint(Transform& _newPoint)
{
	//early out condition
	if (m_vWaypoints.size() >= WAYPOINT_LIMIT)
	{
		//Adding the waypoint to the container
		m_vWaypoints.push_back(positionOf(_newPoint.GetWorldMatrix()));
		return true;
	}

	return false;
}

void AiController::SpeechText()
{
	if (m_pOwner)
	{
		m_pOwner->m_PopTextTimer = 3.0f;
		std::vector<std::wstring> textPool =
		{
			{ L"Attack!" },
			{ L"I'm coming for you!" },
			{ L"Destroy them!" },
			{ L"Send them to the sea floor!" },
			{ L"Go to hell!" },
			{ L"Die!" },
		};



		m_pOwner->m_PopText = textPool[rand() % textPool.size()];
	}
}

void AiController::BeggingText()
{
	if (m_pOwner)
	{
		m_pOwner->m_PopTextTimer = 3.0f;
		std::vector<std::wstring> textPool;

		textPool =
		{
			{ L"Please don't shoot!" },
			{ L"I have a family!" },
			{ L"Nooooooo!%$&#&#%" },
			{ L"I beg you!" },
			{ L"Take whatever you want!" },
			{ L"You bastard!" },
			{ L"$%$&%@#%$%^%*!" },

		};

		m_pOwner->m_PopText = textPool[rand() % textPool.size()];
	}
}

void AiController::ReferenceCounting(Ship* newTarget)
{
	//if the newTarget is a valid pointer 
	if (newTarget)
	{
		//assign the transform Pointer to the  newTarget Transform
		newTarget->RefCountUp();
		SoundManager* soundManager = ((CGame*)CGame::GetApplication())->GetSoundManager();
		soundManager->Play3DSound(AK::EVENTS::PLAY_ASSISTSOUND);
	}
	if (m_pTarget)
	{
		m_pTarget->RefCountDown();
	}

}

bool AiController::SetTarget(Ship* _newTarget)
{
	// if the Alignemt is on our side
	if (_newTarget && _newTarget->GetAlignment() == m_nAligment)
	{
		//do not set a target
		return false;
	}

	if (_newTarget == nullptr)
	{
		m_pAttackSegment->SetEndPoint(VectorZero);
		m_pGameCollision.stopTracking(m_pAttackSegment);
		ReferenceCounting(nullptr);
		m_pTarget = nullptr;
		return false;
	}


	if (m_pTarget == _newTarget)
	{
		return true;
	}


	ReferenceCounting(_newTarget);
	//assigning the target
	m_pTarget = _newTarget;


	Boid* target = nullptr;
	if (m_pTarget)
	{
		target = _newTarget->GetController()->GetBoid();
		SpeechText();
	}

	m_pSteeringBoid->SetTargetBoid(target);

	return true;
}

void AiController::SetType(int newType)
{
	m_eType = (newType > MAXTYPE) ? GOODS : (AI_Type)newType;

	if (m_eType == MILITARY)
	{
		TemplatedCollisionCallback<AiController>* AgroCollisionFunction = new TemplatedCollisionCallback<AiController>(this, &AiController::AgrosphereOnCollision);
		agroSphere = new SphereCollider(m_pGameCollision, m_pOwner->GetLocalMatrix(), m_pOwner, AgroCollisionFunction, 10, VectorZero, true, "NoIsland");
	}
	else if (m_eType == AI_Type::GOODS)
	{
		TemplatedCollisionCallback<AiController>* FleeCollisionFunction = new TemplatedCollisionCallback<AiController>(this, &AiController::FleesphereOnCollision);
		agroSphere = new SphereCollider(m_pGameCollision, m_pOwner->GetLocalMatrix(), m_pOwner, FleeCollisionFunction, 10, VectorZero, true, "NoIsland");
	}
}

void AiController::SetWaypoints(std::vector<Transform>* _waypoints)
{
	//early out check
	if (_waypoints == nullptr)
	{
		return;
	}

	//assigning the waypoints
	for each (Transform trans  in (*_waypoints))
	{
		AddWaypoint(positionOf(trans.GetWorldMatrix()));
	}
}

void AiController::SetRandomWaypoints(std::vector<XMFLOAT3>* _waypoints)
{
	//early out check
	if (_waypoints == nullptr)
	{
		return;
	}

	//assigning the waypoints
	unsigned int WaypointCount = (unsigned int)_waypoints->size();
	for (unsigned int i = 0; i < WAYPOINT_LIMIT; i++)
	{
		XMFLOAT3& currentIndex = (*_waypoints)[rand() % WaypointCount];
		if (AddWaypoint(currentIndex) == false)
		{
			--i;
		}
	}

}

void AiController::SetWaypoints(std::vector<XMFLOAT3>* _waypoints)
{
	//early out check
	if (_waypoints == nullptr)
	{
		return;
	}

	//assigning the waypoints
	unsigned int  vectorSize = (unsigned int)_waypoints->size();
	for (unsigned int index = 0; index < vectorSize; index++)
	{
		XMFLOAT3& newPoint = (*_waypoints)[index];

		AddWaypoint(newPoint);

	}

}

bool AiController::AddWaypoint(XMFLOAT3 & point)
{
	//If the waypoints size if greater that tha tof the limit 
	if (m_vWaypoints.size() >= WAYPOINT_LIMIT)
	{
		// do not allow anymore waypoints
		return false;
	}

	//grabbing the sie before hand
	unsigned int maxWaypoint = (unsigned int)m_vWaypoints.size();

	//looping through all my points 
	for (unsigned int i = 0; i < WAYPOINT_LIMIT && i < maxWaypoint; i++)
	{
		// checking for duplicates
		if (m_vWaypoints[i] == point)
		{
			//if so do not allow them into the vector
			return false;
		}
	}

	//setting the y value to mine so that the ai doesnt move in any y direction
	point.y = m_pOwner->GetPosition().y;

	//Pushing the point into my vecotr
	m_vWaypoints.push_back(point);

	return true;
}

void AiController::ClearWaypoints(void)
{
	m_vWaypoints.clear();
}

void AiController::SetMaxFinderTimer(float time)
{
	m_fMaxFinderTimer = time;
}

void AiController::SetAlignment(int _alignment)
{
	//error checking
	if (ALIGNMENT_LIMIT <= _alignment || _alignment < 0)
	{
		return;
	}

	//assigning
	m_nAligment = _alignment;
}

void AiController::ChaseTarget(Ship * owner)
{
	if (SetTarget(owner) == false)
	{

		return;
	}


	m_pGameCollision.trackThis(m_pAttackSegment);
	SetUpAttackSegment();

	m_fAttackTargetFinderTimer = m_fMaxFinderTimer;
}

void AiController::Alert(const EventArgs2<AiController*, Ship*>& args)
{
	//Grabbing the info from the args
	AiController* sender = args.m_Data1;
	Ship* Target = args.m_Data2;


	if (sender == nullptr || Target == nullptr || m_pOwner == nullptr || m_pOwner->ToBeDeleted())
	{
		return;
	}

	//Make sure Im not talking to myself
	if (sender == this)
	{
		return;
	}

	if (Target->GetAlignment() == m_nAligment)
	{
		return;
	}

	float Distance = sqrDistanceBetween(Target->GetPosition(), m_fPosition);

	//Making sure only those in range can hear the alert
	if (Distance > DISTANCE_ALERT)
	{
		return;
	}

	if (m_eCurrentState == SCATTER || m_eCurrentState == ATTACK || m_eCurrentState == ASSIST || m_eCurrentState == ATTACK_STATIONARY)
	{
		return;
	}


	//Find what action to take
	switch (m_eType)
	{
	case AiController::GOODS:
		m_vtAlertLocation = Target->GetPosition();
		FarthestWaypoint();
		m_bHeardAlert = true;
		break;
	case AiController::MILITARY:

		Assist(m_vtAlertLocation, Target);
		//Telling myself that I heard somthing
		m_bHeardAlert = true;
		//grabbing the position of the alert
		m_vtAlertLocation = Target->GetPosition();

		break;
	}
}

void AiController::Scatter(const XMFLOAT3& pos)
{
	//Grabbing the vector for faster look up
	std::vector<XMFLOAT3>&  allWaypoints = ((CGame*)CGame::GetApplication())->GetGameplayScene()->GetWaypoints();

	//Grabbing the size for faster look up
	const unsigned int& vectorSize = 4;//(unsigned int)allWaypoints.size();
	const unsigned int& waypointCount = (unsigned int)m_vWaypoints.size();

	//Declaring before hand so i do not creating anything inside of the loop
	float Distance = 0;

	//Looping through all points 
	for (unsigned int index = 0, waypointIndex = 0; index < vectorSize && waypointIndex < waypointCount; index++)
	{
		//Getting the XMFLOAT3 at that location
		const XMFLOAT3 & newWaypoint = allWaypoints[index];

		//find the ditance between a Waypoint and the location tha is being past in
		Distance = sqrDistanceBetween(newWaypoint, pos);

		//checking to see if that location is in the range 
		if (Distance < DISTANCE_ALERT)
		{
			//if so find a new point
			continue;
		}
		//looping through my waypoints to find a point that is in range of the alert
		for (; waypointIndex < waypointCount; waypointIndex++)
		{
			//Getting the XMFLOAT3 at that location
			XMFLOAT3& myPoint = m_vWaypoints[waypointIndex];

			//find the ditance between myCurrent Waypoint and the location tha is being past in
			Distance = sqrDistanceBetween(myPoint, pos);

			//checking to see if that location is in range 
			if (Distance <= DISTANCE_ALERT*0.5f)
			{
				//if so set your old point to the new point
				myPoint = newWaypoint;
				//updating the index
				waypointIndex++;
				//and move on to a new waypoint
				break;
			}

		}
	}
}

void AiController::Assist(const XMFLOAT3& pos, Ship* myTarget)
{
	//Assigning the target to this controller
	ChaseTarget(myTarget);
}

XMFLOAT3 AiController::ClampVector(const XMFLOAT3 & _vector, const float _MaxLimit)
{
	//if maxLimit is one it will normalize the vector

	XMFLOAT3 tempVector = _vector;

	float max = _MaxLimit / magnitudeOf(_vector);

	//Checks to see if max is less than than one
	max = (max < 1.0f) ? max : 1.0f;

	//Apply max to the vector truncating the vector so its length is equal to MaxLimit
	return tempVector * max;
}

#pragma endregion

AiController::~AiController()
{

	//Clearing all Waypoints I have
	m_vWaypoints.clear();

	//setting the target to null
	SetTarget(nullptr);

	if (agroSphere)
	{
		delete agroSphere;
		agroSphere = nullptr;
	}

	//checking to see if it not null
	if (m_pAttackSegment != nullptr)
	{
		delete m_pAttackSegment;
		m_pAttackSegment = nullptr;
	}

	//Checking to see if it is not null
	if (m_pAvoidanceSegment != nullptr)
	{
		delete m_pAvoidanceSegment;
		m_pAvoidanceSegment = nullptr;
	}

	//Checking to see if it is not null
	if (m_pLeftAvoidanceSegement != nullptr)
	{
		delete m_pLeftAvoidanceSegement;
		m_pLeftAvoidanceSegement = nullptr;
	}

	//Checking to see if it is not null
	if (m_pRightAvoidanceSegement != nullptr)
	{
		delete m_pRightAvoidanceSegement;
		m_pRightAvoidanceSegement = nullptr;
	}

	//Making sure to tell the event manager that I do not want to be on its list
	fsEventManager::GetInstance()->UnRegisterClient("Alert", this, &AiController::Alert);
}