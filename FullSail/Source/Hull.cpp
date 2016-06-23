/**************************************************************************************/
/*
Limit Ship turn rate

Author : David Ashbourne(Full Sail)
File : Hull
Contributor(s) : Jason Bloomfield (Add name here if you like)
*/
/**************************************************************************************/
#include "pch.h"
#include "Hull.h"


Hull::Hull(Ship* owner, float maxTurnRate, const XMFLOAT4X4& _newTransfrom) : IShipPart(owner, _newTransfrom)
{
	m_fMaxTurnRate = maxTurnRate;
}

Hull & Hull::operator=(const Hull & other)
{
	//Self Check
	if (this == &other)
	{
		return (*this);
	}
	//Calling the base operator
	IShipPart::operator=(other);
	//Assigning the data
	m_fMaxTurnRate = other.m_fMaxTurnRate;
	
	//Returning
	return (*this);
}

Hull::Hull(const Hull & _cpy)
	:IShipPart(_cpy)
{
	m_fMaxTurnRate = _cpy.m_fMaxTurnRate;
}

Hull::~Hull()
{

}

/*
void Hull::OnCollision(Collider& other)
{

}
*/

void Hull::Update(float _dt)
{
	// base update
	IShipPart::Update(_dt);

	//if (m_bWrecked)
	//{
	//	//m_owner->KillOwner(this);
	//}

	// etc....
}

bool Hull::LevelUp()
{
	// base level up
	if (IShipPart::LevelUp() == false)
	{
		return false;
	}


	// turn rate++ (rate += rate * (Lv/10))
	m_fMaxTurnRate += m_fMaxTurnRate * float(m_nLevel * 0.05f);


	// return
	return true;
}

const float& Hull::GetMaxTurnRate()
{
	return m_fMaxTurnRate;
}

void Hull::SetMaxTurnRate(float _tr)
{
	m_fMaxTurnRate = _tr;
}

