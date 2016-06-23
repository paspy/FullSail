#include "pch.h"
#include "CinematicCamera.h"
#include "Ship.h"


CinematicCamera::CinematicCamera() : Camera()
{
}


CinematicCamera::~CinematicCamera()
{
}


/*virtual*/ void CinematicCamera::Update(float dt)
{
	using namespace SimpleMath;
	if (m_Target)
	{
		
		Vector3 targetPos = m_Target->GetPosition();
		m_View->m_Position = targetPos + -Vector3{ -10.0f,m_Offset,m_Offset };
		m_View->m_LookAtDirection = normalizeFloat3(targetPos - m_View->m_Position);
		setPosition(m_transform.GetLocalMatrix(), m_View->m_Position);
		m_View->SetViewMatrix(XMMatrixLookToLH(XMLoadFloat3(&m_View->m_Position), XMLoadFloat3(&m_View->m_LookAtDirection), XMLoadFloat3(&m_View->m_Up)));
		return;
	}

	static float angle = 0.0f;
	angle += 5.0f*dt;
	if (angle >= 360.0f)
	{
		angle = 0.0f;
	}
	normalizeFloat3(m_View->m_LookAtDirection);


	Vector3 targetPos = Vector3(0.0f, 0.0f, 0.0f);
	m_View->m_Position = targetPos + -Vector3{ 0.0f,m_Offset,m_Offset };
	XMStoreFloat3(&m_View->m_Position, XMVector3Transform(XMLoadFloat3(&m_View->m_Position), XMMatrixRotationRollPitchYaw(0.0f, XMConvertToRadians(angle), 0.0f)));
	m_View->m_LookAtDirection = normalizeFloat3(targetPos - m_View->m_Position);

	setPosition(m_transform.GetLocalMatrix(), m_View->m_Position);
	m_View->SetViewMatrix(XMMatrixLookToLH(XMLoadFloat3(&m_View->m_Position), XMLoadFloat3(&m_View->m_LookAtDirection), XMLoadFloat3(&m_View->m_Up)));
}