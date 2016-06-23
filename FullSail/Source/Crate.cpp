#include "pch.h"
#include "Crate.h"
#include "Colliders.h"
#include "Ship.h"
#include "Game.h"
#include "GameplayScene.h"
#include "GameStatistics.h"

#define CRATE_LIFE				((float)15.0f)
#define CRATE_DEATH				((float)8.0f)
#define CRATE_SPEED_L			((float)0.4f)
#define CRATE_SPEED_D			((float)CRATE_SPEED_L * 1.2f)
#define CRATE_VFX_TIME			((float)0.9f)

Crate::Crate(XMFLOAT4X4& _newTransfrom, const char* MeshName, const char* TextureName) : Entity3D(_newTransfrom,MeshName,TextureName)
{
	VFXCountDown	= CRATE_VFX_TIME;

	m_nRum			= 0;
	m_nWaterFood	= 0;
	m_nGold			= 0;
	m_direction		= XMFLOAT3(0.0f, 0.0f, 0.0f);
	m_fLifespan		= CRATE_LIFE;
	m_fSpeed		= CRATE_SPEED_L;
	m_fDeathTimer	= 0.0f;
	m_bHitVFX		= false;

    m_vecParticleSet.push_back( newx Renderer::CParticle( 0, 25, Renderer::CParticleCollection::ParticleType::eTYPE_EXPLODE ) );
    m_vecParticleSet[0]->BlendMode( Renderer::CParticle::eBM_Additive );
    m_vecParticleSet[0]->ParticleLife( 0.75f );
    m_vecParticleSet[0]->MaxTexCount( 1.0f );
    m_vecParticleSet[0]->EmitColor( { 1.f, 1.f, 1.f, 1.f } );
    m_vecParticleSet[0]->EmitSizeW( { 1.0f, 1.0f } );
    m_vecParticleSet[0]->EmitDirectionW( { 0.0f, 7.5f, 0.0f } );
	
	m_pPlayerShip = nullptr;
}


Crate::Crate(const Crate& _cpy) : Entity3D(_cpy)
{
	// setup resource data
	m_nRum			= _cpy.m_nRum;
	m_nWaterFood	= _cpy.m_nWaterFood;
	m_nGold			= _cpy.m_nGold;
	m_direction		= _cpy.m_direction;
	m_fLifespan		= _cpy.m_fLifespan;
	m_fSpeed		= _cpy.m_fSpeed;
	m_fDeathTimer	= _cpy.m_fDeathTimer;
	m_bHitVFX		= _cpy.m_bHitVFX;
	m_pPlayerShip	= _cpy.m_pPlayerShip;

	m_vecParticleSet.push_back(newx Renderer::CParticle(0, 500, Renderer::CParticleCollection::ParticleType::eTYPE_EXPLODE));
	m_vecParticleSet[0]->BlendMode(Renderer::CParticle::eBM_Additive);
	m_vecParticleSet[0]->ParticleLife(0.75f);
	m_vecParticleSet[0]->MaxTexCount(1.0f);
	m_vecParticleSet[0]->EmitColor({ 1.f, 1.f, 1.f, 1.f });
	m_vecParticleSet[0]->EmitSizeW({ 1.0f, 1.0f });
	m_vecParticleSet[0]->EmitDirectionW({ 0.0f, 7.5f, 0.0f });

}


Crate::~Crate()
{
    
}


Crate& Crate::operator=(const Crate& other)
{
	// insert return statement here
	if (this == &other)
		return (*this);


	//Calling the base operator
	Entity3D::operator=(other);


	// setup resource data
	m_nRum			= other.m_nRum;
	m_nWaterFood	= other.m_nWaterFood;
	m_nGold			= other.m_nGold;
	m_direction		= other.m_direction;
	m_fLifespan		= other.m_fLifespan;
	m_fSpeed		= other.m_fSpeed;
	m_fDeathTimer	= other.m_fDeathTimer;
	m_bHitVFX		= other.m_bHitVFX;
	m_pPlayerShip	= other.m_pPlayerShip;

	// assigning the data
	return (*this);
}


void Crate::Update(float dt)
{
	// dont update?
    if ( to_delete == true ) {
		return;
    }

	// base update
	Entity3D::Update(dt);

	VFXCountDown -= dt;
	if (VFXCountDown < 0.0f)
	{
		// reset timer
		VFXCountDown = CRATE_VFX_TIME;

		// toggle hit effect
		m_bHitVFX = !m_bHitVFX;
		m_pRenderInfo->HitEffect(m_bHitVFX);
	}


	// start crate death
	if (m_fLifespan <= 0.0f && m_fDeathTimer == 0.0f)
	{
		m_fDeathTimer	= CRATE_DEATH;
		m_direction.y	= -1;
		m_fSpeed		= 0.0f;
	}

	// dying crate
	if (m_fLifespan <= 0.0f)
	{
		DeathUpdate(dt);
		return;
	}


	// life--
	m_fLifespan -= dt;


	// move ammo
	//m_transform.Translate(m_direction * dt * m_fSpeed, true);
}


void Crate::OnCollision(Collider& other)
{
	if (to_delete == true || other.isTrigger())
		return;

	//SoundManager*	sound	= ((CGame*)CGame::GetApplication())->GetSoundManager();
	Entity3D*		object	= static_cast<Entity3D*>(other.getID());
	EntityType		type	= object->GetType();
    auto&           gameStat = GameStatistics::GetRef();
	//this might need to be changed
	switch (type)
	{
	case Entity3D::SHIP:
		m_pPlayerShip = ((Ship*)object);
		if (m_pPlayerShip->GetAlignment() == Ship::eAlignment::PLAYER)
		{
			//SetParticleSetToPause();
			m_vecParticleSet[0]->Stop();

			CGame *pGame = (CGame*)CGame::GetApplication();
			GameplayScene* pGameplayScene = (GameplayScene*)pGame->GetSceneManager()->GetScene(IScene::SceneType::GAMEPLAY);
			pGameplayScene->AddHudResources(m_nGold,m_nRum,m_nWaterFood);

			GetRenderInfo()->SetVisible(false);
			delete m_pCollider;
			m_pCollider = nullptr;

            // record at stat
            gameStat.CrateCollected( 1 );
            gameStat.GoldCollected( m_nGold );
            gameStat.FoodCollected( m_nWaterFood );
            gameStat.RumCollected( m_nRum );

		}
		else
			m_pPlayerShip = nullptr;
		break;

	case Entity3D::LAND:
		m_fSpeed = 0.0f;
		break;

	case Entity3D::UNKNOWN:
	case Entity3D::SHIP_PART:
	case Entity3D::CRATE:
	case Entity3D::AMMO:
	case Entity3D::WEATHER:
	default:
		break;
	}

}


void Crate::DeathUpdate(float dt)
{
	// death timer
	m_fDeathTimer -= dt;

    m_vecParticleSet[0]->EmitPositionW( positionOf( m_transform.GetWorldMatrix() ) );
    m_vecParticleSet[0]->Play();

	if (m_fDeathTimer < 0.0f)
	{
        m_vecParticleSet[0]->Stop();
        to_delete = true;
		return;
	}

	// sink
	m_transform.Translate(m_direction * dt * CRATE_SPEED_D, true);
}


