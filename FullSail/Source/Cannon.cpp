#include "pch.h"
#include "Cannon.h"
#include "Ship.h"
#include "PlayerShip.h"
#include "Colliders.h"
#include "Game.h"
#include "CollisionManager.h"
#include "GameStatistics.h"


Cannon::Cannon(Ship* owner, WeaponType type, const XMFLOAT4X4& _newTransfrom) : IShipPart(owner, _newTransfrom)
{
	SoundManager*	sound = ((CGame*)CGame::GetApplication())->GetSoundManager();
	sound->RegisterEntity(this, "cannon");

	m_bActive = true;
	m_eWeaponType = type;
	m_fReloadTimer = 0.0f;
	m_fLeftReloadTimer = 0.0f;
	m_fRightReloadTimer = 0.0f;
	m_fLifespan = 0.0f;
	m_fFiringSpread = GetFiringSpread();	// 0.6f;	//1.0f;

	m_nDelayMin = 0;
	m_nDelayMax = 5;
	m_FireLeft = FiringStruct();
	m_FireRight = FiringStruct();
	m_FireLeft.m_sFireOrder = "0";
	m_FireRight.m_sFireOrder = "0";


	switch (m_eWeaponType)
	{
	case WeaponType::CANNON:
		m_fReloadMax = 2.0f;	//5.0f;
		m_nMinDamage = 1;
		m_nMaxDamage = 1;
		m_fLifespan = 0.75f;
		break;

	case WeaponType::LONG_NINE:
		m_fReloadMax = 1.0f;	//8.0f;
		m_nMinDamage = 3;
		m_nMaxDamage = 5;
		m_fLifespan = 2.0f;
		break;

	case WeaponType::HARPOON_GUN:
		m_fReloadMax = 2.0f;	//15.0f;
		m_nMinDamage = 5;
		m_nMaxDamage = 8;
		break;

	case WeaponType::UNKNOWN:
	default:
		break;
	}
}

Cannon & Cannon::operator=(const Cannon & other)
{
	// TODO: insert return statement here
	if (this == &other)
	{
		return (*this);
	}

	IEntity::operator=(other);

	m_bActive = other.m_bActive;
	m_eWeaponType = other.m_eWeaponType;
	m_fReloadTimer = other.m_fReloadTimer;
	m_fReloadMax = other.m_fReloadMax;
	m_nMinDamage = other.m_nMinDamage;
	m_nMaxDamage = other.m_nMaxDamage;
	m_nDelayMax = other.m_nDelayMax;
	m_fLifespan = other.m_fLifespan;
	m_fFiringSpread = other.m_fFiringSpread;
	m_nDelayMin = other.m_nDelayMin;
	m_FireLeft = FiringStruct();
	m_FireRight = FiringStruct();
	m_FireLeft.m_sFireOrder = other.m_FireLeft.m_sFireOrder;
	m_FireRight.m_sFireOrder = other.m_FireRight.m_sFireOrder;
	m_fLeftReloadTimer = 0.0f;
	m_fRightReloadTimer = 0.0f;


	return (*this);
}

Cannon::Cannon(const Cannon & _cpy)
	:IShipPart(_cpy)
{
	m_eWeaponType = _cpy.m_eWeaponType;
	m_fReloadTimer = _cpy.m_fReloadTimer;
	m_fReloadMax = _cpy.m_fReloadMax;
	m_nMinDamage = _cpy.m_nMinDamage;
	m_nMaxDamage = _cpy.m_nMaxDamage;
	m_fLifespan = _cpy.m_fLifespan;
	m_fFiringSpread = _cpy.m_fFiringSpread;
	m_bActive = _cpy.m_bActive;
	m_nDelayMin = _cpy.m_nDelayMin;
	m_nDelayMax = _cpy.m_nDelayMax;
	m_FireLeft = FiringStruct();
	m_FireRight = FiringStruct();
	m_FireLeft.m_sFireOrder = _cpy.m_FireLeft.m_sFireOrder;
	m_FireRight.m_sFireOrder = _cpy.m_FireRight.m_sFireOrder;
	m_fLeftReloadTimer = 0.0f;
	m_fRightReloadTimer = 0.0f;

}

Cannon::~Cannon()
{
	SoundManager*	sound = ((CGame*)CGame::GetApplication())->GetSoundManager();
	sound->UnregisterEntity(this);
}

