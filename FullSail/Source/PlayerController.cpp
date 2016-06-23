#include "pch.h"
#include "PlayerController.h"
#include "Ship.h"
#include "PlayerShip.h"
#include "SideCannonHandler.h"
#include "Game.h"
#include "GameplayScene.h"
#include "PortMenuScene.h"
#include "SoundManager.h"
#include "Colliders.h"
#include "Log.h"
#include "Camera.h"
#include "CollisionManager.h"
#include "Port.h"
#include "AiController.h"
#include "GameStatistics.h"


PlayerController::PlayerController(Ship* _ps) : IController(_ps), inCombat(false), combatMeter(0.0f), singingMeter(0.0f)
{
	m_pOwner->SetAcceleration(0.1f);	//1.0f;	//0.1f; //0.0001f;	// 0.000001f;

	// initialize keyboard controls
	ctrls[InputCommands::FORWARD] = Keyboard::Keys::W;	//'W';
	ctrls[InputCommands::BACKWARDS] = Keyboard::Keys::S;	//'S';
	ctrls[InputCommands::TURN_LEFT] = Keyboard::Keys::A;	//'A';
	ctrls[InputCommands::TURN_RIGHT] = Keyboard::Keys::D;	//'D';
	ctrls[InputCommands::SHOOT_LEFTSIDE] = Keyboard::Keys::J;	//'J';
	ctrls[InputCommands::SHOOT_L9] = Keyboard::Keys::K;	//'K';
	ctrls[InputCommands::SHOOT_RIGHTSIDE] = Keyboard::Keys::L;	//'L';
	ctrls[InputCommands::AMMO_ITER_L] = Keyboard::Keys::N;	//'N';
	ctrls[InputCommands::AMMO_ITER_R] = Keyboard::Keys::M;	//'M';
	ctrls[InputCommands::BOARD] = Keyboard::Keys::O;	//'B';
	ctrls[InputCommands::PORT] = Keyboard::Keys::O;	//'O';
	ctrls[InputCommands::SPYGLASS] = Keyboard::Keys::Tab;	//'I';

	locked = false;
	m_currPort = nullptr;

	TemplatedCollisionCallback<PlayerController>* thing = new TemplatedCollisionCallback<PlayerController>(this, &PlayerController::SpawnCollisionReaction);
	m_pSpawnCollider = new SphereCollider(*((CGame*)CGame::GetApplication())->GetGameplayScene()->GetSceneManager()->GetCollisionManager(),
		_ps->GetTransform().GetWorldMatrix(), m_pOwner, thing, 50, XMFLOAT3(0, -_ps->GetPosition().y + .1f, 0), true, "NoIsland");

	m_pOwner->GetSideCannonHandler()->GetCannons()->SetMaxReload(0.7f);
}


PlayerController::~PlayerController()
{
	delete m_pSpawnCollider;
}


void PlayerController::Update(float dt)
{
	//CInputManager* im = InputManager();
	CGame* game = (CGame*)CGame::GetApplication();


	if (game->TimeScale < 0.1f)
		return;



	if (locked == false)
	{
		(GameSetting::GetRef().GamePadIsConnected())
			? Input_Gamepad(dt)
			: Input_Keyboard(dt);
	}



	///* log ship position - DO NOT ERASE YET! */
	/*
	Keyboard::KeyboardStateTracker*	kbst = game->GetKeyboardTracker();
	if (kbst->IsKeyPressed(Keyboard::Keys::NumPad5) == true)
	{
		XMFLOAT3 pos = m_pOwner->GetPosition();

		std::stringstream ss;
		ss << "{ " << pos.x << "f, " << pos.y << "f, " << pos.z << "f }";

		Log("Player pos: " + ss.str());
	}
	*/


	PlayerSounds(dt);

	Movement(dt);

	nearAgro = false;
	
	if (m_currPort)
		m_currPort->OpenPort();

	m_currPort = nullptr;
}


void PlayerController::StopColliders()
{
	CollisionManager& GameCollision = *((CGame*)CGame::GetApplication())->GetSceneManager()->GetCollisionManager();

	GameCollision.stopTracking(m_pSpawnCollider);

}

