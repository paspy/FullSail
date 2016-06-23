#include "pch.h"
#include "BossShip.h"
#include "PlayerShip.h"
#include "AiController.h"
#include "SideCannonHandler.h"
#include "Port.h"
#include "Squad.h"
#include "Game.h"
#include "GameplayScene.h"
#include "fsEventManager.h"
#include "Camera.h"
#include "Colliders.h"


#define CHANGE_DIR_DIST  ((float)9.0f)	//5.0 //6.0


BossShip::BossShip(const XMFLOAT4X4 & _newTransfrom, std::string meshName, std::string textureName) : AIShip(_newTransfrom, meshName.c_str(), textureName.c_str())
{
	m_ShipType = eShipType::TYPE_BOSS;


	// setup 3D HUD icons data
	m_PortCollision = false;
	m_HealIconPos = SimpleMath::Vector4::Zero;
	ID3D11ShaderResourceView *healIcon = CAssetManager::GetRef().Textures("BossHeal.dds");
	m_HealIcon = std::make_unique<Renderer::C3DHUD>(&m_HealIconPos, 1.0f, 1.0f, healIcon);
	m_HealIcon->SetProgression(0.0f);
	m_HealFrameCounter = -1;


	// setup tracking data
	//m_bTrackingCourseSet = true;
	//m_bChangeTrackingCourse = true;
	m_LastPlayerPort = nullptr;
	//m_PortQuadTree = nullptr;



	//fsEventManager* fsem = fsEventManager::GetInstance();
	//fsem->RegisterClient(fsEvents::BossRethinkTrackingCourse,	this, &BossShip::BossRethinkTrackingCourse);
	//fsem->RegisterClient(fsEvents::BossStayOnTrackingCourse,	this, &BossShip::BossStayOnTrackingCourse);
}



//****************Trilogy*****************//

BossShip& BossShip::operator=(const BossShip & other)
{
	// return statement
	if (this == &other)
		return (*this);


	//Calling the base operator
	AIShip::operator=(other);


	m_ShipType = eShipType::TYPE_BOSS;




	// setup 3D HUD icons data
	m_PortCollision = other.m_PortCollision;
	m_HealFrameCounter = other.m_HealFrameCounter;


	// setup tracking data
	//m_bTrackingCourseSet = other.m_bTrackingCourseSet;
	//m_bChangeTrackingCourse = other.m_bChangeTrackingCourse;
	m_LastPlayerPort = other.m_LastPlayerPort;



	//Assigning the data
	return (*this);
}


BossShip::BossShip(const BossShip & other) : AIShip(other), m_ReinforcementTimer(0.0f), m_ReinforcementCount(0)
{
	CGame*			game = static_cast<CGame*>(CGame::GetApplication());
	GameplayScene*	gps = static_cast<GameplayScene*>(game->GetSceneManager()->GetScene(IScene::SceneType::GAMEPLAY));


	m_ShipType = eShipType::TYPE_BOSS;




	// setup 3D HUD icons data
	m_PortCollision = other.m_PortCollision;
	m_HealIconPos = SimpleMath::Vector4::Zero;
	ID3D11ShaderResourceView *healIcon = CAssetManager::GetRef().Textures("BossHeal.dds");
	m_HealIcon = std::make_unique<Renderer::C3DHUD>(&m_HealIconPos, 1.0f, 1.0f, healIcon);
	m_HealIcon->SetProgression(0.0f);
	CGame::GetApplication()->m_pRenderer->Add3DHUD(m_HealIcon.get());
	m_HealFrameCounter = other.m_HealFrameCounter;


	// setup tracking data
	//m_bTrackingCourseSet = other.m_bTrackingCourseSet;
	//m_bChangeTrackingCourse = other.m_bChangeTrackingCourse;
	m_LastPlayerPort = other.m_LastPlayerPort;
	//m_PortQuadTree = new Quadtree<std::wstring>(AABB(DirectX::XMFLOAT2(0.0f, 0.0f), DirectX::XMFLOAT2(150.0f, 150.0f)));
	//m_PortQuadTree->subdivide();
	//size_t numports = gps->GetPorts().size();
	//for (size_t i = 0; i < numports; i++)
	//	m_PortQuadTree->insert(gps->GetPorts()[i]->GetPortName());
}