void Cannon::OnCollision(Collider& other)
{
	//change this when out of core

	//no need to beat a dead horse
	if (m_bWrecked == true)
	{
		return;
	}

	//Call base class here 
	//	because base class 

	if (m_nHealth <= 0)
	{
		m_bWrecked = true;
	}



}

void Cannon::Update(float dt)
{
	// base update
	IShipPart::Update(dt);



	// cooldown bonus
	float cooldown = (m_owner->GetAlignment() != Ship::eAlignment::PLAYER) ? dt : static_cast<PlayerShip*>(m_owner)->CooldownBonus(dt);	//m_owner->CooldownBonus(dt);
	/*
	float cooldown = dt;
	if (m_owner->CooldownBonus() == true && m_owner->GetAlignment() == Ship::eAlignment::PLAYER)
		cooldown *= 2.0f;
	*/


	// timer--
	m_fReloadTimer -= cooldown;
	m_fLeftReloadTimer -= cooldown;
	m_fRightReloadTimer -= cooldown;

	// cap timers
	if (m_fReloadTimer <= 0.0f)
		m_fReloadTimer = 0.0f;

	if (m_fLeftReloadTimer <= 0.0f)
		m_fLeftReloadTimer = 0.0f;

	if (m_fRightReloadTimer <= 0.0f)
		m_fRightReloadTimer = 0.0f;


	// fire over 1+ frames
	FireLeft(dt);
	FireRight(dt);
}

bool Cannon::LevelUp()
{
	// base level up
	if (IShipPart::LevelUp() == false)
	{
		return false;
	}

	m_nHealth = m_nMaxHealth;


	// x = Lv/10
	//float temp = float(m_nLevel * 0.001f);

	// Att++ (Att += Att * (Lv/10))
	m_nMinDamage += 1;	//int(m_nMinDamage * temp);
	m_nMaxDamage += 1;	//int(m_nMaxDamage * temp);


	//life++
	m_fLifespan += 0.05f;	//m_fLifespan * temp;


	int next_cannon = m_nLevel + 2;
	if (next_cannon < TOTAL_NUM_CANNONS)
	{
		if (next_cannon < MAX_FIRING_BURST)
		{
			// fire order
			std::stringstream ss;
			ss << next_cannon;
			m_FireLeft.m_sFireOrder += ss.str();
			m_FireRight.m_sFireOrder += ss.str();

			// spread
			m_fFiringSpread -= 0.13f;
		}
	}



	// return
	return true;
}


