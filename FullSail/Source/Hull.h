/**************************************************************************************/
/*
Limit Ship turn rate

Author : David Ashbourne(Full Sail)
File : Hull
Contributor(s) : Jason Bloomfield (Add name here if you like)
*/
/**************************************************************************************/
#pragma once
//#include "IShipPart.h"
#include "Ship.h"
#include "IAmmo.h"

//class IShipPart;
class Ship;


class Hull : public IShipPart
{
public:
	Hull(Ship* owner, float maxTurnRate, const XMFLOAT4X4& _newTransfrom);// : m_fMaxTurnRate(MaxTurnRate), IShipPart(owner, _newTransfrom) {}
	
	//****************Trilogy*****************//
	Hull& operator=(const Hull& other);
	Hull(const Hull& _cpy);
	
	virtual ~Hull();


	//**************Functions****************//
	//virtual void OnCollision(Collider& other);
	virtual void Update(float _dt);
	virtual bool LevelUp();
	inline virtual void Fire(IAmmo::eAmmoType ammo)			{ /* DO NOTHING! */ }
	inline virtual void Fire(IShipPart::PartType weapon)	{ /* DO NOTHING! */ }
	inline virtual void WreckPart()							{ /* DO NOTHING! */ }


	/*****************Accessor******************/
	inline virtual const PartType GetPartType() final
	{
		return PartType::HULL;
	}
	const float& GetMaxTurnRate();


	/*****************Mutators*******************/
	void SetMaxTurnRate(float _tr);


private:

	float m_fMaxTurnRate;
};

