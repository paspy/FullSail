/***************************************************************
|	File:		MainMenuScene.h
|	Author:		Jason N. Bloomfield
|	Created:	March 7, 2016
|	Modified:	March 18, 2016
|	Purpose:	MainMenuScene class used to store main menu data
***************************************************************/
#pragma once

#include "IScene.h"

namespace G2D
{
    class Entity2D;
}

class Ship;
class Entity3D;

class MainMenuScene : public IScene
{
	friend class SceneManager;

public:
	MainMenuScene();
	~MainMenuScene();


	void		Enter	(void);
	void		Exit	(void);

	bool		Input	(void);
	void		Update	(float _dt);
	void		Render	(void);

	//SceneType	GetType	(void)	{ return SceneType::MAIN_MENU; }

	void		Input_Start	(void);
	void		Input_End	(void);

	bool		Input_Keyboard	(void);
	bool		Input_Gamepad	(void);

    void        LoadHudList();
    void        ReleaseHudList();

private:

	// Main Menu selection
	enum MenuItems	{ UNKNOWN = -1, GAMEPLAY = 0, INSTRUCTIONS, CREDITS, OPTIONS, EXIT, NUM_CHOICES };
	int				currCursor	= 0;
	float			scrollTimer	= 0.0f;
	float			switchCameraTargetTimer = 0.0f;

    // GUI elements
    GUIMap m_hudElems;

	
	Entity3D*					water = nullptr;
	//Actors
	std::vector<Ship*>			aiShips;
	std::vector<DirectX::XMFLOAT3> aiWaypoints;

};
