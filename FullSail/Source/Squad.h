#pragma once
class Boid;

class Squad
{

	//Containers
	std::vector<Boid*> m_vSquadmates;
	std::vector<DirectX::XMFLOAT3> m_vWaypoints;
	Boid* m_pLeader = nullptr;

	int m_nCurrentWaypointIndex;


	//*****************************Utility*****************************//
	void UpdateWaypointIndex();

public:
	
	Squad();
	Squad(const Squad& other);
	Squad& operator=(const Squad& other);
	bool operator==(const Squad& other);
	~Squad();

//*****************************Utility*****************************//
	
	Boid* GetLeader();
	float GetRadius();
	DirectX::XMFLOAT3 GetPosition();
	void AddSquadmate(Boid*);
	void SetLeader(Boid* newLeader);
	void RemoveSquadMate(Boid* squadmate);
	void RemoveSquadMate(unsigned int index);
	void AddSquad(std::vector<Boid*>& wholeSquad);

	void AddWaypoint(DirectX::XMFLOAT3& newPoint);
	void AddWaypoints(std::vector<DirectX::XMFLOAT3>& points);
	void RemoveWaypoint(DirectX::XMFLOAT3& waypoint);
	void RemoveWaypoint(unsigned int index);


	bool AdvanceToNextPoint(Boid*);
	std::vector<Boid*>& GetSquad();
	std::vector<DirectX::XMFLOAT3>& GetWaypoints();
	DirectX::XMFLOAT3 GetCurrentWaypoint();
	int GetCurrentWaypointIndex();



	inline void * operator new (size_t size, const char *file, int line){
		return FSMM::MemoryManager::GetInstance()->Allocate(static_cast<unsigned int>(size), 0U, file, line);
	}

		inline void * operator new (size_t size){
		return FSMM::MemoryManager::GetInstance()->Allocate(static_cast<unsigned int>(size));
	}

		inline void * operator new[](size_t size, const char *file, int line) {
		return FSMM::MemoryManager::GetInstance()->Allocate(static_cast<unsigned int>(size), 0U, file, line);
	}

		inline void * operator new[](size_t size) {
		return FSMM::MemoryManager::GetInstance()->Allocate(static_cast<unsigned int>(size));
	}

		inline void operator delete (void * p) {
		FSMM::MemoryManager::GetInstance()->DeAllocate(p);
	}

	inline void operator delete (void * p, const char *file, int line) {
		FSMM::MemoryManager::GetInstance()->DeAllocate(p);
	}

	inline void operator delete[](void * p) {
		FSMM::MemoryManager::GetInstance()->DeAllocate(p);
	}

		inline void operator delete[](void * p, const char *file, int line) {
		FSMM::MemoryManager::GetInstance()->DeAllocate(p);
	}

};

