#include "pch.h"
#include "EntityManager.h"
#include "IScene.h"
#include "Entity3D.h"
#include "Ship.h"
#include "PlayerShip.h"
#include "AIShip.h"
#include "BossShip.h"
#include "IAmmo.h"
#include "Crate.h"
#include "Port.h"
#include "AiController.h"
#include <cassert>
#include "Game.h"
#include "Island.h"
#include "EntityLoader.h"
#include "Colliders.h"


EntityManager::Buckets::Buckets()
{
}


EntityManager::Buckets::Buckets(size_t s)
{
	for (int i = 0; i < EntityBucket::NUM_BUCKETS; i++)
	{
		buckets[i].resize(s);
	}
}


EntityManager::Buckets::Buckets(size_t s[NUM_BUCKETS])
{
	for (int i = 0; i < EntityBucket::NUM_BUCKETS; i++)
	{
		buckets[i].resize(s[i]);
	}
}


EntityManager::Buckets::~Buckets()
{
	Clear();
}


void EntityManager::Buckets::GetRenderables(Renderables& r, EntityBucket _eb)
{
	// buckets size
	size_t size = buckets[_eb].size();

	// resize vector
	//r.resize(size);

	// loop through bucket items
	//Renderables::const_iterator begin = r.begin();

	for (size_t i = 0; i < size; i++)
	{
		// get render info
		r.push_back(buckets[_eb][i]->GetRenderInfo());
	}
}


void EntityManager::Buckets::Update(float dt)
{
	// loop through buckets
	for (int i = 0; i < EntityBucket::NUM_BUCKETS; i++)
	{
		// loop through bucket items
		for (size_t j = 0; j < buckets[i].size(); j++)
		{
			//if (j >= buckets[i].size())
			//	break;

			// current entity
			Entity3D* curr = buckets[i][j];

			// update
			curr->Update(dt);
		}
	}
}


void EntityManager::Buckets::Clear(void)
{
	for (int i = 0; i < EntityBucket::NUM_BUCKETS; i++)
	{
		Clear((EntityManager::EntityBucket)i);
	}
}


void EntityManager::Buckets::Clear(EntityBucket _eb)
{
	// loop through items buckets
	//for (size_t j = 0; j < buckets[_eb].size(); j++)
	//{
	//	// null check
	//	//if (buckets[_eb][j] == nullptr)
	//	//	continue;

	//	// delete item
	//	//delete buckets[_eb][j];
	//	buckets[_eb][j] = nullptr;
	//}

	// clear vector
	buckets[_eb].clear();
}


bool EntityManager::Buckets::RemoveDeletables(IScene::SceneType _s, EntityManager* em)
{
	bool result = false;

	// loop through buckets
	for (int i = 0; i < EntityBucket::NUM_BUCKETS; i++)
	{
		// loop through bucket items
		for (size_t j = 0; j < buckets[i].size(); j++)
		{
			// null check
			//if (buckets[i][j] == nullptr)
			//{
			//	em->RemoveEntity(_s, (EntityManager::EntityBucket)i, (unsigned int)j);
			//	j--;
			//	continue;
			//}


			// not to be deleted
			if (buckets[i][j]->ToBeDeleted() == false)
				continue;

			// to be deleted
			em->RemoveEntity(_s, (EntityManager::EntityBucket)i, (unsigned int)j);
			j--;


			// removed somethig
			result = true;
		}
	}

	return result;
}


EntityManager::EntityManager()
{
	entity_map = std::map<IScene::SceneType, Buckets>();
}


EntityManager::~EntityManager()
{
	EntityManager::Shutdown();
}


bool EntityManager::Initialize(void)
{
	for (int i = 0; i < IScene::SceneType::NUM_SCENES; i++)
	{
		entity_map[(IScene::SceneType)i] = EntityManager::Buckets::Buckets();
	}


	if (entity_map.size() != IScene::SceneType::NUM_SCENES)
	{
		assert("EntityManager::Initialize - Entity map not at correct size!");
		return false;
	}

	return true;
}


