#include "pch.h"
#include "SideCannonHandler.h"
#include "Cannon.h"



SideCannonHandler::SideCannonHandler(Ship* owner, const XMFLOAT4X4& _newTransfrom) : IShipPart(owner, _newTransfrom)
{
	m_nNumAcquiredCannons = 1;

	//WHEN XML LOading Starts take this out


	//Cause this is pointless and useless

	// create cannons
	m_pCannons = new Cannon(owner, Cannon::WeaponType::CANNON, _newTransfrom);
	SetNumAcquiredCannons(m_nNumAcquiredCannons);

	// create long9
	m_pLong9 = new Cannon(owner, Cannon::WeaponType::LONG_NINE, _newTransfrom);

	// create harpoon gun
	m_pHarpoonGun = new Cannon(owner, Cannon::WeaponType::HARPOON_GUN, _newTransfrom);


}

SideCannonHandler::~SideCannonHandler()
{
	// delete cannons
	delete m_pCannons;
	m_pCannons = nullptr;


	// delete long9
	delete m_pLong9;
	m_pLong9 = nullptr;


	// delete harpoon gun
	delete m_pHarpoonGun;
	m_pHarpoonGun = nullptr;
}

SideCannonHandler & SideCannonHandler::operator=(const SideCannonHandler & other)
{
	//Self Check
	if (this == &other)
	{
		return (*this);
	}
	
	//Calling the base operator
	IShipPart::operator=(other);

	//Assigning the data
	m_pCannons				= new Cannon((*other.m_pCannons));
	m_pLong9				= new Cannon((*other.m_pLong9));
	m_pHarpoonGun			= new Cannon((*other.m_pHarpoonGun));
	m_nCurrentAmmoIndex		= other.m_nCurrentAmmoIndex;
	m_nNumAcquiredCannons	= other.m_nNumAcquiredCannons;
	SetNumAcquiredCannons(m_nNumAcquiredCannons);


	return (*this);
}

SideCannonHandler::SideCannonHandler(const SideCannonHandler & _cpy)
	:IShipPart(_cpy)
{
	m_pCannons = new Cannon((*_cpy.m_pCannons));
	m_pLong9 = new Cannon((*_cpy.m_pLong9));
	m_pHarpoonGun = new Cannon((*_cpy.m_pHarpoonGun));
	m_nCurrentAmmoIndex = _cpy.m_nCurrentAmmoIndex;
	m_nNumAcquiredCannons = _cpy.m_nNumAcquiredCannons;
	SetNumAcquiredCannons(m_nNumAcquiredCannons);
}

void SideCannonHandler::Update(float dt)
{
	// update cannons
	m_pCannons->Update(dt);

	// update long9
	m_pLong9->Update(dt);

	// update harpoon gun
	m_pHarpoonGun->Update(dt);
}

bool SideCannonHandler::LevelUp()
{
	// base NOT to be called!
	/*
	IShipPart::LevelUp();
	*/



	//// timers
	//float temp = float(m_nLevel * 0.10f);
	//for (size_t i = 0; i < 5; i++)
	//{
	//	// timer-- (timer -= timer * (Lv/10))
	//	//m_fReloadTimers[i] += m_fReloadTimers[i] * temp;
	//}


	// add a cannon
	m_nNumAcquiredCannons++;
	if (m_nNumAcquiredCannons >= MAX_CANNONS)
		m_nNumAcquiredCannons = MAX_CANNONS;


	// upgrade weapons
	//bool upgraded = false;

	// cannons
	bool u1 = m_pCannons->LevelUp();
	//if (upgraded == false)
	//	assert("SideCannonHandler::LevelUp - A left cannon did not level up!");

	// long9
	bool u2 = m_pLong9->LevelUp();

	// harpoon gun
	bool u3 = m_pHarpoonGun->LevelUp();



	// WTF?!?!?!?!
	/*
	if (m_nLevel % 2 == 0 && m_nLevel < 8)
	{
		//Turning on the first cannons that is off
		for (unsigned int i = 0; i < MAX_CANNONS; i++)
		{
			if (m_pCannons[i]->Activate() == true)
			{
				break;
			}
		}

		return true;
	}
	*/


	if (u1 == false && u2 == false && u3 == false)
		return false;

    //IShipPart::LevelUp();
    // max check
    if ( m_nLevel >= LEVEL_MAX )
        return false;

    // level++
    m_nLevel++;

    // cap level
    if ( m_nLevel >= LEVEL_MAX )
        m_nLevel = LEVEL_MAX;





	return true;
}