bool Cannon::Fire(IAmmo::eAmmoType ammo, int num_to_fire, int side)
{
	if (m_bActive == false)
		return false;

    auto &gameStat = GameStatistics::GetRef();

	// which side?
	switch (side)
	{
	case 0:	// left
		if (m_fLeftReloadTimer > 0.0f)
			return false;
		m_fLeftReloadTimer = m_fReloadMax;
		m_FireLeft.m_fCurrDelay = CalculateDelay();
		m_FireLeft.m_bFiring = true;
		m_FireLeft.m_FiringAmmo = ammo;
		m_FireLeft.m_nNumToFire = num_to_fire;
		m_FireLeft.m_nFireCounter = 0;
		m_FireLeft.m_AmmoOffset = GetAmmoOffset();
		//m_FireLeft.m_AmmoOffset = (m_owner->GetAlignment() == Ship::eAlignment::PLAYER) ? XMFLOAT3(0.6f, 0.4f, 0.7f) : XMFLOAT3(0.6f, 0.4f, 0.4f); //XMFLOAT3(0.2f, 0.2f, 0.8f);
		m_FireLeft.m_nDamage = CalculateDamage();
		for (size_t i = 0; i < 5; i++)
			next_permutation(m_FireLeft.m_sFireOrder.begin(), m_FireLeft.m_sFireOrder.end());
		return true;
		break;

	case 1:	// right
		if (m_fRightReloadTimer > 0.0f)
			return false;
		m_fRightReloadTimer = m_fReloadMax;
		m_FireRight.m_fCurrDelay = CalculateDelay();
		m_FireRight.m_bFiring = true;
		m_FireRight.m_FiringAmmo = ammo;
		m_FireRight.m_nNumToFire = num_to_fire;
		m_FireRight.m_nFireCounter = 0;
		m_FireRight.m_AmmoOffset = GetAmmoOffset();
		//m_FireRight.m_AmmoOffset = (m_owner->GetAlignment() == Ship::eAlignment::PLAYER) ? XMFLOAT3(0.6f, 0.4f, 0.7f) : XMFLOAT3(0.6f, 0.4f, 0.4f); //XMFLOAT3(0.2f, 0.2f, 0.8f);
		m_FireRight.m_nDamage = CalculateDamage();
		for (size_t i = 0; i < 5; i++)
			next_permutation(m_FireRight.m_sFireOrder.begin(), m_FireRight.m_sFireOrder.end());
		return true;
		break;

	case 2:	// other...
		// cooldown check
		if (m_fReloadTimer > 0.0f)
			return false;
		m_fReloadTimer = m_fReloadMax;
		break;

	default:
		break;
	}


	// Fire ONLY front ammo after this line
	CGame*			game = (CGame*)CGame::GetApplication();
	SceneManager*	sm = game->GetSceneManager();
	SoundManager*	sound = game->GetSoundManager();
	XMFLOAT3		ammopos = XMFLOAT3(0.0f, 0.0f, 0.0f);
	int				damage = CalculateDamage();


	switch (m_eWeaponType)
	{
	case WeaponType::CANNON:
		// CANNONS DO NOT FIRE FROM HERE!!!
		// CANNONS FIRE OVER 1+ FRAMES TO GET RID OF ROBOTIC FEELING
		// CHECK FUNCTIONS "Cannon::FireLeft()" & "Cannon::FireRight()" FOR UPDATED CANNON SHOOTING
		break;

	case WeaponType::LONG_NINE:
        if ( m_owner->GetAlignment() == Ship::PLAYER ) {
            if ( ammo == IAmmo::CANNONBALL ) {
                gameStat.RegularShotFired( 1 );

            } else if(ammo == IAmmo::CHAIN_SHOT) {

                gameStat.ChainShotFired( 1 );

            }
        }
		ammopos = XMFLOAT3(0.0f, 0.2f, 1.2f);
		fsEventManager::GetInstance()->FireEvent(fsEvents::FireFrontCannon,
												 &EventArgs1<SceneManager::FireCannonEventArgs>(
													SceneManager::FireCannonEventArgs(m_owner, ammo, ammopos, m_fLifespan, damage)));
		/*
		sm->FireFrontCannonEvent(SceneManager::FireCannonEventArgs(m_owner, ammo, ammopos, m_fLifespan, damage));
		*/
		/*
		EventManager()->SendEventInstant("FireFrontCannonEvent", new CGeneralEventArgs<SceneManager::FireFrontCannonEventArgs>(SceneManager::FireFrontCannonEventArgs(m_owner, ammo, ammopos, 6.0f, damage)));
		*/
		// play sound
		sound->Play3DSound(AK::EVENTS::PLAYFIRING);
		break;

	case WeaponType::HARPOON_GUN:
		//ammopos = XMFLOAT3(0.01f, 0.0f, -0.8f);
		//sm->FireBackCannonEvent(SceneManager::FireBackCannonEvent(m_owner, ammo, ammopos, 5.0f, damage));
		////EventManager()->SendEventInstant("FireBackCannonEvent", new CGeneralEventArgs<SceneManager::FireBackCannonEventArgs>(SceneManager::FireBackCannonEventArgs(m_owner, ammo, ammopos, 5.0f, damage)));
		//// play sound
		//sound->Play3DSound(AK::EVENTS::PLAYFIRING);
		break;

	case WeaponType::UNKNOWN:
	default:
		break;
	}

	return true;
}

