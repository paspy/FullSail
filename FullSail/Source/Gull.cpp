#include "pch.h"
#include "Gull.h"
#include "Game.h"

Gull::Gull(XMFLOAT4X4& _transform) : Entity3D(_transform)
{
	m_pCollider = nullptr;
	XMFLOAT4 color = { 1,1,1,1 };
	bool wire = false;
	CGame::GetApplication()->m_pRenderer->AddDebugSphere(this, GetWorldMatrix(), color, wire);
	maxSpeed = 50;
	maxTurn = 1;
	cohesion = .1f;
	separation = -1;
	alignment = 0;
	scohesion = 1.0f;
	sseparation = -.5f;
	salignment = 0;
}


Gull::~Gull()
{
	CGame::GetApplication()->m_pRenderer->RemoveDebugShape(this);
}

void Gull::Update(float _dt, std::vector<Gull*>& boids, std::vector<Ship*>& ships)
{
	XMFLOAT3 centroid = VectorZero;
	XMFLOAT3 scentroid = VectorZero;
	XMFLOAT3 averageRight = VectorZero;
	size_t bcounter = 0;
	size_t scounter = 0;
	for (auto&& boid : boids)
	{
		if (sqrDistanceBetween(GetPosition(), boid->GetPosition()) < 100 && boid != this)
		{
			centroid += boid->GetPosition();
			averageRight += boid->GetRight();
			bcounter++;
		}
	}
	for (auto&& ship : ships)
	{
		if (sqrDistanceBetween(GetPosition(), ship->GetPosition()) < 100)
		{
			scentroid += ship->GetPosition();
			scounter++;
		}
	}
	centroid /= (float)bcounter;
	averageRight /= (float)bcounter;
	scentroid /= (float)scounter;

	XMFLOAT3 desiredPos = centroid - GetPosition();
	XMFLOAT3 sDPos = scentroid - GetPosition();
	/*desiredPos = desiredPos * cohesion + desiredPos * separation;
	sDPos = scentroid * scohesion + scentroid * sseparation;*/
	Turn(_dt, ((dotProduct(averageRight, GetForward())* alignment) + dotProduct(desiredPos, GetRight()) + dotProduct(sDPos, GetRight())));
	FlyForward(_dt, maxSpeed);
}

void Gull::FlyForward(float _dt, float _speed)
{
	m_transform.Translate(forwardAxisOf(m_transform.GetWorldMatrix())* clamp(_speed, maxSpeed * .1f, maxSpeed) * _dt);
}

void Gull::Turn(float _dt, float _speed)
{
	m_transform.Rotate(VectorY * clamp(_speed, -maxTurn, maxTurn) * _dt, true);
}
