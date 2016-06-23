#include "pch.h"
#include "Ship.h"
#include "PlayerController.h"
#include "AiController.h"
#include "SideCannonHandler.h"
#include "Game.h"
#include "EntityLoader.h"
#include "CollisionManager.h"
#include <C3DHUD.h>
#include "CSteeringManger.h"



//*****************************New Functions*****************************//

Ship::Ship(const XMFLOAT4X4 & _newTransfrom, std::string meshName, std::string textureName) : Entity3D(_newTransfrom, meshName.c_str(), textureName.c_str()), m_fSlowModeTimer(0.0f), m_PopTextTimer(0.0f)
{
	m_Alignment = eAlignment::UNKNOWN;
	m_ShipType = eShipType::TYPE_UNKNOWN;


	// Put wake on the ship
	m_vecParticleSet.push_back(newx Renderer::CParticle(0, 500, Renderer::CParticleCollection::ParticleType::eTYPE_WAKE));
	m_vecParticleSet[0]->BlendMode(Renderer::CParticle::eBM_Additive );
	m_vecParticleSet[0]->ParticleLife(1.0f);
	m_vecParticleSet[0]->EmitDirectionW({ 0.0f, 0.5f, 0.0f });
	m_vecParticleSet[0]->EmitColor({ .2f, .35f, .35f, 1.0f });

	m_vecParticleSet.push_back(newx Renderer::CParticle(0, 500, Renderer::CParticleCollection::ParticleType::eTYPE_FIRE));
	m_vecParticleSet[1]->BlendMode(Renderer::CParticle::eBM_Additive);
	m_vecParticleSet[1]->ParticleLife(0.75f);
	m_vecParticleSet[1]->EmitSizeW({ 2,2 });
	m_vecParticleSet[1]->EmitDirectionW({ 0.0f, 2.5f, 0.0f });
	m_vecParticleSet[1]->EmitColor({ 1.f, 1.f, 1.f, 1.f });

	m_vecParticleSet.push_back(newx Renderer::CParticle(0, 250, Renderer::CParticleCollection::ParticleType::eTYPE_SMOKE));
	m_vecParticleSet[2]->BlendMode(Renderer::CParticle::eBM_AlphaBlend);
	m_vecParticleSet[2]->ParticleLife(0.75f);
	m_vecParticleSet[2]->EmitSizeW({ 0.5f, 0.5f });
	m_vecParticleSet[2]->EmitDirectionW({ 0.0f, 0.0f, 0.0f });
	m_vecParticleSet[2]->EmitColor({ 1.f, 1.f, 1.f, 1.f });

    m_vecParticleSet.push_back( newx Renderer::CParticle( 0, 500, Renderer::CParticleCollection::ParticleType::eTYPE_WAKE ) );
    m_vecParticleSet[3]->BlendMode( Renderer::CParticle::eBM_Additive );
    m_vecParticleSet[3]->ParticleLife( 1.0f );
    m_vecParticleSet[3]->EmitDirectionW( { 0.0f, 0.5f, 0.0f } );
    m_vecParticleSet[3]->EmitColor( { .2f, .35f, .35f, 1.0f } );


	m_cWeapon		= nullptr;
	m_nHullLevel	= 0;
	m_nMastLevel	= 0;
	m_pController	= nullptr;


	//Movement
	m_fSpeed		= 0;
	m_fAcceleration	= 0.1f;
	m_fMinSpeed		= 0;


	// setup reputation data
	m_fCurrReputation	= 0.0f;
	m_fMaxReputation	= 100.0f;


	// setup resource data
	m_nCurrRum			= 0;
	m_nCurrWaterFood	= 0;
	m_nCurrGold			= 0;
	m_nCurrCrew			= 0;
	m_nMaxRum			= 100;
	m_nMaxWaterFood		= 100;
	m_nMaxGold			= 9999999;	//314926;
	m_nMaxCrew			= 10;


	// setup boarding data
	m_BoardingState	= eBoardingState::NONE;
	m_bCurrBoarding	= false;


	// setup death (not)animation data
	m_DeathStage	= eDeathStages::ALIVE;
	m_fDeathTimer	= DEATH_TIME;


	// setup sailsdown data
	m_comingBackFromRuinedSails	= false;
	m_sailsDown					= false;


	// setup pop text data
	m_PopText = L"Ahoy!";
	m_PopTextHUD = std::make_unique<Renderer::C3DHUD>(&m_PopText);
	m_PopTextHUD->SetScale(0.0f);
}

