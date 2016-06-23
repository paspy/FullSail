#include "pch.h"
#include "AIShip.h"
#include "PlayerShip.h"
#include "AiController.h"
#include "SideCannonHandler.h"
#include "Game.h"
#include "GameplayScene.h"
#include "GameStatistics.h"
#include "Camera.h"
#include "Colliders.h"



AIShip::AIShip(const XMFLOAT4X4 & _newTransfrom, std::string meshName, std::string textureName) : Ship(_newTransfrom, meshName.c_str(), textureName.c_str())
{
	m_Alignment = eAlignment::ENEMY;
	m_ShipType = eShipType::TYPE_AI;


	// setup death (not)animation data
	m_pKiller = nullptr;


	// setup 3D HUD icons data
	m_healthBarPos	= SimpleMath::Vector4::Zero;
	m_healthBar		= std::make_unique<Renderer::C3DHUD>(&m_healthBarPos, 1.0f, 1.0f);
	m_healthBar->SetFillColor({ 0.0f,1.0f,0.0f,1.0f });

	m_sailsDownIconPos			= SimpleMath::Vector4::Zero;
	m_crewIconPos				= SimpleMath::Vector4::Zero;
	m_cannonIconPos				= SimpleMath::Vector4::Zero;
	m_boardableIconPos			= SimpleMath::Vector4::Zero;

	ID3D11ShaderResourceView *sailsDownIcon = CAssetManager::GetRef().Textures("SailsDown.dds");
	m_sailsDownIcon = std::make_unique<Renderer::C3DHUD>(&m_sailsDownIconPos, 1.0f, 1.0f, sailsDownIcon);
	m_sailsDownIcon->SetProgression(0.0f);

	ID3D11ShaderResourceView *crewIcon = CAssetManager::GetRef().Textures("EnemyCrew.dds");
	m_crewIcon = std::make_unique<Renderer::C3DHUD>(&m_crewIconPos, 1.0f, 1.0f, crewIcon);
	m_crewIcon->SetProgression(0.0f);
	m_NumCrew3D = std::make_unique<Renderer::C3DHUD>(&m_NumCrew);
	m_NumCrew3D->SetScale(0.5f);

	ID3D11ShaderResourceView *sailIcon = CAssetManager::GetRef().Textures("EnemySlowSail.dds");
	m_sailIcon[0] = std::make_unique<Renderer::C3DHUD>(&m_sailsDownIconPos, 1.0f, 1.0f, sailIcon);
	m_sailIcon[0]->SetProgression(0.0f);

	sailIcon = CAssetManager::GetRef().Textures("EnemySail.dds");
	m_sailIcon[1] = std::make_unique<Renderer::C3DHUD>(&m_sailsDownIconPos, 1.0f, 1.0f, sailIcon);
	m_sailIcon[1]->SetProgression(0.0f);

	sailIcon = CAssetManager::GetRef().Textures("EnemyFastSail.dds");
	m_sailIcon[2] = std::make_unique<Renderer::C3DHUD>(&m_sailsDownIconPos, 1.0f, 1.0f, sailIcon);
	m_sailIcon[2]->SetProgression(0.0f);

	ID3D11ShaderResourceView *cannonIcon = CAssetManager::GetRef().Textures("EnemyWeakCannon.dds");
	m_cannonIcon[0] = std::make_unique<Renderer::C3DHUD>(&m_cannonIconPos, 1.0f, 1.0f, cannonIcon);
	m_cannonIcon[0]->SetProgression(0.0f);

	cannonIcon = CAssetManager::GetRef().Textures("EnemyCannon.dds");
	m_cannonIcon[1] = std::make_unique<Renderer::C3DHUD>(&m_cannonIconPos, 1.0f, 1.0f, cannonIcon);
	m_cannonIcon[1]->SetProgression(0.0f);

	cannonIcon = CAssetManager::GetRef().Textures("EnemyStrongCannon.dds");
	m_cannonIcon[2] = std::make_unique<Renderer::C3DHUD>(&m_cannonIconPos, 1.0f, 1.0f, cannonIcon);
	m_cannonIcon[2]->SetProgression(0.0f);

	ID3D11ShaderResourceView *boardableIcon = CAssetManager::GetRef().Textures("Boardable.dds");
	m_boardableIcon = std::make_unique<Renderer::C3DHUD>(&m_boardableIconPos, 1.0f, 1.0f, boardableIcon);
	m_boardableIcon->SetProgression(0.0f);

	ID3D11ShaderResourceView *aggroIcon = CAssetManager::GetRef().Textures("AggroIcon.dds");
	m_aggroIcon = std::make_unique<Renderer::C3DHUD>(&m_aggroIconPos, 1.0f, 1.0f, aggroIcon);
	m_aggroIcon->SetProgression(0.0f);
}



