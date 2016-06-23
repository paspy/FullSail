#include "pch.h"
#include "TradeRoute.h"
#include "GameplayScene.h"
#include "Game.h"
#include "Squad.h"
#include "Entity3D.h"
#include "Ship.h"
#include "IController.h"
#include "Math.h"

#define GOLDSHIP		"GoldShip"
#define FOODSHIP		"MerchantShip"
#define MILITARYSHIP	"MilitaryShip"
#define INRANGEDISTANCE	3025  //  55 sqrd
#define MAXSPAWNLENGTH  10

TradeRoute TradeRoute::m_cInstance;

#pragma region Routes

/*static*/ float								Route::m_fUniversalheight = 0.9f;
/*static*/ std::vector<Entity3D*>				Route::m_vRouteIndicators;
/*static*/ std::vector<Entity3D*>				Route::m_vRouteMarkers;

Route::Route(const std::vector<DirectX::XMFLOAT3>& RoutePoints, unsigned int shipType, unsigned int routeSize)
	: m_rGameplayScene(*((CGame*)CGame::GetApplication())->GetGameplayScene())
{
	m_nRemovedIndicators = 0;

	m_nRemovedMarkers = 0;

	m_fRouteheight = m_fUniversalheight;

	m_eMyRoute = (eRouteType)((shipType < MAX_ROUTES) ? shipType : MILITARY);

	m_nMaxRouteShipSize = 5;

	m_nMaxSpawnLimit = m_nMaxSpawnShipSize = routeSize;

	m_vRouteShips.reserve(routeSize);

	m_nStartRenderIndex = (unsigned int)m_vRouteIndicators.size();

	SetupRoutes(RoutePoints);
}

Route::Route(const Route& other)
	:m_rGameplayScene(*((CGame*)CGame::GetApplication())->GetGameplayScene())
{
	(*this) = other;
}

Route& Route::operator=(const Route& other)
{
	if (&other == this)
	{
		return *this;
	}
	m_nRemovedIndicators = other.m_nRemovedIndicators;
	m_nRemovedMarkers = other.m_nRemovedMarkers;

	DeleteRoute();

	m_vRoutePoints.clear();

	m_vRoutePoints = other.m_vRoutePoints;

	m_nStartLoopIndex = other.m_nStartLoopIndex;

	m_eMyRoute = other.m_eMyRoute;

	m_nMaxRouteShipSize = other.m_nMaxRouteShipSize;

	m_nMaxSpawnLimit = other.m_nMaxSpawnLimit;

	m_nMaxSpawnShipSize = other.m_nMaxSpawnShipSize;

	m_vRouteShips = other.m_vRouteShips;

	m_vRouteIndicators = other.m_vRouteIndicators;


	return *this;
}

void Route::AdjustRoutesShipSize(int size)
{
	//if the size is less than that of the vector 
	if (size < m_vRouteShips.size())
	{
		//no resizing is taking place
		return;
	}

	//resetting the maximum size of the vector
	m_vRouteShips.resize(size);
}

void Route::CloseRoute()
{

	m_vRouteShips.clear();
}

Entity3D* Route::AddShip(int type)
{
	if (type >= MAX_ROUTES)
	{
		return nullptr;
	}

	std::string ShipName;

	if ((eRouteType)type == eRouteType::MIXED)
	{
		//A random high value number
		type = rand() % 100;

		//since military is not a resource Pool
		type = type % MIXED;
	}

	switch (type)
	{
	case eRouteType::GOLD:
		ShipName = GOLDSHIP;
		break;
	case eRouteType::FOOD:
		ShipName = FOODSHIP;
		break;
	case eRouteType::MILITARY:
		ShipName = MILITARYSHIP;
		break;
	}

	Entity3D* newEntity = (Entity3D*)m_rGameplayScene.CreateAIShip(ShipName.c_str(), m_vRoutePoints[0]);

	if (newEntity == nullptr)
	{
		return nullptr;
	}

	m_vRouteShips.push_back(newEntity);

	return  newEntity;
}

void Route::Update(const float& dt)
{
	UpdateRenderingRoute();

	CullObjects();

	RefillVector();

	UpdateTimer(dt);
}

void Route::UpdateTimer(const float & dt)
{
	static float testing = 0;

	m_fSpawnTimer -= dt;
	m_nMaxSpawnShipSize -= (int)(testing += dt);

	if (testing >= 1)
	{
		testing = 0;
	}
}

