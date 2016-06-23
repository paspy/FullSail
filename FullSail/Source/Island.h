#pragma once
#include "Entity3D.h"
class Island :
	public Entity3D
{
	std::vector<XMFLOAT3> m_vSafePoints;
	std::string m_sCollision;
public:

	Island(const XMFLOAT4X4 & _newTransfrom, const char* textureName);

	Island(const Island& cpy);


	Island& operator=(const Island& other);

	const char* GetCollisionMesh();
	void SetCollisionMesh(const char* colMesh);

	//this will return the vector holding all waypoints
	inline std::vector<XMFLOAT3>& GetSafePointVector(void) { return m_vSafePoints; }

	//returns the point at the requested index
	const XMFLOAT3& GetSafePoint(int index);

	std::string PortName;
	XMFLOAT4X4 PortPosition;

	~Island();
};

