#pragma once
#include "Math.h"

struct PathID
{
	const DirectX::XMFLOAT3& GetStartPoint();
	const DirectX::XMFLOAT3& GetEndPoint();

private:
	DirectX::XMFLOAT3 m_vtStartPoint;
	DirectX::XMFLOAT3 m_vtEndPoint;
};

struct PathNode
{
	PathNode(XMFLOAT3& _position) : m_vtPosition(_position) {}
	const DirectX::XMFLOAT3& GetPosition();
	const std::vector<std::pair<PathNode*, float>>& GetNeighbors();
	void AddNeighbor(PathNode* _newNeighbor) { m_vListOfNeighbors.push_back({_newNeighbor, sqrDistanceBetween(m_vtPosition, _newNeighbor->GetPosition())}); }
	float given;
	float cost;
	float heuristic;
	PathNode* parent = nullptr;
	PathNode() {}
private:
	DirectX::XMFLOAT3 m_vtPosition;
	std::vector<std::pair<PathNode*, float>> m_vListOfNeighbors;
	bool visited = false;
};


class WaypointSystem
{
	WaypointSystem();
	WaypointSystem& operator=(const WaypointSystem&) {};
	WaypointSystem(const WaypointSystem&) {};
	~WaypointSystem();
	static WaypointSystem m_Instance;
	std::vector<PathNode*> m_vPiers;
	std::unordered_map<unsigned long long, PathNode> m_vWaypoints;
public:
	static WaypointSystem& GetInstance();
	std::vector<PathNode*> GetPath(PathNode* _destination, PathNode* _current);
	void AddWaypoints(std::pair<XMFLOAT3, std::vector<XMFLOAT3*>> *, unsigned int _howMany);
	const std::vector<PathNode*>& getPiers() { return m_vPiers; }
	std::unordered_map<unsigned long long, PathNode>& GetWaypoints() { return m_vWaypoints; }
};