void Route::CullObjects()
{
	//grabbing the vector's size
	int vecSize = (int)m_vRouteShips.size();

	//looping through all of the route ships
	for (int index = 0; index < vecSize; index++)
	{
		//checking to see if the index entity is to be deleted
		if (m_vRouteShips[index]->ToBeDeleted() == true)
		{
			//if true erase out of my vector
			m_vRouteShips.erase(m_vRouteShips.begin() + index);

			vecSize--;
		}
	}
}

void Route::RefillVector()
{
	float averageReputation = 0;
	float MaxAverageReputation = 0;

	//if its time to spawn 
	if (m_fSpawnTimer > 0)
	{
		return;
	}

	//check to see if the vector is full
	if (m_nMaxSpawnShipSize <= 0)
	{
		return;
	}

	//check to see if the route can spawn anymore ships
	unsigned int vecSize = (unsigned int)m_vRouteShips.size();
	if (vecSize >= m_nMaxRouteShipSize)
	{
		return;
	}

	const XMFLOAT3& m_vtStartPoint = m_vRoutePoints[0];

	m_fSpawnTimer = MAXSPAWNLENGTH;

	//grabbing the check in view Vector
	std::vector<Entity3D*>& checkView = TradeRoute::TradeRoutes().m_vCheckViewObjects;
	const unsigned int& viewSize = (unsigned int)checkView.size();

	//looping through all viewVector
	for (unsigned int index = 0; index < viewSize; index++)
	{
		Entity3D* object = checkView[index];
		//check to see if the distance is less than InRangeDistance
		if (sqrDistanceBetween(object->GetPosition(), m_vtStartPoint) < INRANGEDISTANCE)
		{
			//do not spawn anything
			return;
		}

		//if this is a ship
		if (object->GetType() == Entity3D::SHIP)
		{
			//add the reputation Up
			averageReputation += ((Ship*)object)->GetCurrReputation();
			MaxAverageReputation += ((Ship*)object)->GetMaxReputation();
		}
	}

	//averaging the reputation
	if (viewSize > 1)
	{
		averageReputation /= viewSize;
		MaxAverageReputation /= viewSize;
	}

	//reducing the max to 60% 
	MaxAverageReputation *= 0.60f;

	//Creating the new squadron
	Squad RouteSquad;
	RouteSquad.AddWaypoints(m_vRoutePoints);


	Squad newSquad = RouteSquad;

	float repRatio = averageReputation / MaxAverageReputation;
	unsigned int squadSize = 1;

	//loop through the vector 
	int SpawnMilitary = (int)repRatio * 100;
	Ship* newShip = nullptr;
	int SpawnUnit = (int)m_eMyRoute;

	for (unsigned int index = 0; vecSize < m_nMaxRouteShipSize;)
	{

		//Adding the ship to the vector
		newShip = (Ship*)AddShip(SpawnUnit);
		m_nMaxSpawnShipSize--;


		//If we run out of memory
		if (newShip == nullptr)
		{
			//and our squad is not empty 
			if (newSquad.GetSquad().empty() == false)
			{
				//sending the event
				fsEventManager::GetInstance()->FireEvent(fsEvents::AddSquad, &EventArgs1<Squad*>(&newSquad));
			}
			return;
		}

		// this route is gold
		if (m_eMyRoute == GOLD)
		{
			//assign back up
			Ship* Backup = (Ship*)AddShip(MILITARY);
			//add to route
			newSquad.AddSquadmate(Backup->GetController()->GetBoid());
			//increase index
			index++;
		}

		//Increasing the size
		vecSize++;

		//if the amount of enemies being creating excceds the squadsize
		//adding the new ship to the squad
		newSquad.AddSquadmate(newShip->GetController()->GetBoid());
		index++;

		if (index >= squadSize || vecSize >= m_nMaxRouteShipSize)
		{
			//sending the event
			fsEventManager::GetInstance()->FireEvent(fsEvents::AddSquad, &EventArgs1<Squad*>(&newSquad));
			//reseting the index and squad
			index = 0;
			newSquad = RouteSquad;
			return;
		}

		//Chance for the military to spawn
		if ((rand() % 100) + 1 < SpawnMilitary)
		{
			SpawnUnit = (int)MILITARY;
			SpawnMilitary -= 100;
			continue;
		}

		//getting the routes ship type
		SpawnUnit = (int)m_eMyRoute;
	}

	if (newSquad.GetSquad().empty() == false)
	{
		//sending the event
		fsEventManager::GetInstance()->FireEvent(fsEvents::AddSquad, &EventArgs1<Squad*>(&newSquad));
	}

}

