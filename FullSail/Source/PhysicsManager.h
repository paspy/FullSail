#pragma once
#include "pch.h"
#include "Math.h"

using namespace DirectX;

struct MovementInfo
{
	XMFLOAT3 ltv;
	XMFLOAT3 lav;
	XMFLOAT3 wtv;
	XMFLOAT3 wav;
};

class PhysicsManager
{
private:
	std::unordered_map<XMFLOAT4X4*, MovementInfo>::iterator UpdateIterator;
	std::unordered_map<XMFLOAT4X4*, MovementInfo> MovingObjects;
public:
	PhysicsManager();
	~PhysicsManager();
	//update the positions and rotations of all moving XMFLOAT4X4s
	char Update(float dTime);
	//set the velocity of the XMFLOAT4X4
	void SetVelocity(XMFLOAT4X4& t, XMFLOAT3& v, bool local = false);
	//the passed in velocity will be added to the existing velocity
	void OffsetVelocity(XMFLOAT4X4& t, XMFLOAT3& v, bool local = false);
	//set rotation of the XMFLOAT4X4
	void SetRotationDegrees(XMFLOAT4X4& t, XMFLOAT3& d, bool local = false);
	//the passed in rotation will be added to the existing rotation
	void OffsetRotationDegrees(XMFLOAT4X4& t, XMFLOAT3& d, bool local = false);
	//set rotation of the XMFLOAT4X4
	void SetRotationRadians(XMFLOAT4X4& t, XMFLOAT3& r, bool local = false);
	//the passed in rotation will be added to the existing rotation
	void OffsetRotationRadians(XMFLOAT4X4& t, XMFLOAT3& r, bool local = false);
	//please call this before you destroy your object
	void StopMoving(XMFLOAT4X4& t);
};