void EntityManager::Shutdown(void)
{
	// remove all entities
	RemoveAllEntities();


	// clear map
	entity_map.clear();
}


bool EntityManager::Input(void)
{
	return false;
}


void EntityManager::Update(float _dt, IScene::SceneType _s)
{

	// update buckets
	entity_map[_s].Update(_dt);


	// remove non-update entities
	bool removed = entity_map[_s].RemoveDeletables(_s, this);


	// redo renderables?
	if (removed == true)
	{
		// clear renderables
		CGame::GetApplication()->m_pRenderer->ClearRenderables();
		Renderables rendervect = GetRenderables(_s, EntityManager::EntityBucket::RENDERABLE);
		CGame::GetApplication()->m_pRenderer->SetUpRenderables(rendervect);
	}





	/*
	// look through buckets
	for (int i = 0; i < NUM_BUCKETS; i++)
	{
		// remove update entities
		bool removed = entity_map[_s].RemoveDeletables(_s, this);


		// redo renderables?
		if (removed == true)
		{
			// clear renderables
			CGame::GetApplication()->m_pRenderer->ClearRenderables();
			Renderables rendervect = GetRenderables(_s, EntityManager::EntityBucket::RENDERABLE);
			CGame::GetApplication()->m_pRenderer->SetUpRenderables(rendervect);
		}



		// update buckets
		entity_map[_s].Update(_dt);
	}
	*/

	/*
	// make iters
	std::map<IScene::SceneType, Buckets>::iterator iter	= entity_map.begin();
	std::map<IScene::SceneType, Buckets>::iterator end	= entity_map.end();

	// look through map
	for (; iter != end; iter++)
	{
		// remove update entities
		iter->second.RemoveDeletables(iter->first);

		// update entities
		iter->second.Update(_dt);
	}
	*/
}


void EntityManager::Render(void)
{
}


void EntityManager::AddEntity(IScene::SceneType _s, EntityBucket _eb, Entity3D* _e)
{
	// null check
	if (_e == nullptr)
		assert("EntityManager::AddEntity - entity is NULL");

	// scene check
	if (_s <= IScene::SceneType::UNKNOWN || _s >= IScene::SceneType::NUM_SCENES)
		assert("EntityManager::AddEntity - scene is unknown");

	// bucket check
	if (_eb == EntityBucket::UNKNOWN)
		assert("EntityManager::AddEntity - bucket is unknown");

	// push
	entity_map[_s].buckets[(int)_eb].push_back(_e);
}


bool EntityManager::RemoveEntity(IScene::SceneType _s, EntityBucket _eb, unsigned int index)
{
	// scene check
	if (_s <= IScene::SceneType::UNKNOWN || _s >= IScene::SceneType::NUM_SCENES)
	{
		assert("EntityManager::AddEntity - scene is unknown");
		return false;
	}

	// bucket check
	if (_eb == EntityBucket::UNKNOWN)
	{
		assert("EntityManager::RemoveEntity - bucket is unknown");
		return false;
	}

	// size check
	size_t size = entity_map[_s].buckets[_eb].size();
	if (index >= (unsigned int)size)
	{
		assert("EntityManager::RemoveEntity - index is unknown");
		return false;
	}


	// delete entity
	//delete entity_map[_s].buckets[_eb][index];
	//entity_map[_s].buckets[_eb][index] = nullptr;

	// remove entity
	auto begin = entity_map[_s].buckets[_eb].begin();
	entity_map[_s].buckets[_eb].erase(begin + index);


	// removed?
	if (entity_map[_s].buckets[_eb].size() < size)
	{
		// reset renderables
		CGame::GetApplication()->m_pRenderer->ClearRenderables();
		Renderables rendervect = GetRenderables(_s, _eb);
		CGame::GetApplication()->m_pRenderer->SetUpRenderables(rendervect);
		return true;
	}


	//return
	assert("EntityManager::RemoveEntity - entity was not removed!");
	return false;
}


