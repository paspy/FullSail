/***************************************************************
|	File:		SceneManager.h
|	Author:		Jason N. Bloomfield
|	Created:	March 7, 2016
|	Modified:	March 18, 2016
|	Purpose:	SceneManager class used to store IScene*
***************************************************************/
#pragma once

#include "Stack.h"
#include "IScene.h"
#include "EntityManager.h"
#include "EventManager.h"
#include "Entity3D.h"
#include "IAmmo.h"

#define NEW_SCENE	((bool)true)
#define OLD_SCENE	((bool)false)

class CollisionManager;
//class Entity3D;


class SceneManager
{

public:
	struct FireCannonEventArgs
	{
		Ship*				owner;
		IAmmo::eAmmoType	type;
		XMFLOAT3			posOffset;
		float				lifespan;
		int					damage;

		FireCannonEventArgs(void) : owner(nullptr), type(IAmmo::eAmmoType::UNKNOWN), posOffset(0.0f, 0.0f, 0.0f), lifespan(0.0f), damage(0) {}
		FireCannonEventArgs(Ship* o, IAmmo::eAmmoType t, XMFLOAT3 po, float l, int d) : owner(o), type(t), posOffset(po), lifespan(l), damage(d) {}
	};
	struct SpawnCrateEventArgs
	{
		Ship*		playerShip;
		Ship*		enemyShip;

		SpawnCrateEventArgs(void) : playerShip(nullptr), enemyShip(nullptr) {}
		SpawnCrateEventArgs(Ship* ps, Ship* es) : playerShip(ps), enemyShip(es) {}
	};

	/// DO NOT ERASE ANY OF THIS!!!
	/*
	struct FireSideCannonEventArgs
	{
		int side; //0 Left 1 Right 2 Both
		Ship*				owner;
		IAmmo::eAmmoType	type;
		XMFLOAT3			posOffset;
		float				lifespan;
		int					damage;

		FireSideCannonEventArgs(void) : side(2), owner(nullptr), type(IAmmo::eAmmoType::UNKNOWN), posOffset(0.0f, 0.0f, 0.0f), lifespan(0.0f), damage(0) {}
		FireSideCannonEventArgs(int side, Ship* o, IAmmo::eAmmoType t, XMFLOAT3 po, float l, int d) : side(side), owner(o), type(t), posOffset(po), lifespan(l), damage(d) {}
	};
	*/
	/*
	struct FireLeftSideCannonEventArgs
	{
		Ship*				owner;
		IAmmo::eAmmoType	type;
		XMFLOAT3			posOffset;
		float				lifespan;
		int					damage;

		FireLeftSideCannonEventArgs(void) : owner(nullptr), type(IAmmo::eAmmoType::UNKNOWN), posOffset(0.0f, 0.0f, 0.0f), lifespan(0.0f), damage(0) {}
		FireLeftSideCannonEventArgs(Ship* o, IAmmo::eAmmoType t, XMFLOAT3 po, float l, int d) : owner(o), type(t), posOffset(po), lifespan(l), damage(d) {}
	};
	struct FireRightSideCannonEventArgs
	{
		Ship*				owner;
		IAmmo::eAmmoType	type;
		XMFLOAT3			posOffset;
		float				lifespan;
		int					damage;

		FireRightSideCannonEventArgs(void) : owner(nullptr), type(IAmmo::eAmmoType::UNKNOWN), posOffset(0.0f, 0.0f, 0.0f), lifespan(0.0f), damage(0) {}
		FireRightSideCannonEventArgs(Ship* o, IAmmo::eAmmoType t, XMFLOAT3 po, float l, int d) : owner(o), type(t), posOffset(po), lifespan(l), damage(d) {}
	};
	struct FireFrontCannonEventArgs
	{
		Ship*				owner;
		IAmmo::eAmmoType	type;
		XMFLOAT3			posOffset;
		float				lifespan;
		int					damage;

		FireFrontCannonEventArgs(void) : owner(nullptr), type(IAmmo::eAmmoType::UNKNOWN), posOffset(0.0f, 0.0f, 0.0f), lifespan(0.0f), damage(0) {}
		FireFrontCannonEventArgs(Ship* o, IAmmo::eAmmoType t, XMFLOAT3 po, float l, int d) : owner(o), type(t), posOffset(po), lifespan(l), damage(d) {}
	};
	struct FireBackCannonEventArgs
	{
		Ship*				owner;
		IAmmo::eAmmoType	type;
		XMFLOAT3			posOffset;
		float				lifespan;
		int					damage;


		FireBackCannonEventArgs(void) : owner(nullptr), type(IAmmo::eAmmoType::UNKNOWN), posOffset(0.0f, 0.0f, 0.0f), lifespan(0.0f), damage(0) {}
		FireBackCannonEventArgs(Ship* o, IAmmo::eAmmoType t, XMFLOAT3 po, float l, int d) : owner(o), type(t), posOffset(po), lifespan(l), damage(d) {}
	};
	*/


