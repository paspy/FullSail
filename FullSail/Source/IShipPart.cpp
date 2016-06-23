/**************************************************************************************/
/*
This is an abstract class for all ShipParts
to inherit off of

Author : David Ashbourne(Full Sail)
File : IShipPart
Contributor(s) : Jason Bloomfield (Add name here if you like)
*/
/**************************************************************************************/
#include "pch.h"
#include "IShipPart.h"
#include "IEntity.h"
#include "Transform.h"
#include "Ship.h"
#include "Colliders.h"
#include "Entity3D.h"
#include "IAmmo.h"


IShipPart::IShipPart(Ship* owner) : IEntity(owner->GetWorldMatrix())
{
	m_owner = owner;
	m_nMaxHealth = INIT_HP_MAX;
	m_nHealth = m_nMaxHealth;
	m_nDefense = 0;
	m_nLevel = 0;
	m_bWrecked = false;

	// collider
}

IShipPart::IShipPart(Ship* owner, const XMFLOAT4X4& _newTransfrom) : IEntity(_newTransfrom)
{
	m_owner = owner;
	m_nMaxHealth = INIT_HP_MAX;
	m_nHealth = m_nMaxHealth;
	m_nDefense = 0;
	m_nLevel = 0;
	m_bWrecked = false;

	// collider
}

IShipPart& IShipPart::operator=(const IShipPart & other)
{
	// TODO: insert return statement here
	if (this == &other)
	{
		return (*this);
	}

	IEntity::operator=(other);

	m_owner			= other.m_owner;
	m_nHealth		= other.m_nHealth;
	m_nMaxHealth	= other.m_nMaxHealth;
	m_nDefense		= other.m_nDefense;
	m_nLevel		= other.m_nLevel;
	m_fWreckTimer	= other.m_fWreckTimer;
	m_bWrecked		= other.m_bWrecked;

	return (*this);
}

IShipPart::IShipPart(const IShipPart & _cpy)
	: IEntity(_cpy)
{
	m_owner			= _cpy.m_owner;
	m_nHealth		= _cpy.m_nHealth;
	m_nMaxHealth	= _cpy.m_nMaxHealth;
	m_nDefense		= _cpy.m_nDefense;
	m_nLevel		= _cpy.m_nLevel;
	m_fWreckTimer	= _cpy.m_fWreckTimer;
	m_bWrecked		= _cpy.m_bWrecked;
}

/*
IShipPart::IShipPart(unsigned int level, XMFLOAT4X4& _newTransfrom, const char* MeshName, unsigned int defense, unsigned int maxHealth, const char* TextureName)
	:Entity3D(_newTransfrom, MeshName, TextureName)
{
	m_nHealth = m_nMaxHealth = maxHealth;
	m_nDefense = defense;

	//checking to see if the level being passed in is greater than the maxLevel
	//true = 1
	//then checks to see if the level is less then one
	//true = 1 || false = passed in level
	m_nLevel = (level > m_nMaxLevel) ? 1 : (level < 1) ? 1 : level;
}
*/

IShipPart::~IShipPart()
{
	m_owner = nullptr;

	//delete m_pCollider;
	//m_pCollider = nullptr;
}


void IShipPart::OnCollision(Collider & _other)
{
	//Should decrease health
	// Call wreak part if nessecary
	// also call the hit effect (if that gets in heheheheheheh!!)

	//overwrite the wreakpart and hit_effect in other class so that if necessary for change

// This is happening in the ship class as of right now

//Retrieving the Object from the void pointer
//Entity3D* CollidedObject = static_cast<Entity3D*>(_other.getID());

////Getting the type
//int CollidedType = CollidedObject->type;

//if (CollidedType == Entity3D::EntityType::AMMO)
//{
//	IAmmo* projectile = dynamic_cast<IAmmo*>(CollidedObject);

//	//Change this please Jason/Ash
//	if (projectile)
//	{
//		TakeDamage(projectile->GetDamage());
//	}
//}
}


void IShipPart::Update(float _dt)
{
	// update timer
	if (m_bWrecked == true)
	{

	}

}

bool IShipPart::LevelUp()
{
	// max check
	if (m_nLevel >= LEVEL_MAX)
		return false;


	// level++
	m_nLevel++;


	// cap level
	if (m_nLevel >= LEVEL_MAX)
		m_nLevel = LEVEL_MAX;


	// HP++ (HP += MAX * (Lv/10))
	m_nMaxHealth += int(m_nMaxHealth * float(m_nLevel * 0.10f));


	//Def++ (Def += Def * (Lv/5))
	m_nDefense += int(m_nDefense * float(m_nLevel * 0.05f));


	// return
	return true;
}

bool IShipPart::Repair()
{
	if (m_nHealth == m_nMaxHealth)
		return false;

	// HP = MAX
	m_nHealth = m_nMaxHealth;


	return true;
}

void IShipPart::TakeDamage(const int dmg)
{
	// hp check
	if (m_nHealth <= 0)
		return;


	// damage/defence calc
	int IncomingDamage = (dmg - m_nDefense);

	// no damage?
	if (IncomingDamage < 0)
		return;

	// hp--
	m_nHealth -= IncomingDamage;

	// dead?
	if (m_nHealth <= 0)
	{
		m_nHealth = 0;
		m_bWrecked = true;

		// owner is sunk
		m_owner->KillOwner();
	}
}