void Route::ChangeRoutes(int Routeindex)
{
	if (m_rGameplayScene.SpyglassActive())
	{
		return;
	}

	////grabbing a reference
	//std::vector<std::vector<DirectX::XMFLOAT3>>& destinationPoints = TradeRoute::TradeRoutes().m_vWaypoints;

	////grabbing the size
	//const int& TradeRouteMax = (int)destinationPoints.size();

	////make sure they are not equal to one another
	//while (Routeindex > TradeRouteMax || Routeindex < 0)
	//{
	//	Routeindex = ((rand() % 100) % TradeRouteMax);
	//}

	//m_vRoutePoints.clear();

	//SetupRoutes(destinationPoints[Routeindex]);
	//resetting spawn limit
	m_nMaxSpawnShipSize = m_nMaxSpawnLimit;
	m_fSpawnTimer = 0;
}

void Route::SetupRoutes(const std::vector<DirectX::XMFLOAT3>& RoutePoints)
{
	m_vRoutePoints = RoutePoints;
	int RouteSize = (int)m_vRoutePoints.size();

	//Creating the lines for the Route
	XMFLOAT4X4 Identity;
	SetToIdentity(Identity);

	std::string routeColor;
	std::string routeMaterial;

	switch (m_eMyRoute)
	{
	case GOLD:
		routeColor = "GoldRoute.dds";
		routeMaterial = "GoldShipRouteMaterial.xml";
		break;
	case FOOD:
		routeColor = "FoodRoute.dds";
		routeMaterial = "FoodShipRouteMaterial.xml";
		break;
	case MIXED:
		routeColor = "MixedRoute.dds";
		routeMaterial = "MixedShipRouteMaterial.xml";
		break;
	case MILITARY:
		routeColor = "testShipTexture.dds";
		break;
	}

	//Creating more Indicators if nesscary


	//minusing plus one so it doesnt it the the maxSize of the vector later
	// and you only need routes for every two sets of waypoints


	Entity3D* Indicator = nullptr;

	RouteSize -= 1;

	for (int indicatorIndex = 0; indicatorIndex < RouteSize; ++indicatorIndex)
	{
		Indicator = newx Entity3D(Identity, "TradeRoute.mesh", routeColor.c_str());
		if (RenderingRouteLineSetup(*Indicator, routeColor, routeMaterial, indicatorIndex, indicatorIndex + 1) == false)
		{
			delete Indicator;
			Indicator = nullptr;
			continue;
		}
		m_vRouteIndicators.push_back(Indicator);
	}
	Indicator = nullptr;


	//Looping the waypoints
	m_nStartLoopIndex = (unsigned int)m_vRoutePoints.size();
	for (unsigned int i = m_nStartLoopIndex - 2; i > 0; --i)
	{
		m_vRoutePoints.push_back(m_vRoutePoints[i]);
	}



	//setting the marker to null
	Entity3D* Marker = nullptr;

	//looping through all of the markers setting the there position equal to the index of the routepoints
	for (unsigned int indicatorIndex = 0; indicatorIndex < m_nStartLoopIndex; ++indicatorIndex)
	{
		Marker = newx Entity3D(Identity, "Waypoint.mesh", routeColor.c_str());
		if (SetUpPointMarkers(*Marker, m_vRoutePoints[indicatorIndex], routeColor, routeMaterial) == false)
		{
			delete Marker;
			Marker = nullptr;
			continue;
		}
		m_vRouteMarkers.push_back(Marker);
	}

	Marker = nullptr;



}

bool Route::RenderingRouteLineSetup(Entity3D& m_pRouteIndicator, const std::string& routeColor, const std::string& routeMaterial, const unsigned int& start, const unsigned int& end)
{
	if (SetUpRenderRoute(m_pRouteIndicator, start, end) == false)
	{
		return false;
	}

	m_pRouteIndicator.SetupRenderData(routeMaterial.c_str(), 0, 1, routeColor.c_str());

	m_rGameplayScene.GetSceneManager()->GetEntityManager().AddEntity(IScene::GAMEPLAY, EntityManager::RENDERABLE, &m_pRouteIndicator);

	std::vector<Renderer::CRenderable*> renderList;

	renderList.push_back(m_pRouteIndicator.GetRenderInfo());

	CGame::GetApplication()->m_pRenderer->SetUpRenderables(renderList);

	return true;
}

