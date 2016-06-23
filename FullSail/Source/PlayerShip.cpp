#include "pch.h"
#include "PlayerShip.h"
#include "AIShip.h"
#include "PlayerController.h"
#include "AiController.h"
#include "SideCannonHandler.h"
#include "Game.h"
#include "GameStatistics.h"
#include "GameplayScene.h"
#include "Camera.h"
#include "Colliders.h"
#include "Port.h"

PlayerShip::PlayerShip(const XMFLOAT4X4 & _newTransfrom, std::string meshName, std::string textureName) : Ship(_newTransfrom, meshName, textureName)
{
	m_Alignment = eAlignment::PLAYER;
	m_ShipType = eShipType::TYPE_PLAYER;


	// setup other data
	m_fResourceTimer = RESOURCE_TIME;
	m_bDecreasingRes = false;


	// setup other data
	m_fBufferRep = 0.0f;
	m_nBufferRum = 0;
	m_nBufferWaterFood = 0;
	m_nBufferGold = 0;
	m_fRumResourceTimer = 0.0f;
	m_fWaterFoodResourceTimer = 0.0f;
	m_fGoldResourceTimer = 0.0f;
	m_fCrewResourceTimer = 0.0f;


	// setup porting data
	m_PortCollision = false;
	m_CurrPort = L"";
	m_RecentPorts[0] = nullptr;
	m_RecentPorts[1] = nullptr;
	m_RecentPorts[2] = nullptr;


	// setup boarding data
	m_fBoardingTimer = STARTING_BOARDING_TIME;
	m_pBoardingTarget = nullptr;
	m_pBoardingIter = -1;
	m_boardingTargetPos = { 0.0f, 0.0f, 0.0f, 0.0f };


	// setup boarding fight icons
	m_swordsCrossingTimer = MAX_BOARDING_ICON_TIMER;
	m_swordsCrossedActive = true;
	m_swordsCrossingPos = SimpleMath::Vector4::Zero;
	ID3D11ShaderResourceView *swordsCrossedIcon = CAssetManager::GetRef().Textures("CrossedSwords.dds");
	m_swordsCrossed = std::make_unique<Renderer::C3DHUD>(&m_swordsCrossingPos, 1.0f, 1.0f, swordsCrossedIcon);
	m_swordsCrossed->SetProgression(0.0f);
	ID3D11ShaderResourceView *swordsUncrossedIcon = CAssetManager::GetRef().Textures("UncrossedSwords.dds");
	m_swordsUncrossed = std::make_unique<Renderer::C3DHUD>(&m_swordsCrossingPos, 1.0f, 1.0f, swordsUncrossedIcon);
	m_swordsUncrossed->SetProgression(0.0f);

	m_boardBarPos = SimpleMath::Vector4::Zero;
	m_boardBackBar = std::make_unique<Renderer::C3DHUD>(&m_boardBarPos, 2.0f, 0.25f);
	m_boardBackBar->SetFillColor({ 0.0f,1.0f,0.0f,1.0f });
	m_boardBar = std::make_unique<Renderer::C3DHUD>(&m_boardBarPos, 2.0f, 0.25f);
	m_boardBar->SetFillColor({ 0.0f,1.0f,0.0f,1.0f });
	m_pirateAboardPos = SimpleMath::Vector4::Zero;
	ID3D11ShaderResourceView *pirateAboardIcon = CAssetManager::GetRef().Textures("PirateAboard.dds");
	m_pirateAboard = std::make_unique<Renderer::C3DHUD>(&m_pirateAboardPos, 0.8f, 0.8f, pirateAboardIcon);
	m_pirateAboard->SetFillColor({ 1.0f,1.0f,1.0f,1.0f });
	m_navyAboardPos = SimpleMath::Vector4::Zero;
	ID3D11ShaderResourceView *navyAboardIcon = CAssetManager::GetRef().Textures("NavyAboard.dds");
	m_navyAboard = std::make_unique<Renderer::C3DHUD>(&m_navyAboardPos, 0.8f, 0.8f, navyAboardIcon);
	m_navyAboard->SetFillColor({ 1.0f,1.0f,1.0f,1.0f });

	ID3D11ShaderResourceView *crewIcon = CAssetManager::GetRef().Textures("EnemyCrew.dds");
	m_crewIcon = std::make_unique<Renderer::C3DHUD>(&m_pirateAboardPos, 1.0f, 1.0f, crewIcon);
	m_crewIcon->SetProgression(1.0f);
	m_NumCrew3D = std::make_unique<Renderer::C3DHUD>(&m_NumCrew);
	m_NumCrew3D->SetScale(0.5f);


	// setup 3D HUD for crew decreasing
	ID3D11ShaderResourceView *stickerRedSRV = CAssetManager::GetRef().Textures("3DHUD_pirate_sticker_red.dds");
	ID3D11ShaderResourceView *stickerBlueSRV = CAssetManager::GetRef().Textures("3DHUD_pirate_sticker_blue.dds");
	m_crewStickerRedPos = SimpleMath::Vector4::Zero;
	m_crewStickerRed = std::make_unique<Renderer::C3DHUD>(&m_crewStickerRedPos, 1.0f, 1.0f, stickerRedSRV);
	m_crewStickerRed->SetProgression(0.0f);
	m_crewStickerBluePos = SimpleMath::Vector4::Zero;
	m_crewStickerBlue = std::make_unique<Renderer::C3DHUD>(&m_crewStickerBluePos, 1.0f, 1.0f, stickerBlueSRV);
	m_crewStickerBlue->SetProgression(0.0f);


	// setup cheat data
	m_bGodMode = false;


	// setup win non-pop data
	m_fWinTimer = 8.0f;
	m_fWindt = 0.0f;

	SetupParticles();
}



//****************Trilogy*****************//

PlayerShip& PlayerShip::operator=(const PlayerShip & other)
{
	// return statement
	if (this == &other)
		return (*this);


	//Calling the base operator
	Ship::operator=(other);


	m_Alignment = eAlignment::PLAYER;
	m_ShipType = eShipType::TYPE_PLAYER;


	// setup other data
	m_fResourceTimer = other.m_fResourceTimer;


	// setup other data
	m_fBufferRep = other.m_fBufferRep;
	m_nBufferRum = other.m_nBufferRum;
	m_nBufferWaterFood = other.m_nBufferWaterFood;
	m_nBufferGold = other.m_nBufferGold;
	m_fRumResourceTimer = other.m_fRumResourceTimer;
	m_fWaterFoodResourceTimer = other.m_fWaterFoodResourceTimer;
	m_fGoldResourceTimer = other.m_fGoldResourceTimer;
	m_fCrewResourceTimer = other.m_fCrewResourceTimer;


	// setup porting data
	m_PortCollision = other.m_PortCollision;
	m_CurrPort = other.m_CurrPort;
	m_RecentPorts[0] = other.m_RecentPorts[0];
	m_RecentPorts[1] = other.m_RecentPorts[1];
	m_RecentPorts[2] = other.m_RecentPorts[2];


	// setup boarding data
	m_fBoardingTimer = other.m_fBoardingTimer;
	m_pBoardingTarget = other.m_pBoardingTarget;
	m_pBoardingIter = other.m_pBoardingIter;
	m_pBoardingAllTargets = other.m_pBoardingAllTargets;
	m_boardingTargetPos = other.m_boardingTargetPos;


	// setup cheat data
	m_bGodMode = other.m_bGodMode;


	// setup win non-pop data
	m_fWinTimer = other.m_fWinTimer;
	m_fWindt = other.m_fWindt;

	SetupParticles();
	//Assigning the data
	return (*this);
}


