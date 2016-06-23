#pragma once
#include "IShipPart.h"
#include "Cannon.h"

#define MAX_CANNONS		((int)5)
#define MAX_AMMOTYPES	((int)2)

class Ship;
//class Cannon;
class IAmmo;


class SideCannonHandler : public IShipPart
{
public:
	SideCannonHandler(Ship* owner, const XMFLOAT4X4& _newTransfrom);

	//****************Trilogy*****************//
	SideCannonHandler& operator=(const SideCannonHandler& other);
	SideCannonHandler(const SideCannonHandler& _cpy);
	virtual ~SideCannonHandler();

	//**************Functions****************//
	virtual void Update(float dt);
	virtual bool LevelUp();
	virtual bool Repair();
	void CycleAmmoUp();
	void CycleAmmoDown();
	bool Fire(Cannon::WeaponType weapon,int side); // 0 left 1 right 2 both

	/***************Accessor***************/
	virtual const float GetReloadTime(Cannon::WeaponType Index);
	const int GetCurrentAmmoIndex();
	const int GetNumAcquiredCannons();
	Cannon* GetCannons() const { return m_pCannons; }
	Cannon* GetLongNine() const { return m_pLong9; }


	/*****************Mutators*******************/
	void SetReloadTime(int index, float _rt);
	void SetCurrentAmmoIndex(int _cai);
	void SetNumAcquiredCannons(int _nac);
	void SetLongNine(Cannon* newCannon);
	void SetSideCannon(Cannon* newCannon);
	void SetHarpoon(Cannon* newCannon);
	void SetOwner(Ship* owner);
	void SetLocalMatrix( const XMFLOAT4X4& matrix);
private:
	Cannon*				m_pCannons;
	Cannon*				m_pLong9;
	Cannon*				m_pHarpoonGun;

	int					m_nCurrentAmmoIndex		= 0;
	int					m_nNumAcquiredCannons	= 0;
};

