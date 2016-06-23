#include "pch.h"
#include "Port.h"
#include "Colliders.h"
#include "Game.h"
#include "GameplayScene.h"
#include "Ship.h"

#define PORT_VFX_TIME	((float)1.0f)
#define PORT_SALE_TIME	((float)180.0f)	//180 == 3 mins


Port::Port(XMFLOAT4X4& _newTransfrom, const char* MeshName, const char* TextureName) : Entity3D(_newTransfrom, MeshName, TextureName)
{
	// port data
	VFXCountDown = PORT_VFX_TIME;
	m_PortName = L"default";
	m_OriginalText = L"default";
	m_bHitVFX = false;
	m_fSaleTimer = 0.0f;

	for (size_t i = 0; i < 11; i++)
		m_nDiscounts[i] = 0;

	m_nNumPages = 4;

	for (size_t i = 0; i < 3; i++)
		m_nMaxUpgrades[i] = 5;

	//((CGame*)CGame::GetApplication())->GetGameplayScene()->GetWaypoints().push_back(GetPosition());

	m_portName3D = std::make_unique<Renderer::C3DHUD>(&m_PortName);
}

Port::Port(const Port& other) : Entity3D(other)
{
	// port data
	VFXCountDown = PORT_VFX_TIME;
	m_PortName = other.m_PortName;
	m_OriginalText = other.m_OriginalText;
	m_bHitVFX = other.m_bHitVFX;
	m_fSaleTimer = other.m_fSaleTimer;

	for (size_t i = 0; i < 11; i++)
		m_nDiscounts[i] = other.m_nDiscounts[i];

	m_nNumPages = other.m_nNumPages;

	for (size_t i = 0; i < 3; i++)
		m_nMaxUpgrades[i] = other.m_nMaxUpgrades[i];

	m_portName3D = std::make_unique<Renderer::C3DHUD>(&m_PortName);
	CGame::GetApplication()->m_pRenderer->Add3DHUD(m_portName3D.get());
}

Port& Port::operator=(const Port& other)
{
	// insert return statement here
	if (this == &other)
		return (*this);


	//Calling the base operator
	Entity3D::operator=(other);


	// port data
	VFXCountDown = PORT_VFX_TIME;
	m_PortName = other.m_PortName;
	m_OriginalText = other.m_OriginalText;
	m_bHitVFX = other.m_bHitVFX;
	m_fSaleTimer = other.m_fSaleTimer;

	for (size_t i = 0; i < 11; i++)
		m_nDiscounts[i] = other.m_nDiscounts[i];

	m_nNumPages = other.m_nNumPages;

	for (size_t i = 0; i < 3; i++)
		m_nMaxUpgrades[i] = other.m_nMaxUpgrades[i];

	m_portName3D = std::make_unique<Renderer::C3DHUD>(&m_PortName);
	CGame::GetApplication()->m_pRenderer->Add3DHUD(m_portName3D.get());

	// assigning the data
	return (*this);
}

Port::~Port()
{
	CGame::GetApplication()->m_pRenderer->Remove3DHUD(m_portName3D.get());
	m_portName3D.reset();

}

void Port::Update(float dt)
{
	// dont update?
	if (to_delete == true)
		return;

	if (m_isClosed != wasClosed) {
		if (m_isClosed)
			m_PortName = L"CLOSED";
		else
			m_PortName = m_OriginalText;
	}
	wasClosed = m_isClosed;

	m_portName3D->SetTextPosition({ GetPosition().x, GetPosition().y + 3.0f, GetPosition().z });
	m_portName3D->SetFillColor({ 1.0f, 1.0f, 1.0f, 1.0f });

	// base update
	Entity3D::Update(dt);


	// update vfx timer
	VFXCountDown -= dt;
	if (VFXCountDown < 0.0f)
	{
		// reset timer
		VFXCountDown = PORT_VFX_TIME;

		// toggle vfx
		m_bHitVFX = !m_bHitVFX;
		m_pRenderInfo->HitEffect(m_bHitVFX);
	}



	// update sale timer
	if (m_fSaleTimer > 0.0f)
	{
		m_fSaleTimer -= dt;

		// end of sale
		if (m_fSaleTimer <= 0.0f)
		{
			// stop timer
			m_fSaleTimer = 0.0f;

			// etc...
			fsEventManager::GetInstance()->FireEvent(fsEvents::PortSale, nullptr);
		}
	}

}

