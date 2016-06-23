#include "pch.h"
#include "WaypointSystem.h"

//Static Initalization

/*static*/ WaypointSystem WaypointSystem::m_Instance;



#pragma region PATH_ID FUNCTIONS

const DirectX::XMFLOAT3 & PathID::GetStartPoint()
{
	// TODO: insert return statement here
	return m_vtStartPoint;
}

const DirectX::XMFLOAT3 & PathID::GetEndPoint()
{
	// TODO: insert return statement here
	return m_vtEndPoint;
}

#pragma endregion

#pragma region PATHNODE FUNCTIONS

const DirectX::XMFLOAT3& PathNode::GetPosition()
{
	return m_vtPosition;
}

const std::vector<std::pair<PathNode*, float>>& PathNode::GetNeighbors()
{
	for (auto&& neighbor : m_vListOfNeighbors)
	{
		neighbor.first->parent = this;
	}
	return m_vListOfNeighbors;
}

#pragma endregion

#pragma region WAYPOINT_SYSTEM FUNCTIONS

WaypointSystem::WaypointSystem()
{

}

WaypointSystem::~WaypointSystem()
{

}

WaypointSystem& WaypointSystem::GetInstance()
{
	return m_Instance;
}

std::vector<PathNode*> WaypointSystem::GetPath(PathNode * _destination, PathNode * _current)
{
	std::vector<PathNode*> rtn;
	std::list<PathNode*> open;
	std::list<PathNode*> closed;
	_current->cost = 0;
	_current->given = 0;
	_current->heuristic = sqrDistanceBetween(_current->GetPosition(), _destination->GetPosition());
	open.push_back(_current);

	while (!open.empty())
	{
		PathNode* next;
		float lowest = FLT_MAX;
		for (auto iter = open.begin(); iter != open.end(); ++iter)
		{
			if ((*iter)->cost < lowest)
				next = *iter;
		}
		open.remove(next);
		const std::vector<std::pair<PathNode*, float>>& neighbors = _current->GetNeighbors();
		for (auto&& neighbor : neighbors)
		{
			if (neighbor.first == _destination) {
				rtn.push_back(_destination);
				PathNode* cur = neighbor.first->parent;
				while (cur != nullptr)
				{
					rtn.push_back(cur);
					cur = neighbor.first->parent;
				}
				return rtn;
			}
			neighbor.first->given = next->given + neighbor.second;
			neighbor.first->heuristic = sqrDistanceBetween(neighbor.first->GetPosition(), _destination->GetPosition());
			neighbor.first->cost = neighbor.first->given + neighbor.first->heuristic;

			bool skip = false;
			for (auto&& node : open)
			{
				if (node->parent == neighbor.first->parent && node->cost < neighbor.first->cost) {
					skip = true;
					break;
				}
			}
			if (skip) {//a node with the same position as neighbor is in the OPEN list which has a lower f than neighbor, skip this neighbor
				continue;
			}
			for (auto&& node : closed)
			{
				if (node->parent == neighbor.first->parent && node->cost < neighbor.first->cost) {
					skip = true;
					break;
				}
			}
			if (skip) {//a node with the same position as neighbor is in the CLOSED list which has a lower f than neighbor, skip this neighbor
				continue;
			}
			else {//, add the node to the open list
				open.push_back(neighbor.first);
			}
		}
		//push q on the closed list
		closed.push_back(next);
	}
	return rtn;
}

void WaypointSystem::AddWaypoints(std::pair<XMFLOAT3, std::vector<XMFLOAT3*>> * _waypoints, unsigned int _howMany = 1)
{
	m_vWaypoints.reserve(m_vWaypoints.size() + _howMany);
	for (size_t i = 0; i < _howMany; i++)
	{
		unsigned long long ID = (unsigned int)_waypoints[i].first.x;
		ID <<= 4;
		ID += (unsigned int)_waypoints[i].first.z;
		m_vWaypoints.insert({ ID, PathNode(_waypoints[i].first) });
	}
	for (size_t i = 0; i < _howMany; i++)
	{
		unsigned long long ID = (unsigned int)_waypoints[i].first.x;
		ID <<= 4;
		ID += (unsigned int)_waypoints[i].first.z;
		for (size_t j = 0; j < _waypoints[i].second.size(); ++j) {
			unsigned long long ID2 = (unsigned int)(_waypoints[i].second[j]->x);
			ID2 <<= 4;
			ID2 += (unsigned int)(_waypoints[i].second[j]->z);
			m_vWaypoints[ID].AddNeighbor(&m_vWaypoints.at(ID2));
		}
	}
}
#pragma endregion
