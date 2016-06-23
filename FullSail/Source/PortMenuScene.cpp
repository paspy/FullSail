#include "pch.h"
#include "PortMenuScene.h"
#include "EntityManager.h"
#include "SceneManager.h"
#include "Game.h"
#include "GameplayScene.h"
#include "Ship.h"
#include "PlayerShip.h"
#include "SideCannonHandler.h"
#include "Hull.h"
#include "Mast.h"
#include "PlayerController.h"
#include "Port.h"
#include "GUIAnimation.h"
#include "Colliders.h"


PortMenuScene::PortMenuScene() : IScene()
{
	// scene type
	sceneType = SceneType::PORT;

	currPage		= 0;
	numPagesOpen	= 1;
	currCursor		= 0;
	scrollTimer		= SCENE_SCROLL_TIME;
	scrollStuck		= false;
	portName		= L"";

    LoadHudList();

	baseprices[PortItems::WATER_FOOD]	= 10;	//100;
	baseprices[PortItems::RUM]			= 10;	//100;
	baseprices[PortItems::CREW]			= 10;	//100;
	baseprices[PortItems::REPAIR]		= 100;
	baseprices[PortItems::WEAPONS]		= 200;
	baseprices[PortItems::SAILS]		= 200;
	baseprices[PortItems::HULL]			= 200;
	baseprices[PortItems::FLAMETHROWER]	= 9999999;	//300;
	baseprices[PortItems::BARRELS]		= 9999999;	//300;
	baseprices[PortItems::GREEKSHOT]	= 9999999;	//300;
	baseprices[PortItems::FLEET]		= 9999999;	//400;

	ResetPrices();

	priceOffsets[0] = 200;      //$400
	priceOffsets[1] = 200;      //$600
	priceOffsets[2] = 200;      //$800
	priceOffsets[3] = 200;      //$1000
	priceOffsets[4] = -1000;    //$0

	onsale = false;
	for (size_t i = 0; i < 11; i++)
		discounts[i] = 0;


	resourceIncrements[0] = 2;
	resourceIncrements[1] = 2;
	resourceIncrements[2] = 0;

	incrementNums[0] = 1;
	incrementNums[1] = 5;
	incrementNums[2] = 10;
	incrementNums[3] = 50;
	incrementNums[3] = 100;

	// events
	fsEventManager* fsem = fsEventManager::GetInstance();
	fsem->RegisterClient(fsEvents::PortSetup,		this, &PortMenuScene::PortSetupEvent);
	fsem->RegisterClient(fsEvents::PortCursorUp,	this, &PortMenuScene::CursorDecreaseEvent);
	fsem->RegisterClient(fsEvents::PortCursorDown,	this, &PortMenuScene::CursorIncreaseEvent);
	fsem->RegisterClient(fsEvents::PortCursorRight,	this, &PortMenuScene::CursorIncrementEvent);
	fsem->RegisterClient(fsEvents::PortCursorLeft,	this, &PortMenuScene::CursorDecrementEvent);
}


PortMenuScene::~PortMenuScene()
{
	// events
	fsEventManager* fsem = fsEventManager::GetInstance();
	fsem->UnRegisterClient(fsEvents::PortSetup,			this, &PortMenuScene::PortSetupEvent);
	fsem->UnRegisterClient(fsEvents::PortCursorUp,		this, &PortMenuScene::CursorIncreaseEvent);
	fsem->UnRegisterClient(fsEvents::PortCursorDown,	this, &PortMenuScene::CursorDecreaseEvent);
	fsem->UnRegisterClient(fsEvents::PortCursorRight,	this, &PortMenuScene::CursorIncrementEvent);
	fsem->UnRegisterClient(fsEvents::PortCursorLeft,	this, &PortMenuScene::CursorDecrementEvent);

    ReleaseHudList();

}


void PortMenuScene::Enter(void)
{
	CGame*			game = (CGame*)CGame::GetApplication();
	SoundManager*	sound = game->GetSoundManager();
	//GameplayScene*	gps = (GameplayScene*)game->GetSceneManager()->GetScene(IScene::SceneType::GAMEPLAY);


	// base enter
	IScene::Enter();

	// reset page & cursor
	currPage = 0;
	currCursor = 0;

	m_hudElems["PortMenu_newbg"]->Active(true);
	//m_hudElems["PortMenu1_bg_1920x1080"]->Active(true);
	//m_hudElems["PortMenu2_bg_1920x1080"]->Active(false);
	//m_hudElems["PortMenu3_bg_1920x1080"]->Active(false);

	// play port music
	sound->Play3DSound(AK::EVENTS::PLAYPORTBGM);


	///* DISCOUNTS NOT NEEDED DUE TO INCREMENTIAL BUYING - DO NOT ERASE!!!*/
	/*
	// re-calculate prices for changing stuff - DO NOT ERASE!!!

	MAX_HP	= 100
	CURR_HP	= 75
	$		= $200

	Dsc%	= (CURR_HP/MAX_HP)
			= (75/100)
			= (3/4)

	Dsc$	= $ * Dsc%
			= 200 * (3/4)
			= 150

	new$	= $ - Dsc$
			= 200 - 150
			= 50
	*/
	/*
	int		std_price		= 100;
	float	discounts[4]	= {};
	int		disc_price[4]	= {};

	// discount %s
	discounts[0] = static_cast<float>(gps->playerShip->GetHealth())			/ static_cast<float>(gps->playerShip->GetMaxHealth());
	discounts[1] = static_cast<float>(gps->playerShip->GetCurrWaterFood())	/ static_cast<float>(gps->playerShip->GetMaxWaterFood());
	discounts[2] = static_cast<float>(gps->playerShip->GetCurrRum())		/ static_cast<float>(gps->playerShip->GetMaxRum());
	discounts[3] = static_cast<float>(gps->playerShip->GetCurrCrew())		/ static_cast<float>(gps->playerShip->GetMaxCrew());

	// discount $s
	for (size_t i = 0; i < 4; i++)
		disc_price[i] = static_cast<int>(std_price * discounts[i]);

	// new $s
	prices[PortItems::REPAIR]		= std_price - disc_price[0];
	prices[PortItems::WATER_FOOD]	= std_price - disc_price[1];
	prices[PortItems::RUM]			= std_price - disc_price[2];
	prices[PortItems::CREW]			= std_price - disc_price[3];
	*/

	// fade
	m_FadeIn = true;
	m_Timer = 2.0f;	//FADEINTIMER;
	m_FadeAmount = 0.0f;
	CGame::GetApplication()->m_pRenderer->SetPostProcessFadeAmount(m_FadeAmount);

	for (auto i = m_hudElems.begin(); i != m_hudElems.end(); i++)
	{
		if ((*i).second->Type() == G2D::Entity2D::eTYPE_GUITexture)
		{
			auto color = static_cast<G2D::GUITexture*>((*i).second)->Color();
			static_cast<G2D::GUITexture*>((*i).second)->Color(color * 0.0f);
		}
	}
}


void PortMenuScene::Exit(void)
{
	CGame*			game = (CGame*)CGame::GetApplication();
	SoundManager*	sound = game->GetSoundManager();
	GameplayScene*	gps = (GameplayScene*)game->GetSceneManager()->GetScene(IScene::SceneType::GAMEPLAY);


	// stop port music
	sound->Play3DSound(AK::EVENTS::STOPPORTBGM);

	// port exit bell
	sound->Play3DSound(AK::EVENTS::PLAYPORTBELLS);

	// resume gameplay bgm
	gps->ResumeSounds();


	// cancel fade
	if (m_FadeIn == true)
	{
		m_Timer = FADEINTIMER;
		m_FadeIn = false;
		CGame::GetApplication()->m_pRenderer->SetPostProcessFadeAmount(0.0f);
	}

	// base exit
	IScene::Exit();
}


bool PortMenuScene::Input(void)
{
	//if (m_FadeIn == true)
	//	return OLD_SCENE;

	CGame* game = (CGame*)CGame::GetApplication();
	return (GameSetting::GetRef().GamePadIsConnected()) ? Input_Gamepad() : Input_Keyboard();
}


void PortMenuScene::Update(float _dt)
{
	// base update
	IScene::Update(_dt);

	// etc....

	// scroll timer
	if (scrollTimer > 0.0f)
		scrollTimer -= _dt;
	if (scrollTimer < 0.0f)
		scrollTimer = 0.0f;

	// resets active status
	DeactivateToggleableGUI();

	// reset colors
	static_cast<G2D::GUIText*>(m_hudElems["FoodNum"])->Color({ 0.0f, 0.0f, 0.0f, 1.0f });
	static_cast<G2D::GUIText*>(m_hudElems["CrewNum"])->Color({ 0.0f, 0.0f, 0.0f, 1.0f });
	static_cast<G2D::GUIText*>(m_hudElems["RumNum"])->Color({ 0.0f, 0.0f, 0.0f, 1.0f });

	// input type
	bool is_connected = GameSetting::GetRef().GamePadIsConnected();
	if (is_connected == true)
	{
		m_hudElems["xbox_lb"]->Active(true);
		m_hudElems["xbox_rb"]->Active(true);
	}
	else
	{
		m_hudElems["PageIncInput"]->Active(true);
	}




	// port name
	static_cast<G2D::GUIText*>(m_hudElems["PortName"])->Text(portName);
	//static_cast<G2D::GUIText*>(m_hudElems["PortName"])->Color({ 0,0,0,1 });


	// discount icon
	if (onsale == true)
		m_hudElems["DiscountIcon"]->Active(true);



	// show curr resources
	GameplayScene* gps = ((GameplayScene*)sceneManager->GetScene(IScene::SceneType::GAMEPLAY));
	static_cast<G2D::GUIText*>(m_hudElems["GoldNum"])->Text(std::to_wstring(gps->playerShip->GetCurrGold()) + L"g");
	static_cast<G2D::GUIText*>(m_hudElems["FoodNum"])->Text(std::to_wstring(gps->playerShip->GetCurrWaterFood()) + L"/" + std::to_wstring(gps->playerShip->GetMaxWaterFood()));
	static_cast<G2D::GUIText*>(m_hudElems["CrewNum"])->Text(std::to_wstring(gps->playerShip->GetCurrCrew()) + L"/" + std::to_wstring(gps->playerShip->GetMaxCrew()));
	static_cast<G2D::GUIText*>(m_hudElems["RumNum"])->Text(std::to_wstring(gps->playerShip->GetCurrRum()) + L"/" + std::to_wstring(gps->playerShip->GetMaxRum()));


	// page#
	static_cast<G2D::GUIText*>(m_hudElems["PageNum"])->Text(L"Pg: " + std::to_wstring(currPage + 1) + L"/" + std::to_wstring(numPagesOpen));


	// items & prices
	//std::wstring spaces = L"                                         ";

	switch (currPage)
	{
		// page1
	case 0:
		ActivatePage1GUI();
		break;

		// page2
	case 1:
		ActivatePage2GUI();
		break;

		// page3
	case 2:
		ActivatePage3GUI();
		break;


		// page4
	case 3:
		//m_hudElems["Fleet"]->Active(true);
		//static_cast<G2D::GUIText*>(m_hudElems["Fleet"])->Text(std::to_wstring(prices[FLEET]) + L"g");
		//break;
	default:
		break;
	}

	// items & prices
	//static_cast<G2D::GUIText*>(m_hudElems["Weapon"])->Text(
	//	L"Weapon  Lv. " + std::to_wstring(gps->playerShip->GetSideCannonHandler()->GetLevel()) + L"/5   $" + std::to_wstring(prices[WEAPONS])
	//	);
	//static_cast<G2D::GUIText*>(m_hudElems["Hull"])->Text(
	//	L"Hull  Lv. " + std::to_wstring(gps->playerShip->GetHullLevel()) + L"/5   $" + std::to_wstring(prices[HULL])
	//	);
	//static_cast<G2D::GUIText*>(m_hudElems["Sail"])->Text(
	//	L"Sail  Lv. " + std::to_wstring(gps->playerShip->GetMastLevel()) + L"/5   $" + std::to_wstring(prices[SAILS])
	//	);
	//static_cast<G2D::GUIText*>(m_hudElems["Food"])->Text(
	//	L"Food   $" + std::to_wstring(prices[WATER_FOOD])
	//	);
	//static_cast<G2D::GUIText*>(m_hudElems["Rum"])->Text(
	//	L"Rum   $" + std::to_wstring(prices[RUM])
	//	);
	//static_cast<G2D::GUIText*>(m_hudElems["Crew"])->Text(
	//	L"Crew   $" + std::to_wstring(prices[CREW])
	//	);
	//static_cast<G2D::GUIText*>(m_hudElems["Repair"])->Text(
	//	L"Repair   $" + std::to_wstring(prices[REPAIR])
	//	);



	// cursor
	auto cursorHud = static_cast<G2D::GUITexture*>(m_hudElems["PortMenu_Reticle"]);	//static_cast<G2D::GUITexture*>(m_hudElems["PortMenu_cursor_1920x1080"]);


	// cursor position
	PortItems	curritem	= GetCurrPortItem();
	std::string	posstring	= "";
	switch (curritem)
	{
	case PortItems::WATER_FOOD:
		posstring = "Food";
		break;
	case PortItems::RUM:
		posstring = "Rum";
		break;
	case PortItems::CREW:
		posstring = "Crew";
		break;
	case PortItems::REPAIR:
		posstring = "Repair";
		break;
	case PortItems::WEAPONS:
		posstring = "Weapons";
		break;
	case PortItems::HULL:
		posstring = "Hull";
		break;
	case PortItems::SAILS:
		posstring = "Sails";
		break;
	case PortItems::FLAMETHROWER:
		posstring = "Flamethrower";
		break;
	case PortItems::BARRELS:
		posstring = "Barrels";
		break;
	case PortItems::GREEKSHOT:
		posstring = "Greekshot";
		break;
	case PortItems::FLEET:
		posstring = "Fleet";
		break;
	case PortItems::EXIT:
		posstring = "Exit" + std::to_string(currPage + 1);
		break;
	case PortItems::UNKNOWN:
	default:
		break;
	}

	float2 position = cursorHud->StaticPostions()[posstring];
	cursorHud->Position(position);


	//if (curritem < PortItems::EXIT && curritem != PortItems::REPAIR)
	if (curritem < PortItems::EXIT)
	{
		m_hudAnims.Animation("GUIAnim_ChestOpen")->ShowAnimation();
		//m_hudAnims.Animation("GUIAnim_ChestOpen")->PlayAnimation();
		//m_hudElems["anim_chest_open_1"]->Active(true);
		//m_hudElems["anim_chest_open_2"]->Active(true);
		//m_hudElems["anim_chest_open_3"]->Active(true);

		static_cast<G2D::GUITexture*>(m_hudElems["anim_chest_open_1"])->Position(static_cast<G2D::GUITexture*>(m_hudElems["anim_chest_open_1"])->StaticPostions()[posstring]);
		static_cast<G2D::GUITexture*>(m_hudElems["anim_chest_open_2"])->Position(static_cast<G2D::GUITexture*>(m_hudElems["anim_chest_open_2"])->StaticPostions()[posstring]);
		static_cast<G2D::GUITexture*>(m_hudElems["anim_chest_open_3"])->Position(static_cast<G2D::GUITexture*>(m_hudElems["anim_chest_open_3"])->StaticPostions()[posstring]);
	}

	//float2 position = float2(0.0f, 0.0f);
	//switch (currCursor)
	//{
	//case PortItems::WEAPONS:
	//case PortItems::HULL:
	//case PortItems::SAILS:
	//	position.x = cursorHud->StaticPostions()["Left"].x;
	//	position.y = cursorHud->StaticPostions()["Left"].y + (float)currCursor * cursorHud->Offsets()["Cursor"].y;
	//	//cursorHud->Position({ cursorHud->StaticPostions()["Left"].x,cursorHud->StaticPostions()["Left"].y + (float)currCursor * cursorHud->Offsets()["Cursor"].y });
	//	break;
	//case PortItems::WATER_FOOD:
	//case PortItems::RUM:
	//case PortItems::CREW:
	//	position.x = cursorHud->StaticPostions()["Right"].x;
	//	position.y = cursorHud->StaticPostions()["Right"].y + (float)(currCursor - 4) * cursorHud->Offsets()["Cursor"].y;
	//	//cursorHud->Position({ cursorHud->StaticPostions()["Right"].x, cursorHud->StaticPostions()["Right"].y + (float)(currCursor - 3) * cursorHud->Offsets()["Cursor"].y });
	//	break;
	//case PortItems::REPAIR:
	//	position.x = cursorHud->StaticPostions()["Repair"].x;
	//	position.y = cursorHud->StaticPostions()["Repair"].y;
	//	//cursorHud->Position({ cursorHud->StaticPostions()["Repair"].x, cursorHud->StaticPostions()["Repair"].y });
	//	break;
	//case PortItems::EXIT:
	//	position.x = cursorHud->StaticPostions()["Exit"].x;
	//	position.y = cursorHud->StaticPostions()["Exit"].y;
	//	//cursorHud->Position({ cursorHud->StaticPostions()["Exit"].x, cursorHud->StaticPostions()["Exit"].y });
	//	break;
	//case PortItems::UNKNOWN:
	//default:
	//	break;
	//}



	// highlight curr affected resources
	if (currPage == 0 && curritem <= PortItems::CREW)
	{
		// hud element string
		std::string huditem = posstring + "Num";

		// change color
		static_cast<G2D::GUIText*>(m_hudElems[huditem])->Color({ 1.0f, 0.0f, 0.0f, 1.0f });
		/*
		switch (curritem)
		{
		case PortItems::WATER_FOOD:
			static_cast<G2D::GUIText*>(m_hudElems[huditem])->Color({ 1.0f, 0.0f, 0.0f, 1.0f });
			break;
		case PortItems::RUM:
			static_cast<G2D::GUIText*>(m_hudElems[huditem])->Color({ 0.0f, 1.0f, 0.0f, 1.0f });
			break;
		case PortItems::CREW:
			static_cast<G2D::GUIText*>(m_hudElems[huditem])->Color({ 0.0f, 0.0f, 1.0f, 1.0f });
			break;
		default:
			break;
		}
		*/

	}


	// total $
	std::wstring totalstring = L"Total: ";

	//totalstring += (curritem == PortItems::EXIT) ? L"N/A" : (std::to_wstring(prices[curritem]) + L"g");
	if (curritem == PortItems::EXIT)
	{
		totalstring += L"N/A";
	}
	else
	{
		if (onsale == true)
			totalstring += L"~";

		totalstring += std::to_wstring(prices[curritem]) + L"g";
	}
	static_cast<G2D::GUIText*>(m_hudElems["TotalPrice"])->Text(totalstring);


	// fill out item descriptions
	ActivateItemDescriptions(curritem);






	// update feedback
	UpdateFeedbackElements(_dt);


	// update animations
	m_hudAnims.UpdateAll(_dt);





	// scene fade
	if (m_FadeIn)
	{
		//sceneManager->LockInput();
		m_Timer -= _dt;
		float timeInv = 1.0f - m_Timer / FADEINTIMER;
		if (timeInv >= 1.0f)
		{
			timeInv = 1.0f;
		}

		m_FadeAmount = timeInv;
		CGame::GetApplication()->m_pRenderer->SetPostProcessFadeAmount(timeInv);

		// GUI transparency
		for (auto i = m_hudElems.begin(); i != m_hudElems.end(); i++)
		{
			if ((*i).second->Type() == G2D::Entity2D::eTYPE_GUITexture)
			{
				float val = timeInv * 1.3f;
				static_cast<G2D::GUITexture*>((*i).second)->Color({ val, val, val, val });
			}
			if ((*i).second->Type() == G2D::Entity2D::eTYPE_GUIText)
			{
				auto color	= static_cast<G2D::GUIText*>((*i).second)->Color();
				color.w		= /*1.0f -*/ timeInv;
				static_cast<G2D::GUIText*>((*i).second)->Color(color);
			}
		}

		//m_MainCamera->SetOffset(-25.0f * timeInv);
		if (m_Timer <= 0.0f)
		{
			//sceneManager->UnlockInput();
			m_Timer = FADEINTIMER;
			m_FadeIn = false;
			m_FadeAmount = 0.0f;
			//m_MainCamera->SetOffset(-25.0f);
			CGame::GetApplication()->m_pRenderer->SetPostProcessFadeAmount(0.0f);
		}
	}
}