//****************Trilogy*****************//

Ship& Ship::operator=(const Ship & other)
{
	// return statement
	if (this == &other)
		return (*this);


	//Calling the base operator
	Entity3D::operator=(other);


	m_Alignment = other.m_Alignment;
	m_ShipType = other.m_ShipType;


	m_fSpeed		= other.m_fSpeed;
	m_fAcceleration	= other.m_fAcceleration;
	m_fMinSpeed		= other.m_fMinSpeed;


	// setup reputation data
	m_fCurrReputation	= other.m_fCurrReputation;
	m_fMaxReputation	= other.m_fMaxReputation;


	// setup resource data
	m_nCurrRum			= other.m_nCurrRum;
	m_nCurrWaterFood	= other.m_nCurrWaterFood;
	m_nCurrGold			= other.m_nCurrGold;
	m_nCurrCrew			= other.m_nCurrCrew;
	m_nMaxRum			= other.m_nMaxRum;
	m_nMaxWaterFood		= other.m_nMaxWaterFood;
	m_nMaxGold			= other.m_nMaxGold;
	m_nMaxCrew			= other.m_nMaxCrew;

	m_fMaxRoT				= other.m_fMaxRoT;
	m_nMastLevel			= other.m_nMastLevel;
	m_fCurrentMaxSpeed		= other.m_fCurrentMaxSpeed;
	m_fMaxSpeed				= other.m_fMaxSpeed;
	m_fSlowdownMultiplier	= other.m_fSlowdownMultiplier;
	m_nHullLevel			= other.m_nHullLevel;
	m_nHealth				= other.m_nHealth;
	m_nMaxHealth			= other.m_nMaxHealth;
	m_fMaxTimer				= 10;
	m_fReducedSpeedTimer	= 0;


	//Creating controller for myself

	// setup boarding data
	m_BoardingState			= other.m_BoardingState;
	m_bCurrBoarding			= other.m_bCurrBoarding;


	// setup death (not)animation data
	m_DeathStage	= other.m_DeathStage;
	m_fDeathTimer	= other.m_fDeathTimer;


	// setup sailsdown data
	m_comingBackFromRuinedSails	= other.m_comingBackFromRuinedSails;
	m_sailsDown					= other.m_sailsDown;


	//Assigning the data
	return (*this);
}


