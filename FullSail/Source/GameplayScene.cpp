#include "pch.h"
#include "Log.h"
#include "GameplayScene.h"
#include "SceneManager.h"
#include "WinLoseScene.h"
#include "EntityManager.h"
#include "Camera.h"
#include "Game.h"
#include "MainWindow.h"
#include "GameStatistics.h"
#include "Ship.h"
#include "PlayerShip.h"
#include "AIShip.h"
#include "BossShip.h"
#include "SideCannonHandler.h"
#include "IAmmo.h"
#include "Port.h"
#include "SoundManager.h"
#include "Gull.h"
#include "PlayerController.h"
#include "Math.h"
#include "Water.h"
#include "Crate.h"

#include "Squad.h"
#include "Colliders.h"
#include "AiController.h"
#include "EntityLoader.h"
#include "Entity3D.h"
#include "IBoid.h"
#include "CSteeringManger.h"
#include "CargoSquad.h"
#include "TradeRoute.h"

float2 gfBoundary = { 60.0f,30.0f };
float2 gfBoundaryCenter = { 137.5f, -175.0f };

const std::wstring g_ScreenShotPathW = L"../../Screenshots/";

#define BOUNDARY		300.0f
#define DEATHTIMER		5.0f
#define WAYPOINTVIEWER	0
#define ASHTESTING		0
#define	MAX_BOUNTY		1000000
#define GOLDSHIP		"GoldShip"
#define FOODSHIP		"MerchantShip"
#define MILITARYSHIP	"MilitaryShip"
#define HP_LOW_TIMER_MAX 1.0f
#define CARGOSPAWNTIME  300.0f

GameplayScene::GameplayScene()
	: IScene(),
	outofBoundTimer(DEATHTIMER),
	m_fBossTextTimer(0.0f),
	m_cargoSquad(nullptr),
	m_cargoSquadSpawnTimer(CARGOSPAWNTIME),
	m_cargoSquadTextTimer(0.0f)
	/*, bControl(gulls, aiShips)*/
{
	// scene type
	sceneType = SceneType::GAMEPLAY;

	initialized = false;
	gotopause = false;
	gotoport = false;
	hud_help = false;
	help_timer = 5.0f;

	physicsManager = PhysicsManager();
	cull_timer = CULL_LIMIT;


	water = nullptr;
	playerShip = nullptr;
	m_pReinforcement = nullptr;


	ai_spawn_timer = 0.0f;


	decTimer = 0.0f;
	port_help = false;
	low_timer = 5.0f;
	PromptTimer = 0.0f;

	m_fHpLowTimer = 0.0f;
	m_bHpLowOnBlack = false;

	m_fHudSpeedTimer = -1.0f;

	m_fReconTextTimer = 0.0f;
	m_fPortSaleTimer = 0.0f;

	BossTeam = ShipTeamStruct();

	reconTeam = ShipTeamStruct();
	bossReinforcementTeam = ShipTeamStruct();

	//ReconSquad = nullptr;
	//ReconFleet.clear();

	m_bUpdateResourcesHud = false;


	m_bShowBossMinimap = false;
	m_bShowCargoMinimap = false;

	fsEventManager* fsem = fsEventManager::GetInstance();
	fsem->RegisterClient(fsEvents::SpawnReconSquad, this, &GameplayScene::SpawnReconSquadEvent);
	fsem->RegisterClient(fsEvents::SpawnBossReinforcementSquad, this, &GameplayScene::SpawnBossReinforcementSquadEvent);
	fsem->RegisterClient(fsEvents::BossTracking, this, &GameplayScene::BossTrackingEvent);
	//fsem->RegisterClient(fsEvents::BossNextWaypoint, this, &GameplayScene::BossNextWaypointEvent);
	fsem->RegisterClient(fsEvents::BossHealing, this, &GameplayScene::BossHealingEvent);
}

GameplayScene::~GameplayScene()
{
	//GameplayScene::Exit();

	fsEventManager* fsem = fsEventManager::GetInstance();
	fsem->UnRegisterClient(fsEvents::SpawnReconSquad, this, &GameplayScene::SpawnReconSquadEvent);
	fsem->UnRegisterClient(fsEvents::SpawnBossReinforcementSquad, this, &GameplayScene::SpawnBossReinforcementSquadEvent);
	fsem->UnRegisterClient(fsEvents::BossTracking, this, &GameplayScene::BossTrackingEvent);
	//fsem->UnRegisterClient(fsEvents::BossNextWaypoint, this, &GameplayScene::BossNextWaypointEvent);
	fsem->UnRegisterClient(fsEvents::BossHealing, this, &GameplayScene::BossHealingEvent);


	ai_waypoints.clear();

	initialized = false;

}

#pragma region //*****************************Iscene Functions*****************************//

void GameplayScene::Enter(void)
{
	// base enter
	IScene::Enter();

	/*for (auto i = 0; i < islands.size(); i++) {
		islands[i]->PlayParticleSet();
	}*/

	// get renderables
	std::vector<Renderer::CRenderable*> renderList = sceneManager->GetEntityManager().GetRenderables(GetType(), EntityManager::EntityBucket::RENDERABLE);

	// add renderables
	CGame::GetApplication()->m_pRenderer->SetUpRenderables(renderList);

	m_FadeIn = true;
	m_Timer = FADEINTIMER;
	m_FadeAmount = 0.0f;
	CGame::GetApplication()->m_pRenderer->SetPostProcessFadeAmount(m_FadeAmount);
	SoundManager* sound = ((CGame*)CGame::GetApplication())->GetSoundManager();
	sound->SetStates(AK::STATES::BGMSTATES::GROUP, AK::STATES::BGMSTATES::STATE::SEA);
	sound->Play3DSound(AK::EVENTS::PLAYBGM);

	playerShip->PlayParticleSet();

	m_cargoSquadSpawnTimer = 120.0f;


	//Renderer::CWaterRenderable* waterRenderable = (Renderer::CWaterRenderable*)(water->GetRenderInfo());
	//waterRenderable->SetDarkWaterHalfWidth(gfBoundary.x);
	//waterRenderable->SetDarkWaterHalfHeight(gfBoundary.y);
	//waterRenderable->SetDarkWaterCenter(gfBoundaryCenter.x, gfBoundaryCenter.y);
}

void GameplayScene::Exit(void)
{
	// base exit
	CGame* game = (CGame*)CGame::GetApplication();
	game->TimeScale = 1.0f;

	/*for (auto i = 0; i < islands.size(); i++) {
		islands[i]->StopParticleSet();
	}*/
	playerShip->StopParticleSet();

	SoundManager* sound = game->GetSoundManager();
	//sound->Play3DSound(AK::EVENTS::STOPBGM);
	sound->StopSounds();
	//ShutdownScene();
	IScene::Exit();


}

bool GameplayScene::Input(void)
{
	//if (Keyboard::Get().GetState().F1)
	//{
	//	//FSParticle::CParticleManager::GetInstance()->PauseAll();
	//	PauseSounds();
	//	sceneManager->PushScene(SceneType::INSTRUCTIONS);
	//	spyglass = false;
	//	return NEW_SCENE;
	//}

	// window active state change
	if (CheckGoToPauseScene() == true)
		return NEW_SCENE;

	// port menu change
	if (CheckGoToPortScene() == true)
		return NEW_SCENE;

	// Gamepad connection status
	int status = ((CGame*)CGame::GetApplication())->GetGamePadConnectionStatus();
	if (status != 0)
	{
		//FSParticle::CParticleManager::GetInstance()->PauseAll();
		GamePad::Get().SetVibration(0, 0, 0);
		PauseSounds();
		sceneManager->PushScene(SceneType::PAUSE);
		spyglass = false;
		return NEW_SCENE;
	}

	// player lose check
	PlayerShip* player = static_cast<PlayerShip*>(playerShip);
	if (player->LoseCheck() == true)
	{
		//FSParticle::CParticleManager::GetInstance()->PauseAll();
		sceneManager->PopScene();
        static_cast<WinLoseScene*>( sceneManager->GetScene( IScene::WIN_LOSE ) )->IsVictory( false );
		sceneManager->PushScene(SceneType::WIN_LOSE );
		spyglass = false;
		return NEW_SCENE;
	}

	if (player->WinCheck() == true)
	{
		AddBossShipAndSquad();

		spyglass = false;

		if (BossTeam.fleet[0]->GetHealth() <= 0.0f)
		{
			m_bShowBossMinimap = false;
			//for (size_t i = 1; i < BossTeam.fleet.size(); i++)
			//	BossTeam.fleet[i]->KillOwner();

			if (player->WinCountdown() == true)
			{
				//FSParticle::CParticleManager::GetInstance()->PauseAll();
				sceneManager->PopScene();
                static_cast<WinLoseScene*>( sceneManager->GetScene( IScene::WIN_LOSE ) )->IsVictory( true );
                sceneManager->PushScene( SceneType::WIN_LOSE );
				return NEW_SCENE;
			}
		}
	}



	XMFLOAT3			w_pos = XMFLOAT3(0.0f, 0.0f, 0.0f);	//XMFLOAT3(((rand() % 14) - 7.0f), float(rand() % 8), 0.0f);
	DirectX::XMMATRIX	mat = DirectX::XMMATRIX();			//XMMatrixTranslation(w_pos.x, w_pos.y, w_pos.z);
	XMFLOAT4X4			f4x4 = XMFLOAT4X4();


	// Gameplay scene input
	bool	change = (GameSetting::GetRef().GamePadIsConnected()) ? Input_Gamepad() : Input_Keyboard();


	// Cheat Keys!!!
//#if _DEBUG
	Input_Cheats();
	//#endif // _DEBUG



	return change;

}

void GameplayScene::UpdateReinforcements(float _dt)
{
	if (m_pReinforcement == nullptr)
	{
		return;
	}

	std::vector<Boid*>& Hunters = m_pReinforcement->GetSquad();
	unsigned int HunterSize = (UINT)m_pReinforcement->GetSquad().size();

	if (HunterSize == 0)
	{
		m_pReinforcement = nullptr;
	}

	for (unsigned int i = 0; i < HunterSize; i++)
	{
		Ship* hunter = (Ship*)(Hunters[i]->GetOwner());

		if (hunter->GetDeathStage() != Ship::eDeathStages::ALIVE)
		{
			continue;
		}

		if (((AiController*)hunter->GetController())->GetState() == AiController::WAIT)
		{
			hunter->GetRenderInfo()->SetVisible(false);
			hunter->KillOwner();
			FS_PRINT_OUTPUT(" Hunter Killed \n");
		}
	}



	if (HunterSize == 0)
	{
		m_pReinforcement = nullptr;
	}

}

bool GameplayScene::InHuntingParty(Ship * aiShip)
{

	if (m_pReinforcement == nullptr)
	{
		return false;
	}

	//gathering the party and party size
	std::vector<Boid*>& Hunters = m_pReinforcement->GetSquad();
	unsigned int PartySize = (UINT)Hunters.size();

	//looping through the party to find out 
	for (unsigned int i = 0; i < PartySize; i++)
	{
		if (Hunters[i] == nullptr)
			continue;

		//if any of the are pointing at the same ship
		if (Hunters[i]->GetOwner() == aiShip)
		{
			//if so notify the person with a yes
			return true;
		}
	}

	//if not say no
	return false;
}

void GameplayScene::SetUpTradeRoutes()
{

	TradeRoute& GamePlayRoutes = TradeRoute::TradeRoutes();
	GamePlayRoutes.m_vCheckViewObjects.push_back(playerShip);
	std::vector<XMFLOAT3> portPositions;

	//saving the max
	unsigned int maxPorts = (unsigned int)ports.size();

	if (maxPorts == 0)
	{
		return;
	}

	// looping through all the ports 
	for (unsigned int i = 0; i < maxPorts; i++)
	{
		portPositions.push_back(ports[i]->GetPosition());
	}


	//sending the port position
	std::vector<XMFLOAT3> RoutesTrails;

	RoutesTrails.push_back(portPositions[7]);
	RoutesTrails.push_back({ 215.919f, 0.0f, -225.238f });
	RoutesTrails.push_back({ 191.061f, 0.0f, -207.018f });
	RoutesTrails.push_back(portPositions[1]);
	GamePlayRoutes.OpenNewRoute(RoutesTrails, 2, 3);

	RoutesTrails.clear();
	RoutesTrails.push_back(portPositions[4]);
	RoutesTrails.push_back({ 133.529f, 0.0f, -63.4039f });
	RoutesTrails.push_back(portPositions[1]);
	GamePlayRoutes.OpenNewRoute(RoutesTrails, rand() % 3, 2);

	RoutesTrails.clear();
	RoutesTrails.push_back(portPositions[1]);
	RoutesTrails.push_back({ 111.517f, 0.0f, 213.225f });
	RoutesTrails.push_back(portPositions[6]);
	GamePlayRoutes.OpenNewRoute(RoutesTrails, rand() % 3, 2);


	RoutesTrails.clear();
	RoutesTrails.push_back(portPositions[5]);
	RoutesTrails.push_back({ -147.206f, 0.0f, 192.916f });
	RoutesTrails.push_back({ -214.413f, 0.0f, 193.818f });
	RoutesTrails.push_back({ -149.447f, 0.0f, -111.947f });
	RoutesTrails.push_back(portPositions[3]);
	GamePlayRoutes.OpenNewRoute(RoutesTrails, rand() % 3, 2);


	RoutesTrails.clear();
	RoutesTrails.push_back(portPositions[3]);
	RoutesTrails.push_back({ -149.447f, 0.0f, -111.947f });
	RoutesTrails.push_back(portPositions[4]);
	GamePlayRoutes.OpenNewRoute(RoutesTrails, rand() % 3, 2);

	RoutesTrails.clear();
	RoutesTrails.push_back(portPositions[2]);
	RoutesTrails.push_back({ 111.517f, 0.0f, 213.225f });
	RoutesTrails.push_back(portPositions[6]);
	GamePlayRoutes.OpenNewRoute(RoutesTrails, rand() % 3, 2);


	RoutesTrails.clear();
	RoutesTrails.push_back(portPositions[2]);
	RoutesTrails.push_back({ 226.355f, 0.0f, -88.5038f });
	RoutesTrails.push_back({ 191.061f, 0.0f, -207.018f });
	RoutesTrails.push_back({ 215.919f, 0.0f, -225.238f });
	RoutesTrails.push_back(portPositions[7]);
	GamePlayRoutes.OpenNewRoute(RoutesTrails, rand() % 3, 2);

	RoutesTrails.clear();
	RoutesTrails.push_back(portPositions[6]);
	RoutesTrails.push_back({ 269.181f, 0.0f, 171.572f });
	RoutesTrails.push_back({ 269.181f, 0.0f, 21.3017f });
	RoutesTrails.push_back({ 226.355f, 0.0f, -88.5038f });
	RoutesTrails.push_back({ 191.061f, 0.0f, -207.018f });
	RoutesTrails.push_back({ 215.919f, 0.0f, -225.238f });
	RoutesTrails.push_back(portPositions[7]);
	GamePlayRoutes.OpenNewRoute(RoutesTrails, rand() % 3, 2);

}