///* FOR THE LOVE OF [insert Deity here] PLEASE DO NOT ERASE YET!!! */
/*
void PortMenuScene::Update(float _dt)
{
	// base update
	IScene::Update(_dt);

	// etc....

	// scroll timer
	if (scrollTimer > 0.0f)
		scrollTimer -= _dt;
	if (scrollTimer < 0.0f)
		scrollTimer = 0.0f;



	CGame* game = (CGame*)CGame::GetApplication();




	// resets
	m_hudElems["xbox_lb"]->Active(false);
	m_hudElems["xbox_rb"]->Active(false);
	m_hudElems["Port_KeyboardInput"]->Active(false);
	m_hudElems["PortMenu1_bg_1920x1080"]->Active(false);
	m_hudElems["PortMenu2_bg_1920x1080"]->Active(false);
	m_hudElems["PortMenu3_bg_1920x1080"]->Active(false);
	m_hudElems["Food"]->Active(false);
	m_hudElems["Rum"]->Active(false);
	m_hudElems["Crew"]->Active(false);
	m_hudElems["Repair"]->Active(false);
	m_hudElems["Weapon"]->Active(false);
	m_hudElems["Sail"]->Active(false);
	m_hudElems["Hull"]->Active(false);
	m_hudElems["Flamethrower"]->Active(false);
	m_hudElems["Barrels"]->Active(false);
	m_hudElems["Greekshot"]->Active(false);
	m_hudElems["Fleet"]->Active(false);


	// input type
	bool is_connected = game->GamePadIsConnected();
	if (is_connected == true)
	{
		m_hudElems["xbox_lb"]->Active(true);
		m_hudElems["xbox_rb"]->Active(true);
	}
	else
	{
		m_hudElems["Port_KeyboardInput"]->Active(true);
	}




	// port name
	static_cast<G2D::GUIText*>(m_hudElems["PortName"])->Text(portName);
	//static_cast<G2D::GUIText*>(m_hudElems["PortName"])->Color({ 0,0,0,1 });


	// show curr resources
	GameplayScene* gps = ((GameplayScene*)sceneManager->GetScene(IScene::SceneType::GAMEPLAY));
	static_cast<G2D::GUIText*>(m_hudElems["GoldNum"])->Text(std::to_wstring(gps->playerShip->GetCurrGold()) + L"g");
	static_cast<G2D::GUIText*>(m_hudElems["FoodNum"])->Text(std::to_wstring(gps->playerShip->GetCurrWaterFood()) + L"/" + std::to_wstring(gps->playerShip->GetMaxWaterFood()));
	static_cast<G2D::GUIText*>(m_hudElems["CrewNum"])->Text(std::to_wstring(gps->playerShip->GetCurrCrew()) + L"/" + std::to_wstring(gps->playerShip->GetMaxCrew()));
	static_cast<G2D::GUIText*>(m_hudElems["RumNum"])->Text(std::to_wstring(gps->playerShip->GetCurrRum()) + L"/" + std::to_wstring(gps->playerShip->GetMaxRum()));


	// page#
	static_cast<G2D::GUIText*>(m_hudElems["PageNum"])->Text(L"Pg: " + std::to_wstring(currPage + 1) + L"/" + std::to_wstring(numPagesOpen));


	// items & prices
	int currlevels[3] = { gps->playerShip->GetWeaponsLevel(), gps->playerShip->GetMastLevel(), gps->playerShip->GetHullLevel() };
	std::wstring spaces = L"                                         ";

	switch (currPage)
	{
		// page1
	case 0:
		m_hudElems["PortMenu1_bg_1920x1080"]->Active(true);
		m_hudElems["Food"]->Active(true);
		m_hudElems["Rum"]->Active(true);
		m_hudElems["Crew"]->Active(true);
		m_hudElems["Repair"]->Active(true);
		static_cast<G2D::GUIText*>(m_hudElems["Food"])->Text(std::to_wstring(prices[WATER_FOOD])	+ L"g" + spaces + L"+" + std::to_wstring(incrementNums[resourceIncrements[0]]));
		static_cast<G2D::GUIText*>(m_hudElems["Rum"])->Text(std::to_wstring(prices[RUM])			+ L"g" + spaces + L"+" + std::to_wstring(incrementNums[resourceIncrements[1]]));
		static_cast<G2D::GUIText*>(m_hudElems["Crew"])->Text(std::to_wstring(prices[CREW])			+ L"g" + spaces + L"+" + std::to_wstring(incrementNums[resourceIncrements[2]]));
		static_cast<G2D::GUIText*>(m_hudElems["Repair"])->Text(std::to_wstring(prices[REPAIR])		+ L"g");
		break;

		// page2
	case 1:
		m_hudElems["PortMenu2_bg_1920x1080"]->Active(true);
		m_hudElems["Weapon"]->Active(true);
		m_hudElems["Sail"]->Active(true);
		m_hudElems["Hull"]->Active(true);
		static_cast<G2D::GUIText*>(m_hudElems["Weapon"])->Text(std::to_wstring(prices[WEAPONS])	+ L"g" + spaces + L"Weapon  Lv. "	+ std::to_wstring(currlevels[0]) + L"/5");
		static_cast<G2D::GUIText*>(m_hudElems["Sail"])->Text(std::to_wstring(prices[SAILS])		+ L"g" + spaces + L"Sail  Lv. "		+ std::to_wstring(currlevels[1]) + L"/5");
		static_cast<G2D::GUIText*>(m_hudElems["Hull"])->Text(std::to_wstring(prices[HULL])		+ L"g" + spaces + L"Hull  Lv. "		+ std::to_wstring(currlevels[2]) + L"/5");
		break;

		// page3
	case 2:
		m_hudElems["PortMenu3_bg_1920x1080"]->Active(true);
		m_hudElems["Flamethrower"]->Active(true);
		m_hudElems["Barrels"]->Active(true);
		m_hudElems["Greekshot"]->Active(true);
		static_cast<G2D::GUIText*>(m_hudElems["Flamethrower"])->Text(std::to_wstring(prices[FLAMETHROWER])	+ L"g");
		static_cast<G2D::GUIText*>(m_hudElems["Barrels"])->Text(std::to_wstring(prices[BARRELS])			+ L"g");
		static_cast<G2D::GUIText*>(m_hudElems["Greekshot"])->Text(std::to_wstring(prices[GREEKSHOT])		+ L"g");
		break;

		// page4
	case 3:
		m_hudElems["Fleet"]->Active(true);
		static_cast<G2D::GUIText*>(m_hudElems["Fleet"])->Text(std::to_wstring(prices[FLEET]) + L"g");
		break;

	default:
		break;
	}

	// items & prices
	//static_cast<G2D::GUIText*>(m_hudElems["Weapon"])->Text(
	//	L"Weapon  Lv. " + std::to_wstring(gps->playerShip->GetSideCannonHandler()->GetLevel()) + L"/5   $" + std::to_wstring(prices[WEAPONS])
	//	);
	//static_cast<G2D::GUIText*>(m_hudElems["Hull"])->Text(
	//	L"Hull  Lv. " + std::to_wstring(gps->playerShip->GetHullLevel()) + L"/5   $" + std::to_wstring(prices[HULL])
	//	);
	//static_cast<G2D::GUIText*>(m_hudElems["Sail"])->Text(
	//	L"Sail  Lv. " + std::to_wstring(gps->playerShip->GetMastLevel()) + L"/5   $" + std::to_wstring(prices[SAILS])
	//	);
	//static_cast<G2D::GUIText*>(m_hudElems["Food"])->Text(
	//	L"Food   $" + std::to_wstring(prices[WATER_FOOD])
	//	);
	//static_cast<G2D::GUIText*>(m_hudElems["Rum"])->Text(
	//	L"Rum   $" + std::to_wstring(prices[RUM])
	//	);
	//static_cast<G2D::GUIText*>(m_hudElems["Crew"])->Text(
	//	L"Crew   $" + std::to_wstring(prices[CREW])
	//	);
	//static_cast<G2D::GUIText*>(m_hudElems["Repair"])->Text(
	//	L"Repair   $" + std::to_wstring(prices[REPAIR])
	//	);



	// cursor
#if _DEBUG
	auto cursorHud = static_cast<G2D::GUITexture*>(m_hudElems["PortMenu_cursor_1024x768"]);
#else
	auto cursorHud = static_cast<G2D::GUITexture*>(m_hudElems["PortMenu_Reticle"]);	//static_cast<G2D::GUITexture*>(m_hudElems["PortMenu_cursor_1920x1080"]);
#endif


	// cursor position
	PortItems	curritem	= GetCurrPortItem();
	std::string	posstring	= "";
	switch (curritem)
	{
	case PortItems::WATER_FOOD:
		posstring = "Food";
		break;
	case PortItems::RUM:
		posstring = "Rum";
		break;
	case PortItems::CREW:
		posstring = "Crew";
		break;
	case PortItems::REPAIR:
		posstring = "Repair";
		break;
	case PortItems::WEAPONS:
		posstring = "Weapons";
		break;
	case PortItems::HULL:
		posstring = "Hull";
		break;
	case PortItems::SAILS:
		posstring = "Sails";
		break;
	case PortItems::FLAMETHROWER:
		posstring = "Flamethrower";
		break;
	case PortItems::BARRELS:
		posstring = "Barrels";
		break;
	case PortItems::GREEKSHOT:
		posstring = "Greekshot";
		break;
	case PortItems::FLEET:
		posstring = "Fleet";
		break;
	case PortItems::EXIT:
		posstring = "Exit";
		break;
	case PortItems::UNKNOWN:
	default:
		break;
	}

	float2 position = cursorHud->StaticPostions()[posstring];
	cursorHud->Position(position);
	//float2 position = float2(0.0f, 0.0f);
	//switch (currCursor)
	//{
	//case PortItems::WEAPONS:
	//case PortItems::HULL:
	//case PortItems::SAILS:
	//	position.x = cursorHud->StaticPostions()["Left"].x;
	//	position.y = cursorHud->StaticPostions()["Left"].y + (float)currCursor * cursorHud->Offsets()["Cursor"].y;
	//	//cursorHud->Position({ cursorHud->StaticPostions()["Left"].x,cursorHud->StaticPostions()["Left"].y + (float)currCursor * cursorHud->Offsets()["Cursor"].y });
	//	break;
	//case PortItems::WATER_FOOD:
	//case PortItems::RUM:
	//case PortItems::CREW:
	//	position.x = cursorHud->StaticPostions()["Right"].x;
	//	position.y = cursorHud->StaticPostions()["Right"].y + (float)(currCursor - 4) * cursorHud->Offsets()["Cursor"].y;
	//	//cursorHud->Position({ cursorHud->StaticPostions()["Right"].x, cursorHud->StaticPostions()["Right"].y + (float)(currCursor - 3) * cursorHud->Offsets()["Cursor"].y });
	//	break;
	//case PortItems::REPAIR:
	//	position.x = cursorHud->StaticPostions()["Repair"].x;
	//	position.y = cursorHud->StaticPostions()["Repair"].y;
	//	//cursorHud->Position({ cursorHud->StaticPostions()["Repair"].x, cursorHud->StaticPostions()["Repair"].y });
	//	break;
	//case PortItems::EXIT:
	//	position.x = cursorHud->StaticPostions()["Exit"].x;
	//	position.y = cursorHud->StaticPostions()["Exit"].y;
	//	//cursorHud->Position({ cursorHud->StaticPostions()["Exit"].x, cursorHud->StaticPostions()["Exit"].y });
	//	break;
	//case PortItems::UNKNOWN:
	//default:
	//	break;
	//}





	// scene fade
	if (m_FadeIn)
	{
		//sceneManager->LockInput();
		m_Timer -= _dt;
		float timeInv = 1.0f - m_Timer / FADEINTIMER;
		if (timeInv >= 1.0f)
		{
			timeInv = 1.0f;
		}

		m_FadeAmount = timeInv;
		CGame::GetApplication()->m_pRenderer->SetPostProcessFadeAmount(timeInv);

		// GUI transparency
		for (auto i = m_hudElems.begin(); i != m_hudElems.end(); i++)
		{
			if ((*i).second->Type() == G2D::Entity2D::eTYPE_GUITexture)
			{
				float val = timeInv * 1.3f;
				static_cast<G2D::GUITexture*>((*i).second)->Color({ val, val, val, val });
			}
			if ((*i).second->Type() == G2D::Entity2D::eTYPE_GUIText)
			{
				auto color	= static_cast<G2D::GUIText*>((*i).second)->Color();
				color.w		= timeInv;	//1.0f - timeInv;
				static_cast<G2D::GUIText*>((*i).second)->Color(color);
			}
		}

		//m_MainCamera->SetOffset(-25.0f * timeInv);
		if (m_Timer <= 0.0f)
		{
			//sceneManager->UnlockInput();
			m_Timer = FADEINTIMER;
			m_FadeIn = false;
			m_FadeAmount = 0.0f;
			//m_MainCamera->SetOffset(-25.0f);
			CGame::GetApplication()->m_pRenderer->SetPostProcessFadeAmount(0.0f);
		}
	}
}
*/