PlayerShip::PlayerShip(const PlayerShip & other) : Ship(other)
{
	m_Alignment = eAlignment::PLAYER;
	m_ShipType = eShipType::TYPE_PLAYER;


	// setup other data
	m_fResourceTimer = other.m_fResourceTimer;


	// setup other data
	m_fBufferRep = other.m_fBufferRep;
	m_nBufferRum = other.m_nBufferRum;
	m_nBufferWaterFood = other.m_nBufferWaterFood;
	m_nBufferGold = other.m_nBufferGold;
	m_fRumResourceTimer = other.m_fRumResourceTimer;
	m_fWaterFoodResourceTimer = other.m_fWaterFoodResourceTimer;
	m_fGoldResourceTimer = other.m_fGoldResourceTimer;
	m_fCrewResourceTimer = other.m_fCrewResourceTimer;


	// setup porting data
	m_PortCollision = other.m_PortCollision;
	m_CurrPort = other.m_CurrPort;
	m_RecentPorts[0] = other.m_RecentPorts[0];
	m_RecentPorts[1] = other.m_RecentPorts[1];
	m_RecentPorts[2] = other.m_RecentPorts[2];


	// setup boarding data
	m_fBoardingTimer = other.m_fBoardingTimer;
	m_pBoardingTarget = other.m_pBoardingTarget;
	m_pBoardingIter = other.m_pBoardingIter;
	m_pBoardingAllTargets = other.m_pBoardingAllTargets;
	m_boardingTargetPos = other.m_boardingTargetPos;

	// Boarding fight icons
	m_swordsCrossingTimer = other.m_swordsCrossingTimer;
	m_swordsCrossedActive = other.m_swordsCrossedActive;
	m_swordsCrossingPos = other.m_swordsCrossingPos;
	ID3D11ShaderResourceView *swordsCrossedIcon = CAssetManager::GetRef().Textures("CrossedSwords.dds");
	m_swordsCrossed = std::make_unique<Renderer::C3DHUD>(&m_swordsCrossingPos, 1.0f, 1.0f, swordsCrossedIcon);
	m_swordsCrossed->SetProgression(0.0f);
	ID3D11ShaderResourceView *swordsUncrossedIcon = CAssetManager::GetRef().Textures("UncrossedSwords.dds");
	m_swordsUncrossed = std::make_unique<Renderer::C3DHUD>(&m_swordsCrossingPos, 1.0f, 1.0f, swordsUncrossedIcon);
	m_swordsUncrossed->SetProgression(0.0f);
	CGame::GetApplication()->m_pRenderer->Add3DHUD(m_swordsCrossed.get());
	CGame::GetApplication()->m_pRenderer->Add3DHUD(m_swordsUncrossed.get());

	m_boardBarPos = SimpleMath::Vector4::Zero;
	m_boardBarPos.w = 1;
	m_boardBackBar = std::make_unique<Renderer::C3DHUD>(&m_boardBarPos, 2.0f, 0.25f);
	m_boardBackBar->SetFillColor({ 1.0f,0.0f,0.0f,1.0f });
	CGame::GetApplication()->m_pRenderer->Add3DHUD(m_boardBackBar.get());
	m_boardBar = std::make_unique<Renderer::C3DHUD>(&m_boardBarPos, 2.0f, 0.25f);
	m_boardBar->SetFillColor({ 0.0f,1.0f,0.0f,1.0f });
	CGame::GetApplication()->m_pRenderer->Add3DHUD(m_boardBar.get());
	m_pirateAboardPos = SimpleMath::Vector4::Zero;
	ID3D11ShaderResourceView *pirateAboardIcon = CAssetManager::GetRef().Textures("PirateAboard.dds");
	m_pirateAboard = std::make_unique<Renderer::C3DHUD>(&m_pirateAboardPos, 0.8f, 0.8f, pirateAboardIcon);
	m_pirateAboard->SetFillColor({ 1.0f,1.0f,1.0f,1.0f });
	CGame::GetApplication()->m_pRenderer->Add3DHUD(m_pirateAboard.get());
	m_navyAboardPos = SimpleMath::Vector4::Zero;
	ID3D11ShaderResourceView *navyAboardIcon = CAssetManager::GetRef().Textures("NavyAboard.dds");
	m_navyAboard = std::make_unique<Renderer::C3DHUD>(&m_navyAboardPos, 0.8f, 0.8f, navyAboardIcon);
	m_navyAboard->SetFillColor({ 1.0f,1.0f,1.0f,1.0f });
	CGame::GetApplication()->m_pRenderer->Add3DHUD(m_navyAboard.get());

	ID3D11ShaderResourceView *crewIcon = CAssetManager::GetRef().Textures("EnemyCrew.dds");
	m_crewIcon = std::make_unique<Renderer::C3DHUD>(&m_pirateAboardPos, 1.0f, 1.0f, crewIcon);
	m_crewIcon->SetProgression(1.0f);
	m_NumCrew3D = std::make_unique<Renderer::C3DHUD>(&m_NumCrew);
	m_NumCrew3D->SetScale(0.5f);
	CGame::GetApplication()->m_pRenderer->Add3DHUD(m_crewIcon.get());
	CGame::GetApplication()->m_pRenderer->Add3DHUD(m_NumCrew3D.get());


	// setup 3D HUD for crew decreasing
	ID3D11ShaderResourceView *stickerRedSRV = CAssetManager::GetRef().Textures("3DHUD_pirate_sticker_red.dds");
	ID3D11ShaderResourceView *stickerBlueSRV = CAssetManager::GetRef().Textures("3DHUD_pirate_sticker_blue.dds");
	m_crewStickerRedPos = SimpleMath::Vector4::Zero;
	m_crewStickerRed = std::make_unique<Renderer::C3DHUD>(&m_crewStickerRedPos, 1.0f, 1.0f, stickerRedSRV);
	m_crewStickerRed->SetProgression(0.0f);
	CGame::GetApplication()->m_pRenderer->Add3DHUD(m_crewStickerRed.get());
	m_crewStickerBluePos = SimpleMath::Vector4::Zero;
	m_crewStickerBlue = std::make_unique<Renderer::C3DHUD>(&m_crewStickerBluePos, 1.0f, 1.0f, stickerBlueSRV);
	m_crewStickerBlue->SetProgression(0.0f);
	CGame::GetApplication()->m_pRenderer->Add3DHUD(m_crewStickerBlue.get());


	// setup cheat data
	m_bGodMode = other.m_bGodMode;


	// setup win non-pop data
	m_fWinTimer = other.m_fWinTimer;
	m_fWindt = other.m_fWindt;
	SetupParticles();

}


PlayerShip::~PlayerShip()
{
	// release boarding icons
	CGame::GetApplication()->m_pRenderer->Remove3DHUD(m_swordsCrossed.get());
	CGame::GetApplication()->m_pRenderer->Remove3DHUD(m_swordsUncrossed.get());
	CGame::GetApplication()->m_pRenderer->Remove3DHUD(m_crewStickerRed.get());
	CGame::GetApplication()->m_pRenderer->Remove3DHUD(m_crewStickerBlue.get());
	CGame::GetApplication()->m_pRenderer->Remove3DHUD(m_boardBackBar.get());
	CGame::GetApplication()->m_pRenderer->Remove3DHUD(m_boardBar.get());
	CGame::GetApplication()->m_pRenderer->Remove3DHUD(m_pirateAboard.get());
	CGame::GetApplication()->m_pRenderer->Remove3DHUD(m_navyAboard.get());
	CGame::GetApplication()->m_pRenderer->Remove3DHUD(m_crewIcon.get());
	CGame::GetApplication()->m_pRenderer->Remove3DHUD(m_NumCrew3D.get());


	// release port data
	m_RecentPorts[0] = nullptr;
	m_RecentPorts[1] = nullptr;
	m_RecentPorts[2] = nullptr;


	// release boarding data
	m_pBoardingTarget = nullptr;
	m_pBoardingIter = -1;
	m_pBoardingAllTargets.clear();
}



//**************Functions****************//