Ship::Ship(const Ship & other) : Entity3D(other), m_fSlowModeTimer(0.0f)
{
	m_Alignment = other.m_Alignment;
	m_ShipType = other.m_ShipType;


	m_fSpeed		= other.m_fSpeed;
	m_fAcceleration	= other.m_fAcceleration;
	m_fMinSpeed		= other.m_fMinSpeed;

	m_cWeapon = new SideCannonHandler((*other.m_cWeapon));
	Ship* mySelf = this;
	m_cWeapon->SetOwner(mySelf);
	m_cWeapon->SetLocalMatrix(m_transform.GetLocalMatrix());


	// setup reputation data
	m_fCurrReputation	= other.m_fCurrReputation;
	m_fMaxReputation	= other.m_fMaxReputation;


	// setup resource data
	m_nCurrRum			= other.m_nCurrRum;
	m_nCurrWaterFood	= other.m_nCurrWaterFood;
	m_nCurrGold			= other.m_nCurrGold;
	m_nCurrCrew			= other.m_nCurrCrew;
	m_nMaxRum			= other.m_nMaxRum;
	m_nMaxWaterFood		= other.m_nMaxWaterFood;
	m_nMaxGold			= other.m_nMaxGold;
	m_nMaxCrew			= other.m_nMaxCrew;

	m_fMaxRoT				= other.m_fMaxRoT;
	m_nMastLevel			= other.m_nMastLevel;
	m_fCurrentMaxSpeed		= other.m_fCurrentMaxSpeed;
	m_fMaxSpeed				= other.m_fMaxSpeed;
	m_fSlowdownMultiplier	= other.m_fSlowdownMultiplier;
	m_nHullLevel			= other.m_nHullLevel;
	m_nHealth				= other.m_nHealth;
	m_nMaxHealth			= other.m_nMaxHealth;
	m_fMaxTimer				= other.m_fMaxTimer;
	m_fReducedSpeedTimer	= other.m_fReducedSpeedTimer;


	//Creating controller for myself
	m_stControllerName = other.m_stControllerName;

	//if the iController is 
	if (other.m_stControllerName == "PlayerController")
	{
		m_pController = new PlayerController(this);
	}
	else
	{
		Ship* myself = this;
		m_pController = new AiController(this);
		EntityLoader::ReadInControllerInfo(other.m_stControllerName.c_str(), &myself, ((AiController**)&m_pController));
		//stuff with not work without this
		m_pController->SetOwner(this);
	}


	// setup boarding data
	m_BoardingState	= other.m_BoardingState;
	m_bCurrBoarding	= other.m_bCurrBoarding;


	// setup death (not)animation data
	m_DeathStage	= other.m_DeathStage;
	m_fDeathTimer	= other.m_fDeathTimer;


	// setup sailsdown data
	m_comingBackFromRuinedSails	= other.m_comingBackFromRuinedSails;
	m_sailsDown					= other.m_sailsDown;


	// Put wake on the ship
	m_vecParticleSet.push_back(newx Renderer::CParticle(0, 500, Renderer::CParticleCollection::ParticleType::eTYPE_WAKE));
	m_vecParticleSet[0]->BlendMode(Renderer::CParticle::eBM_Additive );
	m_vecParticleSet[0]->ParticleLife( 1.0f );
	m_vecParticleSet[0]->EmitDirectionW({ 0.0f, 0.5f, 0.0f });
	m_vecParticleSet[0]->EmitColor({ .2f, .35f, .35f, 1.0f });
    //m_vecParticleSet[0]->EmitColor( { 0.5f, 0.5f, 0.5f, 0.5f } );

	m_vecParticleSet.push_back(newx Renderer::CParticle(0, 500, Renderer::CParticleCollection::ParticleType::eTYPE_FIRE));
	m_vecParticleSet[1]->BlendMode(Renderer::CParticle::eBM_Additive);
	m_vecParticleSet[1]->ParticleLife(0.75f);
	m_vecParticleSet[1]->EmitSizeW({ 2,2 });
	m_vecParticleSet[1]->EmitDirectionW({ 0.0f, 2.5f, 0.0f });
	m_vecParticleSet[1]->EmitColor({ 1.f, 1.f, 1.f, 1.f });

	m_vecParticleSet.push_back(newx Renderer::CParticle(0, 250, Renderer::CParticleCollection::ParticleType::eTYPE_SMOKE));
	m_vecParticleSet[2]->BlendMode(Renderer::CParticle::eBM_AlphaBlend);
	m_vecParticleSet[2]->ParticleLife(0.75f);
	m_vecParticleSet[2]->EmitSizeW({ 0.5f, 0.5f });
	m_vecParticleSet[2]->EmitDirectionW({ 0.0f, 0.0f, 0.0f });
	m_vecParticleSet[2]->EmitColor({ 1.f, 1.f, 1.f, 1.f });

    m_vecParticleSet.push_back( newx Renderer::CParticle( 0, 500, Renderer::CParticleCollection::ParticleType::eTYPE_WAKE ) );
    m_vecParticleSet[3]->BlendMode( Renderer::CParticle::eBM_Additive );
    m_vecParticleSet[3]->ParticleLife( 1.0f );
    m_vecParticleSet[3]->EmitDirectionW( { 0.0f, 0.5f, 0.0f } );
    m_vecParticleSet[3]->EmitColor( { .2f, .35f, .35f, 1.0f } );
    //m_vecParticleSet[3]->EmitColor( { 0.5f, 0.5f, 0.5f, 0.5f } );

	// setup pop text data
	m_PopText = L"Ahoy!";
	m_PopTextHUD = std::make_unique<Renderer::C3DHUD>(&m_PopText);
	CGame::GetApplication()->m_pRenderer->Add3DHUD(m_PopTextHUD.get());
	m_PopTextHUD->SetScale(0.0f);
	m_PopTextTimer = 0.0f;
}


