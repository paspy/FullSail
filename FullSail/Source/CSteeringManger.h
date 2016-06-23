#pragma once

class Boid;
class Squad;

class CSteeringManger
{

private:

	CSteeringManger(const CSteeringManger& other) = delete;
	CSteeringManger& operator=(const CSteeringManger& other) = delete;

	//************************Variables*****************************//
	std::vector<Boid*> m_vKnownBoids;
	std::map<Boid*, Squad> m_mpLeader;
	std::map<unsigned int, Squad> m_mpSquadron;

	//************************EventHandler**************************//
	void AddToSquad(const EventArgs1<Boid**>& newEvent);
	void AddSquad(const EventArgs1<Squad*>& newEvent);
	void RemoveSquad(const EventArgs1<Squad*>& newEvent);
	void AddFollower(const EventArgs2<Boid*, Boid**>& newEvent);
	void RemoveSquadMate(const EventArgs2<int, Boid*>& newEvent);
	void StopFollowing(const EventArgs2<Boid*, Boid*>& newEvent);
	void AddBoid(const EventArgs1<Boid*>& newEvent);
	void DeleteBoid(const EventArgs1<Boid*>& newEvent);
	
	//************************Flocking******************************//
	DirectX::XMFLOAT3 Cohesion(Boid* _boid);
	DirectX::XMFLOAT3 Seperation(Boid * _boid, const std::vector<Boid*>& boidHolder);
	DirectX::XMFLOAT3 SquadSeperation(Boid * _boid);
	DirectX::XMFLOAT3 Alignment(Boid* _boid);

	//*************************Ultility*****************************//
	void SetUpSquadWaypoints(Squad& newSquad, unsigned int MaxWaypoints);
	static CSteeringManger m_cInstance;

public:

	CSteeringManger();

	static CSteeringManger& SteeringManger();
	
	Squad* GetSquad(Squad& finder);
	
	std::vector<DirectX::XMFLOAT3> m_vWaypointContainer;

	//*************************Waypoint*****************************//
	void SetWaypoint(const std::vector<DirectX::XMFLOAT3>& waypoints);
	void AddWaypoint(const DirectX::XMFLOAT3& waypoint);

	//*************************Ultility*****************************//
	bool RemoveBoid(Boid* _boid);

	bool AddBoid(Boid* _boid);
	
	//Following the leader
	DirectX::XMFLOAT3 FollowingFlocking(Boid* _boid);

	//Squad
	DirectX::XMFLOAT3 SquadronFlocking(Boid* _boid);

	//Others
	DirectX::XMFLOAT3 GlobalSeperation(Boid* _boid);
	DirectX::XMFLOAT3 Pursuit(Boid*_boid);
	DirectX::XMFLOAT3 Evade(Boid* _boid);
	DirectX::XMFLOAT3 Seek(Boid* _boid);

	//Instance
	~CSteeringManger();
};