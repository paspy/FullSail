/***************************************************************
|	File:		GameplayScene.h
|	Author:		Jason N. Bloomfield
|	Created:	March 7, 2016
|	Modified:	March 18, 2016
|	Purpose:	GameplayScene class used to store gameplay data
***************************************************************/
#pragma once

#include "IScene.h"
#include "PhysicsManager.h"

#define NUM_AI_SHIPS			((int)0)
#define NUM_ISLANDS				((int)1)
#define NUM_PORTS				((int)1)
#define AMMO_LIMIT				((size_t)60)
#define FILTER_LIMIT			((size_t)10)
#define CULL_LIMIT				((float)30)
#define AI_SPAWN_LIMIT			((float)1)
#define ICON_HUD_TIME			((float)1.0f)
#define ICON_HUD_TIME_DELAY		((float)ICON_HUD_TIME/16.0f)

//typedef	unsigned long long	BitPool;

struct TResourcePickUpIcon
{
	G2D::Entity2D*	m_pTexture;
	float			m_fTimer;
	int				m_nResourceAmount;
};

class Entity3D;
class Ship;
class PlayerShip;
class AIShip;
class IAmmo;
class Port;
class Gull;
class Squad;
class Crate;
class CargoSquad;

class GameplayScene : public IScene
{
	friend class SceneManager;
	friend class PortMenuScene;


	// Event structs
	struct BossNextWaypointEventArgs
	{
		Squad* currSquad;
		Port* nextPort;

		BossNextWaypointEventArgs(void) : currSquad(nullptr), nextPort(nullptr) {}
		BossNextWaypointEventArgs(Squad * s, Port * p) : currSquad(s), nextPort(p) {}
	};


	// Other structs
	struct ShipTeamStruct
	{
		std::vector<Ship*>	fleet;
		Squad*				squad;

		ShipTeamStruct(void) : squad(nullptr) { fleet.clear(); }
		void Clear(void);
	};



public:
	GameplayScene();
	~GameplayScene();


	void		Enter	(void);
	void		Exit	(void);

	bool		Input	(void);
	void		Update	(float _dt);
	void		Render	(void);

	//SceneType	GetType	(void)	{ return SceneType::GAMEPLAY; }

	void		Input_Start	(void);
	void		Input_End	(void);

	bool		Input_Keyboard	(void);
	bool		Input_Gamepad	(void);
	void		Input_Cheats	(void);

    void        LoadHudList();
    void        ReleaseHudList();

	void		InitializeScene	(void);
	void		ShutdownScene	(void);

	void		SwapScenePush	(void);
	void		SwapScenePop	(void);


	PhysicsManager*					GetPhysicsManager	(void)		{ return &physicsManager; }
	bool							GetInitialized		(void)		{ return initialized; }
	std::vector<Ship*>&				GetAIShips			(void)		{ return aiShips; }
	inline  std::vector<Entity3D*>& GetIslands			(void)		{ return islands; }
	inline	std::vector<XMFLOAT3>&	GetWaypoints		(void)		{ return ai_waypoints; }
	int								GetNumActiveAIShips	(void);
	void							GetActiveAIShips	(std::vector<Ship*>& shipvec);
	Ship*							GetPlayerShip		(void)		{ return playerShip; }
	Port*							GetPort				(std::wstring name);
	inline	std::vector<Port*>&		GetPorts			(void)		{ return ports; }
	Squad*							GetBossSquad		(void)		{ return BossTeam.squad; }
	std::unordered_map<std::string, G2D::Entity2D*>& GetHUDElements(void) { return m_hudElems; }

	// Helpers/Other/Misc.... 
	void			ResetGameplay			(void);
	void			FilterVectors			(void);
	void			CullVectors				(float _dt);
    void			UpdateHUD				(float dt);
    bool			CheckGoToPauseScene		(void);
    bool			CheckGoToPortScene		(void);
    void			GoToPauseScene			(void) { gotopause = true;}
	void			GoToPortScene			(void) { gotoport = true; }
	void			ToggleHUDInstructions	(bool toggle);
	void			PauseSounds				(void);
	void			ResumeSounds			(void);
	void			StopSounds				(void);
	void			ActivateResourceDecrement(int rum, int food);
	void			TogglePortInstructions	(bool toggle);
	void			UpdatePortInstructions	(void);
	void			TriggerPortCollision	(void);
	void			ToggleLowResourcesGUI	(void);
	void			UpdateLowResourcesGUI	(float dt);
	bool			SpyglassActive			(void) { return spyglass;}
	void			ActivateSpyglass		(bool _active) { spyglass = _active;}
	void			AddBossShipAndSquad		(void);
	void			GetWorldWaypoints		(std::vector<XMFLOAT3>& out_ports, std::vector<XMFLOAT3>& out_triangles, std::vector<XMFLOAT3>& out_other);
	void			SortWorldWaypoints		(char entry_cardinal_direction, std::vector<XMFLOAT3>& out_waypoints);
	void			SortBossWaypoints		(std::vector<XMFLOAT3>& out_waypoints);
	void			ActivateBoardingPrompt	(void);





