#pragma once
#include "Entity3D.h"
#include "PortMenuScene.h"

namespace Renderer {
    class C3DHUD;
}

class Port : public Entity3D
{
public:
	Port(XMFLOAT4X4& _newTransfrom, const char* MeshName, const char* TextureName = nullptr);

	Port(const Port& other);
	~Port();
	Port& operator=(const Port& other);


	//**************Functions****************//
	void		Update			(float dt);
	void		OnCollision		(Collider& other);
	void		ActivateSale	(void);
	bool		ActiveSale		(void)				{ return (m_fSaleTimer > 0) ? true : false; }
	void		DecideDiscount	(void);
	bool		CurrentlyClosed	(void)				{ return (m_PortName == L"CLOSED") ? true : false; }


	//**************Accessors****************//
	virtual const EntityType GetType() final { return EntityType::PORT; }
	std::wstring	GetPortName			(void)							const	{ return m_OriginalText; }
	float			GetSaleTimer		(void)							const	{ return m_fSaleTimer; }
	int				GetDiscount			(PortMenuScene::PortItems i)	const	{ return m_nDiscounts[(int)i]; }
	int				GetNumPages			(void)							const	{ return m_nNumPages; }
	int				GetMaxUpgrade		(int i)							const;


	//**************Mutators****************//
	void			SetPortName		(std::wstring n)	{ m_PortName = n; m_OriginalText = n; }
	void			SetSaleTimer	(float st)			{ m_fSaleTimer = st; }
	void			SetNumPages		(int np)			{ m_nNumPages = np; }
	void			SetMaxUpgrade	(int i, int u);
	void			ClosePort		()					{ m_isClosed = true; }
	void			OpenPort		()					{ m_isClosed = false; }


private:
	std::wstring	m_PortName			= L"";
	std::wstring	m_OriginalText		= L"";
	bool			m_bHitVFX			= false;
	bool			m_isClosed			= false;
	bool			wasClosed			= false;

	float			m_fSaleTimer		= 0.0f;
	int				m_nDiscounts[11]	= {};
	int				m_nNumPages			= 0;
	int				m_nMaxUpgrades[3]	= {};

    std::unique_ptr<Renderer::C3DHUD>    m_portName3D;
};