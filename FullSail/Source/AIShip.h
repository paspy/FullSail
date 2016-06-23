#pragma once
#include "Ship.h"

namespace Renderer
{
	class C3DHUD;
}


/********************This is the new base to start off*************************/
class AIShip : public Ship
{
public:
	AIShip(const XMFLOAT4X4 & _newTransfrom, std::string meshName, std::string textureName);

	//****************Trilogy*****************//
	AIShip& operator=(const AIShip& other);
	AIShip(const AIShip& _cpy);
	virtual ~AIShip();


	//**************Functions****************//
	virtual void	Update(float dt);
	virtual void	DeathUpdate(float dt);
	virtual void	OnCollision(Collider& other);

	//****************************************//
	/*Movement*/
	//****************************************//
	virtual  void	FlipSailsDown();

	//****************************************//
	/*Boarding*/
	//****************************************//
	virtual void	UpdateBoarding(float dt);
	void			EnemyBoarding(float dt);

	//****************************************//
	/*Other*/
	//****************************************//
	virtual void	KillOwner		(void);


	//**************Accessors****************//

	Renderer::C3DHUD*	GetSailsDownIcon() { return m_sailsDownIcon.get(); }

	//****************************************//
	/*Boarding*/
	//****************************************//
	Ship*			GetKiller(void) { return m_pKiller; }


	//**************Mutators****************//

	void			SetKiller(Ship* k) { m_pKiller = k; }




	std::unique_ptr<Renderer::C3DHUD>* tempFunc() {
		return &m_healthBar;
	}

	void BeBoarded(SimpleMath::Vector4& _pos);

	void FinishBoarding();
protected:
	//***************Data Members****************//

	//****************************************//
	/* Mast*/
	//****************************************//
	virtual void		ReduceSpeed();

	//****************************************//
	/* Death (not)animation data  */
	//****************************************//
	Ship*				m_pKiller;

	//****************************************//
	/* 3D HUD icons  */
	//****************************************//
	DirectX::SimpleMath::Vector4		 m_healthBarPos;
	std::unique_ptr<Renderer::C3DHUD>    m_healthBar;

	DirectX::SimpleMath::Vector4		 m_sailsDownIconPos;
	DirectX::SimpleMath::Vector4		 m_crewIconPos;
	DirectX::SimpleMath::Vector4		 m_cannonIconPos;
	DirectX::SimpleMath::Vector4		 m_boardableIconPos;
	DirectX::SimpleMath::Vector4		 m_aggroIconPos;
	std::unique_ptr<Renderer::C3DHUD>    m_sailsDownIcon;
	std::unique_ptr<Renderer::C3DHUD>    m_crewIcon;
	std::unique_ptr<Renderer::C3DHUD>    m_sailIcon[3];
	std::unique_ptr<Renderer::C3DHUD>    m_cannonIcon[3];
	std::unique_ptr<Renderer::C3DHUD>    m_NumCrew3D;
	std::wstring						 m_NumCrew;
	std::unique_ptr<Renderer::C3DHUD>    m_boardableIcon;
	std::unique_ptr<Renderer::C3DHUD>    m_aggroIcon;
};
