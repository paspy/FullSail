#include "pch.h"
#include "IAmmo.h"
#include "Ship.h"
#include "IController.h"
#include "IShipPart.h"
#include "Mast.h"
#include "Colliders.h"
#include "Game.h"
#include "GameStatistics.h"

#define AMMO_SPEED ((float)7.5f)	//((float)0.25f)	//((float)1.5f)


IAmmo::IAmmo(Ship* owner, eAmmoType _ammotype, const XMFLOAT4X4& _newTransfrom, std::string meshName, std::string textureName)
	: Entity3D(_newTransfrom, meshName.c_str(), textureName.c_str())
{
	m_pOwner = owner;
	m_nAmmoType = _ammotype;
	m_vVelocity = XMFLOAT3(0.0f, 0.0f, 0.0f);
	m_fLifespan = 0.0f;	// life
	m_nDamage = 0;	//damage;
	m_fSpeed = AMMO_SPEED;

	CollisionManager& GameCollision = *((CGame*)CGame::GetApplication())->GetSceneManager()->GetCollisionManager();

	TemplatedCollisionCallback<IAmmo>* thing = new TemplatedCollisionCallback<IAmmo>(this, &IAmmo::OnCollision);

	switch (m_nAmmoType)
	{
	case IAmmo::UNKNOWN:
		break;
	case IAmmo::CANNONBALL:
		SetCollider(new SphereCollider(GameCollision, GetLocalMatrix(), this, thing, 0.25f));
		break;
	case IAmmo::CHAIN_SHOT:
		SetCollider(new SphereCollider(GameCollision, GetLocalMatrix(), this, thing, 0.5f));
		break;
	case IAmmo::HARPOON:
		break;
	default:
		break;
	}

	// firing effect
	m_vecParticleSet.push_back(newx Renderer::CParticle(0, 15, Renderer::CParticleCollection::ParticleType::eTYPE_EXPLODE, true));
	m_vecParticleSet[0]->BlendMode(Renderer::CParticle::eBM_Additive);
	m_vecParticleSet[0]->ParticleLife(1.25f);
	m_vecParticleSet[0]->MaxTexCount(4.0f);
	m_vecParticleSet[0]->EmitColor({ 1.f, .31f, .12f, 1.f });
	m_vecParticleSet[0]->EmitSizeW({ 0.5f, 0.5f });
	m_vecParticleSet[0]->EmitDirectionW({ 0.0f, 9.8f, 0.0f });

	// splashes effect
	m_vecParticleSet.push_back(newx Renderer::CParticle(1, 10, Renderer::CParticleCollection::ParticleType::eTYPE_EXPLODE));
	m_vecParticleSet[1]->BlendMode(Renderer::CParticle::eBM_Additive);
	m_vecParticleSet[1]->ParticleLife(0.35f);
	m_vecParticleSet[1]->MaxTexCount(4.0f);
    //m_vecParticleSet[1]->EmitColor({ 0.f, 0.2f, 0.5f, 1.f });
    m_vecParticleSet[1]->EmitColor({ 1.f, 1.f, 1.f, 1.f });
	m_vecParticleSet[1]->EmitSizeW({ 0.25f, 0.25f });
	m_vecParticleSet[1]->EmitDirectionW({ 0.0f, 4.5f, 0.0f });

	// hit on island
	m_vecParticleSet.push_back(newx Renderer::CParticle(2, 10, Renderer::CParticleCollection::ParticleType::eTYPE_EXPLODE));
	m_vecParticleSet[2]->BlendMode(Renderer::CParticle::eBM_Additive);
	m_vecParticleSet[2]->ParticleLife(0.5f);
	m_vecParticleSet[2]->MaxTexCount(4.0f);
	m_vecParticleSet[2]->EmitColor({ 1.f, .31f, .12f, 1.f });
	m_vecParticleSet[2]->EmitSizeW({ 0.35f, 0.35f });
	m_vecParticleSet[2]->EmitDirectionW({ 0.0f, 4.9f, 0.0f });

    // smoke trail
    m_vecParticleSet.push_back( newx Renderer::CParticle( 3, 8, Renderer::CParticleCollection::ParticleType::eTYPE_SMOKE,true ) );
    m_vecParticleSet[3]->BlendMode( Renderer::CParticle::eBM_AlphaBlend );
    m_vecParticleSet[3]->ParticleLife( 0.4f );
    m_vecParticleSet[3]->EmitSizeW( { 0.5f, 0.5f } );
    m_vecParticleSet[3]->EmitColor( { 1.f, 1.f, 1.f, 1.f } );
    m_vecParticleSet[3]->WorldMat(DirectX::SimpleMath::Matrix::Identity );
    m_vecParticleSet[3]->EmitPositionW( positionOf( m_transform.GetWorldMatrix() ) );


	//Change this if you like
	//Wasn't going to wait a for you to respond to a message for a quick fix
	//It broke in update because I killed the owner as you were assigning the variable
	//If the owner is still alive set this variable
	/*if (m_pOwner->ToBeDeleted() == false)
	{

	}
	else
	{
		m_fSpeed = 0.0f;
	}*/

	//this->m_transform.SetParent(&m_pOwner->GetTransform());
	//this->m_transform.GetWorldMatrix();
	CGame*			game = (CGame*)CGame::GetApplication();
	SoundManager*	sound = game->GetSoundManager();
	sound->RegisterEntity(this, "Ball");
}