	SceneManager();
	~SceneManager();


	bool			Initialize		(void);
	void			Shutdown		(void);

	bool			Input			(void);
	void			Update			(float _dt);
	void			Render			(void);

	void			PushScene		(IScene::SceneType _st);
	void			PopScene		(void);
	void			ClearScenes		(void)					{ scene_stack.Clear(); }
	unsigned int	GetSize			(void)					{ return scene_stack.Size(); }
	IScene*			GetCurrScene	(void)					{ return scene_stack.GetTop(); }
    IScene::SceneType		GetCurrSceneType( void )        { return currScene; }

	IScene*			GetScene		(IScene::SceneType _st);


	CollisionManager*	GetCollisionManager	(void)	const	{ return collisionManager; }
	EntityManager&		GetEntityManager	(void)			{ return entityManager; }

	void			ResetScene		(IScene::SceneType _st);

	void			AddGamepadStatus	(void);
	void			RemoveGamepadStatus	(void);

	void			LockInput			(void);
	void			UnlockInput			(void);

	void			LoadSceneHudLists(void);
	void			ReleaseSceneHudLists(void);




	// Helper functions
	void			GetPortHudElementNames				(std::vector<std::string>& imagenames);
	void			SetupPortHudElementColors			(std::vector<std::string> imagenames);






	// Event funtions
	/*
	void			AddEvent					(const CGeneralEventArgs2<Entity3D::EntityType, Entity3D*>& args);
	void			RemoveEvent					(const CGeneralEventArgs<Entity3D*>& args);
	//void			FireSideCannonEvent			(const CGeneralEventArgs<FireSideCannonEventArgs>& args);
	void			FireLeftSideCannonEvent		(const CGeneralEventArgs<FireCannonEventArgs>& args);
	void			FireRightSideCannonEvent	(const CGeneralEventArgs<FireCannonEventArgs>& args);
	void			FireBothSideCannonsEvent	(const CGeneralEventArgs<FireCannonEventArgs>& args);
	void			FireFrontCannonEvent		(const CGeneralEventArgs<FireCannonEventArgs>& args);
	void			FireBackCannonEvent			(const CGeneralEventArgs<FireCannonEventArgs>& args);
	void			AddReputationEvent			(const CGeneralEventArgs<float>& args);
	void			SpawnCrate					(const CGeneralEventArgs<SpawnCrateEventArgs>& args);
	*/
	
	void			AddEvent					(const CGeneralEventArgs2<Entity3D::EntityType, Entity3D*>& args);
	void			RemoveEvent					(const CGeneralEventArgs<Entity3D*>& args);
	//void			FireSideCannonEvent			(const CGeneralEventArgs<FireSideCannonEventArgs>& args);
	void			FireLeftSideCannonEvent		(const EventArgs1<FireCannonEventArgs>& args);
	void			FireRightSideCannonEvent	(const EventArgs1<FireCannonEventArgs>& args);
	void			FireBothSideCannonsEvent	(const EventArgs1<FireCannonEventArgs>& args);
	void			FireFrontCannonEvent		(const EventArgs1<FireCannonEventArgs>& args);
	void			FireBackCannonEvent			(const EventArgs1<FireCannonEventArgs>& args);
	void			AddReputationEvent			(const EventArgs1<float>& args);
	void			SpawnCrate					(const EventArgs1<SpawnCrateEventArgs>& args);
	void			PortSaleEvent				(const EventArgs& args);
	void			PortSetColorEvent			(const EventArgs& args);
	



private:

	Stack<IScene*>		scene_stack;
	IScene*				scenes[IScene::SceneType::NUM_SCENES];
	IScene::SceneType	currScene;

	EntityManager		entityManager;
	CollisionManager*	collisionManager;
	bool				m_fNoticeBool;
	float				m_fNoticeTimer;
};