void PortMenuScene::Render(void)
{
	//// base render
	IScene::Render();
	m_hudAnims.RenderAll();
	m_GUIManager.RenderAll();



	// etc....
}


void PortMenuScene::Input_Start(void)
{

}


void PortMenuScene::Input_End(void)
{

}


bool PortMenuScene::Input_Keyboard(void)
{
	///* TO REPLACE INPUT BELOW - DO NOT DELETE */
	return New_Input_Keyboard();


	///* DO NOT DELETE YET! */
	/*
	CGame*							game = (CGame*)CGame::GetApplication();
	auto							kbs = Keyboard::Get().GetState();
	Keyboard::KeyboardStateTracker*	kbst = game->GetKeyboardTracker();
	SoundManager*					sound = game->GetSoundManager();


	// Input: backspace
	if (kbst->IsKeyPressed(Keyboard::Keys::Back) == true)
	{
		currCursor = PortItems::EXIT;
		sound->Play3DSound(AK::EVENTS::PLAYCURSOR);
		//sceneManager->PopScene();
		//return NEW_SCENE;
	}

	// Input: move cursor down (w/ wrap around)
	if (kbst->IsKeyPressed(Keyboard::Keys::Down) == true || kbst->IsKeyPressed(Keyboard::Keys::NumPad2) == true || kbst->IsKeyPressed(Keyboard::Keys::S) == true)
	{
		currCursor = (currCursor + 1 <= PortItems::EXIT) ? (currCursor + 1) : PortItems::WEAPONS;
		sound->Play3DSound(AK::EVENTS::PLAYCURSOR);
	}

	// Input: move cursor up  (w/ wrap around)
	if (kbst->IsKeyPressed(Keyboard::Keys::Up) == true || kbst->IsKeyPressed(Keyboard::Keys::NumPad8) == true || kbst->IsKeyPressed(Keyboard::Keys::W) == true)
	{
		currCursor = (currCursor - 1 >= PortItems::WEAPONS) ? (currCursor - 1) : PortItems::EXIT;
		sound->Play3DSound(AK::EVENTS::PLAYCURSOR);
	}

	// Input: move cursor left (w/ wrap around)
	if (kbst->IsKeyPressed(Keyboard::Keys::Left) == true || kbst->IsKeyPressed(Keyboard::Keys::NumPad4) == true || kbst->IsKeyPressed(Keyboard::Keys::A) == true)
	{
		currCursor = (currCursor <= PortItems::REPAIR) ? (currCursor + 4) : (currCursor - 4);
		sound->Play3DSound(AK::EVENTS::PLAYCURSOR);
	}

	// Input: move cursor right  (w/ wrap around)
	if (kbst->IsKeyPressed(Keyboard::Keys::Right) == true || kbst->IsKeyPressed(Keyboard::Keys::NumPad6) == true || kbst->IsKeyPressed(Keyboard::Keys::D) == true)
	{
		currCursor = (currCursor <= PortItems::REPAIR) ? (currCursor + 4) : (currCursor - 4);
		sound->Play3DSound(AK::EVENTS::PLAYCURSOR);
	}


	// Input: select curr menu option
	if (kbst->IsKeyPressed(Keyboard::Keys::Enter) == true)
	{
		// upgrade player
		switch (currCursor)
		{
		case PortItems::WEAPONS:
			Upgrade(PortItems::WEAPONS, IShipPart::PartType::WEAPON);
			break;

		case PortItems::HULL:
			Upgrade(PortItems::HULL, IShipPart::PartType::HULL);
			break;

		case PortItems::SAILS:
			Upgrade(PortItems::SAILS, IShipPart::PartType::MAST);
			break;

		case PortItems::REPAIR:
			Repair();
			break;

		case PortItems::WATER_FOOD:
			Refill(PortItems::WATER_FOOD);
			break;

		case PortItems::RUM:
			Refill(PortItems::RUM);
			break;

		case PortItems::CREW:
			Refill(PortItems::CREW);
			break;

		case PortItems::FLEET:
			FleetShip();
			break;

		case PortItems::EXIT:
			sceneManager->PopScene();
			return NEW_SCENE;
			break;

		case PortItems::UNKNOWN:
		default:
			break;
		}
	}



	// stay in same scene
	return OLD_SCENE;
	*/
}


bool PortMenuScene::Input_Gamepad(void)
{
	///* TO REPLACE INPUT BELOW - DO NOT DELETE */
	return New_Input_Gamepad();


	///* DO NOT DELETE YET! */
	/*
	CGame*							game = (CGame*)CGame::GetApplication();
	auto							gps = GamePad::Get().GetState(0);
	GamePad::ButtonStateTracker*	gpst = game->GetGamePadTracker();
	SoundManager*					sound = game->GetSoundManager();


	// Input: B
	if (gpst->b == GamePad::ButtonStateTracker::ButtonState::PRESSED)
	{
		currCursor = PortItems::EXIT;
		sound->Play3DSound(AK::EVENTS::PLAYCURSOR);
		//sceneManager->PopScene();
		//return NEW_SCENE;
	}


	// Input: move cursor down (w/ wrap around)
	if (gps.IsLeftThumbStickDown() == true || gps.IsDPadDownPressed() == true)
	{
		if (scrollTimer <= 0.0f)
		{
			sound->Play3DSound(AK::EVENTS::PLAYCURSOR);
			scrollTimer = SCENE_SCROLL_TIME;
			currCursor = (currCursor + 1 <= PortItems::EXIT) ? (currCursor + 1) : PortItems::WEAPONS;
		}
	}

	// Input: move cursor up  (w/ wrap around)
	if (gps.IsLeftThumbStickUp() == true || gps.IsDPadUpPressed() == true)
	{
		if (scrollTimer <= 0.0f)
		{
			sound->Play3DSound(AK::EVENTS::PLAYCURSOR);
			scrollTimer = SCENE_SCROLL_TIME;
			currCursor = (currCursor - 1 >= PortItems::WEAPONS) ? (currCursor - 1) : PortItems::EXIT;
		}
	}


	// Input: move cursor left (w/ wrap around)
	if (gps.IsLeftThumbStickLeft() == true || gps.IsDPadLeftPressed() == true)
	{
		if (scrollTimer <= 0.0f)
		{
			sound->Play3DSound(AK::EVENTS::PLAYCURSOR);
			scrollTimer = SCENE_SCROLL_TIME;
			currCursor = (currCursor <= PortItems::REPAIR) ? (currCursor + 4) : (currCursor - 4);
		}
	}

	// Input: move cursor right  (w/ wrap around)
	if (gps.IsLeftThumbStickRight() == true || gps.IsDPadRightPressed() == true)
	{
		if (scrollTimer <= 0.0f)
		{
			sound->Play3DSound(AK::EVENTS::PLAYCURSOR);
			scrollTimer = SCENE_SCROLL_TIME;
			currCursor = (currCursor <= PortItems::REPAIR) ? (currCursor + 4) : (currCursor - 4);
		}
	}


	// Input: select curr menu option
	if (gpst->a == GamePad::ButtonStateTracker::ButtonState::PRESSED)
	{
		// upgrade player
		switch (currCursor)
		{
		case PortItems::WEAPONS:
			Upgrade(PortItems::WEAPONS, IShipPart::PartType::WEAPON);
			break;

		case PortItems::HULL:
			Upgrade(PortItems::HULL, IShipPart::PartType::HULL);
			break;

		case PortItems::SAILS:
			Upgrade(PortItems::SAILS, IShipPart::PartType::MAST);
			break;

		case PortItems::REPAIR:
			Repair();
			break;

		case PortItems::WATER_FOOD:
			Refill(PortItems::WATER_FOOD);
			break;

		case PortItems::RUM:
			Refill(PortItems::RUM);
			break;

		case PortItems::CREW:
			Refill(PortItems::CREW);
			break;

		case PortItems::FLEET:
			FleetShip();
			break;

		case PortItems::EXIT:
			sceneManager->PopScene();
			return NEW_SCENE;
			break;

		case PortItems::UNKNOWN:
		default:
			break;
		}
	}



	// stay in same scene
	return OLD_SCENE;
	*/
}


void PortMenuScene::LoadHudList() {
    // reload hud elems
    G2D::CGUIManager::LoadXMLConfig( "../../FullSail/Resources/GUI/Configs/PortMenuScene.xml", m_hudElems );
    for ( auto it = m_hudElems.begin(); it != m_hudElems.end(); it++ )
        m_GUIManager.AddEntity( it->second );

    // reload animations
    m_hudAnims.LoadGUIAnimation( "GUIAnim_ChestOpen", m_hudElems );

    // reload feedback elems
    StartFeedbackColorChange( "WeaponLevel" );
    StartFeedbackColorChange( "SailLevel" );
    StartFeedbackColorChange( "HullLevel" );
}


void PortMenuScene::ReleaseHudList() {
    // release feedback elems
    ReleaseFeedbackElements();
    m_hudAnims.RemoveAllAnimation();

    // release hud elems
    for ( auto it = m_hudElems.begin(); it != m_hudElems.end(); it++ ) {
        m_GUIManager.RemoveEntity( it->second );
        SafeRelease( it->second );
    }
    m_hudElems.clear();

}


PortMenuScene::PortItems PortMenuScene::GetCurrPortItem( void )
{
	PortItems item = PortItems::UNKNOWN;


	switch (currPage)
	{

		// page 1: resources & repair
	case 0:
		switch (currCursor)
		{
			// water/food
		case 0:
			item = PortItems::WATER_FOOD;
			break;
			// rum
		case 1:
			item = PortItems::RUM;
			break;
			// crew
		case 2:
			item = PortItems::CREW;
			break;
			// repair
		case 3:
			item = PortItems::REPAIR;
			break;
			// exit
		case 4:
			item = PortItems::EXIT;
			break;
		default:
			break;
		}
		break;


		// page 2: upgrades
	case 1:
		switch (currCursor)
		{
			// weapons
		case 0:
			item = PortItems::WEAPONS;
			break;
			// sails
		case 1:
			item = PortItems::SAILS;
			break;
			// hull
		case 2:
			item = PortItems::HULL;
			break;
			// exit
		case 3:
			item = PortItems::EXIT;
			break;
		default:
			break;
		}
		break;


		// page 3: special weapons
	case 2:
		switch (currCursor)
		{
			// flamethrower
		case 0:
			item = PortItems::FLAMETHROWER;
			break;
			// explosive barrels
		case 1:
			item = PortItems::BARRELS;
			break;
			// greek shot
		case 2:
			item = PortItems::GREEKSHOT;
			break;
			// exit
		case 3:
			item = PortItems::EXIT;
			break;
		default:
			break;
		}
		break;


		// page 4: fleets
	case 3:
		switch (currCursor)
		{
			// fleet
		case 0:
			item = PortItems::FLAMETHROWER;
			break;
			// exit
		case 1:
			item = PortItems::EXIT;
			break;
		default:
			break;
		}
		break;


		// page ???: ???

	default:
		break;
	}


	return item;
}


bool PortMenuScene::OnExit(void)
{
	bool exit = false;

	switch (currPage)
	{
	case 0:
		exit = (currCursor == (NUM_CHOICES_PAGE_1 - 1)) ? true : false;
		break;
	case 1:
		exit = (currCursor == (NUM_CHOICES_PAGE_2 - 1)) ? true : false;
		break;
	case 2:
		exit = (currCursor == (NUM_CHOICES_PAGE_3 - 1)) ? true : false;
		break;

	case 3:
		exit = false;
		break;

	default:
		break;
	}


	return exit;
}


void PortMenuScene::ResetPrices(void)
{
	prices[PortItems::WATER_FOOD]	= baseprices[PortItems::WATER_FOOD];
	prices[PortItems::RUM]			= baseprices[PortItems::RUM];
	prices[PortItems::CREW]			= baseprices[PortItems::CREW];
	prices[PortItems::REPAIR]		= baseprices[PortItems::REPAIR];
	prices[PortItems::WEAPONS]		= baseprices[PortItems::WEAPONS];
	prices[PortItems::SAILS]		= baseprices[PortItems::SAILS];
	prices[PortItems::HULL]			= baseprices[PortItems::HULL];
	prices[PortItems::FLAMETHROWER]	= baseprices[PortItems::FLAMETHROWER];
	prices[PortItems::BARRELS]		= baseprices[PortItems::BARRELS];
	prices[PortItems::GREEKSHOT]	= baseprices[PortItems::GREEKSHOT];
	prices[PortItems::FLEET]		= baseprices[PortItems::FLEET];
}


