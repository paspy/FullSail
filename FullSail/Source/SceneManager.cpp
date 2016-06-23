#include "pch.h"
#include "Utilities.h"
#include "SceneManager.h"
#include "Math.h"
#include "Stack.h"
#include "MainMenuScene.h"
#include "GameplayScene.h"
#include "OptionsMenuScene.h"
#include "InstructionsScene.h"
#include "PortMenuScene.h"
#include "PauseMenuScene.h"
#include "CreditsScene.h"
#include "WinLoseScene.h"
#include "IntroScene.h"
#include "Game.h"
#include "Camera.h"
#include "CollisionManager.h"
#include "EntityManager.h"
#include "IEntity.h"
#include "Entity3D.h"
#include "Ship.h"
#include "PlayerShip.h"
#include "Crate.h"
#include "Port.h"
#include "IController.h"
#include "CSteeringManger.h"


SceneManager::SceneManager(void)
{
	// assert: array size check
	assert((ARRAYSIZE(scenes) == IScene::SceneType::NUM_SCENES) && "SceneManager::SceneManager() - array size is different!");
	currScene = IScene::SceneType::UNKNOWN;
	m_fNoticeBool = false;
	m_fNoticeTimer = SCENE_NOTICE_TIME;

	// initialize collision manager
	collisionManager = new CollisionManager();

	//CEventManager::CreateInstance();

	// initialize entity manager
	entityManager.Initialize();


	// events
	fsEventManager* fsem = fsEventManager::GetInstance();
	fsem->RegisterClient(fsEvents::FireLeftCannons, this, &SceneManager::FireLeftSideCannonEvent);
	fsem->RegisterClient(fsEvents::FireRightCannons,this, &SceneManager::FireRightSideCannonEvent);
	fsem->RegisterClient(fsEvents::FireSideCannons, this, &SceneManager::FireBothSideCannonsEvent);
	fsem->RegisterClient(fsEvents::FireFrontCannon,	this, &SceneManager::FireFrontCannonEvent);
	fsem->RegisterClient(fsEvents::PortSale,		this, &SceneManager::PortSaleEvent);
	fsem->RegisterClient(fsEvents::PortSetColor,	this, &SceneManager::PortSetColorEvent);
	/*
	*/
	/*
	EventManager()->RegisterClient("AddEvent", this, &SceneManager::AddEvent);
	EventManager()->RegisterClient("RemoveEvent", this, &SceneManager::RemoveEvent);
	EventManager()->RegisterClient("FireSideCannonEvent", this, &SceneManager::FireSideCannonEvent);
	EventManager()->RegisterClient("FireFrontCannonEvent", this, &SceneManager::FireFrontCannonEvent);
	EventManager()->RegisterClient("FireBackCannonEvent", this, &SceneManager::FireBackCannonEvent);
	EventManager()->RegisterClient("AddReputationEvent", this, &SceneManager::AddReputationEvent);
	*/
}


SceneManager::~SceneManager(void)
{
	Shutdown();

	// events
	fsEventManager* fsem = fsEventManager::GetInstance();
	fsem->UnRegisterClient(fsEvents::FireLeftCannons,	this, &SceneManager::FireLeftSideCannonEvent);
	fsem->UnRegisterClient(fsEvents::FireRightCannons,	this, &SceneManager::FireRightSideCannonEvent);
	fsem->UnRegisterClient(fsEvents::FireSideCannons,	this, &SceneManager::FireBothSideCannonsEvent);
	fsem->UnRegisterClient(fsEvents::FireFrontCannon,	this, &SceneManager::FireFrontCannonEvent);
	fsem->UnRegisterClient(fsEvents::PortSale,			this, &SceneManager::PortSaleEvent);
	fsem->UnRegisterClient(fsEvents::PortSetColor,		this, &SceneManager::PortSetColorEvent);
	/*
	*/
	/*
	EventManager()->UnregisterClient("AddEvent", this, &SceneManager::AddEvent);
	EventManager()->UnregisterClient("RemoveEvent", this, &SceneManager::RemoveEvent);
	EventManager()->UnregisterClient("FireSideCannonEvent", this, &SceneManager::FireSideCannonEvent);
	EventManager()->UnregisterClient("FireFrontCannonEvent", this, &SceneManager::FireFrontCannonEvent);
	EventManager()->UnregisterClient("FireBackCannonEvent", this, &SceneManager::FireBackCannonEvent);
	EventManager()->UnregisterClient("AddReputationEvent", this, &SceneManager::AddReputationEvent);
	*/
}


bool SceneManager::Initialize(void)
{
	// create INDIVIDUAL scenes
	scenes[IScene::SceneType::INTRO] = new IntroScene();
	scenes[IScene::SceneType::MAIN_MENU] = new MainMenuScene();
	scenes[IScene::SceneType::GAMEPLAY] = new GameplayScene();
	scenes[IScene::SceneType::OPTIONS] = new OptionsMenuScene();
	scenes[IScene::SceneType::INSTRUCTIONS] = new InstructionsScene();
	scenes[IScene::SceneType::PORT] = new PortMenuScene();
	scenes[IScene::SceneType::PAUSE] = new PauseMenuScene();
	scenes[IScene::SceneType::WIN_LOSE] = new WinLoseScene();
	//scenes[IScene::SceneType::LOSE] = new LoseScene();
	scenes[IScene::SceneType::CREDITS] = new CreditsScene();


	// null check
	for (int i = 0; i < IScene::SceneType::NUM_SCENES; i++)
	{
		// allocated scene is good
		if (scenes[i] != nullptr)
		{
			// set scene manager
			scenes[i]->SetSceneManager(this);

			// initialize scene
			scenes[i]->InitializeScene();

			// next scene
			continue;
		}

		// assert: scene == null
		assert((scenes[i] != nullptr) && "SceneManager::Initialize() - a scene is NULL!");

		// return
		return false;
	}



	// store 1st scene in stack
	PushScene(IScene::SceneType::MAIN_MENU);
	PushScene(IScene::SceneType::INTRO);

	// return
	return true;
}


