#pragma once
#include "IShipPart.h"
#include "Ship.h"
#include "IAmmo.h"

#define TOTAL_NUM_CANNONS	((int)15)
#define MAX_FIRING_BURST	((int)5)

class Ship;
class Collider;

template<typename T>
class TemplatedCollisionCallback;

class Cannon : public IShipPart
{
	struct FiringStruct
	{
		std::string			m_sFireOrder;	// which cannon to fire: [0->4]
		float				m_fCurrDelay;	// current time until next cannon can fire
		bool				m_bFiring;		// are the cannons suppsed to be firing?
		IAmmo::eAmmoType	m_FiringAmmo;	// what ammo cannons are firing
		int					m_nNumToFire;	// total # of shots to fire
		int					m_nFireCounter;	// # of cannons fired so far
		XMFLOAT3			m_AmmoOffset;	// position offset for next shot
		int					m_nDamage;		// damage caused by shot

		FiringStruct()
			: m_fCurrDelay(0.0f), m_bFiring(false), m_FiringAmmo(IAmmo::eAmmoType::UNKNOWN),
			  m_nNumToFire(0), m_nFireCounter(0), m_AmmoOffset(0.0f, 0.0f, 0.0f), m_nDamage(0)
		{}
	};

public:
	enum WeaponType { UNKNOWN = -1, CANNON = 0, LONG_NINE, HARPOON_GUN };

	Cannon(Ship* owner, WeaponType type, const XMFLOAT4X4& _newTransfrom);


	//****************Trilogy*****************//
	Cannon& operator=(const Cannon& other);
	Cannon(const Cannon& _cpy);

	virtual ~Cannon();

	//**************Functions****************//
	//void OnCollision(Collider& other);

	void Update(float dt);
	bool LevelUp();
	bool Fire(IAmmo::eAmmoType ammo, int num_to_fire, int side);
	void WreckPart();
	int CalculateDamage();
	float CalculateDelay();

	XMFLOAT3 GetAmmoOffset();
	float GetFiringSpread();

	void FireLeft(float dt);
	void FireRight(float dt);



	/*****************Accessor******************/
	inline virtual const PartType GetPartType() final
	{
		return PartType::WEAPON;
	}
	const int GetMinDamage();
	const int GetMaxDamage();
	const float GetReloadTime() { return m_fReloadTimer; }
	const float GetRightReloadTime() { return m_fRightReloadTimer; }
	const float GetLeftReloadTime() { return m_fLeftReloadTimer; }
	const float GetMaxReload() { return m_fReloadMax; }
	bool isActive() const { return m_bActive; }
	FiringStruct& GetFiringStructLeft() { return m_FireLeft; }
	FiringStruct& GetFiringStructRight() { return m_FireRight; }


	/***************Mutators***************/
	void SetDamage(int min, int max);
	void SetMaxReload(float _in) {  m_fReloadMax = _in; }
	void OnCollision(Collider&);


private:
	WeaponType	m_eWeaponType;
	float		m_fReloadTimer;
	float		m_fLeftReloadTimer;
	float		m_fRightReloadTimer;
	float		m_fReloadMax;
	float		m_fLifespan;
	float		m_fFiringSpread;

	int			m_nMinDamage;
	int			m_nMaxDamage;
	bool		m_bActive;

	int			m_nDelayMin;
	int			m_nDelayMax;

	FiringStruct		m_FireLeft;
	FiringStruct		m_FireRight;
};

