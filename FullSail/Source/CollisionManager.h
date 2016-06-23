#pragma once

class Collider;

class CollisionManager
{
private:
	std::vector<Collider*> colliders;
	int removePos;
public:
	CollisionManager();
	~CollisionManager();
	void Update();
	void trackThis(Collider* _collider);
	void stopTracking(Collider* _collider);
};