void SceneManager::Shutdown(void)
{
	// shutdown entity manager
	entityManager.Shutdown();


	// Empty stack
	for (;;)
	{
		// empty?
		if (scene_stack.IsEmpty() == true)
			break;

		// Keep popping!
		PopScene();
	}


	// Deallocate scenes
	for (size_t i = 0; i < IScene::SceneType::NUM_SCENES; i++)
	{
		// null check
		if (scenes[i] == nullptr)
			continue;

		// shutdown scene
		scenes[i]->ShutdownScene();

		// deallocated
		delete scenes[i];
		scenes[i] = nullptr;
	}


	// delete collision manager
	delete collisionManager;
	collisionManager = nullptr;

	fsEventManager::DeleteInstance();

}


bool SceneManager::Input(void)
{
	// empty?
	if (scene_stack.IsEmpty() == true)
		return NEW_SCENE;


	// Gamepad connection status
	AddGamepadStatus();
	RemoveGamepadStatus();


	// curr scene
	IScene* curr = GetCurrScene();

	// scene->input
	bool result = curr->Input();


	// Anything else
	// etc....


	return result;
}


void SceneManager::Update(float _dt)
{
	// empty?
	if (scene_stack.IsEmpty() == true)
		return;


	// Gamepad connection status
	if (m_fNoticeBool == true)
		m_fNoticeTimer -= _dt;



	if (currScene == IScene::SceneType::GAMEPLAY)
	{
		GameplayScene* gps = ((GameplayScene*)scenes[IScene::SceneType::GAMEPLAY]);
		gps->port_help = false;
		//gps->TogglePortInstructions(false);
	}


	// update collision manager
	collisionManager->Update();


	// update entities
	entityManager.Update(_dt, currScene);


	// scene->update
	GetCurrScene()->Update(_dt);
}


void SceneManager::Render(void)
{
	// empty?
	if (scene_stack.IsEmpty() == true)
		return;


	// curr scene
	IScene* curr = GetCurrScene();


	// scene->render
	curr->Render();
}


void SceneManager::PushScene(IScene::SceneType _st)
{
	// index check
	if (_st <= IScene::SceneType::UNKNOWN || _st >= IScene::SceneType::NUM_SCENES)
	{
		// assert: index out of range
		assert("SceneManager::PushScene() - index out of range!");
		return;
	}


	// not empty?
	if (scene_stack.IsEmpty() == false)
	{
		// top == same scene
		if (scene_stack.GetTop()->GetType() == _st)
		{
			// assert: top == new scene
			assert("SceneManager::PushScene() - scene already on top!");
			return;
		}
	}


	// clear renderables
	CGame::GetApplication()->m_pRenderer->ClearRenderables();
	//CGame::GetApplication()->m_pRenderer->SetMainCamera(nullptr);



	// end prev scene input
	if (scene_stack.IsEmpty() == false)
	{

		scene_stack.GetTop()->Input_End();
		//scene_stack.GetTop()->SwapScenePush();
	}




	// scane already in stack
	/*
	if (scene_stack.Find(scenes[_st]) == true)
	{
		return;
	}
	*/


	// add new scene
	currScene = _st;
	scene_stack.Push(scenes[_st]);

	// enter new scene
	scenes[_st]->Enter();


	// start newest scene input
	scenes[_st]->Input_Start();


}


void SceneManager::PopScene(void)
{
	// empty?
	if (scene_stack.IsEmpty() == true)
		return;


	// end old scene input
	GetCurrScene()->Input_End();


	// clear renderables
	CGame::GetApplication()->m_pRenderer->ClearRenderables();


	// exit old scene
	GetCurrScene()->Exit();


	// pop old scene
	scene_stack.Pop();


	// empty?
	if (scene_stack.IsEmpty() == true)
	{
		//CGame::GetApplication()->m_pRenderer->SetMainCamera(nullptr);
	}
	else
	{
		// newest scene
		currScene = GetCurrScene()->GetType();
		//GetCurrScene()->SwapScenePop();




		CGame::GetApplication()->m_pRenderer->SetMainCamera(GetCurrScene()->GetMainCamera()->View());
		CGame::GetApplication()->m_pRenderer->SetPostProcessFadeAmount(GetCurrScene()->GetFadeAmount());
		CGame::GetApplication()->m_pRenderer->SetUpRenderables(entityManager.GetRenderables(GetCurrScene()->GetType(), EntityManager::EntityBucket::RENDERABLE));


		// start newest scene input
		GetCurrScene()->Input_Start();
	}


}


