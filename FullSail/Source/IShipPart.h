/**************************************************************************************/
/*
This is an abstract class for all ShipParts
to inherit off of

Author : David Ashbourne(Full Sail)
File : IShipPart
Contributor(s) : Jason Bloomfield (Add name here if you like)
*/
/**************************************************************************************/
#pragma once
#include "IEntity.h"

#define LEVEL_MAX		((int)5)
#define INIT_HP_MAX		((int)20)		//100
#define TIMER_LIMIT		((float)10.0f)

class Ship;
class Collider;


class IShipPart : public IEntity
{
public:
	enum PartType { UNKNOWN = -1, HULL = 0, MAST, WEAPON };

	IShipPart(Ship* owner);
	IShipPart(Ship* owner, const XMFLOAT4X4& _newTransfrom);

	//****************Trilogy*****************//
	IShipPart& operator=(const IShipPart& other);
	IShipPart(const IShipPart& _cpy);

	virtual ~IShipPart();



	//**************Functions****************//
	virtual void OnCollision(Collider& _other);
	virtual void Update(float _dt) override;
	virtual bool LevelUp();
	virtual bool Repair();
	// this functions is for when the part is at zero health each part does somthing different???
	virtual void WreckPart() {}
	void		TakeDamage(const int dmg);
	bool		IsAlive(void)	{ return (m_nHealth > 0) ? true : false; }


	/*****************Accessors******************/
	virtual const	PartType	GetPartType		(void)	{ return PartType::UNKNOWN; }
	virtual const	int			GetHealth		(void)	{ return m_nHealth; }
	virtual const	int			GetMaxHealth	(void)	{ return m_nMaxHealth; }
	const			Ship*		GetOwner		(void)	{ return m_owner; }
	const			Collider*	GetCollider		(void)	{ return m_pCollider; }
	const			int			GetDefence		(void)	{ return m_nDefense; }
	const			int			GetLevel		(void)	{ return m_nLevel; }
	int							GetLevel		(void)	const { return m_nLevel; }
	const			bool		GetWrecked		(void)	{ return m_bWrecked; }
	const			float		GetWreckTimer	(void)	{ return m_fWreckTimer; }


	/*****************Mutators*******************/
	void	SetOwner		(Ship* _s)		{ m_owner		= _s; }
	void	SetCollider		(Collider* _c)	{ m_pCollider	= _c; }
	void	SetHealth		(int _hp)		{ m_nHealth		= _hp; }
	void	SetMaxHealth	(int _maxhp)	{ m_nMaxHealth = m_nHealth = _maxhp; }
	void	SetDefence		(int _def)		{ m_nDefense	= _def; }
	void	SetLevel		(int _lv)		{ m_nLevel		= _lv; }
	void	SetWrecked		(bool _w)		{ m_bWrecked	= _w; }
	void	SetWreckTimer	(float _t)		{ m_fWreckTimer	= _t; }

protected:
	//***************Data Members****************//
	Ship*			m_owner			= nullptr;
	Collider*		m_pCollider		= nullptr;
	int				m_nHealth		= 0;
	int				m_nMaxHealth	= 0;
	int				m_nDefense		= 0;
	int				m_nLevel		= 0;
	float			m_fWreckTimer	= 0.0f;
	//This only turns on once the health goes to zero or lower
	//Once this bool turns on, the function WreakPart should be called and it will stop the update of child 
	//Then it will go directly here for it to call the childrens Wreak part
	bool			m_bWrecked		= false;

};