bool Route::SetUpPointMarkers(Entity3D& m_pRouteIndicator, const DirectX::XMFLOAT3& position, const std::string& routeColor, const std::string& routeMaterial)
{
	const XMFLOAT4X4& FirstTransform = m_vRouteIndicators[0]->GetTransform().GetLocalMatrix();

	XMFLOAT4X4 newMatrix;
	SetToIdentity(newMatrix);

	newMatrix._11 = 1;
	newMatrix._22 = 1;
	newMatrix._33 = 1;

	//placing the position into the matrix
	memcpy_s(&newMatrix._41, sizeof(newMatrix), &position, sizeof(position));

	newMatrix._42 = -0.9f;


	m_pRouteIndicator.SetupRenderData(routeMaterial.c_str(), 0, 1, routeColor.c_str());

	//setting up the Renderable object
	const unsigned int& Size = (unsigned int)m_vRouteMarkers.size();
	XMFLOAT3 markerPos = positionOf(newMatrix);

	for (unsigned int i = 0; i < Size; i++)
	{
		Entity3D* Marker = m_vRouteMarkers[i];
		if (Marker->GetPosition() == markerPos)
		{
			if (strcmp(Marker->GetTextureListName(), routeColor.c_str()) != 0)
			{
				Marker->SetupRenderData("MixedShipRouteMaterial.xml", 0, 1, "MixedRoute.dds");
			}
			return false;
		}
	}

	m_pRouteIndicator.GetTransform().SetLocalMatrix(newMatrix);


	//tell the gameplayscene that I need to be a renderable
	m_rGameplayScene.GetSceneManager()->GetEntityManager().AddEntity(IScene::GAMEPLAY, EntityManager::RENDERABLE, &m_pRouteIndicator);

	std::vector<Renderer::CRenderable*> renderList;
	//updating the renderable list
	renderList.push_back(m_pRouteIndicator.GetRenderInfo());

	CGame::GetApplication()->m_pRenderer->SetUpRenderables(renderList);

	return true;
}

bool Route::SetUpRenderRoute(Entity3D& m_pRouteIndicator, const unsigned int& start, const unsigned int& end)
{
	XMFLOAT4X4 Identity;
	SetToIdentity(Identity);

	m_pRouteIndicator.GetLocalMatrix() = Identity;

	XMFLOAT3& m_vtEndPoint = m_vRoutePoints[end];
	XMFLOAT3& m_vtStartPoint = m_vRoutePoints[start];

	//finding the vector to the endpoint
	XMFLOAT3 Centerpoint = (m_vtEndPoint + m_vtStartPoint) *0.5f;
	Centerpoint.y = m_fRouteheight;
	XMFLOAT3  toEndPoint = m_vtEndPoint - m_vtStartPoint;
	//halving it
	float Distance = magnitudeOf(toEndPoint);
	//Applying a direction to a point

	//getting the dot between the Right of the quad and the vector to the endpoint
	float3 normToEndPoint = toEndPoint;
	normalizeFloat3(normToEndPoint);

	//getting the angle between the my right and the normalized direction towards my endpoint
	XMFLOAT3 angleOfRotation;
	XMStoreFloat3(&angleOfRotation, XMVector3AngleBetweenNormals(XMLoadFloat3(&VectorZ), XMLoadFloat3(&normToEndPoint)));

	//Rotating the vector 90 degrees
	normToEndPoint.y = normToEndPoint.z;
	normToEndPoint.z = normToEndPoint.x;
	normToEndPoint.x = normToEndPoint.y;
	normToEndPoint.y = 0;
	normToEndPoint.x = -normToEndPoint.x;

	XMFLOAT4X4& RouteLocalMatrix = m_pRouteIndicator.GetTransform().GetLocalMatrix();

	//Changing my right
	memcpy(&RouteLocalMatrix._11, &normToEndPoint, sizeof(normToEndPoint));
	//Changing my forward
	memcpy(&RouteLocalMatrix._31, &toEndPoint, sizeof(toEndPoint));
	//Changing my position
	memcpy(&RouteLocalMatrix._41, &Centerpoint, sizeof(Centerpoint));



	//setting up the Renderable object
	const unsigned int& Size = (unsigned int)m_vRouteIndicators.size();
	for (unsigned int i = 0; i < Size; i++)
	{
		Entity3D* Marker = m_vRouteIndicators[i];
		if (Marker->GetPosition() == Centerpoint)
		{
			if (strcmp(Marker->GetTextureListName(), m_pRouteIndicator.GetTextureListName()) != 0)
			{
				Marker->SetupRenderData("MixedShipRouteMaterial.xml", 0, 1, "MixedRoute.dds");
				m_pRouteIndicator.SetupRenderData("MixedShipRouteMaterial.xml", 0, 1, "MixedRoute.dds");
			}
			m_nRemovedIndicators++;
		}
	}

	m_pRouteIndicator.GetTransform().DirtyTransform();
	return true;
}