IScene* SceneManager::GetScene(IScene::SceneType _st)
{
	// index check
	if (_st <= IScene::SceneType::UNKNOWN || _st >= IScene::SceneType::NUM_SCENES)
	{
		// assert: index out of range
		assert("SceneManager::GetScene() - index out of range!");
		return nullptr;
	}


	// null check
	if (scenes[_st] == nullptr)
	{
		// assert: scene is NULL
		assert("SceneManager::GetScene() - index out of range!");
		return nullptr;
	}


	// return
	return scenes[_st];
}


void SceneManager::ResetScene(IScene::SceneType _st)
{
	//enum SceneType { UNKNOWN = -1, INTRO = 0, MAIN_MENU, GAMEPLAY, OPTIONS, INSTRUCTIONS, PORT, PAUSE, WIN, LOSE, CREDITS, NUM_SCENES };

	if (_st > IScene::SceneType::UNKNOWN && _st < IScene::SceneType::NUM_SCENES)
	{
		scenes[_st]->ShutdownScene();
	}


	switch (_st)
	{
	case IScene::SceneType::INTRO:
		delete scenes[IScene::SceneType::INTRO];
		scenes[IScene::SceneType::INTRO] = nullptr;
		scenes[IScene::SceneType::INTRO] = new IntroScene();
		scenes[_st]->InitializeScene();
		break;
	case IScene::SceneType::MAIN_MENU:
		delete scenes[IScene::SceneType::MAIN_MENU];
		scenes[IScene::SceneType::MAIN_MENU] = nullptr;
		scenes[IScene::SceneType::MAIN_MENU] = new MainMenuScene();
		scenes[_st]->InitializeScene();
		break;
	case IScene::SceneType::GAMEPLAY:
		delete scenes[IScene::SceneType::GAMEPLAY];
		scenes[IScene::SceneType::GAMEPLAY] = nullptr;
		scenes[IScene::SceneType::GAMEPLAY] = new GameplayScene();
		scenes[_st]->InitializeScene();
		break;
	case IScene::SceneType::OPTIONS:
		delete scenes[IScene::SceneType::OPTIONS];
		scenes[IScene::SceneType::OPTIONS] = nullptr;
		scenes[IScene::SceneType::OPTIONS] = new OptionsMenuScene();
		scenes[_st]->InitializeScene();
		break;
	case IScene::SceneType::INSTRUCTIONS:
		delete scenes[IScene::SceneType::INSTRUCTIONS];
		scenes[IScene::SceneType::INSTRUCTIONS] = nullptr;
		scenes[IScene::SceneType::INSTRUCTIONS] = new InstructionsScene();
		scenes[_st]->InitializeScene();
		break;
	case IScene::SceneType::PORT:
		delete scenes[IScene::SceneType::PORT];
		scenes[IScene::SceneType::PORT] = nullptr;
		scenes[IScene::SceneType::PORT] = new PortMenuScene();
		scenes[_st]->InitializeScene();
		break;
	case IScene::SceneType::PAUSE:
		delete scenes[IScene::SceneType::PAUSE];
		scenes[IScene::SceneType::PAUSE] = nullptr;
		scenes[IScene::SceneType::PAUSE] = new PauseMenuScene();
		scenes[_st]->InitializeScene();
		break;
	case IScene::SceneType::WIN_LOSE:
		delete scenes[IScene::SceneType::WIN_LOSE];
		scenes[IScene::SceneType::WIN_LOSE] = nullptr;
		scenes[IScene::SceneType::WIN_LOSE] = new WinLoseScene();
		scenes[_st]->InitializeScene();
		break;
	//case IScene::SceneType::LOSE:
	//	delete scenes[IScene::SceneType::LOSE];
	//	scenes[IScene::SceneType::LOSE] = nullptr;
	//	scenes[IScene::SceneType::LOSE] = new LoseScene();
	//	scenes[_st]->InitializeScene();
	//	break;
	case IScene::SceneType::CREDITS:
		delete scenes[IScene::SceneType::CREDITS];
		scenes[IScene::SceneType::CREDITS] = nullptr;
		scenes[IScene::SceneType::CREDITS] = new CreditsScene();
		scenes[_st]->InitializeScene();
		break;


	case IScene::SceneType::UNKNOWN:
	case IScene::SceneType::NUM_SCENES:
		break;
	default:
		break;
	}
}


void SceneManager::AddGamepadStatus(void)
{
	// result check
	int result = ((CGame*)CGame::GetApplication())->GetGamePadConnectionStatus();	//GamepadConnectionNotice();
	if (result == 0)
		return;
	// text
	std::wstring status = (result == 1) ? L"Connected" : L"Disconnected";


	// tell scenes to draw status
	for (int i = 0; i < IScene::SceneType::NUM_SCENES; i++)
	{
		scenes[i]->DrawGamepadStatus(status);
	}


	// trigger notice
	m_fNoticeBool = true;
	m_fNoticeTimer = SCENE_NOTICE_TIME;
}


void SceneManager::RemoveGamepadStatus(void)
{
	// timer check
	if (m_fNoticeTimer > 0.0f)
		return;


	// reset notice
	m_fNoticeBool = false;
	m_fNoticeTimer = SCENE_NOTICE_TIME;


	// tell scenes NOT to draw status
	for (int i = 0; i < IScene::SceneType::NUM_SCENES; i++)
	{
		scenes[i]->DrawGamepadStatus(L"Quit");
	}
}