//****************Trilogy*****************//

AIShip& AIShip::operator=(const AIShip & other)
{
	// return statement
	if (this == &other)
		return (*this);


	//Calling the base operator
	Ship::operator=(other);


	m_Alignment = eAlignment::ENEMY;
	m_ShipType = eShipType::TYPE_AI;


	// setup death (not)animation data
	m_pKiller = nullptr;


	//Assigning the data
	return (*this);
}


AIShip::AIShip(const AIShip & other) : Ship(other)
{
	m_Alignment = eAlignment::ENEMY;
	m_ShipType = eShipType::TYPE_AI;


	// setup death (not)animation data
	m_pKiller = nullptr;


	// setup 3D HUD icons data
	m_healthBarPos	= SimpleMath::Vector4::Zero;
	m_healthBar		= std::make_unique<Renderer::C3DHUD>(&m_healthBarPos, 1.0f, 0.25f);
	m_healthBar->SetFillColor({ 0.0f,1.0f,0.0f,1.0f });
	CGame::GetApplication()->m_pRenderer->Add3DHUD(m_healthBar.get());

	m_sailsDownIconPos			= SimpleMath::Vector4::Zero;
	m_crewIconPos				= SimpleMath::Vector4::Zero;
	m_cannonIconPos				= SimpleMath::Vector4::Zero;
	m_boardableIconPos			= SimpleMath::Vector4::Zero;

	ID3D11ShaderResourceView *sailsDownIcon = CAssetManager::GetRef().Textures("SailsDown.dds");
	m_sailsDownIcon = std::make_unique<Renderer::C3DHUD>(&m_sailsDownIconPos, 1.0f, 1.0f, sailsDownIcon);
	m_sailsDownIcon->SetProgression(0.0f);
	CGame::GetApplication()->m_pRenderer->Add3DHUD(m_sailsDownIcon.get());

	ID3D11ShaderResourceView *crewIcon = CAssetManager::GetRef().Textures("EnemyCrew.dds");
	m_crewIcon = std::make_unique<Renderer::C3DHUD>(&m_crewIconPos, 1.0f, 1.0f, crewIcon);
	m_crewIcon->SetProgression(0.0f);
	CGame::GetApplication()->m_pRenderer->Add3DHUD(m_crewIcon.get());
	m_NumCrew3D = std::make_unique<Renderer::C3DHUD>(&m_NumCrew);
	CGame::GetApplication()->m_pRenderer->Add3DHUD(m_NumCrew3D.get());
	m_NumCrew3D->SetScale(0.5f);

	ID3D11ShaderResourceView *sailIcon = CAssetManager::GetRef().Textures("EnemySlowSail.dds");
	m_sailIcon[0] = std::make_unique<Renderer::C3DHUD>(&m_sailsDownIconPos, 1.0f, 1.0f, sailIcon);
	m_sailIcon[0]->SetProgression(0.0f);
	CGame::GetApplication()->m_pRenderer->Add3DHUD(m_sailIcon[0].get());

	sailIcon = CAssetManager::GetRef().Textures("EnemySail.dds");
	m_sailIcon[1] = std::make_unique<Renderer::C3DHUD>(&m_sailsDownIconPos, 1.0f, 1.0f, sailIcon);
	m_sailIcon[1]->SetProgression(0.0f);
	CGame::GetApplication()->m_pRenderer->Add3DHUD(m_sailIcon[1].get());

	sailIcon = CAssetManager::GetRef().Textures("EnemyFastSail.dds");
	m_sailIcon[2] = std::make_unique<Renderer::C3DHUD>(&m_sailsDownIconPos, 1.0f, 1.0f, sailIcon);
	m_sailIcon[2]->SetProgression(0.0f);
	CGame::GetApplication()->m_pRenderer->Add3DHUD(m_sailIcon[2].get());

	ID3D11ShaderResourceView *cannonIcon = CAssetManager::GetRef().Textures("EnemyWeakCannon.dds");
	m_cannonIcon[0] = std::make_unique<Renderer::C3DHUD>(&m_cannonIconPos, 1.0f, 1.0f, cannonIcon);
	m_cannonIcon[0]->SetProgression(0.0f);
	CGame::GetApplication()->m_pRenderer->Add3DHUD(m_cannonIcon[0].get());

	cannonIcon = CAssetManager::GetRef().Textures("EnemyCannon.dds");
	m_cannonIcon[1] = std::make_unique<Renderer::C3DHUD>(&m_cannonIconPos, 1.0f, 1.0f, cannonIcon);
	m_cannonIcon[1]->SetProgression(0.0f);
	CGame::GetApplication()->m_pRenderer->Add3DHUD(m_cannonIcon[1].get());

	cannonIcon = CAssetManager::GetRef().Textures("EnemyStrongCannon.dds");
	m_cannonIcon[2] = std::make_unique<Renderer::C3DHUD>(&m_cannonIconPos, 1.0f, 1.0f, cannonIcon);
	m_cannonIcon[2]->SetProgression(0.0f);
	CGame::GetApplication()->m_pRenderer->Add3DHUD(m_cannonIcon[2].get());

	ID3D11ShaderResourceView *boardableIcon = CAssetManager::GetRef().Textures("Boardable.dds");
	m_boardableIcon = std::make_unique<Renderer::C3DHUD>(&m_boardableIconPos, 1.0f, 1.0f, boardableIcon);
	m_boardableIcon->SetProgression(0.0f);
	CGame::GetApplication()->m_pRenderer->Add3DHUD(m_boardableIcon.get());

	ID3D11ShaderResourceView *aggroIcon = CAssetManager::GetRef().Textures("AggroIcon.dds");
	m_aggroIcon = std::make_unique<Renderer::C3DHUD>(&m_aggroIconPos, 1.0f, 1.0f, aggroIcon);
	m_aggroIcon->SetProgression(0.0f);
	CGame::GetApplication()->m_pRenderer->Add3DHUD(m_aggroIcon.get());
}