void PlayerController::StartColliders()
{
	CollisionManager& GameCollision = *((CGame*)CGame::GetApplication())->GetSceneManager()->GetCollisionManager();

	GameCollision.trackThis(m_pSpawnCollider);
}


void PlayerController::Input_Keyboard(float dt)
{
	CGame*							game	= (CGame*)CGame::GetApplication();
	auto							kbs		= Keyboard::Get().GetState();
	Keyboard::KeyboardStateTracker*	kbst	= game->GetKeyboardTracker();
	SoundManager*					sound	= game->GetSoundManager();
	GameplayScene*					gpScene	= (GameplayScene*)game->GetSceneManager()->GetScene(IScene::SceneType::GAMEPLAY);


	// Function Args




	if (kbs.IsKeyDown(ctrls[InputCommands::FORWARD]) == true)
	{
		SpeedUp(dt);
		game->GetGameplayScene()->SetHudSpeedTimer(5.0f);
	}

	else if (kbs.IsKeyDown(ctrls[InputCommands::BACKWARDS]) == true)
	{
		SlowDown(dt);
		game->GetGameplayScene()->SetHudSpeedTimer(5.0f);
	}


	if (kbs.IsKeyDown(ctrls[InputCommands::TURN_LEFT]) == true)
	{
		RotateLeft(dt);
	}

	else if (kbs.IsKeyDown(ctrls[InputCommands::TURN_RIGHT]) == true)
	{
		RotateRight(dt);
	}
	// Input(Rotate): ??? (???)
	else
	{
		// reset rotation
	/*	m_fCurrentRotation = 0.0f;*/
	}




	if (kbs.IsKeyDown(ctrls[InputCommands::SPYGLASS]) == true)
	{
		// turn on spygalss
		gpScene->ActivateSpyglass(true);
	}
	else
	{
		// turn off spygalss
		gpScene->ActivateSpyglass(false);

		/* Input(Ammotypes): Switching ammo */
		if (kbst->IsKeyPressed(ctrls[InputCommands::AMMO_ITER_L]) == true)
		{
			m_pOwner->GetSideCannonHandler()->SetCurrentAmmoIndex(IAmmo::eAmmoType::CANNONBALL);
			sound->Play3DSound(AK::EVENTS::PLAYSWITCHWEAPON);
		}
		if (kbst->IsKeyPressed(ctrls[InputCommands::AMMO_ITER_R]) == true)
		{
			m_pOwner->GetSideCannonHandler()->SetCurrentAmmoIndex(IAmmo::eAmmoType::CHAIN_SHOT);
			sound->Play3DSound(AK::EVENTS::PLAYSWITCHWEAPON);
		}



		if (kbs.IsKeyDown(ctrls[InputCommands::SHOOT_LEFTSIDE]) == true)
		{
			FireCannons(dt, 0);
		}
		if (kbs.IsKeyDown(ctrls[InputCommands::SHOOT_RIGHTSIDE]) == true)
		{
			FireCannons(dt, 1);
		}


		if (kbs.IsKeyDown(ctrls[InputCommands::SHOOT_L9]) == true)
		{
			FireLong9(dt);
		}

		if (kbs.IsKeyDown(ctrls[InputCommands::SHOOT_H]) == true)
		{
			FireHarpoons(dt);
		}




		/*if (kbst->IsKeyPressed(ctrls[InputCommands::BOARD]) == true)
		{
			if (m_pOwner->GetPortCollision() == false)
				BoardEnemyShip(dt);
		}*/

		if (kbst->IsKeyPressed(ctrls[InputCommands::PORT]) == true)
		{
			bool docked = nearAgro ? false : DockAtPort(dt);

			if (docked == false && false == BoardEnemyShip(dt) && static_cast<PlayerShip*>(m_pOwner)->GetPortCollision() == true)
				sound->Play3DSound(AK::EVENTS::PLAYREJECT);
		}
	}

}