void SceneManager::LockInput(void)
{
	GameplayScene* gamescene = ((GameplayScene*)scenes[IScene::SceneType::GAMEPLAY]);
	static_cast<PlayerShip*>(gamescene->playerShip)->LockInput();
}


void SceneManager::UnlockInput(void)
{
	GameplayScene* gamescene = ((GameplayScene*)scenes[IScene::SceneType::GAMEPLAY]);
	static_cast<PlayerShip*>(gamescene->playerShip)->UnlockInput();
}


void SceneManager::LoadSceneHudLists( void ) {
    for ( int i = 0; i < IScene::SceneType::NUM_SCENES; i++ ) {
        if ( scene_stack.Find( scenes[i] ) == true ) {
            scenes[i]->LoadHudList();
        }
    }
}


void SceneManager::ReleaseSceneHudLists( void ) {
    for ( int i = 0; i < IScene::SceneType::NUM_SCENES; i++ ) {
        if ( scene_stack.Find( scenes[i] ) == true ) {
            scenes[i]->ReleaseHudList();
        }
    }
}


void SceneManager::GetPortHudElementNames(std::vector<std::string>& imagenames)
{
	GameplayScene* gps = ((GameplayScene*)scenes[IScene::SceneType::GAMEPLAY]);
	if (gps == nullptr)
		return;


	// get # of ports
	size_t numports = gps->ports.size();

	// no ports?
	if (numports <= 0)
		return;


	// clear outvec
	if (imagenames.empty() == false)
		imagenames.clear();


	// get minimap image names
	std::string image_prefix = "MiniMap_Port";

	for (size_t i = 0; i < numports; i++)
	{
		// get port name
		std::wstring	portname	= gps->ports[i]->GetPortName();
		int				last		= (int)portname.find_last_of(L" ");

		// spaces check
		if (last >= 0)
		{
			std::wstring	tempname	= portname;
			size_t			namelength	= tempname.size();
			std::wstring	space		= L" ";

			// clear name
			portname.clear();

			// get name without spaces
			for (size_t iter = 0; iter < namelength; iter++)
			{
				// skip spaces
				if (tempname[iter] == space[0])
					continue;

				// add non-spaces
				portname += tempname[iter];
			}
		}

		// wstring to string
		std::string		imagename	= image_prefix + AssetUtilities::ws2s(portname);
		imagenames.push_back(imagename);
	}
}


void SceneManager::SetupPortHudElementColors(std::vector<std::string> imagenames)
{
	GameplayScene* gps = ((GameplayScene*)scenes[IScene::SceneType::GAMEPLAY]);
	if (gps == nullptr)
		return;


	// get # of ports
	size_t numports = gps->ports.size();

	// no ports?
	if (numports <= 0)
		return;



	// reset colors
	SimpleMath::Color color_black	= SimpleMath::Color({ 0.0f, 0.0f, 0.0f, 1.0f });
	SimpleMath::Color color_red		= SimpleMath::Color({ 1.0f, 0.0f, 0.0f, 1.0f });

	for (size_t i = 0; i < numports; i++)
	{
		(gps->ports[i]->ActiveSale() == false)
			? static_cast<G2D::GUITexture*>(gps->m_hudElems[imagenames[i]])->Color(color_black)
			: static_cast<G2D::GUITexture*>(gps->m_hudElems[imagenames[i]])->Color(color_red);
	}
}


void SceneManager::AddEvent(const CGeneralEventArgs2<Entity3D::EntityType, Entity3D*>& args)
{
	if (currScene == IScene::SceneType::UNKNOWN || currScene == IScene::SceneType::NUM_SCENES)
		return;

	GameplayScene* gamescene = ((GameplayScene*)scenes[IScene::SceneType::GAMEPLAY]);
	if (gamescene == nullptr)
		return;
	// add entity to entity manager
	entityManager.AddEntity(GetCurrScene()->GetType(), EntityManager::EntityBucket::RENDERABLE, args.GetData2());

	// re-setup renderables
	std::vector<Renderer::CRenderable*> renderList;
	renderList.push_back(args.GetData2()->GetRenderInfo());
	CGame::GetApplication()->m_pRenderer->SetUpRenderables(renderList);
	//CGame::GetApplication()->m_pRenderer->ClearRenderables();
	//CGame::GetApplication()->m_pRenderer->SetUpRenderables(entityManager.GetRenderables(GetCurrScene()->GetType(), EntityManager::EntityBucket::RENDERABLE));



	if (GetCurrScene()->GetType() != IScene::SceneType::GAMEPLAY)
		return;


	//GameplayScene* gamescene = ((GameplayScene*)scenes[IScene::SceneType::GAMEPLAY]);
	/*
	if (currScene == IScene::SceneType::UNKNOWN || currScene == IScene::SceneType::NUM_SCENES)
		return;

	GameplayScene* gamescene = ((GameplayScene*)scenes[IScene::SceneType::GAMEPLAY]);
	if (gamescene == nullptr)
		return;

	//if (currScene != IScene::SceneType::GAMEPLAY)
	//	return;
	*/



	// save entity in gamplay vector
	/*
	Ship*					ship	= nullptr;
	Crate*					crate	= nullptr;
	IAmmo*					ammo	= nullptr;
	Weather*				weather	= nullptr;
	*/


	switch (args.GetData())
	{
	case Entity3D::EntityType::SHIP:
		gamescene->aiShips.push_back((Ship*)args.GetData2());
		/*
		ship = (Ship*)args.GetData2();
		switch (ship->GetAlignment())
		{
		case Ship::eAlignment::ENEMY:
		case Ship::eAlignment::OTHER:
			gamescene->aiShips.push_back(args.GetData2());
			break;

		case Ship::eAlignment::PLAYER:
		case Ship::eAlignment::UNKNOWN:
		default:
			break;
		}
		*/
		break;

	case Entity3D::EntityType::CRATE:
		gamescene->crates.push_back((Crate*)args.GetData2());
		break;

	case Entity3D::EntityType::AMMO:
		gamescene->ammo.push_back((IAmmo*)args.GetData2());
		break;

	case Entity3D::EntityType::LAND:
		gamescene->islands.push_back(args.GetData2());
		break;

	case Entity3D::EntityType::PORT:
		gamescene->ports.push_back((Port*)args.GetData2());
		break;

	case Entity3D::EntityType::WEATHER:
		gamescene->weather.push_back(args.GetData2());
		break;

	case Entity3D::EntityType::UNKNOWN:
	case Entity3D::EntityType::SHIP_PART:
	default:
		break;
	}





	// get renderables
	//std::vector<Renderer::CRenderable*> renderList = entityManager().GetRenderables(GetType(), EntityManager::EntityBucket::RENDERABLE);

	// add renderables
	//CGame::GetApplication()->m_pRenderer->SetUpRenderables(renderList);





}