void PlayerShip::Update(float dt)
{
	CGame*			game = static_cast<CGame*>(CGame::GetApplication());
	SoundManager*	soundManager = game->GetSoundManager();



	if (to_delete)
		return;
	static float rotation = 0.0f;

	//XMMATRIX ownerWorld = XMMatrixTranslationFromVector(XMLoadFloat3(&XMFLOAT3(0, 0.5f, 0))) * XMLoadFloat4x4(&m_transform.GetWorldMatrix());

	// win timer
	m_fWindt = dt;



	// pop-up text?
	m_PopTextHUD->SetTextPosition({ GetPosition().x, GetPosition().y , GetPosition().z });
	if (m_PopTextTimer > 0.0f)
	{
		m_PopTextHUD->SetScale(m_PopTextHUD->GetScale() + 2.0f * dt);
		m_PopTextTimer -= dt;
		m_PopTextHUD->SetFillColor({ 0.0f, 0.0f, 1.0f, 1.0f });	// { 1.0, 0.5f, 0.0f, 1.0f }
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
			m_sailsDown = false;
			FlipComingBackFromRuinedSails();
		}
	}
	m_fReducedSpeedTimer -= dt;



	// death (not)animation
	if (m_DeathStage > eDeathStages::ALIVE)
	{
		if (GameSetting::GetRef().GamePadIsConnected())
			GamePad::Get().SetVibration(0, 0, 0);

		auto camera = CGame::GetApplication()->m_pRenderer->GetMainCamera();
		if (camera)
		{
			camera->SetCameraShakeAmount(0.0f);
			camera->SetCameraShakeDuration(0.0f);
		}
		Ship::Update(dt);
		DeathUpdate(dt);
		return;
	}



	// hit effects
	if (VFXCountDown > 0.0f)
		VFXCountDown -= dt;
	if (VFXCountDown < 0.0f)
	{
		VFXCountDown = 0.0f;
		m_pRenderInfo->HitEffect(false);

		if (GameSetting::GetRef().GamePadIsConnected())
			GamePad::Get().SetVibration(0, 0, 0);

		auto camera = CGame::GetApplication()->m_pRenderer->GetMainCamera();
		if (camera)
		{
			camera->SetCameraShakeAmount(0.0f);
			camera->SetCameraShakeDuration(0.0f);
		}
	}

    UpdateParticles();

    ////////////////////////////////////////////////////////////////////////////
    //// update particle
    //float speed = m_fSpeed;
    //if ( speed <= 0.1f ) {
    //    m_vecParticleSet[0]->Stop();
    //    m_vecParticleSet[0]->Reset();
    //    m_vecParticleSet[3]->Stop();
    //    m_vecParticleSet[3]->Reset();
    //} else {
    //    m_vecParticleSet[0]->Play();
    //    m_vecParticleSet[3]->Play();
    //}
    //m_vecParticleSet[0]->EmitDirectionW( -this->GetForward() );
    ////m_vecParticleSet[0]->EmitPositionW( DirectX::SimpleMath::Vector3( GetPosition().x, GetPosition().y, GetPosition().z) );
    //XMMATRIX playerWorld = XMMatrixTranslationFromVector( XMLoadFloat3( &XMFLOAT3( 0.25f, 0.0f, 0.5f ) ) ) * XMLoadFloat4x4( &m_transform.GetWorldMatrix() );
    //m_vecParticleSet[0]->EmitSizeW( { 0.5f, 0.5f } );
    //m_vecParticleSet[0]->WorldMat( playerWorld );
    ////m_vecParticleSet[0]->EmitVelocity( -this->GetForward() * 0 );
    //m_vecParticleSet[0]->EmitVelocity( this->GetRight() *speed *0.025f );
    //m_vecParticleSet[0]->EmitSpeed( speed );

    //m_vecParticleSet[3]->EmitDirectionW( -this->GetForward() );
    //playerWorld = XMMatrixTranslationFromVector( XMLoadFloat3( &XMFLOAT3( -0.25f, 0.0f, 0.5f ) ) ) * XMLoadFloat4x4( &m_transform.GetWorldMatrix() );
    //m_vecParticleSet[3]->EmitSizeW( { 0.5f, 0.5f } );
    //m_vecParticleSet[3]->WorldMat( playerWorld );
    ////m_vecParticleSet[3]->EmitVelocity( -this->GetForward() * 0 );
    //m_vecParticleSet[3]->EmitVelocity( -this->GetRight() *speed*0.025f );
    //m_vecParticleSet[3]->EmitSpeed( speed );

    ////m_vecParticleSet[2]->EmitPositionW( this->GetPosition() );

    //m_vecParticleSet[1]->WorldMat( ownerWorld );

    //float healthPercentage = static_cast<float>( GetHealth() ) / static_cast<float>( GetMaxHealth() );
    //if ( healthPercentage <= 0.3f ) {
    //    m_vecParticleSet[1]->Play();
    //} else {
    //    m_vecParticleSet[1]->Stop();
    //    m_vecParticleSet[1]->Reset();
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
		m_CurrPort = L"";
	m_PortCollision = false;



	// resources update
	UpdateResources(dt);



	// controller update
	if (m_pController != nullptr)
	{
		m_pController->Update(dt);
		Translate(dt);
	}

	// base update
	Ship::Update(dt);

	static_cast<G2D::GUIText*>(static_cast<GameplayScene*>(game->GetSceneManager()->GetScene(IScene::SceneType::GAMEPLAY))->m_spyElems["CrewNumber"])->Text(std::to_wstring(m_nCurrCrew));


	// boarding update
	UpdateBoarding(dt);



	// update ship parts
	if (to_delete)
		return;

	m_cWeapon->Update(dt);
}


bool PlayerShip::LevelUp(IShipPart::PartType partType)
{
	switch (partType)
	{
	case IShipPart::PartType::UNKNOWN:
	case IShipPart::PartType::HULL:
	case IShipPart::PartType::MAST:
		return Ship::LevelUp(partType);


	case IShipPart::PartType::WEAPON:
	{
		bool	rtn = Ship::LevelUp(partType);
		int		level = m_cWeapon->GetLevel();

		CGame*			game = static_cast<CGame*>(CGame::GetApplication());
		SceneManager*	sm = game->GetSceneManager();
		GameplayScene*	gps = static_cast<GameplayScene*>(sm->GetScene(IScene::SceneType::GAMEPLAY));

		if (level <= 2)
		{
			gps->m_spyElems["Player_Weak_Cannons"]->Active(true);
			gps->m_spyElems["Player_Cannons"]->Active(false);
			gps->m_spyElems["Player_Strong_Cannons"]->Active(false);
		}
		else if (level > 2 && level < 4)
		{
			gps->m_spyElems["Player_Weak_Cannons"]->Active(false);
			gps->m_spyElems["Player_Cannons"]->Active(true);
			gps->m_spyElems["Player_Strong_Cannons"]->Active(false);
		}
		else
		{
			gps->m_spyElems["Player_Weak_Cannons"]->Active(false);
			gps->m_spyElems["Player_Cannons"]->Active(false);
			gps->m_spyElems["Player_Strong_Cannons"]->Active(true);
		}
		return rtn;
	}
	break;

	default:
		break;
	}


	return false;
}


void PlayerShip::OnCollision(Collider & other)
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


	CGame*			game = static_cast<CGame*>(CGame::GetApplication());
	SceneManager*	sm = game->GetSceneManager();
	SoundManager*	sound = game->GetSoundManager();
	GameplayScene*	gps = static_cast<GameplayScene*>(sm->GetScene(IScene::SceneType::GAMEPLAY));



	//Getting the type
	EntityType CollidedType = CollidedObject->GetType();


	//this might need to be changed
	switch (CollidedType)
	{
	case Entity3D::UNKNOWN:
	case Entity3D::SHIP_PART:
	case Entity3D::WEATHER:
		// do nothing
		break;


	case Entity3D::SHIP:
	{
		float dot = dotProduct(CollidedObject->GetForward(), -GetForward());
		float interp = (dot + 1) * .5f;
		SetSpeed(GetSpeed() * (1 - (0.05f * interp)));

		if (static_cast<Ship*>(CollidedObject)->GetDeathStage() == eDeathStages::ALIVE && m_fSpeed != 0)
			sound->Play3DSound(AK::EVENTS::PLAY_SHIPCOLLISION);
	}
	break;


	case Entity3D::CRATE:
		if (sm->GetCurrScene()->GetType() == IScene::SceneType::GAMEPLAY)
			sound->Play3DSound(AK::EVENTS::PLAYCRATEPICKUP);
		break;


	case Entity3D::PORT:
	{
		Port* port = static_cast<Port*>(CollidedObject);   // used to be dynamic_cast

		m_PortCollision = true;
		m_CurrPort = port->GetPortName();
		gps->TriggerPortCollision();

		if (m_RecentPorts[0] != port)
		{
			m_RecentPorts[2] = m_RecentPorts[1];
			m_RecentPorts[1] = m_RecentPorts[0];
			m_RecentPorts[0] = port;
		}
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
		if (GameSetting::GetRef().GamePadIsConnected())
			GamePad::Get().SetVibration(0, 0.2f, 0.5f);
		auto camera = CGame::GetApplication()->m_pRenderer->GetMainCamera();
		if (camera)
		{
			camera->SetCameraShakeAmount(0.1f);
			camera->SetCameraShakeDuration(0.00002f);
		}
		VFXCountDown = 0.5f;

		// hp--
		eDeathStages prevDeath = m_DeathStage;
		if (m_bGodMode == false)
		{
			switch (projectile->GetAmmoType())
			{
			case IAmmo::eAmmoType::CANNONBALL:
				TakeDamage(projectile->GetDamage());
				sound->Play3DSound(AK::EVENTS::PLAY_PLAYERSHIPHIT);
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
		}
	}
	break;



	case Entity3D::LAND:
		SetSpeed(GetSpeed() *0.9f);
		if (GetAlignment() == eAlignment::PLAYER && m_fSpeed > 0)
			sound->Play3DSound(AK::EVENTS::PLAY_LANDCOLLISION);
		break;
	}


}