void GameplayScene::Update(float _dt)
{
	// base update
	IScene::Update(_dt);

	spyGUI.UpdateAll(_dt);

	UpdateReinforcements(_dt);

	TradeRoute& GamePlayRoutes = TradeRoute::TradeRoutes();
	GamePlayRoutes.UpdateRoutes(_dt);

	// cull increasing vectors
	CullVectors(_dt);

	// filter increasing vectors
	FilterVectors();

	UpdateHUD(_dt);
	// etc....
	physicsManager.Update(_dt);


	XMFLOAT4X4 newMatrix;
	XMStoreFloat4x4(&newMatrix, XMMatrixTranslation(std::roundf(playerShip->GetPosition().x), 0.0f, std::roundf(playerShip->GetPosition().z)));
	water->GetTransform().SetLocalMatrix(newMatrix);

	/*else
	{
		CGame::GetApplication()->m_pRenderer->SetPostProcessIndex(0);
		m_HUD_Off = false;
	}*/

	if (outofBoundTimer <= 0.0f)
	{
		CGame::GetApplication()->m_pRenderer->SetPostProcessIndex(0);
		playerShip->KillOwner();
		outofBoundTimer = 0.0f;
		static_cast<G2D::GUIText*>(m_hudElems["Warning"])->Text(L"Death...be your treasure!!");
		return;
	}

	auto playerPosition = playerShip->GetPosition();
	if (playerPosition.x > BOUNDARY || playerPosition.x <-BOUNDARY ||
		playerPosition.z >BOUNDARY || playerPosition.z < -BOUNDARY)
	{
		m_HUD_Off = false;
		m_hudElems["Warning"]->Active(true);
		outofBoundTimer -= _dt;
		std::wstringstream wss;
		//wss << L"Seagull(Damien) is coming after you!\n                         " << std::fixed << std::setprecision(2) << outofBoundTimer;
		wss << L"Only death be found in Dark Waters Cap'n!\n                         " << std::fixed << std::setprecision(2) << outofBoundTimer;
		static_cast<G2D::GUIText*>(m_hudElems["Warning"])->Text(wss.str());

		PlayDarkwater();

		CGame::GetApplication()->m_pRenderer->SetPostProcessIndex(3);

		m_hudElems["MinimapShip"]->Active(false);
	}
	else if (spyglass)
	{
		CGame::GetApplication()->m_pRenderer->SetPostProcessIndex(6);
		m_HUD_Off = true;
		if (GetMainCamera()->spyTime >= 1)
			((CGame*)CGame::GetApplication())->TimeScale = 0.3f;
	}
	else
	{
		m_hudElems["Warning"]->Active(false);
		outofBoundTimer = DEATHTIMER;
		CGame::GetApplication()->m_pRenderer->SetPostProcessIndex(0);
		InDarkWater = false;
		m_hudElems["MinimapShip"]->Active(true);
		m_HUD_Off = false;
		if (!m_hudElems["HUD_LayoutTips"]->Active())
		{
			((CGame*)CGame::GetApplication())->TimeScale = 1.0f;
		}

	}

	CGame::GetApplication()->m_pRenderer->SetTotalTime((float)CGame::GetApplication()->Timer.TotalTime());
	if (m_FadeIn)
	{
		sceneManager->LockInput();
		m_Timer -= _dt;
		float timeInv = 1.0f - m_Timer / FADEINTIMER;
		if (timeInv >= 1.0f)
		{
			timeInv = 1.0f;
		}


		m_FadeAmount = timeInv;
		CGame::GetApplication()->m_pRenderer->SetPostProcessFadeAmount(m_FadeAmount);
		m_MainCamera->SetOffset(-25.0f * timeInv);
		if (m_Timer <= 0.0f)
		{
			sceneManager->UnlockInput();
			m_Timer = FADEINTIMER;
			m_FadeIn = false;
			m_FadeAmount = 0.0f;
			m_MainCamera->SetOffset(-25.0f);
			CGame::GetApplication()->m_pRenderer->SetPostProcessFadeAmount(m_FadeAmount);
			m_hudElems["HUD_LayoutTips"]->Active(true);
			((CGame*)CGame::GetApplication())->TimeScale = 0.0f;
		}
	}



	/// /* TUTORIAL STUFF - DO NOT ERASE YET!*/
	/*
	if (m_bTurtorialActive)
	{
		CGame*	game = (CGame*)CGame::GetApplication();
		if (m_hudElems["FinalTurtorialMessage"]->Active() && 0.0f > m_Timer)
		{
			m_hudElems["FinalTurtorialMessage"]->Active(false);
			m_bTurtorialActive = false;
			LoadAIShips();
			fsEventManager::GetInstance()->RegisterClient(fsEvents::SpawnAI, this, &GameplayScene::SpawnAI);

			Renderer::CWaterRenderable* waterRenderable = (Renderer::CWaterRenderable*)(water->GetRenderInfo());

			gfBoundary = { BOUNDARY,BOUNDARY };
			gfBoundaryCenter = { 0.0f, -0.0f };

			waterRenderable->SetDarkWaterHalfWidth(gfBoundary.x);
			waterRenderable->SetDarkWaterHalfHeight(gfBoundary.y);
			waterRenderable->SetDarkWaterCenter(gfBoundaryCenter.x, gfBoundaryCenter.y);
		}
		else if ((m_bPlayerHitPort || playerShip->GetPortCollision()) && !(m_hudElems["SecondShipFirstMessage"]->Active()))
		{
			m_hudElems["FinalTurtorialMessage"]->Active(true);
			m_hudElems["SecondShipSecondMessage"]->Active(false);
			m_Timer -= _dt;
			m_bPlayerHitPort = true;
		}
		else if (nullptr != m_pSecondTurtorialShip && Ship::eDeathStages::ALIVE != m_pSecondTurtorialShip->GetDeathStage())
		{
			m_hudElems["SecondShipFirstMessage"]->Active(false);
			m_hudElems["SecondShipSecondMessage"]->Active(true);

			auto Text = static_cast<G2D::GUIText*>(m_hudElems["FirstShipSecondMessage"]);
			if (game->GamePadIsConnected())
				Text->Text(L"Now port (A) at Tortuga");
			else
				Text->Text(L"Now port (O) at Tortuga");

			m_Timer = 5.0f;
		}
		else if (nullptr != m_pSecondTurtorialShip)
		{

		}
		else if (nullptr != m_pFirstTurtorialShip->GetKiller())
		{
			m_hudElems["FirstShipSecondMessage"]->Active(false);
			m_MainCamera->m_maxDiff = 9;
			if (1.0f > m_Timer)
			{
				m_Timer += _dt;
				m_MainCamera->SetOffset(-25.0f - (m_Timer*15.0f));
			}
			else
			{
				m_pSecondTurtorialShip = CreateAIShip(MILITARYSHIP, DirectX::XMFLOAT3(100.0f, -0.1f, -160.0f));
				((AiController*)m_pSecondTurtorialShip->GetController())->ChaseTarget(playerShip);
				((AiController*)m_pSecondTurtorialShip->GetController())->SetEngageDistance(150.0f);
				((AiController*)m_pSecondTurtorialShip->GetController())->SetAttackDistance(150.0f);
				((AiController*)m_pSecondTurtorialShip->GetController())->ChaseTarget(playerShip);

				auto Text = static_cast<G2D::GUIText*>(m_hudElems["FirstShipSecondMessage"]);
				if (game->GamePadIsConnected())
					Text->Text(L"Switch to cannonball (X) then destroy the navy ship");
				else
					Text->Text(L"Switch to cannonball (N) then destroy the navy ship");

				m_hudElems["SecondShipFirstMessage"]->Active(true);

				std::vector<Renderer::CRenderable*> renderList;
				renderList.push_back(m_pSecondTurtorialShip->GetRenderInfo());
				CGame::GetApplication()->m_pRenderer->SetUpRenderables(renderList);
			}
		}
		else if (m_pFirstTurtorialShip->GetSailsDown())
		{
			((AiController*)m_pFirstTurtorialShip->GetController())->ChaseTarget(nullptr);

			auto Text = static_cast<G2D::GUIText*>(m_hudElems["FirstShipSecondMessage"]);
			if (game->GamePadIsConnected())
				Text->Text(L"Now board(A) by getting close to the navy ship");
			else
				Text->Text(L"Now board(O) by getting close to the navy ship");

			m_hudElems["FirstShipSecondMessage"]->Active(true);
			m_hudElems["FirstShipFirstMessage"]->Active(false);
			m_Timer = 0.0f;
		}
		else if (nullptr == m_pSecondTurtorialCrate->GetCollider())
		{
			((AiController*)m_pFirstTurtorialShip->GetController())->ChaseTarget(nullptr);
			m_pFirstTurtorialShip->SetMaxHealth(10000);

			auto FirstShipFirstText = static_cast<G2D::GUIText*>(m_hudElems["FirstShipFirstMessage"]);
			if (game->GamePadIsConnected())
				FirstShipFirstText->Text(L"Switch to chainshot (Y) then shoot the navy ship");
			else
				FirstShipFirstText->Text(L"Switch to chainshot (M) then shoot the navy ship");

			m_hudElems["FirstShipFirstMessage"]->Active(true);
		}
		else if (nullptr == m_pFirstTurtorialCrate->GetCollider())
			m_hudElems["CrateMessage"]->Active(false);
		else if (!(m_hudElems["HUD_LayoutTips"]->Active()))
			m_hudElems["CrateMessage"]->Active(true);
		else
			m_hudElems["CrateMessage"]->Active(false);
	}
	*/



	if (m_fReconTextTimer > 0.0f)
		m_fReconTextTimer -= _dt;

	if (m_fReconTextTimer <= 0.0f)
	{
		m_fReconTextTimer = 0.0f;
		m_hudElems["ReconText1"]->Active(false);
		m_hudElems["ReconText2"]->Active(false);
	}



	if (m_fBossTextTimer > 0.0f)
	{
		m_fBossTextTimer -= _dt;
	}
	if (m_fBossTextTimer <= 0.0f)
	{
		m_fBossTextTimer = 0.0f;
		m_hudElems["Boss"]->Active(false);
		m_hudElems["Boss2"]->Active(false);
		m_hudElems["Boss3"]->Active(false);
		m_hudElems["Boss4"]->Active(false);
	}

	//playerShip->GetParticleSet().front()->EmitDirectionW(-playerShip->GetForward());
	//playerShip->GetParticleSet().front()->EmitPositionW(playerShip->GetPosition());
	//float speed = playerShip->GetMast()->GetMaxSpeed();
	//playerShip->GetParticleSet().front()->EmitVelocity(-playerShip->GetForward()*speed);




	if (m_cargoSquadSpawnTimer <= 0.0f)
	{
		auto cargoText = static_cast<G2D::GUIText*>(m_hudElems["Cargo"]);
		cargoText->Text(L"There is a trading fleet coming through.");
		m_hudElems["Cargo"]->Active(true);
		m_hudElems["MinimapCargo"]->Active(true);
		m_cargoSquadTextTimer = 5.0f;


		float2 spawnLocations[] =
		{
			{ 320.0f,FSUtilities::Misc::RandFloat(-320.0f,320.0f) },
			{ -320.0f,FSUtilities::Misc::RandFloat(-320.0f,320.0f) },
			{ FSUtilities::Misc::RandFloat(-320.0f,320.0f),320.0f },
			{ FSUtilities::Misc::RandFloat(-320.0f,320.0f),-320.0f },
		};

		int index = rand() % 4;
		if (m_cargoSquad)
		{
			delete m_cargoSquad;
			m_cargoSquad = nullptr;
		}

		m_cargoSquad = new CargoSquad(sceneManager, this, spawnLocations[index], rand() % 8);
		m_cargoSquadSpawnTimer = CARGOSPAWNTIME;
	}
	else
	{
		m_cargoSquadSpawnTimer -= _dt;
		m_cargoSquadTextTimer -= _dt;
		if (m_cargoSquadTextTimer <= 0.0f)
		{
			m_hudElems["Cargo"]->Active(false);
		}
		if (m_cargoSquad)
		{
			m_cargoSquad->Update(_dt);
			//m_cargoSquad->DeleteDeadShips();
		}
	}


	//port_help = false;
}

void GameplayScene::Render(void)
{
	// base render
	IScene::Render();
	if (!m_FadeIn && !m_HUD_Off)
	{
		//FSParticle::CParticleManager::GetInstance()->RenderAll();
		m_GUIManager.RenderAll();
	}
	else if (!m_FadeIn)
	{
		spyGUI.RenderAll();
	}

	// etc....
}

void GameplayScene::Input_Start(void)
{
	return;
	//// input manager
	//CInputManager* input_manager = InputManager();


	//// Activate Input: instructions, pause
	//input_manager->ActivateKey(VK_F1);
	//input_manager->ActivateKey('P');
	////input_manager->ActivateKey('Q');
	////input_manager->ActivateKey('Z');


	//// Player Gameplay Input
	//((PlayerController*)playerShip->GetController())->Input_Start();
}

void GameplayScene::Input_End(void)
{

	return;
	//// input manager
	//CInputManager* input_manager = InputManager();


	//// Dectivate Input: WASD, numpad, arrows
	//input_manager->DeActivateKey(VK_F1);
	//input_manager->DeActivateKey('P');
	////input_manager->DeActivateKey('Q');
	////input_manager->DeActivateKey('Z');
	//	//Entity3D* s = entityManager.CreatePlayerShip();

	//// Player Gameplay Input
	//((PlayerController*)playerShip->GetController())->Input_End();
}

bool GameplayScene::Input_Keyboard(void)
{
	//CInputManager* input_manager = InputManager();
	CGame*							game = (CGame*)CGame::GetApplication();
	auto							kbs = Keyboard::Get().GetState();
	Keyboard::KeyboardStateTracker*	kbst = game->GetKeyboardTracker();

    if ( kbst->IsKeyPressed( Keyboard::Keys::F10 ) == true && m_FadeIn == false ) {
        GameSetting::GetRef().SaveScreeshotToFile(g_ScreenShotPathW);
    }

	// freeze and tips
	if (kbst->IsKeyPressed(Keyboard::Keys::F1) == true && m_FadeIn == false)
	{
		if (game->TimeScale > 0.0f) {
			m_hudElems["HUD_LayoutTips"]->Active(true);
			m_hudElems["HUD_GoldIcon"]->Active(true);
			m_hudElems["GoldStatus"]->Active(true);
			m_hudElems["HUD_RumIcon"]->Active(true);
			m_hudElems["RumStatus"]->Active(true);
			m_hudElems["HUD_FoodIcon"]->Active(true);
			m_hudElems["FoodStatus"]->Active(true);
			m_hudElems["HUD_CrewIcon"]->Active(true);
			m_hudElems["CrewStatus"]->Active(true);
			game->TimeScale = 0.0f;
		}
		else {
			m_hudElems["HUD_LayoutTips"]->Active(false);
			m_hudElems["HUD_GoldIcon"]->Active(false);
			m_hudElems["GoldStatus"]->Active(false);
			m_hudElems["HUD_RumIcon"]->Active(false);
			m_hudElems["RumStatus"]->Active(false);
			m_hudElems["HUD_FoodIcon"]->Active(false);
			m_hudElems["FoodStatus"]->Active(false);
			m_hudElems["HUD_CrewIcon"]->Active(false);
			m_hudElems["CrewStatus"]->Active(false);
			game->TimeScale = 1.0f;
		}
		spyglass = false;
	}

	if (game->TimeScale < 0.1f)
		return OLD_SCENE;

	/* Input: pause */
	if (kbst->IsKeyPressed(Keyboard::Keys::Escape) == true)
	{
		//DirectX::SaveDDSTextureToFile( game->m_pRenderer->m_deviceResources->GetD3DDeviceContext(), reinterpret_cast<ID3D11Resource*>(game->m_pRenderer->m_deviceResources->GetBackBuffer()), L"./screenShot.tmp" );
		GamePad::Get().SetVibration(0, 0, 0);
		PauseSounds();
		sceneManager->PushScene(SceneType::PAUSE);
		spyglass = false;
		return NEW_SCENE;
	}


	// stay in same scene
	return OLD_SCENE;
}

bool GameplayScene::Input_Gamepad(void)
{
	CGame*							game = (CGame*)CGame::GetApplication();
	auto							gps = GamePad::Get().GetState(0);
	GamePad::ButtonStateTracker*	gpst = game->GetGamePadTracker();


    if ( gpst->rightStick == GamePad::ButtonStateTracker::PRESSED &&
         gpst->leftStick == GamePad::ButtonStateTracker::PRESSED && m_FadeIn == false ) {
        GameSetting::GetRef().SaveScreeshotToFile( g_ScreenShotPathW );
    }

	/* Input: instructions */
	if (gpst->back == GamePad::ButtonStateTracker::ButtonState::PRESSED && m_FadeIn == false)
	{
		if (game->TimeScale > 0.0f) {
			m_hudElems["HUD_LayoutTips"]->Active(true);
			m_hudElems["HUD_GoldIcon"]->Active(true);
			m_hudElems["GoldStatus"]->Active(true);
			m_hudElems["HUD_RumIcon"]->Active(true);
			m_hudElems["RumStatus"]->Active(true);
			m_hudElems["HUD_FoodIcon"]->Active(true);
			m_hudElems["FoodStatus"]->Active(true);
			m_hudElems["HUD_CrewIcon"]->Active(true);
			m_hudElems["CrewStatus"]->Active(true);
			game->TimeScale = 0.0f;
		}
		else {
			m_hudElems["HUD_LayoutTips"]->Active(false);
			m_hudElems["HUD_GoldIcon"]->Active(false);
			m_hudElems["GoldStatus"]->Active(false);
			m_hudElems["HUD_RumIcon"]->Active(false);
			m_hudElems["RumStatus"]->Active(false);
			m_hudElems["HUD_FoodIcon"]->Active(false);
			m_hudElems["FoodStatus"]->Active(false);
			m_hudElems["HUD_CrewIcon"]->Active(false);
			m_hudElems["CrewStatus"]->Active(false);
			game->TimeScale = 1.0f;
		}
		//FSParticle::CParticleManager::GetInstance()->PauseAll();
		//sceneManager->PopScene();
		//sound->Play3DSound(AK::EVENTS::PAUSEBGM);
		//sceneManager->PushScene(SceneType::INSTRUCTIONS);
		//return NEW_SCENE;
	}
	if (game->TimeScale < 0.1f)
		return OLD_SCENE;


	/* Input: pause */
	if (gpst->start == GamePad::ButtonStateTracker::ButtonState::PRESSED)
	{
		//FSParticle::CParticleManager::GetInstance()->PauseAll();
		GamePad::Get().SetVibration(0, 0, 0);
		PauseSounds();
		sceneManager->PushScene(SceneType::PAUSE);
		spyglass = false;
		return NEW_SCENE;
	}


	/* Input: on-screen HUD instructions */
	if (gpst->rightStick == GamePad::ButtonStateTracker::ButtonState::PRESSED)
	{
		ToggleHUDInstructions(false);
		spyglass = false;
	}


	// stay in same scene
	return OLD_SCENE;
}