AIShip::~AIShip()
{
	CGame::GetApplication()->m_pRenderer->Remove3DHUD(m_healthBar.get());
	CGame::GetApplication()->m_pRenderer->Remove3DHUD(m_sailsDownIcon.get());
	CGame::GetApplication()->m_pRenderer->Remove3DHUD(m_crewIcon.get());
	CGame::GetApplication()->m_pRenderer->Remove3DHUD(m_NumCrew3D.get());
	m_NumCrew3D.reset();
	CGame::GetApplication()->m_pRenderer->Remove3DHUD(m_sailIcon[0].get());
	CGame::GetApplication()->m_pRenderer->Remove3DHUD(m_sailIcon[1].get());
	CGame::GetApplication()->m_pRenderer->Remove3DHUD(m_sailIcon[2].get());
	CGame::GetApplication()->m_pRenderer->Remove3DHUD(m_cannonIcon[0].get());
	CGame::GetApplication()->m_pRenderer->Remove3DHUD(m_cannonIcon[1].get());
	CGame::GetApplication()->m_pRenderer->Remove3DHUD(m_cannonIcon[2].get());
	CGame::GetApplication()->m_pRenderer->Remove3DHUD(m_boardableIcon.get());
	CGame::GetApplication()->m_pRenderer->Remove3DHUD(m_aggroIcon.get());


	m_pKiller = nullptr;
}