void PlayerShip::LoadPlayer(void)
{
	// load player progression data
}


//****************************************//
/*Movement*/
//****************************************//

void PlayerShip::Accelerate(const float& dt)
{
	float distanceToMaxSpeed = m_fCurrentMaxSpeed - m_fSpeed;
	AdjustSpeed(PlayerShip::SpeedBonus(dt, m_fAcceleration, distanceToMaxSpeed));
}



//****************************************//
/*Bonus*/
//****************************************//

float PlayerShip::SpeedBonus(float dt, float accel, float distToMax)
{
	// standard
	float no_boost = distToMax * accel * dt;

	// no food
	if (m_nCurrWaterFood <= 0.0f)
		return no_boost;

	// boost!
	float boost = (static_cast<float>(m_nCurrWaterFood) / static_cast<float>(m_nMaxWaterFood)) * 4.0f;

	// boosted!
	boost *= no_boost;

	// return
	return boost;
}


float PlayerShip::BoardingBonus(float dt)
{
	// standard
	float no_boost = dt;

	// no food
	if (m_nCurrRum <= 0.0f)
		return no_boost;

	// boost!
	float boost = (static_cast<float>(m_nCurrRum) / static_cast<float>(m_nMaxRum)) * 0.25f;

	// boosted!
	boost = boost * no_boost + no_boost;

	// return
	return boost;
}


float PlayerShip::CooldownBonus(float dt)
{
	// standard
	float no_boost = dt;

	// no food
	if (m_nCurrCrew <= 0.0f)
		return no_boost;

	// boost!
	float boost = (static_cast<float>(m_nCurrCrew) / static_cast<float>(m_nMaxCrew)) * 0.5f;

	// boosted!
	boost *= no_boost;

	// return
	return boost;
}



//****************************************//
/*Resource*/
//****************************************//

void PlayerShip::UpdateResources(float dt)
{
	// boarding check
	if (m_BoardingState > eBoardingState::NONE || m_DeathStage > eDeathStages::ALIVE)
		return;


	CGame*			game = static_cast<CGame*>(CGame::GetApplication());
	GameplayScene*	gps = static_cast<GameplayScene*>(game->GetSceneManager()->GetScene(IScene::SceneType::GAMEPLAY));
	SoundManager*	sound = game->GetSoundManager();


	// update res over time
#pragma region Resource Buffer Update

	if (m_fBufferRep > 0.0f)
	{
		float buffminus = dt * INCREASE_SPEED * INCREASE_SPEED;
		float prevRep = m_fCurrReputation;

		m_fBufferRep -= buffminus;
		if (m_fCurrReputation < m_fMaxReputation)
			m_fCurrReputation += buffminus;

		// spawn recon squad: prev < (MAXREP/2) < curr
		float reconmark = m_fMaxReputation * 0.5f;
		if (prevRep < reconmark && m_fCurrReputation > reconmark)
			fsEventManager::GetInstance()->FireEvent(fsEvents::SpawnReconSquad);

		if (m_fBufferRep < 0.0f)
			m_fBufferRep = 0.0f;
	}

	if (m_nBufferRum > 0)
	{
		int buffminus = int(std::ceilf(dt * INCREASE_SPEED));

		m_nBufferRum -= buffminus;
		if (m_nCurrRum < m_nMaxRum)
			m_nCurrRum += buffminus;
		if (m_nBufferRum <= 0)
		{
			m_nBufferRum = 0;
			m_fRumResourceTimer = MAX_ICON_TIMER;
		}
	}

	if (m_nBufferWaterFood > 0)
	{
		int buffminus = int(std::ceilf(dt * INCREASE_SPEED));

		m_nBufferWaterFood -= buffminus;
		if (m_nCurrWaterFood < m_nMaxWaterFood)
			m_nCurrWaterFood += buffminus;
		if (m_nBufferWaterFood <= 0)
		{
			m_nBufferWaterFood = 0;
			m_fWaterFoodResourceTimer = MAX_ICON_TIMER;
		}
	}

	if (m_nBufferGold > 0)
	{
		int buffminus = int(std::ceilf(dt * INCREASE_SPEED * INCREASE_SPEED));

		m_nBufferGold -= buffminus;
		if (m_nCurrGold < m_nMaxGold)
			m_nCurrGold += buffminus;
		if (m_nBufferGold <= 0)
		{
			m_nBufferGold = 0;
			m_fGoldResourceTimer = MAX_ICON_TIMER;
		}
	}
#pragma endregion


	// HUD stuff
#pragma region HUD icons & timer stuff

	bool atPort = GetPortCollision();
	if (atPort)
	{
		m_fGoldResourceTimer = MAX_ICON_TIMER;
		reinterpret_cast<G2D::GUITexture*>(gps->GetHUDElements()["HUD_GoldIcon"])->Active(true);
		reinterpret_cast<G2D::GUIText*>(gps->GetHUDElements()["GoldStatus"])->Active(true);
	}
	if (0.0f < m_fGoldResourceTimer)
	{
		if (!atPort)
			m_fGoldResourceTimer -= dt;
		auto pGoldIcon = reinterpret_cast<G2D::GUITexture*>(gps->GetHUDElements()["HUD_GoldIcon"]);
		auto pGoldIconText = reinterpret_cast<G2D::GUIText*>(gps->GetHUDElements()["GoldStatus"]);

		pGoldIcon->Color(SimpleMath::Color(1.0f, 1.0f, 1.0f, m_fGoldResourceTimer / MAX_ICON_TIMER));
		pGoldIconText->Color(SimpleMath::Color(1.0f, 1.0f, 1.0f, m_fGoldResourceTimer / MAX_ICON_TIMER));

		if (0.0f >= m_fGoldResourceTimer)
		{
			pGoldIcon->Active(false);
			pGoldIcon->Color(SimpleMath::Color(1.0f, 1.0f, 1.0f, 1.0f));
			pGoldIconText->Active(false);
			pGoldIconText->Color(SimpleMath::Color(1.0f, 1.0f, 1.0f, 1.0f));
		}
	}

	if (atPort || m_nCurrWaterFood <= m_nCurrCrew * 2)
	{
		m_fWaterFoodResourceTimer = MAX_ICON_TIMER;
		reinterpret_cast<G2D::GUITexture*>(gps->GetHUDElements()["HUD_FoodIcon"])->Active(true);
		reinterpret_cast<G2D::GUIText*>(gps->GetHUDElements()["FoodStatus"])->Active(true);
	}
	if (0.0f < m_fWaterFoodResourceTimer)
	{
		if (!atPort)
			m_fWaterFoodResourceTimer -= dt;
		auto pWaterFoodIcon = reinterpret_cast<G2D::GUITexture*>(gps->GetHUDElements()["HUD_FoodIcon"]);
		auto pWaterFoodIconText = reinterpret_cast<G2D::GUIText*>(gps->GetHUDElements()["FoodStatus"]);

		static float crittime;
		float critical = 1.0f;
		if (m_nCurrWaterFood <= m_nCurrCrew * 2)
		{
			crittime += dt;
			critical = (float)((int)floor(crittime) % 2) * 0.5f + 0.5f;
		}
		pWaterFoodIcon->Color(SimpleMath::Color(1.0f, 1.0f, 1.0f, m_fWaterFoodResourceTimer / MAX_ICON_TIMER * critical));
		pWaterFoodIconText->Color(SimpleMath::Color(1.0f, 1.0f, 1.0f, m_fWaterFoodResourceTimer / MAX_ICON_TIMER));

		if (0.0f >= m_fWaterFoodResourceTimer)
		{
			pWaterFoodIcon->Active(false);
			pWaterFoodIcon->Color(SimpleMath::Color(1.0f, 1.0f, 1.0f, 1.0f));
			pWaterFoodIconText->Active(false);
			pWaterFoodIconText->Color(SimpleMath::Color(1.0f, 1.0f, 1.0f, 1.0f));
		}
	}

	if (atPort || m_nCurrRum <= m_nCurrCrew * 2)
	{
		m_fRumResourceTimer = MAX_ICON_TIMER;
		reinterpret_cast<G2D::GUITexture*>(gps->GetHUDElements()["HUD_RumIcon"])->Active(true);
		reinterpret_cast<G2D::GUIText*>(gps->GetHUDElements()["RumStatus"])->Active(true);
	}
	if (0.0f < m_fRumResourceTimer)
	{
		if (!atPort)
			m_fRumResourceTimer -= dt;
		auto pRumIcon = reinterpret_cast<G2D::GUITexture*>(gps->GetHUDElements()["HUD_RumIcon"]);
		auto pRumIconText = reinterpret_cast<G2D::GUIText*>(gps->GetHUDElements()["RumStatus"]);

		static float crittime;
		float critical = 1.0f;
		if (m_nCurrRum <= m_nCurrCrew * 2)
		{
			crittime += dt;
			critical = (float)((int)floor(crittime) % 2) * 0.5f + 0.5f;
		}
		pRumIcon->Color(SimpleMath::Color(1.0f, 1.0f, 1.0f, m_fRumResourceTimer / MAX_ICON_TIMER * critical));
		pRumIconText->Color(SimpleMath::Color(1.0f, 1.0f, 1.0f, m_fRumResourceTimer / MAX_ICON_TIMER));

		if (0.0f >= m_fRumResourceTimer)
		{
			pRumIcon->Active(false);
			pRumIcon->Color(SimpleMath::Color(1.0f, 1.0f, 1.0f, 1.0f));
			pRumIconText->Active(false);
			pRumIconText->Color(SimpleMath::Color(1.0f, 1.0f, 1.0f, 1.0f));
		}
	}


	if (atPort || m_nCurrCrew <= 2)
	{
		m_fCrewResourceTimer = MAX_ICON_TIMER;
		reinterpret_cast<G2D::GUITexture*>(gps->GetHUDElements()["HUD_CrewIcon"])->Active(true);
		reinterpret_cast<G2D::GUIText*>(gps->GetHUDElements()["CrewStatus"])->Active(true);
	}
	if (0.0f < m_fCrewResourceTimer)
	{
		if (!atPort)
			m_fCrewResourceTimer -= dt;
		auto pCrewIcon = reinterpret_cast<G2D::GUITexture*>(gps->GetHUDElements()["HUD_CrewIcon"]);
		auto pCrewIconText = reinterpret_cast<G2D::GUIText*>(gps->GetHUDElements()["CrewStatus"]);

		static float crittime;
		float critical = 1.0f;
		if (m_nCurrCrew <= 1)
		{
			crittime += dt;
			critical = (float)((int)floor(crittime) % 2) * 0.5f + 0.5f;
		}
		pCrewIcon->Color(SimpleMath::Color(1.0f, 1.0f, 1.0f, m_fCrewResourceTimer / MAX_ICON_TIMER * critical));
		pCrewIconText->Color(SimpleMath::Color(0.0f, 0.0f, 0.0f, m_fCrewResourceTimer / MAX_ICON_TIMER));

		if (0.0f >= m_fCrewResourceTimer)
		{
			pCrewIcon->Active(false);
			pCrewIcon->Color(SimpleMath::Color(1.0f, 1.0f, 1.0f, 1.0f));
			pCrewIconText->Active(false);
			pCrewIconText->Color(SimpleMath::Color(0.0f, 0.0f, 0.0f, 1.0f));
		}
	}
#pragma endregion



	// update resource timer
	m_fResourceTimer -= dt;
	if (m_fResourceTimer > 0.0f)
	{
		m_bDecreasingRes = false;
		return;
	}


	// reset HUD timers
	m_fWaterFoodResourceTimer = MAX_ICON_TIMER;
	m_fRumResourceTimer = MAX_ICON_TIMER;


	// turn on HUD icons
	auto pRumIcon = reinterpret_cast<G2D::GUITexture*>(gps->GetHUDElements()["HUD_RumIcon"]);
	auto pRumIconText = reinterpret_cast<G2D::GUIText*>(gps->GetHUDElements()["RumStatus"]);
	pRumIcon->Active(true);
	pRumIconText->Active(true);

	auto pWaterFoodIcon = reinterpret_cast<G2D::GUITexture*>(gps->GetHUDElements()["HUD_FoodIcon"]);
	auto pWaterFoodIconText = reinterpret_cast<G2D::GUIText*>(gps->GetHUDElements()["FoodStatus"]);
	pWaterFoodIcon->Active(true);
	pWaterFoodIconText->Active(true);


	//playing a sound
	sound->Play3DSound(AK::EVENTS::PLAY_RESOURCETAKEAWAY);


	// restart timer
	m_fResourceTimer = RESOURCE_TIME;
	m_bDecreasingRes = true;


	int prevrum = m_nCurrRum;
	int prevfood = m_nCurrWaterFood;


	// rum--
	m_nCurrRum -= m_nCurrCrew;
	if (m_nCurrRum < 0)
		m_nCurrRum = 0;

	// starvation/cannibalism
	if (m_nCurrWaterFood <= 0)
	{
		DecreaseCrew();
		sound->Play3DSound(AK::EVENTS::PLAYCREWDEATH);
		auto pCrewIcon = reinterpret_cast<G2D::GUITexture*>(gps->GetHUDElements()["HUD_CrewIcon"]);
		auto pCrewIconText = reinterpret_cast<G2D::GUIText*>(gps->GetHUDElements()["CrewStatus"]);
		pCrewIcon->Active(true);
		pCrewIconText->Active(true);
		m_fCrewResourceTimer = MAX_ICON_TIMER;
	}

	// water/food--
	m_nCurrWaterFood -= m_nCurrCrew;
	if (m_nCurrWaterFood < 0)
		m_nCurrWaterFood = 0;


	// on-screen decrement
	int dec1 = m_nCurrRum - prevrum;
	int dec2 = m_nCurrWaterFood - prevfood;

	if (dec1 < 0 || dec2 < 0)
	{
		gps->ActivateResourceDecrement(dec1, dec2);
	}
}