void GameplayScene::Input_Cheats(void)
{
	//CInputManager* input_manager = InputManager();
	CGame*							game = (CGame*)CGame::GetApplication();
	auto							kbs = Keyboard::Get().GetState();
	Keyboard::KeyboardStateTracker*	kbst = game->GetKeyboardTracker();
	SoundManager*					sound = game->GetSoundManager();


	// CTRL key (Left or Right) is NOT being held down
	if (kbs.IsKeyDown(Keyboard::Keys::LeftControl) == false && kbs.IsKeyDown(Keyboard::Keys::RightControl) == false)
		return;


	// Ship -> PlayerShip
	PlayerShip* player = static_cast<PlayerShip*>(playerShip);



	/* Input: CTRL + numpad0 */
	//if (GetAsyncKeyState(VK_NUMPAD0) & 1)
	//if (input_manager->GetKeyPress(VK_NUMPAD0) == true)
	if (kbst->IsKeyPressed(Keyboard::Keys::NumPad0) == true)
	{
		/* Toggle God Mode */
		//-HP = MAX
		//-Invincibility
		player->ToggleGodMode();
		//sound->Play3DSound((player->GetGodMode() == true) ? AK::EVENTS::PLAYUPGRADE : AK::EVENTS::PLAYREJECT);
		//Log("Cheat: Toggle God Mode");
	}


	/* Input: CTRL + numpad1 */
	//if (GetAsyncKeyState(VK_NUMPAD1) & 1)
	//if (input_manager->GetKeyPress(VK_NUMPAD1) == true)
	if (kbs.NumPad1)	//if (kbst->IsKeyPressed(Keyboard::Keys::NumPad1) == true)
	{
		/* Increase progression */
		//-rep += 1
		player->AddReputation(1.0f);
		//Log("Cheat: Rep++");
	}


	/* Input: CTRL + numpad2 */
	//if (GetAsyncKeyState(VK_NUMPAD2) & 1)
	//if (input_manager->GetKeyPress(VK_NUMPAD2) == true)
	if (kbst->IsKeyPressed(Keyboard::Keys::NumPad2) == true)
	{
		/* Refill Resources */
		//-rum = MAX
		//-crew = MAX
		//-water/food = MAX
		//-gold = MAX
		player->RefillResources();
		//sound->Play3DSound(AK::EVENTS::PLAYUPGRADE);
		//Log("Cheat: MAX Resources");
	}


	/* Input: CTRL + numpad3 */
	//if (GetAsyncKeyState(VK_NUMPAD3) & 1)
	//if (input_manager->GetKeyPress(VK_NUMPAD3) == true)
	if (kbst->IsKeyPressed(Keyboard::Keys::NumPad3) == true)
	{
		/* Max Level */
		//-Ship.Lv = MAX
		player->GoToMaxLevel();
		//sound->Play3DSound(AK::EVENTS::PLAYUPGRADE);
		//Log("Cheat: MAX Level");
	}


	/* Input: CTRL + numpad4 */
	//if (GetAsyncKeyState(VK_NUMPAD4) & 1)
	//if (input_manager->GetKeyPress(VK_NUMPAD4) == true)
	if (kbst->IsKeyPressed(Keyboard::Keys::NumPad4) == true)
	{
		/* Spawn Boss */
		//-aiShips.push_back(new Ship);
		//Log("Cheat: Spawn Boss");
	}


	///* TUTORIAL STUFF - DO NOT ERASE YET! */
	/*
	if (kbst->IsKeyPressed(Keyboard::Keys::NumPad5) == true)
	{
		if (m_bTurtorialActive)
		{
			LoadAIShips();
			fsEventManager::GetInstance()->RegisterClient(fsEvents::SpawnAI, this, &GameplayScene::SpawnAI);
			m_hudElems["FinalTurtorialMessage"]->Active(false);
			m_hudElems["SecondShipSecondMessage"]->Active(false);
			m_hudElems["SecondShipFirstMessage"]->Active(false);
			m_hudElems["FirstShipSecondMessage"]->Active(false);
			m_hudElems["FirstShipFirstMessage"]->Active(false);
			m_Timer = 0.0f;
			m_hudElems["CrateMessage"]->Active(false);
			m_MainCamera->m_maxDiff = 9;
			m_MainCamera->SetOffset(-25.0f);

			Renderer::CWaterRenderable* waterRenderable = (Renderer::CWaterRenderable*)(water->GetRenderInfo());
			gfBoundary = { BOUNDARY,BOUNDARY };
			gfBoundaryCenter = { 0.0f, -0.0f };

			waterRenderable->SetDarkWaterHalfWidth(gfBoundary.x);
			waterRenderable->SetDarkWaterHalfHeight(gfBoundary.y);
			waterRenderable->SetDarkWaterCenter(gfBoundaryCenter.x, gfBoundaryCenter.y);
		}

		m_bTurtorialActive = false;
	}
	*/




	/* New cheats! CTRL + numpad[7-9] */
	float camdt = 1.0f;

	// Zoom in
	if (kbs.NumPad7)
		m_MainCamera->SetOffset(m_MainCamera->GetOffset() + camdt);
	
	// Zoom out
	if (kbs.NumPad9)
		m_MainCamera->SetOffset(m_MainCamera->GetOffset() - camdt);
	
	// Reset zoom to default
	if (kbst->IsKeyPressed(Keyboard::Keys::NumPad8) == true)
		m_MainCamera->SetOffset(-25.0f);



	// Increase # of cannons fired
	/*
	if (kbst->IsKeyPressed(Keyboard::Keys::NumPad4) == true)
	{
		SideCannonHandler* sch = player->GetSideCannonHandler();
		if (sch != nullptr)
			sch->SetNumAcquiredCannons(sch->GetNumAcquiredCannons() + 1);
	}
	*/
}

void GameplayScene::LoadHudList() {
	G2D::CGUIManager::LoadXMLConfig("../../FullSail/Resources/GUI/Configs/GameplayScene.xml", m_hudElems);
	G2D::CGUIManager::LoadXMLConfig("../../FullSail/Resources/GUI/Configs/spyglass.xml", m_spyElems);

    for ( auto it = m_hudElems.begin(); it != m_hudElems.end(); it++ ) {
        m_GUIManager.AddEntity( it->second );
    }
    for ( auto it = m_spyElems.begin(); it != m_spyElems.end(); it++ ) {
        spyGUI.AddEntity( it->second );
    }
	m_hudElems["SailsSpeedDamage"]->SpriteData().customFloat1 = 1.0f;
}

void GameplayScene::ReleaseHudList() {
	for (auto it = m_hudElems.begin(); it != m_hudElems.end(); it++) {
		m_GUIManager.RemoveEntity(it->second);
		SafeRelease(it->second);
	}
	for (auto it = m_spyElems.begin(); it != m_spyElems.end(); it++) {
		spyGUI.RemoveEntity(it->second);
		SafeRelease(it->second);
	}
}

void GameplayScene::InitializeScene(void)
{

	//******************************************//
	//**************Scene layout****************//
	//******************************************//

	physicsManager = PhysicsManager();
	m_pReinforcement = nullptr;

	//Creating the the waypoints for the Ai Ship
	//ai_waypoints.push_back(XMFLOAT3(-142.204514f, 0, 84.5360565f));

	ai_waypoints.push_back(XMFLOAT3(2.40453f, -0.1f, 17.3623f));	//Not
	ai_waypoints.push_back(XMFLOAT3(-189.07f, -0.1f, -52.6343f));//Snake
	ai_waypoints.push_back(XMFLOAT3(-138.254f, -0.1f, -227.739f));	//Bottom
	ai_waypoints.push_back(XMFLOAT3(-226.359f, -0.1f, 216.963f));//Dragon Tooth
	ai_waypoints.push_back(XMFLOAT3(-19.7164f, -0.1f, 149.543f));//Canal

	//Add points after this line until we have waypoint creation through XML

	safeZones.push_back(XMFLOAT3(-18.5592f, 0, -24.2072f));
	safeZones.push_back(XMFLOAT3(29.1576f, 0, 70.1131f));
	safeZones.push_back(XMFLOAT3(208.024f, 0, 101.203f));
	safeZones.push_back(XMFLOAT3(132.154f, 0, 215.552f));

	//Setting up the SpawnPoints
	SpawnPoints.push_back(XMFLOAT3(11.9934f, 0, 18.1399f));
	SpawnPoints.push_back(XMFLOAT3(-126.748f, 0, -237.127f));
	SpawnPoints.push_back(XMFLOAT3(202.643f, 0, 142.876f));
	SpawnPoints.push_back(XMFLOAT3(-234.115f, 0, 240.981f));
	SpawnPoints.push_back(XMFLOAT3(-8.96196f, 0, 177.479f));
	SpawnPoints.push_back(XMFLOAT3(-202.203f, 0, 47.1904f));
	SpawnPoints.push_back(XMFLOAT3(-66.2882f, 0, 74.0914f));

	//Register for events
	fsEventManager::GetInstance()->RegisterClient(fsEvents::LoadObject, this, &GameplayScene::LoadObject);
	fsEventManager::GetInstance()->RegisterClient(fsEvents::AI_Help, this, &GameplayScene::CallInMilitary);


	// Load stuff
	LoadWater();
	LoadScene();
	LoadHudList();
	//LoadGulls();
	//bossShip = LoadBossShip();

	////* Turtorial stuff - DO NOT ERASE YET! */
	/*
	m_bTurtorialActive = false;
	m_bPlayerHitPort = false;
	m_pFirstTurtorialCrate = nullptr;
	m_pSecondTurtorialCrate = nullptr;
	m_pFirstTurtorialShip = nullptr;
	m_pSecondTurtorialShip = nullptr;



	if (m_bTurtorialActive)
	{
		// Walls

		//BoxCollider* m_pFirstWall = newx BoxCollider(GameCollision, newObject->GetLocalMatrix(), newObject, thing, XMFLOAT3(0.8f, rand() % 3, 1.5f), XMFLOAT3(0, 0.75f, 0), false);

		// Turtorial crates... bread crumbs
		DirectX::XMFLOAT4X4 mTurtorialMat = (DirectX::XMFLOAT4X4)playerShip->GetWorldMatrix();
		mTurtorialMat.m[3][0] += -3;
		mTurtorialMat.m[3][2] += -3;
		Entity3D* TCrate = sceneManager->GetEntityManager().CreateEntity3D("Crate", mTurtorialMat);
		m_pFirstTurtorialCrate = ((Crate*)TCrate);
		m_pFirstTurtorialCrate->SetGold(10);
		m_pFirstTurtorialCrate->SetRum(5);
		m_pFirstTurtorialCrate->SetWaterFood(5);
		m_pFirstTurtorialCrate->SetLifespan(1000000.0f);
		sceneManager->GetEntityManager().AddEntity(IScene::GAMEPLAY, EntityManager::EntityBucket::RENDERABLE, m_pFirstTurtorialCrate);

		for (size_t i = 0; i < 5; i++)
		{
			mTurtorialMat.m[3][0] += -3;
			mTurtorialMat.m[3][2] += -3;
			Entity3D* TCrate1 = sceneManager->GetEntityManager().CreateEntity3D("Crate", mTurtorialMat);
			Crate* pTurtorialCrate = ((Crate*)TCrate1);
			pTurtorialCrate->SetGold(10);
			pTurtorialCrate->SetRum(5);
			pTurtorialCrate->SetWaterFood(5);
			pTurtorialCrate->SetLifespan(1000000.0f);
			sceneManager->GetEntityManager().AddEntity(IScene::GAMEPLAY, EntityManager::EntityBucket::RENDERABLE, pTurtorialCrate);
		}

		for (size_t i = 0; i < 4; i++)
		{
			mTurtorialMat.m[3][0] += -3;
			mTurtorialMat.m[3][2] += 3;
			Entity3D* TCrate1 = sceneManager->GetEntityManager().CreateEntity3D("Crate", mTurtorialMat);
			Crate* pTurtorialCrate = ((Crate*)TCrate1);
			pTurtorialCrate->SetGold(10);
			pTurtorialCrate->SetRum(5);
			pTurtorialCrate->SetWaterFood(5);
			pTurtorialCrate->SetLifespan(1000000.0f);
			sceneManager->GetEntityManager().AddEntity(IScene::GAMEPLAY, EntityManager::EntityBucket::RENDERABLE, pTurtorialCrate);
		}

		mTurtorialMat.m[3][0] += -3;
		mTurtorialMat.m[3][2] += 3;
		Entity3D* TCrate1 = sceneManager->GetEntityManager().CreateEntity3D("Crate", mTurtorialMat);
		m_pSecondTurtorialCrate = ((Crate*)TCrate1);
		m_pSecondTurtorialCrate->SetGold(10);
		m_pSecondTurtorialCrate->SetRum(5);
		m_pSecondTurtorialCrate->SetWaterFood(5);
		m_pSecondTurtorialCrate->SetLifespan(1000000.0f);
		sceneManager->GetEntityManager().AddEntity(IScene::GAMEPLAY, EntityManager::EntityBucket::RENDERABLE, m_pSecondTurtorialCrate);

		mTurtorialMat.m[3][0] += -5;
		m_pFirstTurtorialShip = CreateAIShip(MILITARYSHIP, DirectX::XMFLOAT3(mTurtorialMat.m[3][0], mTurtorialMat.m[3][1], mTurtorialMat.m[3][2]));
		m_pFirstTurtorialShip->SetCurrCrew(2);
	}
	*/

	//LoadAmmoPool();

	// Attach camera to player
	m_MainCamera.get()->m_Target = playerShip;
	m_MainCamera.get()->m_targetSpeed = &playerShip->GetSpeed();
	m_MainCamera.get()->m_maxTargetSpeed = &playerShip->GetMaxSpeed();
	//playerShip->GetTransform().SetChild(&((IEntity*)m_MainCamera.get())->GetTransform());
	m_MainCamera.get()->GetTransform().SetParent(&playerShip->GetTransform());
	aiShips.push_back(playerShip);


	// setup audio
	SoundManager* sound = ((CGame*)CGame::GetApplication())->GetSoundManager();
	sound->RegisterListener(m_MainCamera.get(), "GameplayScene Camera");
	sound->RegisterEntity(playerShip, "Player Ship");


	//// get renderables
	//std::vector<Renderer::CRenderable*> renderList = sceneManager->GetEntityManager().GetRenderables(GetType(), EntityManager::EntityBucket::RENDERABLE);

	//// add renderables
	//CGame::GetApplication()->m_pRenderer->SetUpRenderables(renderList);

	outofBoundTimer = DEATHTIMER;


	BossTeam.Clear();
	reconTeam.Clear();
	bossReinforcementTeam.Clear();


	m_hudElems["Cargo"]->Active(false);
	m_pReinforcement = nullptr;

	bossReinforcementTeam.squad = newx Squad();
	m_bShowBossMinimap = false;
	m_bShowCargoMinimap = false;

	initialized = true;
}

void GameplayScene::ShutdownScene(void)
{
	//unregister the client
	fsEventManager::GetInstance()->UnRegisterClient(fsEvents::LoadObject, this, &GameplayScene::LoadObject);
	fsEventManager::GetInstance()->UnRegisterClient(fsEvents::AI_Help, this, &GameplayScene::CallInMilitary);


	TradeRoute& GamePlayRoutes = TradeRoute::TradeRoutes();
	GamePlayRoutes.CloseAllRoutes();
	GamePlayRoutes.m_vCheckViewObjects.clear();
	m_pReinforcement = nullptr;
	ai_waypoints.clear();

	// clear renderables
	CGame::GetApplication()->m_pRenderer->ClearRenderables();

	// clear particle map
	CGame::GetApplication()->m_pRenderer->ParticleGroup().clear();


	// audio
	// unattach camera from player
	//m_MainCamera.get()->GetTransform().SetParent(nullptr);

	ReleaseHudList();

	SoundManager* sound = ((CGame*)CGame::GetApplication())->GetSoundManager();
	sound->UnregisterEntity(playerShip);
	sound->UnregisterListener(m_MainCamera.get());
	//m_MainCamera->GetTransform().SetParent( nullptr );
	//m_MainCamera->m_Target = nullptr;


	sceneManager->GetEntityManager().RemoveAllEntities(GetType(), EntityManager::EntityBucket::RENDERABLE);
	sceneManager->GetEntityManager().RemoveAllEntities(GetType(), EntityManager::EntityBucket::NON_RENDERABLE);


	delete water;
	water = nullptr;

	//delete playerShip;
	playerShip = nullptr;

	reconTeam.Clear();
	bossReinforcementTeam.Clear();
	//reconTeam.fleet.clear();
	//if (reconTeam.squad != nullptr)
	//{
	//	delete reconTeam.squad;
	//	reconTeam.squad = nullptr;
	//}


	FilterVectors();

	for (size_t i = 0; i < aiShips.size(); i++)
	{
		//if (aiShips[i] == nullptr)
		//	continue;
		delete aiShips[i];
		aiShips[i] = nullptr;
	}
	aiShips.clear();

	for (size_t i = 0; i < islands.size(); i++)
	{
		//if (islands[i] == nullptr)
		//	continue;
		delete islands[i];
		islands[i] = nullptr;
	}
	islands.clear();

	for (size_t i = 0; i < ports.size(); i++)
	{
		//if (islands[i] == nullptr)
		//	continue;
		delete ports[i];
		ports[i] = nullptr;
	}
	ports.clear();

	for (size_t i = 0; i < ammo.size(); i++)
	{
		//if (ammo[i] == nullptr)
		//	continue;
		delete ammo[i];
		ammo[i] = nullptr;
	}
	ammo.clear();
	/*
	for (size_t i = 0; i < AMMO_LIMIT; i++)
	{
		delete ammopool[i];
		ammopool[i] = nullptr;
	}
	*/


	for (size_t i = 0; i < crates.size(); i++)
	{
		//if (crates[i] == nullptr)
		//	continue;
		delete crates[i];
		crates[i] = nullptr;
	}
	crates.clear();

	for (size_t i = 0; i < weather.size(); i++)
	{
		//if (weather[i] == nullptr)
		//	continue;
		delete weather[i];
		weather[i] = nullptr;
	}
	weather.clear();


	for (auto&& gull : gulls)
		delete gull;
	gulls.clear();


	BossTeam.Clear();
	reconTeam.Clear();
	bossReinforcementTeam.Clear();
	m_bShowBossMinimap = false;
	m_bShowCargoMinimap = false;


	m_cargoSquadSpawnTimer = CARGOSPAWNTIME;
	if (m_cargoSquad)
	{
		delete m_cargoSquad;
		m_cargoSquad = nullptr;
	}

	initialized = false;
}

void GameplayScene::SwapScenePush(void)
{
	// a new scene is to be added over this one
	SoundManager* soundManager = ((CGame*)CGame::GetApplication())->GetSoundManager();
	soundManager->Play3DSound(AK::EVENTS::STOP_ALL);

	return;
	size_t vecsize = 0;



	// water
	if (water->ToBeDeleted() == false)
	{
	}

	// player
	if (playerShip->ToBeDeleted() == false)
	{
	}

	// ai ships
	vecsize = aiShips.size();
	for (size_t i = 0; i < vecsize; i++)
	{
		if (aiShips[i]->ToBeDeleted() == false)
			continue;

	}

	// ammo
	vecsize = ammo.size();
	for (size_t i = 0; i < vecsize; i++)
	{
		if (ammo[i]->ToBeDeleted() == false)
			continue;

	}

	// crates
	vecsize = crates.size();
	for (size_t i = 0; i < vecsize; i++)
	{
		if (crates[i]->ToBeDeleted() == false)
			continue;

	}

	// weather
	vecsize = weather.size();
	for (size_t i = 0; i < vecsize; i++)
	{
		if (weather[i]->ToBeDeleted() == false)
			continue;

	}


}

void GameplayScene::SwapScenePop(void)
{
	// this scene is to be the scene on top of stack
	SoundManager* soundManager = ((CGame*)CGame::GetApplication())->GetSoundManager();
	soundManager->Play3DSound(AK::EVENTS::STOP_ALL);

	return;
	size_t vecsize = 0;



	// water
	if (water->ToBeDeleted() == false)
	{
	}

	// player
	if (playerShip->ToBeDeleted() == false)
	{
	}

	// ai ships
	vecsize = aiShips.size();
	for (size_t i = 0; i < vecsize; i++)
	{
		if (aiShips[i]->ToBeDeleted() == false)
			continue;

	}

	// ammo
	vecsize = ammo.size();
	for (size_t i = 0; i < vecsize; i++)
	{
		if (ammo[i]->ToBeDeleted() == false)
			continue;

	}

	// crates
	vecsize = crates.size();
	for (size_t i = 0; i < vecsize; i++)
	{
		if (crates[i]->ToBeDeleted() == false)
			continue;

	}

	// weather
	vecsize = weather.size();
	for (size_t i = 0; i < vecsize; i++)
	{
		if (weather[i]->ToBeDeleted() == false)
			continue;

	}


}