//**************Functions****************//

void AIShip::Update(float dt)
{
	CGame*			game			= static_cast<CGame*>(CGame::GetApplication());
	SoundManager*	soundManager	= game->GetSoundManager();
	SceneManager*	sm				= nullptr;
	GameplayScene*	gps				= nullptr;


	if (to_delete)
		return;



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
		Ship::Update(dt);
		DeathUpdate(dt);
		return;
	}

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



	// resources update - ONLY FOR PLAYERSHIP
	//UpdateResources(dt);



	// controller update
	if (m_pController != nullptr)
	{
		m_pController->Update(dt);
		Translate(dt);
	}


	// base update
	Ship::Update(dt);


	sm	= game->GetSceneManager();
	gps	= static_cast<GameplayScene*>(sm->GetScene(IScene::SceneType::GAMEPLAY));



	// sail down icon update
	XMFLOAT3 camRight = VectorZ;
	XMFLOAT3 camUp;
	crossProduct(gps->GetMainCamera()->LookAtDirection(), camRight, camUp);
	m_sailsDownIconPos = SimpleMath::Vector4(GetPosition().x + camUp.x * 2 + camRight.x*.5f, GetPosition().y + camUp.y * 2 + camRight.y*.5f, GetPosition().z + camUp.z * 2 + camRight.z*.5f, 1.0f);



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
}


void AIShip::DeathUpdate(float dt)
{
	// regular death sequence
	Ship::DeathUpdate(dt);


	// enemy finished dying after being killed by player
	if (m_DeathStage == eDeathStages::DEATH_DONE && m_pKiller != nullptr && m_pKiller->GetAlignment() == eAlignment::PLAYER)
	{
		// added to lost crew when do boarding.
		/*if (m_BoardingState == eBoardingState::BOARDING_ENEMY) {
		m_pKiller->DecreaseCrew();
		}*/

		// boarding bonus?
		float boardbonus = (m_BoardingState == eBoardingState::BOARDING_ENEMY) ? 2.0f : 1.0f;

		// calculate reputation to give player
		float rep_to_add = boardbonus;
		switch (static_cast<AiController*>(m_pController)->GetAIType())
		{
			//AI_Type::GOODS
		case 0:
			rep_to_add *= 2.0f;	// 2.0f;	//3.0f;
			break;

			//AI_Type::MILITARY
		case 1:
			rep_to_add *= 3.5f;	//5.0f;
			break;
		}

		// give reputation to player
		static_cast<PlayerShip*>(m_pKiller)->AddReputation(rep_to_add);
	}





}


