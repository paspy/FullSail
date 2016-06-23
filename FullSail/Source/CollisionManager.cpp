#include "pch.h"
#include "CollisionManager.h"
#include "CollisionLibrary.h"
#include "Colliders.h"
#include "Game.h"


CollisionManager::CollisionManager()
{
	colliders.reserve(300);
}


CollisionManager::~CollisionManager()
{
	for (int i = (int)(colliders.size() - 1); i > -1; --i)
		delete colliders[i];
}

void CollisionManager::Update()
{
#ifdef _DEBUG
	for (size_t i = 0; i < colliders.size(); i++)
	{
		CGame::GetApplication()->m_pRenderer->SetDebugShapeColor(colliders[i], DirectX::XMFLOAT4{ 0.0f,1.0f,0.0f,0.5f });
	}
#endif //DEBUG
	removePos = -1;
	for (size_t i = 0; i < colliders.size(); i++)
	{
		colliders[i]->rebuildTransform();
		for (size_t j = i + 1; j < colliders.size(); j++)
		{
			if (&colliders[i]->getParent() == &colliders[j]->getParent())
				continue;
			else if (!strcmp(colliders[i]->tag.c_str(), "Island") && !strcmp(colliders[j]->tag.c_str(), "NoIsland"))
				continue;
			else if(!strcmp(colliders[i]->tag.c_str(), "NoIsland") && !strcmp(colliders[j]->tag.c_str(), "Island"))
				continue;
			if (ParseColliders(*colliders[i], *colliders[j]))
			{
#ifdef _DEBUG
				CGame::GetApplication()->m_pRenderer->SetDebugShapeColor(colliders[i], DirectX::XMFLOAT4{ 1.0f,0.0f,0.0f,0.5f });
				CGame::GetApplication()->m_pRenderer->SetDebugShapeColor(colliders[j], DirectX::XMFLOAT4{ 1.0f,0.0f,0.0f,0.5f });
#endif //DEBUG
				colliders[i]->OnCollision(*colliders[j]);

				if (removePos > -1)
				{
					if (i <= colliders.size())
					{
						j = i;
						removePos = -1;
						continue;
					}
				}
				colliders[j]->OnCollision(*colliders[i]);
				if (removePos > -1)
				{
					--j;
					removePos = -1;
				}
			}
		}
	}
}

void CollisionManager::trackThis(Collider* _collider)
{

	if (_collider == nullptr)
	{
		return;
	}

	//grabbing the size
	 unsigned int ContainerSize = (UINT)colliders.size();

	 //Loop through all of the Colliders
	 for (unsigned int ColliderIndex = 0; ColliderIndex < ContainerSize; ColliderIndex++)
	 {
		 //if I find one pointing at the same object
		 if (_collider == colliders[ColliderIndex])
		 {
			 //do not add another one to he vector
			 return;
		 }
	 }

	 //Adding the new Colliders to the vector
	colliders.push_back(_collider);
}

void CollisionManager::stopTracking(Collider* _collider)
{
	if (_collider == nullptr)
	{
		return;
	}

	if (colliders.size() > 0)
	{
		auto it = std::find(colliders.begin(), colliders.end(), _collider);
		removePos = (int)(it - colliders.begin());
		removePos = removePos < colliders.size() ? removePos : -1;
		colliders.erase(std::remove(colliders.begin(), colliders.end(), _collider), colliders.end());
	}
}