#pragma once

class Entity3D;
class Boid;
class Squad;
class CSteeringManger;
typedef DirectX::XMFLOAT3(CSteeringManger::*SteeringFunction)(Boid*);

class Boid
{
	//the object a Max speed for me to look at
	float* m_pMaxSpeed = nullptr;
	
	//these are pointers the Steeringmanger handles
	Boid* m_pLeader = nullptr;
	Entity3D* m_pOwner = nullptr;

	Squad* m_pSquad = nullptr;
	std::vector<Boid*>* m_vSquad = nullptr;
	
	Boid* m_pBoidTarget = nullptr;
	DirectX::XMFLOAT3 m_vtTarget;
	bool m_bGuidance = false;

	//**********************Weights*************************//
	
	float m_fLeaderCohesionWeight = 10;
	float m_fLeaderAlignmentWeight = 10;
	float m_fLeaderSeperationWeight = 10;
	
	float m_fSquadCohesionWeight = 10;
	float m_fSquadAlignmentWeight = 10;
	float m_fSquadSeperationWeight = 10;
	
	float m_fPursuitWeight = 10;
	float m_fEvadeWeight = 10;
	float m_fFleeWeight = 10;
	float m_fSeekWeight = 10;

	float m_fFlockRadius = 5;
	float m_fMaxCombinedRadius = 16;

	float m_fStartingLeaderCohesionWeight = 10;
	float m_fStartingLeaderAlignmentWeight = 10;
	float m_fStartingLeaderSeperationWeight = 10;

	float m_fStartingSquadCohesionWeight = 10;
	float m_fStartingSquadAlignmentWeight = 10;
	float m_fStartingSquadSeperationWeight = 10;

	float m_fStartingPursuitWeight = 10;
	float m_fStartingEvadeWeight = 10;
	float m_fStartingFleeWeight = 10;
	float m_fStartingSeekWeight = 10;

	float m_fStartingFlockRadius = 5;
	float m_fStartingMaxCombinedRadius = 16;

	float m_fStartingGlobalSeperation = 10, m_fGlobalSeperationWieght = 10;
	//Averages
	DirectX::XMFLOAT3 m_vtAveragePosition;
	DirectX::XMFLOAT3 m_vtAverageForward;


	//************************Helper Functions************************//
	void CalculateAverages();
	void SquadCommands();
public:

	Boid();
	//**********************Utility*************************//

	DirectX::XMFLOAT3 Update();
	DirectX::XMFLOAT3 RunSteeringFunctions();
	DirectX::XMFLOAT3 FindingAveragePosition();
	bool AtWaypoint();
	void EnableGuidance(bool guide);

	//**********************Accessor*************************//
	std::vector<SteeringFunction>& GetSteeringVector();
	Entity3D* GetOwner();
	const DirectX::XMFLOAT3& GetTarget();
	const Boid*		GetLeader();
	const std::vector<Boid*>* GetSquadVector() { return m_vSquad; };
	Squad* GetSquad() { return m_pSquad; }
	//Boid info
	DirectX::XMFLOAT3& GetAverageForward();
	DirectX::XMFLOAT3& GetAveragePosition();

	inline void	SetFollowingCohesion(float value) { m_fLeaderCohesionWeight = (value > 0) ? value : 0; }
	inline void	SetFollowingAlignment(float value) { m_fLeaderAlignmentWeight = (value > 0) ? value : 0; }
	inline void	SetFollowingSeperation(float value) { m_fLeaderSeperationWeight = (value > 0) ? value : 0; }
	
	inline void	SetSquadCohesion(float value)	{ m_fSquadCohesionWeight = (value > 0) ? value : 0; }
	inline void	SetSquadAlignment(float value)	{ m_fSquadAlignmentWeight = (value > 0) ? value : 0; }
	inline void	SetSquadSeperation(float value) { m_fSquadSeperationWeight = (value > 0) ? value : 0; }

	inline void	SetPursuitWeight(float value) { m_fPursuitWeight = (value > 0) ? value : 0; }
	inline void	SetEvadeWeight(float value) { m_fEvadeWeight = (value > 0) ? value : 0; }
	inline void	SetFleeWeight(float value) { m_fFleeWeight = (value > 0) ? value : 0; }
	inline void	SetSeekWeight(float value) { m_fSeekWeight = (value > 0) ? value : 0; }
	inline void SetGlobalSeperation(float value) { m_fGlobalSeperationWieght = (value > 0) ? value : 0; }
	inline void	SetRadius(float value) { m_fFlockRadius = (value > 0) ? value : 0; }
	inline void	SetMaxCombinedRadius(float value) { m_fFlockRadius = (value > 0) ? value : 0; }
	void SetTargetBoid(Boid* newTarget);

	Boid*  GetTargetBoid();
	bool&  GetGuidance();
	float& GetFlockRadius();
	float& GetMaxCombinedRadius();
	float* GetMaxSpeed();
	//Weights
	float& GetLeaderAlignment();
	float& GetSquadAlignment();

	float& GetLeaderCohesion();
	float& GetSquadCohesion();
	
	float& GetLeaderSeperation();
	float& GetSquadSeperation();
	
	float& GetPursuitWeight();
	float& GetEvadeWeight();
	float& GetFleeWeight();
	float& GetSeekWeight();
	float& GetGlobalSeperation();
	void ResetWeights();
	//Owner Info
	DirectX::XMFLOAT3 GetPosition();
	DirectX::XMFLOAT3 GetForward();
	DirectX::XMFLOAT3 GetRight();

	//**********************Mutator**************************//
	void SetLeader(Boid* newLeader);
	void SetOwner(Entity3D* newOwner);
	void SetTarget(DirectX::XMFLOAT3 newTarget);
	void SetSquad(Squad* squadmates);

	//**********************Destructor**************************//
	virtual ~Boid();
protected:
	std::vector<SteeringFunction> m_vSteeringFunctions;
};