BossShip::~BossShip()
{
	//fsEventManager* fsem = fsEventManager::GetInstance();
	//fsem->UnRegisterClient(fsEvents::BossRethinkTrackingCourse,	this, &BossShip::BossRethinkTrackingCourse);
	//fsem->UnRegisterClient(fsEvents::BossStayOnTrackingCourse,	this, &BossShip::BossStayOnTrackingCourse);


	CGame::GetApplication()->m_pRenderer->Remove3DHUD(m_HealIcon.get());



	m_LastPlayerPort = nullptr;

	//delete m_PortQuadTree;
	//m_PortQuadTree = nullptr;
}



//**************Functions****************//
/*virtual*/ void BossShip::Update(float dt) /*final*/
{
	if (to_delete)
		return;


	CGame*			game			= static_cast<CGame*>(CGame::GetApplication());
	SoundManager*	soundManager	= game->GetSoundManager();
	SceneManager*	sm				= nullptr;
	GameplayScene*	gps				= nullptr;





	// pop-up text?
	m_PopTextHUD->SetTextPosition({ GetPosition().x, GetPosition().y , GetPosition().z });
	if (m_PopTextTimer > 0.0f)
	{
		m_PopTextHUD->SetScale(m_PopTextHUD->GetScale() + 2.0f*dt);
		m_PopTextTimer -= dt;
		m_PopTextHUD->SetFillColor({ 1.0f, 0.0f, 0.0f, 1.0f });
	}
	else
	{
		m_PopTextHUD->SetFillColor({ 0.0f, 0.0f, 0.0f, 0.0f });
		m_PopTextHUD->SetScale(0.0f);
	}




	// speed update
	if (m_fReducedSpeedTimer <= 0)
	{
		m_fCurrentMaxSpeed = m_fMaxSpeed;
		if (GetComingBackFromRuinedSails())
		{
			m_sailsDownIcon->SetProgression(0.0f);
			m_sailsDown = false;
			FlipComingBackFromRuinedSails();
		}
	}
	m_fReducedSpeedTimer -= dt;




	// death (not)animation
	if (m_DeathStage > eDeathStages::ALIVE)
	{
		AIShip::Update(dt);
		DeathUpdate(dt);
		return;
	}




	// boss reinforcements
	CallReinforcements(dt);




	if (GetPosition().y > 0.0f)
	{
		m_transform.Translate(XMFLOAT3(0.0f, -3.0f*dt, 0.0f), true);
	}
	if (GetPosition().y < 0.0f)
	{
		m_transform.Translate(XMFLOAT3(0.0f, 3.0f*dt, 0.0f), true);
	}




	// hit effects
	if (VFXCountDown > 0.0f)
		VFXCountDown -= dt;
	if (VFXCountDown < 0.0f)
	{
		VFXCountDown = 0.0f;
		m_pRenderInfo->HitEffect(false);
	}

    UpdateParticles();

	////////////////////////////////////////////////////////////////////////////
	//// update particle
	//float speed = m_fSpeed;
	//if (speed <= 0.1f)
	//{
	//	m_vecParticleSet[0]->Stop();
	//	m_vecParticleSet[0]->Reset();
	//}
	//else
	//{
	//	m_vecParticleSet[0]->Play();
	//}
	//m_vecParticleSet[0]->EmitDirectionW(-this->GetForward());
	//m_vecParticleSet[0]->EmitPositionW(this->GetPosition());
	//m_vecParticleSet[0]->EmitVelocity(-this->GetForward()*speed);
	////m_vecParticleSet[2]->EmitPositionW( this->GetPosition() );

	//XMMATRIX ownerWorld = XMMatrixTranslationFromVector(XMLoadFloat3(&XMFLOAT3(0, 0.5f, 0))) * XMLoadFloat4x4(&m_transform.GetWorldMatrix());
	//m_vecParticleSet[1]->WorldMat(ownerWorld);

	float healthPercentage = static_cast<float>(GetHealth()) / static_cast<float>(GetMaxHealth());
	//if (healthPercentage <= 0.3f)
	//{
	//	m_vecParticleSet[1]->Play();
	//}
	//else
	//{
	//	m_vecParticleSet[1]->Stop();
	//	m_vecParticleSet[1]->Reset();
	//}
	//m_vecParticleSet[2]->Stop();




	// boarding process
	if (m_BoardingState > eBoardingState::NONE)
	{
		UpdateBoarding(dt);
		return;
	}



	// port collision update
	if (m_PortCollision == false)
	{
		m_HealFrameCounter = -1;
		m_HealIcon->SetProgression(0.0f);
		m_HealIcon->SetFillColor({ 0.0f, 0.0f, 0.0f, 0.0f });
		soundManager->Play3DSound(AK::EVENTS::STOP_BOSSHEAL);
	}
	m_PortCollision = false;



	// controller update
	if (m_pController != nullptr)
	{
		m_pController->Update(dt);
		Translate(dt);
		//TrackPlayer();
	}




	// base update
	Ship::Update(dt);
	
	sm = game->GetSceneManager();
	gps = static_cast<GameplayScene*>(sm->GetScene(IScene::SceneType::GAMEPLAY));

	// sail down icon update
	XMFLOAT3 camRight = VectorZ;
	XMFLOAT3 camUp;
	crossProduct(gps->GetMainCamera()->LookAtDirection(), camRight, camUp);
	m_sailsDownIconPos = SimpleMath::Vector4(GetPosition().x + camUp.x * 2 + camRight.x*.5f, GetPosition().y + camUp.y * 2 + camRight.y*.5f, GetPosition().z + camUp.z * 2 + camRight.z*.5f, 1.0f);


	//////////////////////////////////////////////////////////////////////////
	// update health bar
	if (gps->SpyglassActive())
	{
		m_crewIconPos = SimpleMath::Vector4(
			GetPosition().x + camUp.x*2.8f,
			GetPosition().y + camUp.y*2.8f,
			GetPosition().z + camUp.z*2.8f,
			1.0f);
		m_cannonIconPos = SimpleMath::Vector4(
			GetPosition().x + camUp.x * 2 - camRight.x*.5f,
			GetPosition().y + camUp.y * 2 - camRight.y*.5f,
			GetPosition().z + camUp.z * 2 - camRight.z*.5f,
			1.0f);
		m_healthBarPos = SimpleMath::Vector4(
			GetPosition().x + camUp.x * 0.75f,
			GetPosition().y + camUp.y * 0.75f,
			GetPosition().z + camUp.z * 0.75f,
			1.0f);
		m_boardableIconPos = SimpleMath::Vector4(
			GetPosition().x + camUp.x * 1.2f - camRight.x * 1.1f,
			GetPosition().y + camUp.y * 1.2f - camRight.y * 1.1f,
			GetPosition().z + camUp.z * 1.2f - camRight.z * 1.1f,
			1.0f);
		m_aggroIconPos = SimpleMath::Vector4(
			GetPosition().x + camUp.x * 1.2f - camRight.x * 1.1f,
			GetPosition().y + camUp.y * 1.2f - camRight.y * 1.1f,
			GetPosition().z + camUp.z * 1.2f + camRight.z * 1.1f,
			1.0f);

		m_healthBar->SetProgression(healthPercentage);
		m_healthBar->SetFillColor(DirectX::SimpleMath::Color::Lerp({ 1.0f,0.0f,0.0f,1.0f }, { 0.0f, 1.0f, 0.0f, 1.0f }, healthPercentage));

		if (gps->GetPlayerShip()) {

			XMFLOAT3 aggroIconOffset = XMFLOAT3(m_aggroIconPos.x, m_aggroIconPos.y, m_aggroIconPos.z) - gps->GetPlayerShip()->GetPosition();
			if (sqrMagnitudeOf(aggroIconOffset) > 100) {
				aggroIconOffset = gps->GetPlayerShip()->GetPosition() + aggroIconOffset * (10 / magnitudeOf(aggroIconOffset));
				m_aggroIconPos.x = aggroIconOffset.x;
				m_aggroIconPos.y = aggroIconOffset.y;
				m_aggroIconPos.z = aggroIconOffset.z;
			}

			float total = static_cast<float>(GetCurrCrew() + gps->GetPlayerShip()->GetCurrCrew());
			float ratio = GetCurrCrew() / total;
			m_crewIcon->SetProgression(1.0f);
			if (ratio < 0.4f)
				m_crewIcon->SetFillColor({ 0.0f,1.0f,1.0f,1.0f });
			else if (ratio > 0.6f)
				m_crewIcon->SetFillColor({ 1.0f,0.0f,0.0f,1.0f });
			else
				m_crewIcon->SetFillColor({ 1.0f,1.0f,0.0f,1.0f });
			m_NumCrew = std::to_wstring(GetCurrCrew());
			m_NumCrew3D->SetTextPosition({ m_crewIconPos.x - camUp.x * .6f + camRight.x * .4f, m_crewIconPos.y - camUp.y * .6f + camRight.y * .4f, m_crewIconPos.z - camUp.z * .6f + camRight.z * .4f });
			m_NumCrew3D->SetFillColor({ 0.0f, 0.0f, 0.0f, 1.0f });
			total = static_cast<float>(GetCurrentMaxSpeed() + gps->GetPlayerShip()->GetCurrentMaxSpeed());
			ratio = GetCurrentMaxSpeed() / total;
			float indexRatio = GetMastLevel() / 5.0f;
			int index = 1;// indexRatio < .35f ? 0 : indexRatio > .65f ? 2 : 1;
			m_sailIcon[index]->SetProgression(1.0f);
			if (ratio < 0.4f)
				m_sailIcon[index]->SetFillColor({ 0.0f,1.0f,1.0f,1.0f });
			else if (ratio > 0.6f)
				m_sailIcon[index]->SetFillColor({ 1.0f,0.0f,0.0f,1.0f });
			else
				m_sailIcon[index]->SetFillColor({ 1.0f,1.0f,0.0f,1.0f });
			total = static_cast<float>(m_cWeapon->GetNumAcquiredCannons() + gps->GetPlayerShip()->GetSideCannonHandler()->GetNumAcquiredCannons());
			ratio = m_cWeapon->GetNumAcquiredCannons() / total;
			//indexRatio = m_cWeapon->GetLevel() / 5.0f;
			index = m_cWeapon->GetNumAcquiredCannons() < 3 ? 0 : m_cWeapon->GetNumAcquiredCannons() > 3 ? 2 : 1;
			m_cannonIcon[index]->SetProgression(1.0f);
			if (ratio < 0.4f)
				m_cannonIcon[index]->SetFillColor({ 0.0f,1.0f,1.0f,1.0f });
			else if (ratio > 0.6f)
				m_cannonIcon[index]->SetFillColor({ 1.0f,0.0f,0.0f,1.0f });
			else
				m_cannonIcon[index]->SetFillColor({ 1.0f,1.0f,0.0f,1.0f });

			m_boardableIcon->SetProgression(1.0f);
			(static_cast<AiController*>(GetController())->GetTarget()) ?
				m_aggroIcon->SetProgression(1.0f)
				: m_aggroIcon->SetProgression(0.0f);
			(m_BoardingState == eBoardingState::UNBOARDABLE) ?
				m_boardableIcon->SetFillColor({ 1.0f, 0.0f, 0.0f, 1.0f })		// red
				: m_boardableIcon->SetFillColor({ 0.0f, 1.0f, 0.0f, 1.0f });	// green
		}

	}
	else {
		m_healthBar->SetProgression(0);
		m_crewIcon->SetProgression(0.0f);
		m_NumCrew = L"";
		m_NumCrew3D->SetProgression(0.0f);
		m_NumCrew3D->SetFillColor({ 0, 0, 0, 0 });
		for (size_t i = 0; i < 3; i++) {
			m_sailIcon[i]->SetProgression(0.0f);
			m_cannonIcon[i]->SetProgression(0.0f);
		}
		m_boardableIcon->SetProgression(0.0f);
		m_boardableIcon->SetFillColor({ 0.0f, 0.0f, 0.0f, 0.0f });
		m_aggroIcon->SetProgression(0.0f);
	}



	// boarding update
	UpdateBoarding(dt);




	// update ship parts
	if (to_delete)
		return;
	m_cWeapon->Update(dt);




	//////////////////////////////////////////////////////////////////////////
	// update health bar
	if (gps->SpyglassActive())
	{
		m_crewIconPos = SimpleMath::Vector4(GetPosition().x + camUp.x*2.8f, GetPosition().y + camUp.y*2.8f, GetPosition().z + camUp.z*2.8f, 1.0f);
		m_cannonIconPos = SimpleMath::Vector4(GetPosition().x + camUp.x * 2 - camRight.x*.5f, GetPosition().y + camUp.y * 2 - camRight.y*.5f, GetPosition().z + camUp.z * 2 - camRight.z*.5f, 1.0f);

		m_healthBarPos = SimpleMath::Vector4(GetPosition().x + camUp.x, GetPosition().y + camUp.y, GetPosition().z + camUp.z, 1.0f);
		m_healthBar->SetProgression(healthPercentage);
		m_healthBar->SetFillColor(DirectX::SimpleMath::Color::Lerp({ 1.0f,0.0f,0.0f,1.0f }, { 0.0f, 1.0f, 0.0f, 1.0f }, healthPercentage));

		m_boardableIconPos.x = GetPosition().x + camUp.x * 1.2f - camRight.x * 1.1f;
		m_boardableIconPos.y = GetPosition().y + camUp.y * 1.2f - camRight.y * 1.1f;
		m_boardableIconPos.z = GetPosition().z + camUp.z * 1.2f - camRight.z * 1.1f;
		m_boardableIconPos.w = 1.0f;

		if (gps->GetPlayerShip())
		{
			float total = static_cast<float>(GetCurrCrew() + gps->GetPlayerShip()->GetCurrCrew());
			float ratio = GetCurrCrew() / total;
			m_crewIcon->SetProgression(1.0f);
			if (ratio < 0.4f)
				m_crewIcon->SetFillColor({ 0.0f,1.0f,1.0f,1.0f });
			else if (ratio > 0.6f)
				m_crewIcon->SetFillColor({ 1.0f,0.0f,0.0f,1.0f });
			else
				m_crewIcon->SetFillColor({ 1.0f,1.0f,0.0f,1.0f });
			m_NumCrew = std::to_wstring(GetCurrCrew());
			m_NumCrew3D->SetTextPosition({ m_crewIconPos.x - camUp.x * .6f + camRight.x * .4f, m_crewIconPos.y - camUp.y * .6f + camRight.y * .4f, m_crewIconPos.z - camUp.z * .6f + camRight.z * .4f });
			m_NumCrew3D->SetFillColor({ 0.0f, 0.0f, 0.0f, 1.0f });
			total = static_cast<float>(GetCurrentMaxSpeed() + gps->GetPlayerShip()->GetCurrentMaxSpeed());
			ratio = GetCurrentMaxSpeed() / total;
			float indexRatio = GetMastLevel() / 5.0f;
			int index = 1;// indexRatio < .35f ? 0 : indexRatio > .65f ? 2 : 1;
			m_sailIcon[index]->SetProgression(1.0f);
			if (ratio < 0.4f)
				m_sailIcon[index]->SetFillColor({ 0.0f,1.0f,1.0f,1.0f });
			else if (ratio > 0.6f)
				m_sailIcon[index]->SetFillColor({ 1.0f,0.0f,0.0f,1.0f });
			else
				m_sailIcon[index]->SetFillColor({ 1.0f,1.0f,0.0f,1.0f });
			total = static_cast<float>(m_cWeapon->GetNumAcquiredCannons() + gps->GetPlayerShip()->GetSideCannonHandler()->GetNumAcquiredCannons());
			ratio = m_cWeapon->GetNumAcquiredCannons() / total;
			//indexRatio = m_cWeapon->GetLevel() / 5.0f;
			index = m_cWeapon->GetNumAcquiredCannons() < 3 ? 0 : m_cWeapon->GetNumAcquiredCannons() > 3 ? 2 : 1;
			m_cannonIcon[index]->SetProgression(1.0f);
			if (ratio < 0.4f)
				m_cannonIcon[index]->SetFillColor({ 0.0f,1.0f,1.0f,1.0f });
			else if (ratio > 0.6f)
				m_cannonIcon[index]->SetFillColor({ 1.0f,0.0f,0.0f,1.0f });
			else
				m_cannonIcon[index]->SetFillColor({ 1.0f,1.0f,0.0f,1.0f });

			m_boardableIcon->SetProgression(1.0f);
			(m_BoardingState == eBoardingState::UNBOARDABLE)
				? m_boardableIcon->SetFillColor({ 1.0f, 0.0f, 0.0f, 1.0f })		// red
				: m_boardableIcon->SetFillColor({ 0.0f, 1.0f, 0.0f, 1.0f });	// green
		}

	}
	else
	{
		m_healthBar->SetProgression(0);
		m_crewIcon->SetProgression(0.0f);
		m_NumCrew = L"";
		m_NumCrew3D->SetProgression(0.0f);
		m_NumCrew3D->SetFillColor({ 0, 0, 0, 0 });
		for (size_t i = 0; i < 3; i++)
		{
			m_sailIcon[i]->SetProgression(0.0f);
			m_cannonIcon[i]->SetProgression(0.0f);
		}
		m_boardableIcon->SetProgression(0.0f);
		m_boardableIcon->SetFillColor({ 0.0f, 0.0f, 0.0f, 0.0f });
	}
}