void AIShip::OnCollision(Collider & other)
{
	//Retrieving the Object from the void pointer
	Entity3D* CollidedObject = reinterpret_cast<Entity3D*>(other.getID());
	if (to_delete == true || other.isTrigger() && CollidedObject->GetType() == EntityType::SHIP)
	{
		return;
	}

	//if (m_DeathStage > eDeathStages::ALIVE)
	//	return;


	// port collision
	//m_PortCollision = false;


	CGame*			game	= static_cast<CGame*>(CGame::GetApplication());
	SceneManager*	sm		= game->GetSceneManager();
	SoundManager*	sound	= game->GetSoundManager();
	GameplayScene*	gps		= static_cast<GameplayScene*>(sm->GetScene(IScene::SceneType::GAMEPLAY));



	//Getting the type
	EntityType CollidedType = CollidedObject->GetType();


	//this might need to be changed
	switch (CollidedType)
	{
	case Entity3D::UNKNOWN:
	case Entity3D::SHIP_PART:
	case Entity3D::CRATE:
	case Entity3D::PORT:
	case Entity3D::WEATHER:
		// do nothing
		break;


	case Entity3D::SHIP:
		{
			float dot = dotProduct(CollidedObject->GetForward(), -GetForward());
			float interp = (dot + 1) * .5f;
			SetSpeed(GetSpeed() * (1 - (0.05f * interp)));
		}
		break;


	case Entity3D::AMMO:
		{
			//Get Collided Object
			IAmmo* projectile = static_cast<IAmmo*>(CollidedObject);    // used to be dynamic_cast
			if (projectile == nullptr)
				return;
	
			// Get ammo owner
			Ship*  owner = projectile->GetOwner();
	
			// ammo hit !owner
			if (owner == this || owner->ToBeDeleted() == true)
				return;
	
			// hit effect
			m_pRenderInfo->HitEffect(true);
			VFXCountDown = 0.5f;
	
			// hp--
			eDeathStages prevDeath = m_DeathStage;
			switch (projectile->GetAmmoType())
			{
			case IAmmo::eAmmoType::CANNONBALL:
				TakeDamage(projectile->GetDamage());
				sound->Play3DSound(AK::EVENTS::PLAYHIT);
				//DecreaseCrew();
				break;
			case IAmmo::eAmmoType::CHAIN_SHOT:
				ReduceSpeed();
				TakeDamage(static_cast<int>(projectile->GetDamage() * 0.25f));
				sound->Play3DSound(AK::EVENTS::PLAYBROKENSAILS);
				break;
			default:
				break;
			}
	
			// prepare for death by player
			if (m_DeathStage != eDeathStages::ALIVE)
			{
				// ship sinking
                if ( prevDeath != m_DeathStage ) {
                    if ( std::string( GetTextureListName() ) == "Military" ) {
                        GameStatistics::GetRef().MilitaryShipDestroyed( 1 );
                    } else if ( std::string( GetTextureListName() ) == "Gold" ) {
                        GameStatistics::GetRef().GoldShipDestroyed( 1 );
                    } else if ( std::string( GetTextureListName() ) == "Merchant" ) {
                        GameStatistics::GetRef().FoodShipDestroyed( 1 );
                    }

					sm->SpawnCrate(SceneManager::SpawnCrateEventArgs(owner, this));
                }
	
				// player hit enemy ship
				if (m_Alignment == eAlignment::ENEMY && owner->GetAlignment() == eAlignment::PLAYER)
					m_pKiller = owner;
			}


			// AI ALert event
			AiController * aiship = static_cast<AiController*>(m_pController);
			//Send an event to the ai notifying them that there is a fight starting
			fsEventManager::GetInstance()->FireEvent(fsEvents::AI_Alert, &EventArgs2<AiController*, Ship*>(aiship, owner));
			sound->Play3DSound(AK::EVENTS::PLAY_ALERTSOUND);
			//Once that is sent the  AI will retaliate back for being shoot at
			aiship->ChaseTarget(owner);
		}
		break;


	case Entity3D::LAND:
		SetSpeed(GetSpeed() *0.9f);
		break;
	}


}



//****************************************//
/*Movement*/
//****************************************//

void AIShip::FlipSailsDown()
{
	Ship::FlipSailsDown();

	if (m_sailsDown)
		m_sailsDownIcon->SetProgression(1.0f);
	else
		m_sailsDownIcon->SetProgression(0.0f);
}



//****************************************//
/*Boarding*/
//****************************************//

void AIShip::UpdateBoarding(float dt)
{
	// boarding check
	if (m_BoardingState <= eBoardingState::NONE)
		return;


	// player boarding logic
	EnemyBoarding(dt);
}


void AIShip::EnemyBoarding(float dt)
{
	// Enemy doing player's boarding process check
	if (m_BoardingState != eBoardingState::BOARDING_ENEMY)
		return;

	m_sailsDownIcon->SetProgression(0.0f);
}



//****************************************//
/*Other*/
//****************************************//