#pragma endregion

int GameplayScene::GetNumActiveAIShips(void)
{
	int count = 0;
	for (size_t i = 0; i < aiShips.size(); i++)
	{
		// delete check
		if (aiShips[i]->ToBeDeleted() == true)
			continue;

		// player check
		if (aiShips[i] == playerShip)
			continue;

		// count++
		count++;
	}
	return count;
}

void GameplayScene::GetActiveAIShips(std::vector<Ship*>& shipvec)
{
	size_t aisize = aiShips.size();
	for (size_t i = 0; i < aisize; i++)
	{
		// delete check
		if (aiShips[i]->ToBeDeleted() == true)
			continue;

		// player check
		if (aiShips[i] == playerShip)
			continue;

		//// alignment check
		//if (aiShips[i]->GetAlignment() == Ship::eAlignment::PLAYER)
		//	continue;

		// count++
		shipvec.push_back(aiShips[i]);
	}



	// cargo ships - These ships are NOT in the "aiShips" vector
	if (m_cargoSquad != nullptr)
	{
		size_t cargosize = m_cargoSquad->GetFleet().size();
		for (size_t i = 0; i < cargosize; i++)
		{
			// null check
			if (m_cargoSquad->GetFleet()[i] == nullptr)
				continue;

			// delete check
			if (m_cargoSquad->GetFleet()[i]->ToBeDeleted() == true)
				continue;

			// player check
			if (m_cargoSquad->GetFleet()[i] == playerShip)
				continue;

			// count++
			shipvec.push_back(m_cargoSquad->GetFleet()[i]);
		}
	}

}

Port* GameplayScene::GetPort(std::wstring name)
{
	size_t numports = ports.size();
	for (size_t i = 0; i < numports; i++)
	{
		if (ports[i]->GetPortName() != name)
			continue;

		return ports[i];
	}

	return nullptr;
}

void GameplayScene::ResetGameplay(void)
{
	// setup camera
	SoundManager* sound = ((CGame*)CGame::GetApplication())->GetSoundManager();
	sound->UnregisterListener(m_MainCamera.get());
	m_MainCamera.reset(newx Camera);
	EntityLoader::ReadCameraOffset(m_MainCamera.get());

	ShutdownScene();
	InitializeScene();

	spyglass = false;

	// port sales
	fsEventManager* fsem = fsEventManager::GetInstance();
	for (size_t i = 0; i < 3; i++)
	{
		fsem->FireEvent(fsEvents::PortSale, nullptr);
	}
	// reset statistics data
	GameStatistics::GetRef().Reset();
}

void GameplayScene::FilterVectors(void)
{
	// ai ships
	if (aiShips.size() > FILTER_LIMIT)
	{
		for (size_t i = 0; i < aiShips.size(); i++)
		{
			if (aiShips[i]->ToBeDeleted() == false)
				continue;

			// check other vectors
			for (size_t j = 0; j < reconTeam.fleet.size(); j++)
			{
				if (reconTeam.fleet[j] != aiShips[i])
					continue;
				reconTeam.fleet.erase(reconTeam.fleet.begin() + j);
				if (reconTeam.squad != nullptr)
					reconTeam.squad->RemoveSquadMate(aiShips[i]->GetController()->GetBoid());

				float reconscale = m_hudElems["MinimapRecon"]->Scale() - 0.6f;
				if (reconscale <= 0.0f)
					reconscale = 1.0f;
				m_hudElems["MinimapRecon"]->Scale(reconscale);
				break;
			}


			// delete ai ship
			delete aiShips[i];
			aiShips[i] = nullptr;

			// erase ai ship
			auto begin = aiShips.begin();
			aiShips.erase(begin + i);
			i--;
		}
	}
	//if (m_cargoSquad != nullptr)
	//{
	//	m_cargoSquad->DeleteDeadShips();
	//}




	// ammo
	if (ammo.size() > FILTER_LIMIT)
	{
		for (size_t i = 0; i < ammo.size(); i++)
		{
			if (ammo[i]->ToBeDeleted() == false)
				continue;

			// delete ammo
			delete ammo[i];
			ammo[i] = nullptr;

			// erase ammo
			auto begin1 = ammo.begin();
			ammo.erase(begin1 + i);
			//ammo_timers.erase(ammo_timers.begin() + i);
			i--;
		}
	}



	// crates
	if (crates.size() > FILTER_LIMIT)
	{
		for (size_t i = 0; i < crates.size(); i++)
		{
			if (crates[i]->ToBeDeleted() == false)
				continue;

			// delete ai ship
			delete crates[i];
			crates[i] = nullptr;

			// erase ai ship
			auto begin = crates.begin();
			crates.erase(begin + i);
			i--;
		}
	}



	// weather
	if (weather.size() > FILTER_LIMIT)
	{
		for (size_t i = 0; i < weather.size(); i++)
		{
			if (weather[i]->ToBeDeleted() == false)
				continue;

			// delete ai ship
			delete weather[i];
			weather[i] = nullptr;

			// erase ai ship
			auto begin = weather.begin();
			weather.erase(begin + i);
			i--;
		}
	}

}

void GameplayScene::CullVectors(float _dt)
{
	cull_timer -= _dt;

	if (cull_timer > 0.0f)
		return;

	cull_timer = CULL_LIMIT;



	// ai ships
	for (size_t i = 0; i < aiShips.size(); i++)
	{
		if (aiShips[i]->ToBeDeleted() == false)
			continue;

		// check other vectors
		for (size_t j = 0; j < reconTeam.fleet.size(); j++)
		{
			if (reconTeam.fleet[j] != aiShips[i])
				continue;
			reconTeam.fleet.erase(reconTeam.fleet.begin() + j);
			if (reconTeam.squad != nullptr)
				reconTeam.squad->RemoveSquadMate(aiShips[i]->GetController()->GetBoid());

			float reconscale = m_hudElems["MinimapRecon"]->Scale() - 0.6f;
			if (reconscale <= 0.0f)
				reconscale = 1.0f;
			m_hudElems["MinimapRecon"]->Scale(reconscale);
			break;
		}


		// delete ai ship
		delete aiShips[i];
		aiShips[i] = nullptr;

		// erase ai ship
		auto begin = aiShips.begin();
		aiShips.erase(begin + i);
		i--;
	}



	// ammo
	for (size_t i = 0; i < ammo.size(); i++)
	{
		if (ammo[i]->ToBeDeleted() == false)
			continue;

		// delete ammo
		delete ammo[i];
		ammo[i] = nullptr;

		// erase ammo
		auto begin = ammo.begin();
		ammo.erase(begin + i);
		i--;
	}



	// crates
	for (size_t i = 0; i < crates.size(); i++)
	{
		if (crates[i]->ToBeDeleted() == false)
			continue;

		// delete ai ship
		delete crates[i];
		crates[i] = nullptr;

		// erase ai ship
		auto begin = crates.begin();
		crates.erase(begin + i);
		i--;
	}



	// weather
	for (size_t i = 0; i < weather.size(); i++)
	{
		if (weather[i]->ToBeDeleted() == false)
			continue;

		// delete ai ship
		delete weather[i];
		weather[i] = nullptr;

		// erase ai ship
		auto begin = weather.begin();
		weather.erase(begin + i);
		i--;
	}


	// re-setup renderables
	//std::vector<Renderer::CRenderable*> renderList = sceneManager->GetEntityManager().GetRenderables(GetType(), EntityManager::EntityBucket::RENDERABLE);
	//CGame::GetApplication()->m_pRenderer->SetUpRenderables(renderList);

}

void GameplayScene::LoadWater(void)
{
	XMFLOAT3			w_pos = XMFLOAT3(0.0f, 0.0f, 0.0f);	//XMFLOAT3(((rand() % 14) - 7.0f), float(rand() % 8), 0.0f);
	DirectX::XMMATRIX	mat = DirectX::XMMATRIX();			//XMMatrixTranslation(w_pos.x, w_pos.y, w_pos.z);
	XMFLOAT4X4			f4x4 = XMFLOAT4X4();


	// Add water
	//water = sceneManager->GetEntityManager().CreateEntity3D("testDefaultMaterial.xml", "testWaterPlane.mesh", 2, 1, "WaterDiffuse.dds", nullptr, "WaterHeight.dds", nullptr, "SkyboxOcean.dds");
	water = sceneManager->GetEntityManager().CreateWater();
	sceneManager->GetEntityManager().AddEntity(GetType(), EntityManager::EntityBucket::RENDERABLE, water);
}

void GameplayScene::LoadPlayerShip(void)
{
	//Starting position of the Player is handled in xml

	// Load prev player data
	static_cast<PlayerShip*>(playerShip)->LoadPlayer();
}

void GameplayScene::LoadScene(void)
{
	sceneManager->GetEntityManager().LoadXMLScene("SceneCaribbean");

	SetUpTradeRoutes();
}

Ship* GameplayScene::LoadBossShip(void)
{
	DirectX::XMFLOAT4X4	mat = DirectX::XMFLOAT4X4(1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 310.0f, 1.0f);

	Ship*	newShip = CreateAIShip("BossShip", XMFLOAT3(0.0f, 0.0f, 310.0f));
	if (newShip == nullptr)
		return nullptr;

	((AiController*)newShip->GetController())->SetRandomWaypoints(&ai_waypoints);



	return newShip;
}