void SceneManager::RemoveEvent(const CGeneralEventArgs<Entity3D*>& args)
{
	// find entity
	IScene::SceneType			scene = IScene::SceneType::UNKNOWN;
	EntityManager::EntityBucket	bucket = EntityManager::EntityBucket::UNKNOWN;
	int							index = entityManager.FindEntity(scene, bucket, args.GetData());

	// not current scene?
	if (scene != currScene)
		return;

	// not found?
	if (index == -1)
		return;

	// remove entity from entity manager
	args.GetData()->SetToBeDeleted(true);
}


void SceneManager::FireLeftSideCannonEvent(const EventArgs1<FireCannonEventArgs>& args)
{
	/*
	GameplayScene* gamescene = ((GameplayScene*)scenes[IScene::SceneType::GAMEPLAY]);

	// pool check?
	unsigned int pool_used = gamescene->GetAmmoPopulation();
	if (pool_used + 2 >= AMMO_LIMIT)
		return;


	// check out earliast 2 bits to use
	int e1 = gamescene->GetEarliestOffBitIndex();
	gamescene->SetAmmoIndex((unsigned int)e1, true);
	int e2 = gamescene->GetEarliestOffBitIndex();
	gamescene->SetAmmoIndex((unsigned int)e2, true);
	*/

	// get ammo based on ammo pool
	FireCannonEventArgs	ev_args = args.m_Data;
	IAmmo*				ammo1 = nullptr;			//gamescene->ammopool[e1];
	XMFLOAT4X4			localpos = ev_args.owner->GetLocalMatrix();
	XMFLOAT3			offset = ev_args.posOffset;
	float				lifespan = ev_args.lifespan;
	int					damage = ev_args.damage;
	Ship*				owner = ev_args.owner;
	IAmmo::eAmmoType	ammotype = ev_args.type;


	// create side cannon ammo
	switch (ev_args.type)
	{
	case IAmmo::eAmmoType::CANNONBALL:
		ammo1 = entityManager.CreateCannonball(ev_args.owner);
		break;

	case IAmmo::eAmmoType::CHAIN_SHOT:
		ammo1 = entityManager.CreateChainShot(ev_args.owner);
		break;

	case IAmmo::eAmmoType::HARPOON:
	case IAmmo::eAmmoType::UNKNOWN:
	default:
		break;
	}


	// Velocity of the boat
	XMFLOAT3 BoatVeloctiy;
	forwardAxisOf(owner->GetWorldMatrix(), BoatVeloctiy);
	BoatVeloctiy *= owner->GetSpeed();

	// Set position
	ammo1->GetTransform().SetLocalMatrix(localpos);
	ammo1->GetTransform().Translate(XMFLOAT3(-offset.x, offset.y, offset.z), true);
	ammo1->Offset(XMFLOAT3(-offset.x, offset.y, offset.z));

	// Set Velocity
	// Direction from the right side of boat
	XMFLOAT3 PortSideVelocity;
	rightAxisOf(owner->GetWorldMatrix(), PortSideVelocity);
	PortSideVelocity *= -1;
	// Move into velocity
	PortSideVelocity *= ammo1->GetSpeed() * (1 + lifespan);
	// Get final velocity
	XMFLOAT3 FinalVelocity = PortSideVelocity + BoatVeloctiy;
	ammo1->SetVelocity(FinalVelocity);

	// set lifespan and damage then add event
	ammo1->SetLifespan(lifespan);
	ammo1->SetDamage(damage);
	AddEvent(CGeneralEventArgs2<Entity3D::EntityType, Entity3D*>(ammo1->GetType(), ammo1));
}


