#include "pch.h"
#include "IEntity.h"
//#include "ParticleManager.h"


IEntity::IEntity() : m_refCount(0)
{
	m_transform = Transform();
	m_transform.SetLocalMatrix(m_transform.GetLocalMatrix());
	m_transform.GetWorldMatrix();

	CoCreateGuid(&m_tGUID);
	OLECHAR* bstrGuid;
	StringFromCLSID(m_tGUID, &bstrGuid);
	m_sName = std::wstring(bstrGuid);
	CoTaskMemFree(bstrGuid);

	to_delete = false;
}


IEntity::IEntity(const XMFLOAT4X4& transform) : m_refCount(0)
{
	m_transform.SetLocalMatrix(transform);
	m_transform.GetWorldMatrix();

	CoCreateGuid(&m_tGUID);
	OLECHAR* bstrGuid;
	StringFromCLSID(m_tGUID, &bstrGuid);
	m_sName = std::wstring(bstrGuid);
	CoTaskMemFree(bstrGuid);

	to_delete = false;
}


IEntity& IEntity::operator=(const IEntity & other)
{
	if (this == &other)
	{
		return (*this);
	}

	to_delete = false;

	return (*this);
}

IEntity::IEntity(const IEntity & _cpy) : m_refCount(0)
{
	// TODO: insert return statement here
	m_transform = _cpy.m_transform;
	m_transform.GetWorldMatrix();

	CoCreateGuid(&m_tGUID);
	OLECHAR* bstrGuid;
	StringFromCLSID(m_tGUID, &bstrGuid);
	m_sName = std::wstring(bstrGuid);
	CoTaskMemFree(bstrGuid);

	to_delete = false;
}

IEntity::~IEntity()
{
	m_transform = Transform();

	//ClearParticles();
}



Transform & IEntity::GetTransform()
{
	// insert return statement here
	return m_transform;
}


XMFLOAT4X4 & IEntity::GetLocalMatrix()
{
	// insert return statement here
	return m_transform.GetLocalMatrix();
}


const XMFLOAT4X4 & IEntity::GetWorldMatrix()
{
	// insert return statement here
	return m_transform.GetWorldMatrix();
}


const bool IEntity::ToBeDeleted()
{
	return to_delete;
}


void IEntity::SetPosition(XMFLOAT3 & pos)
{
	setPosition(m_transform.GetLocalMatrix(), pos);
	m_transform.DirtyTransform();

}

void IEntity::SetTransform(Transform& _transform)
{
	m_transform = _transform;
	m_transform.DirtyTransform();
}


void IEntity::SetToBeDeleted(bool d)
{
	to_delete = d;
}


