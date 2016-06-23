/**************************************************************************************/
/*
	This is an abstract class for all entities
	to inherit off of

	Author : David Ashbourne(Full Sail)
	Contributor(s) : Jason Bloomfield , Chen Lu (Add name here if you like)
*/
/**************************************************************************************/
#pragma once
#include <vector>
#include "Transform.h"
#include <MemoryManager.h>

//namespace FSParticle {
//    class CParticleSystem;
//}

class IEntity
{
public:
	IEntity();
	IEntity(const XMFLOAT4X4& transform);
	virtual ~IEntity();

	//****************Trilogy*****************//
	IEntity& operator=(const IEntity& other);
	IEntity(const IEntity& _cpy);

	//**************Functions****************//
	virtual void Update(float dt) = 0;

	//**************Accessors****************//
	virtual	Transform& GetTransform();
	XMFLOAT3 GetPosition() { return positionOf(m_transform.GetWorldMatrix()); }
	XMFLOAT3 GetForward() { return forwardAxisOf(m_transform.GetWorldMatrix()); }
	XMFLOAT3 GetRight() { return rightAxisOf(m_transform.GetWorldMatrix()); }
	virtual XMFLOAT4X4& GetLocalMatrix();
	virtual const XMFLOAT4X4& GetWorldMatrix();

	virtual const bool ToBeDeleted();
	virtual void Name(std::wstring val) { m_sName = val; }


	//**************Mutators****************//
	void		SetPosition(XMFLOAT3& pos);
	virtual void	SetTransform(Transform& _transform);
	virtual void	SetToBeDeleted(bool d);
	virtual std::wstring Name() const { return m_sName; }


	void RefCountUp() { m_refCount++; }
	void RefCountDown()
	{
		m_refCount--;
		if (m_refCount < 0)
		{
			m_refCount = 0;
		}
	}
	int GeRefCount() const { return m_refCount; }
protected:
	//***************Data Members****************//
	std::wstring            m_sName;
	GUID                    m_tGUID;
	Transform			    m_transform;
	bool				    to_delete;

	int						m_refCount;
	//***************Allocator Overloads****************//
public:
	inline void * operator new (size_t size, const char *file, int line) {
		return FSMM::MemoryManager::GetInstance()->Allocate(static_cast<unsigned int>(size), 0U, file, line);
	}

	inline void * operator new (size_t size) {
		return FSMM::MemoryManager::GetInstance()->Allocate(static_cast<unsigned int>(size));
	}

	inline void * operator new[](size_t size, const char *file, int line) {
		return FSMM::MemoryManager::GetInstance()->Allocate(static_cast<unsigned int>(size), 0U, file, line);
	}

		inline void * operator new[](size_t size) {
		return FSMM::MemoryManager::GetInstance()->Allocate(static_cast<unsigned int>(size));
	}

		inline void operator delete (void * p) {
		FSMM::MemoryManager::GetInstance()->DeAllocate(p);
	}

	inline void operator delete (void * p, const char *file, int line) {
		FSMM::MemoryManager::GetInstance()->DeAllocate(p);
	}

	inline void operator delete[](void * p) {
		FSMM::MemoryManager::GetInstance()->DeAllocate(p);
	}

		inline void operator delete[](void * p, const char *file, int line) {
		FSMM::MemoryManager::GetInstance()->DeAllocate(p);
	}

};