void BossShip::OnCollision(Collider& other)
{
	// base->OnCollision
	AIShip::OnCollision(other);




	//Retrieving the Object from the void pointer
	Entity3D* CollidedObject = reinterpret_cast<Entity3D*>(other.getID());
	if (to_delete == true || other.isTrigger() && CollidedObject->GetType() == EntityType::SHIP)
		return;



	//CGame*			game	= static_cast<CGame*>(CGame::GetApplication());
	//SceneManager*	sm		= game->GetSceneManager();
	//SoundManager*	sound	= game->GetSoundManager();
	//GameplayScene*	gps		= static_cast<GameplayScene*>(sm->GetScene(IScene::SceneType::GAMEPLAY));
	//GameplayScene*	gps		= static_cast<GameplayScene*>(game->GetSceneManager()->GetScene(IScene::SceneType::GAMEPLAY));



	//Getting the type
	EntityType CollidedType = CollidedObject->GetType();


	//this might need to be changed
	switch (CollidedType)
	{
	case Entity3D::UNKNOWN:
	case Entity3D::SHIP_PART:
	case Entity3D::CRATE:
	case Entity3D::WEATHER:
		// do nothing
		break;


	case Entity3D::PORT:
		m_PortCollision = true;
		HealAtPort(static_cast<Port*>(CollidedObject));
		break;



	case Entity3D::SHIP:
	case Entity3D::AMMO:
	case Entity3D::LAND:
	default:
		// do nothing
		break;
	}


}