bool SideCannonHandler::Repair()
{
	// repair cannons
	bool r1 = m_pCannons->Repair();


	// repair long9
	bool r2 = m_pLong9->Repair();


	// repair harpoon gun
	bool r3 = m_pHarpoonGun->Repair();


	if (r1 == false && r2 == false && r3 == false)
		return false;

	return true;
}

void SideCannonHandler::CycleAmmoUp()
{
	// ammo++
	m_nCurrentAmmoIndex = (m_nCurrentAmmoIndex + 1 <= MAX_AMMOTYPES) ? (m_nCurrentAmmoIndex + 1) : 0;
}

void SideCannonHandler::CycleAmmoDown()
{
	// ammo--
	m_nCurrentAmmoIndex = (m_nCurrentAmmoIndex - 1 >= 0) ? (m_nCurrentAmmoIndex - 1) : MAX_AMMOTYPES;
}

bool SideCannonHandler::Fire(Cannon::WeaponType weapon ,int side)
{
	// use selected ammo
	IAmmo::eAmmoType ammo = (IAmmo::eAmmoType)m_nCurrentAmmoIndex;


	// fire weapons
	switch (weapon)
	{
	case Cannon::WeaponType::CANNON:
		return m_pCannons->Fire(ammo, m_nNumAcquiredCannons, side);
		break;

	case Cannon::WeaponType::LONG_NINE:
		return m_pLong9->Fire(ammo, m_nNumAcquiredCannons, side);
		break;

	case Cannon::WeaponType::HARPOON_GUN:
		return m_pHarpoonGun->Fire(IAmmo::eAmmoType::HARPOON, m_nNumAcquiredCannons, side);
		break;



	case Cannon::WeaponType::UNKNOWN:
	default:
		break;
	}
	return false;
}

const float SideCannonHandler::GetReloadTime(Cannon::WeaponType index)
{
	// index check
	if (index <= Cannon::WeaponType::UNKNOWN)
		assert("SideCannonHandler::GetReloadTime - ammo type is unknown!");


	// get reload time
	switch (index)
	{
	case Cannon::WeaponType::CANNON:
		return m_pCannons->GetReloadTime();
		break;

	case Cannon::WeaponType::LONG_NINE:
		return m_pLong9->GetReloadTime();
		break;

	case Cannon::WeaponType::HARPOON_GUN:
		return m_pHarpoonGun->GetReloadTime();
		break;

	case Cannon::WeaponType::UNKNOWN:
	default:
		break;
	}


	return -1.0f;
}

const int SideCannonHandler::GetCurrentAmmoIndex()
{
	return (IAmmo::eAmmoType)m_nCurrentAmmoIndex;
}

const int SideCannonHandler::GetNumAcquiredCannons()
{
	return m_nNumAcquiredCannons;
}

void SideCannonHandler::SetReloadTime(int index, float _rt)
{
	// index check
	if (index <= IAmmo::eAmmoType::UNKNOWN)
		assert("SideCannonHandler::GetReloadTime - ammo type is unknown!");

	//m_fReloadTimers[(int)index] = _rt;
}

void SideCannonHandler::SetCurrentAmmoIndex(int _cai)
{
	m_nCurrentAmmoIndex = _cai;
}

