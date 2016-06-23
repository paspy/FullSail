#pragma once
#include "IController.h"
class Ship;
class SphereCollider;
class Collider;
class Port;

class PlayerController : public IController
{
	enum InputCommands	{ FORWARD = 0, BACKWARDS, TURN_LEFT, TURN_RIGHT, SHOOT_LEFTSIDE, SHOOT_L9, SHOOT_RIGHTSIDE, SHOOT_H, AMMO_ITER_L, AMMO_ITER_R, BOARD, PORT, SPYGLASS, NUM_COMMANDS };
public:
	PlayerController(Ship* _ps);
	~PlayerController();

	void			Update				(float dt);
	const int GetType(void) final				{ return PLAYER; }

	//void			Input_Start			(void);
	//void			Input_End			(void);
	void StopColliders();
	void StartColliders();
	Keyboard::Keys/*unsigned char*/	GetKeyboardControls	(InputCommands ic) const { return ctrls[ic]; }


	void			Lock		(void)	{ locked = true; }
	void			Unlock		(void)	{ locked = false; }
	void			ResetHealth(void) { LowHealthIndicate = true; }

private:
	SphereCollider* m_pSpawnCollider = nullptr;

	bool LowHealthIndicate = true;
	bool locked;

	bool inCombat;
	bool nearAgro;
	float combatMeter;
	float singingMeter;
	Port* m_currPort;


	// keyboard keys
	Keyboard::Keys	ctrls[InputCommands::NUM_COMMANDS];		//unsigned char	ctrls[InputCommands::NUM_COMMANDS];


	void		Input_Keyboard	(float dt);
	void		Input_Gamepad	(float dt);


	// Pre-function check
	//bool	OK_to_Continue	(void);


	// Translation
	void	SpeedUp			(float dt);
	void	SlowDown		(float dt);

	// Rotation
	void	RotateLeft		(float dt);
	void	RotateRight		(float dt);

	// Shooting
	void	FireCannons		(float dt,int side);
	void	FireLong9		(float dt);
	void	FireHarpoons	(float dt);
	void	AmmoIterLeft	(float dt);
	void	AmmoIterRight	(float dt);

	// Boarding
	bool	BoardEnemyShip	(float dt);

	// Porting
	bool	DockAtPort		(float dt);

	//******************Feedback*********************//
	bool m_bSpawnMoreEnemies = true;
	void SpawnCollisionReaction(Collider & other);
	void LowHealth();
	void PlayerSounds(float dt);
	void Movement(float dt);
};