// FOR THE LOVE OF [insert Deity here] PLEASE DO NOT ERASE!!!
/*
void Cannon::Fire(IAmmo::eAmmoType ammo, int num_to_fire, int side)
{
	if (m_bActive == false)		//if (m_bActive == false || m_bWrecked == false)
		return;


	// which side?
	switch (side)
	{
	case 0:
		// cooldown check
		if (m_fLeftReloadTimer > 0.0f)
			return;

		// reset timer
		m_fLeftReloadTimer = m_fReloadMax;
		break;

	case 1:
		// cooldown check
		if (m_fRightReloadTimer > 0.0f)
			return;

		// reset timer
		m_fRightReloadTimer = m_fReloadMax;
		break;

	case 2:
		// cooldown check
		if (m_fReloadTimer > 0.0f)
			return;

		// reset timer
		m_fReloadTimer = m_fReloadMax;
		break;

	default:
		break;
	}
	//if ((m_fLeftReloadTimer > 0.0f && side == 0) ||
	//	(m_fRightReloadTimer > 0.0f && side == 1) ||
	//	(m_fReloadTimer > 0.0f && side == 2))
	//	return;
	//
	//
	//// reset timer
	//if (side == 0)
	//{
	//	m_fLeftReloadTimer = m_fReloadMax;
	//}
	//else if (side == 1)
	//{
	//	m_fRightReloadTimer = m_fReloadMax;
	//}
	//else
	//{
	//	m_fReloadTimer = m_fReloadMax;
	//}




	// Fire Cannonballs after this line
	SceneManager*	sm		= ((CGame*)CGame::GetApplication())->GetSceneManager();
	SoundManager*	sound	= ((CGame*)CGame::GetApplication())->GetSoundManager();
	XMFLOAT3		ammopos	= XMFLOAT3(0.0f, 0.0f, 0.0f);
	int				damage	= CalculateDamage();


	switch (m_eWeaponType)
	{
	case WeaponType::CANNON:
		ammopos = (m_owner->GetAlignment() == Ship::eAlignment::PLAYER) ? XMFLOAT3(0.6f, 0.4f, 0.8f) : XMFLOAT3(0.6f, 0.4f, 0.4f); //XMFLOAT3(0.2f, 0.2f, 0.8f);

		for (int i = 0; i < num_to_fire; i++)
		{
			ammopos.z -= 0.3f;	//0.5f;

			switch (side)
			{
			case 0:
				sm->FireLeftSideCannonEvent(SceneManager::FireCannonEventArgs(m_owner, ammo, ammopos, 0.75f, damage));
				break;
			case 1:
				sm->FireRightSideCannonEvent(SceneManager::FireCannonEventArgs(m_owner, ammo, ammopos, 0.75f, damage));
				break;
			case 2:
				sm->FireBothSideCannonsEvent(SceneManager::FireCannonEventArgs(m_owner, ammo, ammopos, 0.75f, damage));
				break;
			default:
				break;
			}

			//sm->FireSideCannonEvent(SceneManager::FireCannonEventArgs(side, m_owner, ammo, ammopos, 3.0f, damage));
			//m_fLeftReloadTimer = m_fReloadMax;
			//EventManager()->SendEventInstant("FireSideCannonEvent", new CGeneralEventArgs<SceneManager::FireSideCannonEventArgs>(SceneManager::FireSideCannonEventArgs(m_owner, ammo, ammopos, 3.0f, damage)));
		}
		// play sound
		sound->Play3DSound(AK::EVENTS::PLAYFIRING);
		break;


	case WeaponType::LONG_NINE:
		ammopos = XMFLOAT3(0.0f, 0.2f, 1.2f);
		sm->FireFrontCannonEvent(SceneManager::FireCannonEventArgs(m_owner, ammo, ammopos, 2.0f, damage));
		//EventManager()->SendEventInstant("FireFrontCannonEvent", new CGeneralEventArgs<SceneManager::FireFrontCannonEventArgs>(SceneManager::FireFrontCannonEventArgs(m_owner, ammo, ammopos, 6.0f, damage)));
		// play sound
		sound->Play3DSound(AK::EVENTS::PLAYFIRING);
		break;


	case WeaponType::HARPOON_GUN:
		//ammopos = XMFLOAT3(0.01f, 0.0f, -0.8f);
		//sm->FireBackCannonEvent(SceneManager::FireBackCannonEvent(m_owner, ammo, ammopos, 5.0f, damage));
		////EventManager()->SendEventInstant("FireBackCannonEvent", new CGeneralEventArgs<SceneManager::FireBackCannonEventArgs>(SceneManager::FireBackCannonEventArgs(m_owner, ammo, ammopos, 5.0f, damage)));
		//// play sound
		//sound->Play3DSound(AK::EVENTS::PLAYFIRING);
		break;



	case WeaponType::UNKNOWN:
	default:
		break;
	}
}
*/


void Cannon::WreckPart()
{
	//Decreasing damage
	m_nMinDamage = int(m_nMinDamage* 0.5f);
	m_nMaxDamage = int(m_nMaxDamage* 0.5f);
}

int Cannon::CalculateDamage()
{
	float r = (float)rand() / (float)RAND_MAX;
	int damage =  static_cast<int>(m_nMinDamage + r * (m_nMaxDamage - m_nMinDamage));
	return damage;
}

float Cannon::CalculateDelay()
{
	// random delay
	int idelay = rand() % (m_nDelayMax - m_nDelayMin + 1) + m_nDelayMin;

	// delay: int->float
	float fdelay = static_cast<float>(idelay);

	// 4/100 == 0.004
	fdelay *= 0.01f;

	// return
	return fdelay;
}