void SideCannonHandler::SetNumAcquiredCannons(int _nac)
{
	m_nNumAcquiredCannons = _nac;

	if (m_nNumAcquiredCannons < 0)
		m_nNumAcquiredCannons = 0;
	if (m_nNumAcquiredCannons > TOTAL_NUM_CANNONS)
		m_nNumAcquiredCannons = TOTAL_NUM_CANNONS;

	m_pCannons->GetFiringStructLeft().m_sFireOrder.clear();
	m_pCannons->GetFiringStructRight().m_sFireOrder.clear();

	for (int i = 0; i < m_nNumAcquiredCannons; i++)
	{
		if (i >= MAX_FIRING_BURST)
			break;
		m_pCannons->GetFiringStructLeft().m_sFireOrder.append(std::to_string(i));
		m_pCannons->GetFiringStructRight().m_sFireOrder.append(std::to_string(i));
	}
	/*
	if (_nac < 0)
		_nac = 0;
	if (_nac > TOTAL_NUM_CANNONS)
		_nac = 15;

	m_nNumAcquiredCannons = _nac;

	for (int i = 0; i < MAX_FIRING_BURSTS; i++)
	{
		m_pCannons->GetFiringStructLeft().m_sFireOrder[i].clear();
		m_pCannons->GetFiringStructRight().m_sFireOrder[i].clear();
	}

	for (int i = 0; i < m_nNumAcquiredCannons; i++)
	{
		int index = Cannon::GetFiringOrderIndex(i);
		m_pCannons->GetFiringStructLeft().m_sFireOrder[index].append(std::to_string(i));
		m_pCannons->GetFiringStructRight().m_sFireOrder[index].append(std::to_string(i));
	}
	*/
}

void SideCannonHandler::SetLongNine(Cannon * newCannon)
{

	if (m_pLong9)
	{
		delete m_pLong9;
		m_pLong9 = nullptr;
	}

	m_pLong9 = newCannon;
}

void SideCannonHandler::SetSideCannon(Cannon * newCannon)
{

	if (m_pCannons)
	{
		delete m_pCannons;
		m_pCannons = nullptr;
	}

	m_pCannons = newCannon;
}

void SideCannonHandler::SetHarpoon(Cannon * newCannon)
{

	if (m_pHarpoonGun)
	{
		delete m_pHarpoonGun;
		m_pHarpoonGun = nullptr;
	}

	m_pHarpoonGun = newCannon;
}

void SideCannonHandler::SetOwner(Ship * owner)
{
	m_owner = owner;
	m_pCannons->SetOwner(owner);
	m_pLong9->SetOwner(owner);
	m_pHarpoonGun->SetOwner(owner);
}

void SideCannonHandler::SetLocalMatrix(const XMFLOAT4X4 & matrix)
{
	m_transform.SetLocalMatrix(matrix);
	m_pCannons->GetTransform().SetLocalMatrix(matrix);
	m_pLong9->GetTransform().SetLocalMatrix(matrix);
	m_pHarpoonGun->GetTransform().SetLocalMatrix(matrix);
}

#pragma region Comment out So Jason doesnt yell at me

//Jason FireCode
/*
void SideCannonHandler::Fire(IShipPart::PartType weapon)
{
	IAmmo::eAmmoType ammo = (IAmmo::eAmmoType)m_nCurrentAmmoIndex;
	switch (weapon)
	{
		// cannons
	case IShipPart::PartType::WEAPON_CANNON:
		for (int i = 0; i < m_nNumAcquiredCannons; i++)
		{
			m_pCannonsLeft[i]->Fire(ammo);
			m_pCannonsRight[i]->Fire(ammo);
		}
		break;

		// long9
	case IShipPart::PartType::WEAPON_LONG9:
		break;

		// harpoon gun
	case IShipPart::PartType::WEAPON_HARPOONGUN:
		break;

		// etc....
	case IShipPart::PartType::UNKNOWN:
	case IShipPart::PartType::HULL:
	case IShipPart::PartType::MAST:
	default:
		assert("SideCannonHandler::Fire - unknown weapon to fire!");
		break;
	}
}
*/




#pragma endregion