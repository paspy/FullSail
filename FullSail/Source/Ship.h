#pragma once
#include "Entity3D.h"
#include "IShipPart.h"

#define RESOURCE_TIME	30.0f
#define BOARDING_TIME	10.0f
#define STARTING_BOARDING_TIME	5.0f
#define BOARDING_DIST	1.75f
#define BOARDING_SPEED	0.8f
#define DEATH_TIME		2.0f

#define INCREASE_SPEED  4.0f

#define MAX_BOARDING_ICON_TIMER 0.5f;	// used to be in Ship.cpp
#define MAX_ICON_TIMER 1.0f				// used to be in Ship.cpp

class IController;
class SideCannonHandler;
class Mast;

namespace Renderer
{
	class C3DHUD;
}


/********************This is the new base*************************/
class Ship : public Entity3D	//IEntity
{
	friend class AiController;
	friend class PlayerController;
	friend class Mast;
public:
	enum eShipType		{ TYPE_UNKNOWN = 0, TYPE_PLAYER, TYPE_AI, TYPE_BOSS, };
	enum eAlignment		{ UNKNOWN = -1, PLAYER = 0, ENEMY, OTHER };
	enum eBoardingState	{ UNBOARDABLE = -1, NONE = 0, BOARDING_MOVE, BOARDING_SHIP, BOARDING_INTERUPT, BOARDING_FINISHED, BOARDING_ENEMY };
	enum eDeathStages	{ ALIVE = 0, DEATH_ROTATE, DEATH_SINK, DEATH_DONE };

	Ship(const XMFLOAT4X4 & _newTransfrom, std::string meshName, std::string textureName);

	//****************Trilogy*****************//
	//You must set the controller and Collider
	Ship& operator=(const Ship& other);
	//You must set the controller and Collider
	Ship(const Ship& _cpy);
	virtual ~Ship();


	//**************Functions****************//
	virtual void	Update(float dt);
	virtual void	OnCollision(Collider& other);
	virtual void	DeathUpdate(float dt);
	virtual bool	LevelUp(IShipPart::PartType partType);

	bool			Repair(IShipPart::PartType partType);
	void			StopColliders(void);
	void			StartColliders(void);

	//****************************************//
	/*Movement*/
	//****************************************//
	virtual void	FlipSailsDown();
	void			FlipComingBackFromRuinedSails() { m_comingBackFromRuinedSails = !m_comingBackFromRuinedSails; }
	void			TurnDegrees(float _degrees, float _dt);
	void			TurnRadians(float _radians, float _dt);
	void			TurnTowardPoint(XMFLOAT3& _point, float _dt);
	void			TurnTowardVector(XMFLOAT3& _vector, float _dt);
	virtual void	Accelerate(const float & dt);
	void			Decelerate(const float & dt);

	//****************************************//
	/*Attack*/
	//****************************************//
	void			SideCannonFire(void);
	void			LongNineFire(void);

	//****************************************//
	/*Boarding*/
	//****************************************//
	virtual void	UpdateBoarding(float dt);
	float			GetSqrDistance(Ship* target);

	//****************************************//
	/*Other*/
	//****************************************//
	void			TakeDamage(const int dmg);
	virtual void	KillOwner(void);

    void            UpdateParticles();


	//**************Accessors****************//

	const eAlignment			GetAlignment();
	const eShipType				GetShipType();
	virtual const EntityType	GetType() final
	{
		return EntityType::SHIP;
	}
	int							GetShipPartLevel(int index);
	inline IController*			GetController() { return m_pController; }
	SideCannonHandler*			GetSideCannonHandler(void) const { return m_cWeapon; }

	float&			GetMaxSpeed() { return m_fMaxSpeed; }
	const float&	GetMaxSpeed() const { return m_fMaxSpeed; }

	const int		GetMastLevel() { return m_nMastLevel; }
	const int		GetHullLevel() { return m_nHullLevel; }
	const int		GetWeaponsLevel();	//{ return m_cWeapon->GetLevel(); }
	const float		GetCurrentMaxSpeed() { return m_fCurrentMaxSpeed; }
	const int		GetMaxHealth() { return m_nMaxHealth; }
	const int		GetHealth() { return m_nHealth; }

