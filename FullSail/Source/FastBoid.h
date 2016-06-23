#pragma once
#include "IBoid.h"
class FastBoid :
	public IBoid
{
public:
	FastBoid(AiController* Owner, XMFLOAT3& Position, XMFLOAT3& Heading, const float& MaxTurnSpeed, const float& MaxSpeed);

	virtual void Update(float dt);

	~FastBoid();
protected:
	virtual void Movement(float dt) override;
	virtual XMFLOAT3 TravelToAttackTarget() override;
	virtual void FireCannons(const XMFLOAT3& toVector) override;
};

