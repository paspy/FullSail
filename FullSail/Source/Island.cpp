#include "pch.h"
#include "Island.h"


Island::Island(const XMFLOAT4X4 & _newTransfrom, const char* textureName)
	:Entity3D(_newTransfrom, "", textureName)
{
	PortName = "";

}

Island::Island(const Island & cpy)
	:Entity3D(cpy)
{
	m_vSafePoints = cpy.m_vSafePoints;
	m_sCollision = cpy.m_sCollision;

	PortName = cpy.PortName;
	PortPosition = cpy.PortPosition;

	//Firing an event
	if (strcmp(PortName.c_str(),""))
	{
		fsEventManager::GetInstance()->FireEvent(fsEvents::LoadObject, &EventArgs2<const char*, XMFLOAT4X4>(PortName.c_str(), PortPosition * m_transform.GetWorldMatrix()));
	}

}


Island& Island::operator=(const Island& other)
{
	if (this == &other)
	{
		return (*this);
	}

	//Parent assignment 

	Entity3D::operator=(other);

	m_vSafePoints = other.m_vSafePoints;
	m_sCollision = other.m_sCollision;
	PortName = other.PortName;
	PortPosition = other.PortPosition;

	//Firing an event
	if (strcmp(PortName.c_str(), ""))
	{
		fsEventManager::GetInstance()->FireEvent(fsEvents::LoadObject, &EventArgs2<const char*, XMFLOAT4X4>(PortName.c_str(), PortPosition));
	}

	return (*this);
}

//This can break if you pass in a number while you do not have any wayoints
//I want it to break cause I dont know invalid outputs
const XMFLOAT3 & Island::GetSafePoint(int index)
{
	//Getting the size of the Vector
	int vectorSize = (int)m_vSafePoints.size();

	//Checking if you pass an invaild index
	if (index >= vectorSize)
	{
		//if so then set the index equal to your last index
		index = vectorSize - 1;
	}

	//returning the vector of safepoints
	return m_vSafePoints[index];
}

const char * Island::GetCollisionMesh()
{
	return m_sCollision.c_str();
}

void Island::SetCollisionMesh(const char * colMesh)
{
	if (colMesh)
	{
		m_sCollision = colMesh;
		m_sCollision += ".mesh";
	}
}

Island::~Island()
{

}