Ship::~Ship()
{
	CGame::GetApplication()->m_pRenderer->Remove3DHUD(m_PopTextHUD.get());

	if (m_pController != nullptr)
	{
		delete m_pController;
		m_pController = nullptr;
	}

	if (m_cWeapon != nullptr)
	{
		delete m_cWeapon;
		m_cWeapon = nullptr;
	}

}



//**************Functions****************//

void Ship::Update(float dt)
{
	Entity3D::Update(dt);

	memcpy(&m_renderMatrix, &(XMMatrixRotationAxis(XMLoadFloat3(&VectorZ), XMConvertToRadians(roll)) * m_transform.GetWorldMatrix()), sizeof(m_renderMatrix));
	roll -= roll * dt;

}


void Ship::OnCollision(Collider & other)
{
	// do nothing!
}


void Ship::DeathUpdate(float dt)
{
	m_fDeathTimer -= dt;


	CGame* game = static_cast<CGame*>(CGame::GetApplication());
	if (m_fSlowdownMultiplier > 0.0f)
	{
		m_fSlowdownMultiplier -= dt / game->TimeScale;
	}
	else if (m_fSlowdownMultiplier < 0.0f)
	{
		m_fSlowdownMultiplier = 0.0f;
		game->TimeScale = 1.0f;
		SoundManager* soundManager = game->GetSoundManager();
		soundManager->audioSystem.SetRTCPValue(AK::GAME_PARAMETERS::PLAYBACKSPEED, 0);
	}

	float speed = dt * 0.5f;

    m_vecParticleSet[0]->Stop();
    m_vecParticleSet[3]->Stop();
	if (m_vecParticleSet[1]->EmitSizeW().x <= 0.1f)
		m_vecParticleSet[1]->Stop();
	else
		m_vecParticleSet[1]->EmitSizeW({ m_vecParticleSet[1]->EmitSizeW().x - dt*0.5f, m_vecParticleSet[1]->EmitSizeW().y - dt*0.5f });


	DeathSound();
	switch (m_DeathStage)
	{
		// rotate
	case eDeathStages::DEATH_ROTATE:
		m_transform.Rotate(XMFLOAT3(-1.0f, 0.0f, 0.0f) * speed, true);
		if (m_fDeathTimer < 0.0f)
		{
			m_DeathStage = eDeathStages::DEATH_SINK;
			m_fDeathTimer = DEATH_TIME;
		}
		break;


		// sink
	case eDeathStages::DEATH_SINK:
		m_transform.Translate(XMFLOAT3(0.0f, -1.0f, 0.0f) * speed, false);

		if (GetPosition().y < -0.7f)
			game->GetSceneManager()->GetCollisionManager()->stopTracking(m_pCollider);

		if (m_fDeathTimer < 0.0f)
		{
			m_DeathStage = eDeathStages::DEATH_DONE;
			m_fDeathTimer = DEATH_TIME;
		}
		break;


		// done!
	case eDeathStages::DEATH_DONE:
		to_delete = true;
		break;


	case eDeathStages::ALIVE:
	default:
		break;
	}
}


bool Ship::LevelUp(IShipPart::PartType partType)
{
	switch (partType)
	{
	case IShipPart::PartType::HULL:
		if (HullLevelUp() == true && Repair(IShipPart::PartType::HULL) == true)
			return true;
		return false;

	case IShipPart::PartType::MAST:
		return MastLevelUp();

	case IShipPart::PartType::WEAPON:
		return m_cWeapon->LevelUp();

	case IShipPart::PartType::UNKNOWN:
	default:
		break;
	}

	return false;
}


bool Ship::Repair(IShipPart::PartType partType)
{
	// repair ship parts
	switch (partType)
	{
	case IShipPart::PartType::HULL:
		return Repair();

	case IShipPart::PartType::MAST:
		return RepairSails();

	case IShipPart::PartType::WEAPON:
		return m_cWeapon->Repair();

	case IShipPart::PartType::UNKNOWN:
	default:
		break;
	}


	return false;
	//m_cHull->Repair();
	//m_cSail->Repair();
	//m_cWeapon->Repair();
	//for (size_t i = 0; i < 3; i++)
	//	m_pShipParts[i]->Repair();
}


