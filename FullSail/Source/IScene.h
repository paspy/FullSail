/***************************************************************
|	File:		IScene.h
|	Author:		Jason N. Bloomfield
|	Created:	March 7, 2016
|	Modified:	April 8, 2016
|	Purpose:	IScene class used as an ABC for any kids
***************************************************************/
#pragma once
#include "pch.h"
#include "GUIManager.h"

#define SCENE_SCROLL_TIME	((float)0.22f)
#define SCENE_NOTICE_TIME	((float)4.0f)
#define FADEINTIMER			2.0f

class SceneManager;
class Camera;
class IScene
{
public:
	enum SceneType { UNKNOWN = -1, INTRO = 0, MAIN_MENU, GAMEPLAY, OPTIONS, INSTRUCTIONS, PORT, PAUSE, WIN_LOSE, CREDITS, NUM_SCENES };

	IScene(void);
	virtual	~IScene(void);

	virtual	void		Enter(void);
	virtual	void		Exit(void);

	virtual	bool		Input(void) = 0;
	virtual	void		Update(float _dt);
	virtual	void		Render(void);

	/*virtual*/	SceneType	GetType(void)	const { return sceneType; }

	virtual	void		Input_Start(void) = 0;
	virtual	void		Input_End(void) = 0;

	virtual	bool		Input_Keyboard(void) = 0;
	virtual	bool		Input_Gamepad(void) = 0;

	virtual	void		InitializeScene(void) {}
	virtual	void		ShutdownScene(void) {}

	virtual	void		SwapScenePush(void) {}
	virtual	void		SwapScenePop(void) {}

    virtual void        LoadHudList() = 0;
    virtual void        ReleaseHudList() = 0;

	void				DrawGamepadStatus(std::wstring status);
	// Accessors
	float				GetFadeAmount(void)	const { return m_FadeAmount; }
	SceneManager*		GetSceneManager(void)	const { return sceneManager; }
	Camera*				GetMainCamera(void)	const { return m_MainCamera.get(); }

	// Mutators
	void				SetSceneManager(SceneManager* _sm) { sceneManager = _sm; }


protected:
	bool m_FadeIn;
	bool m_HUD_Off;
	float m_Timer = 3.0f;
	float m_FadeAmount = 0.0f;

	SceneType				        sceneType;
	SceneManager*			        sceneManager;
	G2D::CGUIManager		        m_GUIManager;
	std::unique_ptr<Camera>	        m_MainCamera;
	G2D::Entity2D*					m_pGamepadNotice;
};
