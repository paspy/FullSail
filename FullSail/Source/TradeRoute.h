#pragma once

class Ship;
class GameplayScene;
class Entity3D;
class Squad;

enum eRouteType { GOLD, FOOD, MIXED, MILITARY, MAX_ROUTES };

struct Route
{
	Route(const std::vector<DirectX::XMFLOAT3>& RoutePoints, unsigned int shipType, unsigned int routeSize = 35);
	Route(const Route & other);
	Route & operator=(const Route & other);
	~Route();
	/************************************Variables************************************/
	eRouteType			m_eMyRoute;

	GameplayScene&		m_rGameplayScene;

	static float m_fUniversalheight;
	
	float m_fRouteheight = 0;
	unsigned int m_nStartLoopIndex = 0;

	static std::vector<Entity3D*>				m_vRouteIndicators;
	static std::vector<Entity3D*>				m_vRouteMarkers;
	
	std::vector<DirectX::XMFLOAT3>		m_vRoutePoints;

	unsigned int m_nRemovedIndicators = 0;
	unsigned int m_nRemovedMarkers = 0;
	unsigned int m_nStartRenderIndicator = 0;
	unsigned int m_nStartRenderMarker = 0;
	unsigned int m_nStartRenderIndex = 0;

	/************************************Utility************************************/
	const std::vector<Entity3D*>& GetRouteShips() { return m_vRouteShips; }
	void AdjustRoutesShipSize(int size);
	Entity3D* AddShip(int);
	void RefillVector();
	void ChangeRoutes(int Routeindex = -1);
	void CullObjects();
	void CloseRoute();
	void Update(const float& dt);
	void UpdateTimer(const float& dt);
	void DeleteRoute();
	void UpdateRenderingRoute();

#pragma region Allocation and Deleting
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
#pragma endregion

private:

	std::vector<Entity3D*> m_vRouteShips;
	//This is the max amount of ships that a route can have in-game
	unsigned int m_nMaxRouteShipSize;
	//This is the max amount of ships that a route can spawn for the game
	unsigned int m_nMaxSpawnShipSize;
	unsigned int m_nMaxSpawnLimit;
	//This is the amount that the military will step in and start flooding the route
	unsigned int m_nMilitaryIntervention;

	float m_fSpawnTimer = 0;
	bool RenderingRouteLineSetup(Entity3D& m_pRouteIndicator, const std::string& routeColor, const std::string& routeMaterial, const unsigned int& start, const unsigned int& end);
	void SetupRoutes(const std::vector<DirectX::XMFLOAT3>& RoutePoints);
	bool SetUpRenderRoute(Entity3D& m_pRouteIndicator,const unsigned int& start, const unsigned int& end);
	bool SetUpPointMarkers(Entity3D & m_pRouteIndicator, const DirectX::XMFLOAT3 & position, const std::string & routeColor, const std::string & routeMaterial);
};

class TradeRoute
{
	std::vector<Route*> m_vRoutes;

	TradeRoute();
	TradeRoute(const TradeRoute& other) = delete;
	TradeRoute& operator=(const TradeRoute& other) = delete;

	static TradeRoute m_cInstance;
	bool m_bRenderRoutes;

public:

	std::vector<Entity3D*> m_vCheckViewObjects;
	std::vector<std::vector<DirectX::XMFLOAT3>> m_vWaypoints;

	static TradeRoute& TradeRoutes();

	void OpenNewRoute(const std::vector<DirectX::XMFLOAT3>& route, unsigned int RouteType, unsigned int maxRouteSize);

	void SetupRoutes(const std::vector<std::vector<DirectX::XMFLOAT3>>& waypoint);

	const std::vector<Route*>& GetRoutes() { return m_vRoutes; }

	void AddWaypoints(const std::vector<DirectX::XMFLOAT3>& points);

	void UpdateRoutes(const float& dt);

	void CloseAllRoutes();

	virtual ~TradeRoute();

#pragma region Allocation and Deleting
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
#pragma endregion

private:

};