void SceneManager::FireRightSideCannonEvent(const EventArgs1<FireCannonEventArgs>& args)
{
	/*
	GameplayScene* gamescene = ((GameplayScene*)scenes[IScene::SceneType::GAMEPLAY]);

	// pool check?
	unsigned int pool_used = gamescene->GetAmmoPopulation();
	if (pool_used + 2 >= AMMO_LIMIT)
		return;


	// check out earliast 2 bits to use
	int e1 = gamescene->GetEarliestOffBitIndex();
	gamescene->SetAmmoIndex((unsigned int)e1, true);
	int e2 = gamescene->GetEarliestOffBitIndex();
	gamescene->SetAmmoIndex((unsigned int)e2, true);
	*/

	// get ammo based on ammo pool
	FireCannonEventArgs	ev_args = args.m_Data;
	IAmmo*				ammo2 = nullptr;			//gamescene->ammopool[e2];
	XMFLOAT4X4			localpos = ev_args.owner->GetLocalMatrix();
	XMFLOAT3			offset = ev_args.posOffset;
	float				lifespan = ev_args.lifespan;
	int					damage = ev_args.damage;
	Ship*				owner = ev_args.owner;
	IAmmo::eAmmoType	ammotype = ev_args.type;


	// create side cannon ammo
	switch (ev_args.type)
	{
	case IAmmo::eAmmoType::CANNONBALL:
		ammo2 = entityManager.CreateCannonball(ev_args.owner);
		break;

	case IAmmo::eAmmoType::CHAIN_SHOT:
		ammo2 = entityManager.CreateChainShot(ev_args.owner);
		break;

	case IAmmo::eAmmoType::HARPOON:
	case IAmmo::eAmmoType::UNKNOWN:
	default:
		break;
	}


	// Velocity of the boat
	XMFLOAT3 BoatVeloctiy;
	forwardAxisOf(owner->GetWorldMatrix(), BoatVeloctiy);
	BoatVeloctiy *= owner->GetSpeed();

	// Set position
	ammo2->GetTransform().SetLocalMatrix(localpos);
	ammo2->GetTransform().Translate(XMFLOAT3(offset.x, offset.y, offset.z), true);
	ammo2->Offset(XMFLOAT3(offset.x, offset.y, offset.z));

	// Set Velocity
	// Direction from the right side of boat
	XMFLOAT3 StarboardSideVelocity;
	rightAxisOf(owner->GetWorldMatrix(), StarboardSideVelocity);
	// Move into velocity
	StarboardSideVelocity *= ammo2->GetSpeed() * (1 + lifespan);
	// Get final velocity
	XMFLOAT3 FinalVelocity = StarboardSideVelocity + BoatVeloctiy;
	ammo2->SetVelocity(FinalVelocity);

	// set lifespan and damage then add event
	ammo2->SetLifespan(lifespan);
	ammo2->SetDamage(damage);
	AddEvent(CGeneralEventArgs2<Entity3D::EntityType, Entity3D*>(ammo2->GetType(), ammo2));
}


void SceneManager::FireBothSideCannonsEvent(const EventArgs1<FireCannonEventArgs>& args)
{
	FireLeftSideCannonEvent(args);
	FireRightSideCannonEvent(args);


	/*
	GameplayScene* gamescene = ((GameplayScene*)scenes[IScene::SceneType::GAMEPLAY]);

	// pool check?
	unsigned int pool_used = gamescene->GetAmmoPopulation();
	if (pool_used + 2 >= AMMO_LIMIT)
		return;


	// check out earliast 2 bits to use
	int e1 = gamescene->GetEarliestOffBitIndex();
	gamescene->SetAmmoIndex((unsigned int)e1, true);
	int e2 = gamescene->GetEarliestOffBitIndex();
	gamescene->SetAmmoIndex((unsigned int)e2, true);
	*/
	/*
	// get ammo based on ammo pool
	FireCannonEventArgs		ev_args		= args.GetData();
	IAmmo*					ammo1		= nullptr;			//gamescene->ammopool[e1];
	IAmmo*					ammo2		= nullptr;			//gamescene->ammopool[e2];
	XMFLOAT4X4				localpos	= ev_args.owner->GetLocalMatrix();
	XMFLOAT3				offset		= ev_args.posOffset;
	float					lifespan	= ev_args.lifespan;
	int						damage		= ev_args.damage;
	Ship*					owner		= ev_args.owner;
	IAmmo::eAmmoType		ammotype	= ev_args.type;


	// create side cannon ammo
	switch (ev_args.type)
	{
	case IAmmo::eAmmoType::CANNONBALL:
		ammo1 = entityManager.CreateCannonball(ev_args.owner);
		ammo2 = entityManager.CreateCannonball(ev_args.owner);
		break;

	case IAmmo::eAmmoType::CHAIN_SHOT:
		ammo1 = entityManager.CreateChainShot(ev_args.owner);
		ammo2 = entityManager.CreateChainShot(ev_args.owner);
		break;

	case IAmmo::eAmmoType::HARPOON:
	case IAmmo::eAmmoType::UNKNOWN:
	default:
		break;
	}


	// Velocity of the boat
	XMFLOAT3 BoatVeloctiy;
	forwardAxisOf(owner->GetWorldMatrix(), BoatVeloctiy);
	BoatVeloctiy *= owner->GetController()->GetSpeed();
	XMFLOAT3 FinalVelocity;


	// Set position
	ammo1->GetTransform().SetLocalMatrix(localpos);
	ammo1->GetTransform().Translate(XMFLOAT3(-offset.x, offset.y, offset.z), true);

	// Set Velocity
	// Direction from the right side of boat
	XMFLOAT3 PortSideVelocity;
	rightAxisOf(owner->GetWorldMatrix(), PortSideVelocity);
	PortSideVelocity *= -1;
	// Move into velocity
	PortSideVelocity *= ammo1->GetSpeed();
	// Get final velocity
	FinalVelocity = PortSideVelocity + BoatVeloctiy;
	ammo1->SetVelocity(FinalVelocity);

	// set lifespan and damage then add event
	ammo1->SetLifespan(lifespan);
	ammo1->SetDamage(damage);
	AddEvent(CGeneralEventArgs2<Entity3D::EntityType, Entity3D*>(ammo1->GetType(), ammo1));



	// Set position
	ammo2->GetTransform().SetLocalMatrix(localpos);
	ammo2->GetTransform().Translate(XMFLOAT3(-offset.x, offset.y, offset.z), true);

	// Set Velocity
	// Direction from the right side of boat
	XMFLOAT3 StarboardSideVelocity;
	rightAxisOf(owner->GetWorldMatrix(), StarboardSideVelocity);
	// Move into velocity
	StarboardSideVelocity *= ammo2->GetSpeed();
	// Get final velocity
	FinalVelocity = StarboardSideVelocity + BoatVeloctiy;
	ammo2->SetVelocity(FinalVelocity);

	// set lifespan and damage then add event
	ammo2->SetLifespan(lifespan);
	ammo2->SetDamage(damage);
	AddEvent(CGeneralEventArgs2<Entity3D::EntityType, Entity3D*>(ammo2->GetType(), ammo2));
	*/
}