void Ship::StopColliders(void)
{
	//CollisionManager& GameCollision = *((CGame*)CGame::GetApplication())->GetSceneManager()->GetCollisionManager();
	CollisionManager& GameCollision = *(static_cast<CGame*>(CGame::GetApplication())->GetSceneManager()->GetCollisionManager());

	GameCollision.stopTracking(m_pCollider);

	m_pController->StopColliders();

	CSteeringManger::SteeringManger().RemoveBoid(m_pController->GetBoid());
}


void Ship::StartColliders(void)
{
	//CollisionManager& GameCollision = *((CGame*)CGame::GetApplication())->GetSceneManager()->GetCollisionManager();
	CollisionManager& GameCollision = *(static_cast<CGame*>(CGame::GetApplication())->GetSceneManager()->GetCollisionManager());

	GameCollision.trackThis(m_pCollider);

	m_pController->StartColliders();
	
	CSteeringManger::SteeringManger().AddBoid(m_pController->GetBoid());
}



//****************************************//
/*Movement*/
//****************************************//

void Ship::FlipSailsDown()
{
	m_sailsDown = !m_sailsDown;
}


void Ship::TurnDegrees(float _degrees, float _dt)
{
	XMFLOAT3 rotation = VectorZero;
	rotation.y = XMConvertToRadians(_degrees);
	clampValue(rotation.y, -m_fMaxRoT, m_fMaxRoT);
	rotation.y *= _dt;
	m_transform.Rotate(rotation, true);
	roll += ((_degrees > 0 ? 15 : -15 - roll) * (m_fSpeed / m_fMaxSpeed)) * _dt;
}


void Ship::TurnRadians(float _radians, float _dt)
{
	XMFLOAT3 rotation = VectorZero;
	rotation.y = _radians;
	clampValue(rotation.y, -m_fMaxRoT, m_fMaxRoT);
	rotation.y *= _dt;
	m_transform.Rotate(rotation, true);
	roll += ((_radians > 0 ? 15 : -15 - roll) * (m_fSpeed / m_fMaxSpeed)) * _dt;
}


void Ship::TurnTowardPoint(XMFLOAT3 & _point, float _dt)
{
}


void Ship::TurnTowardVector(XMFLOAT3 & _vector, float _dt)
{
	XMFLOAT3 myForward;
	forwardAxisOf(m_transform.GetWorldMatrix(), myForward);
	float angle = atan2(_vector.y, _vector.x) - atan2(myForward.y, myForward.x);
}


void Ship::Accelerate(const float& dt)
{
	float distanceToMaxSpeed = m_fCurrentMaxSpeed - m_fSpeed;
	AdjustSpeed(dt * m_fAcceleration * distanceToMaxSpeed);
}


void Ship::Decelerate(const float& dt)
{
	float distanceToMinSpeed = m_fMinSpeed - m_fSpeed;
	//Applying a negative amount to slow down the the owner
	AdjustSpeed(distanceToMinSpeed * m_fSlowdownMultiplier * dt);
}



//****************************************//
/*Attack*/
//****************************************//

void Ship::SideCannonFire()
{
	m_cWeapon->Fire(Cannon::WeaponType::CANNON, 2);
}


void Ship::LongNineFire()
{
	m_cWeapon->CycleAmmoUp();
	m_cWeapon->Fire(Cannon::WeaponType::LONG_NINE, 2);
	m_cWeapon->CycleAmmoDown();
}



//****************************************//
/*Boarding*/
//****************************************//

void Ship::UpdateBoarding(float dt)
{
	// do nothing!
}


float Ship::GetSqrDistance(Ship* target)
{
	XMFLOAT3 pos1, pos2;
	positionOf(m_transform.GetWorldMatrix(), pos1);
	positionOf(target->m_transform.GetWorldMatrix(), pos2);
	return sqrDistanceBetween(pos1, pos2);
}



