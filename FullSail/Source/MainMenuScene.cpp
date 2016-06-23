#include "pch.h"
#include "MainMenuScene.h"
#include "SceneManager.h"
#include "GameplayScene.h"
#include "PortMenuScene.h"
#include "AiController.h"
#include "Ship.h"
#include "EntityManager.h"
#include "Camera.h"
#include <Renderable.h>
#include <Water.h>
#include "CinematicCamera.h"
#include "Game.h"
#include "CollisionManager.h"


MainMenuScene::MainMenuScene() : IScene(), switchCameraTargetTimer(0.0f) {
	m_MainCamera.reset(new CinematicCamera);
	// scene type
	sceneType = SceneType::MAIN_MENU;
	currCursor = (int)MenuItems::GAMEPLAY;
	scrollTimer = SCENE_SCROLL_TIME;

	SoundManager* sound = ((CGame*)CGame::GetApplication())->GetSoundManager();
	sound->RegisterListener(m_MainCamera.get(), "MainMenuScene Camera");

	water = sceneManager->GetEntityManager().CreateWater();
	sceneManager->GetEntityManager().AddEntity(GetType(), EntityManager::EntityBucket::RENDERABLE, water);

	aiWaypoints.push_back({ 30.0f,0.0f,0.0f });
	aiWaypoints.push_back({ -30.0f,0.0f,0.0f });
	aiWaypoints.push_back({ 0.0f,0.0f,30.0f });
	aiWaypoints.push_back({ 0.0f,0.0f,-30.0f });

	for (size_t i = 0; i < 4; i++) {
		DirectX::XMFLOAT4X4	mat = DirectX::XMFLOAT4X4(1.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f,
			0.0f, 0.0f, (float)i*5.0f, 1.0f);

		Ship*	aiShip = (Ship*)sceneManager->GetEntityManager().CreateEntity3D("MilitaryShip", mat);
		sceneManager->GetEntityManager().AddEntity(GetType(), EntityManager::EntityBucket::RENDERABLE, aiShip);
		((AiController*)aiShip->GetController())->SetWaypoints(&aiWaypoints);
		aiShips.push_back(aiShip);
		aiShip->StopColliders();
	}


	CGame::GetApplication()->m_pRenderer->SetPostProcessIndex(5);
}


MainMenuScene::~MainMenuScene() {


	for (size_t i = 0; i < aiShips.size(); i++) {
		delete aiShips[i];
		aiShips[i] = nullptr;
	}
	aiShips.clear();


	currCursor = (int)MenuItems::UNKNOWN;
	if (water) {
		delete water;
	}
}


void MainMenuScene::Enter(void) {
	// base enter
	IScene::Enter();
	LoadHudList();
	// get renderables
	std::vector<Renderer::CRenderable*> renderList = sceneManager->GetEntityManager().GetRenderables(GetType(), EntityManager::EntityBucket::RENDERABLE);

	// add renderables
	CGame::GetApplication()->m_pRenderer->SetupSkybox();
	CGame::GetApplication()->m_pRenderer->SetUpRenderables(renderList);
	m_MainCamera->SetOffset(-10.0f);

	SoundManager* soundManager = ((CGame*)CGame::GetApplication())->GetSoundManager();
	soundManager->Play3DSound(AK::EVENTS::PLAYMAINMENU);

	for (size_t i = 0; i < aiShips.size(); i++)
	{
		aiShips[i]->StartColliders();
	}
}


void MainMenuScene::Exit(void) {
	ReleaseHudList();
	// base exit
	IScene::Exit();
}


bool MainMenuScene::Input(void) {
	CGame* game = (CGame*)CGame::GetApplication();
	return (GameSetting::GetRef().GamePadIsConnected()) ? Input_Gamepad() : Input_Keyboard();
}