void GameplayScene::UpdateHUD(float dt) {
	static float timerForKnot = 0;
	bool	gamePadconnected = GameSetting::GetRef().GamePadIsConnected();
	PlayerShip* player = static_cast<PlayerShip*>(playerShip);

	std::string GUI_EXT,
		GUI_CANCEL;

	if (gamePadconnected)
	{
		GUI_EXT = "_xbox";
		GUI_CANCEL = "_keyboard";
	}
	else
	{
		GUI_EXT = "_keyboard";
		GUI_CANCEL = "_xbox";
	}

	m_hudElems["FireSkillIcon-1" + GUI_CANCEL]->Active(false);
	m_hudElems["FireSkillIcon-2" + GUI_CANCEL]->Active(false);
	m_hudElems["FireSkillIcon-3" + GUI_CANCEL]->Active(false);
	m_hudElems["Cannonball" + GUI_CANCEL]->Active(false);
	m_hudElems["ChainBall" + GUI_CANCEL]->Active(false);
	m_hudElems["Spyglass" + GUI_CANCEL]->Active(false);

	m_hudElems["FireSkillIcon-1" + GUI_EXT]->Active(true);
	m_hudElems["FireSkillIcon-2" + GUI_EXT]->Active(true);
	m_hudElems["FireSkillIcon-3" + GUI_EXT]->Active(true);
	m_hudElems["Cannonball" + GUI_EXT]->Active(true);
	m_hudElems["ChainBall" + GUI_EXT]->Active(true);
	m_hudElems["Spyglass" + GUI_EXT]->Active(true);

	int maxHullHP = playerShip->GetMaxHealth();
	int curHullHP = playerShip->GetHealth();

	int maxMastHP = (int)playerShip->GetMaxSpeed();
	int curMastHP = (int)playerShip->GetCurrentMaxSpeed();

	int maxSideCanHP = playerShip->GetSideCannonHandler()->GetMaxHealth();
	int curSideCanHP = playerShip->GetSideCannonHandler()->GetHealth();

	int maxCrew = playerShip->GetMaxCrew();
	int curCrew = playerShip->GetCurrCrew();

	int maxFood = playerShip->GetMaxWaterFood();
	int curFood = playerShip->GetCurrWaterFood();

	int maxGold = playerShip->GetMaxGold();
	int curGold = playerShip->GetCurrGold();

	int maxRum = playerShip->GetMaxRum();
	int curRum = playerShip->GetCurrRum();

	float maxRep = player->GetMaxReputation();
	float curRep = player->GetCurrReputation();

	int currAmmo = playerShip->GetSideCannonHandler()->GetCurrentAmmoIndex();
	auto ammoSelector = reinterpret_cast<G2D::GUITexture*>(m_hudElems["AmmoTypeSelector"]);
	switch (currAmmo)
	{
	case 0:
		ammoSelector->Position(
		{
			ammoSelector->StaticPostions()["Cannonball"].x,
			ammoSelector->StaticPostions()["Cannonball"].y
		});
		break;
	case 1:
		ammoSelector->Position(
		{
			ammoSelector->StaticPostions()["Cannonball"].x + ammoSelector->Offsets()["Chainshot"].x,
			ammoSelector->StaticPostions()["Cannonball"].y + ammoSelector->Offsets()["Chainshot"].y
		});
		break;
	case 2:
		ammoSelector->Position(
		{
			ammoSelector->StaticPostions()["Cannonball"].x + ammoSelector->Offsets()["Grapeshot"].x,
			ammoSelector->StaticPostions()["Cannonball"].y + ammoSelector->Offsets()["Grapeshot"].y
		});
		break;
	default:
		break;
	}

	// speed
	auto SpeedText = static_cast<G2D::GUIText*>(m_hudElems["SpeedIconText"]);
	float	curSpeed = playerShip->GetSpeed();
	SpeedText->Text(std::to_wstring(int(std::ceilf(curSpeed * 2))) + L" Knots");	// 1 knot (nautical mile) = 6000 ft.

	timerForKnot += dt;
	if (timerForKnot >= 1.0f) {
		GameStatistics::GetRef().TotalKnot(std::ceilf(curSpeed * 2));
		timerForKnot = 0.0f;
	}

	m_hudElems["SailsSpeedDamage"]->SpriteData().customFloat0 = (playerShip->GetCurrentMaxSpeed() / playerShip->GetMaxSpeed());
	if (0.0f <= m_fHudSpeedTimer)
	{
		m_fHudSpeedTimer -= dt;
		auto speedIcon = static_cast<G2D::GUISlider*>(m_hudElems["SailSpeed"]);
		if (3.0f <= m_fHudSpeedTimer)
		{
			speedIcon->FrontColor(SimpleMath::Color(1.0f, 1.0f, 1.0f, 1.0f));
			speedIcon->BackColor(SimpleMath::Color(1.0f, 1.0f, 1.0f, 1.0f));
			SpeedText->Color(SimpleMath::Color(0.0f, 0.0f, 0.0f, 1.0f));
			float	maxSpeed = playerShip->GetMaxSpeed(),
				speedRatio = curSpeed / maxSpeed;
			speedIcon->Value(speedRatio);
		}
		else
		{
			speedIcon->FrontColor(SimpleMath::Color(1.0f, 1.0f, 1.0f, m_fHudSpeedTimer / 3.0f));
			speedIcon->BackColor(SimpleMath::Color(1.0f, 1.0f, 1.0f, m_fHudSpeedTimer / 3.0f));
			SpeedText->Color(SimpleMath::Color(0.0f, 0.0f, 0.0f, m_fHudSpeedTimer / 3.0f));
		}
	}

	// hull
	auto hullHP = static_cast<G2D::GUITexture*>(m_hudElems["Ship"]);
	float hullHpRatio = float(curHullHP) / float(maxHullHP);

	if (curHullHP < 6)
	{
		m_fHpLowTimer += dt;
		if (m_bHpLowOnBlack)
			hullHP->Color(SimpleMath::Color::Lerp({ 1.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, m_fHpLowTimer / HP_LOW_TIMER_MAX));
		else
			hullHP->Color(SimpleMath::Color::Lerp({ 0.0f, 0.0f, 0.0f }, { 1.0f, 0.0f, 0.0f }, m_fHpLowTimer / HP_LOW_TIMER_MAX));

		if (HP_LOW_TIMER_MAX < m_fHpLowTimer)
		{
			m_fHpLowTimer = 0.0f;
			m_bHpLowOnBlack = !m_bHpLowOnBlack;
		}
	}
	else
		hullHP->Color(SimpleMath::Color::Lerp({ 1.0f, 0.0f, 0.0f }, { 0.0f, 1.0f, 0.0f }, hullHpRatio));

	// sail
	auto sailHP = static_cast<G2D::GUITexture*>(m_hudElems["Sails"]);
	float sailHPRatio = float(curMastHP) / float(maxMastHP);

	sailHP->Color(SimpleMath::Color::Lerp({ 1.0f, 0.0f, 0.0f }, { 0.0f, 1.0f, 0.0f }, sailHPRatio));

	// cannons
	auto cannonHP = static_cast<G2D::GUITexture*>(m_hudElems["Cannons"]);
	float cannonHPRatio = float(curSideCanHP) / float(maxSideCanHP);

	cannonHP->Color(SimpleMath::Color::Lerp({ 1.0f, 0.0f, 0.0f }, { 0.0f, 1.0f, 0.0f }, cannonHPRatio));

	// crappy reload timers Chen he is writing other ones later
	float currentReloadTime = playerShip->GetSideCannonHandler()->GetReloadTime(Cannon::WeaponType::LONG_NINE);
	float maxReloadTime = playerShip->GetSideCannonHandler()->GetLongNine()->GetMaxReload();
	float ratio = currentReloadTime / maxReloadTime;
	static bool reloadComplete = false;
	static bool reloadStarted = false;
	if (ratio != 0)
		reloadStarted = true;
	if (ratio != 0 && reloadStarted == true)
		reloadComplete = false;
	else
		reloadComplete = true;

	if (reloadComplete == false || reloadStarted == false) {
		static_cast<G2D::GUITexture*>(m_hudElems["FireSkillIcon-1" + GUI_EXT])->Color(SimpleMath::Color::Lerp({ 1.0f, 1.0f, 1.0f }, { 0.0f, 0.0f, 0.0f }, ratio));
		if (reloadStarted) {
			static_cast<G2D::GUITexture*>(m_hudElems["Cooldown-1"])->Active(true);
			static_cast<G2D::GUITexture*>(m_hudElems["Cooldown-1"])->SpriteData().customFloat0 = ratio;
			static_cast<G2D::GUITexture*>(m_hudElems["Cooldown-1"])->SpriteData().customFloat1 = 1.0f;
		}
	}
	else
	{
		static_cast<G2D::GUITexture*>(m_hudElems["FireSkillIcon-1" + GUI_EXT])->Color({ 0.0f, 1.0f, 0.0f });
		static float aliveTime = 0;
		aliveTime += dt;
		if (aliveTime > .05f)
		{
			reloadStarted = false;
			static_cast<G2D::GUITexture*>(m_hudElems["Cooldown-1"])->Active(false);
			aliveTime = 0;
		}
	}

	float currentLeftReloadTime = playerShip->GetSideCannonHandler()->GetCannons()->GetLeftReloadTime();
	maxReloadTime = playerShip->GetSideCannonHandler()->GetCannons()->GetMaxReload();
	ratio = currentLeftReloadTime / maxReloadTime;
	static bool reloadCompleteL = false;
	static bool reloadStartedL = false;
	if (ratio != 0)
		reloadStartedL = true;
	if (ratio != 0 && reloadStartedL == true)
		reloadCompleteL = false;
	else
		reloadCompleteL = true;

	static_cast<G2D::GUITexture*>(m_hudElems["Cooldown-2"])->Active(false);
	if (reloadCompleteL == false || reloadStartedL == false)
	{
		static_cast<G2D::GUITexture*>(m_hudElems["FireSkillIcon-2" + GUI_EXT])->Color(SimpleMath::Color::Lerp({ 1.0f, 1.0f, 1.0f }, { 0.0f, 0.0f, 0.0f }, ratio));
		if (reloadStartedL)
		{
			static_cast<G2D::GUITexture*>(m_hudElems["Cooldown-2"])->Active(true);
			static_cast<G2D::GUITexture*>(m_hudElems["Cooldown-2"])->SpriteData().customFloat0 = ratio;
			static_cast<G2D::GUITexture*>(m_hudElems["Cooldown-2"])->SpriteData().customFloat1 = 1.0f;
		}
	}
	else
	{
		static_cast<G2D::GUITexture*>(m_hudElems["FireSkillIcon-2" + GUI_EXT])->Color({ 0.0f, 1.0f, 0.0f });
		static float aliveTime = 0;
		aliveTime += dt;
		if (aliveTime > .05f)
		{
			reloadStartedL = false;
			static_cast<G2D::GUITexture*>(m_hudElems["Cooldown-2"])->Active(false);
			aliveTime = 0;
		}
	}

	float currentRightReloadTime = playerShip->GetSideCannonHandler()->GetCannons()->GetRightReloadTime();
	maxReloadTime = playerShip->GetSideCannonHandler()->GetCannons()->GetMaxReload();
	ratio = currentRightReloadTime / maxReloadTime;
	static bool reloadCompleteR = false;
	static bool reloadStartedR = false;
	if (ratio != 0)
		reloadStartedR = true;
	if (ratio != 0 && reloadStartedR == true)
		reloadCompleteR = false;
	else
		reloadCompleteR = true;

	static_cast<G2D::GUITexture*>(m_hudElems["Cooldown-3"])->Active(false);
	if (reloadCompleteR == false || reloadStartedR == false)
	{
		static_cast<G2D::GUITexture*>(m_hudElems["FireSkillIcon-3" + GUI_EXT])->Color(SimpleMath::Color::Lerp({ 1.0f, 1.0f, 1.0f }, { 0.0f, 0.0f, 0.0f }, ratio));
		if (reloadStartedR)
		{
			static_cast<G2D::GUITexture*>(m_hudElems["Cooldown-3"])->Active(true);
			static_cast<G2D::GUITexture*>(m_hudElems["Cooldown-3"])->SpriteData().customFloat0 = ratio;
			static_cast<G2D::GUITexture*>(m_hudElems["Cooldown-3"])->SpriteData().customFloat1 = 1.0f;
		}
	}
	else
	{
		static_cast<G2D::GUITexture*>(m_hudElems["FireSkillIcon-3" + GUI_EXT])->Color({ 0.0f, 1.0f, 0.0f });
		static float aliveTime = 0;
		aliveTime += dt;
		if (aliveTime > .05f)
		{
			reloadStartedR = false;
			static_cast<G2D::GUITexture*>(m_hudElems["Cooldown-3"])->Active(false);
			aliveTime = 0;
		}
	}

	// name & bounty
	auto repTitle = static_cast<G2D::GUIText*>(m_hudElems["ReputationTitle"]);
	auto repBounty = static_cast<G2D::GUIText*>(m_hudElems["ReputationBounty"]);
	float repRatio = float(curRep) / float(maxRep);
	static_cast<G2D::GUISlider*>(m_hudElems["Reputation"])->Value(curRep / maxRep);
	static_cast<G2D::GUISlider*>(m_hudElems["Reputation"])->BackColor(SimpleMath::Color::Lerp({ 0.239f,0.741f, 0.929f }, { 1.0f, 0.0f, 0.0f }, repRatio));

	if (0.1f > repRatio)
		repTitle->Text(L"Nobody");
	else if (0.2f > repRatio)
		repTitle->Text(L"Sea Dog");
	else if (0.3f > repRatio)
		repTitle->Text(L"Freebooter");
	else if (0.4f > repRatio)
		repTitle->Text(L"Sailor");
	else if (0.5f > repRatio)
		repTitle->Text(L"Buccaneer");
	else if (0.6f > repRatio)
		repTitle->Text(L"Privateer");
	else if (0.7f > repRatio)
		repTitle->Text(L"Raider");
	else if (0.8f > repRatio)
		repTitle->Text(L"Infamous Pirate");
	else if (0.9f > repRatio)
	{
		repTitle->Text(L"Robin Hood of the Sea");
		repTitle->Scale(0.7f);
	}
	else if (1.0f > repRatio)
		repTitle->Text(L"Prince of Pirates");
	else
		repTitle->Text(L"King of the Pirates");

	repBounty->Text(L"$" + std::to_wstring(int(std::ceilf(repRatio * MAX_BOUNTY))));



	//MinimapShip
	auto minimapShip = static_cast<G2D::GUITexture*>(m_hudElems["MinimapShip"]);
	auto miniShipOffset = (256.0f / 600.0f) * (((CGame*)CGame::GetApplication())->m_window->GetWindowWidth() / 1920.0f);
	minimapShip->Position(
	{
		minimapShip->StaticPostions()["Initial"].x + playerShip->GetPosition().z * miniShipOffset,
		minimapShip->StaticPostions()["Initial"].y + playerShip->GetPosition().x * miniShipOffset
	});
	XMFLOAT3 &heading = forwardAxisOf(playerShip->GetWorldMatrix());
	minimapShip->Rotation(atan2f(heading.x, heading.z));


	//MinimapBoss
	auto minimapBoss = static_cast<G2D::GUITexture*>(m_hudElems["MinimapBoss"]);
	if (m_bShowBossMinimap == true)
	{
		auto miniBossOffset = (256.0f / 600.0f) * (((CGame*)CGame::GetApplication())->m_window->GetWindowWidth() / 1920.0f);
		minimapBoss->Position(
		{
			minimapBoss->StaticPostions()["Initial"].x + BossTeam.fleet[0]->GetPosition().z * miniBossOffset,
			minimapBoss->StaticPostions()["Initial"].y + BossTeam.fleet[0]->GetPosition().x * miniBossOffset
		});

		XMFLOAT3 &bossheading = forwardAxisOf(BossTeam.fleet[0]->GetWorldMatrix());
		minimapBoss->Rotation(atan2f(bossheading.x, bossheading.z));
	}
	else {
		minimapBoss->Active(false);
	}


	//MinimapRecon
	auto minimapRecon = static_cast<G2D::GUITexture*>(m_hudElems["MinimapRecon"]);
	if (reconTeam.squad != nullptr)
	{
		// get all active recon ships
		std::vector<Ship*> goodReconShips;
		size_t reconsize = reconTeam.fleet.size();
		for (size_t i = 0; i < reconsize; i++)
		{
			// null check
			if (reconTeam.fleet[i] == nullptr)
				continue;

			// to delete check
			if (reconTeam.fleet[i]->ToBeDeleted() == true)
				continue;

			// HP check
			if (reconTeam.fleet[i]->GetHealth() <= 0)
				continue;

			// good recon ship
			goodReconShips.push_back(reconTeam.fleet[i]);
		}


		// enough good recon ships to draw icon
		size_t recongoodsize = goodReconShips.size();
		if (recongoodsize > 0)
		{
			SimpleMath::Vector2	iconpos = SimpleMath::Vector2(0.0f, 0.0f);
			auto				miniReconOffset = (256.0f / 600.0f) * (((CGame*)CGame::GetApplication())->m_window->GetWindowWidth() / 1920.0f);

			// average pos
			XMFLOAT3 averagepos = XMFLOAT3(0.0f, 0.0f, 0.0f);
			for (size_t i = 0; i < recongoodsize; i++)
				averagepos += goodReconShips[i]->GetPosition();
			averagepos /= static_cast<float>(recongoodsize);

			// icon pos
			iconpos.x = minimapRecon->StaticPostions()["Initial"].x + averagepos.z * miniReconOffset;
			iconpos.y = minimapRecon->StaticPostions()["Initial"].y + averagepos.x * miniReconOffset;
			minimapRecon->Position(iconpos);

			// icon scale
			float reconscale = minimapRecon->Scale();
			if (reconscale < 3.6f && recongoodsize == 4)
			{
				reconscale += 0.12f;
				if (reconscale > 3.6f)
					reconscale = 3.6f;
				minimapRecon->Scale(reconscale);
			}

			// gui 
			minimapRecon->Active(true);
		}
		else
		{
			minimapRecon->Active(false);
		}
		goodReconShips.clear();
	}
	else
	{
		minimapRecon->Active(false);
	}


	//MinimapCargo
	if (m_cargoSquad && m_cargoSquad->GetCargoShip() && !m_cargoSquad->GetCargoShip()->ToBeDeleted())
	{
		auto minimapCargo = static_cast<G2D::GUITexture*>(m_hudElems["MinimapCargo"]);
		auto miniCargoOffset = (256.0f / 600.0f) * (((CGame*)CGame::GetApplication())->m_window->GetWindowWidth() / 1920.0f);
		float2 cargoWorldPos = { m_cargoSquad->GetCargoShip()->GetPosition().z,m_cargoSquad->GetCargoShip()->GetPosition().x };
		if (cargoWorldPos.x > BOUNDARY)
		{
			cargoWorldPos.x = BOUNDARY;
		}
		if (cargoWorldPos.x < -BOUNDARY)
		{
			cargoWorldPos.x = -BOUNDARY;
		}

		if (cargoWorldPos.y > BOUNDARY)
		{
			cargoWorldPos.y = BOUNDARY;
		}
		if (cargoWorldPos.y < -BOUNDARY)
		{
			cargoWorldPos.y = -BOUNDARY;
		}
		minimapCargo->Position(
		{
			minimapCargo->StaticPostions()["Initial"].x + cargoWorldPos.x * miniCargoOffset,
			minimapCargo->StaticPostions()["Initial"].y + cargoWorldPos.y * miniCargoOffset
		});

		XMFLOAT3 &cargoheading = forwardAxisOf(m_cargoSquad->GetCargoShip()->GetWorldMatrix());
		minimapCargo->Rotation(atan2f(cargoheading.x, cargoheading.z));
	}
	else
	{
		m_hudElems["MinimapCargo"]->Active(false);
	}



	if (player->BufferGold() > 0) {
		static_cast<G2D::GUIText*>(m_hudElems["GoldStatus"])->Color({ 0,1,0,1 });
		static_cast<G2D::GUIText*>(m_hudElems["GoldStatus"])->Scale(1.25f);
	}
	else {
		static_cast<G2D::GUIText*>(m_hudElems["GoldStatus"])->Color({ 0,0,0,1 });
		static_cast<G2D::GUIText*>(m_hudElems["GoldStatus"])->Scale(1.0f);
	}

	if (player->BufferWaterFood() > 0) {
		static_cast<G2D::GUIText*>(m_hudElems["FoodStatus"])->Color({ 0,1,0,1 });
		static_cast<G2D::GUIText*>(m_hudElems["FoodStatus"])->Scale(1.25f);
	}
	else {
		if (player->DecreasingRes()) {
			static_cast<G2D::GUIText*>(m_hudElems["FoodStatus"])->Color({ 1,0,0,1 });
			static_cast<G2D::GUIText*>(m_hudElems["FoodStatus"])->Scale(1.25f);
		}
		else {
			static_cast<G2D::GUIText*>(m_hudElems["FoodStatus"])->Color({ 0,0,0,1 });
		}
		static_cast<G2D::GUIText*>(m_hudElems["FoodStatus"])->Scale(1.0f);
	}

	if (player->BufferRum() > 0) {
		static_cast<G2D::GUIText*>(m_hudElems["RumStatus"])->Color({ 0,1,0,1 });
		static_cast<G2D::GUIText*>(m_hudElems["RumStatus"])->Scale(1.25f);
	}
	else {
		if (player->DecreasingRes()) {
			static_cast<G2D::GUIText*>(m_hudElems["RumStatus"])->Color({ 1,0,0,1 });
			static_cast<G2D::GUIText*>(m_hudElems["RumStatus"])->Scale(1.25f);
		}
		else {
			static_cast<G2D::GUIText*>(m_hudElems["RumStatus"])->Color({ 0,0,0,1 });
		}
		static_cast<G2D::GUIText*>(m_hudElems["RumStatus"])->Scale(1.0f);
	}

	if (m_bUpdateResourcesHud)
		UpdateHudResources(dt);

	static_cast<G2D::GUIText*>(m_hudElems["GoldStatus"])->Text(std::to_wstring(curGold) + L"g");
	static_cast<G2D::GUIText*>(m_hudElems["RumStatus"])->Text(std::to_wstring(curRum) + L"/" + std::to_wstring(maxRum));
	static_cast<G2D::GUIText*>(m_hudElems["CrewStatus"])->Text(std::to_wstring(curCrew) + L"/" + std::to_wstring(maxCrew));
	static_cast<G2D::GUIText*>(m_hudElems["FoodStatus"])->Text(std::to_wstring(curFood) + L"/" + std::to_wstring(maxFood));

	// low resources icons
	ToggleLowResourcesGUI();
	UpdateLowResourcesGUI(dt);

	// port interaction icons
	UpdatePortInstructions();

	// port sale timer
	if (m_fPortSaleTimer > 0.0f)
	{
		m_fPortSaleTimer -= dt;

		if (m_fPortSaleTimer <= 0.0f)
			m_fPortSaleTimer = 0.0f;
	}

	// convert [seconds] to [minutes:seconds]
	int minutes = int(std::floor(m_fPortSaleTimer * 0.0167f));	//(1/60) == (0.01666666666666666666666666666667) == approximately (0.0167f)
	int seconds = int(m_fPortSaleTimer) - (minutes * 60);
	if (seconds < 0)
		seconds = 0;	// cancel out float precision

	// text = [minutes:seconds]
	std::wstring timetext = std::to_wstring(minutes) + L":";
	if (seconds < 10)
		timetext += L"0";
	timetext += std::to_wstring(seconds);
	static_cast<G2D::GUIText*>(m_hudElems["PortSaleTime"])->Text(L"Time until sales refresh: " + timetext);



	// decrement
	decTimer -= dt;
	// turn off gui
	if (decTimer <= 0.0f)
	{
		decTimer = 0.0f;
		std::string guis[4] = { "HUD_RumDec", "RumDec", "FoodDec", "HUD_FoodDec" };

		for (size_t i = 0; i < 4; i++)
		{
			if (m_hudElems[guis[i]]->Active() == false)
				continue;
			m_hudElems[guis[i]]->Active(false);
		}
	}




	// interact prompt
	if (PromptTimer > 0.0f)
	{
		PromptTimer -= dt * 3.0f;

		G2D::GUITexture*	hud_gui		= nullptr;
		G2D::GUITexture*	other_gui	= nullptr;
		SimpleMath::Color	startcolor	= SimpleMath::Color(1.0f, 1.0f, 1.0f, 1.0f);
		SimpleMath::Color	endcolor	= SimpleMath::Color(1.0f, 1.0f, 1.0f, 0.0f);

		if (GameSetting::GetRef().GamePadIsConnected() == true)
		{
			hud_gui = static_cast<G2D::GUITexture*>(m_hudElems["BoardButtonXbox"]);
			other_gui = static_cast<G2D::GUITexture*>(m_hudElems["BoardButtonKeyboard"]);
		}
		else
		{
			hud_gui = static_cast<G2D::GUITexture*>(m_hudElems["BoardButtonKeyboard"]);
			other_gui = static_cast<G2D::GUITexture*>(m_hudElems["BoardButtonXbox"]);
		}


		// activate/deactivate
		hud_gui->Active(true);
		other_gui->Active(false);

		// color
		SimpleMath::Color lerped_color = SimpleMath::Color::Lerp(endcolor, startcolor, PromptTimer/3.0f);
		hud_gui->Color(lerped_color);
		other_gui->Color(startcolor);


		if (PromptTimer <= 0.0f)
		{
			PromptTimer = 0.0f;

			hud_gui->Active(false);
			hud_gui->Color(startcolor);

			other_gui->Active(false);
			other_gui->Color(startcolor);

		}
	}
}

bool GameplayScene::CheckGoToPauseScene(void)
{
	if (gotopause == false)
		return false;

	// reset bool
	gotopause = false;

	// stop particles
	//for (auto i = 0; i < islands.size(); i++)
	//	islands[i]->StopParticleSet();

	// pause bgm
	GamePad::Get().SetVibration(0, 0, 0);
	PauseSounds();

	// go to pause scene
	sceneManager->PushScene(SceneType::PAUSE);
	return true;
}

bool GameplayScene::CheckGoToPortScene(void)
{
	if (gotoport == false)
		return false;

	// reset bool
	gotoport = false;

	// stop particles
	for (auto i = 0; i < islands.size(); i++)
		islands[i]->StopParticleSet();

	// pause bgm
	GamePad::Get().SetVibration(0, 0, 0);
	PauseSounds();

	// go to port sound
	SoundManager* sound = ((CGame*)CGame::GetApplication())->GetSoundManager();
	sound->Play3DSound(AK::EVENTS::PLAYPORTBELLS);

	// go to port scene
	sceneManager->PushScene(SceneType::PORT);
	return true;
}

void GameplayScene::ToggleHUDInstructions(bool toggle)
{
	return;
	if (toggle == true)
		hud_help = !hud_help;



	// turn off ALL help
	m_hudElems["PPP"]->Active(false);


	// turn off?
	if (hud_help == false)
		return;


	// turn on help
	bool	connected = GameSetting::GetRef().GamePadIsConnected();


	// keyboard
	if (connected == false)
	{
		m_hudElems["PPP"]->Active(true);
		return;
	}


	// controller
	m_hudElems["PPP"]->Active(true);
}

void GameplayScene::PauseSounds(void)
{
	SoundManager* sound = ((CGame*)CGame::GetApplication())->GetSoundManager();

	// gameplay bgm
	sound->Play3DSound(AK::EVENTS::PAUSEBGM);

	// boarding sfx
	if (playerShip->GetBoardingStage() > Ship::eBoardingState::NONE)
		sound->Play3DSound(AK::EVENTS::PAUSEBOARDINGSOUNDS);
}

void GameplayScene::ResumeSounds(void)
{
	SoundManager* sound = ((CGame*)CGame::GetApplication())->GetSoundManager();

	// gameplay bgm
	sound->Play3DSound(AK::EVENTS::RESUMEBGM);

	// boarding sfx
	if (playerShip->GetBoardingStage() > Ship::eBoardingState::NONE)
		sound->Play3DSound(AK::EVENTS::RESUMEBOARDINGSOUNDS);
}

void GameplayScene::StopSounds(void)
{
	SoundManager* sound = ((CGame*)CGame::GetApplication())->GetSoundManager();

	// gameplay bgm
	sound->Play3DSound(AK::EVENTS::STOPBGM);

	// boarding sfx
	if (playerShip->GetBoardingStage() > Ship::eBoardingState::NONE)
		sound->Play3DSound(AK::EVENTS::STOPBOARDINGSOUNDS);
}

void GameplayScene::ActivateResourceDecrement(int rum, int food)
{
	if (rum > 0 && food > 0)
		return;


	decTimer = 3.0f;
	CGame*			game = (CGame*)CGame::GetApplication();
	std::wstring	front = L"                ";
	XMFLOAT2		screencenter = XMFLOAT2(0.0f, 0.0f);
	screencenter.x = static_cast<float>(game->m_window->GetWindowWidth()) * 0.5f;
	screencenter.y = static_cast<float>(game->m_window->GetWindowHeight()) * 0.5f;
	XMFLOAT2		pos = screencenter;
	pos.y += 10.0f;


	// rum dec
	if (rum != 0)
	{
		pos.x -= 40.0f;
		pos.y -= 40.0f;

		// setup gui text
		std::wstringstream wss;
		wss << rum;

		m_hudElems["RumDec"]->Active(true);
		m_hudElems["RumDec"]->Position(pos);
		static_cast<G2D::GUIText*>(m_hudElems["RumDec"])->Text(front + wss.str());
		static_cast<G2D::GUIText*>(m_hudElems["RumDec"])->Scale(1.0f);


		// setup gui texture
		m_hudElems["HUD_RumDec"]->Active(true);
		m_hudElems["HUD_RumDec"]->Position(pos);
	}


	// food dec
	if (food != 0)
	{
		//pos.x -= 40.0f;
		pos.y -= 40.0f;

		// setup gui text
		std::wstringstream wss;
		wss << food;

		m_hudElems["FoodDec"]->Active(true);
		m_hudElems["FoodDec"]->Position(pos);
		static_cast<G2D::GUIText*>(m_hudElems["FoodDec"])->Text(front + wss.str());
		static_cast<G2D::GUIText*>(m_hudElems["FoodDec"])->Scale(1.0f);


		// setup gui texture
		m_hudElems["HUD_FoodDec"]->Active(true);
		m_hudElems["HUD_FoodDec"]->Position(pos);
	}

}

void GameplayScene::TogglePortInstructions(bool toggle)
{
	// already on/off
	//if (port_help == toggle)
	//	return;

	// toggle on/off
	port_help = toggle;


	bool	connected = GameSetting::GetRef().GamePadIsConnected();


	// turn off
	m_hudElems["PortButtonKeyboard"]->Active(false);
	m_hudElems["PortButtonXbox"]->Active(false);

	if (port_help == false)
		return;

	// turn on
	if (connected == true)
	{
		m_hudElems["PortButtonXbox"]->Active(true);
	}
	else
	{
		m_hudElems["PortButtonKeyboard"]->Active(true);
	}
}

void GameplayScene::UpdatePortInstructions(void)
{
	// don't update?
	if (port_help == false)
	{
		TogglePortInstructions(false);
		return;
	}


	//TogglePortInstructions(true);

	auto    game = (CGame*)CGame::GetApplication();
	int		status = game->GetGamePadConnectionStatus();

	switch (status)
	{
		// nothing
	case 0:
		break;

		// reconnected
	case 1:
		m_hudElems["PortButtonKeyboard"]->Active(false);
		m_hudElems["PortButtonXbox"]->Active(true);
		break;

		// disconnected
	case -1:
		m_hudElems["PortButtonKeyboard"]->Active(true);
		m_hudElems["PortButtonXbox"]->Active(false);
		break;

	default:
		break;
	}


}

void GameplayScene::TriggerPortCollision(void)
{
	port_help = true;
	TogglePortInstructions(true);
}

void GameplayScene::ToggleLowResourcesGUI(void)
{
	// don't need to continue
	if (playerShip->GetCurrRum() > 0.0f && playerShip->GetCurrWaterFood() > 0.0f)
	{
		low_timer = 0.0f;
		// turn off
		m_hudElems["LowFoodIcon"]->Active(false);
		m_hudElems["LowRumIcon"]->Active(false);
		return;
	}


	if (low_timer < 0.0f)
		return;


	// rum
	if (playerShip->GetCurrRum() <= 0.0f && m_hudElems["LowRumIcon"]->Active() == false)
	{
		low_timer = 4.0f;
		// turn on
		m_hudElems["LowRumIcon"]->Active(true);
	}


	// food
	if (playerShip->GetCurrWaterFood() <= 0.0f && m_hudElems["LowFoodIcon"]->Active() == false)
	{
		low_timer = 4.0f;
		// turn on
		m_hudElems["LowFoodIcon"]->Active(true);
	}



}

void GameplayScene::UpdateLowResourcesGUI(float dt)
{
	if (low_timer == 0.0f)
		return;


	// turning off
	if (low_timer > 0.0f)
	{
		low_timer -= dt;

		if (low_timer <= 0.0f)
		{
			low_timer = -8.0f;
			m_hudElems["LowFoodIcon"]->Active(false);
			m_hudElems["LowRumIcon"]->Active(false);
			return;
		}
	}



	// turning on
	if (low_timer < 0.0f)
	{
		low_timer += dt;

		if (low_timer >= 0.0f)
		{

			if (playerShip->GetCurrWaterFood() <= 0.0f)
			{
				low_timer = 4.0f;
				m_hudElems["LowFoodIcon"]->Active(true);
			}
			if (playerShip->GetCurrRum() <= 0.0f)
			{
				low_timer = 4.0f;
				m_hudElems["LowRumIcon"]->Active(true);
			}
		}
	}



}

void GameplayScene::AddBossShipAndSquad(void)
{
	if (BossTeam.fleet.size() > 0)
		return;


	// souns stuff
	SoundManager* sound = ((CGame*)CGame::GetApplication())->GetSoundManager();
	sound->StopSounds();
	sound->Play3DSound(AK::EVENTS::PLAYBGM);
	sound->PostEvent(AK::EVENTS::PLAY_BOSSPLAYLIST);


	// create a squad with the ships
	BossTeam.squad = newx Squad();

	// Load & add boss ship: [0]
	BossTeam.fleet.push_back(LoadBossShip());
	BossTeam.squad->AddSquadmate(BossTeam.fleet[0]->GetController()->GetBoid());
	//BossTeam.squad->SetLeader(BossTeam.fleet[0]->GetController()->GetBoid());
	//XMFLOAT3 bossPos = { 0.0f, 0.0f, 310.0f };
	//XMFLOAT3 bossForward = playerShip->GetPosition() - bossPos;
	//normalizeFloat3(bossForward);
	//BossTeam.fleet[0]->GetTransform().GetWorldMatrix()._31 = bossForward.x;
	//BossTeam.fleet[0]->GetTransform().GetWorldMatrix()._32 = bossForward.y;
	//BossTeam.fleet[0]->GetTransform().GetWorldMatrix()._33 = bossForward.z;
	//BossTeam.fleet[0]->SetCurrHealth(10);


	// boss position
	XMFLOAT3 bosspos = BossTeam.fleet[0]->GetPosition();


	// Add 1st 3 boss fleet ships
	size_t numships = 1;
	for (size_t i = 0; i < numships; i++)
	{
		DirectX::XMFLOAT4X4	mat = DirectX::XMFLOAT4X4(1.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f,
			(float)i*2.0f, bosspos.y, bosspos.z + 1.4f, 1.0f);

		Ship* fleetMember = CreateAIShip("FleetMember", XMFLOAT3(mat._41, mat._42, mat._43));
		BossTeam.fleet.push_back(fleetMember);
		BossTeam.squad->AddSquadmate(fleetMember->GetController()->GetBoid());
	}

	// Add 2nd 3 boss fleet ships
	for (size_t i = 0; i < numships; i++)
	{
		DirectX::XMFLOAT4X4	mat = DirectX::XMFLOAT4X4(1.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f,
			-(float)i*2.0f, bosspos.y, bosspos.z + 1.4f, 1.0f);

		Ship* fleetMember = CreateAIShip("FleetMember", XMFLOAT3(mat._41, mat._42, mat._43));
		BossTeam.fleet.push_back(fleetMember);
		BossTeam.squad->AddSquadmate(fleetMember->GetController()->GetBoid());
	}


	size_t fleetsize = BossTeam.fleet.size();
	for (size_t i = 0; i < fleetsize; i++)
	{
		AiController* ai_ctrlr = static_cast<AiController*>(BossTeam.fleet[i]->GetController());
		ai_ctrlr->ClearWaypoints();
		//ai_ctrlr->GetBoid()->SetSquadCohesion(0.2f);
		ai_ctrlr->GetBoid()->SetSquadSeperation(0.01f);
	}



	// Configure squad waypoints
	//BossSquad->AddWaypoint(XMFLOAT3{ 0.0f,0.0f,0.0f });

	// fill out waypoint ordering
	//std::vector<XMFLOAT3> waypoints;
	//SortWorldWaypoints('E', waypoints);
	//SortBossWaypoints(waypoints);

	// add waypoints
	/*
	size_t wpsize = waypoints.size();
	for (size_t i = 0; i < wpsize; i++)
		BossTeam.squad->AddWaypoint(waypoints[i]);
	*/
	/*
	size_t wpsize = ai_waypoints.size();
	for (size_t i = 0; i < wpsize; i++)
		BossTeam.squad->AddWaypoint(ai_waypoints[i]);
	*/
	XMFLOAT3 waypoint = XMFLOAT3(-2.16307f, -0.1f, 242.48f);	//(-2.16307f, -0.1f, 242.48f); // XMFLOAT3(BossTeam.fleet[0]->GetPosition().x, BossTeam.fleet[0]->GetPosition().y, BossTeam.fleet[0]->GetPosition().z - 50.0f);
	BossTeam.squad->AddWaypoint(waypoint);
	//Player pos : { -2.05607f, -0.1f, 242.56f }
	//Player pos : { -2.08942f, -0.1f, 242.535f }
	//Player pos : { -2.1077f, -0.1f, 242.522f }
	//Player pos : { -2.13591f, -0.1f, 242.501f }
	//Player pos : { -2.16307f, -0.1f, 242.48f }



	// Boss HUD stuff
	m_bShowBossMinimap = true;
	m_hudElems["Boss"]->Active(true);
	m_hudElems["Boss2"]->Active(true);
	m_hudElems["Boss3"]->Active(false);
	m_hudElems["Boss4"]->Active(false);
	m_hudElems["MinimapBoss"]->Active(true);
	m_fBossTextTimer = 8.0f;
}

void GameplayScene::GetWorldWaypoints(std::vector<XMFLOAT3>& out_ports, std::vector<XMFLOAT3>& out_triangles, std::vector<XMFLOAT3>& out_other)
{
	// Port waypoints
	XMFLOAT3 wp_ports[8] =
	{
		// new map
		XMFLOAT3(221.819f,	-0.1f,	-214.409f),	// Port: Isle De Cervantes
		XMFLOAT3(125.067f,	-0.1f,	-157.578f),	// Port: Saltier Spitoon
		XMFLOAT3(-15.0859f,	-0.1f,	-208.848f),	// Port: Dark Beard's Enclave
		XMFLOAT3(-223.237f,	-0.1f,	-117.864f),	// Port: Port Royalty
		XMFLOAT3(167.453f,	-0.1f,	-77.0619f),	// Port: One Piece Cove
		XMFLOAT3(159.272f,	-0.1f,	52.0826f),	// Port: Dead Kraken Harbor
		XMFLOAT3(167.749f,	-0.1f,	213.043f),	// Port: Monsoon Retreat
		XMFLOAT3(-142.898f,	-0.1f,	166.739f)	// Port: Tortuga
	};


	// Triangulated waypoints: approximated center points of triangles made from port names(acronyms) & locations
	XMFLOAT3 wp_triangles[3] =
	{
		// new map
		XMFLOAT3(177.406f,	-0.1f,	-169.511f),	// Triangle: SS, IDC, OPC
		XMFLOAT3(112.17f,	-0.1f,	-240.714f),	// Triangle: DBE, IDC, SS
		XMFLOAT3(-129.614f,	-0.1f,	-46.4882f)	// Triangle: PR, DBE, T
	};


	// Other points in the world (NOT on island or port locations)
	XMFLOAT3 wp_others[10] =
	{
		// Spots
		XMFLOAT3(0.0f,		-0.1f,	0.0f),		// Spot: world center/origin
		XMFLOAT3(-8.60797f,	-0.1f,	-17.9099f),	// Spot: minimap center

		// Voyage points
		XMFLOAT3(79.8104f,	-0.1f,	196.933f),	// Voyage: 1/3 way from MR island to T island
		XMFLOAT3(20.8302f,	-0.1f,	197.896f),	// Voyage: 2/3 way from MR island to T island
		XMFLOAT3(-66.6024f,	-0.1f,	198.188f),	// Voyage: 3/3 way from MR island to T island
		XMFLOAT3(-156.649f,	-0.1f,	108.007f),	// Voyage: 1/3 way from T island to PR island
		XMFLOAT3(-171.509f,	-0.1f,	15.7212f),	// Voyage: 2/3 way from T island to PR island
		XMFLOAT3(-142.017f,	-0.1f,	-72.3846f),	// Voyage: 3/3 way from T island to PR island

		// Others.....
		XMFLOAT3(235.349f,	-0.1f,	-34.9223f),	// Other: S of midpoint between OPC & DKH
		XMFLOAT3(215.221f,	-0.1f,	139.811f)	// Other: midway between DKH island & MR island
	};



	// # of waypoints
	size_t portsize = ARRAYSIZE(wp_ports);
	size_t trisize = ARRAYSIZE(wp_triangles);
	size_t othersize = ARRAYSIZE(wp_others);



	// add to out ports vec
	out_ports.resize(portsize);
	for (size_t i = 0; i < portsize; i++)
		out_ports.insert(out_ports.begin() + i, wp_ports[i]);

	// add to out triangles vec
	out_triangles.resize(trisize);
	for (size_t i = 0; i < trisize; i++)
		out_triangles.insert(out_triangles.begin() + i, wp_triangles[i]);

	// add to out other vec
	out_other.resize(othersize);
	for (size_t i = 0; i < othersize; i++)
		out_other.insert(out_other.begin() + i, wp_others[i]);

}

void GameplayScene::SortWorldWaypoints(char entry_cardinal_direction, std::vector<XMFLOAT3>& out_waypoints)
{
	// Get world waypoints
	std::vector<XMFLOAT3> wp_ports;
	std::vector<XMFLOAT3> wp_triangles;
	std::vector<XMFLOAT3> wp_other;
	GetWorldWaypoints(wp_ports, wp_triangles, wp_other);


	// # of waypoints
	size_t portsize = wp_ports.size();
	size_t trisize = wp_triangles.size();
	size_t othersize = wp_other.size();


	// clear out vec
	out_waypoints.clear();


	// waypoint order
	switch (entry_cardinal_direction)
	{
	case 'N':
	case 'S':
	case 'W':
	case 'E':
		for (size_t i = 0; i < portsize; i++)
			out_waypoints.push_back(wp_ports[i]);
		for (size_t i = 0; i < trisize; i++)
			out_waypoints.push_back(wp_triangles[i]);
		for (size_t i = 0; i < othersize; i++)
			out_waypoints.push_back(wp_other[i]);
		break;

	default:
		break;
	}
}

void GameplayScene::SortBossWaypoints(std::vector<XMFLOAT3>& out_waypoints)
{
	// Get world waypoints
	std::vector<XMFLOAT3> wp_ports;
	std::vector<XMFLOAT3> wp_triangles;
	std::vector<XMFLOAT3> wp_other;
	GetWorldWaypoints(wp_ports, wp_triangles, wp_other);


	// # of waypoints
	size_t portsize = wp_ports.size();
	size_t trisize = wp_triangles.size();
	size_t othersize = wp_other.size();


	// clear out vec
	out_waypoints.clear();


	//out_waypoints.push_back(wp_ports[0]);
	for (size_t i = 0; i < portsize; i++)
		reconTeam.squad->AddWaypoint(wp_ports[i]);
}

void GameplayScene::ActivateBoardingPrompt(void)
{
	m_hudElems["BoardButtonXbox"]->Active(false);
	m_hudElems["BoardButtonKeyboard"]->Active(false);


	//G2D::GUITexture* hud_gui = nullptr;
	//if (GameSetting::GetRef().GamePadIsConnected() == true)
	//{
	//	hud_gui = reinterpret_cast<G2D::GUITexture*>(m_hudElems["BoardButtonXbox"]);
	//}
	//else
	//{
	//	hud_gui = reinterpret_cast<G2D::GUITexture*>(m_hudElems["BoardButtonKeyboard"]);
	//}
	G2D::GUITexture* hud_gui = (GameSetting::GetRef().GamePadIsConnected() == true)
								? static_cast<G2D::GUITexture*>(m_hudElems["BoardButtonXbox"])
								: static_cast<G2D::GUITexture*>(m_hudElems["BoardButtonKeyboard"]);

	SimpleMath::Color guicolor = hud_gui->Color();
	hud_gui->Color(SimpleMath::Color(1.0f, 1.0f, 1.0f, 1.0f));


	PromptTimer = 3.0f;
}

// Event funtions
void GameplayScene::SpawnReconSquadEvent(const EventArgs& args)
{
	if (reconTeam.fleet.size() > 0)
		return;


	// new squad for recon ships
	reconTeam = ShipTeamStruct();
	reconTeam.squad = newx Squad();



	// Minimap center: XMFLOAT3(-8.60797f, -0.1f, -17.9099f)
	char		dir[4] = { 'N', 'S', 'E', 'W' };
	XMFLOAT2	spawnloc[4] =
	{
		XMFLOAT2(-295.0f,	-17.9099f),	// N
		XMFLOAT2(295.0f,	-17.9099f),	// S
		XMFLOAT2(-8.60797f,	295.0f),	// E
		XMFLOAT2(-8.60797f,	-295.0f)	// W
	};


	// farthest point from player
	XMFLOAT3 playerpos;
	positionOf(playerShip->GetWorldMatrix(), playerpos);

	int		dir_index = -1;
	float	prevdist = FLT_MIN;
	for (int i = 0; i < 4; i++)
	{
		// new distance
		float newdist = sqrDistanceBetween(playerpos, XMFLOAT3(spawnloc[i].x, -0.1f, spawnloc[i].y));

		// new distance shorter?
		if (newdist <= prevdist)
			continue;

		// new distance farther
		prevdist = newdist;
		dir_index = i;
	}



	// ship pos
	XMFLOAT3 reconpos = XMFLOAT3(spawnloc[dir_index].x, -0.1f, spawnloc[dir_index].y);
	//DirectX::XMFLOAT4X4	mat = SimpleMath::Matrix::Identity;
	//mat._41 = spawnloc[index].x;
	//mat._42 = -0.1f;
	//mat._43 = spawnloc[index].y;
	//mat._43 = 310.0f;


	// # of recon ships to make
	int numships = 4;
	for (int i = 0; i < numships; i++)
	{
		// adjust ship position
		if (i > 0)
		{
			(dir_index >= 2)
				? reconpos.x += 0.30f	// vertical
				: reconpos.z += 0.30f;	// horizontal
		}

		// load & add ship
		Ship* reconship = CreateAIShip("FleetMember", reconpos);
		//Ship*	reconship = (AIShip*)sceneManager->GetEntityManager().CreateEntity3D("FleetMember", mat);
		//sceneManager->GetEntityManager().AddEntity(GetType(), EntityManager::EntityBucket::RENDERABLE, reconship);
		//renderList.push_back(reconship->GetRenderInfo());

		//// set recon squad leader
		//if (i == 0)
		//	reconTeam.squad->SetLeader(reconship->GetController()->GetBoid());

		// add to recon fleet & recon squad
		reconTeam.fleet.push_back(reconship);
		Boid* myBoid = reconship->GetController()->GetBoid();
		reconTeam.squad->AddSquadmate(myBoid);
	
		((AiController*)reconship->GetController())->SetEngageDistance(400);
	}



	// get starting ship cardinal direction
	/*
	if (mat._43 > 300.0f)
		char dir = 'E';
	if (mat._43 < -300.0f)
		char dir = 'W';
	if (mat._41 < -210.0f)
		char dir = 'N';
	if (mat._41 > 280.0f)
		char dir = 'S';
	*/

	//std::vector<XMFLOAT3> waypoints;

	// fill out waypoint ordering
	//SortWorldWaypoints(dir[dir_index], waypoints);

	// add waypoints
	size_t wpsize = ports.size();
	for (size_t i = 0; i < wpsize; i++)
		reconTeam.squad->AddWaypoint(ports[i]->GetPosition());	//reconTeam.squad->AddWaypoint(waypoints[i]);

	//// pacify ships
	//size_t fleetsize = reconTeam.fleet.size();
	//for (size_t i = 0; i < fleetsize; i++)
	//	((AiController*)reconTeam.fleet[i]->GetController())->SetEngageDistance(-1.0f);


	// recon HUD stuff
	std::wstring text = L"";
	switch (dir[dir_index])
	{
	case 'N':
		text = L"North";
		break;
	case 'S':
		text = L"South";
		break;
	case 'E':
		text = L"East";
		break;
	case 'W':
		text = L"West";
		break;

	default:
		break;
	}
	text = L"Reports of recon ships in the " + text + L" Cap'n!";


	m_hudElems["MinimapRecon"]->Active(true);
	m_hudElems["MinimapRecon"]->Scale(0.0f);	// 3.6f
	m_hudElems["ReconText1"]->Active(true);
	m_hudElems["ReconText2"]->Active(true);
	static_cast<G2D::GUIText*>(m_hudElems["ReconText1"])->Text(text);
	m_fReconTextTimer = 8.0f;
	static_cast<CGame*>(CGame::GetApplication())->GetSoundManager()->Play3DSound(AK::EVENTS::PLAY_RECONSPAWN);
}

void GameplayScene::SpawnBossReinforcementSquadEvent(const EventArgs& args)
{
	static std::random_device rseed;
	static std::mt19937 rng(rseed());

	static std::uniform_int<int> numofShips(7, 10);
	int numships = numofShips(rng);
	for (int i = 0; i < numships; i++)
	{
		// adjust ship position
		auto bossPos = BossTeam.fleet[0]->GetPosition();

		bool AddMe = true;

		// moving the point behind the player 
		XMFLOAT3& targetPosition = bossPos + BossTeam.fleet[0]->GetForward();


		static std::uniform_int<int> dist(-1, 1);

		while (AddMe)
		{
			//adjust the spawn points
			int x = dist(rng);
			while (x == 0)
			{
				x = dist(rng);
			}
			int z = dist(rng);
			while (z == 0)
			{
				z = dist(rng);
			}
			targetPosition.x += x*30.0f;
			targetPosition.z += z*30.0f;

			//loop through all of the island
			for (auto& island : islands)
			{
				//grabbing the meshIsland
				MeshCollider* MeshIsland = ((MeshCollider*)island->GetCollider());

				//find if the point is in the meshTree
				if (!MeshIsland->getQuadTree().GetMyAABB().contains(targetPosition))
				{
					AddMe = false;
				}
			}
		}












		// load & add ship
		Ship*	reinforcementship = CreateAIShip("FleetMember", targetPosition);

		AiController* aiCon = (AiController*)reinforcementship->GetController();
		if (playerShip)
		{
			aiCon->ChaseTarget(playerShip);
		}


		bossReinforcementTeam.fleet.push_back(reinforcementship);
		bossReinforcementTeam.squad->AddSquadmate(reinforcementship->GetController()->GetBoid());

	}


	// add waypoints
	if (BossTeam.squad)
	{
		bossReinforcementTeam.squad->AddWaypoint(BossTeam.squad->GetPosition());
	}


}

void GameplayScene::BossTrackingEvent(const EventArgs1<Squad*>& args)
{
	// null check
	if (BossTeam.squad == nullptr)
		return;

	// boss squad check
	if (args.m_Data != BossTeam.squad)
		return;

	// boss fleet check
	size_t fleetsize = BossTeam.fleet.size();
	if (fleetsize == 0)
		return;


	// boss ship
	BossShip* boss = nullptr;
	for (size_t i = 0; i < fleetsize; i++)
	{
		static_cast<AiController*>(BossTeam.fleet[i]->GetController())->ClearWaypoints();

		if (BossTeam.fleet[i]->GetShipType() != Ship::eShipType::TYPE_BOSS)
			continue;
		boss = static_cast<BossShip*>(BossTeam.fleet[i]);
		break;
	}
	if (boss == nullptr)
		return;




	/// PLEASE DO NOT ERASE!!!
	/*
	====================
	Boss Tracking Logic!
	====================

	-Boss spawn:
		-A: After the boss spawns it will go to the 1st & only specified waypoint it knows.
			-NOTE: This waypoint is just an entry waypoint for the player to know the boss exists & will soon start to do stuff.

	-Boss normal movement:
		-A: If the boss has 0 waypoints to track the player with, it will continuously port hop (based on port distance) until it encounters the player, and switches to [Boss player encounter] logic.
		-B: Else, it will continuously port hop (based on ports visited by player) until it encounters the player, and switches to [Boss player encounter] logic.

	-Boss player encounter:
		-A: If the boss encounters the player & the player runs away, it will chase the player until the end of time.
		-B: If the player visits(at least collides) with a port while running from the boss, the boss will revert to [Boss normal movement] logic.
	*/



	// store current waypoint
	XMFLOAT3 oldwaypoint = BossTeam.squad->GetCurrentWaypoint();
	if (oldwaypoint.y == FLT_MAX)
		return;


	// get port waypoints
	size_t							wpsize = ports.size();
	std::vector<DirectX::XMFLOAT3>	portpoints;
	for (size_t i = 0; i < wpsize; i++)
		portpoints.push_back(ports[i]->GetPosition());



	// find which port boss was at last time
	for (size_t i = 0; i < wpsize; i++)
	{
		if (oldwaypoint != ports[i]->GetPosition())
			continue;
		boss->SetLastPlayerPort(ports[i]);
		break;
	}



	// waypoints check
	if (BossTeam.squad->GetWaypoints().size() < 3)
	{
		// get rid of spawn waypoints
		for (;;)
		{
			if (BossTeam.squad->GetWaypoints().size() == 0)
				break;
			BossTeam.squad->RemoveWaypoint(0);
		}
		BossTeam.squad->GetWaypoints().clear();


		// sort based on distance from boss position
		XMFLOAT3 bosspos = boss->GetPosition();
		std::sort(portpoints.begin(), portpoints.end(),
				  [&bosspos](DirectX::XMFLOAT3 const &a, DirectX::XMFLOAT3 const &b) -> bool {
					return sqrDistanceBetween(bosspos, a) < sqrDistanceBetween(bosspos, b);
				 });


		// add port waypoints
		for (size_t i = 0; i < wpsize; i++)
			BossTeam.squad->AddWaypoint(portpoints[i]);
	}



	// get last port visited by player (VARIABLE TO BE REUSED AS SOMETHING ELSE)
	Port*	recentport	= static_cast<PlayerShip*>(playerShip)->GetMostRecentPort();




	// HUD text
	//std::wstring b4_text = L"";



	// go to last recent port
	if (recentport != nullptr && recentport != boss->GetLastPlayerPort())
	{
		// last recent port
		boss->SetLastPlayerPort(recentport);


		// get rid of ANY previous waypoints
		for (;;)
		{
			if (BossTeam.squad->GetWaypoints().size() == 0)
				break;
			BossTeam.squad->RemoveWaypoint(0);
		}
		BossTeam.squad->GetWaypoints().clear();


		// redirect to player's most recently visited port
		BossTeam.squad->AddWaypoint(recentport->GetPosition());
		//b4_text = L"(Boss ship headin' for " + recentport->GetPortName() + L")";

		// prepare for next change of course
		//boss->SetTrackingCourseSet(true);
		//boss->SetChangeTrackingCourse(true);
	}

	// player visited 0-1 ports
	else if (recentport == nullptr || recentport == boss->GetLastPlayerPort())
	{
		// stay on course - SHOULD HAPPENS AUTOMATICALLY!!!
		//XMFLOAT3 nextwaypoint = BossTeam.squad->GetCurrentWaypoint();
		int nextwaypoint = BossTeam.squad->GetCurrentWaypointIndex();

		if (nextwaypoint < 0 || nextwaypoint >= BossTeam.squad->GetWaypoints().size())
			nextwaypoint = 0;


		// find which port boss is going to next
		for (size_t i = 0; i < wpsize; i++)
		{
			if (BossTeam.squad->GetWaypoints()[nextwaypoint] != ports[i]->GetPosition())
				continue;
			recentport = ports[i];
			//b4_text = L"(Boss ship headin' for " + recentport->GetPortName() + L")";
			break;
		}
	}





	// HUD stuff
	BossNextWaypointEvent(EventArgs1<BossNextWaypointEventArgs>(BossNextWaypointEventArgs(BossTeam.squad, recentport)));
	//fsEventManager::GetInstance()->FireEvent(fsEvents::BossNextWaypoint, &EventArgs1<BossNextWaypointEventArgs>(BossNextWaypointEventArgs(BossTeam.squad, recentport)));

	//if (b4_text.size() > 3)
	//{
	//	// turn on/off gui
	//	m_hudElems["Boss"]->Active(false);
	//	m_hudElems["Boss2"]->Active(false);
	//	m_hudElems["Boss3"]->Active(true);
	//	m_hudElems["Boss4"]->Active(true);
	//
	//	// setup boss text
	//	static_cast<G2D::GUIText*>(m_hudElems["Boss4"])->Text(b4_text);
	//
	//	// set gui timer
	//	m_fBossTextTimer = 8.0f;
	//}
}

/// PLEASE DO NOT ERASE YET!!!
/*
void GameplayScene::BossTrackingEvent(const EventArgs& args)
{
	/// PLEASE DO NOT ERASE!!!
	//====================
	//Boss Tracking Logic!
	//====================
	//
	//-Boss spawn:
	//	-A: After the boss spawns it will go to the 1st & only specified waypoint it knows.
	//		-NOTE: This waypoint is just an entry waypoint for the player to know the boss exists & will soon start to do stuff.
	//
	//-Boss normal movement:
	//	-A: If the boss has 0 waypoints to track the player with, it will continuously port hop (based on port distance) until it encounters the player, and switches to [Boss player encounter] logic.
	//	-B: Else, it will continuously port hop (based on ports visited by player) until it encounters the player, and switches to [Boss player encounter] logic.
	//
	//-Boss player encounter:
	//	-A: If the boss encounters the player & the player runs away, it will chase the player until the end of time.
	//	-B: If the player visits(at least collides) with a port while running from the boss, the boss will revert to [Boss normal movement] logic.

	std::wstring b4_text = L"";


	// store current waypoint
	XMFLOAT3 oldwaypoint = BossTeam.squad->GetCurrentWaypoint();
	if (oldwaypoint.y == FLT_MAX)
		return;


	// get rid of spawn waypoints
	if (BossTeam.squad->GetWaypoints().size() < 3)
	{
		for (;;)
		{
			if (BossTeam.squad->GetWaypoints().size() == 0)
				break;
			BossTeam.squad->RemoveWaypoint(0);
		}
		BossTeam.squad->GetWaypoints().clear();
	}



	// boss ship
	BossShip* boss = static_cast<BossShip*>(BossTeam.fleet[0]);



	// get last port visited by player (VARIABLE TO BE REUSED AS SOMETHING ELSE)
	Port* recentport = static_cast<PlayerShip*>(playerShip)->GetMostRecentPort();


	// player visited 0-1 ports
	if (recentport == nullptr || operator==(recentport->GetPosition(), oldwaypoint) == true)
	{
		// rethink course
		if (boss->GetChangeTrackingCourse() == true || BossTeam.squad->GetWaypoints().size() == 0)
		{
			// get port waypoints
			size_t							wpsize = ports.size();
			std::vector<DirectX::XMFLOAT3>	portpoints;
			for (size_t i = 0; i < wpsize; i++)
				portpoints.push_back(ports[i]->GetPosition());


			// sort based on distance from current position
			XMFLOAT3 bosspos = boss->GetPosition();
			std::sort(portpoints.begin(), portpoints.end(),
				[&bosspos](DirectX::XMFLOAT3 const &a, DirectX::XMFLOAT3 const &b) -> bool {
				return sqrDistanceBetween(bosspos, a) < sqrDistanceBetween(bosspos, b);
			});


			// set waypoints for Boss ship (& squad)
			for (size_t i = 0; i < wpsize; i++)
				BossTeam.squad->AddWaypoint(portpoints[i]);


			// find which port boss is going to next
			for (size_t i = 0; i < wpsize; i++)
			{
				if (portpoints[0] != ports[i]->GetPosition())
					continue;
				recentport = ports[i];
				b4_text = L"(Boss ship headin' for " + recentport->GetPortName() + L")";
				break;
			}


			// don't change tracking course
			boss->SetTrackingCourseSet(true);
			boss->SetChangeTrackingCourse(false);
		}

		// stay on course - SHOULD HAPPENS AUTOMATICALLY!!!
		else
		{
			// get current waypoints
			XMFLOAT3 currwaypoint = BossTeam.squad->GetCurrentWaypoint();
			if (oldwaypoint.y != FLT_MAX)
			{
				// get port waypoints
				size_t							wpsize = ports.size();
				std::vector<DirectX::XMFLOAT3>	portpoints;
				for (size_t i = 0; i < wpsize; i++)
					portpoints.push_back(ports[i]->GetPosition());


				// find which port boss is going to next
				for (size_t i = 0; i < wpsize; i++)
				{
					if (portpoints[0] != ports[i]->GetPosition())
						continue;
					recentport = ports[i];
					b4_text = L"(Boss ship headin' for " + recentport->GetPortName() + L")";
					break;
				}
			}
		}
	}
	else
	{
		// last recent port
		boss->SetLastPlayerPort(recentport);


		// get rid of ANY previous waypoints
		for (;;)
		{
			if (BossTeam.squad->GetWaypoints().size() == 0)
				break;
			BossTeam.squad->RemoveWaypoint(0);
		}
		BossTeam.squad->GetWaypoints().clear();


		// redirect to player's most recently visited port
		BossTeam.squad->AddWaypoint(recentport->GetPosition());
		b4_text = L"(Boss ship headin' for " + recentport->GetPortName() + L")";

		// prepare for next change of course
		boss->SetTrackingCourseSet(true);
		boss->SetChangeTrackingCourse(true);
	}



	if (b4_text.size() > 3)
	{
		// turn on/off gui
		m_hudElems["Boss"]->Active(false);
		m_hudElems["Boss2"]->Active(false);
		m_hudElems["Boss3"]->Active(true);
		m_hudElems["Boss4"]->Active(true);

		// setup boss text
		static_cast<G2D::GUIText*>(m_hudElems["Boss4"])->Text(b4_text);

		// set gui timer
		m_fBossTextTimer = 8.0f;
	}
}
*/

void GameplayScene::BossHealingEvent(const EventArgs& args)
{
	// text
	std::wstring b4_text = L"Boss ship making repairs Cap'n!";


	// turn on/off gui
	m_hudElems["Boss"]->Active(false);
	m_hudElems["Boss2"]->Active(false);
	m_hudElems["Boss3"]->Active(false);
	m_hudElems["Boss4"]->Active(true);

	// setup boss text
	static_cast<G2D::GUIText*>(m_hudElems["Boss4"])->Text(b4_text);

	// set gui timer
	m_fBossTextTimer = 8.0f;
}

void GameplayScene::BossNextWaypointEvent(const EventArgs1<BossNextWaypointEventArgs>& args)
{
	BossNextWaypointEventArgs ev_args = args.m_Data;

	// null check
	if (BossTeam.squad == nullptr)
		return;

	// boss squad check
	if (ev_args.currSquad != BossTeam.squad)
		return;

	// port check
	if (ev_args.nextPort == nullptr)
		return;

	// waypoint check
	if (BossTeam.squad->GetWaypoints().size() == 0)
		return;


	//// get current waypoints
	//XMFLOAT3 currwaypoint = BossTeam.squad->GetCurrentWaypoint();
	//if (currwaypoint.y == FLT_MAX)
	//	return;


	//// get port waypoints
	//size_t							wpsize = ports.size();
	//std::vector<DirectX::XMFLOAT3>	portpoints;
	//for (size_t i = 0; i < wpsize; i++)
	//	portpoints.push_back(ports[i]->GetPosition());


	// find which port boss is going to next
	std::wstring b4_text = L"(Boss ship headin' for " + ev_args.nextPort->GetPortName() + L")";


	// turn on/off gui
	m_hudElems["Boss"]->Active(false);
	m_hudElems["Boss2"]->Active(false);
	m_hudElems["Boss3"]->Active(true);
	m_hudElems["Boss4"]->Active(true);

	// setup boss text
	static_cast<G2D::GUIText*>(m_hudElems["Boss4"])->Text(b4_text);

	// set gui timer
	m_fBossTextTimer = 8.0f;
}

//Struct functions
void GameplayScene::ShipTeamStruct::Clear(void)
{
	// clear fleet
	fleet.clear();

	// clear squad
	if (squad != nullptr)
	{
		delete squad;
		squad = nullptr;
	}

	/*
	// clear squad
	if (squad != nullptr)
	{
		delete squad;
		squad = nullptr;
	}

	// clear fleet
	for (size_t i = 0; i < fleet.size(); i++)
	{
		if (fleet[i] == nullptr)
			continue;
		delete fleet[i];
		fleet[i] = nullptr;
	}
	fleet.clear();
	*/
}

//Dont Catch the pointer unless you want to modify the ship
Ship* GameplayScene::CreateAIShip(const char* _fileName, XMFLOAT3 &_pos)
{
	DirectX::XMFLOAT4X4	mat =
		DirectX::XMFLOAT4X4(
			1.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f,
			_pos.x, _pos.y, _pos.z, 1.0f);

	Ship*	aiShip = (AIShip*)sceneManager->GetEntityManager().CreateEntity3D(_fileName, mat);

	if (aiShip == nullptr)
	{
		return nullptr;
	}

	AiController* newAi = ((AiController*)aiShip->GetController());
	if (newAi->GetAIType() == AiController::AI_Type::MILITARY && aiShip->GetShipType() != Ship::eShipType::TYPE_BOSS) {
		static_cast<BoxCollider*>(aiShip->GetCollider())->setDimensions(XMFLOAT3(.76f, 1, 2.5f)); // setup for model swap
		static_cast<BoxCollider*>(aiShip->GetCollider())->setOffset(XMFLOAT3(0, .296f, -.038f)); // setup for model swap
	}

	newAi->m_vtAllBoids = &GetAIShips();

	sceneManager->GetEntityManager().AddEntity(GetType(), EntityManager::EntityBucket::RENDERABLE, aiShip);

	std::vector<Renderer::CRenderable*> renderList;

	renderList.push_back(aiShip->GetRenderInfo());

	CGame::GetApplication()->m_pRenderer->SetUpRenderables(renderList);

	aiShips.push_back(aiShip);

	return aiShip;
}

void GameplayScene::LoadGulls(void)
{
	XMFLOAT4X4 gmat;
	XMStoreFloat4x4(&gmat, XMMatrixIdentity());
	setScale(gmat, XMFLOAT3(.1f, .1f, .1f));
	for (size_t i = 0; i < 100; i++)
	{
		setPosition(gmat, XMFLOAT3((float)(rand() % 100 - 50), (float)(rand() % 2 + 3), (float)(rand() % 100 - 50)));
		gulls.push_back(new Gull(gmat));
	}
}

void GameplayScene::LoadObject(const EventArgs2<const char*, XMFLOAT4X4>& loadInfo)
{
	//Grabbing the string that is being passed through
	std::string objectName = loadInfo.m_Data1;
	Entity3D* newObject;

	//Creating the object
	newObject = sceneManager->GetEntityManager().CreateEntity3D(objectName.c_str(), loadInfo.m_Data2);

	//Adding the Objec to be rendered
	if (newObject != nullptr)
	{
		sceneManager->GetEntityManager().AddEntity(GetType(), EntityManager::EntityBucket::RENDERABLE, newObject);

		switch (newObject->GetType())
		{
		case Entity3D::EntityType::LAND:
			islands.push_back(newObject);
			break;
		case Entity3D::EntityType::SHIP:
			//Checking to see if its the player 
			if (strcmp(objectName.c_str(), "PlayerShip") == 0)
			{

				playerShip = (PlayerShip*)newObject;

				//if so call the load player function
				LoadPlayerShip();

				break;
			}
			//adds Enemys tothe list
			aiShips.push_back((Ship*)newObject);
			break;
		case Entity3D::EntityType::CRATE:
			crates.push_back(newObject);
			break;
		case Entity3D::EntityType::PORT:
			ports.push_back((Port*)newObject);
			break;
		}
	}


}

void GameplayScene::CallInMilitary(const EventArgs1<Ship*>& Target)
{
	if (m_pReinforcement != nullptr || BossTeam.fleet.empty() == false)
	{
		return;
	}

	Squad hunters;

	//Ship& newTarget = *Target.m_Data;
	PlayerShip& newTarget = ((PlayerShip&)*Target.m_Data);

	if (newTarget.ToBeDeleted())
	{
		return;
	}

	const unsigned int&  IslandSize = (const unsigned int)islands.size();

	std::vector<XMFLOAT3> SpawnPoints;

	XMFLOAT3& targetForward = newTarget.GetForward();

	//scaling the vector by -20
	targetForward *= -50;

	bool AddMe = true;

	for (unsigned int i = 0; i < 8; i++)
	{
		//for adding  in spawnpoints
		AddMe = true;

		// moving the point behind the player 
		XMFLOAT3& targetPosition = newTarget.GetPosition() + targetForward;

		//adjust the spawn points
		targetPosition.x += ((i + 1) % 4) - 3.0f;
		targetPosition.z += i / 4;

		//loop through all of the island
		for (unsigned int islandIndex = 0; islandIndex < IslandSize; islandIndex++)
		{
			//grabbing the meshIsland
			MeshCollider* MeshIsland = ((MeshCollider*)islands[islandIndex]->GetCollider());

			//find if the point is in the meshTree
			if (MeshIsland->getQuadTree().GetMyAABB().contains(targetPosition))
			{
				AddMe = false;
			}
		}

		//Adding the point to the vector of spawn points
		if (AddMe == true)
		{
			SpawnPoints.push_back(targetPosition);
		}
	}

	std::vector<Boid*> squadmates;
	unsigned int SquadSize = (unsigned int)SpawnPoints.size();

	if (SquadSize == 0)
	{
		return;
	}

	float spawnRepMax = newTarget.GetMaxReputation() * 0.20f;

	float ratio = (newTarget.GetCurrReputation() + 1) / spawnRepMax;

	ratio *= 100.0f;

	Ship* aiShip = nullptr;

	if (Target.m_Data)
	{
		Target.m_Data->m_PopTextTimer = 3.0f;

		Target.m_Data->m_PopText = L"Bounty Hunters are after us Capt'n";
	}

	for (unsigned int i = 0; ratio > 0 && i < SquadSize; i++)
	{
		aiShip = CreateAIShip(MILITARYSHIP, SpawnPoints[i]);
		AiController* hunter = ((AiController*)aiShip->GetController());
		hunter->ChaseTarget(&newTarget);
		hunter->SetAttackFinderTimer(2000);
		aiShip->SetCurrGold(200);
		hunters.AddSquadmate(aiShip->GetController()->GetBoid());

		ratio -= 100;
	}

	//Spawning the new squad
	fsEventManager::GetInstance()->FireEvent(fsEvents::AddSquad, &EventArgs1<Squad*>(&hunters));
	m_pReinforcement = CSteeringManger::SteeringManger().GetSquad(hunters);

	if (m_pReinforcement != nullptr)
	{
		m_pReinforcement->GetWaypoints().clear();
	}
}

void GameplayScene::FindFarthestShip(std::vector<Ship*>& MoveAbleShip)
{
	XMFLOAT3& playerPos = playerShip->GetPosition();

	unsigned int AiSize = (unsigned int)aiShips.size();

	float sqrFarthestDistance = 400;

	float newDistance = 0;

	Ship* farthest = nullptr;

	//looping through all of the ships
	for (unsigned int i = 1; i < AiSize; i++)
	{
		Ship* farthest = aiShips[i];

		newDistance = sqrDistanceBetween(playerPos, farthest->GetPosition());

		if (sqrFarthestDistance < newDistance)
		{
			MoveAbleShip.push_back(farthest);
		}
	}

	return;
}

void GameplayScene::PlayDarkwater(void)
{
	if (InDarkWater == false)
	{
		SoundManager* soundManager = ((CGame*)CGame::GetApplication())->GetSoundManager();
		soundManager->Play3DSound(AK::EVENTS::PLAY_DARKWATERS);
		InDarkWater = true;
	}
}

void GameplayScene::AddHudResources(int _gold, int _rum, int _food)
{
	size_t startingIndex = 0;
	if (_gold)
	{
		int numOfGoldIcons = 1;
		for (int iGoldIcon = 0; iGoldIcon < numOfGoldIcons; iGoldIcon++)
		{
			G2D::GUITexture *pNewGold = new G2D::GUITexture(*((G2D::GUITexture*)m_hudElems["3DHUD_GoldBagIcon"]));
			TResourcePickUpIcon *pNewResourcePickUp = new TResourcePickUpIcon();
			pNewResourcePickUp->m_pTexture = pNewGold;
			pNewResourcePickUp->m_fTimer = -1.0f;
			pNewResourcePickUp->m_nResourceAmount = _gold;
			m_vRewards.push_back(pNewResourcePickUp);
			m_GUIManager.AddEntity(pNewGold);
		}

		int GoldScale = _gold % 10;
		if (0 <= GoldScale)
		{
			G2D::GUITexture *pNewGold = new G2D::GUITexture(*((G2D::GUITexture*)m_hudElems["3DHUD_GoldBagIcon"]));
			pNewGold->Scale(GoldScale / 10.0f);
			TResourcePickUpIcon *pNewResourcePickUp = new TResourcePickUpIcon();
			pNewResourcePickUp->m_pTexture = pNewGold;
			pNewResourcePickUp->m_fTimer = -1.0f;
			pNewResourcePickUp->m_nResourceAmount = GoldScale;
			m_vRewards.push_back(pNewResourcePickUp);
			m_GUIManager.AddEntity(pNewGold);
		}
		if (startingIndex != m_vRewards.size())
		{
			m_vRewards[startingIndex]->m_pTexture->Active(true);
			m_vRewards[startingIndex]->m_fTimer = 0.0f;
		}
	}

	if (_rum)
	{
		startingIndex = m_vRewards.size();

		int numOfRumIcons = 1;
		for (int iRumIcon = 0; iRumIcon < numOfRumIcons; iRumIcon++)
		{
			G2D::GUITexture *pNewRum = new G2D::GUITexture(*((G2D::GUITexture*)m_hudElems["3DHUD_RumIcon"]));
			TResourcePickUpIcon *pNewResourcePickUp = new TResourcePickUpIcon();
			pNewResourcePickUp->m_pTexture = pNewRum;
			pNewResourcePickUp->m_fTimer = -1.0f;
			pNewResourcePickUp->m_nResourceAmount = _rum;
			m_vRewards.push_back(pNewResourcePickUp);
			m_GUIManager.AddEntity(pNewRum);
		}

		int RumScale = _rum % 10;
		if (0 <= RumScale)
		{
			G2D::GUITexture *pNewRum = new G2D::GUITexture(*((G2D::GUITexture*)m_hudElems["3DHUD_RumIcon"]));
			pNewRum->Scale(RumScale / 10.0f);
			TResourcePickUpIcon *pNewResourcePickUp = new TResourcePickUpIcon();
			pNewResourcePickUp->m_pTexture = pNewRum;
			pNewResourcePickUp->m_fTimer = -1.0f;
			pNewResourcePickUp->m_nResourceAmount = RumScale;
			m_vRewards.push_back(pNewResourcePickUp);
			m_GUIManager.AddEntity(pNewRum);
		}
		if (startingIndex != m_vRewards.size())
		{
			m_vRewards[startingIndex]->m_pTexture->Active(true);
			m_vRewards[startingIndex]->m_fTimer = 0.0f;
		}
	}

	if (_food)
	{
		startingIndex = m_vRewards.size();

		int numOfFoodIcons = 1;
		for (int iFoodIcon = 0; iFoodIcon < numOfFoodIcons; iFoodIcon++)
		{
			G2D::GUITexture *pNewFood = new G2D::GUITexture(*((G2D::GUITexture*)m_hudElems["3DHUD_FoodIcon"]));
			TResourcePickUpIcon *pNewResourcePickUp = new TResourcePickUpIcon();
			pNewResourcePickUp->m_pTexture = pNewFood;
			pNewResourcePickUp->m_fTimer = -1.0f;
			pNewResourcePickUp->m_nResourceAmount = _food;
			m_vRewards.push_back(pNewResourcePickUp);
			m_GUIManager.AddEntity(pNewFood);
		}

		int FoodScale = _food % 10;
		if (0 <= FoodScale)
		{
			G2D::GUITexture *pNewFood = new G2D::GUITexture(*((G2D::GUITexture*)m_hudElems["3DHUD_FoodIcon"]));
			pNewFood->Scale(FoodScale / 10.0f);
			TResourcePickUpIcon *pNewResourcePickUp = new TResourcePickUpIcon();
			pNewResourcePickUp->m_pTexture = pNewFood;
			pNewResourcePickUp->m_fTimer = -1.0f;
			pNewResourcePickUp->m_nResourceAmount = FoodScale;
			m_vRewards.push_back(pNewResourcePickUp);
			m_GUIManager.AddEntity(pNewFood);
		}
		if (startingIndex != m_vRewards.size())
		{
			m_vRewards[startingIndex]->m_pTexture->Active(true);
			m_vRewards[startingIndex]->m_fTimer = 0.0f;
		}
	}

	m_bUpdateResourcesHud = true;
}

void GameplayScene::UpdateHudResources(float _deltaTime)
{
	if (_deltaTime > 0.125f)
		_deltaTime = 0.125f;
	for (size_t iItemPickup = 0; iItemPickup < m_vRewards.size(); iItemPickup++)
	{
		if (0.0f <= m_vRewards[iItemPickup]->m_fTimer)
		{
			G2D::GUITexture* currPickup = reinterpret_cast<G2D::GUITexture*>(m_vRewards[iItemPickup]->m_pTexture);
			if (!currPickup)
				continue;

			if (iItemPickup + 1 < m_vRewards.size())
			{
				if (0 == currPickup->TextureName().compare(((G2D::GUITexture*)m_vRewards[iItemPickup + 1]->m_pTexture)->TextureName()))
				{
					if (ICON_HUD_TIME_DELAY <= m_vRewards[iItemPickup]->m_fTimer && 0 > m_vRewards[iItemPickup + 1]->m_fTimer)
					{
						m_vRewards[iItemPickup + 1]->m_fTimer = 0.0f;
						m_vRewards[iItemPickup + 1]->m_pTexture->Active(true);
					}
				}
			}

			m_vRewards[iItemPickup]->m_fTimer += _deltaTime;

			currPickup->SetPosition(SimpleMath::Vector3());
			currPickup->Position(
			{
				currPickup->StaticPostions()["Start"].x + (currPickup->Offsets()["IconPosition"].x * (m_vRewards[iItemPickup]->m_fTimer / ICON_HUD_TIME)),
				currPickup->StaticPostions()["Start"].y + (currPickup->Offsets()["IconPosition"].y * (m_vRewards[iItemPickup]->m_fTimer / ICON_HUD_TIME))
			});

			if (m_vRewards[iItemPickup]->m_fTimer > ICON_HUD_TIME)
			{
				if (0 == currPickup->TextureName().compare("3DHUD_GoldBagIcon"))
				{
					auto pGoldIcon = reinterpret_cast<G2D::GUITexture*>(m_hudElems["HUD_GoldIcon"]);
					auto pGoldIconText = reinterpret_cast<G2D::GUIText*>(m_hudElems["GoldStatus"]);
					pGoldIcon->Active(true);
					pGoldIconText->Active(true);

					playerShip->IncreaseGold(m_vRewards[iItemPickup]->m_nResourceAmount);
				}
				else if (0 == currPickup->TextureName().compare("3DHUD_RumIcon"))
				{
					auto pRumIcon = reinterpret_cast<G2D::GUITexture*>(m_hudElems["HUD_RumIcon"]);
					auto pRumIconText = reinterpret_cast<G2D::GUIText*>(m_hudElems["RumStatus"]);
					pRumIcon->Active(true);
					pRumIconText->Active(true);

					playerShip->IncreaseRum(m_vRewards[iItemPickup]->m_nResourceAmount);
				}
				else if (0 == currPickup->TextureName().compare("3DHUD_FoodIcon"))
				{
					auto pWaterFoodIcon = reinterpret_cast<G2D::GUITexture*>(m_hudElems["HUD_FoodIcon"]);
					auto pWaterFoodIconText = reinterpret_cast<G2D::GUIText*>(m_hudElems["FoodStatus"]);
					pWaterFoodIcon->Active(true);
					pWaterFoodIconText->Active(true);

					playerShip->IncreaseFood(m_vRewards[iItemPickup]->m_nResourceAmount);
				}

				m_vRewards.erase(m_vRewards.begin() + iItemPickup);
				m_GUIManager.RemoveEntity(currPickup);
				SafeRelease(currPickup);
				--iItemPickup;
			}
		}
	}

	if (0 == m_vRewards.size())
		m_bUpdateResourcesHud = false;
}