void PlayerShip::RefillResources(void)
{
	//m_nCurrRum = m_nMaxRum;
	//m_nCurrWaterFood = m_nMaxWaterFood;
	//m_nCurrGold = m_nMaxGold;
	m_nCurrCrew = m_nMaxCrew;
	m_nCurrGold = m_nMaxGold;	//IncreaseGold(m_nMaxGold);
	IncreaseRum(m_nMaxRum);
	IncreaseFood(m_nMaxWaterFood);

	m_fResourceTimer = RESOURCE_TIME;
}



//****************************************//
/*Cheats*/
//****************************************//

void PlayerShip::GoToMaxLevel(void)
{
	// level up player
	for (size_t i = 0; i < 5; i++)
	{
		LevelUp(IShipPart::PartType::HULL);
		LevelUp(IShipPart::PartType::MAST);
		LevelUp(IShipPart::PartType::WEAPON);
	}


	CGame*			game = static_cast<CGame*>(CGame::GetApplication());
	SceneManager*	sm = game->GetSceneManager();

	// change mesh/texture/etc...
	sm->GetEntityManager().RemoveEntity(IScene::SceneType::GAMEPLAY, EntityManager::EntityBucket::RENDERABLE, this);
	ChangeRenderMesh("PlayerShip3.mesh", 0, 1);
	ChangeRenderTexture("PlayerShip3Diffuse.dds");
	static_cast<BoxCollider*>(GetCollider())->setDimensions(XMFLOAT3(0.85f, 1.1f, 3.1f));
	static_cast<BoxCollider*>(GetCollider())->setOffset(XMFLOAT3(0, 0.28f, -0.584f));
	sm->GetEntityManager().AddEntity(IScene::SceneType::GAMEPLAY, EntityManager::EntityBucket::RENDERABLE, this);

	// redo renderables
	game->m_pRenderer->ClearRenderables();
	std::vector<Renderer::CRenderable*> renderList = sm->GetEntityManager().GetRenderables(IScene::SceneType::GAMEPLAY, EntityManager::EntityBucket::RENDERABLE);
	game->m_pRenderer->SetUpRenderables(renderList);
}


void PlayerShip::ToggleGodMode(void)
{
	m_bGodMode = !m_bGodMode;


	//size_t size = m_pParticleSet.size();
	//for (size_t i = 0; i < size; i++)
	//{
	//	m_pParticleSet[i]->Reset();
	//}


	if (m_bGodMode == false)
	{
		//SetParticleSetToPause();
		return;
	}

	//SetParticleSetToPlay();
	Repair(IShipPart::PartType::HULL);
	Repair(IShipPart::PartType::MAST);
	Repair(IShipPart::PartType::WEAPON);
}



