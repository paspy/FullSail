#pragma once
class Ship;
class Transform;
class PhysicsManager;
class Boid;

using namespace DirectX;

#define MATH_PI	(3.1415926f)
class IController
{
public:
	IController(Ship* _owner);
	IController(const IController& other);
	IController& operator=(const IController& other);
	enum ControllerType { PLAYER, ENEMY };

	/****************Pure Virtual***************/
	virtual void Update(float dt) = 0;
	virtual const int GetType(void) = 0;
	//******************Utility****************//
	void TurnOff();
	Boid* GetBoid();
	virtual void StopColliders();
	virtual void StartColliders();
	void SetOwner(Ship* newOwner);
	void SetBoid(Boid* newSteeringBoid);
	/***************Destructor******************/
	virtual ~IController();
protected:
	//Boid
	Boid* m_pSteeringBoid;

	Ship* m_pOwner = nullptr;
	PhysicsManager* m_cPhysics = nullptr;
	//Helps with the translation;
	XMFLOAT3 m_fForward;

	//for setting rotation and speed

	//Quick look up for the Limits to the speed and turning speed of the object
	const float& m_fMaxSpeedLimit;
	const float& m_fSlowdownMultiplier;
	bool   m_bSTOP = false;

	void Accelerate(const float& dt);
	void Decelerate(const float& dt);
};