void SceneManager::FireFrontCannonEvent(const EventArgs1<FireCannonEventArgs>& args)
{
	FireCannonEventArgs	ev_args = args.m_Data;
	IAmmo*				ammo = nullptr;
	XMFLOAT4X4			localpos = ev_args.owner->GetLocalMatrix();
	XMFLOAT3			offset = ev_args.posOffset;
	float				lifespan = ev_args.lifespan;
	int					damage = ev_args.damage;
	Ship*				owner = ev_args.owner;


	// create side cannon ammo
	switch (ev_args.type)
	{
	case IAmmo::eAmmoType::CANNONBALL:
		ammo = entityManager.CreateCannonball(ev_args.owner);
		break;

	case IAmmo::eAmmoType::CHAIN_SHOT:
		ammo = entityManager.CreateChainShot(ev_args.owner);
		break;

	case IAmmo::eAmmoType::HARPOON:
	case IAmmo::eAmmoType::UNKNOWN:
	default:
		break;
	}


	// fill out ammo data
	ammo->GetTransform().SetLocalMatrix(localpos);
	ammo->GetTransform().Translate(XMFLOAT3(-offset.x, offset.y, offset.z), true);
	ammo->Offset(XMFLOAT3(-offset.x, offset.y, offset.z));

	// Set Velocity
	XMFLOAT3 BoatVeloctiy;
	forwardAxisOf(owner->GetWorldMatrix(), BoatVeloctiy);

	XMFLOAT3 CannonballVelocity = BoatVeloctiy;
	// Move into velocity
	BoatVeloctiy *= owner->GetSpeed();
	CannonballVelocity *= ammo->GetSpeed() * (1 + lifespan);

	// Get final velocity
	XMFLOAT3 FinalVelocity = CannonballVelocity + BoatVeloctiy;
	ammo->SetVelocity(FinalVelocity);

	ammo->SetLifespan(lifespan);
	ammo->SetDamage(damage);
	//ammo->AddParticles("Cannon Ball");


	// add ammo
	AddEvent(CGeneralEventArgs2<Entity3D::EntityType, Entity3D*>(ammo->GetType(), ammo));
}


void SceneManager::FireBackCannonEvent(const EventArgs1<FireCannonEventArgs>& args)
{
	FireCannonEventArgs	ev_args = args.m_Data;
	IAmmo*				ammo = nullptr;
	XMFLOAT4X4			localpos = ev_args.owner->GetLocalMatrix();
	XMFLOAT3			offset = ev_args.posOffset;
	float				lifespan = ev_args.lifespan;
	int					damage = ev_args.damage;


	// create side cannon ammo
	switch (ev_args.type)
	{
	case IAmmo::eAmmoType::CANNONBALL:
		ammo = entityManager.CreateCannonball(ev_args.owner);
		break;

	case IAmmo::eAmmoType::CHAIN_SHOT:
		ammo = entityManager.CreateChainShot(ev_args.owner);
		break;

	case IAmmo::eAmmoType::HARPOON:
		ammo = entityManager.CreateHarpoon(ev_args.owner);
		break;

	case IAmmo::eAmmoType::UNKNOWN:
	default:
		break;
	}


	// fill out ammo data
	ammo->GetTransform().SetLocalMatrix(localpos);
	ammo->GetTransform().GetWorldMatrix();
	ammo->GetTransform().Translate(XMFLOAT3(-offset.x, offset.y, offset.z), true);
	ammo->GetTransform().GetWorldMatrix();
	//ammo->SetDirection(XMFLOAT3(0.0f, 0.0f, -1.0f));
	ammo->SetLifespan(lifespan);
	ammo->SetDamage(damage);
	//ammo->AddParticles("Cannon Ball", 1);



	// add ammo
	AddEvent(CGeneralEventArgs2<Entity3D::EntityType, Entity3D*>(ammo->GetType(), ammo));
}