int PortMenuScene::GetCurrentPrice(PortItems item, int level)
{
	// over level cap check
	if (level > 5 || level < 0)
		return -1000;

	// base$
	int curr = GetBasePrice(item);

	// base$ += (offset$ * lvl)
	for (int i = 0; i < level; i++)
		curr += priceOffsets[i];

	// return
	return curr;
}


int PortMenuScene::CalculateLoweredPrice(int baseprice, int curr, int max)
{
	/*
	MAX_HP	= 100
	CURR_HP	= 75
	$		= $200

	Dsc%	= (CURR_HP/MAX_HP)
			= (75/100)
			= (3/4)

	Dsc$	= $ * Dsc%
			= 200 * (3/4)
			= 150

	new$	= $ - Dsc$
			= 200 - 150
			= 50
	*/
	float	fcurr		= static_cast<float>(curr);
	float	fmax		= static_cast<float>(max);
	float	less_prcnt	= fcurr / fmax;
	int		less_price	= static_cast<int>(baseprice * less_prcnt);
	return less_price;
}


int PortMenuScene::CalculateDiscountPrice(int baseprice, int discount)
{
	/*
	$		= $400
	Dsc%	= 25%
			= 25 / 100
			= 25 * 0.01
			= 0.25

	Dsc$	= $ * Dsc%
			= 400 * 0.25
			= 100

	new$	= $ - Dsc$
			= 400 - 100
			= 300
	*/


	float	fprice		= static_cast<float>(baseprice);
	float	disc_prcnt	= static_cast<float>(discount) * 0.01f;
	float	disc_price	= fprice * disc_prcnt;
	int		newprice	= static_cast<int>(fprice - disc_price);
	return newprice;
}


void PortMenuScene::SetupStandardPrices(void)
{
	GameplayScene* gps = ((GameplayScene*)sceneManager->GetScene(IScene::SceneType::GAMEPLAY));


	// price fixes
	baseprices[PortItems::REPAIR] = gps->playerShip->GetMaxHealth() - gps->playerShip->GetHealth();


	// reset prices
	ResetPrices();


	// scale repair price based on curr HP
	//prices[PortItems::REPAIR] = gps->playerShip->GetMaxHealth() - gps->playerShip->GetHealth(); //CalculateLoweredPrice(baseprices[PortItems::REPAIR], gps->playerShip->GetHealth(), gps->playerShip->GetMaxHealth());


	// scale upgrade prices based on curr levels
	int currlevels[3] = { gps->playerShip->GetWeaponsLevel(), gps->playerShip->GetMastLevel(), gps->playerShip->GetHullLevel() };

	for (int i = 0; i < currlevels[0]; i++)
		prices[PortItems::WEAPONS] += priceOffsets[i];

	for (int i = 0; i < currlevels[1]; i++)
		prices[PortItems::SAILS] += priceOffsets[i];

	for (int i = 0; i < currlevels[2]; i++)
		prices[PortItems::HULL] += priceOffsets[i];



	// TODO: standard prices for pages 3-4
	/*
	prices[PortItems::FLAMETHROWER]	= 9999999;	//300;
	prices[PortItems::BARRELS]		= 9999999;	//300;
	prices[PortItems::GREEKSHOT]	= 9999999;	//300;

	prices[PortItems::FLEET]		= 9999999;	//400;
	*/



	// cancel discounts
	for (size_t i = 0; i < 11; i++)
		discounts[i] = 0;
}


void PortMenuScene::SetupDiscountPrices(Port* p)
{
	GameplayScene* gps = ((GameplayScene*)sceneManager->GetScene(IScene::SceneType::GAMEPLAY));


	// reset prices
	SetupStandardPrices();
	//ResetPrices();



	int		less_price[11]	= {};
	int		currlevels[3]	= { gps->playerShip->GetWeaponsLevel(), gps->playerShip->GetMastLevel(), gps->playerShip->GetHullLevel() };
	int		maxupgrades[3]	= {};


	// upgrades available @ visited port
	for (int i = 0; i < 3; i++)
		maxupgrades[i] = p->GetMaxUpgrade(i);


	// lower $ based on (curr/max) resources
	//less_price[PortItems::WATER_FOOD]	= CalculateLoweredPrice(prices[PortItems::WATER_FOOD],	gps->playerShip->GetCurrWaterFood(),	gps->playerShip->GetMaxWaterFood());
	//less_price[PortItems::RUM]			= CalculateLoweredPrice(prices[PortItems::RUM],			gps->playerShip->GetCurrRum(),			gps->playerShip->GetMaxRum());
	//less_price[PortItems::CREW]			= CalculateLoweredPrice(prices[PortItems::CREW],		gps->playerShip->GetCurrCrew(),			gps->playerShip->GetMaxCrew());
	//less_price[PortItems::REPAIR]		= CalculateLoweredPrice(prices[PortItems::REPAIR], gps->playerShip->GetHealth(), gps->playerShip->GetMaxHealth());

	// $ based on (curr/available) upgrades
	//less_price[PortItems::WEAPONS]		= (currlevels[0] >= maxupgrades[0]) ? CalculateLoweredPrice(prices[PortItems::WEAPONS],	currlevels[0],	maxupgrades[0]) : 0;
	//less_price[PortItems::SAILS]		= (currlevels[1] >= maxupgrades[1]) ? CalculateLoweredPrice(prices[PortItems::SAILS],	currlevels[1],	maxupgrades[1]) : 0;
	//less_price[PortItems::HULL]			= (currlevels[2] >= maxupgrades[2]) ? CalculateLoweredPrice(prices[PortItems::HULL],	currlevels[2],	maxupgrades[2]) : 0;

	// $ based on (acquired: Y/N) special weapon
	less_price[PortItems::FLAMETHROWER]	= (false) ? prices[PortItems::FLAMETHROWER]	: 0;
	less_price[PortItems::BARRELS]		= (false) ? prices[PortItems::BARRELS]		: 0;
	less_price[PortItems::GREEKSHOT]	= (false) ? prices[PortItems::GREEKSHOT]	: 0;

	// $ based on (curr == max) # of ships in fleet
	less_price[PortItems::FLEET]		= (false) ? prices[PortItems::FLEET] : 0;


	// new $s
	for (size_t i = 0; i < 11; i++)
		prices[i] = prices[i] - less_price[i];


	// get discounts %s: 5, 10, 15, 20, 25
	for (size_t i = 0; i < 11; i++)
		discounts[i] = p->GetDiscount((PortItems)i);



	// lower $ based on sale discounts
	for (size_t i = 0; i < 11; i++)
	{
		// cannot get a discount
		if (prices[i] == 0)
			continue;

		// $ = $ - ($ * %)
		prices[i] = CalculateDiscountPrice(prices[i], discounts[i]);


		// minimum $
		if (prices[i] < 0)
			prices[i] = 0;
	}

}


void PortMenuScene::DeactivateToggleableGUI(void)
{
	// resets active status


	// shared stuff
	m_hudElems["PortMenu_newbg"]->Active(true);
	//m_hudElems["PortMenu1_bg_1920x1080"]->Active(false);
	//m_hudElems["PortMenu2_bg_1920x1080"]->Active(false);
	//m_hudElems["PortMenu3_bg_1920x1080"]->Active(false);
	m_hudElems["DiscountIcon"]->Active(false);
	m_hudElems["PortResources"]->Active(true);

	m_hudElems["PortFullCapacity1"]->Active(false);
	m_hudElems["PortFullCapacity2"]->Active(false);
	m_hudElems["PortFullCapacity3"]->Active(false);
	m_hudElems["PortRepaired"]->Active(false);
	m_hudElems["PortOutOfStock1"]->Active(false);
	m_hudElems["PortOutOfStock2"]->Active(false);
	m_hudElems["PortOutOfStock3"]->Active(false);


	// input
	m_hudElems["xbox_lb"]->Active(false);
	m_hudElems["xbox_rb"]->Active(false);
	m_hudElems["PageIncInput"]->Active(false);
	//m_hudElems["Port_KeyboardInput"]->Active(false);



	// page 1
	m_hudElems["Page1Columns"]->Active(false);
	m_hudElems["PortPricesResources"]->Active(false);

	m_hudElems["FoodCount"]->Active(false);
	m_hudElems["FoodPrice"]->Active(false);
	m_hudElems["FoodDiscount"]->Active(false);
	m_hudElems["FoodDiscPrice"]->Active(false);

	m_hudElems["RumCount"]->Active(false);
	m_hudElems["RumPrice"]->Active(false);
	m_hudElems["RumDiscount"]->Active(false);
	m_hudElems["RumDiscPrice"]->Active(false);

	m_hudElems["CrewCount"]->Active(false);
	m_hudElems["CrewPrice"]->Active(false);
	m_hudElems["CrewDiscount"]->Active(false);
	m_hudElems["CrewDiscPrice"]->Active(false);

	m_hudElems["RepairCount"]->Active(false);
	m_hudElems["RepairPrice"]->Active(false);
	m_hudElems["RepairDiscount"]->Active(false);
	m_hudElems["RepairDiscPrice"]->Active(false);



	// page 2
	m_hudElems["Page2Columns"]->Active(false);
	m_hudElems["PortPricesUpgrades"]->Active(false);

	m_hudElems["WeaponLevel"]->Active(false);
	m_hudElems["WeaponPrice"]->Active(false);
	m_hudElems["WeaponDiscount"]->Active(false);
	m_hudElems["WeaponDiscPrice"]->Active(false);

	m_hudElems["SailLevel"]->Active(false);
	m_hudElems["SailPrice"]->Active(false);
	m_hudElems["SailDiscount"]->Active(false);
	m_hudElems["SailDiscPrice"]->Active(false);

	m_hudElems["HullLevel"]->Active(false);
	m_hudElems["HullPrice"]->Active(false);
	m_hudElems["HullDiscount"]->Active(false);
	m_hudElems["HullDiscPrice"]->Active(false);



	// page 3
	//m_hudElems["PortPricesWeapons"]->Active(false);
	/*

	m_hudElems["FlamethrowerPrice"]->Active(false);
	m_hudElems["FlamethrowerDiscount"]->Active(false);
	//m_hudElems["FlamethrowerDiscPrice"]->Active(true);

	m_hudElems["BarrelsPrice"]->Active(false);
	m_hudElems["BarrelsDiscount"]->Active(false);
	//m_hudElems["BarrelsDiscPrice"]->Active(true);

	m_hudElems["GreekshotPrice"]->Active(false);
	m_hudElems["GreekshotDiscount"]->Active(false);
	//m_hudElems["GreekshotDiscPrice"]->Active(true);
	*/



	// page 4
	/*
	m_hudElems["Fleet"]->Active(false);
	*/



	// animations
	m_hudAnims.Animation("GUIAnim_ChestOpen")->HideAnimation();

}


void PortMenuScene::ActivatePage1GUI(void)
{
	GameplayScene*		gps		= ((GameplayScene*)sceneManager->GetScene(IScene::SceneType::GAMEPLAY));
	Ship*				player	= gps->playerShip;
	XMINT2				food	= XMINT2(player->GetCurrWaterFood(), player->GetMaxWaterFood());
	XMINT2				rum		= XMINT2(player->GetCurrRum(), player->GetMaxRum());
	XMINT2				crew	= XMINT2(player->GetCurrCrew(), player->GetMaxCrew());
	XMINT2				health	= XMINT2(player->GetHealth(), player->GetMaxHealth());



	// page setup
	m_hudElems["Page1Columns"]->Active(true);
	m_hudElems["PortPricesResources"]->Active(true);
	m_hudElems["PortFullCapacity1"]->Active((food.x >= food.y) ? true : false);
	m_hudElems["PortFullCapacity2"]->Active((rum.x >= rum.y) ? true : false);
	m_hudElems["PortFullCapacity3"]->Active((crew.x >= crew.y) ? true : false);
	m_hudElems["PortRepaired"]->Active((health.x >= health.y) ? true : false);
	static_cast<G2D::GUIText*>(m_hudElems["PageTitle"])->Text(L"Resources");



	// resource count
	m_hudElems["FoodCount"]->Active(true);
	m_hudElems["RumCount"]->Active(true);
	m_hudElems["CrewCount"]->Active(true);
	m_hudElems["RepairCount"]->Active(true);

	static_cast<G2D::GUIText*>(m_hudElems["FoodCount"])->Text(L"+"		+ std::to_wstring(incrementNums[resourceIncrements[0]]));
	static_cast<G2D::GUIText*>(m_hudElems["RumCount"])->Text(L"+"		+ std::to_wstring(incrementNums[resourceIncrements[1]]));
	static_cast<G2D::GUIText*>(m_hudElems["CrewCount"])->Text(L"+"		+ std::to_wstring(incrementNums[resourceIncrements[2]]));
	//static_cast<G2D::GUIText*>(m_hudElems["RepairCount"])->Text(L"+"	+ std::to_wstring(gps->playerShip->GetMaxHealth() - gps->playerShip->GetHealth()));



	// resource price
	m_hudElems["FoodPrice"]->Active(true);
	m_hudElems["RumPrice"]->Active(true);
	m_hudElems["CrewPrice"]->Active(true);
	m_hudElems["RepairPrice"]->Active(true);

	static_cast<G2D::GUIText*>(m_hudElems["FoodPrice"])->Text(std::to_wstring(baseprices[WATER_FOOD])	+ L"g");
	static_cast<G2D::GUIText*>(m_hudElems["RumPrice"])->Text(std::to_wstring(baseprices[RUM])			+ L"g");
	static_cast<G2D::GUIText*>(m_hudElems["CrewPrice"])->Text(std::to_wstring(baseprices[CREW])			+ L"g");
	static_cast<G2D::GUIText*>(m_hudElems["RepairPrice"])->Text(std::to_wstring(baseprices[REPAIR])		+ L"g");



	// resource discount
	m_hudElems["FoodDiscount"]->Active(true);
	m_hudElems["RumDiscount"]->Active(true);
	m_hudElems["CrewDiscount"]->Active(true);
	m_hudElems["RepairDiscount"]->Active(true);

	static_cast<G2D::GUIText*>(m_hudElems["FoodDiscount"])->Text(std::to_wstring((discounts[WATER_FOOD]) * -1)	+ L"%");
	static_cast<G2D::GUIText*>(m_hudElems["RumDiscount"])->Text(std::to_wstring((discounts[RUM]) * -1)			+ L"%");
	static_cast<G2D::GUIText*>(m_hudElems["CrewDiscount"])->Text(std::to_wstring((discounts[CREW]) * -1)		+ L"%");
	static_cast<G2D::GUIText*>(m_hudElems["RepairDiscount"])->Text(std::to_wstring((discounts[REPAIR]) * -1)	+ L"%");



	// resource discprice
	m_hudElems["FoodDiscPrice"]->Active(true);
	m_hudElems["RumDiscPrice"]->Active(true);
	m_hudElems["CrewDiscPrice"]->Active(true);
	m_hudElems["RepairDiscPrice"]->Active(true);

	static_cast<G2D::GUIText*>(m_hudElems["FoodDiscPrice"])->Text(L"-"		+ std::to_wstring(baseprices[WATER_FOOD]	- prices[WATER_FOOD])	+ L"g");
	static_cast<G2D::GUIText*>(m_hudElems["RumDiscPrice"])->Text(L"-"		+ std::to_wstring(baseprices[RUM]			- prices[RUM])			+ L"g");
	static_cast<G2D::GUIText*>(m_hudElems["CrewDiscPrice"])->Text(L"-"		+ std::to_wstring(baseprices[CREW]			- prices[CREW])			+ L"g");
	static_cast<G2D::GUIText*>(m_hudElems["RepairDiscPrice"])->Text(L"-"	+ std::to_wstring(baseprices[REPAIR]		- prices[REPAIR])		+ L"g");



	/// /* DO NOT ERASE YET */
	/*
	// active status
	m_hudElems["PortPricesResources"]->Active(true);
	//m_hudElems["PortMenu1_bg_1920x1080"]->Active(true);
	m_hudElems["FoodPrice"]->Active(true);
	m_hudElems["FoodDiscount"]->Active(true);
	m_hudElems["RumPrice"]->Active(true);
	m_hudElems["RumDiscount"]->Active(true);
	m_hudElems["CrewPrice"]->Active(true);
	m_hudElems["CrewDiscount"]->Active(true);
	m_hudElems["RepairPrice"]->Active(true);
	m_hudElems["RepairDiscount"]->Active(true);
	m_hudElems["Page1Columns"]->Active(true);

	// text
	static_cast<G2D::GUIText*>(m_hudElems["FoodPrice"])->Text(std::to_wstring(prices[WATER_FOOD])	+ L"g");
	static_cast<G2D::GUIText*>(m_hudElems["RumPrice"])->Text(std::to_wstring(prices[RUM])			+ L"g");
	static_cast<G2D::GUIText*>(m_hudElems["CrewPrice"])->Text(std::to_wstring(prices[CREW])			+ L"g");
	static_cast<G2D::GUIText*>(m_hudElems["RepairPrice"])->Text(std::to_wstring(prices[REPAIR])		+ L"g");
	static_cast<G2D::GUIText*>(m_hudElems["FoodDiscount"])->Text(std::to_wstring((discounts[WATER_FOOD]) * -1)	+ L"%");
	static_cast<G2D::GUIText*>(m_hudElems["RumDiscount"])->Text(std::to_wstring((discounts[RUM]) * -1)			+ L"%");
	static_cast<G2D::GUIText*>(m_hudElems["CrewDiscount"])->Text(std::to_wstring((discounts[CREW]) * -1)		+ L"%");
	static_cast<G2D::GUIText*>(m_hudElems["RepairDiscount"])->Text(std::to_wstring((discounts[REPAIR]) * -1)	+ L"% (-" + std::to_wstring(baseprices[WATER_FOOD] - prices[WATER_FOOD]) + L"g)");
	*/
}