//****************************************//
/*Other*/
//****************************************//

void BossShip::KillOwner(void)
{
	if (m_DeathStage > eDeathStages::ALIVE)
		return;

	// base kill owner
	AIShip::KillOwner();

	// remove 3D HUD
	CGame::GetApplication()->m_pRenderer->Remove3DHUD(m_HealIcon.get());
}


void BossShip::CallReinforcements(float dt)
{
	m_ReinforcementTimer -= dt;
	if (GetHealth() > 0 && GetHealth() < GetMaxHealth() * 0.6f && m_ReinforcementTimer <= 0.0f && m_ReinforcementCount < 5)
	{
		fsEventManager* fsem = fsEventManager::GetInstance();
		fsem->FireEvent(fsEvents::SpawnBossReinforcementSquad);
		m_ReinforcementTimer = 30.0f;
		m_ReinforcementCount++;
		m_nHealth = static_cast<int>(GetMaxHealth() * 0.6f);
		m_PopTextTimer = 5.0f;
		m_PopText = L"Reinforcement!";
		CGame*			game = static_cast<CGame*>(CGame::GetApplication());
		SoundManager*	sound = game->GetSoundManager();
		sound->Play3DSound(AK::EVENTS::PLAY_ALERTSOUND);
	}
}


void BossShip::HealAtPort(Port* port)
{
	CGame*			game	= static_cast<CGame*>(CGame::GetApplication());
	SceneManager*	sm		= game->GetSceneManager();
	SoundManager*	sound	= game->GetSoundManager();
	GameplayScene*	gps		= static_cast<GameplayScene*>(sm->GetScene(IScene::SceneType::GAMEPLAY));


	// stop sound replay in other scenes 
	if (sm->GetCurrScene() != gps)
	{
		sound->Play3DSound(AK::EVENTS::STOP_BOSSHEAL);
		return;
	}



	// frame++
	m_HealFrameCounter++;


	// null & HP check
	if (port == nullptr || m_nHealth >= m_nMaxHealth || port->CurrentlyClosed() == true || m_HealFrameCounter > 69)
	{
		m_HealFrameCounter = -1;

		// stop sfx
		sound->Play3DSound(AK::EVENTS::STOP_BOSSHEAL);

		// stop vfx
		m_HealIcon->SetProgression(0.0f);
		m_HealIcon->SetFillColor({ 0.0f, 0.0f, 0.0f, 0.0f });

		// return
		return;
	}

	if ((m_HealFrameCounter % 10) != 0)
		return;



	// HUD stuff
	if (m_HealFrameCounter == 0)
	{
		fsEventManager::GetInstance()->FireEvent(fsEvents::BossHealing);
	}



	// HP++
	m_nHealth += 5;
	if (m_nHealth > m_nMaxHealth)
		m_nHealth = m_nMaxHealth;


	// play sfx
	sound->Play3DSound(AK::EVENTS::PLAY_BOSSHEAL);


	// play vfx
	m_HealIcon->SetProgression(1.0f);
	m_HealIcon->SetFillColor({ 0.0f, 1.0f, 0.0f, 0.0f });


	// camera up
	XMFLOAT3 shippos = GetPosition();
	XMFLOAT3 camRight = VectorZ;
	XMFLOAT3 camUp;
	crossProduct(gps->GetMainCamera()->LookAtDirection(), camRight, camUp);


	// on-screen dimensions
	XMINT2 minmax = XMINT2(-1, 2);
	XMFLOAT2 offsets = XMFLOAT2(0.0f, 0.0f);
	offsets.x = static_cast<float>(rand() % (minmax.y - minmax.x + 1) + minmax.x);
	offsets.y = static_cast<float>(rand() % (minmax.y - minmax.x + 1) + minmax.x);


	// decimal calculation
	XMINT2 minmax2 = XMINT2(-9, 9);
	XMFLOAT2 offsets2 = XMFLOAT2(0.0f, 0.0f);
	offsets2.x = static_cast<float>(rand() % (minmax2.y - minmax2.x + 1) + minmax2.x);
	offsets2.y = static_cast<float>(rand() % (minmax2.y - minmax2.x + 1) + minmax2.x);

	// eg. (4/10) == (4*0.1) == 0.4
	offsets2.x *= 0.1f;
	offsets2.y *= 0.1f;

	// offset the offsets
	offsets.x += offsets2.x;
	offsets.y += offsets2.y;


	// cap offsets
	if (offsets.x < float(minmax.x))
		offsets.x = float(minmax.x);
	if (offsets.x > float(minmax.y))
		offsets.x = float(minmax.y);
	if (offsets.y < float(minmax.x))
		offsets.y = float(minmax.x);
	if (offsets.y > float(minmax.y))
		offsets.y = float(minmax.y);

	// icon pos
	m_HealIconPos.x = shippos.x + camUp.x * offsets.x - camRight.x * offsets.y;
	m_HealIconPos.y = shippos.y + camUp.y * offsets.x - camRight.y * offsets.y;
	m_HealIconPos.z = shippos.z + camUp.z * offsets.x - camRight.z * offsets.y;
	m_HealIconPos.w = 1.0f;
	//m_HealIconPos = SimpleMath::Vector4(shippos.x + camUp.x * offsets.x - camRight.x * offsets.y,
	//									shippos.y + camUp.y * offsets.x - camRight.y * offsets.y,
	//									shippos.z + camUp.z * offsets.x - camRight.z * offsets.y,
	//									1.0f);
}