void PlayerController::Input_Gamepad(float dt)
{
	CGame*							game	= (CGame*)CGame::GetApplication();
	auto							gps		= GamePad::Get().GetState(0);
	GamePad::ButtonStateTracker*	gpst	= game->GetGamePadTracker();
	SoundManager*					sound	= game->GetSoundManager();
	GameplayScene*					gpScene	= (GameplayScene*)game->GetSceneManager()->GetScene(IScene::SceneType::GAMEPLAY);



	// Function Args


	/* Input(Translate): Full sails (forward) */
	//if (gps.IsLeftThumbStickUp() == true)
	//{
	//	//SpeedUp(dt);
	//}

	///* Input(Translate): No sails (slow down/stop) */
	//else if (gps.IsLeftThumbStickDown() == true)
	//{
	//	//SpeedUp(dt);
	//	//SlowDown(dt);
	//}
	if (gpst->leftStick == GamePad::ButtonStateTracker::ButtonState::HELD)
	{
		SlowDown(dt);
		game->GetGameplayScene()->SetHudSpeedTimer(5.0f);
	}

	else if (gps.IsLeftThumbStickUp() || gps.IsLeftThumbStickDown() || gps.IsLeftThumbStickLeft() || gps.IsLeftThumbStickRight())
	{
		SpeedUp(dt);
		game->GetGameplayScene()->SetHudSpeedTimer(5.0f);
	}

	XMFLOAT3 Forward = m_pOwner->GetForward();
	normalizeFloat3(Forward);

	XMFLOAT3 leftStickForward = { -gps.thumbSticks.leftY,0.0f,gps.thumbSticks.leftX };
	normalizeFloat3(leftStickForward);
	XMFLOAT3 leftStickRight = { gps.thumbSticks.leftX, 0 ,gps.thumbSticks.leftY };
	normalizeFloat3(leftStickRight);
	float turnRate = dotProduct(Forward, leftStickForward);
	if (abs(turnRate) > 0.0001f)
	{
		float turnDir = dotProduct(leftStickRight, Forward);
		if (turnDir > 0.0f)
		{
			m_pOwner->TurnRadians(-acos(turnRate), dt);
		}
		else
		{
			m_pOwner->TurnRadians(acos(turnRate), dt);
		}
	}

	if (turnRate < -0.0001f)
	{
		SlowDown(dt);
	}
	else if (turnRate > 0.0001f && gpst->leftStick != GamePad::ButtonStateTracker::ButtonState::HELD)
	{
		SpeedUp(dt);
	}
	//else
	//{
	//	//SlowDown(0.5f*dt);
	//}



	///* Input(Rotate): Starboard (rotate right) */
	//else if (gps.IsLeftThumbStickRight() == true)
	//{
	//	RotateRight(dt);
	//}

	///* Input(Rotate): ??? (???) */
	//else
	//{
	//	// reset rotation
	//	/*	m_fCurrentRotation = 0.0f;*/
	//}


	if (gpst->leftShoulder == GamePad::ButtonStateTracker::ButtonState::HELD)
	{
		// turn on spygalss
		gpScene->ActivateSpyglass(true);
	}
	else
	{
		// turn off spygalss
		gpScene->ActivateSpyglass(false);


		/* Input(Ammotypes): Switching ammo */
		if (gpst->x == GamePad::ButtonStateTracker::ButtonState::PRESSED)
		{
			m_pOwner->GetSideCannonHandler()->SetCurrentAmmoIndex(IAmmo::eAmmoType::CANNONBALL);
			sound->Play3DSound(AK::EVENTS::PLAYSWITCHWEAPON);
		}
		if (gpst->y == GamePad::ButtonStateTracker::ButtonState::PRESSED)
		{
			m_pOwner->GetSideCannonHandler()->SetCurrentAmmoIndex(IAmmo::eAmmoType::CHAIN_SHOT);
			sound->Play3DSound(AK::EVENTS::PLAYSWITCHWEAPON);
		}



		/* Input(Shooting): Cannons (Fire) */
		if (gps.IsRightTriggerPressed() == true)
		{
			FireCannons(dt, 1);
		}
		if (gps.IsLeftTriggerPressed() == true)
		{
			FireCannons(dt, 0);
		}

		/* Input(Shooting): Long9 (Fire) */
		if (gps.IsRightShoulderPressed())
		{
			FireLong9(dt);
		}




		/* Input(Porting/Boarding): Go to PortScene OR Board an enemy ship */
		if (gpst->a == GamePad::ButtonStateTracker::ButtonState::PRESSED)
		{
			bool docked = inCombat && nearAgro ? false : DockAtPort(dt);

			if (docked == false && false == BoardEnemyShip(dt) && static_cast<PlayerShip*>(m_pOwner)->GetPortCollision() == true)
				sound->Play3DSound(AK::EVENTS::PLAYREJECT);
		}
	}
}