void PortMenuScene::ActivatePage2GUI(void)
{
	GameplayScene* gps	= ((GameplayScene*)sceneManager->GetScene(IScene::SceneType::GAMEPLAY));
	int currlevels[3]	= { gps->playerShip->GetWeaponsLevel(), gps->playerShip->GetMastLevel(), gps->playerShip->GetHullLevel() };
	int baseoffsets[3]	= { GetCurrentPrice(WEAPONS, currlevels[0]), GetCurrentPrice(SAILS, currlevels[1]), GetCurrentPrice(HULL, currlevels[2]) };



	// page setup
	m_hudElems["Page2Columns"]->Active(true);
	m_hudElems["PortPricesUpgrades"]->Active(true);
	m_hudElems["PortOutOfStock1"]->Active((currlevels[0] >= 5) ? true : false);
	m_hudElems["PortOutOfStock2"]->Active((currlevels[1] >= 5) ? true : false);
	m_hudElems["PortOutOfStock3"]->Active((currlevels[2] >= 5) ? true : false);
	static_cast<G2D::GUIText*>(m_hudElems["PageTitle"])->Text(L"Upgrades");



	// upgrade level
	m_hudElems["WeaponLevel"]->Active(true);
	m_hudElems["SailLevel"]->Active(true);
	m_hudElems["HullLevel"]->Active(true);

	static_cast<G2D::GUIText*>(m_hudElems["WeaponLevel"])->Text(std::to_wstring(currlevels[0])	+ L"/5");
	static_cast<G2D::GUIText*>(m_hudElems["SailLevel"])->Text(std::to_wstring(currlevels[1])	+ L"/5");
	static_cast<G2D::GUIText*>(m_hudElems["HullLevel"])->Text(std::to_wstring(currlevels[2])	+ L"/5");



	// upgrade price
	m_hudElems["WeaponPrice"]->Active(true);
	m_hudElems["SailPrice"]->Active(true);
	m_hudElems["HullPrice"]->Active(true);

	static_cast<G2D::GUIText*>(m_hudElems["WeaponPrice"])->Text(std::to_wstring(baseoffsets[0])	+ L"g");
	static_cast<G2D::GUIText*>(m_hudElems["SailPrice"])->Text(std::to_wstring(baseoffsets[1])	+ L"g");
	static_cast<G2D::GUIText*>(m_hudElems["HullPrice"])->Text(std::to_wstring(baseoffsets[2])	+ L"g");



	// upgrade discount
	m_hudElems["WeaponDiscount"]->Active(true);
	m_hudElems["SailDiscount"]->Active(true);
	m_hudElems["HullDiscount"]->Active(true);

	static_cast<G2D::GUIText*>(m_hudElems["WeaponDiscount"])->Text(std::to_wstring((discounts[WEAPONS] * -1))	+ L"%");
	static_cast<G2D::GUIText*>(m_hudElems["SailDiscount"])->Text(std::to_wstring((discounts[SAILS] * -1))		+ L"%");
	static_cast<G2D::GUIText*>(m_hudElems["HullDiscount"])->Text(std::to_wstring((discounts[HULL] * -1))		+ L"%");



	// upgrade discprice
	m_hudElems["WeaponDiscPrice"]->Active(true);
	m_hudElems["SailDiscPrice"]->Active(true);
	m_hudElems["HullDiscPrice"]->Active(true);

	static_cast<G2D::GUIText*>(m_hudElems["WeaponDiscPrice"])->Text(L"-"	+ std::to_wstring(baseoffsets[0]	- prices[WEAPONS])	+ L"g");
	static_cast<G2D::GUIText*>(m_hudElems["SailDiscPrice"])->Text(L"-"		+ std::to_wstring(baseoffsets[1]	- prices[SAILS])	+ L"g");
	static_cast<G2D::GUIText*>(m_hudElems["HullDiscPrice"])->Text(L"-"		+ std::to_wstring(baseoffsets[2]	- prices[HULL])		+ L"g");



	/// /* DO NOT ERASE YET */
	/*
	// active status
	m_hudElems["PortPricesUpgrades"]->Active(true);
	//m_hudElems["PortMenu2_bg_1920x1080"]->Active(true);
	m_hudElems["WeaponPrice"]->Active(true);
	m_hudElems["SailPrice"]->Active(true);
	m_hudElems["HullPrice"]->Active(true);
	m_hudElems["WeaponDiscount"]->Active(true);
	m_hudElems["SailDiscount"]->Active(true);
	m_hudElems["HullDiscount"]->Active(true);
	m_hudElems["WeaponLevel"]->Active(true);
	m_hudElems["SailLevel"]->Active(true);
	m_hudElems["HullLevel"]->Active(true);
	m_hudElems["Page2Columns"]->Active(true);

	// text
	static_cast<G2D::GUIText*>(m_hudElems["WeaponPrice"])->Text(std::to_wstring(prices[WEAPONS])	+ L"g");
	static_cast<G2D::GUIText*>(m_hudElems["SailPrice"])->Text(std::to_wstring(prices[SAILS])		+ L"g");
	static_cast<G2D::GUIText*>(m_hudElems["HullPrice"])->Text(std::to_wstring(prices[HULL])			+ L"g");
	static_cast<G2D::GUIText*>(m_hudElems["WeaponDiscount"])->Text(std::to_wstring((discounts[WEAPONS] * -1))	+ L"%");
	static_cast<G2D::GUIText*>(m_hudElems["SailDiscount"])->Text(std::to_wstring((discounts[SAILS] * -1))		+ L"%");
	static_cast<G2D::GUIText*>(m_hudElems["HullDiscount"])->Text(std::to_wstring((discounts[HULL] * -1))		+ L"%");
	static_cast<G2D::GUIText*>(m_hudElems["WeaponLevel"])->Text	(L"Weapon  Lv. "	+ std::to_wstring(currlevels[0]) + L"/5");
	static_cast<G2D::GUIText*>(m_hudElems["SailLevel"])->Text(L"Sail  Lv. "			+ std::to_wstring(currlevels[1]) + L"/5");
	static_cast<G2D::GUIText*>(m_hudElems["HullLevel"])->Text(L"Hull  Lv. "			+ std::to_wstring(currlevels[2]) + L"/5");
	break;
	*/
}


void PortMenuScene::ActivatePage3GUI(void)
{
	return;

	// page setup
	m_hudElems["Page3Columns"]->Active(true);
	//m_hudElems["PortPricesWeapons"]->Active(true);
	static_cast<G2D::GUIText*>(m_hudElems["PageTitle"])->Text(L"Special Weapons");



	// weapon price
	m_hudElems["FlamethrowerPrice"]->Active(true);
	m_hudElems["BarrelsPrice"]->Active(true);
	m_hudElems["GreekshotPrice"]->Active(true);

	static_cast<G2D::GUIText*>(m_hudElems["FlamethrowerPrice"])->Text(std::to_wstring(baseprices[FLAMETHROWER])	+ L"g");
	static_cast<G2D::GUIText*>(m_hudElems["BarrelsPrice"])->Text(std::to_wstring(baseprices[BARRELS])			+ L"g");
	static_cast<G2D::GUIText*>(m_hudElems["GreekshotPrice"])->Text(std::to_wstring(baseprices[GREEKSHOT])		+ L"g");



	// weapon discount
	m_hudElems["FlamethrowerDiscount"]->Active(true);
	m_hudElems["BarrelsDiscount"]->Active(true);
	m_hudElems["GreekshotDiscount"]->Active(true);

	static_cast<G2D::GUIText*>(m_hudElems["FlamethrowerDiscount"])->Text(std::to_wstring((discounts[FLAMETHROWER]) * -1)	+ L"%");
	static_cast<G2D::GUIText*>(m_hudElems["BarrelsDiscount"])->Text(std::to_wstring((discounts[BARRELS]) * -1)				+ L"%");
	static_cast<G2D::GUIText*>(m_hudElems["GreekshotDiscount"])->Text(std::to_wstring((discounts[GREEKSHOT]) * -1)			+ L"%");



	// weapon discprice
	m_hudElems["FlamethrowerDiscPrice"]->Active(true);
	m_hudElems["BarrelsDiscPrice"]->Active(true);
	m_hudElems["GreekshotDiscPrice"]->Active(true);

	static_cast<G2D::GUIText*>(m_hudElems["FlamethrowerDiscPrice"])->Text(L"-"	+ std::to_wstring(baseprices[FLAMETHROWER]	- prices[FLAMETHROWER])	+ L"g");
	static_cast<G2D::GUIText*>(m_hudElems["BarrelsDiscPrice"])->Text(L"-"		+ std::to_wstring(baseprices[BARRELS]		- prices[BARRELS])		+ L"g");
	static_cast<G2D::GUIText*>(m_hudElems["GreekshotDiscPrice"])->Text(L"-"		+ std::to_wstring(baseprices[GREEKSHOT]		- prices[GREEKSHOT])	+ L"g");



	/// /* DO NOT ERASE YET */
	/*
	// active status
	m_hudElems["PortPricesWeapons"]->Active(true);
	//m_hudElems["PortMenu3_bg_1920x1080"]->Active(true);
	m_hudElems["FlamethrowerPrice"]->Active(true);
	m_hudElems["BarrelsPrice"]->Active(true);
	m_hudElems["GreekshotPrice"]->Active(true);
	m_hudElems["FlamethrowerDiscount"]->Active(true);
	m_hudElems["BarrelsDiscount"]->Active(true);
	m_hudElems["GreekshotDiscount"]->Active(true);

	// text
	static_cast<G2D::GUIText*>(m_hudElems["FlamethrowerPrice"])->Text(std::to_wstring(prices[FLAMETHROWER])	+ L"g");
	static_cast<G2D::GUIText*>(m_hudElems["BarrelsPrice"])->Text(std::to_wstring(prices[BARRELS])			+ L"g");
	static_cast<G2D::GUIText*>(m_hudElems["GreekshotPrice"])->Text(std::to_wstring(prices[GREEKSHOT])		+ L"g");
	static_cast<G2D::GUIText*>(m_hudElems["FlamethrowerDiscount"])->Text(std::to_wstring((discounts[FLAMETHROWER]) * -1)	+ L"%");
	static_cast<G2D::GUIText*>(m_hudElems["BarrelsDiscount"])->Text(std::to_wstring((discounts[BARRELS]) * -1)				+ L"%");
	static_cast<G2D::GUIText*>(m_hudElems["GreekshotDiscount"])->Text(std::to_wstring((discounts[GREEKSHOT]) * -1)			+ L"%");
	break;
	*/
}


