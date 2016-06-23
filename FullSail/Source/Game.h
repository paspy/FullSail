//--------------------------------------------------------------------------------
// Game
// Written By: Justin Murphy
//
// Overall game class, this class creates, holds and manipulates 
// all objects and any non framework subsystems
//--------------------------------------------------------------------------------
#ifndef Game_h
#define Game_h
//--------------------------------------------------------------------------------
#include "Application.h"
#include "SceneManager.h"
#include "SoundManager.h"
#include "GameSetting.h"

#define FRAME 1.0f / 60.0f

//--------------------------------------------------------------------------------
class GameplayScene;

class CGame : public CApplication {
public:

	virtual void Pause();
	virtual void UnPause();
	
	void Show_Cursor();
	void Hide_Cursor();

	CGame();
	~CGame();
private:
	typedef CApplication super;
	typedef Keyboard::KeyboardStateTracker	KeyTracker;
	typedef GamePad::ButtonStateTracker		GamepadTracker;

	static bool			m_bShowCursor;
	static bool			m_bKeepCursorPos;
	POINT				m_InitialCursorPos; // used to reset the cursor to the correct position after click is released

	virtual void DoFrame(float timeDelta, float totaltime);
	virtual void DoIdleFrame( float timeDelta, float totaltime );
	virtual void SceneInit();
	virtual void SceneEnd();
	
	void Update(float timeDelta, float totaltime);
    void FixedUpdate( float timeDelta );
	void StopGame(const CGeneralEventArgs<float>& args);

	SceneManager						sceneManager;
	SoundManager						soundManager;
	std::unique_ptr<Keyboard>			keyboard;				//Keyboard*		keyboard;
	std::unique_ptr<KeyTracker>			keyboardTracker;		//KeyTracker*	keyboardTracker;
	std::unique_ptr<GamePad>			gamePad;				//GamePad*		gamepadManager;
	std::unique_ptr<GamepadTracker>		gamePadTracker;			//Tracker*		gamePadTracker;
	bool								gamePadPrevConnected;
	bool								gamePadCurrConnected;
	int									gamePadConnectionStatus;

    float                               m_fAccumulatedTime;
public:

	SceneManager*					GetSceneManager				(void)			{ return &sceneManager; }
	GameplayScene*					GetGameplayScene			(void);
	SoundManager*					GetSoundManager				(void)			{ return &soundManager; }
	KeyTracker*						GetKeyboardTracker			(void)	const	{ return keyboardTracker.get(); }
	GamepadTracker*					GetGamePadTracker			(void)	const	{ return gamePadTracker.get(); }
	bool							GamePadWasConnected			(void)	const	{ return gamePadPrevConnected; }
	bool							GamePadIsConnected			(void)	const	{ return gamePadCurrConnected; }
	int								GetGamePadConnectionStatus	(void)	const	{ return gamePadConnectionStatus; }
	int								TestGamepadConnection		(void);
    bool							FirstTimePlaying            ( void ) const  { return GameSetting::GetRef().FirstTimePlayer(); }
    void							FirstTimePlaying            ( bool val )    { GameSetting::GetRef().FirstTimePlayer( val ); }
};
//--------------------------------------------------------------------------------
#endif // Game_h
//--------------------------------------------------------------------------------