//****************************************//
/*Boarding*/
//****************************************//

void PlayerShip::UpdateBoarding(float dt)
{
	// boarding check
	if (m_BoardingState <= eBoardingState::NONE)
		return;


	// player boarding logic
	PlayerBoarding(dt);
}


void PlayerShip::PlayerBoarding(float dt)
{
	// Player doing enemy's boarding process check
	if (m_BoardingState == eBoardingState::BOARDING_ENEMY)
	{
		m_BoardingState = eBoardingState::NONE;
		return;
	}



	CGame*			game = static_cast<CGame*>(CGame::GetApplication());
	GameplayScene*	gps = static_cast<GameplayScene*>(game->GetSceneManager()->GetScene(IScene::SceneType::GAMEPLAY));
	SoundManager*	sound = game->GetSoundManager();



	if (m_pBoardingTarget != nullptr && eDeathStages::ALIVE == m_pBoardingTarget->GetDeathStage())
	{
		fsEventManager::GetInstance()->FireEvent(fsEvents::AI_Alert, &EventArgs2<AiController*, Ship*>(static_cast<AiController*>(m_pBoardingTarget->GetController()), this));

		// Phase #1: Player moves towards enemy
		if (m_BoardingState == eBoardingState::BOARDING_MOVE)
		{
			m_fBoardingTimer -= dt;
			if (m_fBoardingTimer > 18.0f)
			{
				if (0.0f == m_boardingTargetPos.w)
				{
					// position
					XMVECTOR enemyPos = { m_pBoardingTarget->GetPosition().x, m_pBoardingTarget->GetPosition().y, m_pBoardingTarget->GetPosition().z };
					XMVECTOR playerPos = { GetPosition().x, GetPosition().y, GetPosition().z };
					XMVECTOR boardingTarget = XMVectorSubtract(playerPos, enemyPos);
					boardingTarget = XMVector3Normalize(boardingTarget);
					boardingTarget = XMVectorScale(boardingTarget, 1.5f);
					XMStoreFloat4(&m_boardingTargetPos, boardingTarget);
					m_boardingTargetPos.x += m_pBoardingTarget->GetPosition().x;
					m_boardingTargetPos.y += m_pBoardingTarget->GetPosition().y;
					m_boardingTargetPos.z += m_pBoardingTarget->GetPosition().z;
					m_boardingTargetPos.w = 1.0f;

				}
				// position & rotation
				XMVECTOR enemyScale;
				XMVECTOR enemyQuat;
				XMVECTOR enemyTrans;
				XMMATRIX enemyMatrix = XMLoadFloat4x4(&m_pBoardingTarget->GetWorldMatrix());
				XMMatrixDecompose(&enemyScale, &enemyQuat, &enemyTrans, enemyMatrix);

				XMVECTOR playerScale;
				XMVECTOR playerQuat;
				XMVECTOR playerTrans;
				XMMATRIX playerMatrix = XMLoadFloat4x4(&GetWorldMatrix());
				XMMatrixDecompose(&playerScale, &playerQuat, &playerTrans, playerMatrix);
				XMVECTOR orgin = { 0,0,0,0 };

				// interpolate
				enemyTrans = XMLoadFloat4(&m_boardingTargetPos);
				playerTrans = XMVectorLerp(playerTrans, enemyTrans, (STARTING_BOARDING_TIME - m_fBoardingTimer) / STARTING_BOARDING_TIME);
				//XMVECTOR finalQuat = XMQuaternionSlerp(playerQuat, enemyQuat, (STARTING_BOARDING_TIME - m_fBoardingTimer) / STARTING_BOARDING_TIME);


				// matrix math
				XMMATRIX finalMatrix = XMMatrixAffineTransformation(playerScale, orgin, playerQuat, playerTrans);

				// set new matrix
				XMFLOAT4X4 finalPlayerPos;
				XMStoreFloat4x4(&finalPlayerPos, finalMatrix);
				//negating all rotation
				finalPlayerPos._12 = 0;
				finalPlayerPos._21 = 0;			finalPlayerPos._22 = 1;			finalPlayerPos._23 = 0;
				finalPlayerPos._32 = 0;

				GetTransform().SetLocalMatrix(finalPlayerPos);

				return;
			}
			m_fBoardingTimer = BOARDING_TIME;
			m_pBoardingIter = -1;
			m_BoardingState = eBoardingState::BOARDING_SHIP;
			return;
		}

		if (0.0f == m_swordsCrossingPos.w)
		{
			XMVECTOR playerPos = { GetPosition().x, GetPosition().y, GetPosition().z };
			XMVECTOR enemyPos = { m_pBoardingTarget->GetPosition().x, m_pBoardingTarget->GetPosition().y, m_pBoardingTarget->GetPosition().z };
			XMVECTOR iconPos = XMVectorLerp(playerPos, enemyPos, 0.5f);
			XMStoreFloat4(&m_swordsCrossingPos, iconPos);

			m_vecParticleSet[2]->EmitPositionW({ m_swordsCrossingPos.x, m_swordsCrossingPos.y + 1.0f, m_swordsCrossingPos.z });
			m_vecParticleSet[2]->Reset();



			m_crewStickerBluePos = m_swordsCrossingPos;
			m_crewStickerBluePos.y += 1.0f;
			m_crewStickerRedPos = m_swordsCrossingPos;
			m_crewStickerRedPos.y += 1.0f;

			m_swordsCrossingPos.y += 2.0f;
			m_swordsCrossingPos.z -= 0.5f;
			m_swordsCrossingPos.w = 1.0f;
			m_swordsCrossed->SetProgression(1.0f);
		}

		// Phase #2: Player boarding enemy ship
		if (m_BoardingState == eBoardingState::BOARDING_SHIP)
		{
			// boarding speed bonus
			//float boardspeed = BoardingBonus(dt);
			/*
			float boardspeed = dt;
			if (BoardingBonus() == true)
				boardspeed *= 2.0f;
			*/

			m_vecParticleSet[2]->Play();

			// update timers
			m_swordsCrossingTimer -= dt;
			m_fBoardingTimer -= dt;

			if (m_swordsCrossingTimer <= 0.0f)
			{
				if (m_swordsCrossedActive)
				{
					XMFLOAT3 camRight = VectorZ;
					XMFLOAT3 camUp;
					crossProduct(gps->GetMainCamera()->LookAtDirection(), camRight, camUp);

					m_NumCrew = std::to_wstring(GetCurrCrew());

					m_swordsCrossed->SetProgression(0.0f);
					m_swordsUncrossed->SetProgression(1.0f);
					float ours = (float)GetCurrCrew();
					float theirs = (float)m_pBoardingTarget->GetCurrCrew();
					float total = ours + theirs;
					float ratio = ours / total;
					m_boardBackBar->SetProgression(1.0f);
					//m_pirateAboard->SetProgression(1.0f);
					//m_navyAboard->SetProgression(1.0f);
					m_crewIcon->SetProgression(1.0f);
					m_boardBar->SetProgression(ratio);
					XMFLOAT3 m_boardPos = GetPosition() + (m_pBoardingTarget->GetPosition() - GetPosition());
					m_boardBarPos = SimpleMath::Vector4(
						m_boardPos.x - camRight.x + camUp.x * 0.125f,
						m_boardPos.y - camRight.y + camUp.y * 0.125f,
						m_boardPos.z - camRight.z + camUp.z * 0.125f, 1.0f);
					m_pirateAboardPos = m_boardBarPos;
					m_pirateAboardPos.x += camRight.x * -0.6f + camUp.x * 0.4f;
					m_pirateAboardPos.y += camRight.y * -0.6f + camUp.y * 0.4f;
					m_pirateAboardPos.z += camRight.z * -0.6f + camUp.z * 0.4f;
					m_navyAboardPos = m_boardBarPos;
					m_navyAboardPos.x += camRight.x * 1.6f + camUp.x * 0.4f;
					m_navyAboardPos.y += camRight.y * 1.6f + camUp.y * 0.4f;
					m_navyAboardPos.z += camRight.z * 1.6f + camUp.z * 0.4f;
					m_pBoardingTarget->BeBoarded(m_navyAboardPos);

					m_NumCrew3D->SetTextPosition({
						m_pirateAboardPos.x - camUp.x * .6f + camRight.x * .4f,
						m_pirateAboardPos.y - camUp.y * .6f + camRight.y * .4f,
						m_pirateAboardPos.z - camUp.z * .6f + camRight.z * .4f });
					m_NumCrew3D->SetFillColor({ 0.0f, 0.0f, 0.0f, 1.0f });
					float boostedratio = BoardingBonus(ratio);
					if (rand() % 100 > boostedratio * 100)
					{
						DecreaseCrew();
						GameStatistics::GetRef().BodyCollected(1);
						sound->Play3DSound(AK::EVENTS::PLAYCREWDEATH);

						auto pCrewIcon = reinterpret_cast<G2D::GUITexture*>(gps->GetHUDElements()["HUD_CrewIcon"]);
						auto pCrewIconText = reinterpret_cast<G2D::GUIText*>(gps->GetHUDElements()["CrewStatus"]);
						pCrewIcon->Active(true);
						pCrewIconText->Active(true);
						m_fCrewResourceTimer = MAX_ICON_TIMER;
					}
					else
					{
						GameStatistics::GetRef().BodyCollected(1);
						m_pBoardingTarget->DecreaseCrew();
					}
				}
				else
				{
					m_swordsCrossed->SetProgression(1.0f);
					m_swordsUncrossed->SetProgression(0.0f);
				}

				m_swordsCrossedActive = !m_swordsCrossedActive;
				m_swordsCrossingTimer = MAX_BOARDING_ICON_TIMER;
			}


			// finished boarding?
			if (m_pBoardingTarget->GetCurrCrew() <= 0)
			{
				// stop boarding sounds
				sound->Play3DSound(AK::EVENTS::STOPBOARDINGSOUNDS);

				// restart timer
				m_fBoardingTimer = BOARDING_TIME;

				// stop sailsdown stuff
				m_pBoardingTarget->SetSailsDown(false);
				m_pBoardingTarget->GetSailsDownIcon()->SetProgression(0.0f);
				m_boardBar->SetProgression(0.0f);
				m_boardBackBar->SetProgression(0.0f);
				//m_pirateAboard->SetProgression(0.0f);
				//m_navyAboard->SetProgression(0.0f);
				m_crewIcon->SetProgression(0.0f);
				m_NumCrew = L"";
				m_NumCrew3D->SetProgression(0.0f);
				m_NumCrew3D->SetFillColor({ 0, 0, 0, 0 });

				// All my life I hustled just to get that mula

				GameStatistics::GetRef().FoodCollected(m_pBoardingTarget->GetCurrWaterFood());
				GameStatistics::GetRef().RumCollected(m_pBoardingTarget->GetCurrRum());
				GameStatistics::GetRef().GoldCollected(m_pBoardingTarget->GetCurrGold());
				gps->AddHudResources(m_pBoardingTarget->GetCurrGold(), m_pBoardingTarget->GetCurrRum(), m_pBoardingTarget->GetCurrWaterFood());

				// get rid of target
				m_pBoardingTarget->SetKiller(this);
				m_pBoardingTarget->KillOwner();
				m_pBoardingTarget = nullptr;

				// done boarding
				m_BoardingState = eBoardingState::BOARDING_FINISHED;
				sound->Play3DSound(AK::EVENTS::PLAY_PLAYERWINBOARDING);
				return;
			}
			else if (GetCurrCrew() <= 1 || m_pBoardingTarget->ToBeDeleted())
			{
				m_pBoardingTarget->FinishBoarding();
				m_BoardingState = eBoardingState::BOARDING_FINISHED;
				m_pBoardingTarget->SetBoardingStage(eBoardingState::NONE);
				sound->Play3DSound(AK::EVENTS::STOPBOARDINGSOUNDS);
				m_pBoardingTarget->SetSailsDown(false);
				m_pBoardingTarget->GetSailsDownIcon()->SetProgression(0.0f);
				m_boardBar->SetProgression(0.0f);
				m_boardBackBar->SetProgression(0.0f);
				//m_pirateAboard->SetProgression(0.0f);
				//m_navyAboard->SetProgression(0.0f);
				m_crewIcon->SetProgression(0.0f);
				m_NumCrew = L"";
				m_NumCrew3D->SetProgression(0.0f);
				m_NumCrew3D->SetFillColor({ 0, 0, 0, 0 });
				sound->Play3DSound(AK::EVENTS::PLAY_LOSEBOARDING);
			}
		}



		// Phase #3: Player finished boarding enemy ship
		if (m_BoardingState == eBoardingState::BOARDING_FINISHED)
		{
			m_BoardingState = eBoardingState::NONE;
			m_fBoardingTimer = STARTING_BOARDING_TIME;
			m_pBoardingTarget = nullptr;
			m_pBoardingIter = -1;
			m_bCurrBoarding = false;
			SecureZeroMemory(&m_boardingTargetPos, sizeof(m_boardingTargetPos));

			m_comingBackFromRuinedSails = false;
			m_sailsDown = false;

			// Boarding fight icons
			m_swordsCrossingTimer = MAX_BOARDING_ICON_TIMER;
			m_swordsCrossedActive = true;
			m_swordsCrossingPos = SimpleMath::Vector4::Zero;
			m_swordsCrossed->SetProgression(0.0f);
			m_swordsUncrossed->SetProgression(0.0f);
		}
	}
	else
	{
		m_BoardingState = eBoardingState::NONE;
		m_fBoardingTimer = STARTING_BOARDING_TIME;
		m_pBoardingIter = -1;
		m_pBoardingTarget = nullptr;
		m_bCurrBoarding = false;
		SecureZeroMemory(&m_boardingTargetPos, sizeof(m_boardingTargetPos));

		m_comingBackFromRuinedSails = false;
		m_sailsDown = false;

		// Boarding fight icons
		m_swordsCrossingTimer = MAX_BOARDING_ICON_TIMER;
		m_swordsCrossedActive = true;
		m_swordsCrossingPos = SimpleMath::Vector4::Zero;
		m_swordsCrossed->SetProgression(0.0f);
		m_swordsUncrossed->SetProgression(0.0f);
	}
}