bool EntityManager::RemoveEntity(IScene::SceneType _s, EntityBucket _eb, Entity3D* _e)
{
	// null check
	if (_e == nullptr)
	{
		assert("EntityManager::RemoveEntity - entity is NULL");
		return false;
	}

	// scene check
	if (_s <= IScene::SceneType::UNKNOWN || _s >= IScene::SceneType::NUM_SCENES)
	{
		assert("EntityManager::AddEntity - scene is unknown");
		return false;
	}

	// bucket check
	if (_eb == EntityBucket::UNKNOWN)
	{
		assert("EntityManager::RemoveEntity - bucket is unknown");
		return false;
	}

	// find entity
	size_t size = entity_map[_s].buckets[_eb].size();
	for (size_t i = 0; i < size; i++)
	{
		// not entity
		if (entity_map[_s].buckets[_eb][i] != _e)
			continue;

		// delete entity
		//delete entity_map[_s].buckets[_eb][i];
		//entity_map[_s].buckets[_eb][i] = nullptr;

		// erase entity
		auto begin = entity_map[_s].buckets[_eb].begin();
		entity_map[_s].buckets[_eb].erase(begin + i);

		// break
		break;
	}


	// removed?
	if (entity_map[_s].buckets[_eb].size() < size)
	{
		// reset renderables
		CGame::GetApplication()->m_pRenderer->ClearRenderables();
		Renderables rendervect = GetRenderables(_s, _eb);
		CGame::GetApplication()->m_pRenderer->SetUpRenderables(rendervect);
		return true;
	}


	//return
	assert("EntityManager::RemoveEntity - entity was not removed!");
	return false;
}


void EntityManager::RemoveAllEntities(IScene::SceneType _s, EntityBucket _eb)
{
	// scene check
	if (_s <= IScene::SceneType::UNKNOWN || _s >= IScene::SceneType::NUM_SCENES)
		assert("EntityManager::AddEntity - scene is unknown");


	// bucket check
	if (_eb == EntityBucket::UNKNOWN)
		assert("EntityManager::RemoveAllEntities - bucket is unknown");


	// clear _eb bucket at scene _s
	entity_map[_s].Clear(_eb);


	// clear renderables
	CGame::GetApplication()->m_pRenderer->ClearRenderables();

}


void EntityManager::RemoveAllEntities(void)
{
	// make iters
	std::map<IScene::SceneType, Buckets>::iterator iter = entity_map.begin();
	std::map<IScene::SceneType, Buckets>::iterator end = entity_map.end();


	// look through map
	for (; iter != end; iter++)
	{
		// look through buckets
		for (int i = 0; i < EntityBucket::NUM_BUCKETS; i++)
		{
			// clear buckets
			entity_map[iter->first].Clear((EntityBucket)i);
		}
	}


	// clear renderables
	CGame::GetApplication()->m_pRenderer->ClearRenderables();



	//Clearing out the instance map
		//creating the iters for the containter
	std::unordered_map<std::string, Entity3D*>::iterator InstanceIter = m_mpInstanceContainer.begin();
	std::unordered_map<std::string, Entity3D*>::iterator InstanceEnd = m_mpInstanceContainer.end();

	//looping through the map from begin to end
	for (; InstanceIter != InstanceEnd; InstanceIter++)
	{
		//deleting every instance that we find
		delete InstanceIter->second;
	}

	//clearing the map
	m_mpInstanceContainer.clear();

	/*
	// look through map
	for (; iter != end; iter++)
	{
		// look through buckets
		for (int i = 0; i < NUM_BUCKETS; i++)
		{
			// loop through bucket items
			size_t size = entity_map[iter->first].buckets[i].size();
			for (size_t j = 0; j < size; j++)
			{
				entity_map[iter->first].Clear();

				// delete entity
				//delete entity_map[iter->first].buckets[i];
				//entity_map[iter->first].buckets[i][j] = nullptr;
			}


			// remove entities
			//entity_map[iter->first].buckets[i].clear();
		}
	}
	*/
}