void Route::DeleteRoute()
{
	//grabbing the routeSize
	const unsigned int& RouteSize = (unsigned int)m_vRouteIndicators.size();

	//Deleting all of the indicators
	for (unsigned int i = 0; i < RouteSize; i++)
	{
		SafeDelete(m_vRouteIndicators[i]);
	}
	//clearing the vector
	m_vRouteIndicators.clear();

	//increasing the size of the 

	const unsigned int& MarkerSize = (unsigned int)m_vRouteMarkers.size();

	for (unsigned int i = 0; i < MarkerSize; i++)
	{
		SafeDelete(m_vRouteMarkers[i]);
	}

	//clearing the vector
	m_vRouteMarkers.clear();
}

void Route::UpdateRenderingRoute()
{
	//Takes out obejcts that are to be deleted
	bool Render = false;

	//Check to see if the gameplay scene has the Spyglass active
	if (m_rGameplayScene.SpyglassActive())
	{
		Render = true;
	}

	//max of the indicator container
	const unsigned int& RouteIndicatorSize = (unsigned int)m_vRouteIndicators.size();

	//RouteIndicatorSize >>= 1;

	//RouteIndicatorSize -= m_nRemovedIndicators;
	//loop through all of them
	Entity3D* Indicator = nullptr;
	for (unsigned int i = 0; i < RouteIndicatorSize; i++)
	{
		Indicator = m_vRouteIndicators[i];

		//Set all of the indicators to show or not
		Indicator->GetRenderInfo()->SetVisible(Render);
	}

	Indicator = nullptr;

	const unsigned int& indicatorMaxSize = (unsigned int)m_vRouteMarkers.size();


	for (unsigned int i = 0; i < indicatorMaxSize; i++)
	{
		Indicator = m_vRouteMarkers[i];


		//Set all of the Markers to show or not
		Indicator->GetRenderInfo()->SetVisible(Render);
	}

	Indicator = nullptr;
}

Route::~Route()
{
	DeleteRoute();
	m_vRoutePoints.clear();
}

#pragma endregion

TradeRoute::TradeRoute()
{

}

TradeRoute & TradeRoute::TradeRoutes()
{
	return m_cInstance;
}

void TradeRoute::OpenNewRoute(const std::vector<DirectX::XMFLOAT3>& route, unsigned int RouteType, unsigned int maxRouteSize)
{
	m_vRoutes.push_back(new Route(route, RouteType, maxRouteSize));
	m_vWaypoints.push_back(route);
}

void TradeRoute::SetupRoutes(const std::vector<std::vector<DirectX::XMFLOAT3>>& waypoint)
{
	const unsigned int&  waypointSize = (UINT)waypoint.size();

	for (unsigned int i = 0; i < waypointSize; i++)
	{
		const  int waypointIndex = (rand() % 100) % waypointSize;

		//Creating the Route
		m_vRoutes.push_back(newx Route(waypoint[waypointIndex], rand() % 3));
	}
}

void TradeRoute::AddWaypoints(const std::vector<DirectX::XMFLOAT3>& points)
{
	m_vWaypoints.push_back(points);
}

void TradeRoute::UpdateRoutes(const float & dt)
{
	unsigned int  RouteSize = (UINT)m_vRoutes.size();

	for (unsigned int index = 0; index < RouteSize; index++)
	{
		m_vRoutes[index]->Update(dt);
	}
}

void TradeRoute::CloseAllRoutes()
{
	unsigned int RouteSize = (UINT)m_vRoutes.size();

	for (unsigned int i = 0; i < RouteSize; i++)
	{
		m_vRoutes[i]->CloseRoute();
		SafeDelete(m_vRoutes[i]);
	}

	m_vRoutes.clear();
	m_vWaypoints.clear();
	Route::m_fUniversalheight = 0.001f;
}

TradeRoute::~TradeRoute()
{
	CloseAllRoutes();
}
