/**************************************************************************************/
/*
This is an abstract class for all Ammo class
to inherit off of

Author : David Ashbourne(Full Sail)
Contributor(s) : (Add name here if you like)
*/
/**************************************************************************************/
#pragma once
#include "Entity3D.h"
//#include "Ship.h"

class Ship;

//namespace FSParticle {
//	class CParticleSystem;
//}


class IAmmo : public Entity3D
{
public:
	enum eAmmoType { UNKNOWN = -1, CANNONBALL = 0, CHAIN_SHOT, HARPOON };

	IAmmo(Ship* owner, eAmmoType _ammotype, const XMFLOAT4X4& _newTransfrom, std::string meshName, std::string textureName);
	//IAmmo(Ship* owner, eAmmoType _ammotype, float life, int damage, XMFLOAT3 dir, const XMFLOAT4X4& _newTransfrom, std::string meshName, std::string textureName);
	//****************Trilogy*****************//
	IAmmo& operator=(const IAmmo& other);
	IAmmo(const IAmmo& _cpy);
	virtual ~IAmmo();


	//**************Functions****************//
	virtual void OnCollision(Collider& _other);
	virtual void Update(float dt);
	//virtual void Render();


	//**************Accessors****************//
	inline const EntityType GetType() override
	{
		return EntityType::AMMO;
	}

	const int GetDamage();
	Ship* GetOwner() const { return m_pOwner; }
	//const Ship::eAlignment GetShipAlignment();
	eAmmoType GetAmmoType() const { return m_nAmmoType; }
	float GetSpeed() { return m_fSpeed; }

	//**************Mutators****************//
	void SetOwner(Ship* _owner);
	void SetVelocity(XMFLOAT3 velo) { m_vVelocity = velo; }
	void SetLifespan(float ls) { m_fLifespan = 0.5f; }
	void SetDamage(int d) { m_nDamage = d; }
	void SetAmmoType(eAmmoType t) { m_nAmmoType = t; }

    XMFLOAT3 Offset() const { return m_offset; }
    void Offset( XMFLOAT3 val ) { m_offset = val; }

private:
	Ship*		m_pOwner		= nullptr;
	eAmmoType	m_nAmmoType		= eAmmoType::UNKNOWN;
	XMFLOAT3	m_vVelocity		= XMFLOAT3(0.0f, 0.0f, 0.0f);
	float		m_fLifespan		= 0.0f;
	int			m_nDamage		= 0;
	float		m_fSpeed		= 0.0f;
    XMFLOAT3    m_offset;
	//float m_fReloadtime;
	//FSParticle::CParticleSystem *m_fire = nullptr;
};
