#pragma once
#include "AIShip.h"
#include "Colliders.h"

class Port;


/********************This is the new base to start off*************************/
class BossShip : public AIShip
{
public:
	BossShip(const XMFLOAT4X4 & _newTransfrom, std::string meshName, std::string textureName);

	//****************Trilogy*****************//
	BossShip& operator=(const BossShip& other);
	BossShip(const BossShip& _cpy);
	virtual ~BossShip();


	//**************Functions****************//
	virtual void	Update(float dt) final;
	virtual void	OnCollision(Collider& other);

	//****************************************//
	/*Other*/
	//****************************************//
	virtual void	KillOwner			(void);
	void			CallReinforcements	(float dt);
	void			HealAtPort			(Port* port);
	//void			TrackPlayer			(void);

	//****************************************//
	/*Events*/
	//****************************************//
	//void			BossRethinkTrackingCourse	(const EventArgs& args);
	//void			BossStayOnTrackingCourse	(const EventArgs& args);




	//**************Accessors****************//

	//****************************************//
	/* Tracking with ports  */
	//****************************************//
	//bool			GetTrackingCourseSet	(void) const { return m_bTrackingCourseSet; }
	//bool			GetChangeTrackingCourse	(void) const { return m_bChangeTrackingCourse; }
	Port*			GetLastPlayerPort		(void) const { return m_LastPlayerPort; }


	//**************Mutators****************//

	//****************************************//
	/* Tracking with ports  */
	//****************************************//
	//void			SetTrackingCourseSet	(bool tcs)	{ m_bTrackingCourseSet = tcs; }
	//void			SetChangeTrackingCourse	(bool cts)	{ m_bChangeTrackingCourse = cts; }
	void			SetLastPlayerPort		(Port* lpp)	{ m_LastPlayerPort = lpp; }



private:
	//***************Data Members****************//

	//****************************************//
	/* Reinforcements  */
	//****************************************//
	float m_ReinforcementTimer;
	int m_ReinforcementCount;

	//****************************************//
	/* Healing at port  */
	//****************************************//
	bool								m_PortCollision;
	std::unique_ptr<Renderer::C3DHUD>	m_HealIcon;
	DirectX::SimpleMath::Vector4		m_HealIconPos;
	int									m_HealFrameCounter;

	//****************************************//
	/* Tracking with ports  */
	//****************************************//
	//bool								m_bTrackingCourseSet;
	//bool								m_bChangeTrackingCourse;
	Port*								m_LastPlayerPort;
	//Quadtree<std::wstring>*				m_PortQuadTree;
};