	//****************************************//
	/*Resource*/
	//****************************************//
	float			GetCurrReputation	(void) const { return m_fCurrReputation; }
	float			GetMaxReputation	(void) const { return m_fMaxReputation; }

	//****************************************//
	/*Movement*/
	//****************************************//
	const float&	GetSpeedLimit();
	const float&	GetTurnSpeedLimit();
	const float&	GetSlowDownRate();
	bool			GetSailsDown() { return m_sailsDown; }
	bool			GetComingBackFromRuinedSails() { return m_comingBackFromRuinedSails; }
	const float&	GetSpeed() { return m_fSpeed; }

	//****************************************//
	/*Resource*/
	//****************************************//
	int				GetCurrRum(void) const { return m_nCurrRum; }
	int				GetMaxRum(void) const { return m_nMaxRum; }
	int				GetCurrWaterFood(void) const { return m_nCurrWaterFood; }
	int				GetMaxWaterFood(void) const { return m_nMaxWaterFood; }
	int				GetCurrGold(void) const { return m_nCurrGold; }
	int				GetMaxGold(void) const { return m_nMaxGold; }
	int				GetCurrCrew(void) const { return m_nCurrCrew; }
	int				GetMaxCrew(void) const { return m_nMaxCrew; }

	//****************************************//
	/*Port & Board*/
	//****************************************//
	eBoardingState	GetBoardingStage(void) const { return m_BoardingState; }

	//****************************************//
	/*Death*/
	//****************************************//
	eDeathStages	GetDeathStage(void) const { return m_DeathStage; }


	//**************Mutators****************//

	void			SetAlignment(int);
	void			SetShipType(int);
	void			SetController(IController*);
	void			SetWeapon(SideCannonHandler*);
	void			SetDeathStage(eDeathStages ds) { m_DeathStage = ds; }
	void			SetBoardingStage(eBoardingState bs) { m_BoardingState = bs; }
	void			SetDefense(int def);
	void			SetAcceleration(float Acceleration) { m_fAcceleration = Acceleration; }
	void			SetSailsDown(bool sails) { m_sailsDown = sails; }
	void			SetCurrHealth(int hp) { m_nHealth = hp; }

	//****************************************//
	/*Resource*/
	//****************************************//
	virtual void	IncreaseGold	(int amount);
	virtual void	DecreaseGold	(int amount);
	virtual void	IncreaseRum		(int amount);
	virtual void	IncreaseFood	(int amount);
	virtual void	IncreaseCrew	(int amount);
	virtual void	DecreaseCrew	(void);

	virtual bool	RefillRum		(void);			//{ m_nCurrRum = m_nMaxRum;}
	virtual bool	RefillRum		(int amount);
	virtual bool	RefillFood		(void);			//{ m_nCurrWaterFood = m_nMaxWaterFood;}
	virtual bool	RefillFood		(int amount);
	virtual bool	RefillCrew		(void);			//{ m_nCurrCrew = m_nMaxCrew;}
	virtual bool	RefillCrew		(int amount);

	bool			Repair(void);
	bool			RepairSails(void);

	//****************************************//
	/* load from xml  */
	//****************************************//
	void SetCurrGold(int val) { m_nCurrGold = val; }
	void SetCurrRum(int val) { m_nCurrRum = val; }
	void SetCurrWaterFood(int val) { m_nCurrWaterFood = val; }
	void SetCurrCrew(int val) { m_nCurrCrew = val; }

	//****************************************//
	/*Movement*/
	//****************************************//
	void SetMaxSpeed(float newSpeed);
	//minSpeed   = if newMin is less than zero assign minSpeed 
		//if it is larger than  MAXSpeed assign MinSpeed 
			//else assign newMIn 
	void SetMinSpeed(float newMin) { m_fMinSpeed = (newMin < 0) ? m_fMinSpeed : (newMin > m_fMaxSpeed) ? m_fMinSpeed : newMin; }
	void SetSlowDownMultipler(float newSlowDown);
	void SetMaxTurnRate(float turnRate);
	void SetMaxHealth(int health);
	void SetSpeed(float speed);
	void AdjustSpeed(float speed);
	void SetControllerName(const char* controllerName) { m_stControllerName = controllerName; }




protected:
	//***************Data Members****************//
	eAlignment			m_Alignment;
	eShipType			m_ShipType;