IAmmo & IAmmo::operator=(const IAmmo& other)
{
	//Self Check
	if (this == &other)
	{
		return (*this);
	}

	//Calling the base operator
	Entity3D::operator=(other);

	//Assigning the data
	m_pOwner = other.m_pOwner;
	m_nAmmoType = other.m_nAmmoType;
	m_vVelocity = other.m_vVelocity;
	m_fLifespan = other.m_fLifespan;
	m_nDamage = other.m_nDamage;

	//Returning
	return (*this);
}


IAmmo::IAmmo(const IAmmo & _cpy)
	:Entity3D(_cpy)
{
	m_pOwner = _cpy.m_pOwner;
	m_nAmmoType = _cpy.m_nAmmoType;
	m_vVelocity = _cpy.m_vVelocity;
	m_fLifespan = _cpy.m_fLifespan;
	m_nDamage = _cpy.m_nDamage;
}


IAmmo::~IAmmo()
{
	m_pOwner = nullptr;
	m_nAmmoType = eAmmoType::UNKNOWN;
	CGame*			game = (CGame*)CGame::GetApplication();
	SoundManager*	sound = game->GetSoundManager();
	sound->UnregisterEntity(this);
}


void IAmmo::OnCollision(Collider & _other)
{
	//Retrieving the Object from the void pointer
	Entity3D* CollidedObject = static_cast<Entity3D*>(_other.getID());
    auto&           gameStat = GameStatistics::GetRef();

	if (_other.isTrigger() && CollidedObject->GetType() == EntityType::SHIP)
		return;

	//Getting the type
	EntityType CollidedType = CollidedObject->GetType();
	/*
	if (CollidedType != EntityType::AMMO)
	{
		for ( auto i = 0; i < m_pParticleSet.size(); i++ )
		{
			m_pParticleSet[i]->Pause();
		}
		to_delete = true;
		delete m_pCollider;
		m_pCollider = nullptr;
	}
	*/


	// Ammo reset position
	XMMATRIX	mat = XMMatrixTranslation(0.0f, -10.0f, 0.0f);
	XMFLOAT4X4	f4x4 = XMFLOAT4X4();
	XMStoreFloat4x4(&f4x4, mat);




	switch (CollidedType)
	{
	case Entity3D::SHIP:
		if (m_pOwner != CollidedObject)
		{
            m_vecParticleSet[2]->EmitPositionW( positionOf( m_transform.GetWorldMatrix() ) );
            m_vecParticleSet[2]->EmitDirectionW( -forwardAxisOf( m_transform.GetWorldMatrix() ) );
            m_vecParticleSet[2]->Play();
            //m_vecParticleSet[3]->Stop();
			to_delete = true;
			delete m_pCollider;
			m_pCollider = nullptr;

            if ( m_pOwner->GetAlignment() == Ship::PLAYER && static_cast<Ship*>( CollidedObject )->GetAlignment() == Ship::ENEMY ) {
                if ( m_nAmmoType == CANNONBALL ) {
                    gameStat.RegularShotHit( 1 );
                } 
                if ( m_nAmmoType == CHAIN_SHOT ) {
                    gameStat.ChainShotHit( 1 );
                }
            }

			//return;
			//m_transform.SetLocalMatrix(f4x4);
			//m_transform.GetWorldMatrix();
			//EventManager()->SendEventInstant("ReStockAmmoPoolEvent", new CGeneralEventArgs<IAmmo*>(this));
		}
		break;

	case Entity3D::CRATE:
		break;

	case Entity3D::AMMO:
		break;

	case Entity3D::WEATHER:
		break;

	case Entity3D::LAND:
	{

        //m_vecParticleSet[3]->Stop();
        m_vecParticleSet[2]->Play();
		m_vecParticleSet[2]->EmitPositionW(positionOf(m_transform.GetWorldMatrix()));

		//Playing the Cannonball Land Collision sound
		SoundManager* soundManager = ((CGame*)CGame::GetApplication())->GetSoundManager();
		soundManager->Play3DSound(AK::EVENTS::PLAY_CANNONLANDCOLLISION);

		to_delete = true;
		delete m_pCollider;
		m_pCollider = nullptr;

		//m_transform.SetLocalMatrix(f4x4);
		//m_transform.GetWorldMatrix();
		//EventManager()->SendEventInstant("ReStockAmmoPoolEvent", new CGeneralEventArgs<IAmmo*>(this));
	}
	break;

	case Entity3D::UNKNOWN:
	case Entity3D::SHIP_PART:
	default:
		break;
	}

	//Dies regardless what they touch - Not really
	//SetParticleSetToPause();
	//to_delete = true;
	//delete m_pCollider;
	//m_pCollider = nullptr;

}