XMFLOAT3 Cannon::GetAmmoOffset()
{
	XMFLOAT3 offset = XMFLOAT3(0.0f, 0.0f, 0.0f);


	Ship::eAlignment	alignment	= m_owner->GetAlignment();
	std::string			mesh		= m_owner->GetMeshName();

	switch (alignment)
	{
	case Ship::eAlignment::PLAYER:
		offset = XMFLOAT3(0.6f, 0.4f, 0.0f); //XMFLOAT3(0.6f, 0.4f, 0.7f); //XMFLOAT3(0.2f, 0.2f, 0.8f);
		if (mesh == "PlayerShip.mesh")
			offset.z = 0.7f;
		else if (mesh == "PlayerShip2.mesh")
			offset.z = 0.4f;
		else if (mesh == "PlayerShip3.mesh")
			offset.z = 0.1f;
		break;

	case Ship::eAlignment::ENEMY:
	case Ship::eAlignment::OTHER:
		offset = XMFLOAT3(0.6f, 0.4f, 0.4f);

		if (mesh == "BossShip.mesh")
			offset = XMFLOAT3(0.6f, 0.4f, 1.2f);
		break;

	case Ship::eAlignment::UNKNOWN:
	default:
		break;
	}

	return offset;
}

float Cannon::GetFiringSpread()
{
	float spread = 0.0f;


	Ship::eAlignment alignment = m_owner->GetAlignment();

	switch (alignment)
	{
	case Ship::eAlignment::PLAYER:
		spread = 0.6f;
		break;

	case Ship::eAlignment::ENEMY:
	case Ship::eAlignment::OTHER:
		spread = 0.4f;
		break;

	case Ship::eAlignment::UNKNOWN:
	default:
		break;
	}

	return spread;
}

void Cannon::FireLeft(float dt)
{
	// not firing check
	if (m_FireLeft.m_bFiring == false)
		return;

	// delay--
	m_FireLeft.m_fCurrDelay -= dt;

	// delay check
	if (m_FireLeft.m_fCurrDelay > 0.0f)
		return;

	CGame*			game = (CGame*)CGame::GetApplication();
	SceneManager*	sm = game->GetSceneManager();
	SoundManager*	sound = game->GetSoundManager();
    auto&           gameStat = GameStatistics::GetRef();


	// adjust offset based on firing order
	int next = static_cast<int>(m_FireLeft.m_sFireOrder[m_FireLeft.m_nFireCounter]) - 48;
	float z = m_FireLeft.m_AmmoOffset.z;
	z = z - (m_fFiringSpread * static_cast<float>(next));

	// fire
	fsEventManager::GetInstance()->FireEvent(fsEvents::FireLeftCannons, &EventArgs1<SceneManager::FireCannonEventArgs>
																(SceneManager::FireCannonEventArgs
																	(m_owner, m_FireLeft.m_FiringAmmo,
																	 XMFLOAT3(m_FireLeft.m_AmmoOffset.x, m_FireLeft.m_AmmoOffset.y, z),
																	 m_fLifespan, m_FireLeft.m_nDamage)));
	/*
	sm->FireLeftSideCannonEvent(SceneManager::FireCannonEventArgs(m_owner, m_FireLeft.m_FiringAmmo,
		XMFLOAT3(m_FireLeft.m_AmmoOffset.x, m_FireLeft.m_AmmoOffset.y, z),
		m_fLifespan, m_FireLeft.m_nDamage));
	*/

	// play sound
	sound->Play3DSound(AK::EVENTS::PLAYLEFTCANNON);
	
    // Record fire count
    if ( m_owner->GetAlignment() == Ship::PLAYER ) {
        if ( m_FireLeft.m_FiringAmmo == IAmmo::CANNONBALL ) {
            gameStat.RegularShotFired( 1 );

        } else if ( m_FireLeft.m_FiringAmmo == IAmmo::CHAIN_SHOT ) {

            gameStat.ChainShotFired( 1 );

        }
    }

	// count++
	m_FireLeft.m_nFireCounter++;

	// fired 5 shots on 1 side
	if (m_FireLeft.m_nFireCounter % MAX_FIRING_BURST == 0)
	{
		m_FireLeft.m_nFireCounter	-= MAX_FIRING_BURST;
		m_FireLeft.m_nNumToFire		-= MAX_FIRING_BURST;
	}

	// stop firing
	if (m_FireLeft.m_nFireCounter >= m_FireLeft.m_nNumToFire)
	{
		m_FireLeft.m_bFiring = false;
		return;
	}

	// next delay
	m_FireLeft.m_fCurrDelay = CalculateDelay();
}