	// Event funtions
	void		SpawnReconSquadEvent				(const EventArgs& args);
	void		SpawnBossReinforcementSquadEvent	(const EventArgs& args);
	void		BossTrackingEvent					(const EventArgs1<Squad*>& args);
	void		BossHealingEvent					(const EventArgs& args);
	void		BossNextWaypointEvent				(const EventArgs1<BossNextWaypointEventArgs>& args);



	// Load functions
	void		LoadWater		(void);
	void		LoadPlayerShip	(void);
	Ship* 		LoadBossShip	(void);
	void		LoadScene		(void);
	void		LoadGulls		(void);
	void		LoadObject(const EventArgs2<const char*, XMFLOAT4X4>& loadInfo);

	void		CallInMilitary(const EventArgs1<Ship*>& Target);

	float		GetHudSpeedTimer(void)		{ return m_fHudSpeedTimer; }
	void		SetHudSpeedTimer(float val) { m_fHudSpeedTimer = val;  }

	std::unordered_map<std::string, G2D::Entity2D*> m_spyElems;

	void		AddHudResources( int _gold, int _rum, int _food );

	Ship* CreateAIShip(const char* _fileName, XMFLOAT3 &_pos);
private:
	bool						initialized;
	bool						gotopause;
	bool						gotoport;
	bool						hud_help;
	bool						InDarkWater = false;
	bool						spyglass = false;
	float						help_timer;

	PhysicsManager				physicsManager;
	std::vector<XMFLOAT3>		ai_waypoints;
	float						cull_timer;
	float						ai_spawn_timer;

	//// Turtorial
	////BoxCollider*				m_pFirstWall;
	//bool						m_bTurtorialActive = true;
	//bool						m_bPlayerHitPort = false;
	//Crate*						m_pFirstTurtorialCrate = nullptr;
	//Crate*						m_pSecondTurtorialCrate = nullptr;
	//Ship*						m_pFirstTurtorialShip = nullptr;
	//Ship*						m_pSecondTurtorialShip = nullptr;

	// Game entities
	Entity3D*					water		= nullptr;
	Ship*						playerShip	= nullptr;
	std::vector<Ship*>			aiShips;

	//*********************Fleets**********************//
	std::vector<Ship*>			MerchantFleet;
	std::vector<Ship*>			GoldFleet;
	std::vector<Ship*>			MilitaryFleet;
	ShipTeamStruct				BossTeam;

	ShipTeamStruct				reconTeam;
	ShipTeamStruct				bossReinforcementTeam;
	//std::vector<Ship*>			ReconFleet;
	//Squad*						ReconSquad;


	std::vector<Entity3D*>		islands;
	std::vector<Port*>			ports;
	std::vector<IAmmo*>			ammo;
	std::vector<Entity3D*>		crates;
	std::vector<Entity3D*>		weather;
	std::vector<Gull*>			gulls;
	std::vector<XMFLOAT3>		safeZones;
	std::vector<XMFLOAT3>		SpawnPoints;
	Squad* m_pReinforcement;

	//Cargo Ships
	float		m_cargoSquadSpawnTimer;
	float		m_cargoSquadTextTimer;
	CargoSquad* m_cargoSquad;

    // HUD
    std::unordered_map<std::string, G2D::Entity2D*> m_hudElems;
	G2D::CGUIManager spyGUI;
	float outofBoundTimer;
	float decTimer;
	bool port_help;
	bool m_bUpdateResourcesHud;
	float PromptTimer;
	float low_timer;
	float m_fHpLowTimer;
	bool m_bHpLowOnBlack;
	float m_fHudSpeedTimer;
	float m_fBossTextTimer;
	float m_fReconTextTimer;
	float m_fPortSaleTimer;
	std::vector<TResourcePickUpIcon*>	m_vRewards;
	bool m_bShowBossMinimap;
	bool m_bShowCargoMinimap;
	// Factory Methods
	void FindFarthestShip(std::vector<Ship*>& MoveAbleShip);
	void UpdateReinforcements(float _dt); 
	bool InHuntingParty(Ship* aiShip);
	void SetUpTradeRoutes();
	// Helpers/Other/Misc....
	void		PlayDarkwater(void);
	void		UpdateHudResources(float _deltaTime);

};