int EntityManager::FindEntity(IScene::SceneType& out_s, EntityBucket& out_eb, Entity3D* _e)
{
	// make iters
	std::map<IScene::SceneType, Buckets>::iterator iter = entity_map.begin();
	std::map<IScene::SceneType, Buckets>::iterator end = entity_map.end();


	// look through map
	for (; iter != end; iter++)
	{
		// look through buckets
		for (int i = 0; i < EntityBucket::NUM_BUCKETS; i++)
		{
			// buckets.item[n]
			size_t size = entity_map[iter->first].buckets[i].size();
			for (size_t j = 0; j < size; j++)
			{
				// not correct entity?
				if (entity_map[iter->first].buckets[i][j] != _e)
					continue;

				// entity found
				out_s = iter->first;
				out_eb = (EntityBucket)i;
				return (int)j;
			}
		}
	}

	// entity not found
	out_s = IScene::SceneType::UNKNOWN;
	out_eb = EntityBucket::UNKNOWN;
	return -1;
}


void EntityManager::LoadXMLScene(const char * fileName)
{
	EntityLoader::XML_LoadScene(fileName);
}


PlayerShip* EntityManager::CreatePlayerShip(void)
{
	//grabbing the adddress from the object from the container
	Entity3D** object = &m_mpInstanceContainer["PlayerShip"];

	//Checking to see if its null
	if ((*object) == nullptr)
	{
		//read in the data into xml
		(*object) = EntityLoader::XML_LoadObject("PlayerShip");

		(*object)->SetToBeDeleted(true);

		//if read in incorrectly
		if ((*object) == nullptr)
		{
			//notify the devs
			FS_PRINT_OUTPUT("Player was not able to be loaded");
			return nullptr;
		}
	}


	CollisionManager& GameCollision = *((CGame*)CGame::GetApplication())->GetSceneManager()->GetCollisionManager();


	//if the object was created correctly 
	//Sending a copy of the deferenced pointer 
	Entity3D* newObject = nullptr;

	//Creating the newx Object
	newObject = newx PlayerShip(*((PlayerShip*)(*object)));
	// setup collider
	TemplatedCollisionCallback<Ship>* thing = newx TemplatedCollisionCallback<Ship>((Ship*)newObject, &Ship::OnCollision);
	BoxCollider* collider = newx BoxCollider(GameCollision, newObject->GetLocalMatrix(), newObject, thing, XMFLOAT3(0.7f, 1, 1.7f), XMFLOAT3(0, 0.311f, -0.154f), false);

	//Setting the Collider
	newObject->SetCollider(collider);


	newObject->SetToBeDeleted(false);

	//if the object was crated correctly 

	//Sending a copy of the deferenced pointer 
	return (PlayerShip*)newObject;
}


AIShip* EntityManager::CreateAIShip(const char* Object, const XMFLOAT4X4& matrix)
{

	return (AIShip*)CreateEntity3D(Object, matrix);
}


AIShip* EntityManager::CreateAIShip(const char* Object, const XMFLOAT4X4& matrix, Ship * _target, std::vector<Transform>* _waypoints)
{
	// Making the newx location Identity
	XMFLOAT4X4 newMatrix;
	XMStoreFloat4x4(&newMatrix, XMMatrixIdentity());


	// create ship
	AIShip* aiship = (AIShip*)CreateEntity3D(Object, matrix);

	//getting access to the controller
	AiController*  aiController = ((AiController*)aiship->GetController());


	aiController->SetWaypoints(_waypoints);

	aiController->ChaseTarget(_target);
	return aiship;
}