void MainMenuScene::Update(float _dt) {
	using namespace std;
	static float total = 0;
	total += _dt*2.0f;

	if (switchCameraTargetTimer <= 0.0f) {
		switchCameraTargetTimer = 10.0f;
		m_MainCamera->m_Target = aiShips[rand() % aiShips.size()];
		m_FadeIn = true;
		m_Timer = FADEINTIMER;
		m_FadeAmount = 0.0f;
		CGame::GetApplication()->m_pRenderer->SetPostProcessFadeAmount(m_FadeAmount);
	}
	else {
		switchCameraTargetTimer -= _dt;
	}


	if (m_FadeIn) {
		m_Timer -= _dt;
		float timeInv = 1.0f - m_Timer / FADEINTIMER;
		if (timeInv >= 1.0f) {
			timeInv = 1.0f;
		}

		CGame::GetApplication()->m_pRenderer->SetPostProcessFadeAmount(timeInv);
		if (m_Timer <= 0.0f) {
			m_Timer = FADEINTIMER;
			m_FadeIn = false;
			CGame::GetApplication()->m_pRenderer->SetPostProcessFadeAmount(0.0f);
		}
	}



	// base update
	IScene::Update(_dt);


	// scroll timer
	if (scrollTimer > 0.0f)
		scrollTimer -= _dt;
	if (scrollTimer < 0.0f)
		scrollTimer = 0.0f;

	auto bgKeyboard = static_cast<G2D::GUITexture*>(m_hudElems["MainMenu_bg_keyboard"]);
	auto bgGamePad = static_cast<G2D::GUITexture*>(m_hudElems["MainMenu_bg_gamepad"]);
	if (GameSetting::GetRef().GamePadIsConnected()) {
		bgKeyboard->Active(false);
		bgGamePad->Active(true);
	}
	else {
		bgKeyboard->Active(true);
		bgGamePad->Active(false);
	}
	auto cursorNG = static_cast<G2D::GUITexture*>(m_hudElems["MainMenu_cursor_newgame"]);
	auto cursorHTP = static_cast<G2D::GUITexture*>(m_hudElems["MainMenu_cursor_howtoplay"]);
	auto cursorCD = static_cast<G2D::GUITexture*>(m_hudElems["MainMenu_cursor_credits"]);
	auto cursorOP = static_cast<G2D::GUITexture*>(m_hudElems["MainMenu_cursor_option"]);
	auto cursorEX = static_cast<G2D::GUITexture*>(m_hudElems["MainMenu_cursor_exit"]);
	cursorNG->Active(false);
	cursorHTP->Active(false);
	cursorCD->Active(false);
	cursorOP->Active(false);
	cursorEX->Active(false);
	switch (currCursor) {
	case GAMEPLAY:
		cursorNG->Active(true);
		break;
	case INSTRUCTIONS:
		cursorHTP->Active(true);
		break;
	case CREDITS:
		cursorCD->Active(true);
		break;
	case OPTIONS:
		cursorOP->Active(true);
		break;
	case EXIT:
		cursorEX->Active(true);
		break;
	default:
		break;
	}

}


void MainMenuScene::Render(void) {
	// base render
	IScene::Render();
	m_GUIManager.RenderAll();

}


void MainMenuScene::Input_Start(void)
{
	SoundManager* soundManager = ((CGame*)CGame::GetApplication())->GetSoundManager();
	soundManager->Play3DSound(AK::EVENTS::PLAYMAINMENU);

	CGame::GetApplication()->m_pRenderer->DrawSkybox();
	CGame::GetApplication()->m_pRenderer->SetPostProcessIndex(5);

	for (size_t i = 0; i < aiShips.size(); i++)
	{
		((AiController*)aiShips[i]->GetController())->ChaseTarget(nullptr);
		aiShips[i]->StartColliders();
	}
}


void MainMenuScene::Input_End(void)
{
	SoundManager* soundManager = ((CGame*)CGame::GetApplication())->GetSoundManager();
	soundManager->Play3DSound(AK::EVENTS::STOPMAINMENU);


	for (size_t i = 0; i < aiShips.size(); i++)
	{
		aiShips[i]->StopColliders();
	}
}


