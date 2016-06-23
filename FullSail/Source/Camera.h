#pragma once
#include "IEntity.h"
#include<View.h>

#define SPYOFFSET -42

class Ship;
/**************************************************************************************/
/*
Camera class

Author : David Ashbourne(Full Sail)
Contributor(s) : Junshu Chen (Add name here if you like)
*/
/**************************************************************************************/
class Camera :
	public IEntity
{
public:
	static const float m_fDefaultFOV;
	static const float m_fDefaultAspect;
	static const float m_fDefaultNearPlane;
	static const float m_fDefaultFarPlane;

	float m_Offset;
	float m_maxDiff;
	Ship* m_Target;
	const float* m_targetSpeed;
	const float* m_maxTargetSpeed;
	float spyTime = 0;
	
protected:
	//View contains View and Projection Matrices
	std::unique_ptr<Renderer::CView> m_View;

public:
	Camera();
	Camera(float _fov, float _aspect,float _near,float _far);
	/**Destructor***/
	~Camera();

	virtual void Reset();
	virtual void Update(float dt);
	virtual void UpdateProjMatrix();

	void SetOffset(float offset) { m_Offset = offset; }
	float GetOffset() { return m_Offset; }
	Renderer::CView* View() { return m_View.get(); }
	inline const DirectX::XMFLOAT3& Position() const { return m_View->m_Position; }
	inline const DirectX::XMFLOAT3& LookAtDirection() const { return m_View->m_LookAtDirection; }
	inline const DirectX::XMFLOAT3& Up() const { return m_View->m_Up; }
	inline const DirectX::XMFLOAT3& Right() const { return m_View->m_Right; }
};