// DO NOT ERASE!!!
/*
bool PlayerController::OK_to_Continue(void)
{
	//if (m_bSTOP == true)
	//{
	//	return false;
	//}
	return true;
}
*/

void PlayerController::SpeedUp(float args)
{
	//if (OK_to_Continue() == false)
	//	return;

	//Moving the pbject forward
	Accelerate(args);

}


void PlayerController::SlowDown(float args)
{
	//if (OK_to_Continue() == false)
	//	return;

	Decelerate(args);

}


void PlayerController::RotateLeft(float args)
{
	//if (OK_to_Continue() == false)
	//	return;

	//m_RotationAngle -= MATH_PI * 3 * args.GetData();
	m_pOwner->TurnRadians(-m_pOwner->GetTurnSpeedLimit(), args);

}


void PlayerController::RotateRight(float args)
{
	//if (OK_to_Continue() == false)
	//	return;

	//m_RotationAngle += MATH_PI * 3 * args.GetData();
	m_pOwner->TurnRadians(m_pOwner->GetTurnSpeedLimit(), args);

}


void PlayerController::FireCannons(float args, int side)
{
	//if (OK_to_Continue() == false)
	//	return;
	if (m_pOwner->GetSideCannonHandler()->Fire(Cannon::WeaponType::CANNON, side))
	{
		singingMeter -= 1.0f;
	}



}


void PlayerController::FireLong9(float args)
{
	//if (OK_to_Continue() == false)
	//	return;

	// fire long 9
	m_pOwner->GetSideCannonHandler()->Fire(Cannon::WeaponType::LONG_NINE, 2);
	singingMeter -= 1.0f;
}


void PlayerController::FireHarpoons(float args)
{
	//if (OK_to_Continue() == false)
	//	return;

	// fire harpoon gun
	m_pOwner->GetSideCannonHandler()->Fire(Cannon::WeaponType::HARPOON_GUN, 2);
}


void PlayerController::AmmoIterLeft(float args)
{
	//if (OK_to_Continue() == false)
	//	return;

	// ammo--
}


void PlayerController::AmmoIterRight(float args)
{
	//if (OK_to_Continue() == false)
	//	return;


	// ammo++
}


bool PlayerController::BoardEnemyShip(float args)
{
	//if (OK_to_Continue() == false)
	//	return;

	// currently boarding
	if (m_pOwner->GetBoardingStage() > Ship::eBoardingState::NONE)
	{
		return false;
	}

	PlayerShip* player = static_cast<PlayerShip*>(m_pOwner);


	// find a target
	if (!player->FindPossibleBoardingTargets())
		return false;
	if (!player->SetCurrentBoardingTarget())
		return false;

	// align next to enemy ship
	if (!player->CheckBoardingDistance())
		return false;

	return true;
}