Crate* EntityManager::CreateCrate(void)
{
	Crate* crate = nullptr;

	crate = (Crate*)EntityLoader::XML_LoadObject("CrateXML.xml");
	if (crate == nullptr)
		assert("EntityManager::CreateCrate - crate could not be created!");
	/*
	if (crate != nullptr)
	{
		crate->SetRum(0);
		crate->SetWaterFood(0);
		crate->SetGold(0);
		crate->SetDirection(XMFLOAT3(0.0f, 0.0f, 0.0f));
		crate->SetLifespan(0.0f);
	}
	*/


	//Entity3D::EntityType type = crate->GetType();

	return crate;
	/*
	// Making the newx location Identity
	XMFLOAT4X4 newMatrix;
	XMStoreFloat4x4(&newMatrix, XMMatrixIdentity());


	// create ship
	Crate* crate = newx Crate(newMatrix, "testShip.mesh", "testPlayerShipTexture.dds");
	if (crate == nullptr)
		assert("EntityManager::CreateCrate - crate could not be created!");

	crate->SetupRenderData("testDefaultMaterial.xml", 0, 1, "testPlayerShipTexture.dds");
	crate->SetParticleSetToPause();

	return crate;
	*/
}


IAmmo* EntityManager::CreateAmmo(Ship * owner)
{
	// Making the newx location Identity
	XMFLOAT4X4 newMatrix;
	XMStoreFloat4x4(&newMatrix, XMMatrixIdentity());


	// create ship
	IAmmo* ammo = newx IAmmo(owner, IAmmo::eAmmoType::UNKNOWN, newMatrix, "CannonBall.mesh", "TestTexture.dds");
	if (ammo == nullptr)
		assert("EntityManager::CreateChainShot - chainshot could not be created!");

	ammo->SetupRenderData("testDefaultMaterial.xml", 0, 1, "TestTexture.dds");
	//ammo->SetParticleSetToPause();

	return ammo;


}


IAmmo* EntityManager::CreateCannonball(Ship * owner)
{
	// Making the newx location Identity
	XMFLOAT4X4 newMatrix = owner->GetWorldMatrix();


	// create ship
	IAmmo* cannonball = newx IAmmo(owner, IAmmo::eAmmoType::CANNONBALL, newMatrix, "CannonBall.mesh", "TestTexture.dds");
	if (cannonball == nullptr)
		assert("EntityManager::CreateCannonball - cannonball could not be created!");

	cannonball->SetupRenderData("Cannonball.xml", 0, 1, "TestTexture.dds");

	return cannonball;


}


IAmmo* EntityManager::CreateChainShot(Ship * owner)
{
	// Making the newx location Identity
	XMFLOAT4X4 newMatrix = owner->GetWorldMatrix();

	// create ship
	IAmmo* chainshot = newx IAmmo(owner, IAmmo::eAmmoType::CHAIN_SHOT, newMatrix, "ChainCannonball.mesh", "TestTexture.dds");
	if (chainshot == nullptr)
		assert("EntityManager::CreateChainShot - chainshot could not be created!");

	chainshot->SetupRenderData("Cannonball.xml", 0, 1, "TestTexture.dds");

	return chainshot;


}


IAmmo* EntityManager::CreateHarpoon(Ship * owner)
{
	// Making the newx location Identity
	XMFLOAT4X4 newMatrix;
	XMStoreFloat4x4(&newMatrix, XMMatrixIdentity());


	// create ship
	IAmmo* harpoon = newx IAmmo(owner, IAmmo::eAmmoType::HARPOON, newMatrix, "CannonBall.mesh", "TestTexture.dds");
	if (harpoon == nullptr)
		assert("EntityManager::CreateHarpoon - cannonball could not be created!");

	harpoon->SetupRenderData("testDefaultMaterial.xml", 0, 1, "TestTexture.dds");

	return harpoon;


}


Port* EntityManager::CreatePort(void)
{
	Port* port = nullptr;

	port = (Port*)EntityLoader::XML_LoadObject("PortXML.xml");
	if (port == nullptr)
		assert("EntityManager::CreatePort - port could not be created!");

	return port;
}


Entity3D* EntityManager::CreateWind(void)
{
	return nullptr;
}


Entity3D* EntityManager::CreateWhirlpool(void)
{
	return nullptr;
}


Entity3D* EntityManager::CreateHurricane(void)
{
	return nullptr;
}

