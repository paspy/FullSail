/**************************************************************************************/
/*
Limits Ship Speed

Author : David Ashbourne(Full Sail)
File : Mast
Contributor(s) : Jason Bloomfield (Add name here if you like)
*/
/**************************************************************************************/
#include "pch.h"
#include "Mast.h"
#include <Particle.h>
#include "Game.h"
#include "GameplayScene.h"


Mast::Mast(Ship* owner, float MaxSpeed, const XMFLOAT4X4& _newTransfrom) : IShipPart(owner, _newTransfrom)
{
	m_fCurrentMaxSpeed = m_fMaxSpeed = MaxSpeed;
	m_fSlowdownMultiplier = 0.5f;
}

Mast & Mast::operator=(const Mast & other)
{
	//Self Check
	if (this == &other)
	{
		return (*this);
	}
	//Calling the base operator
	IShipPart::operator=(other);
	//Assigning the data
	m_fCurrentMaxSpeed = m_fMaxSpeed = other.m_fMaxSpeed;
	m_fSlowdownMultiplier = other.m_fSlowdownMultiplier;

	//Returning
	return (*this);
}

Mast::Mast(const Mast & _cpy)
	:IShipPart(_cpy)
{
	m_fCurrentMaxSpeed = m_fMaxSpeed = _cpy.m_fMaxSpeed;
	m_fSlowdownMultiplier = _cpy.m_fSlowdownMultiplier;
}

Mast::~Mast()
{

}

/*
void Mast::OnCollision(Collider& other)
{

}
*/

void Mast::Update(float _dt)
{
	// base update
	IShipPart::Update(_dt);


	// etc....

	if (m_fReducedSpeedTimer <= 0)
	{
		m_fCurrentMaxSpeed = m_fMaxSpeed;
		if (m_owner->GetComingBackFromRuinedSails())
		{
			m_owner->FlipSailsDown();
			m_owner->FlipComingBackFromRuinedSails();
		}
	}

	m_fReducedSpeedTimer -= _dt;



	// able to board
	if (m_fCurrentMaxSpeed <= BOARDING_SPEED)
	{
		// display boarding if in range if not player
		if (m_owner->GetAlignment() == Ship::eAlignment::ENEMY)
		{
			CGame*			game = (CGame*)CGame::GetApplication();
			SceneManager*	sm = game->GetSceneManager();
			GameplayScene*	gps = (GameplayScene*)sm->GetScene(IScene::SceneType::GAMEPLAY);

			float dist = m_owner->GetSqrDistance(gps->GetPlayerShip());
			if (dist <= BOARDING_DIST * BOARDING_DIST)
				gps->TriggerPortCollision();

			
		}
	}
}

bool Mast::LevelUp()
{
	// base level up
	if (IShipPart::LevelUp() == false)
	{
		return false;
	}


	// speed++ (speed += speed * (Lv/10.0f))
	m_fMaxSpeed += m_fMaxSpeed * float(m_nLevel * 0.05f);
	m_fCurrentMaxSpeed = m_fMaxSpeed;

	// slowdown++ (slowdown += slowdown * (Lv/10.0f))
	m_fSlowdownMultiplier += m_fSlowdownMultiplier * float(m_nLevel * 0.10f);


	// return
	return true;
}

const float& Mast::GetMaxSpeed()
{
	return m_fCurrentMaxSpeed;
}

const float& Mast::GetSlowdownMultiplier()
{
	return m_fSlowdownMultiplier;
}

void Mast::SetMaxSpeed(float _s)
{
	m_fMaxSpeed = m_fCurrentMaxSpeed = _s;
}

void Mast::SetSlowdownMultiplier(float _s)
{
	m_fSlowdownMultiplier = _s;
}

void Mast::ReduceSpeed()
{
	//Reducing the speed
	m_fCurrentMaxSpeed *= 0.7f;

	// able to board
	if (m_fCurrentMaxSpeed <= BOARDING_SPEED)
	{
		// boarding effects
		// show sail down icon
		if (false == m_owner->GetSailsDown())
		{
			m_owner->FlipSailsDown();
			m_owner->FlipComingBackFromRuinedSails();
		}
		//// display boarding if in range if not player
		//if (m_owner->GetAlignment() == Ship::eAlignment::ENEMY)
		//{
		//	CGame*			game = (CGame*)CGame::GetApplication();
		//	SceneManager*	sm = game->GetSceneManager();
		//	GameplayScene*	gps = (GameplayScene*)sm->GetScene(IScene::SceneType::GAMEPLAY);
		//
		//	float dist = m_owner->GetDistance(gps->GetPlayerShip());
		//	if (dist < BOARDING_DIST)
		//		gps->TriggerPortCollision();
		//
		//}
	}

	//Set timer
	m_fReducedSpeedTimer = m_fMaxTimer;
}