bool PlayerController::DockAtPort(float args)
{
	//if (OK_to_Continue() == false)
	//	return;


	CGame*	game = (CGame*)CGame::GetApplication();
	IScene*	currscene = game->GetSceneManager()->GetCurrScene();


	// Not in correct scene
	if (currscene->GetType() != IScene::SceneType::GAMEPLAY)
		return false;


	// Is it possible to go to port?
	if (static_cast<PlayerShip*>(m_pOwner)->GetPortCollision() == false)
		return false;


	// Setup Port Menu
	/*
	PortMenuScene* port = (PortMenuScene*)game->GetSceneManager()->GetScene(IScene::SceneType::PORT);
	port->SetPortName(m_pOwner->GetPortName());
	*/
	Port* collidedport = ((GameplayScene*)currscene)->GetPort(static_cast<PlayerShip*>(m_pOwner)->GetPortName());
	if (collidedport == nullptr)
		return false;
	fsEventManager* fsem = fsEventManager::GetInstance();
	fsem->FireEvent(fsEvents::PortSetup, &EventArgs1<PortMenuScene::PortSetupEventArgs>(PortMenuScene::PortSetupEventArgs(collidedport)));



	// Lock player to port
	m_pOwner->SetSpeed(0);


	// Switch to Port Menu
	static_cast<GameplayScene*>(currscene)->GoToPortScene();
	return true;
}

void PlayerController::SpawnCollisionReaction(Collider & other)
{
	Entity3D* CollidedObject = reinterpret_cast<Entity3D*>(other.getID());

	//I do not want to check my self
	if (CollidedObject->GetType() == Entity3D::EntityType::PORT)
	{
		Port* port = reinterpret_cast<Port*>(CollidedObject);
		if (nearAgro) {
			port->ClosePort();
		}
		else {
			port->OpenPort();
		}
		m_currPort = port;
	}
	else if (CollidedObject == m_pOwner || other.isTrigger())
	{
		return;
	}

	//If you collide with a Ship 
	if (CollidedObject->GetType() == Entity3D::EntityType::SHIP)
	{
		if (static_cast<AiController*>(static_cast<Ship*>(CollidedObject)->GetController())->GetTarget() == m_pOwner) {
			nearAgro = true;
			if (m_currPort)
				m_currPort->ClosePort();
		}
	}

}

void PlayerController::LowHealth()
{
	//if the teh player health gets lower then 40%
	float healthPercent = (m_pOwner->GetHealth() / (float)m_pOwner->GetMaxHealth());

	SoundManager* soundManager = ((CGame*)CGame::GetApplication())->GetSoundManager();
	if (healthPercent < 0.3 && LowHealthIndicate)
	{
		soundManager->Play3DSound(AK::EVENTS::PLAY_PLAYERLOWHEALTH);
		soundManager->Play3DSound(AK::EVENTS::PLAYSHIPONFIRE);
		LowHealthIndicate = false;
	}
	if (healthPercent > 0.3)
	{
		soundManager->PostEvent(AK::EVENTS::STOPSHIPONFIRE);
	}

}

void PlayerController::PlayerSounds(float dt)
{
	LowHealth();


	//Music Switching
	SoundManager* soundManager = ((CGame*)CGame::GetApplication())->GetSoundManager();
	if (!inCombat && m_pOwner->GeRefCount() > 0)
	{
		inCombat = true;
		combatMeter = 5.0f;
		soundManager->SetStates(AK::STATES::BGMSTATES::GROUP, AK::STATES::BGMSTATES::STATE::COMBAT);
	}

	if (inCombat)
	{
		if (m_pOwner->GeRefCount() <= 0) {
			combatMeter -= dt;
		}
		else {
			singingMeter = 0.0f;
			combatMeter = 5.0f;
		}
	}

	if (combatMeter <= 0.0f)
	{
		inCombat = false;
		soundManager->SetStates(AK::STATES::BGMSTATES::GROUP, AK::STATES::BGMSTATES::STATE::SEA);
	}

	//Singing

	singingMeter += dt;
	if (!inCombat && singingMeter >= 15.0f)
	{
		singingMeter = 0.0f;
		soundManager->SetStates(AK::STATES::BGMSTATES::GROUP, AK::STATES::BGMSTATES::STATE::SINGSONGS);
	}
	if (!inCombat && singingMeter < 0.0f)
	{
		soundManager->SetStates(AK::STATES::BGMSTATES::GROUP, AK::STATES::BGMSTATES::STATE::SEA);
	}
}

void PlayerController::Movement(float dt)
{
	// Movement
	IController::Update(dt);
}