///* PLEASE DO NOT ERASE YET! */
//void BossShip::TrackPlayer(void)
//{
//	CGame*			game	= static_cast<CGame*>(CGame::GetApplication());
//	GameplayScene*	gps		= static_cast<GameplayScene*>(game->GetSceneManager()->GetScene(IScene::SceneType::GAMEPLAY));
//
//
//	// boss squad
//	Squad* bosssquad = gps->GetBossSquad();
//	if (bosssquad == nullptr)
//		return;
//
//
//	// no waypoints
//	if (bosssquad->GetWaypoints().size() == 0)
//	{
//		// decide where to go
//		fsEventManager::GetInstance()->FireEvent(fsEvents::BossTracking);
//		return;
//	}
//
//
//	// boss & current waypoint positions
//	XMFLOAT3 bosspos, currwaypoint;
//	positionOf(m_transform.GetWorldMatrix(), bosspos);
//	currwaypoint = bosssquad->GetCurrentWaypoint();
//
//	// bad waypoint
//	if (currwaypoint.y == FLT_MAX)
//	{
//		// decide where to go
//		fsEventManager::GetInstance()->FireEvent(fsEvents::BossTracking);
//		return;
//	}
//
//
//	// distance to current waypoint
//	float dist = sqrDistanceBetween(bosspos, currwaypoint); //distanceBetween(bosspos, currwaypoint);
//
//
//	// currently tracking
//	if (m_bTrackingCourseSet == true)
//	{
//		// check dist to waypoint
//		m_bTrackingCourseSet = (dist < (CHANGE_DIR_DIST * CHANGE_DIR_DIST))
//								? false
//								: true;
//	}
//
//
//	// close enough to waypoint to consider a change in course
//	if (m_bTrackingCourseSet == false && m_bChangeTrackingCourse == true)
//	{
//		// try to track player
//		fsEventManager::GetInstance()->FireEvent(fsEvents::BossTracking);
//	}
//
//
//	// too far from waypoint to consider change
//
//}



//****************************************//
/*Events*/
//****************************************//

//void BossShip::BossRethinkTrackingCourse(const EventArgs& args)
//{
//	SetChangeTrackingCourse(true);
//}
//
//
//void BossShip::BossStayOnTrackingCourse(const EventArgs& args)
//{
//	SetChangeTrackingCourse(false);
//}



//**************Accessors****************//



//**************Mutators****************//



//***************Data Members****************//
