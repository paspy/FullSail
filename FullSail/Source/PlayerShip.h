#pragma once
#include "Ship.h"

class AIShip;
class Port;

namespace Renderer
{
	class C3DHUD;
}


/********************This is the new base to start off*************************/
class PlayerShip : public Ship
{
public:
	PlayerShip(const XMFLOAT4X4 & _newTransfrom, std::string meshName, std::string textureName);

	//****************Trilogy*****************//
	PlayerShip& operator=(const PlayerShip& other);
	PlayerShip(const PlayerShip& _cpy);
	virtual ~PlayerShip();


	//**************Functions****************//
	virtual void	Update(float dt);
	virtual bool	LevelUp(IShipPart::PartType partType);
	virtual void	OnCollision(Collider& other);
	void			LoadPlayer(void);


	//****************************************//
	/*Movement*/
	//****************************************//
	virtual void	Accelerate(const float& dt);

	//****************************************//
	/*Bonus*/
	//****************************************//
	float			SpeedBonus(float dt, float accel, float distToMax);
	float			CooldownBonus(float dt);
	float			BoardingBonus(float dt);

	//****************************************//
	/*Resource*/
	//****************************************//
	void			UpdateResources(float dt);
	void			RefillResources(void);

	//****************************************//
	/*Cheats*/
	//****************************************//
	void			GoToMaxLevel(void);
	void			ToggleGodMode(void);

	//****************************************//
	/*Boarding*/
	//****************************************//
	virtual void	UpdateBoarding(float dt);
	void			PlayerBoarding(float dt);
	bool			FindPossibleBoardingTargets(void);
	bool			SetCurrentBoardingTarget(void);
	bool			CheckBoardingDistance(void);

	//****************************************//
	/*Other*/
	//****************************************//
	virtual void	KillOwner		(void);
	void			AddReputation	(float rep);
	void			LockInput		(void);
	void			UnlockInput		(void);
	bool			WinCheck		(void);
	bool			WinCountdown	(void);
	bool			LoseCheck		(void);


	//**************Accessors****************//

	//****************************************//
	/*Porting*/
	//****************************************//
	bool			GetPortCollision	(void) const { return (m_CurrPort != L"") ? true : false; }
	std::wstring	GetPortName			(void) const { return m_CurrPort; }
	Port*			GetMostRecentPort	(void) const { return m_RecentPorts[0]; }

	//****************************************//
	/*Cheat*/
	//****************************************//
	bool			GetGodMode			(void) const { return m_bGodMode; }

	//****************************************//
	/* Buffer testing purpose  */
	//****************************************//
	float			BufferRep			(void) const { return m_fBufferRep; }
	int				BufferRum			(void) const { return m_nBufferRum; }
	int				BufferWaterFood		(void) const { return m_nBufferWaterFood; }
	int				BufferGold			(void) const { return m_nBufferGold; }
	bool			DecreasingRes		(void) const { return m_bDecreasingRes; }


	//**************Mutators****************//

	//****************************************//
	/*Resource*/
	//****************************************//
	virtual void	IncreaseGold	(int amount);
	virtual void	DecreaseGold	(int amount);
	virtual void	IncreaseRum		(int amount);
	virtual void	IncreaseFood	(int amount);
	virtual void	IncreaseCrew	(int amount);
	virtual void	DecreaseCrew	(void);
	bool			Purchase		(int cost);

	virtual bool	RefillRum		(void);
	virtual bool	RefillRum		(int amount);
	virtual bool	RefillFood		(void);
	virtual bool	RefillFood		(int amount);




private:
	//***************Data Members****************//

	//****************************************//
	/* Mast*/
	//****************************************//
	virtual void		ReduceSpeed();
	virtual bool		MastLevelUp();

	//****************************************//
	/* Other Data  */
	//****************************************//
	float				m_fResourceTimer;
	bool                m_bDecreasingRes;

	float				m_fBufferRep;
	int					m_nBufferRum;
	float				m_fRumResourceTimer;
	int					m_nBufferWaterFood;
	float				m_fWaterFoodResourceTimer;
	int					m_nBufferGold;
	float				m_fGoldResourceTimer;
	float				m_fCrewResourceTimer;
	unsigned int		m_nBoardingIndex;
	//****************************************//
	/* Porting Data  */
	//****************************************//
	bool				m_PortCollision;
	std::wstring		m_CurrPort;
	Port*				m_RecentPorts[3];	// [0] == most recent

	//****************************************//
	/* Boarding Data  */
	//****************************************//
	float					m_fBoardingTimer;
	AIShip*					m_pBoardingTarget;
	int						m_pBoardingIter;
	std::vector<AIShip*>	m_pBoardingAllTargets;
	XMFLOAT4				m_boardingTargetPos;

	// Boarding icons
	DirectX::SimpleMath::Vector4		m_swordsCrossingPos;
	std::unique_ptr<Renderer::C3DHUD>	m_swordsCrossed;
	std::unique_ptr<Renderer::C3DHUD>	m_swordsUncrossed;
	float								m_swordsCrossingTimer;
	bool								m_swordsCrossedActive;

	DirectX::SimpleMath::Vector4		m_boardBarPos;
	DirectX::SimpleMath::Vector4		m_pirateAboardPos;
	DirectX::SimpleMath::Vector4		m_navyAboardPos;
	std::unique_ptr<Renderer::C3DHUD>	m_boardBar;
	std::unique_ptr<Renderer::C3DHUD>	m_boardBackBar;
	std::unique_ptr<Renderer::C3DHUD>	m_pirateAboard;
	std::unique_ptr<Renderer::C3DHUD>	m_navyAboard;
	std::unique_ptr<Renderer::C3DHUD>    m_crewIcon;
	std::unique_ptr<Renderer::C3DHUD>    m_NumCrew3D;
	std::wstring						 m_NumCrew;

	// 3D HUD for crew decreasing
	DirectX::SimpleMath::Vector4		m_crewStickerBluePos;
	DirectX::SimpleMath::Vector4		m_crewStickerRedPos;
	std::unique_ptr<Renderer::C3DHUD>	m_crewStickerBlue;
	std::unique_ptr<Renderer::C3DHUD>	m_crewStickerRed;

	//****************************************//
	/* Cheat data  */
	//****************************************//
	bool				m_bGodMode;

	//****************************************//
	/* Win non-pop  */
	//****************************************//
	float				m_fWinTimer;
	float				m_fWindt;
	//****************************************//
	/* Win non-pop  */
	//****************************************//
	void SetupParticles();
};
