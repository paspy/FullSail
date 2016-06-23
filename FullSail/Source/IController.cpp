#include "pch.h"
#include "GameplayScene.h"
#include "IController.h"
#include "Game.h"
#include "Ship.h"
#include "Transform.h"
#include "PhysicsManager.h"
#include "Math.h"
#include "IBoid.h"

#define SIZE_FLOAT2 sizeof(XMFLOAT2)
#define SIZE_FLOAT sizeof(float)

IController::IController(Ship* _owner)
	:m_pOwner(nullptr),
	m_cPhysics(nullptr)
	, m_fMaxSpeedLimit((_owner->GetSpeedLimit()))
	, m_fSlowdownMultiplier((_owner->GetSlowDownRate()))
{
	m_fForward = VectorZero;

	//Getting the pointer to the physics system
	CGame* game = (CGame*)CGame::GetApplication();
	SceneManager* gs = game->GetSceneManager();
	GameplayScene* G_playscn = (GameplayScene*)gs->GetScene(IScene::GAMEPLAY);
	m_cPhysics = G_playscn->GetPhysicsManager();

	//Assigning the owner of the controller
	m_pOwner = _owner;

	m_pSteeringBoid = new Boid();
	m_pSteeringBoid->SetOwner(_owner);
}

IController::IController(const IController & other)
	:m_pOwner(nullptr),
	m_cPhysics(nullptr)
	, m_fMaxSpeedLimit((other.m_pOwner->GetSpeedLimit()))
	, m_fSlowdownMultiplier((other.m_pOwner->GetSlowDownRate()))
{
	m_fForward = VectorZero;

	//Getting the pointer to the physics system
	CGame* game = (CGame*)CGame::GetApplication();
	SceneManager* gs = game->GetSceneManager();
	GameplayScene* G_playscn = (GameplayScene*)gs->GetScene(IScene::GAMEPLAY);
	m_cPhysics = G_playscn->GetPhysicsManager();

	//Assigning the owner of the controller
	m_pOwner = other.m_pOwner;
}

IController & IController::operator=(const IController & other)
{
	if (this == &other)
	{
		return *this;
	}

	m_fForward = VectorZero;

	//Getting the pointer to the physics system
	CGame* game = (CGame*)CGame::GetApplication();
	SceneManager* gs = game->GetSceneManager();
	GameplayScene* G_playscn = (GameplayScene*)gs->GetScene(IScene::GAMEPLAY);
	m_cPhysics = G_playscn->GetPhysicsManager();

	//Assigning the owner of the controller
	m_pOwner = other.m_pOwner;

	return *this;
}

IController::~IController()
{
	if (m_pSteeringBoid)
	{
		delete m_pSteeringBoid;
		m_pSteeringBoid = nullptr;
	}


	m_pOwner = nullptr;
}

void IController::Accelerate(const float& dt)
{
	m_pOwner->Accelerate(dt);
}

void IController::Decelerate(const float& dt)
{
	m_pOwner->Decelerate(dt);
}

void IController::Update(float dt)
{

}

void IController::TurnOff()
{
	m_bSTOP = true;
}

Boid * IController::GetBoid()
{
	return m_pSteeringBoid;
}

void IController::StopColliders()
{

}

void IController::StartColliders()
{

}

void IController::SetOwner(Ship * newOwner)
{	
	//no nulls that bad
	if (newOwner == nullptr)
		return;
	
	//Setting the owner
	m_pOwner = newOwner;
	m_pSteeringBoid->SetOwner(newOwner);
}

void IController::SetBoid(Boid * newSteeringBoid)
{
	if (m_pSteeringBoid != nullptr)
	{
		delete m_pSteeringBoid;
		m_pSteeringBoid = nullptr;
	}
}
