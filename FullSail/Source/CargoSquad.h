#pragma once

class Ship;
class Squad;
class SceneManager;
class IScene;
class CargoSquad
{
	SceneManager*				m_SceneManager;
	IScene*						m_CurrentScene;
	std::vector<Ship*>			m_Fleet;
	Squad*						m_SquadController;
	DirectX::XMFLOAT3			m_Destination;
	float						m_DeathTimer;
public:
	CargoSquad(SceneManager* sceneManager, IScene* currentScene, DirectX::XMFLOAT2 spwanPos, unsigned int numOfGuards);
	~CargoSquad();
	
	Ship* GetCargoShip();

	std::vector<Ship*> GetFleet() { return m_Fleet; }

	float GetDeathTimer() { return m_DeathTimer; }


	void Update(float dt);
	//void DeleteDeadShips();

	bool CanReUse();


};