void PortMenuScene::ActivateItemDescriptions(PortItems item)
{
	GameplayScene* gps = ((GameplayScene*)sceneManager->GetScene(IScene::SceneType::GAMEPLAY));


	// player data
	Ship*				player		= gps->playerShip;
	XMINT2				food		= XMINT2(0, 0);
	XMINT2				rum			= XMINT2(0, 0);
	XMINT2				crew		= XMINT2(0, 0);
	XMINT2				health		= XMINT2(0, 0);
	int					w_lvl		= 0;
	int					s_lvl		= 0;
	int					h_lvl		= 0;
	int					max_lvl		= 5;
	SideCannonHandler*	weapons		= nullptr;
	XMINT2				c_dmg		= XMINT2(0, 0);
	XMINT2				l9_dmg		= XMINT2(0, 0);
	//XMFLOAT2			w_rld		= XMFLOAT2(0.0f, 0.0f);
	float				crr_spd		= 0.0f;
	float				max_spd		= 0.0f;
	float				trn_rte		= 0.0f;
	std::wstringstream	wss1;// , wss2;


	// colors
	SimpleMath::Color	color_none		= SimpleMath::Color({ 0.0f, 0.0f, 0.0f, 0.0f });
	SimpleMath::Color	color_white		= SimpleMath::Color({ 1.0f, 1.0f, 1.0f, 1.0f });

	SimpleMath::Color	color_red		= SimpleMath::Color({ 1.0f, 0.0f, 0.0f, 1.0f });
	SimpleMath::Color	color_green		= SimpleMath::Color({ 0.0f, 1.0f, 0.0f, 1.0f });
	SimpleMath::Color	color_blue		= SimpleMath::Color({ 0.0f, 0.0f, 1.0f, 1.0f });

	SimpleMath::Color	color_yellow	= SimpleMath::Color({ 1.0f, 1.0f, 0.0f, 1.0f });
	SimpleMath::Color	color_cyan		= SimpleMath::Color({ 0.0f, 1.0f, 1.0f, 1.0f });
	SimpleMath::Color	color_magenta	= SimpleMath::Color({ 1.0f, 0.0f, 1.0f, 1.0f });



	// item descriptions
	DescriptionStruct	id1		= DescriptionStruct();
	DescriptionStruct	id2		= DescriptionStruct();
	DescriptionStruct	id3		= DescriptionStruct();
	DescriptionStruct	id4		= DescriptionStruct();
	DescriptionStruct	id5		= DescriptionStruct();


	switch (item)
	{
		// Page 1
	case PortItems::WATER_FOOD:
		food	= XMINT2(player->GetCurrWaterFood(), player->GetMaxWaterFood());
		id1		= DescriptionStruct(L"Food: " + std::to_wstring(food.x) + L"/" + std::to_wstring(food.y), ((food.x < food.y) ? color_yellow : color_green));
		id2		= DescriptionStruct(L"", color_none);
		id3		= (food.x < food.y)
					? DescriptionStruct(L"Prevent starvation Cap'n!",	color_red)
					: DescriptionStruct(L"Full bellies Cap'n!",			color_green);
		id4		= DescriptionStruct(L"", color_none);
		id5		= DescriptionStruct(L"", color_none);
		break;

	case PortItems::RUM:
		rum	= XMINT2(player->GetCurrRum(), player->GetMaxRum());
		id1 = DescriptionStruct(L"Rum: " + std::to_wstring(rum.x) + L"/" + std::to_wstring(rum.y), ((rum.x < rum.y) ? color_yellow : color_green));
		id2 = DescriptionStruct(L"", color_none);
		id3 = (rum.x < rum.y)
					? DescriptionStruct(L"THE CREW BE SOBER CAP'N!",				color_red)
					: DescriptionStruct(L"hic...l-life's...good...Cap'n...hic!",	color_green);
		id4	= (rum.x < rum.y)
					? DescriptionStruct(L"ALL THE RUM BE GONE!!!",	color_red)
					: DescriptionStruct(L"", color_none);;
		id5	= DescriptionStruct(L"", color_none);
		break;

	case PortItems::CREW:
		crew	= XMINT2(player->GetCurrCrew(), player->GetMaxCrew());
		id1		= DescriptionStruct(L"Crew: " + std::to_wstring(crew.x) + L"/" + std::to_wstring(crew.y), ((crew.x < crew.y) ? color_yellow : color_green));
		id2		= DescriptionStruct(L"", color_none);
		id3		= (crew.x < crew.y)
					? DescriptionStruct(L"We could use some men Cap'n!",	color_red)
					: DescriptionStruct(L"Full crew Cap'n!",				color_green);
		id4		= DescriptionStruct(L"", color_none);
		id5		= DescriptionStruct(L"", color_none);
		break;

	case PortItems::REPAIR:
		health	= XMINT2(player->GetHealth(), player->GetMaxHealth());
		crr_spd	= player->GetCurrentMaxSpeed();
		max_spd	= player->GetMaxSpeed();

		id1		= DescriptionStruct(L"HP: " + std::to_wstring(health.x) + L"/" + std::to_wstring(health.y), ((health.x < health.y) ? color_yellow : color_green));
		id2		= DescriptionStruct(L"", color_none);
		id3		= (health.x >= health.y && crr_spd >= max_spd)
					? DescriptionStruct(L"Looking ship shape Cap'n!", color_green)
					: DescriptionStruct(L"Problems with the ship Cap'n!", color_red);
		id4		= DescriptionStruct(L"", color_none);
		id5		= DescriptionStruct(L"", color_none);
		break;



		// Page 2
	case PortItems::WEAPONS:
		w_lvl	= player->GetWeaponsLevel();
		weapons	= player->GetSideCannonHandler();
		c_dmg	= XMINT2(weapons->GetCannons()->GetMinDamage(), weapons->GetCannons()->GetMaxDamage());
		l9_dmg	= XMINT2(weapons->GetLongNine()->GetMinDamage(), weapons->GetLongNine()->GetMaxDamage());
		//w_rld	= XMFLOAT2(weapons->GetCannons()->GetMaxReload(), weapons->GetLongNine()->GetMaxReload());
		//wss1 << w_rld.x;
		//wss2 << w_rld.y;

		id1		= (w_lvl < max_lvl)
					? DescriptionStruct(L"Our weapons can get better Cap'n!",	color_yellow)
					: DescriptionStruct(L"Our weapons are the best Cap'n!",		color_green);

		id2		= DescriptionStruct(L"", color_none);
		id3		= DescriptionStruct(L"Cannon damage: "	+ std::to_wstring(c_dmg.x), color_red);
		id4		= DescriptionStruct(L"", color_none);
		id5		= DescriptionStruct(L"Long 9 damage: "	+ std::to_wstring(l9_dmg.x), color_cyan);

		if (c_dmg.x != c_dmg.y)
			id3.text += L" - " + std::to_wstring(c_dmg.y);
		if (l9_dmg.x != l9_dmg.y)
			id5.text += L" - " + std::to_wstring(l9_dmg.y);

		//id2		= DescriptionStruct(L"Cannon dmg: "			+ std::to_wstring(c_dmg.x) + L" - " + std::to_wstring(c_dmg.y),	color_red);
		//id3		= DescriptionStruct(L"", color_none);	//id3		= DescriptionStruct(L"Cannon cooldown: "	+ wss1.str() + L" sec.",											color_red);
		//id4		= DescriptionStruct(L"Long 9 dmg: "			+ std::to_wstring(l9_dmg.x) + L" - " + std::to_wstring(l9_dmg.y),	color_cyan);
		//id5		= DescriptionStruct(L"", color_none);	//id5		= DescriptionStruct(L"Long 9 cooldown: "	+ wss2.str() + L" sec.",											color_cyan);
		break;

	case PortItems::SAILS:
		s_lvl	= player->GetMastLevel();
		max_spd	= player->GetMaxSpeed();
		//wss1.precision(2);
		wss1 << (max_spd * 2.0f);

		id1		= (s_lvl < max_lvl)
					? DescriptionStruct(L"Our sails can get better Cap'n!",	color_yellow)
					: DescriptionStruct(L"Our sails are the best Cap'n!",		color_green);
		id2		= DescriptionStruct(L"", color_none);
		id3		= DescriptionStruct(L"Max speed: " + wss1.str(),			color_red);
		id4		= DescriptionStruct(L"", color_none);
		id5		= DescriptionStruct(L"", color_none);
		break;

	case PortItems::HULL:
		h_lvl		= player->GetHullLevel();
		health.x	= player->GetMaxHealth();
		food.x		= player->GetMaxWaterFood();
		rum.x		= player->GetMaxRum();
		crew.x		= player->GetMaxCrew();
		trn_rte		= player->GetTurnSpeedLimit();
		wss1.precision(3);
		wss1 << trn_rte;

		id1		= (h_lvl < max_lvl)
					? DescriptionStruct(L"Our hull can get better Cap'n!",		color_yellow)
					: DescriptionStruct(L"Our hull is the best Cap'n!",				color_green);
		id2		= DescriptionStruct(L"Max HP: "			+ std::to_wstring(health.x),	color_red);
		id3		= DescriptionStruct(L"Max food/rum: "	+ std::to_wstring(food.x),		color_blue);
		id4		= DescriptionStruct(L"Max crew: "		+ std::to_wstring(crew.x),		color_cyan);
		id5		= DescriptionStruct(L"Turn rate: "		+ wss1.str(),		color_white);
		break;



		// Page 3
	case PortItems::FLAMETHROWER:
	case PortItems::BARRELS:
	case PortItems::GREEKSHOT:
		// Page 4
	case PortItems::FLEET:
		break;



	case PortItems::EXIT:
		id1	= DescriptionStruct(L"", color_none);
		id2	= DescriptionStruct(L"", color_none);
		id3	= DescriptionStruct(L"Ready to set sail Cap'n?", color_cyan);
		id4	= DescriptionStruct(L"", color_none);
		id5	= DescriptionStruct(L"", color_none);
		break;



		// etc...
	case PortItems::UNKNOWN:
	default:
		break;
	}




	static_cast<G2D::GUIText*>(m_hudElems["ItemDesc1"])->Text(id1.text);
	static_cast<G2D::GUIText*>(m_hudElems["ItemDesc2"])->Text(id2.text);
	static_cast<G2D::GUIText*>(m_hudElems["ItemDesc3"])->Text(id3.text);
	static_cast<G2D::GUIText*>(m_hudElems["ItemDesc4"])->Text(id4.text);
	static_cast<G2D::GUIText*>(m_hudElems["ItemDesc5"])->Text(id5.text);

	static_cast<G2D::GUIText*>(m_hudElems["ItemDesc1"])->Color(id1.color);
	static_cast<G2D::GUIText*>(m_hudElems["ItemDesc2"])->Color(id2.color);
	static_cast<G2D::GUIText*>(m_hudElems["ItemDesc3"])->Color(id3.color);
	static_cast<G2D::GUIText*>(m_hudElems["ItemDesc4"])->Color(id4.color);
	static_cast<G2D::GUIText*>(m_hudElems["ItemDesc5"])->Color(id5.color);
}


void PortMenuScene::StartFeedbackColorChange(std::string guiname)
{
	if (m_feedbackElems.find(guiname) != m_feedbackElems.end())
	{
		m_feedbackElems[guiname]->Restart();
		return;
	}

	m_feedbackElems[guiname] = new FeedbackColorChange(static_cast<G2D::GUIText*>(m_hudElems[guiname]));
}


void PortMenuScene::UpdateFeedbackElements(float dt)
{
	// release feedback elems
	auto iter = m_feedbackElems.begin();
	auto end = m_feedbackElems.end();

	for (; iter != end; iter++)
	{
		// already at max
		if (iter->second->colorTimer == COLOR_CHANGE_TIMER)
			continue;

		// timer++
		iter->second->colorTimer += dt;

		// cap timer
		if (iter->second->colorTimer > COLOR_CHANGE_TIMER)
			iter->second->colorTimer = COLOR_CHANGE_TIMER;

		// lerp
		iter->second->currColor = SimpleMath::Color::Lerp(iter->second->startColor, iter->second->endColor, (iter->second->colorTimer / COLOR_CHANGE_TIMER));

		// change color
		iter->second->text->Color(iter->second->currColor);
	}
}


void PortMenuScene::ReleaseFeedbackElements(void)
{
	// release feedback elems
	auto iter = m_feedbackElems.begin();
	auto end = m_feedbackElems.end();

	for (; iter != end; iter++)
	{
		iter->second->text->Color(iter->second->endColor);
		delete iter->second;
		iter->second = nullptr;
	}

	m_feedbackElems.clear();
}


void PortMenuScene::Upgrade(PortItems item, IShipPart::PartType part)
{
	GameplayScene*	gps = ((GameplayScene*)sceneManager->GetScene(IScene::SceneType::GAMEPLAY));
	SoundManager*	sound = ((CGame*)CGame::GetApplication())->GetSoundManager();
	PlayerShip*		player = static_cast<PlayerShip*>(gps->playerShip);

	bool purchased = player->Purchase(prices[item]);
	if (purchased == false)
	{
		sound->Play3DSound(AK::EVENTS::PLAYREJECT);
		return;
	}


	int	 prevlevel = player->GetShipPartLevel(part);
	bool leveledup = player->LevelUp(part);
	if (leveledup == false)
	{
		sound->Play3DSound(AK::EVENTS::PLAYREJECT);
		return;
	}


	// play animation
	m_hudAnims.Animation("GUIAnim_ChestOpen")->PlayAnimation();

	// purchase sound
	sound->Play3DSound(AK::EVENTS::PLAYUPGRADE);

	// $--
	player->DecreaseGold(prices[item]);

	// cost++
	if (onsale == true)
	{
		prices[item] = GetCurrentPrice(item, prevlevel + 1);
		/*
		ResetPrice(item);

		int currlevel = prevlevel + 1;
		for (int i = 0; i < currlevel; i++)
			prices[item] += priceOffsets[i];
		*/

		prices[item] = CalculateDiscountPrice(prices[item], discounts[(int)item]);
	}
	else
	{
		prices[item] += priceOffsets[prevlevel];
	}




	// feedback
	switch (item)
	{
	case PortItems::WEAPONS:
		StartFeedbackColorChange("WeaponLevel");
		break;

	case PortItems::SAILS:
		StartFeedbackColorChange("SailLevel");
		break;

	case PortItems::HULL:
		StartFeedbackColorChange("HullLevel");
		break;

	default:
		break;
	}

	if ( 5 == player->GetHullLevel() + player->GetMastLevel() + player->GetWeaponsLevel())
	{
		sceneManager->GetEntityManager().RemoveEntity(IScene::SceneType::GAMEPLAY, EntityManager::EntityBucket::RENDERABLE, player);
		player->ChangeRenderMesh("PlayerShip2.mesh", 0, 1);
		player->ChangeRenderTexture("PlayerShip2Diffuse.dds");
		static_cast<BoxCollider*>(player->GetCollider())->setDimensions(XMFLOAT3(0.85f, 1.0f, 2.2f));
		static_cast<BoxCollider*>(player->GetCollider())->setOffset(XMFLOAT3(0, 0.242f, -0.255f));
		sceneManager->GetEntityManager().AddEntity(IScene::SceneType::GAMEPLAY, EntityManager::EntityBucket::RENDERABLE, player);
	}
	else if (10 == player->GetHullLevel() + player->GetMastLevel() + player->GetWeaponsLevel())
	{
		sceneManager->GetEntityManager().RemoveEntity(IScene::SceneType::GAMEPLAY, EntityManager::EntityBucket::RENDERABLE, player);
		player->ChangeRenderMesh("PlayerShip3.mesh", 0, 1);
		player->ChangeRenderTexture("PlayerShip3Diffuse.dds");
		static_cast<BoxCollider*>(player->GetCollider())->setDimensions(XMFLOAT3(0.85f, 1.1f, 3.1f));
		static_cast<BoxCollider*>(player->GetCollider())->setOffset(XMFLOAT3(0, 0.28f, -0.584f));
		sceneManager->GetEntityManager().AddEntity(IScene::SceneType::GAMEPLAY, EntityManager::EntityBucket::RENDERABLE, player);
	}
	
	
}