//****************************************//
/*Other*/
//****************************************//

void Ship::TakeDamage(const int dmg)
{
	// hp check
	if (m_nHealth <= 0)
		return;


	// damage/defence calc
	int IncomingDamage = (dmg - m_nDefense);

	// no damage?
	if (IncomingDamage < 0)
		return;

	// hp--
	m_nHealth -= IncomingDamage;

	// dead?
	if (m_nHealth <= 0)
	{
		m_nHealth = 0;

		// owner is sunk
		KillOwner();
	}
}


void Ship::KillOwner(void)
{
	// already dying/dead
	if (m_DeathStage > eDeathStages::ALIVE)
		return;

	// start death sequence
	m_DeathStage = eDeathStages::DEATH_ROTATE;

	// audio thing
	CGame*			game	= static_cast<CGame*>(CGame::GetApplication());
	SoundManager*	sound	= game->GetSoundManager();
	/*game->SetGlobalTimeScale(0.5f);
	m_fSlowModeTimer = 0.3f;*/
	sound->audioSystem.SetRTCPValue(AK::GAME_PARAMETERS::PLAYBACKSPEED, 100);
}



void Ship::UpdateParticles() {
    XMMATRIX ownerWorld = XMMatrixTranslationFromVector( XMLoadFloat3( &XMFLOAT3( 0, 0.5f, 0 ) ) ) * XMLoadFloat4x4( &m_transform.GetWorldMatrix() );

    //////////////////////////////////////////////////////////////////////////
    // update particle
    float speed = m_fSpeed;
    if ( speed <= 0.1f ) {
        m_vecParticleSet[0]->Stop();
        m_vecParticleSet[0]->Reset();
        m_vecParticleSet[3]->Stop();
        m_vecParticleSet[3]->Reset();
    } else {
        m_vecParticleSet[0]->Play();
        m_vecParticleSet[3]->Play();
    }
    m_vecParticleSet[0]->EmitDirectionW( -this->GetForward() );
    //m_vecParticleSet[0]->EmitPositionW( DirectX::SimpleMath::Vector3( GetPosition().x, GetPosition().y, GetPosition().z) );
    XMMATRIX playerWorld = XMMatrixTranslationFromVector( XMLoadFloat3( &XMFLOAT3( 0.25f, 0.0f, 0.5f ) ) ) * XMLoadFloat4x4( &m_transform.GetWorldMatrix() );
    m_vecParticleSet[0]->EmitSizeW( { 0.4f, 0.4f } );
    m_vecParticleSet[0]->WorldMat( playerWorld );
    //m_vecParticleSet[0]->EmitVelocity( -this->GetForward() * 0 );
    m_vecParticleSet[0]->EmitVelocity( this->GetRight() *speed *0.025f );
    m_vecParticleSet[0]->EmitSpeed( speed );

    m_vecParticleSet[3]->EmitDirectionW( -this->GetForward() );
    playerWorld = XMMatrixTranslationFromVector( XMLoadFloat3( &XMFLOAT3( -0.25f, 0.0f, 0.5f ) ) ) * XMLoadFloat4x4( &m_transform.GetWorldMatrix() );
    m_vecParticleSet[3]->EmitSizeW( { 0.4f, 0.4f } );
    m_vecParticleSet[3]->WorldMat( playerWorld );
    //m_vecParticleSet[3]->EmitVelocity( -this->GetForward() * 0 );
    m_vecParticleSet[3]->EmitVelocity( -this->GetRight() *speed*0.025f );
    m_vecParticleSet[3]->EmitSpeed( speed );

    //m_vecParticleSet[2]->EmitPositionW( this->GetPosition() );

    m_vecParticleSet[1]->WorldMat( ownerWorld );

    float healthPercentage = static_cast<float>( GetHealth() ) / static_cast<float>( GetMaxHealth() );
    if ( healthPercentage <= 0.3f ) {
        m_vecParticleSet[1]->Play();
    } else {
        m_vecParticleSet[1]->Stop();
        m_vecParticleSet[1]->Reset();
    }
    m_vecParticleSet[2]->Stop();
}

//**************Accessors****************//