	// Holds the pointer to how the ship will be moving
	IController*		m_pController;
	std::string			m_stControllerName;
	// Holds the the three parts of the ship
	SideCannonHandler*	m_cWeapon;


	// Resource data

	//****************************************//
	/* Reputation  */
	//****************************************//
	float				m_fCurrReputation;
	float				m_fMaxReputation;

	//****************************************//
	/* Mast*/
	//****************************************//
	int					m_nMastLevel = 0;
	float				m_fCurrentMaxSpeed;
	float				m_fMaxSpeed;
	float				m_fSpeed;
	float				m_fSlowdownMultiplier;
	float				m_fMaxTimer = 10;
	float				m_fReducedSpeedTimer = 0;
	float				m_fAcceleration = 0.1f;
	float				m_fMinSpeed = 0;
	virtual void		ReduceSpeed();
	virtual bool		MastLevelUp();
	void				Translate(float dt);

	//****************************************//
	/* Hull */
	//****************************************//
	int					m_nHullLevel = 0;
	int					m_nHealth = 100;
	int					m_nMaxHealth = 100;
	float				m_fMaxRoT = 0;
	int					m_nCurrGold;
	int					m_nMaxGold;
	int					m_nDefense = 0;
	int					m_nCurrRum;
	int					m_nCurrCrew;
	int					m_nCurrWaterFood;
	int					m_nMaxWaterFood;
	int					m_nMaxCrew;
	int					m_nMaxRum;
	bool				HullLevelUp();

	//****************************************//
	/* Pop text?  */
	//****************************************//
public:
	float				m_PopTextTimer;
	std::wstring		m_PopText;

protected:
	std::unique_ptr<Renderer::C3DHUD>    m_PopTextHUD;

	//****************************************//
	/* Death (not)animation data  */
	//****************************************//
	eDeathStages		m_DeathStage;
	float				m_fDeathTimer;
	float roll = 0;

	//Collision
	bool				m_bColllision = false;

	//??
	float				m_fSlowModeTimer;

	//****************************************//
	/* Boarding Data  */
	//****************************************//
	eBoardingState		m_BoardingState;
	bool				m_bCurrBoarding;

	//****************************************//
	/* Sailsdown Data  */
	//****************************************//
	bool				m_comingBackFromRuinedSails;
	bool				m_sailsDown;

	//****************************************//
	/* Sound Data  */
	//****************************************//
	bool				m_bDead = false;
	void				DeathSound();
};