void AIShip::KillOwner(void)
{
	if (m_DeathStage > eDeathStages::ALIVE)
		return;

	// base kill owner
	Ship::KillOwner();

	// remove 3D HUD
	CGame::GetApplication()->m_pRenderer->Remove3DHUD(m_healthBar.get());
	CGame::GetApplication()->m_pRenderer->Remove3DHUD(m_sailsDownIcon.get());
	CGame::GetApplication()->m_pRenderer->Remove3DHUD(m_crewIcon.get());
	CGame::GetApplication()->m_pRenderer->Remove3DHUD(m_NumCrew3D.get());
	CGame::GetApplication()->m_pRenderer->Remove3DHUD(m_sailIcon[0].get());
	CGame::GetApplication()->m_pRenderer->Remove3DHUD(m_sailIcon[1].get());
	CGame::GetApplication()->m_pRenderer->Remove3DHUD(m_sailIcon[2].get());
	CGame::GetApplication()->m_pRenderer->Remove3DHUD(m_cannonIcon[0].get());
	CGame::GetApplication()->m_pRenderer->Remove3DHUD(m_cannonIcon[1].get());
	CGame::GetApplication()->m_pRenderer->Remove3DHUD(m_cannonIcon[2].get());
	CGame::GetApplication()->m_pRenderer->Remove3DHUD(m_boardableIcon.get());
}



//***************Data Members****************//

//****************************************//
/* Mast*/
//****************************************//

void AIShip::BeBoarded(SimpleMath::Vector4 & _pos) {
	m_crewIconPos = _pos;
	m_crewIcon->SetProgression(1.0f);

	XMFLOAT3 camRight = VectorZ;
	XMFLOAT3 camUp;
	crossProduct(static_cast<GameplayScene*>(static_cast<CGame*>(CGame::GetApplication())->GetSceneManager()->GetScene(IScene::SceneType::GAMEPLAY))->GetMainCamera()->LookAtDirection(), camRight, camUp);

	m_NumCrew = std::to_wstring(GetCurrCrew());
	m_NumCrew3D->SetTextPosition({ m_crewIconPos.x - camUp.x * .6f + camRight.x * .4f, m_crewIconPos.y - camUp.y * .6f + camRight.y * .4f, m_crewIconPos.z - camUp.z * .6f + camRight.z * .4f });
	m_NumCrew3D->SetFillColor({ 0.0f, 0.0f, 0.0f, 1.0f });
}

void AIShip::FinishBoarding()
{
	m_crewIcon->SetProgression(0.0f);
	m_NumCrew = L"";
	m_NumCrew3D->SetProgression(0.0f);
	m_NumCrew3D->SetFillColor({ 0, 0, 0, 0 });
}

void AIShip::ReduceSpeed()
{
	//Reducing the speed
	m_fCurrentMaxSpeed *= 0.7f;

	// able to board
	if (m_fCurrentMaxSpeed <= BOARDING_SPEED)
	{
		// boarding effects
		// show sail down icon
		if (false == m_sailsDown)
		{
			m_sailsDown = true;
			m_sailsDownIcon->SetProgression(1.0f);
			FlipComingBackFromRuinedSails();
		}
		//// display boarding if in range if not player
		//if (m_owner->GetAlignment() == Ship::eAlignment::ENEMY)
		//{
		//	CGame*			game = (CGame*)CGame::GetApplication();
		//	SceneManager*	sm = game->GetSceneManager();
		//	GameplayScene*	gps = (GameplayScene*)sm->GetScene(IScene::SceneType::GAMEPLAY);
		//
		//	float dist = m_owner->GetDistance(gps->GetPlayerShip());
		//	if (dist < BOARDING_DIST)
		//		gps->TriggerPortCollision();
		//
		//}
		m_PopText = L"Sails Down!";
		m_PopTextTimer = 2.0f;
	}

	//Set timer
	m_fReducedSpeedTimer = m_fMaxTimer;
}