const Ship::eAlignment Ship::GetAlignment()
{
	return m_Alignment;
}


const Ship::eShipType Ship::GetShipType()
{
	return m_ShipType;
}


int Ship::GetShipPartLevel(int index)
{
	enum PartType { UNKNOWN = -1, HULL = 0, MAST, WEAPON };


	switch (index)
	{
	case HULL:
		return m_nHullLevel;
		break;
	case MAST:
		return m_nMastLevel;
		break;
	case WEAPON:
		return m_cWeapon->GetLevel();
		break;
	}
	return -1;
}


const int Ship::GetWeaponsLevel()
{
	return m_cWeapon->GetLevel();
}



//****************************************//
/*Movement*/
//****************************************//

const float &Ship::GetSpeedLimit()
{
	return m_fCurrentMaxSpeed;
}


const float &Ship::GetTurnSpeedLimit()
{
	return m_fMaxRoT;
}


const float & Ship::GetSlowDownRate()
{
	return m_fSlowdownMultiplier;
}



//**************Mutators****************//

void Ship::SetAlignment(int align)
{
	m_Alignment = (eAlignment)align;
}


void Ship::SetShipType(int type)
{
	m_ShipType = (eShipType)type;
}


void Ship::SetController(IController * newController)
{
	if (m_pController)
	{
		delete m_pController;
		m_pController = nullptr;
	}

	m_pController = newController;
}


void Ship::SetWeapon(SideCannonHandler * newWeapon)
{
	if (newWeapon == nullptr)
	{
		return;
	}

	if (m_cWeapon)
	{
		delete m_cWeapon;
		m_cWeapon = nullptr;
	}
	m_cWeapon = newWeapon;
}


void Ship::SetDefense(int def)
{
	m_nDefense = def;
}



//****************************************//
/*Resource*/
//****************************************//

void Ship::IncreaseGold(int amount)
{
	m_nCurrGold += amount;

	if (m_nCurrGold > m_nMaxGold)
		m_nCurrGold = m_nMaxGold;
}


void Ship::DecreaseGold(int amount)
{
	m_nCurrGold -= amount;

	if (m_nCurrGold < 0)
		m_nCurrGold = 0;
}


void Ship::IncreaseRum(int amount)
{
	m_nCurrRum += amount;

	if (m_nCurrRum > m_nMaxRum)
		m_nCurrRum = m_nMaxRum;
}


void Ship::IncreaseFood(int amount)
{
	m_nCurrWaterFood += amount;

	if (m_nCurrWaterFood > m_nMaxWaterFood)
		m_nCurrWaterFood = m_nMaxWaterFood;
}


void Ship::IncreaseCrew(int amount)
{
	m_nCurrCrew += amount;

	if (m_nCurrCrew > m_nMaxCrew)
		m_nCurrCrew = m_nMaxCrew;
}


void Ship::DecreaseCrew(void)
{
	// crew--
	m_nCurrCrew--;

	// Enter hud update her


	// min crew
	if (m_nCurrCrew < 0)
		KillOwner();
}


bool Ship::RefillRum(void)
{
	if (m_nCurrRum == m_nMaxRum)
		return false;

	m_nCurrRum = m_nMaxRum;
	return true;
}


bool Ship::RefillRum(int amount)
{
	if (m_nCurrRum == m_nMaxRum)
		return false;

	IncreaseRum(amount);
	return true;
}


bool Ship::RefillFood(void)
{
	if (m_nCurrWaterFood == m_nMaxWaterFood)
		return false;

	m_nCurrWaterFood = m_nMaxWaterFood;
	return true;
}


bool Ship::RefillFood(int amount)
{
	if (m_nCurrWaterFood == m_nMaxWaterFood)
		return false;

	IncreaseFood(amount);
	return true;
}


bool Ship::RefillCrew(void)
{
	if (m_nCurrCrew == m_nMaxCrew)
		return false;

	m_nCurrCrew = m_nMaxCrew;
	return true;
}


bool Ship::RefillCrew(int amount)
{
	if (m_nCurrCrew == m_nMaxCrew)
		return false;

	IncreaseCrew(amount);
	return true;
}


