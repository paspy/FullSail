#pragma once
#include "IController.h"
#include <vector>
#include "Transform.h"
#include "Ship.h"

class Collider;
class SegmentCollider;
class CollisionManager;
class SphereCollider;

class AiController :
	public IController
{
public:

	enum AI_States { ATTACK, WAYPOINT, WAIT, FLEE, SCATTER, ASSIST, ATTACK_STATIONARY, MAX_STATE };
	AiController(Ship* owner);
	AiController(const AiController& owner);
	AiController& operator=(const AiController& owner);
	~AiController();
	//*******************Utility********************//
	void Update(float dt);
	const int GetType(void) override { return ENEMY; }
	void FireSideCannons(int side);
	void FireFrontCannons();

	// Adds the waypoint to the vector of waypoints
	bool AddWaypoint(Transform& _newPoint);
	bool AddWaypoint(XMFLOAT3& point);
	void ClearWaypoints(void);


	//*******************Segment Collision Handling********************//
	void AgrosphereOnCollision(Collider & other);
	void FleesphereOnCollision(Collider & other);
	void SegementOnCollisionR(Collider & other);
	void SegementOnCollisionM(Collider & other);
	void SegementOnCollisionL(Collider & other);
	void CannonSightCollision(Collider & other);
	void ToggleRayCollider(bool turnOn, Collider * collider);

	inline const float& GetEngageDistance() { return m_fEngageDistance; }

	//*******************Accessor********************//
	inline const int GetAlignment() { return m_nAligment; }
	inline const float& GetSpeedLimit() { return m_fMaxSpeedLimit; }
	inline const float& GetTurnLimit() { return m_pOwner->GetTurnSpeedLimit(); }
	inline XMFLOAT3& GetPosition() { return m_fPosition; }
	inline XMFLOAT3& GetForward() { return m_fForward; }
	inline const int GetState() { return m_eCurrentState; }
	inline int GetAIType() const { return m_eType; }
	const XMFLOAT4X4& GetWorldMatrix() { return m_pOwner->GetWorldMatrix(); }
	const Ship* GetTarget() { return m_pTarget; }
	const std::vector<XMFLOAT3>& GetWaypoints() { return m_vWaypoints; }

	//*******************Mutator********************//
	bool SetTarget(Ship* _newTarget);
	void SetType(int newType);
	void SetFleeHealth(int fhealth) { m_nFleeHealth = fhealth; };
	void SetMovement(int move) { m_nAccelerating = move; }
	void SetWaypoints(std::vector<Transform>* _waypoints);
	void SetRandomWaypoints(std::vector<XMFLOAT3>* _waypoints);
	void SetWaypoints(std::vector<XMFLOAT3>* _waypoints);
	void SetMaxFinderTimer(float time);
	void SetAlignment(int _alignment);
	void ChaseTarget(Ship*);

	inline void SetEngageDistance(float distance) { m_fEngageDistance = distance; }
	inline void SetSlowDownRadius(float Weight) { m_fSlowDownRadius = Weight; }
	inline void SetAttackDistance(float Weight) { m_fAttackDistance = Weight; }
	inline void SetSideAttackDistance(float Weight) { m_fDistanceSideAttack = Weight; }
	inline void SetAttackPointRange(float Weight) { m_fSideAttackPointRange = Weight; }
	inline void SetDistanceStationary(float distance) { m_fDistanceStationary = distance; }

	inline void SetSideCannonFireDistance(float Weight) { m_fSideCannonAttackDistance = Weight; }
	inline void SetSideCannonAngleLimit(float Weight) { m_fSideCannonAngleLimit = Weight; }
	inline void SetFrontCannonRange(float Weight) { m_fFrontCannonRange = Weight; }

	inline void SetFlockingRadius(float Weight) { m_fFlockRadius = Weight; }
	inline void SetMaxCombinedRadius(float Weight) { m_fMaxCombinedRadius = Weight; }

	inline void SetCohesionWieght(float Weight) { m_fCohesionWieight = Weight; }
	inline void SetAlignmentWieght(float Weight) { m_fAlignmentWieight = Weight; }
	inline void SetSeperationWieght(float Weight) { m_fSeperationWieight = Weight; }
	inline void SetForward(const XMFLOAT3& _vector) { m_fForward = _vector; }

	inline void SetAttackFinderTimer(float time) { m_fAttackTargetFinderTimer = time; }

	//*******************EventMessages********************//
	void Alert(const EventArgs2<AiController*, Ship*>& args);

	//*******************EventHandlerMessages**************************//
	void Scatter(const XMFLOAT3&  pos);
	void Assist(const XMFLOAT3&  pos, Ship* myTarget);

protected:
	//*******************Variables********************//
	bool m_bFleeMode = false;

	CollisionManager& m_pGameCollision;

	Ship* m_pTarget = nullptr;

#pragma region//*******************Collision********************//
	SphereCollider* agroSphere = nullptr;
	SegmentCollider* m_pAttackSegment = nullptr;
	SegmentCollider* m_pAvoidanceSegment = nullptr;
	SegmentCollider* m_pRightAvoidanceSegement = nullptr;
	SegmentCollider* m_pLeftAvoidanceSegement = nullptr;
	XMFLOAT3 m_vtCollisionCorrectionDirection = VectorZero;
	bool m_bCollisionHappened = false;
	bool m_bLeftCollision = false;
	bool m_bRightCollision = false;
	bool stillColliding = false;
	XMFLOAT3 lastDirection = VectorZero;
	Entity3D* m_pLastObject = nullptr;
	bool m_bSameTarget = false;
	XMFLOAT4X4 RightSegment;
	XMFLOAT4X4 LeftSegment;
#pragma endregion

	int m_nAccelerating = 0;

	//Event Handling
		//Alert Function
	XMFLOAT3 m_vtAlertLocation;
	bool m_bHeardAlert = false;

	//Other Varables
	std::vector<XMFLOAT3> m_vWaypoints;

	enum ShootingSide { LEFT, RIGHT, MAX_SIDE };
public:
	enum AI_Type { GOODS = 0, MILITARY, MAXTYPE };
protected:


	AI_States m_eCurrentState = MAX_STATE;
	AI_States m_ePrevState = MAX_STATE;
	AI_Type m_eType = MAXTYPE;
	ShootingSide m_eFiringSide = MAX_SIDE;


	Ship* playerShip = nullptr;
	//Holds the direction of the new target
public:
	std::vector<Ship*>* m_vtAllBoids;
protected:

#pragma region Private Variables
	XMFLOAT3 m_fPosition;
	XMFLOAT3 toTarget;

	int CurrentWaypointIndex;
	int CurrentAttackWaypointIndex;
	float m_fWaypointDistance;

	int prevWaypoint = 0;

	int m_nAligment;
	int	m_nFleeHealth = 0;
	//Movement
	float m_fSlowDownRadius = 16.0f;


	//State Checking
	//Attack
	float m_fAttackDistance = 110.0f;
	float m_fDistanceSideAttack = 30;
	float m_fSideAttackPointRange = 50.0f;
	float m_fDistanceStationary = 25.0f;
	bool  m_bOpenFire = false;

	//Cannon Firing 
	float m_fSideCannonAttackDistance = 30;
	float m_fSideCannonAngleLimit = 0.95f;
	float m_fFrontCannonRange = 0.1f;

	//Flocking Values
	float m_fFlockRadius = 5;
	float m_fMaxCombinedRadius = 16;
	float m_fCohesionWieight = 0;
	float m_fAlignmentWieight = 0;
	float m_fSeperationWieight = 0;

	//*******************Timer********************//
	float m_fAttackTargetFinderTimer = 0.0f;

	float m_fMaxFinderTimer = 20.0f;

	float m_fBoundsTravelTimer = 0;
	float m_fMaxBoundsTime = 10;
	float m_fDistanceAlert = 100;
	float m_fEngageDistance = 0;
#pragma endregion


	//*******************Utility********************//
		//Updates Functions
	void CollisionSetup();
	void StopColliders();
	void StartColliders();
	AI_States UpdateCurrentState();
	void UpdateStartup();
	void UpdateMovement(float dt);
	void UpdateTimers(float dt);
	void UpdateStates();
	XMFLOAT3 CalculateForward(float dt);

	//States
	bool CheckAttackState();
	bool CheckWaypointState();
	bool CheckFleeState();
	bool CheckAlertState();
	bool CheckStationaryState();

	XMFLOAT3 StationaryPositioning(XMFLOAT3 toVector);

	//Flee state
	bool EnemyNearBy();
	void Teleport();
	void FarthestWaypoint();


	XMFLOAT3 VectorToWaypoint();
	bool CloserObject(const XMFLOAT3 & target1, const XMFLOAT3 & target2);
	bool CloserObject(const XMFLOAT3 & target1, const XMFLOAT3 & target2, const XMFLOAT3& position);
	virtual XMFLOAT3 ClampVector(const XMFLOAT3& _vector, const float _MaxLimit = 1);
	XMFLOAT3 CollisionCorrection(float dt);
	void ComeBackIntoWorld();
	void Movement(float dt);
	bool CorrectForwardHeading(XMFLOAT3 & steering);
	bool Regroup();


	//Junshu Code
	void SpeechText();
	void BeggingText();
	void ReferenceCounting(Ship * newTarget);

	//Attacking
	void FireCannons();
	void FaceDirection(const XMFLOAT3& _direction, const float& dt);
	void SetUpAttackSegment();
};