void Port::OnCollision(Collider& other)
{
	// collision (apparently) does not matter here!
	return;


	if (to_delete == true)
		return;


	//SoundManager*	sound	= ((CGame*)CGame::GetApplication())->GetSoundManager();
	Entity3D*		object = static_cast<Entity3D*>(other.getID());
	EntityType		type = object->GetType();
	Ship*			ship = nullptr;



	// this might need to be changed
	switch (type)
	{
	case Entity3D::SHIP:
		break;

	case Entity3D::LAND:
	case Entity3D::UNKNOWN:
	case Entity3D::SHIP_PART:
	case Entity3D::CRATE:
	case Entity3D::AMMO:
	case Entity3D::WEATHER:
	default:
		break;
	}

}

void Port::ActivateSale(void)
{
	// start timer
	m_fSaleTimer = PORT_SALE_TIME;


	// setup discounts
	DecideDiscount();
}

void Port::DecideDiscount(void)
{
	CGame*			game = (CGame*)CGame::GetApplication();
	SceneManager*	sm = game->GetSceneManager();
	PortMenuScene*	pm = (PortMenuScene*)sm->GetScene(IScene::SceneType::PORT);


	//enum PortItems { UNKNOWN = -1, WATER_FOOD = 0, RUM, CREW, REPAIR, WEAPONS, SAILS, HULL, FLAMETHROWER, BARRELS, GREEKSHOT, FLEET, EXIT };
	int	n_off[5] = { 5, 10, 15, 20, 25 };
	int n_max = 0;
	int n_min = 0;
	int resources = 0;
	int repairs = 0;
	int upgrades = 0;
	int weapons = 0;
	int fleets = 0;


	// ship resources
	//n_max		= 45;
	//n_min		= 10;
	resources = n_off[rand() % 5];	//rand() % (n_max - n_min + 1) + n_min;	//n_off[rand() % 5];

	// ship repairs
	//n_max		= 45;
	//n_min		= 10;
	repairs = n_off[rand() % 5];	//rand() % (n_max - n_min + 1) + n_min;	//n_off[rand() % 5];

	// ship upgrades
	//n_max		= 45;
	//n_min		= 10;
	upgrades = n_off[rand() % 5];	//rand() % (n_max - n_min + 1) + n_min;	//n_off[rand() % 5];

	// special weapons
	//n_max		= 45;
	//n_min		= 10;
	weapons = n_off[rand() % 5];	//rand() % (n_max - n_min + 1) + n_min;	//n_off[rand() % 5];

	// fleets
	//n_max		= 45;
	//n_min		= 10;
	fleets = n_off[rand() % 5];	//rand() % (n_max - n_min + 1) + n_min;	//n_off[rand() % 5];




	// page1
	m_nDiscounts[PortMenuScene::PortItems::WATER_FOOD] = resources;
	m_nDiscounts[PortMenuScene::PortItems::RUM] = resources;
	m_nDiscounts[PortMenuScene::PortItems::CREW] = resources;
	m_nDiscounts[PortMenuScene::PortItems::REPAIR] = repairs;

	// page2
	m_nDiscounts[PortMenuScene::PortItems::WEAPONS] = upgrades;
	m_nDiscounts[PortMenuScene::PortItems::HULL] = upgrades;
	m_nDiscounts[PortMenuScene::PortItems::SAILS] = upgrades;

	// page3
	m_nDiscounts[PortMenuScene::PortItems::FLAMETHROWER] = weapons;
	m_nDiscounts[PortMenuScene::PortItems::BARRELS] = weapons;
	m_nDiscounts[PortMenuScene::PortItems::GREEKSHOT] = weapons;

	// page4
	m_nDiscounts[PortMenuScene::PortItems::FLEET] = fleets;
}

int Port::GetMaxUpgrade(int i)	const
{
	if (i < 0 || i > 2)
		return -1;

	return m_nMaxUpgrades[i];
}

void Port::SetMaxUpgrade(int i, int u)
{
	if (i < 0 || i > 2)
		return;

	m_nMaxUpgrades[i] = u;
}