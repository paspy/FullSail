#include "pch.h"
#include "FastBoid.h"

#include "Math.h"
#include "AiController.h"

FastBoid::FastBoid(AiController* Owner , XMFLOAT3& Position, XMFLOAT3& Heading, const float& MaxTurnSpeed, const float& MaxSpeed)
	: IBoid(Owner, Position, Heading, MaxTurnSpeed, MaxSpeed)
{
}

void FastBoid::Update(float dt)
{
}


FastBoid::~FastBoid()
{
}

void FastBoid::Movement(float dt)
{

}

XMFLOAT3 FastBoid::TravelToAttackTarget()
{
	return VectorZero;
}


void FastBoid::FireCannons(const XMFLOAT3 & toVector)
{
}