void PortMenuScene::Repair(void)
{
	GameplayScene*	gps = ((GameplayScene*)sceneManager->GetScene(IScene::SceneType::GAMEPLAY));
	SoundManager*	sound = ((CGame*)CGame::GetApplication())->GetSoundManager();
	PlayerShip*		player = static_cast<PlayerShip*>(gps->playerShip);


	bool purchased = player->Purchase(prices[PortItems::REPAIR]);
	if (purchased == false)
	{
		sound->Play3DSound(AK::EVENTS::PLAYREJECT);
		return;
	}


	bool r1 = player->Repair(IShipPart::PartType::HULL);
	bool r2 = player->Repair(IShipPart::PartType::MAST);
	bool r3 = player->Repair(IShipPart::PartType::WEAPON);

	if (r1 == true || r2 == true || r3 == true)
	{
		// play animation
		m_hudAnims.Animation("GUIAnim_ChestOpen")->PlayAnimation();

		// purchase sound
		sound->Play3DSound(AK::EVENTS::PLAYUPGRADE);

		// $--
		player->DecreaseGold(prices[PortItems::REPAIR]);


		//change to use events
		((PlayerController*)player->GetController())->ResetHealth();
		
		// reset cost
		prices[PortItems::REPAIR] = 0;
		return;
	}


	// reject sound
	sound->Play3DSound(AK::EVENTS::PLAYREJECT);
}


void PortMenuScene::Refill(PortItems item)
{
	GameplayScene*	gps = ((GameplayScene*)sceneManager->GetScene(IScene::SceneType::GAMEPLAY));
	SoundManager*	sound = ((CGame*)CGame::GetApplication())->GetSoundManager();
	PlayerShip*		player = static_cast<PlayerShip*>(gps->playerShip);


	// able to purchase?
	bool purchased = player->Purchase(prices[item]);
	if (purchased == false)
	{
		sound->Play3DSound(AK::EVENTS::PLAYREJECT);
		return;
	}


	// able to refill?
	int		curr		= 0;
	int		max			= 0;
	bool	refilled	= false;
	switch (item)
	{
	case PortItems::WATER_FOOD:
		refilled	= player->RefillFood(incrementNums[resourceIncrements[0]]);	//gps->playerShip->RefillFood();
		curr		= player->GetCurrWaterFood();
		max			= player->GetMaxWaterFood();
		break;
	case PortItems::RUM:
		refilled	= player->RefillRum(incrementNums[resourceIncrements[1]]);		//gps->playerShip->RefillRum();
		curr		= player->GetCurrRum();
		max			= player->GetMaxRum();
		break;
	case PortItems::CREW:
		refilled	= player->RefillCrew(incrementNums[resourceIncrements[2]]);	//gps->playerShip->RefillCrew();
		curr		= player->GetCurrCrew();
		max			= player->GetMaxCrew();
		break;
	default:
		break;
	}



	// refilled?
	if (refilled == true)
	{
		// play animation
		m_hudAnims.Animation("GUIAnim_ChestOpen")->PlayAnimation();

		// purchase sound
		sound->Play3DSound(AK::EVENTS::PLAYUPGRADE);

		// $--
		player->DecreaseGold(prices[item]);

		// reset cost?
		if (curr == max)
			prices[item] = 0;
		return;
	}


	// reject sound
	sound->Play3DSound(AK::EVENTS::PLAYREJECT);
}


void PortMenuScene::SpecialWeapons(PortItems item)
{
	GameplayScene*	gps = ((GameplayScene*)sceneManager->GetScene(IScene::SceneType::GAMEPLAY));
	SoundManager*	sound = ((CGame*)CGame::GetApplication())->GetSoundManager();
	PlayerShip*		player = static_cast<PlayerShip*>(gps->playerShip);

	sound->Play3DSound(AK::EVENTS::PLAYREJECT);
	return;



	bool purchased = player->Purchase(prices[item]);
	if (purchased == false)
	{
		sound->Play3DSound(AK::EVENTS::PLAYREJECT);
		return;
	}


	bool getweapon = false;
	
	switch (item)
	{
	case PortItems::FLAMETHROWER:
		//getweapon = player->RefillFood(10);
		break;

	case PortItems::BARRELS:
		//getweapon = player->RefillRum(10);
		break;

	case PortItems::GREEKSHOT:
		//getweapon = player->RefillCrew(10);
		break;
	default:
		break;
	}


	if (getweapon == true)
	{
		// play animation
		m_hudAnims.Animation("GUIAnim_ChestOpen")->PlayAnimation();

		// purchase sound
		sound->Play3DSound(AK::EVENTS::PLAYUPGRADE);

		// $--
		player->DecreaseGold(prices[item]);

		//change to use events
		((PlayerController*)player->GetController())->ResetHealth();
		
		// reset cost
		prices[item] = 0;
		return;
	}


	// reject sound
	sound->Play3DSound(AK::EVENTS::PLAYREJECT);
}


void PortMenuScene::FleetShip(void)
{
	return;
	GameplayScene*	gps = ((GameplayScene*)sceneManager->GetScene(IScene::SceneType::GAMEPLAY));
	SoundManager*	sound = ((CGame*)CGame::GetApplication())->GetSoundManager();
	PlayerShip*		player = static_cast<PlayerShip*>(gps->playerShip);


	bool purchased = player->Purchase(prices[PortItems::FLEET]);
	if (purchased == false)
	{
		sound->Play3DSound(AK::EVENTS::PLAYREJECT);
		return;
	}


	bool add_ship = false;	//gps->playerShip->AddFleetShip();
	if (add_ship == true)
	{
		// play animation
		m_hudAnims.Animation("GUIAnim_ChestOpen")->PlayAnimation();

		// purchase sound
		sound->Play3DSound(AK::EVENTS::PLAYUPGRADE);

		// $--
		player->DecreaseGold(prices[PortItems::FLEET]);

		// reset cost
		prices[PortItems::FLEET] = 0;
		return;
	}


	// reject sound
	sound->Play3DSound(AK::EVENTS::PLAYREJECT);
}





bool PortMenuScene::New_Input_Keyboard(void)
{
	CGame*							game	= (CGame*)CGame::GetApplication();
	auto							kbs		= Keyboard::Get().GetState();
	Keyboard::KeyboardStateTracker*	kbst	= game->GetKeyboardTracker();
	SoundManager*					sound	= game->GetSoundManager();


	/* Input: backspace */
	if (kbst->IsKeyPressed(Keyboard::Keys::Back) == true)
	{
		switch (currPage)
		{
			// page 1
		case 0:
			currCursor = 4;
			break;

			// pages 2-3
		case 1:
		case 2:
			currCursor = 3;
			break;

			// page 4
		case 3:
			break;

		default:
			break;
		}

		sound->Play3DSound(AK::EVENTS::PLAYCURSOR);
		//sceneManager->PopScene();
		//return NEW_SCENE;
		m_hudAnims.Animation("GUIAnim_ChestOpen")->ResetAnimation();
	}




	/* Input: E */
	if (kbst->IsKeyPressed(Keyboard::Keys::E) == true)
	{
		currPage	= (currPage + 1 < numPagesOpen) ? (currPage + 1) : 0;	//(currPage + 1 < NUM_PORT_MENU_PAGES) ? (currPage + 1) : 0;
		currCursor	= 0;
		m_hudAnims.Animation("GUIAnim_ChestOpen")->ResetAnimation();
	}


	/* Input: Q */
	if (kbst->IsKeyPressed(Keyboard::Keys::Q) == true)
	{
		currPage	= (currPage - 1 >= 0) ? (currPage - 1) : (numPagesOpen - 1);	//currPage	= (currPage - 1 >= 0) ? (currPage - 1) : (NUM_PORT_MENU_PAGES - 1);
		currCursor	= 0;
		m_hudAnims.Animation("GUIAnim_ChestOpen")->ResetAnimation();
	}
	/*
	//Input: 1
	if (kbst->IsKeyPressed(Keyboard::Keys::D1) == true)
	{
		if (currPage != 0)
		{
			currPage = 0;
			currCursor = 0;
		}
	}

	// Input: 2
	if (kbst->IsKeyPressed(Keyboard::Keys::D2) == true)
	{
		if (numPagesOpen >= 2 && currPage != 1)
		{
			currPage = 1;
			currCursor = 0;
		}
	}

	// Input: 3
	if (kbst->IsKeyPressed(Keyboard::Keys::D3) == true)
	{
		if (numPagesOpen >= 3 && currPage != 2)
		{
			currPage = 2;
			currCursor = 0;
		}
	}

	// Input: 4
	if (kbst->IsKeyPressed(Keyboard::Keys::D4) == true)
	{
		if (numPagesOpen >= 4 && currPage != 3)
		{
			currPage = 3;
			currCursor = 0;
		}
	}
	*/



	// Page[x] input
	switch (currPage)
	{
		// page 1: resources & repairs
	case 0:
		return Page1_Input_Keyboard();
		break;

		// page 2: upgrades
	case 1:
		return Page2_Input_Keyboard();
		break;

		// page 3: special weapons
	case 2:
		return Page3_Input_Keyboard();
		break;

		// page 4: fleets
	case 3:
		return Page4_Input_Keyboard();
		break;

	default:
		break;
	}



	// stay in same scene
	return OLD_SCENE;
}


bool PortMenuScene::Page1_Input_Keyboard(void)
{
	CGame*							game	= (CGame*)CGame::GetApplication();
	auto							kbs		= Keyboard::Get().GetState();
	Keyboard::KeyboardStateTracker*	kbst	= game->GetKeyboardTracker();
	SoundManager*					sound	= game->GetSoundManager();
	fsEventManager*					fsem	= fsEventManager::GetInstance();
	CursorUpDownEventArgs			ea1		= CursorUpDownEventArgs(NUM_CHOICES_PAGE_1, true);
	CursorLeftRightEventArgs		ea2		= CursorLeftRightEventArgs(GetCurrPortItem(), true);


	/* Input: move cursor down (w/ wrap around) */
	if (kbst->IsKeyPressed(Keyboard::Keys::Down) == true || kbst->IsKeyPressed(Keyboard::Keys::NumPad2) == true || kbst->IsKeyPressed(Keyboard::Keys::S) == true)
	{
		fsem->FireEvent(fsEvents::PortCursorDown,&EventArgs1<CursorUpDownEventArgs>(ea1));
	}

	/* Input: move cursor up  (w/ wrap around) */
	if (kbst->IsKeyPressed(Keyboard::Keys::Up) == true || kbst->IsKeyPressed(Keyboard::Keys::NumPad8) == true || kbst->IsKeyPressed(Keyboard::Keys::W) == true)
	{
		fsem->FireEvent(fsEvents::PortCursorUp,&EventArgs1<CursorUpDownEventArgs>(ea1));
	}


	/* Input: decrement option */
	if (kbst->IsKeyPressed(Keyboard::Keys::Left) == true || kbst->IsKeyPressed(Keyboard::Keys::NumPad4) == true || kbst->IsKeyPressed(Keyboard::Keys::A) == true)
	{
		//fsem->FireEvent(fsEvents::PortCursorLeft, &EventArgs1<CursorLeftRightEventArgs>(ea2));
	}

	/* Input: increment option */
	if (kbst->IsKeyPressed(Keyboard::Keys::Right) == true || kbst->IsKeyPressed(Keyboard::Keys::NumPad6) == true || kbst->IsKeyPressed(Keyboard::Keys::D) == true)
	{
		//fsem->FireEvent(fsEvents::PortCursorRight,&EventArgs1<CursorLeftRightEventArgs>(ea2));
	}



	/* Input: select curr menu option */
	if (kbst->IsKeyPressed(Keyboard::Keys::Enter) == true)
	{
		switch (currCursor)
		{
			// water/food
		case 0:
			Refill(PortItems::WATER_FOOD);
			break;

			// rum
		case 1:
			Refill(PortItems::RUM);
			break;

			// crew
		case 2:
			Refill(PortItems::CREW);
			break;

			// repair
		case 3:
			Repair();
			break;

			// exit
		case 4:
			sceneManager->PopScene();
			return NEW_SCENE;
			break;

		default:
			break;
		}
	}



	// stay in same scene
	return OLD_SCENE;
}


bool PortMenuScene::Page2_Input_Keyboard(void)
{
	CGame*							game	= (CGame*)CGame::GetApplication();
	auto							kbs		= Keyboard::Get().GetState();
	Keyboard::KeyboardStateTracker*	kbst	= game->GetKeyboardTracker();
	SoundManager*					sound	= game->GetSoundManager();
	fsEventManager*					fsem	= fsEventManager::GetInstance();
	CursorUpDownEventArgs			ev_args	= CursorUpDownEventArgs(NUM_CHOICES_PAGE_2, true);


	/* Input: move cursor down (w/ wrap around) */
	if (kbst->IsKeyPressed(Keyboard::Keys::Down) == true || kbst->IsKeyPressed(Keyboard::Keys::NumPad2) == true || kbst->IsKeyPressed(Keyboard::Keys::S) == true)
	{
		fsem->FireEvent(fsEvents::PortCursorDown, &EventArgs1<CursorUpDownEventArgs>(ev_args));
	}

	/* Input: move cursor up  (w/ wrap around) */
	if (kbst->IsKeyPressed(Keyboard::Keys::Up) == true || kbst->IsKeyPressed(Keyboard::Keys::NumPad8) == true || kbst->IsKeyPressed(Keyboard::Keys::W) == true)
	{
		fsem->FireEvent(fsEvents::PortCursorUp, &EventArgs1<CursorUpDownEventArgs>(ev_args));
	}


	/* Input: select curr menu option */
	if (kbst->IsKeyPressed(Keyboard::Keys::Enter) == true)
	{
		switch (currCursor)
		{
			// weapons
		case 0:
			Upgrade(PortItems::WEAPONS, IShipPart::PartType::WEAPON);
			break;

			// sails
		case 1:
			Upgrade(PortItems::SAILS, IShipPart::PartType::MAST);
			break;

			// hull
		case 2:
			Upgrade(PortItems::HULL, IShipPart::PartType::HULL);
			break;

			// exit
		case 3:
			sceneManager->PopScene();
			return NEW_SCENE;
			break;

		default:
			break;
		}
	}



	// stay in same scene
	return OLD_SCENE;
}


bool PortMenuScene::Page3_Input_Keyboard(void)
{
	CGame*							game	= (CGame*)CGame::GetApplication();
	auto							kbs		= Keyboard::Get().GetState();
	Keyboard::KeyboardStateTracker*	kbst	= game->GetKeyboardTracker();
	SoundManager*					sound	= game->GetSoundManager();
	fsEventManager*					fsem	= fsEventManager::GetInstance();
	CursorUpDownEventArgs			ev_args = CursorUpDownEventArgs(NUM_CHOICES_PAGE_3, true);


	/* Input: move cursor down (w/ wrap around) */
	if (kbst->IsKeyPressed(Keyboard::Keys::Down) == true || kbst->IsKeyPressed(Keyboard::Keys::NumPad2) == true || kbst->IsKeyPressed(Keyboard::Keys::S) == true)
	{
		fsem->FireEvent(fsEvents::PortCursorDown, &EventArgs1<CursorUpDownEventArgs>(ev_args));
	}

	/* Input: move cursor up  (w/ wrap around) */
	if (kbst->IsKeyPressed(Keyboard::Keys::Up) == true || kbst->IsKeyPressed(Keyboard::Keys::NumPad8) == true || kbst->IsKeyPressed(Keyboard::Keys::W) == true)
	{
		fsem->FireEvent(fsEvents::PortCursorUp, &EventArgs1<CursorUpDownEventArgs>(ev_args));
	}


	/* Input: select curr menu option */
	if (kbst->IsKeyPressed(Keyboard::Keys::Enter) == true)
	{
		switch (currCursor)
		{
			// flamethrower
		case 0:
			SpecialWeapons(PortItems::FLAMETHROWER);
			break;

			// explosive barrels
		case 1:
			SpecialWeapons(PortItems::BARRELS);
			break;

			// greek shot
		case 2:
			SpecialWeapons(PortItems::GREEKSHOT);
			break;

			// exit
		case 3:
			sceneManager->PopScene();
			return NEW_SCENE;
			break;

		default:
			break;
		}
	}



	// stay in same scene
	return OLD_SCENE;
}