//This will not created the collider for you
Entity3D* EntityManager::CreateEntity3D(const char* objectName)
{
	//grabbing the adddress from the object from the container
	Entity3D** object = &m_mpInstanceContainer[objectName];

	//Checking to see if its null
	if ((*object) == nullptr)
	{
		//read in the data into xml
		(*object) = EntityLoader::XML_LoadObject(objectName);

		//if read in incorrectly
		if ((*object) == nullptr)
		{
			//notify the devs
			FS_PRINT_OUTPUT("Player was not able to be loaded");
			return nullptr;
		}
	}



	//if the object was created correctly 
	//Sending a copy of the deferenced pointer 
	Entity3D* newObject = nullptr;

	switch ((*object)->GetType())
	{
	case Entity3D::EntityType::LAND:
	{
		//Creating the newx Object
		newObject = newx Island(*((Island*)(*object)));
	}
	break;
	case Entity3D::EntityType::SHIP:
	{
		//Creating the newx Object
		Ship::eShipType shiptype = ((Ship*)(*object))->GetShipType();
		switch (shiptype)
		{
		case Ship::eShipType::TYPE_PLAYER:
			newObject = newx PlayerShip(*((PlayerShip*)(*object)));
			break;
		case Ship::eShipType::TYPE_AI:
			newObject = newx AIShip(*((AIShip*)(*object)));
			break;
		case Ship::eShipType::TYPE_BOSS:
			newObject = newx BossShip(*((BossShip*)(*object)));
			break;
		case Ship::eShipType::TYPE_UNKNOWN:
		default:
			newObject = newx Ship(*((Ship*)(*object)));
			break;
		}
	}
	break;
	case Entity3D::EntityType::PORT:
	{
		//Creating the newx Object
		newObject = newx Port(*((Port*)(*object)));
	}
	break;
	case Entity3D::EntityType::CRATE:
	{
		newObject = newx Crate(*((Crate*)(*object)));
	}
	break;
	}

	return newObject;
}

