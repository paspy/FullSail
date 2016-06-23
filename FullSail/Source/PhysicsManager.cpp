#include "pch.h"
#include "PhysicsManager.h"


PhysicsManager::PhysicsManager()
{
	MovingObjects.reserve(300);
}


PhysicsManager::~PhysicsManager()
{
}

char PhysicsManager::Update(float dTime)
{
	for (UpdateIterator = MovingObjects.begin(); UpdateIterator != MovingObjects.end(); /*nothing*/)
	{
		if (UpdateIterator->first)
		{
			*UpdateIterator->first = XMMatrixTranslationFromVector(XMLoadFloat3(&(UpdateIterator->second.wtv * dTime * 3))) * *UpdateIterator->first;
			*UpdateIterator->first = XMMatrixRotationRollPitchYawFromVector(XMLoadFloat3(&(UpdateIterator->second.lav * dTime * 3))) * *UpdateIterator->first;
			*UpdateIterator->first *= XMMatrixTranslationFromVector(XMLoadFloat3(&(UpdateIterator->second.wtv * dTime * 3)));
			*UpdateIterator->first *= XMMatrixRotationRollPitchYawFromVector(XMLoadFloat3(&(UpdateIterator->second.wav * dTime * 3)));
			++UpdateIterator;
		}
		else
			MovingObjects.erase(UpdateIterator++);
	}
	return false;
}

void PhysicsManager::SetVelocity(XMFLOAT4X4 & t, XMFLOAT3& v, bool local)
{
	if (local)
		MovingObjects[&t].ltv = v;
	else
		MovingObjects[&t].wtv = v;
}

void PhysicsManager::OffsetVelocity(XMFLOAT4X4 & t, XMFLOAT3& v, bool local)
{
	if (local)
		MovingObjects[&t].ltv += v;
	else
		MovingObjects[&t].wtv += v;
}

void PhysicsManager::SetRotationDegrees(XMFLOAT4X4 & t, XMFLOAT3& d, bool local)
{
	if (local)
		MovingObjects[&t].lav = d / RotationScaleDegrees;
	else
		MovingObjects[&t].wav = d / RotationScaleDegrees;
}

void PhysicsManager::OffsetRotationDegrees(XMFLOAT4X4 & t, XMFLOAT3& d, bool local)
{
	if (local)
		MovingObjects[&t].lav += d / RotationScaleDegrees;
	else
		MovingObjects[&t].wav += d / RotationScaleDegrees;
}

void PhysicsManager::SetRotationRadians(XMFLOAT4X4 & t, XMFLOAT3& r, bool local)
{
	if (local)
		MovingObjects[&t].lav = r / XM_PI;
	else
		MovingObjects[&t].wav = r / XM_PI;
}

void PhysicsManager::OffsetRotationRadians(XMFLOAT4X4 & t, XMFLOAT3& r, bool local)
{
	if (local)
		MovingObjects[&t].lav += r / XM_PI;
	else
		MovingObjects[&t].wav += r / XM_PI;
}

void PhysicsManager::StopMoving(XMFLOAT4X4 & t)
{
	MovingObjects.erase(&t);
}