bool PortMenuScene::Page4_Input_Keyboard(void)
{
	// stay in same scene
	return OLD_SCENE;
}





bool PortMenuScene::New_Input_Gamepad(void)
{
	CGame*							game	= (CGame*)CGame::GetApplication();
	auto							gps		= GamePad::Get().GetState(0);
	GamePad::ButtonStateTracker*	gpst	= game->GetGamePadTracker();
	SoundManager*					sound	= game->GetSoundManager();


	/* Input: B */
	if (gpst->b == GamePad::ButtonStateTracker::ButtonState::PRESSED)
	{
		switch (currPage)
		{
			// page 1
		case 0:
			currCursor = 4;
			break;

			// pages 2-3
		case 1:
		case 2:
			currCursor = 3;
			break;

			// page 4
		case 3:
			break;

		default:
			break;
		}

		sound->Play3DSound(AK::EVENTS::PLAYCURSOR);
		//sceneManager->PopScene();
		//return NEW_SCENE;
		m_hudAnims.Animation("GUIAnim_ChestOpen")->ResetAnimation();
	}



	/* Input: RT */
	if (gpst->rightShoulder == GamePad::ButtonStateTracker::ButtonState::PRESSED)
	{
		currPage	= (currPage + 1 < numPagesOpen) ? (currPage + 1) : 0;	//(currPage + 1 < NUM_PORT_MENU_PAGES) ? (currPage + 1) : 0;
		currCursor	= 0;
		m_hudAnims.Animation("GUIAnim_ChestOpen")->ResetAnimation();
	}


	/* Input: LT */
	if (gpst->leftShoulder == GamePad::ButtonStateTracker::ButtonState::PRESSED)
	{
		currPage	= (currPage - 1 >= 0) ? (currPage - 1) : (numPagesOpen - 1);	//currPage	= (currPage - 1 >= 0) ? (currPage - 1) : (NUM_PORT_MENU_PAGES - 1);
		currCursor	= 0;
		m_hudAnims.Animation("GUIAnim_ChestOpen")->ResetAnimation();
	}



	// Page[x] input
	switch (currPage)
	{
		// page 1: resources & repairs
	case 0:
		return Page1_Input_Gamepad();
		break;

		// page 2: upgrades
	case 1:
		return Page2_Input_Gamepad();
		break;

		// page 3: special weapons
	case 2:
		return Page3_Input_Gamepad();
		break;

		// page 4: fleets
	case 3:
		return Page4_Input_Gamepad();
		break;

	default:
		break;
	}



	// stay in same scene
	return OLD_SCENE;
}


bool PortMenuScene::Page1_Input_Gamepad(void)
{
	CGame*							game	= (CGame*)CGame::GetApplication();
	auto							gps		= GamePad::Get().GetState(0);
	GamePad::ButtonStateTracker*	gpst	= game->GetGamePadTracker();
	SoundManager*					sound	= game->GetSoundManager();
	fsEventManager*					fsem	= fsEventManager::GetInstance();
	CursorUpDownEventArgs			ea1		= CursorUpDownEventArgs(NUM_CHOICES_PAGE_1, false);
	CursorLeftRightEventArgs		ea2		= CursorLeftRightEventArgs(GetCurrPortItem(), false);



	/* Input: move cursor down (w/ wrap around) */
	if (gps.IsLeftThumbStickDown() == true || gps.IsDPadDownPressed() == true)
	{
		if (scrollTimer <= 0.0f)
			fsem->FireEvent(fsEvents::PortCursorDown,&EventArgs1<CursorUpDownEventArgs>(ea1));
	}

	/* Input: move cursor up  (w/ wrap around) */
	if (gps.IsLeftThumbStickUp() == true || gps.IsDPadUpPressed() == true)
	{
		if (scrollTimer <= 0.0f)
			fsem->FireEvent(fsEvents::PortCursorUp, & EventArgs1<CursorUpDownEventArgs>(ea1));
	}


	/* Input: decrement option */
	if (gps.IsLeftThumbStickLeft() == true || gps.IsDPadLeftPressed() == true)
	{
		//if (scrollTimer <= 0.0f)
		//	fsem->FireEvent(fsEvents::PortCursorLeft, &EventArgs1<CursorLeftRightEventArgs>(ea2));
	}

	/* Input: increment option */
	if (gps.IsLeftThumbStickRight() == true || gps.IsDPadRightPressed() == true)
	{
		//if (scrollTimer <= 0.0f)
		//	fsem->FireEvent(fsEvents::PortCursorRight, &EventArgs1<CursorLeftRightEventArgs>(ea2));
	}



	/* Input: select curr menu option */
	if (gpst->a == GamePad::ButtonStateTracker::ButtonState::PRESSED)
	{
		switch (currCursor)
		{
			// water/food
		case 0:
			Refill(PortItems::WATER_FOOD);
			break;

			// rum
		case 1:
			Refill(PortItems::RUM);
			break;

			// crew
		case 2:
			Refill(PortItems::CREW);
			break;

			// repair
		case 3:
			Repair();
			break;

			// exit
		case 4:
			sceneManager->PopScene();
			return NEW_SCENE;
			break;

		default:
			break;
		}
	}



	// stay in same scene
	return OLD_SCENE;
}


bool PortMenuScene::Page2_Input_Gamepad(void)
{
	CGame*							game	= (CGame*)CGame::GetApplication();
	auto							gps		= GamePad::Get().GetState(0);
	GamePad::ButtonStateTracker*	gpst	= game->GetGamePadTracker();
	SoundManager*					sound	= game->GetSoundManager();
	fsEventManager*					fsem	= fsEventManager::GetInstance();
	CursorUpDownEventArgs			ev_args	= CursorUpDownEventArgs(NUM_CHOICES_PAGE_2, false);



	/* Input: move cursor down (w/ wrap around) */
	if (gps.IsLeftThumbStickDown() == true || gps.IsDPadDownPressed() == true)
	{
		if (scrollTimer <= 0.0f)
			fsem->FireEvent(fsEvents::PortCursorDown, &EventArgs1<CursorUpDownEventArgs>(ev_args));
	}

	/* Input: move cursor up  (w/ wrap around) */
	if (gps.IsLeftThumbStickUp() == true || gps.IsDPadUpPressed() == true)
	{
		if (scrollTimer <= 0.0f)
			fsem->FireEvent(fsEvents::PortCursorUp, &EventArgs1<CursorUpDownEventArgs>(ev_args));
	}



	/* Input: select curr menu option */
	if (gpst->a == GamePad::ButtonStateTracker::ButtonState::PRESSED)
	{
		// upgrade player
		switch (currCursor)
		{
			// weapons
		case 0:
			Upgrade(PortItems::WEAPONS, IShipPart::PartType::WEAPON);
			break;

			// sails
		case 1:
			Upgrade(PortItems::SAILS, IShipPart::PartType::MAST);
			break;

			// hull
		case 2:
			Upgrade(PortItems::HULL, IShipPart::PartType::HULL);
			break;

			// exit
		case 3:
			sceneManager->PopScene();
			return NEW_SCENE;
			break;

		default:
			break;
		}
	}



	// stay in same scene
	return OLD_SCENE;
}


bool PortMenuScene::Page3_Input_Gamepad(void)
{
	CGame*							game	= (CGame*)CGame::GetApplication();
	auto							gps		= GamePad::Get().GetState(0);
	GamePad::ButtonStateTracker*	gpst	= game->GetGamePadTracker();
	SoundManager*					sound	= game->GetSoundManager();
	fsEventManager*					fsem	= fsEventManager::GetInstance();
	CursorUpDownEventArgs			ev_args	= CursorUpDownEventArgs(NUM_CHOICES_PAGE_3, false);



	/* Input: move cursor down (w/ wrap around) */
	if (gps.IsLeftThumbStickDown() == true || gps.IsDPadDownPressed() == true)
	{
		if (scrollTimer <= 0.0f)
			fsem->FireEvent(fsEvents::PortCursorDown, &EventArgs1<CursorUpDownEventArgs>(ev_args));
	}

	/* Input: move cursor up  (w/ wrap around) */
	if (gps.IsLeftThumbStickUp() == true || gps.IsDPadUpPressed() == true)
	{
		if (scrollTimer <= 0.0f)
			fsem->FireEvent(fsEvents::PortCursorUp, &EventArgs1<CursorUpDownEventArgs>(ev_args));
	}


	/* Input: select curr menu option */
	if (gpst->a == GamePad::ButtonStateTracker::ButtonState::PRESSED)
	{
		// upgrade player
		switch (currCursor)
		{
			// flamethrower
		case 0:
			SpecialWeapons(PortItems::FLAMETHROWER);
			break;

			// explosive barrels
		case 1:
			SpecialWeapons(PortItems::BARRELS);
			break;

			// greek shot
		case 2:
			SpecialWeapons(PortItems::GREEKSHOT);
			break;

			// exit
		case 3:
			sceneManager->PopScene();
			return NEW_SCENE;
			break;

		default:
			break;
		}
	}



	// stay in same scene
	return OLD_SCENE;
}


bool PortMenuScene::Page4_Input_Gamepad(void)
{
	// stay in same scene
	return OLD_SCENE;
}






void PortMenuScene::PortSetupEvent(const EventArgs1<PortSetupEventArgs>& args)
{
	PortSetupEventArgs	ea	= args.m_Data;


	// rename port
	SetPortName(ea.currPort->GetPortName());


	// limit pages
	numPagesOpen = ea.currPort->GetNumPages();


	// having a sale?
	onsale = ea.currPort->ActiveSale();


	// setup sale prices
	if (onsale == true)
	{
		m_hudElems["DiscountIcon"]->Active(true);
		SetupDiscountPrices(ea.currPort);
		return;
	}


	// setup non-sale prices
	m_hudElems["DiscountIcon"]->Active(false);
	SetupStandardPrices();
}


void PortMenuScene::CursorIncreaseEvent(const EventArgs1<CursorUpDownEventArgs>& args)
{
	CGame*					game	= (CGame*)CGame::GetApplication();
	SoundManager*			sound	= game->GetSoundManager();
	CursorUpDownEventArgs	ea		= args.m_Data;


	// play sound
	sound->Play3DSound(AK::EVENTS::PLAYCURSOR);

	// reset input delay on controller
	if (ea.keyboard == false)
		scrollTimer = SCENE_SCROLL_TIME;

	// move cursor
	currCursor = (currCursor + 1 < ea.numChoices) ? (currCursor + 1) : 0;

	// reset animation
	m_hudAnims.Animation("GUIAnim_ChestOpen")->ResetAnimation();
}


void PortMenuScene::CursorDecreaseEvent(const EventArgs1<CursorUpDownEventArgs>& args)
{
	CGame*					game	= (CGame*)CGame::GetApplication();
	SoundManager*			sound	= game->GetSoundManager();
	CursorUpDownEventArgs	ea		= args.m_Data;


	// play sound
	sound->Play3DSound(AK::EVENTS::PLAYCURSOR);

	// reset input delay on controller
	if (ea.keyboard == false)
		scrollTimer = SCENE_SCROLL_TIME;

	// move cursor
	currCursor = (currCursor - 1 >= 0) ? (currCursor - 1) : (ea.numChoices - 1);

	// reset animation
	m_hudAnims.Animation("GUIAnim_ChestOpen")->ResetAnimation();
}


void PortMenuScene::CursorIncrementEvent(const EventArgs1<CursorLeftRightEventArgs>& args)
{
	CGame*						game	= (CGame*)CGame::GetApplication();
	SoundManager*				sound	= game->GetSoundManager();
	CursorLeftRightEventArgs	ea		= args.m_Data;


	// bad index
	if (ea.item <= PortItems::UNKNOWN || ea.item >= PortItems::REPAIR)
		return;


	// play sound
	sound->Play3DSound(AK::EVENTS::PLAYCURSOR);

	// reset input delay on controller
	if (ea.keyboard == false)
		scrollTimer = SCENE_SCROLL_TIME;

	// increase increment
	switch (ea.item)
	{
	case PortItems::WATER_FOOD:
		resourceIncrements[0]++;
		if (resourceIncrements[0] >= NUM_RESOURCE_INCREMENTS)
			resourceIncrements[0] = (NUM_RESOURCE_INCREMENTS - 1);
		break;

	case PortItems::RUM:
		resourceIncrements[1]++;
		if (resourceIncrements[1] >= NUM_RESOURCE_INCREMENTS)
			resourceIncrements[1] = (NUM_RESOURCE_INCREMENTS - 1);
		break;

	case PortItems::CREW:
		resourceIncrements[2]++;
		if (resourceIncrements[2] >= NUM_RESOURCE_INCREMENTS)
			resourceIncrements[2] = (NUM_RESOURCE_INCREMENTS - 1);
		break;

	//case PortItems::UNKNOWN:
	//case PortItems::REPAIR:
	//case PortItems::WEAPONS:
	//case PortItems::SAILS:
	//case PortItems::HULL:
	//case PortItems::FLAMETHROWER:
	//case PortItems::BARRELS:
	//case PortItems::GREEKSHOT:
	//case PortItems::FLEET:
	//case PortItems::EXIT:
	default:
		break;
	}
}


void PortMenuScene::CursorDecrementEvent(const EventArgs1<CursorLeftRightEventArgs>& args)
{
	CGame*						game	= (CGame*)CGame::GetApplication();
	SoundManager*				sound	= game->GetSoundManager();
	CursorLeftRightEventArgs	ea		= args.m_Data;


	// bad index
	if (ea.item <= PortItems::UNKNOWN || ea.item >= PortItems::REPAIR)
		return;


	// play sound
	sound->Play3DSound(AK::EVENTS::PLAYCURSOR);

	// reset input delay on controller
	if (ea.keyboard == false)
		scrollTimer = SCENE_SCROLL_TIME;

	// increase increment
	switch (ea.item)
	{
	case PortItems::WATER_FOOD:
		resourceIncrements[0]--;
		if (resourceIncrements[0] < 0)
			resourceIncrements[0] = 0;
		break;

	case PortItems::RUM:
		resourceIncrements[1]--;
		if (resourceIncrements[1] < 0)
			resourceIncrements[1] = 0;
		break;

	case PortItems::CREW:
		resourceIncrements[2]--;
		if (resourceIncrements[2] < 0)
			resourceIncrements[2] = 0;
		break;

	//case PortItems::UNKNOWN:
	//case PortItems::REPAIR:
	//case PortItems::WEAPONS:
	//case PortItems::SAILS:
	//case PortItems::HULL:
	//case PortItems::FLAMETHROWER:
	//case PortItems::BARRELS:
	//case PortItems::GREEKSHOT:
	//case PortItems::FLEET:
	//case PortItems::EXIT:
	default:
		break;
	}
}