//This will create a collider for you  =) your welcome  have a nice day
Entity3D* EntityManager::CreateEntity3D(const char* objectName, const XMFLOAT4X4& matrix)
{
	//grabbing the address from the object from the container
	Entity3D** object = &m_mpInstanceContainer[objectName];

	//Checking to see if its null
	if ((*object) == nullptr)
	{
		//read in the data into xml
		(*object) = EntityLoader::XML_LoadObject(objectName);

		if ((*object) == nullptr)
		{
			return nullptr;
		}

		(*object)->SetToBeDeleted(true);

		//if read in incorrectly
		if ((*object) == nullptr)
		{
			//notify the devs
			FS_PRINT_OUTPUT("Player was not able to be loaded");
			return nullptr;
		}
	}

	//setting my local matrix
	(*object)->GetTransform().SetLocalMatrix(matrix);

	CollisionManager& GameCollision = *((CGame*)CGame::GetApplication())->GetSceneManager()->GetCollisionManager();


	//if the object was created correctly 
	//Sending a copy of the deferenced pointer 
	Entity3D* newObject = nullptr;

	switch ((*object)->GetType())
	{
	case Entity3D::EntityType::LAND:
	{
		//Creating the newx Object
		newObject = newx Island(*((Island*)(*object)));

		if (newObject == nullptr)
		{
			return nullptr;
		}

		//Setting up the Collider 
		TemplatedCollisionCallback<Entity3D>*	thing = newx TemplatedCollisionCallback<Entity3D>(newObject, &Entity3D::OnCollision);
		MeshCollider*							collider = newx MeshCollider(GameCollision, newObject->GetLocalMatrix(),
			*CGame::GetApplication()->m_pAssetManager->Meshes(((Island*)newObject)->GetCollisionMesh()),
			newObject, thing, false, "Island");

		//Setting the Collider
		newObject->SetCollider(collider);
	}
	break;
	case Entity3D::EntityType::SHIP:
	{
		XMFLOAT3 dims;
		XMFLOAT3 offset;
		Ship::eShipType shiptype = ((Ship*)(*object))->GetShipType();
		switch (shiptype)
		{
		case Ship::eShipType::TYPE_PLAYER:
			newObject = newx PlayerShip(*((PlayerShip*)(*object)));
			dims = { 0.7f, 1, 1.7f };
			offset = { 0,0.311f,-0.154f };
			break;
		case Ship::eShipType::TYPE_AI:
			newObject = newx AIShip(*((AIShip*)(*object)));
			dims = { 0.6f, 0.9f, 1.6f };
			offset = { 0,0.348f,-0.144f };
			break;
		case Ship::eShipType::TYPE_BOSS:
			newObject = newx BossShip(*((BossShip*)(*object)));
			dims = { 0.85f, 1.1f, 3.1f };
			offset = { 0,0.28f,-0.584f };
			break;
		case Ship::eShipType::TYPE_UNKNOWN:
			newObject = newx Ship(*((Ship*)(*object)));
			dims = { 0.7f, 1, 1.7f };
			offset = { 0,0.311f,-0.154f };
			break;
		}

		if (newObject == nullptr)
		{
			return nullptr;
		}

		// setup collider
		TemplatedCollisionCallback<Ship>* thing = newx TemplatedCollisionCallback<Ship>((Ship*)newObject, &Ship::OnCollision);
		BoxCollider* collider = newx BoxCollider(GameCollision, newObject->GetLocalMatrix(), newObject, thing, dims, offset, false);

		//Setting the Collider
		newObject->SetCollider(collider);
	}
	break;
	case Entity3D::EntityType::PORT:
	{
		//Creating the newx Object
		newObject = newx Port(*((Port*)(*object)));

		if (newObject == nullptr)
		{
			return nullptr;
		}

		// setup collider
		TemplatedCollisionCallback<Port>*	thing = newx TemplatedCollisionCallback<Port>((Port*)newObject, &Port::OnCollision);
		BoxCollider* collider = newx BoxCollider(GameCollision, newObject->GetLocalMatrix(), newObject, thing, XMFLOAT3(2.0f, 1.0f, 1.0f), XMFLOAT3(0, 0.0f, 0), true);

		//Setting the Collider
		newObject->SetCollider(collider);
	}
	break;
	case Entity3D::EntityType::CRATE:
	{
		newObject = newx Crate(*((Crate*)(*object)));

		if (newObject == nullptr)
		{
			return nullptr;
		}

		TemplatedCollisionCallback<Crate>* thing = newx TemplatedCollisionCallback<Crate>((Crate*)newObject, &Crate::OnCollision);
		SphereCollider* collider = newx SphereCollider(GameCollision, newObject->GetLocalMatrix(), newObject, thing, 1.5f);

		newObject->SetCollider(collider);
	}
	break;
	}

	newObject->SetToBeDeleted(false);

	return newObject;
}


Entity3D* EntityManager::CreateWater(void)
{
	// Making the newx location Identity
	XMFLOAT4X4 newMatrix;
	XMStoreFloat4x4(&newMatrix, XMMatrixIdentity());

	Entity3D* entity = newx Entity3D(newMatrix, "testWaterPlane.mesh");
	if (entity == nullptr)
		assert("EntityManager::CreateWater - Water could not be created!");

	entity->CreateWaterRenderData();
	return entity;
}


Renderables EntityManager::GetRenderables(IScene::SceneType _s, EntityBucket _eb)
{
	// vector
	Renderables renderList;


	// get renderables
	entity_map[_s].GetRenderables(renderList, _eb);


	// return
	return renderList;
}


Renderables EntityManager::GetRenderables(IScene::SceneType _s)
{
	// vector
	Renderables renderList;


	for (int i = 0; i < EntityBucket::NUM_BUCKETS; i++)
	{
		// get renderables
		entity_map[_s].GetRenderables(renderList, (EntityBucket)i);
	}


	// return
	return renderList;
}

Entity3D& EntityManager::GetObjectInstance(std::string objectName)
{
	return *m_mpInstanceContainer[objectName];
}
