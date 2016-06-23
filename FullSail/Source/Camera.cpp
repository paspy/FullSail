#include "pch.h"
#include "Camera.h"
#include "Ship.h"
#include "IController.h"
#include "Math.h"
#include "Game.h"
#include "GameplayScene.h"

const float Camera::m_fDefaultFOV = DirectX::XM_PIDIV4;
const float Camera::m_fDefaultAspect = 16.0f / 9.0f;
const float Camera::m_fDefaultNearPlane = 0.01f;
const float Camera::m_fDefaultFarPlane = 10000.0f;

using namespace DirectX;

Camera::Camera() :
	m_View(new Renderer::CView), m_Target(nullptr)
{
	m_View->m_fFOV = m_fDefaultFOV;
	auto resolution = CGame::GetApplication()->m_pRenderer->m_deviceResources->GetOutputSize();
	m_View->m_fAspect = (float)resolution.cx / (float)resolution.cy;
	m_View->m_fNearPlane = m_fDefaultNearPlane;
	m_View->m_fFarPlane = m_fDefaultFarPlane;
	UpdateProjMatrix();
	Reset();
}

Camera::Camera(float _fov, float _aspect, float _near, float _far) :
	m_View(new Renderer::CView), m_Target(nullptr)
{
	m_View->m_fFOV = _fov;
	m_View->m_fAspect = _aspect;
	m_View->m_fNearPlane = _near;
	m_View->m_fFarPlane = _far;
	UpdateProjMatrix();
	Reset();
}


Camera::~Camera()
{
	m_View.reset();
}


/*virtual*/ void Camera::Reset()
{
	m_View->m_Position = XMFLOAT3(0.0f, 3.0f, -10.0f);
	m_View->m_LookAtDirection = XMFLOAT3(-1.0f, -1.0f, 0.0f);
	normalizeFloat3(m_View->m_LookAtDirection);
	m_View->m_Up = XMFLOAT3(0.0f, 1.0f, 0.0f);
	m_View->m_Right = XMFLOAT3(0.0f, 0.0f, 1.0f);
	//m_Offset = -25.0f;
	m_View->SetViewMatrix(XMMatrixLookToLH(XMLoadFloat3(&m_View->m_Position), XMLoadFloat3(&m_View->m_LookAtDirection), XMLoadFloat3(&m_View->m_Up)));

}

/*virtual*/ void Camera::Update(float dt)
{
	using namespace SimpleMath;
	Vector3  targetPos;
	float interp = m_Offset;
	if (m_Target)
	{
		targetPos = XMFLOAT3(&(m_Target->GetTransform().GetWorldMatrix()._41));

		CGame*				game = (CGame*)CGame::GetApplication();
		GameplayScene*		gps = (GameplayScene*)game->GetSceneManager()->GetScene(IScene::SceneType::GAMEPLAY);
		if (gps->SpyglassActive()) {
			spyTime += dt * 2;
			if (spyTime > 1.0f)
				spyTime = 1.0f;

		}
		else {
			spyTime -= dt * 2;
			if (spyTime < 0.0f)
				spyTime = 0.0f;
		}
		float interpSpeed = *m_targetSpeed / *m_maxTargetSpeed;
		interp -= m_maxDiff * interpSpeed;
		float spyDiff = interp - SPYOFFSET;
		interp -= spyDiff * spyTime;
	}
	else
	{
		targetPos = XMFLOAT3(0.0f, 0.0f, 0.0f);
	}


	normalizeFloat3(m_View->m_LookAtDirection);
	m_View->m_Position = targetPos + m_View->m_LookAtDirection * (interp);

	setPosition(m_transform.GetLocalMatrix(), m_View->m_Position);
	m_View->SetViewMatrix(XMMatrixLookToLH(XMLoadFloat3(&m_View->m_Position), XMLoadFloat3(&m_View->m_LookAtDirection), XMLoadFloat3(&m_View->m_Up)));

}


/*virtual*/ void Camera::UpdateProjMatrix()
{
	using namespace SimpleMath;
	Matrix proj = XMMatrixPerspectiveFovLH(m_View->m_fFOV, m_View->m_fAspect, m_View->m_fNearPlane, m_View->m_fFarPlane);
	m_View->SetProjMatrix(proj);
}