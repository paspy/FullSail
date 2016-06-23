#include "pch.h"
#include "CargoSquad.h"
#include "Ship.h"
#include "Squad.h"
#include "IScene.h"
#include "Game.h"
#include "SceneManager.h"
#include "IController.h"
#include "AiController.h"
#include "Colliders.h"


#define DESPAWN_DISTANCE 10.0f
CargoSquad::CargoSquad(SceneManager* sceneManager, IScene* currentScene, DirectX::XMFLOAT2 spwanPos, unsigned int numOfGuards)
	:
	m_SceneManager(sceneManager),
	m_CurrentScene(currentScene),
	m_SquadController(newx Squad),
	m_DeathTimer(300.0f)
{
	using namespace DirectX;
	std::vector<Renderer::CRenderable*> renderList;

	XMFLOAT4X4	mat =
		XMFLOAT4X4(1.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f,
			spwanPos.x, 0.0f, spwanPos.y, 1.0f);

	Ship*	cargoShip = (Ship*)sceneManager->GetEntityManager().CreateEntity3D("CargoShip", mat);
	static_cast<BoxCollider*>(cargoShip->GetCollider())->setDimensions(XMFLOAT3(1, 1, 2.2f));
	static_cast<BoxCollider*>(cargoShip->GetCollider())->setOffset(XMFLOAT3(0, 0, 0));
	if (!cargoShip)
		assert(false && "Can't create CargoShip.");
	sceneManager->GetEntityManager().AddEntity(currentScene->GetType(), EntityManager::EntityBucket::RENDERABLE, cargoShip);
	m_Fleet.push_back(cargoShip);
	m_SquadController->AddSquadmate(cargoShip->GetController()->GetBoid());
	renderList.push_back(cargoShip->GetRenderInfo());
	AiController* con = (AiController*)(cargoShip->GetController());
	con->SetEngageDistance(-FLT_MAX);
	con->SetDistanceStationary(-FLT_MAX);
	con->SetMaxFinderTimer(-1.0f);

	for (size_t i = 0; i < numOfGuards; i++)
	{
		Ship*	guardShip = (Ship*)sceneManager->GetEntityManager().CreateEntity3D("CargoGuard", mat);
		static_cast<BoxCollider*>(guardShip->GetCollider())->setDimensions(XMFLOAT3(.76f, 1, 2.5f)); // setup for model swap
		static_cast<BoxCollider*>(guardShip->GetCollider())->setOffset(XMFLOAT3(0, .296f,-.038f)); // setup for model swap
		if (!guardShip)
			assert(false && "Can't create GuardShip.");
		sceneManager->GetEntityManager().AddEntity(currentScene->GetType(), EntityManager::EntityBucket::RENDERABLE, guardShip);
		m_Fleet.push_back(guardShip);
		m_SquadController->AddSquadmate(guardShip->GetController()->GetBoid());
		renderList.push_back(guardShip->GetRenderInfo());
	}
	m_Destination = { -spwanPos.x, 0.0f, -spwanPos.y };	// Test Pos: { 280.909f, 0.0f, -146.952f };
	m_SquadController->AddWaypoint(m_Destination);

	CGame::GetApplication()->m_pRenderer->SetUpRenderables(renderList);
}


CargoSquad::~CargoSquad()
{
	for (auto& fleetMember : m_Fleet)
	{
		if (fleetMember != nullptr)
		{
			fleetMember->SetToBeDeleted(true);
			m_SceneManager->GetEntityManager().RemoveEntity(m_CurrentScene->GetType(), EntityManager::EntityBucket::RENDERABLE, fleetMember);
			delete fleetMember;
			fleetMember = nullptr;
		}
	}
	m_Fleet.clear();

	if (m_SquadController)
	{
		delete m_SquadController;
	}
}

Ship* CargoSquad::GetCargoShip()
{
	//the cargo ship 
	return (m_Fleet.empty()) ? nullptr : m_Fleet[0]; 
}

void CargoSquad::Update(float dt)
{
	if (m_SquadController == nullptr || m_SquadController->GetSquad().size() == 0 || m_SquadController->GetSquad()[0] == nullptr)
		return;



	m_DeathTimer -= dt;
	if (distanceBetween(m_SquadController->GetPosition(), m_Destination) < DESPAWN_DISTANCE && m_DeathTimer <= 0.0f)
	{
		for (auto& fleetMember : m_Fleet)
		{
			// already deleted
			if (fleetMember == nullptr)
				continue;

			// dead
			if (fleetMember->ToBeDeleted() == true)
				continue;

			// kill ship
			fleetMember->SetToBeDeleted(true);
			//if (!fleetMember->ToBeDeleted())
			//{
			//	fleetMember->KillOwner();
			//}
		}
	}

}

//void CargoSquad::DeleteDeadShips()
//{
//	for (size_t i = 0; i < m_Fleet.size(); i++)
//	{
//		if (m_Fleet[i] == nullptr)
//			continue;
//
//		if (m_Fleet[i]->ToBeDeleted() == false)
//			continue;
//
//		// delete ai ship
//		delete m_Fleet[i];
//		m_Fleet[i] = nullptr;
//
//		// erase ai ship
//		auto begin = m_Fleet.begin();
//		m_Fleet.erase(begin + i);
//		i--;
//	}
//}

