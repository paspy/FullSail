/***************************************************************
|	File:		EntityManager.h
|	Author:		Jason N. Bloomfield
|	Created:	March 9, 2016
|	Modified:	March 17, 2016
|	Purpose:	EntityManager class used to store Entity3D*
***************************************************************/
#pragma once

//#include "LinkedList.h"
#include "pch.h"
#include "IScene.h"
#include "Transform.h"

//#define NUM_BUCKETS		((int)2)

class Entity3D;
class Ship;
class PlayerShip;
class AIShip;
class BossShip;
class IAmmo;
class Crate;
class Port;

typedef	std::vector<Renderer::CRenderable*>		Renderables;


class EntityManager
{
public:
	enum EntityBucket { UNKNOWN = -1, RENDERABLE = 0, NON_RENDERABLE, NUM_BUCKETS };


private:
	struct Buckets
	{
		std::vector<Entity3D*>	buckets[EntityBucket::NUM_BUCKETS];

		Buckets();
		Buckets(size_t s);
		Buckets(size_t s[EntityBucket::NUM_BUCKETS]);
		~Buckets();

		void	Update(float dt);
		void	Clear(void);
		void	Clear(EntityBucket _eb);

		bool	RemoveDeletables(IScene::SceneType _s, EntityManager* em);
		void	GetRenderables(Renderables& r, EntityBucket _eb);
	};


public:
	EntityManager();
	~EntityManager();

	bool			Initialize(void);
	void			Shutdown(void);

	bool			Input(void);
	void			Update(float _dt, IScene::SceneType _s);
	void			Render(void);


	// Manager list Methods
	void			AddEntity(IScene::SceneType _s, EntityBucket _eb, Entity3D* _e);
	bool			RemoveEntity(IScene::SceneType _s, EntityBucket _eb, unsigned int index);
	bool			RemoveEntity(IScene::SceneType _s, EntityBucket _eb, Entity3D* _e);
	void			RemoveAllEntities(IScene::SceneType _s, EntityBucket _eb);
	void			RemoveAllEntities(void);
	int				FindEntity(IScene::SceneType& out_s, EntityBucket& out_eb, Entity3D* _e);

	// Factory Methods
	void			LoadXMLScene(const char* fileName);
	PlayerShip*		CreatePlayerShip(void);
	AIShip*			CreateAIShip(const char* Object, const XMFLOAT4X4& matrix);
	AIShip*			CreateAIShip(const char* Object, const XMFLOAT4X4& matrix, Ship * _target, std::vector<Transform>* _waypoints);
	Crate*			CreateCrate(void);
	IAmmo*			CreateAmmo(Ship * owner);
	IAmmo*			CreateCannonball(Ship * owner);
	IAmmo*			CreateChainShot(Ship * owner);
	IAmmo*			CreateHarpoon(Ship * owner);
	Port*			CreatePort(void);
	Entity3D*		CreateWind(void);
	Entity3D*		CreateWhirlpool(void);
	Entity3D*		CreateHurricane(void);
	Entity3D*		CreateEntity3D(const char* FileName);
	Entity3D*		CreateEntity3D(const char* FileName, const XMFLOAT4X4& maxtrix);
	Entity3D*		CreateWater(void);

	// Renderer Methods
	Renderables		GetRenderables(IScene::SceneType _s, EntityBucket _eb);
	Renderables		GetRenderables(IScene::SceneType _s);

	Entity3D& GetObjectInstance(std::string objectName);
private:

	std::map<IScene::SceneType, Buckets>	entity_map;
	//EntityManger ObjectHolder
	std::unordered_map<std::string, Entity3D*> m_mpInstanceContainer;
};