/********************This is the old base*************************/
//class Ship : public Entity3D	//IEntity
//{
//	friend class AiController;
//	friend class PlayerController;
//	friend class Mast;
//public:
//	enum eAlignment { UNKNOWN = -1, PLAYER = 0, ENEMY, OTHER };
//	enum eBoardingState { NONE = 0, BOARDING_MOVE, BOARDING_SHIP, BOARDING_INTERUPT, BOARDING_FINISHED, BOARDING_ENEMY };
//	enum eDeathStages { ALIVE = 0, DEATH_ROTATE, DEATH_SINK, DEATH_DONE };
//
//	Ship(const XMFLOAT4X4 & _newTransfrom, std::string meshName, std::string textureName);
//
//	//****************Trilogy*****************//
//	//You must set the controller and Collider
//	Ship& operator=(const Ship& other);
//	//You must set the controller and Collider
//	Ship(const Ship& _cpy);
//	virtual ~Ship();
//
//
//	//**************Functions****************//
//
//	virtual void	Update(float dt);
//	void			DeathUpdate(float dt);
//	bool			LevelUp(IShipPart::PartType partType);
//	void			OnCollision(Collider& other);
//	bool			Repair(IShipPart::PartType partType);
//	void			LoadPlayer(void);
//	void			StopColliders(void);
//	void			StartColliders(void);
//	//****************************************//
//	/*Bonus*/
//	//****************************************//
//	float			SpeedBonus(float dt, float accel, float distToMax);
//	float			CooldownBonus(float dt);
//	float			BoardingBonus(float dt);
//
//	//****************************************//
//	/*Resource*/
//	//****************************************//
//	void			UpdateResources(float dt);
//	void			RefillResources(void);
//
//	//****************************************//
//	/*Movement*/
//	//****************************************//
//	void FlipSailsDown();
//	void TurnDegrees(float _degrees, float _dt);
//	void TurnRadians(float _radians, float _dt);
//	void TurnTowardPoint(XMFLOAT3& _point, float _dt);
//	void TurnTowardVector(XMFLOAT3& _vector, float _dt);
//	void FlipComingBackFromRuinedSails() { m_comingBackFromRuinedSails = !m_comingBackFromRuinedSails; }
//	void Accelerate(const float & dt);
//	void Decelerate(const float & dt);
//	//****************************************//
//	/*Cheats*/
//	//****************************************//
//	void			GoToMaxLevel(void);
//	void			ToggleGodMode(void);
//
//
//	//****************************************//
//	/*Attack*/
//	//****************************************//
//	void			SideCannonFire(void);
//	void			LongNineFire(void);
//
//	//****************************************//
//	/*Boarding*/
//	//****************************************//
//	void			EnemyBoarding(float dt);
//	void			UpdateBoarding(float dt);
//	void			PlayerBoarding(float dt);
//	bool			FindPossibleBoardingTargets(void);
//	bool			SetCurrentBoardingTarget(void);
//	bool			CheckBoardingDistance(void);
//	float			GetDistance(Ship* target);
//
//	//****************************************//
//	/*Other*/
//	//****************************************//
//	void			TakeDamage(const int dmg);
//	float&			GetMaxSpeed() { return m_fMaxSpeed; }
//	const float&	GetMaxSpeed() const { return m_fMaxSpeed; }
//
//
//	const int		GetMastLevel() { return m_nMastLevel; }
//	const int		GetHullLevel() { return m_nHullLevel; }
//	const int		GetWeaponsLevel();	//{ return m_cWeapon->GetLevel(); }
//	const float		GetCurrentMaxSpeed() { return m_fCurrentMaxSpeed; }
//	const int		GetMaxHealth() { return m_nMaxHealth; }
//	const int		GetHealth() { return m_nHealth; }
//	void			AddReputation(float rep);
//	void			UnlockInput(void);
//	void			LockInput(void);
//	bool			LoseCheck(void);
//	void			KillOwner(void);
//	bool			WinCheck(void);
//	bool			WinCountdown(void);
//
//
//	//**************Accessors****************//
//	const eAlignment GetAlignment();
//	virtual const EntityType GetType() final
//	{
//		return EntityType::SHIP;
//	}
//	int GetShipPartLevel(int index);
//	inline IController* GetController() { return m_pController; }
//	SideCannonHandler*	GetSideCannonHandler(void) const { return m_cWeapon; }
//
//	//****************************************//
//	/*Movement*/
//	//****************************************//
//	const float& GetSpeedLimit();
//	const float& GetTurnSpeedLimit();
//	const float& GetSlowDownRate();
//	bool GetSailsDown() { return m_sailsDown; }
//	bool GetComingBackFromRuinedSails() { return m_comingBackFromRuinedSails; }
//	const float& GetSpeed() { return m_fSpeed; }
//	//****************************************//
//	/*Resource*/
//	//****************************************//
//	float				GetCurrReputation(void) const { return m_fCurrReputation; }
//	float				GetMaxReputation(void) const { return m_fMaxReputation; }
//	int					GetCurrRum(void) const { return m_nCurrRum; }
//	int					GetMaxRum(void) const { return m_nMaxRum; }
//	int					GetCurrWaterFood(void) const { return m_nCurrWaterFood; }
//	int					GetMaxWaterFood(void) const { return m_nMaxWaterFood; }
//	int					GetCurrGold(void) const { return m_nCurrGold; }
//	int					GetMaxGold(void) const { return m_nMaxGold; }
//	int					GetCurrCrew(void) const { return m_nCurrCrew; }
//	int					GetMaxCrew(void) const { return m_nMaxCrew; }
//
//	//****************************************//
//	/*Port & Board*/
//	//****************************************//
//	bool				GetPortCollision(void) const { return (m_CurrPort != L"") ? true : false; }
//	std::wstring		GetPortName(void) const { return m_CurrPort; }
//	eBoardingState		GetBoardingStage(void) const { return m_BoardingState; }
//	Ship*				GetKiller(void) { return m_pKiller; }
//
//	//****************************************//
//	/*Cheat*/
//	//****************************************//
//	eDeathStages		GetDeathStage(void) const { return m_DeathStage; }
//	bool				GetGodMode(void) const { return m_bGodMode; }
//
//
//	//**************Mutators****************//
//	void	SetAlignment(int);
//	void	SetController(IController*);
//	void	SetWeapon(SideCannonHandler*);
//	void	SetDeathStage(eDeathStages ds) { m_DeathStage = ds; }
//	void	SetBoardingStage(eBoardingState bs) { m_BoardingState = bs; }
//	void	SetDefense(int def);
//	void	SetAcceleration(float Acceleration) { m_fAcceleration = Acceleration; }
//	//****************************************//
//	/*Resource*/
//	//****************************************//
//	void	IncreaseGold(int amount);
//
//	void	IncreaseRum(int amount);
//	void	IncreaseFood(int amount);
//	void	IncreaseCrew(int amount);
//
//	void	DecreaseGold(int amount);
//	void	DecreaseCrew(void);
//
//	bool	Purchase(int cost);
//	bool	RefillRum(void);	//{ m_nCurrRum = m_nMaxRum;}
//	bool	RefillRum(int amount);
//	bool	RefillFood(void);	//{ m_nCurrWaterFood = m_nMaxWaterFood;}
//	bool	RefillFood(int amount);
//	bool	RefillCrew(void);	//{ m_nCurrCrew = m_nMaxCrew;}
//	bool	RefillCrew(int amount);
//	bool	Repair(void);
//	bool	RepairSails(void);
//
//	// load from xml
//	void SetCurrGold(int val) { m_nCurrGold = val; }
//	void SetCurrRum(int val) { m_nCurrRum = val; }
//	void SetCurrWaterFood(int val) { m_nCurrWaterFood = val; }
//	void SetCurrCrew(int val) { m_nCurrCrew = val; }
//
//	// buffer testing purpose
//	float BufferRep() const { return m_fBufferRep; }
//	int	BufferRum() const { return m_nBufferRum; }
//	int	BufferWaterFood() const { return m_nBufferWaterFood; }
//	int	BufferGold() const { return m_nBufferGold; }
//	bool DecreasingRes() const { return m_bDecreasingRes; }
//
//	//****************************************//
//	/*Movement*/
//	//****************************************//
//	void SetMaxSpeed(float newSpeed);
//	//minSpeed   = if newMin is less than zero assign minSpeed 
//		//if it is larger than  MAXSpeed assign MinSpeed 
//			//else assign newMIn 
//	void SetMinSpeed(float newMin) { m_fMinSpeed = (newMin < 0) ? m_fMinSpeed : (newMin > m_fMaxSpeed) ? m_fMinSpeed : newMin; }
//	void SetSlowDownMultipler(float newSlowDown);
//	void SetMaxTurnRate(float turnRate);
//	void SetMaxHealth(int health);
//	void SetSpeed(float speed);
//	void AdjustSpeed(float speed);
//	void SetControllerName(const char* controllerName) { m_stControllerName = controllerName; }
//private:
//	//***************Data Members****************//
//	eAlignment			m_Alignment;
//
//	// Holds the pointer to how the ship will be moving
//	IController*		m_pController;
//	std::string			m_stControllerName;
//	// Holds the the three parts of the ship
//	SideCannonHandler*	m_cWeapon;
//
//
//	// Resource data
//
//	//****************************************//
//	/* Mast*/
//	//****************************************//
//	int					m_nMastLevel = 0;
//	float				m_fCurrentMaxSpeed;
//	float				m_fMaxSpeed;
//	float				m_fSpeed;
//	float				m_fSlowdownMultiplier;
//	float				m_fMaxTimer = 10;
//	float				m_fReducedSpeedTimer = 0;
//	float				m_fAcceleration = 0.1f;
//	float				m_fMinSpeed = 0;
//	void				ReduceSpeed();
//	bool				MastLevelUp();
//	void				Translate(float dt);
//
//	//****************************************//
//	/* Hull */
//	//****************************************//
//	int					m_nHullLevel = 0;
//	int					m_nHealth = 100;
//	int					m_nMaxHealth = 100;
//	float				m_fMaxRoT = 0;
//	int					m_nCurrGold;
//	int					m_nMaxGold;
//	int					m_nDefense = 0;
//	int					m_nCurrRum;
//	int					m_nCurrCrew;
//	int					m_nCurrWaterFood;
//	int					m_nMaxWaterFood;
//	int					m_nMaxCrew;
//	int					m_nMaxRum;
//	bool				HullLevelUp();
//
//	float				m_fCurrReputation;
//	float				m_fMaxReputation;
//	//****************************************//
//	/* Other Data  */
//	//****************************************//
//	float				m_fResourceTimer;
//	bool                m_bDecreasingRes;
//
//	float				m_fBufferRep;
//	int					m_nBufferRum;
//	float				m_fRumResourceTimer;
//	int					m_nBufferWaterFood;
//	float				m_fWaterFoodResourceTimer;
//	int					m_nBufferGold;
//	float				m_fGoldResourceTimer;
//	float				m_fCrewResourceTimer;
//public:
//
//	float				m_PopTextTimer;
//	std::wstring		m_PopText;
//
//private:
//
//	std::unique_ptr<Renderer::C3DHUD>    m_PopTextHUD;
//
//	// Death (not)animation data
//	eDeathStages		m_DeathStage;
//	float				m_fDeathTimer;
//	Ship*				m_pKiller;
//
//	//Collision
//	bool				m_bColllision = false;
//
//	// Cheat data
//	bool				m_bGodMode;
//
//	//float				VFXCountDown;
//
//	float				m_fSlowModeTimer;
//	//****************************************//
//	/* Porting Data  */
//	//****************************************//
//	bool				m_PortCollision;
//	std::wstring		m_CurrPort;
//
//	//****************************************//
//	/* Boarding Data  */
//	//****************************************//
//	eBoardingState		m_BoardingState;
//	float				m_fBoardingTimer;
//	Ship*				m_pBoardingTarget;
//	int					m_pBoardingIter;
//	std::vector<Ship*>	m_pBoardingAllTargets;
//	bool				m_bCurrBoarding;
//	XMFLOAT4			m_boardingTargetPos;
//
//	//Boarding icons
//	DirectX::SimpleMath::Vector4		 m_swordsCrossingPos;
//	std::unique_ptr<Renderer::C3DHUD>    m_swordsCrossed;
//	std::unique_ptr<Renderer::C3DHUD>    m_swordsUncrossed;
//	float								 m_swordsCrossingTimer;
//	bool								 m_swordsCrossedActive;
//
//	//Health Bar
//	DirectX::SimpleMath::Vector4		 m_healthBarPos;
//	std::unique_ptr<Renderer::C3DHUD>    m_healthBar;
//
//	//Sails down icon
//	bool								 m_comingBackFromRuinedSails;
//	bool								 m_sailsDown;
//	DirectX::SimpleMath::Vector4		 m_sailsDownIconPos;
//	DirectX::SimpleMath::Vector4		 m_crewIconPos;
//	DirectX::SimpleMath::Vector4		 m_cannonIconPos;
//	std::unique_ptr<Renderer::C3DHUD>    m_sailsDownIcon;
//	std::unique_ptr<Renderer::C3DHUD>    m_crewIcon;
//	std::unique_ptr<Renderer::C3DHUD>    m_sailIcon[3];
//	std::unique_ptr<Renderer::C3DHUD>    m_cannonIcon[3];
//	std::unique_ptr<Renderer::C3DHUD>    m_NumCrew3D;
//	std::wstring						 m_NumCrew;
//
//	DirectX::SimpleMath::Vector4		 m_crewStickerBluePos;
//	DirectX::SimpleMath::Vector4		 m_crewStickerRedPos;
//	std::unique_ptr<Renderer::C3DHUD>    m_crewStickerBlue;
//	std::unique_ptr<Renderer::C3DHUD>    m_crewStickerRed;
//
//	// Win non-pop
//	float				m_fWinTimer;
//	float				m_fWindt;
//
//	//****************************************//
//	/* Sound Data  */
//	//****************************************//
//	bool				m_bDead = false;
//	void				DeathSound();
//};