bool PlayerShip::FindPossibleBoardingTargets(void)
{
	// clear prev targets
	m_pBoardingAllTargets.clear();
	m_pBoardingIter = -1;


	CGame*				game = static_cast<CGame*>(CGame::GetApplication());
	GameplayScene*		gps = static_cast<GameplayScene*>(game->GetSceneManager()->GetScene(IScene::SceneType::GAMEPLAY));
	std::vector<Ship*>	aivec;
	gps->GetActiveAIShips(aivec);


	for (size_t i = 0; i < aivec.size(); i++)
	{
		// boardable?
		if (aivec[i]->GetBoardingStage() == eBoardingState::UNBOARDABLE)
			continue;

		// alive check
		if (aivec[i]->GetDeathStage() != eDeathStages::ALIVE)
			continue;

		// speed check
		if (aivec[i]->GetCurrentMaxSpeed() > BOARDING_SPEED)
			continue;

		// push back
		m_pBoardingAllTargets.push_back(static_cast<AIShip*>(aivec[i]));
	}
	return (m_pBoardingAllTargets.size() > 0);
}


bool PlayerShip::SetCurrentBoardingTarget(void)
{
	// vecsize
	size_t vecsize = m_pBoardingAllTargets.size();


	// empty?
	if (vecsize == 0)
		return false;


	// only 1 ship
	if (vecsize == 1)
	{
		m_pBoardingIter = 0;
		m_pBoardingTarget = m_pBoardingAllTargets[m_pBoardingIter];
		return true;
	}


	// find closest ship
	float closest = GetSqrDistance(m_pBoardingAllTargets[0]);
	for (size_t i = 1; i < vecsize; i++)
	{
		// get distance
		float dist = GetSqrDistance(m_pBoardingAllTargets[i]);

		// closer check
		if (closest < dist)
			continue;

		// closer target
		m_pBoardingIter = int(i);
		m_pBoardingTarget = m_pBoardingAllTargets[m_pBoardingIter];
	}
	return true;
}


bool PlayerShip::CheckBoardingDistance(void)
{
	if (m_pBoardingTarget == nullptr)
		return false;
	if (m_BoardingState > eBoardingState::NONE)
		return false;

	// distance check
	float dist = GetSqrDistance(m_pBoardingTarget);
	if (dist > BOARDING_DIST * BOARDING_DIST)
		return false;

	// start boarding process
	m_BoardingState = eBoardingState::BOARDING_MOVE;
	m_pBoardingTarget->SetBoardingStage(eBoardingState::BOARDING_ENEMY);

	SoundManager* sound = static_cast<CGame*>(CGame::GetApplication())->GetSoundManager();
	sound->Play3DSound(AK::EVENTS::PLAYBOARDINGSOUNDS);
	GameStatistics::GetRef().TotalBoarding(1);
	return true;
}