bool MainMenuScene::Input_Keyboard(void) {
	//CInputManager* input_manager = InputManager();
	CGame*							game = (CGame*)CGame::GetApplication();
	auto							kbs = Keyboard::Get().GetState();
	Keyboard::KeyboardStateTracker*	kbst = game->GetKeyboardTracker();
	SoundManager*					sound = game->GetSoundManager();


	if (kbst->IsKeyPressed(Keyboard::Keys::Back) == true) {
		currCursor = MenuItems::EXIT;
		sound->Play3DSound(AK::EVENTS::PLAYCURSOR);
	}

	if (kbst->IsKeyPressed(Keyboard::Keys::Down) == true || kbst->IsKeyPressed(Keyboard::Keys::NumPad2) == true || kbst->IsKeyPressed(Keyboard::Keys::S) == true) {
		currCursor = (currCursor + 1 <= MenuItems::EXIT) ? (currCursor + 1) : MenuItems::GAMEPLAY;
		//if (currCursor == MenuItems::CREDITS)
		//	currCursor = MenuItems::EXIT;
		//else
		//	currCursor = (currCursor + 1 <= MenuItems::EXIT) ? (currCursor + 1) : MenuItems::GAMEPLAY;
		sound->Play3DSound(AK::EVENTS::PLAYCURSOR);
	}

	if (kbst->IsKeyPressed(Keyboard::Keys::Up) == true || kbst->IsKeyPressed(Keyboard::Keys::NumPad8) == true || kbst->IsKeyPressed(Keyboard::Keys::W) == true) {
		currCursor = (currCursor - 1 >= MenuItems::GAMEPLAY) ? (currCursor - 1) : MenuItems::EXIT;
		//if (currCursor == MenuItems::EXIT)
		//	currCursor = MenuItems::CREDITS;
		//else
		//	currCursor = (currCursor - 1 >= MenuItems::GAMEPLAY) ? (currCursor - 1) : MenuItems::EXIT;
		sound->Play3DSound(AK::EVENTS::PLAYCURSOR);
	}


	if (kbst->IsKeyPressed(Keyboard::Keys::Enter) == true) {
		sound->PostEvent(AK::EVENTS::STOPMAINMENU);
		for (auto& aiShip : aiShips)
			sceneManager->GetCollisionManager()->stopTracking(aiShip->GetCollider());


		// change curr scene
		switch (currCursor) {
		case MenuItems::GAMEPLAY:
			((GameplayScene*)sceneManager->GetScene(SceneType::GAMEPLAY))->ResetGameplay();
			((PortMenuScene*)sceneManager->GetScene(SceneType::PORT))->ResetPrices();
			sceneManager->PushScene(SceneType::GAMEPLAY);
			//sceneManager->PushScene(SceneType::WIN_LOSE );
			return NEW_SCENE;
			break;

		case MenuItems::INSTRUCTIONS:
			sceneManager->PushScene(SceneType::INSTRUCTIONS);
			return NEW_SCENE;
			break;

		case MenuItems::CREDITS:
			sceneManager->PushScene(SceneType::CREDITS);
			return NEW_SCENE;
			break;

		case MenuItems::OPTIONS:
			sceneManager->PushScene(SceneType::OPTIONS);
			return NEW_SCENE;
			break;

		case MenuItems::EXIT:
			sceneManager->PopScene();
			return NEW_SCENE;
			break;

		case MenuItems::UNKNOWN:
		case MenuItems::NUM_CHOICES:
		default:
			break;
		}
	}



	// stay in same scene
	return OLD_SCENE;
}


