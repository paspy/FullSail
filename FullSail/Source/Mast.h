/**************************************************************************************/
/*
Limits Ship Speed

Author : David Ashbourne(Full Sail)
File : Mast
Contributor(s) : Jason Bloomfield (Add name here if you like)
*/
/**************************************************************************************/
#pragma once
#include "IShipPart.h"
#include "Ship.h"
#include "IAmmo.h"

class Mast : public IShipPart
{
public:
	Mast(Ship* owner, float MaxSpeed, const XMFLOAT4X4& _newTransfrom);// : m_fMaxSpeed(MaxSpeed), IShipPart(owner, _newTransfrom) {}
	
	//****************Trilogy*****************//
	Mast& operator=(const Mast& other);
	Mast(const Mast& _cpy);

	virtual ~Mast();
		
	//**************Functions****************//
	/*
	void OnCollision(Collider& other);
	*/

	void Update(float _dt);
	bool LevelUp();
	virtual const	int			GetHealth(void)				{ return (int)m_fCurrentMaxSpeed; }
	virtual const	int			GetMaxHealth(void)			{ return (int)m_fMaxSpeed; }

	inline virtual void Fire(IAmmo::eAmmoType ammo)			{ /* DO NOTHING! */ }
	inline virtual void Fire(IShipPart::PartType weapon)	{ /* DO NOTHING! */ }
	inline virtual void WreckPart()							{ /* DO NOTHING! */ }


	/*****************Accessor******************/
	inline virtual const PartType GetPartType() final
	{
		return PartType::MAST;
	}
	const float& GetMaxSpeed();
	const float& GetSlowdownMultiplier();


	/*****************Mutators*******************/
	void SetMaxSpeed(float _s);
	void SetSlowdownMultiplier(float _s);
	void ReduceSpeed();

	float m_fMaxSpeed;
private:

	float m_fCurrentMaxSpeed;
	float m_fSlowdownMultiplier;
	float m_fMaxTimer = 10;
	float m_fReducedSpeedTimer = 0;


};