bool Ship::Repair(void)
{
	if (m_nHealth == m_nMaxHealth)
		return false;

	// HP = MAX
	m_nHealth = m_nMaxHealth;


	return true;
}


bool Ship::RepairSails(void)
{
	if (m_fReducedSpeedTimer <= 0)
		return false;

	m_fReducedSpeedTimer = 0;
	return true;
}



//****************************************//
/*Movement*/
//****************************************//

void Ship::SetMaxSpeed(float newSpeed)
{
	m_fCurrentMaxSpeed = m_fMaxSpeed = newSpeed;
}


void Ship::SetSlowDownMultipler(float newSlowDown)
{
	m_fSlowdownMultiplier = newSlowDown;
}


void Ship::SetMaxTurnRate(float turnRate)
{
	m_fMaxRoT = turnRate;
}


void Ship::SetMaxHealth(int health)
{

	m_nHealth = m_nMaxHealth = health;

}


void Ship::SetSpeed(float speed)
{
	if (speed < 0)
	{
		m_fSpeed = 0;
	}

	m_fSpeed = speed;
}


void Ship::AdjustSpeed(float speed)
{
	m_fSpeed += speed;

	//Making sure speed does not go over the max speed;
	if (m_fSpeed > m_fCurrentMaxSpeed)
	{
		m_fSpeed = m_fCurrentMaxSpeed;
	}

	//Making sure speed does not go under 0;
	if (m_fSpeed < 0)
	{
		m_fSpeed = 0;
	}
}



//***************Data Members****************//

//****************************************//
/* Mast*/
//****************************************//

void Ship::ReduceSpeed()
{
	//Reducing the speed
	m_fCurrentMaxSpeed *= 0.7f;

	// able to board
	if (m_fCurrentMaxSpeed <= BOARDING_SPEED)
	{
		// boarding effects
		// show sail down icon
		if (false == m_sailsDown)
		{
			m_sailsDown = true;
			FlipComingBackFromRuinedSails();
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
		m_PopText = L"Sails Down!";
		m_PopTextTimer = 2.0f;
	}

	//Set timer
	m_fReducedSpeedTimer = m_fMaxTimer;
}


bool Ship::MastLevelUp()
{
	// max check
	if (m_nMastLevel >= LEVEL_MAX)
		return false;

	// level++
	m_nMastLevel++;

	// speed++ (speed += speed * (Lv/10.0f))
	m_fMaxSpeed += 1.0f;	//m_fMaxSpeed * float(m_nMastLevel * 0.05f);
	m_fCurrentMaxSpeed = m_fMaxSpeed;

	// slowdown++ (slowdown += slowdown * (Lv/10.0f))
	m_fSlowdownMultiplier += m_fSlowdownMultiplier * float(m_nMastLevel * 0.10f);


	return true;
}


void Ship::Translate(float dt)
{
	m_transform.Translate(GetForward()*m_fSpeed* dt);
}



//****************************************//
/* Hull */
//****************************************//

bool Ship::HullLevelUp()
{

	// max check
	if (m_nHullLevel >= LEVEL_MAX)
		return false;

	// level++
	m_nHullLevel++;

	// turn rate++ (rate += rate * (Lv/10))
	m_fMaxRoT += 0.20f;	//m_fMaxRoT * float(m_nHullLevel * 0.05f);
	m_nMaxHealth += 20;	//50;	//int(m_nMaxHealth * float(m_nHullLevel * 0.10f));

	//this spot is for increase the max count for your resources
	m_nMaxCrew += 1;
	m_nMaxRum += 10;
	m_nMaxWaterFood += 10;
	/*
	m_nCurrCrew = m_nMaxCrew;
	m_nCurrRum = m_nMaxRum;
	m_nCurrWaterFood = m_nMaxWaterFood;
	*/

	return true;
}



//****************************************//
/* Sound Data  */
//****************************************//

void Ship::DeathSound()
{
	if (m_bDead == false)
	{
		SoundManager* soundManager = static_cast<CGame*>(CGame::GetApplication())->GetSoundManager();
		soundManager->Play3DSound(AK::EVENTS::PLAY_SHIPDEATH);
		m_bDead = true;
	}
}