bool MainMenuScene::Input_Gamepad(void) {
	CGame*							game = (CGame*)CGame::GetApplication();
	auto							gps = GamePad::Get().GetState(0);
	GamePad::ButtonStateTracker*	gpst = game->GetGamePadTracker();
	SoundManager*					sound = game->GetSoundManager();

	/* Input: B */
	if (gpst->b == GamePad::ButtonStateTracker::ButtonState::PRESSED) {
		currCursor = MenuItems::EXIT;
		sound->Play3DSound(AK::EVENTS::PLAYCURSOR);
	}

	/* Input: move cursor down (w/ wrap around) */
	if (gps.IsLeftThumbStickDown() == true || gps.IsDPadDownPressed() == true) {
		if (scrollTimer <= 0.0f) {
			scrollTimer = SCENE_SCROLL_TIME;
			currCursor = (currCursor + 1 <= MenuItems::EXIT) ? (currCursor + 1) : MenuItems::GAMEPLAY;

			//if (currCursor == MenuItems::CREDITS)
			//	currCursor = MenuItems::EXIT;
			//else
			//	currCursor = (currCursor + 1 <= MenuItems::EXIT) ? (currCursor + 1) : MenuItems::GAMEPLAY;
			sound->Play3DSound(AK::EVENTS::PLAYCURSOR);
		}
	}


	/* Input: move cursor up  (w/ wrap around) */
	if (gps.IsLeftThumbStickUp() == true || gps.IsDPadUpPressed() == true) {
		if (scrollTimer <= 0.0f) {
			scrollTimer = SCENE_SCROLL_TIME;
			currCursor = (currCursor - 1 >= MenuItems::GAMEPLAY) ? (currCursor - 1) : MenuItems::EXIT;

			//if (currCursor == MenuItems::EXIT)
			//	currCursor = MenuItems::CREDITS;
			//else
			//	currCursor = (currCursor - 1 >= MenuItems::GAMEPLAY) ? (currCursor - 1) : MenuItems::EXIT;
			sound->Play3DSound(AK::EVENTS::PLAYCURSOR);
		}
	}


	/* Input: select curr menu option */
	if (gpst->a == GamePad::ButtonStateTracker::ButtonState::PRESSED) {
		sound->PostEvent(AK::EVENTS::STOPMAINMENU);
		for (auto& aiShip : aiShips)
			sceneManager->GetCollisionManager()->stopTracking(aiShip->GetCollider());


		// change curr scene
		switch (currCursor) {
		case MenuItems::GAMEPLAY:
			((GameplayScene*)sceneManager->GetScene(SceneType::GAMEPLAY))->ResetGameplay();
			((PortMenuScene*)sceneManager->GetScene(SceneType::PORT))->ResetPrices();
			//sceneManager->ResetScene(SceneType::GAMEPLAY);
			sceneManager->PushScene(SceneType::GAMEPLAY);
			return NEW_SCENE;
			break;

		case MenuItems::INSTRUCTIONS:
			sceneManager->PushScene(SceneType::INSTRUCTIONS);
			return NEW_SCENE;
			break;

		case MenuItems::CREDITS:
			sceneManager->PushScene(SceneType::CREDITS);
			return NEW_SCENE;
			break;

		case MenuItems::OPTIONS:
			sceneManager->PushScene(SceneType::OPTIONS);
			return NEW_SCENE;
			break;

		case MenuItems::EXIT:
			sceneManager->PopScene();
			return NEW_SCENE;
			break;

		case MenuItems::UNKNOWN:
		case MenuItems::NUM_CHOICES:
		default:
			break;
		}
	}



	// stay in same scene
	return OLD_SCENE;
}

void MainMenuScene::LoadHudList() {
	G2D::CGUIManager::LoadXMLConfig("../../FullSail/Resources/GUI/Configs/MainMenuScene.xml", m_hudElems);

	for (auto it = m_hudElems.begin(); it != m_hudElems.end(); it++) {
		m_GUIManager.AddEntity(it->second);
	}
}

void MainMenuScene::ReleaseHudList() {
	// Clean up GUI stuff
	for (auto it = m_hudElems.begin(); it != m_hudElems.end(); it++) {
		m_GUIManager.RemoveEntity(it->second);
		SafeRelease(it->second);
	}
	m_hudElems.clear();
}