void IAmmo::Update(float dt)
{
    m_vecParticleSet[3]->EmitPositionW( positionOf( m_transform.GetWorldMatrix() ) );
    m_vecParticleSet[3]->EmitDirectionW( -forwardAxisOf( m_transform.GetWorldMatrix() ) );

	// dont update?
	if (to_delete == true)
	{
		return;
	}

	// rotate if chain shot
	if (CHAIN_SHOT == GetAmmoType())
	{
		XMFLOAT3 rotation = { 0, 25.0f * dt, 0 };
		m_transform.Rotate(rotation, true);
	}

	// base update
	Entity3D::Update(dt);


	// life--
	m_fLifespan -= dt;


	// dead ammo
	if (m_fLifespan <= 0.0f)
	{
		m_vecParticleSet[1]->Play();
		m_vecParticleSet[1]->EmitPositionW(positionOf(m_transform.GetWorldMatrix()));
		to_delete = true;
		delete m_pCollider;
		m_pCollider = nullptr;
		CGame*			game = (CGame*)CGame::GetApplication();
		SoundManager*	sound = game->GetSoundManager();
		sound->Play3DSound(AK::EVENTS::PLAYWATERSPLASH,GetPosition());

		return;
	}

	// Update position
	m_transform.Translate(m_vVelocity * dt);

    XMMATRIX ownerWorld = XMLoadFloat4x4( &m_pOwner->GetWorldMatrix() );
    ownerWorld = XMMatrixTranslationFromVector( XMLoadFloat3( &Offset() ) ) * ownerWorld;
    m_vecParticleSet[0]->WorldMat( ownerWorld );
}


/*
const Ship::eAlignment IAmmo::GetShipAlignment()
{
	if (m_pOwner == nullptr)
		assert("IAmmo::GetAlignment - ammo does not have an owner!");

	return m_pOwner->GetAlignment();
}
*/


const int IAmmo::GetDamage()
{
	return m_nDamage;
}


void IAmmo::SetOwner(Ship * owner)
{
	m_pOwner = owner;
}