//****************************************//
/*Other*/
//****************************************//

void PlayerShip::KillOwner(void)
{
	if (m_DeathStage > eDeathStages::ALIVE)
		return;

	// base kill owner
	Ship::KillOwner();

	// remove 3D HUD
	CGame::GetApplication()->m_pRenderer->Remove3DHUD(m_swordsCrossed.get());
	CGame::GetApplication()->m_pRenderer->Remove3DHUD(m_swordsUncrossed.get());
	CGame::GetApplication()->m_pRenderer->Remove3DHUD(m_crewStickerRed.get());
	CGame::GetApplication()->m_pRenderer->Remove3DHUD(m_crewStickerBlue.get());
}


void PlayerShip::AddReputation(float rep)
{
	m_fBufferRep += rep;
	m_fBufferRep = (m_fBufferRep < m_fMaxReputation) ? m_fBufferRep : m_fMaxReputation;
	//m_fCurrReputation += rep;
	//if (m_fCurrReputation >= m_fMaxReputation)
	//	m_fCurrReputation = m_fMaxReputation;
}


void PlayerShip::LockInput(void)
{
	static_cast<PlayerController*>(m_pController)->Lock();
}


void PlayerShip::UnlockInput(void)
{
	static_cast<PlayerController*>(m_pController)->Unlock();
}


bool PlayerShip::WinCheck(void)
{
	// win check
	if (m_fCurrReputation < m_fMaxReputation)
		return false;

	// win timer
	return true;
	/*
	m_fWinTimer -= m_fWindt;
	if (m_fWinTimer < 0.0f)
	{
		SoundManager* sound = ((CGame*)CGame::GetApplication())->GetSoundManager();
		if (m_BoardingState > Ship::eBoardingState::NONE)
			sound->Play3DSound(AK::EVENTS::STOPBOARDINGSOUNDS);
		return true;
	}

	return false;
	*/
}


bool PlayerShip::WinCountdown(void)
{
	// win timer
	m_fWinTimer -= m_fWindt;
	if (m_fWinTimer < 0.0f)
	{
		SoundManager* sound = static_cast<CGame*>(CGame::GetApplication())->GetSoundManager();
		if (m_BoardingState > Ship::eBoardingState::NONE)
			sound->Play3DSound(AK::EVENTS::STOPBOARDINGSOUNDS);
		return true;
	}

	return false;
}


bool PlayerShip::LoseCheck(void)
{
	// win check
	//if (m_cHull->GetHealth() <= 0 || to_delete == true)
	if (m_DeathStage == eDeathStages::DEATH_DONE || to_delete == true)
	{
		SoundManager* sound = static_cast<CGame*>(CGame::GetApplication())->GetSoundManager();
		if (m_BoardingState > Ship::eBoardingState::NONE)
			sound->Play3DSound(AK::EVENTS::STOPBOARDINGSOUNDS);
		return true;
	}
	return false;
}



//**************Mutators****************//

//****************************************//
/*Resource*/
//****************************************//

void PlayerShip::IncreaseGold(int amount)
{
	m_nBufferGold += amount;
	m_nBufferGold = (m_nBufferGold < m_nMaxGold) ? m_nBufferGold : m_nMaxGold;
}


void PlayerShip::DecreaseGold(int amount)
{
	m_nCurrGold -= amount;

	if (m_nCurrGold < 0)
		m_nCurrGold = 0;
}


void PlayerShip::IncreaseRum(int amount)
{
	m_nBufferRum += amount;
	m_nBufferRum = (m_nBufferRum < m_nMaxRum) ? m_nBufferRum : m_nMaxRum;
}


void PlayerShip::IncreaseFood(int amount)
{
	m_nBufferWaterFood += amount;
	m_nBufferWaterFood = (m_nBufferWaterFood < m_nMaxWaterFood) ? m_nBufferWaterFood : m_nMaxWaterFood;
}


void PlayerShip::IncreaseCrew(int amount)
{
	m_nCurrCrew += amount;
	m_nCurrCrew = (m_nCurrCrew < m_nMaxCrew) ? m_nCurrCrew : m_nMaxCrew;
}


void PlayerShip::DecreaseCrew(void)
{
	// crew--
	m_nCurrCrew--;


	// Enter hud update her


	// min crew
	if (m_nCurrCrew <= 0)
		KillOwner();
}


bool PlayerShip::Purchase(int cost)
{
	if (m_nCurrGold < cost)
		return false;

	//DecreaseGold(cost);
	return true;
}


bool PlayerShip::RefillRum(void)
{
	if (m_nCurrRum == m_nMaxRum)
		return false;

	// rum = MAX
	m_nCurrRum = m_nMaxRum;

	// restart timer
	m_fResourceTimer = RESOURCE_TIME;

	return true;
}


bool PlayerShip::RefillRum(int amount)
{
	if (m_nCurrRum == m_nMaxRum)
		return false;

	// rum++
	Ship::IncreaseRum(amount);

	// restart timer
	m_fResourceTimer = RESOURCE_TIME;

	return true;
}


bool PlayerShip::RefillFood(void)
{
	if (m_nCurrWaterFood == m_nMaxWaterFood)
		return false;

	// food = MAX
	m_nCurrWaterFood = m_nMaxWaterFood;

	// restart timer
	m_fResourceTimer = RESOURCE_TIME;

	return true;
}


bool PlayerShip::RefillFood(int amount)
{
	if (m_nCurrWaterFood == m_nMaxWaterFood)
		return false;

	// food++
	Ship::IncreaseFood(amount);

	// restart timer
	m_fResourceTimer = RESOURCE_TIME;

	return true;
}


//***************Data Members****************//

//****************************************//
/* Mast*/
//****************************************//

void PlayerShip::ReduceSpeed()
{
	//Reducing the speed
	m_fCurrentMaxSpeed *= 0.7f;

	if (m_fSpeed > m_fCurrentMaxSpeed)
		m_fSpeed = m_fCurrentMaxSpeed;

	// able to board
	if (m_fCurrentMaxSpeed <= BOARDING_SPEED)
	{
		// boarding effects
		// show sail down icon
		if (false == m_sailsDown)
		{
			m_sailsDown = true;
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

bool PlayerShip::MastLevelUp()
{
	if (Ship::MastLevelUp() == false)
		return false;


	CGame*			game = static_cast<CGame*>(CGame::GetApplication());
	SceneManager*	sm = game->GetSceneManager();
	GameplayScene*	gps = static_cast<GameplayScene*>(sm->GetScene(IScene::SceneType::GAMEPLAY));

	if (m_nMastLevel <= 2)
	{
		gps->m_spyElems["Player_Slow_Sails"]->Active(true);
		gps->m_spyElems["Player_Sails"]->Active(false);
		gps->m_spyElems["Player_Fast_Sails"]->Active(false);
	}
	else if (m_nMastLevel > 2 && m_nMastLevel < 4)
	{
		gps->m_spyElems["Player_Slow_Sails"]->Active(false);
		gps->m_spyElems["Player_Sails"]->Active(true);
		gps->m_spyElems["Player_Fast_Sails"]->Active(false);
	}
	else
	{
		gps->m_spyElems["Player_Slow_Sails"]->Active(false);
		gps->m_spyElems["Player_Sails"]->Active(false);
		gps->m_spyElems["Player_Fast_Sails"]->Active(true);
	}



	return true;
}

void PlayerShip::SetupParticles()
{
	//Boarding Particles
	//m_vecParticleSet.push_back(newx Renderer::CParticle(0, 250, Renderer::CParticleCollection::ParticleType::eTYPE_B_EFFECTS));
	//m_nBoardingIndex = (unsigned int)(m_vecParticleSet.size() - 1);
	//m_vecParticleSet[m_nBoardingIndex]->BlendMode(Renderer::CParticle::eBM_AlphaBlend);
	//m_vecParticleSet[m_nBoardingIndex]->ParticleLife(1.0f);
	//m_vecParticleSet[m_nBoardingIndex]->EmitSizeW({ 1.0f, 1.0f });
	//m_vecParticleSet[m_nBoardingIndex]->EmitDirectionW({ 0.0f, 0.0f, 0.0f });
	//m_vecParticleSet[m_nBoardingIndex]->EmitColor({ 0.5f, 0.5f, 0.5f, 1.f });
	//m_vecParticleSet[m_nBoardingIndex]->MaxTexCount(2);
}