void SceneManager::AddReputationEvent(const EventArgs1<float>& args)
{
	// add rep to player
	//((GameplayScene*)scenes[IScene::SceneType::GAMEPLAY])->playerShip->AddReputation(args.GetData() * 5.0f);
}


void SceneManager::SpawnCrate(const EventArgs1<SpawnCrateEventArgs>& args)
{
	SpawnCrateEventArgs		ev_args = args.m_Data;
	Crate*					crate = nullptr;
	Ship*					player = ev_args.playerShip;
	Ship*					enemy = ev_args.enemyShip;
	XMFLOAT4X4				playerlocmat = player->GetLocalMatrix();
	XMFLOAT4X4				enemylocmat = enemy->GetLocalMatrix();
	XMFLOAT3				playerpos = XMFLOAT3(0.0f, 0.0f, 0.0f);
	XMFLOAT3				enemypos = XMFLOAT3(0.0f, 0.0f, 0.0f);
	positionOf(player->GetWorldMatrix(), playerpos);
	positionOf(enemy->GetWorldMatrix(), enemypos);
	XMFLOAT3				direction = playerpos - enemypos;
	DirectX::normalizeFloat3(direction);


	// create crate
	crate = (Crate*)entityManager.CreateEntity3D("Crate", enemylocmat);


	// fill out crate data
	crate->SetRum(static_cast<int>(enemy->GetCurrRum() * 0.5f));
	crate->SetWaterFood(static_cast<int>(enemy->GetCurrWaterFood() * 0.5f));
	crate->SetGold(static_cast<int>(enemy->GetCurrGold() * 0.5f));
	crate->SetDirection(direction);



	// add ammo
	AddEvent(CGeneralEventArgs2<Entity3D::EntityType, Entity3D*>(Entity3D::EntityType::CRATE, crate));
}


void SceneManager::PortSaleEvent(const EventArgs& args)
{
	GameplayScene* gps = ((GameplayScene*)scenes[IScene::SceneType::GAMEPLAY]);
	if (gps == nullptr)
		return;


	// get # of ports
	size_t numports = gps->ports.size();

	// no ports?
	if (numports <= 0)
		return;



	// get minimap image names
	/*
	std::string image_prefix = "MiniMap_Port";

	for (size_t i = 0; i < numports; i++)
	{
		// get port name
		std::wstring	portname	= gps->ports[i]->GetPortName();
		int				last		= (int)portname.find_last_of(L" ");

		// spaces check
		if (last >= 0)
		{
			std::wstring	tempname	= portname;
			size_t			namelength	= tempname.size();
			std::wstring	space		= L" ";

			// clear name
			portname.clear();

			// get name without spaces
			for (size_t iter = 0; iter < namelength; iter++)
			{
				// skip spaces
				if (tempname[iter] == space[0])
					continue;

				// add non-spaces
				portname += tempname[iter];
			}
		}

		// wstring to string
		std::string		imagename	= image_prefix + AssetUtilities::ws2s(portname);
		imagenames.push_back(imagename);
	}
	*/
	std::vector<std::string> imagenames;
	GetPortHudElementNames(imagenames);



	// reset colors: white = { 0,0,0 }, black = { 1,1,1 }
	/*
	SimpleMath::Color color_black	= SimpleMath::Color({ 0.0f, 0.0f, 0.0f, 1.0f });
	SimpleMath::Color color_red		= SimpleMath::Color({ 1.0f, 0.0f, 0.0f, 1.0f });

	for (size_t i = 0; i < numports; i++)
	{
		(gps->ports[i]->ActiveSale() == false)
			? static_cast<G2D::GUITexture*>(gps->m_hudElems[imagenames[i]])->Color(color_black)
			: static_cast<G2D::GUITexture*>(gps->m_hudElems[imagenames[i]])->Color(color_red);
	}
	*/
	SetupPortHudElementColors(imagenames);



	// pick which port is having a sale
	SimpleMath::Color	color_red	= SimpleMath::Color({ 1.0f, 0.0f, 0.0f, 1.0f });
	int					sale		= rand() % numports;

	for (;;)
	{
		// OMIT Tortuga
		//while (gps->ports[sale]->GetPortName() == L"Tortuga")
		//	sale = rand() % numports;

		// already having a sale
		if (gps->ports[sale]->ActiveSale() == false)
		{
			// start sale
			gps->ports[sale]->ActivateSale();

			// activate sale color on minimap icon (red)
			static_cast<G2D::GUITexture*>(gps->m_hudElems[imagenames[sale]])->Color(color_red);
			break;
		}

		// rechoose port
		int next = rand() % numports;

		// next port is same port?
		while (next == sale)
			next = rand() % numports;

		// next port
		sale = next;
	}


	// set gameplay port time
	gps->m_fPortSaleTimer = gps->ports[sale]->GetSaleTimer();
}


void SceneManager::PortSetColorEvent(const EventArgs& args)
{
	GameplayScene* gps = ((GameplayScene*)scenes[IScene::SceneType::GAMEPLAY]);
	if (gps == nullptr)
		return;


	// get # of ports
	size_t numports = gps->ports.size();

	// no ports?
	if (numports <= 0)
		return;



	// get minimap image names
	std::vector<std::string> imagenames;
	GetPortHudElementNames(imagenames);



	// reset colors
	SetupPortHudElementColors(imagenames);
}