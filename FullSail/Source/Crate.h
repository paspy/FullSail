#pragma once
#include "Entity3D.h"
#include "Entity2D.h"
#include "GUIManager.h"

class Ship;

class Crate : public Entity3D
{
public:
	Crate(XMFLOAT4X4& _newTransfrom, const char* MeshName, const char* TextureName = nullptr);

	Crate(const Crate& _cpy);
	~Crate();
	Crate& operator=(const Crate& other);


	//**************Functions****************//
	void		Update			(float dt);
	void		OnCollision		(Collider& other);
	void		DeathUpdate		(float dt);

	//**************Accessors****************//
	virtual const EntityType GetType() final { return EntityType::CRATE; }
	int			GetRum			(void) const	{ return m_nRum; }
	int			GetWaterFood	(void) const	{ return m_nWaterFood; }
	int			GetGold			(void) const	{ return m_nGold; }
	XMFLOAT3	GetDirection	(void) const	{ return m_direction; }
	float		GetLifespan		(void) const	{ return m_fLifespan; }
	float		GetSpeed		(void) const	{ return m_fSpeed; }


	//**************Mutators****************//
	void		SetRum			(int r)			{ m_nRum		= r; }
	void		SetWaterFood	(int wf)		{ m_nWaterFood	= wf; }
	void		SetGold			(int g)			{ m_nGold		= g; }
	void		SetDirection	(XMFLOAT3 dir)	{ m_direction	= dir; }
	void		SetLifespan		(float ls)		{ m_fLifespan	= ls; }
	void		SetSpeed		(float s)		{ m_fSpeed		= s; }


private:
	int			m_nRum			= 0;
	int			m_nWaterFood	= 0;
	int			m_nGold			= 0;
	XMFLOAT3	m_direction		= XMFLOAT3(0.0f, 0.0f, 0.0f);
	float		m_fLifespan		= 0.0f;
	float		m_fSpeed		= 0.0f;
    float		m_fDeathTimer   = 0.0f;
	bool		m_bHitVFX		= false;

	Ship*		m_pPlayerShip;

};