void Cannon::FireRight(float dt)
{
	// not firing check
	if (m_FireRight.m_bFiring == false)
		return;

	// delay--
	m_FireRight.m_fCurrDelay -= dt;

	// delay check
	if (m_FireRight.m_fCurrDelay > 0.0f)
		return;

	CGame*			game = (CGame*)CGame::GetApplication();
	SceneManager*	sm = game->GetSceneManager();
	SoundManager*	sound = game->GetSoundManager();
    auto&           gameStat = GameStatistics::GetRef();


	// adjust offset based on firing order
	int next = static_cast<int>(m_FireRight.m_sFireOrder[m_FireRight.m_nFireCounter]) - 48;
	float z = m_FireRight.m_AmmoOffset.z;
	z = z - (m_fFiringSpread * static_cast<float>(next));
	/*
	m_FireRight.m_AmmoOffset.z -= 0.3f;	//0.5f;
	m_FireLeft.m_AmmoOffset.z = m_FireLeft.m_AmmoOffset.z - (0.3f * static_cast<float>(next));
	*/

	// fire
	fsEventManager::GetInstance()->FireEvent(fsEvents::FireRightCannons, &EventArgs1<SceneManager::FireCannonEventArgs>
																(SceneManager::FireCannonEventArgs
																	(m_owner, m_FireRight.m_FiringAmmo,
																	 XMFLOAT3(m_FireRight.m_AmmoOffset.x, m_FireRight.m_AmmoOffset.y, z),
																	 m_fLifespan, m_FireRight.m_nDamage)));
	/*
	sm->FireRightSideCannonEvent(SceneManager::FireCannonEventArgs(m_owner, m_FireRight.m_FiringAmmo,
		XMFLOAT3(m_FireRight.m_AmmoOffset.x, m_FireRight.m_AmmoOffset.y, z),
		m_fLifespan, m_FireRight.m_nDamage));
	*/

	// play sound
	sound->Play3DSound(AK::EVENTS::PLAYRIGHTCANNON);

    // Record fire count
    if ( m_owner->GetAlignment() == Ship::PLAYER ) {
        if ( m_FireLeft.m_FiringAmmo == IAmmo::CANNONBALL ) {
            gameStat.RegularShotFired( 1 );

        } else if ( m_FireLeft.m_FiringAmmo == IAmmo::CHAIN_SHOT ) {

            gameStat.ChainShotFired( 1 );

        }
    }

	// count++
	m_FireRight.m_nFireCounter++;

	// fired 5 shots on 1 side
	if (m_FireRight.m_nFireCounter % MAX_FIRING_BURST == 0)
	{
		m_FireRight.m_nFireCounter	-= MAX_FIRING_BURST;
		m_FireRight.m_nNumToFire	-= MAX_FIRING_BURST;
	}

	// stop firing
	if (m_FireRight.m_nFireCounter >= m_FireRight.m_nNumToFire)
	{
		m_FireRight.m_bFiring = false;
		return;
	}

	// next delay
	m_FireRight.m_fCurrDelay = CalculateDelay();
}

const int Cannon::GetMinDamage()
{
	return m_nMinDamage;
}

const int Cannon::GetMaxDamage()
{
	return m_nMaxDamage;
}

void Cannon::SetDamage(int min, int max)
{
	if (min >= 0)
		m_nMinDamage = min;

	if (max >= 0)
		m_nMaxDamage = max;
}


#pragma region Jason code commented out

/*
void Cannon::Fire(IAmmo::eAmmoType& ammo)
{



	//wrecked?
	if (m_bWrecked == true)
		return;


	if (m_owner == nullptr)
		assert("Cannon::Fire - cannon has no owner!");

	SideCannonHandler* cannonholder = (SideCannonHandler*)m_owner->GetShipPart(IShipPart::PartType::WEAPON_CANNON);
	if (cannonholder == nullptr)
		assert("Cannon::Fire - owner has no side cannons!");

	if (cannonholder->GetCurrentAmmoIndex() > IAmmo::eAmmoType::UNKNOWN && cannonholder->GetCurrentAmmoIndex() < IAmmo::eAmmoType::HARPOON)
		return;


	// TODO: Firing a cannonball

}
*/

#pragma endregion 