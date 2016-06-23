//--------------------------------------------------------------------------------
// Written by Justin Murphy
//--------------------------------------------------------------------------------
#include "pch.h"
#include "Game.h"
#include "MainWindow.h"
#include "Log.h"
//--------------------------------------------------------------------------------
bool CGame::m_bShowCursor = true;
bool CGame::m_bKeepCursorPos = true;
//--------------------------------------------------------------------------------
CGame::CGame() : CApplication(), sceneManager(), soundManager()
{
    m_fAccumulatedTime = 0.0f;
	//firstTime = false;
}
//--------------------------------------------------------------------------------
void CGame::SceneInit() {
	// Audio stuff
	soundManager.Initialize();

	// Scene stuff
	sceneManager.Initialize();

	// Keyboard controls
	keyboard		= std::make_unique<Keyboard>();
	keyboardTracker	= std::make_unique<Keyboard::KeyboardStateTracker>();

	// Gamepad stuff
	gamePad					= std::make_unique<GamePad>();
	gamePadTracker			= std::make_unique<GamePad::ButtonStateTracker>();

	auto gp					= gamePad->GetState(0);
	gamePadCurrConnected	= gp.IsConnected();
	gamePadPrevConnected	= gamePadCurrConnected;
	gamePadConnectionStatus	= 0;

    auto& gameSetting = GameSetting::GetRef();

    if ( !gamePadCurrConnected )
        gameSetting.InputDevice( GameSetting::eINDEV_Keyboard );

    soundManager.audioSystem.SetRTCPValue( AK::GAME_PARAMETERS::MUSICVOLUME, (AkRtpcValue)gameSetting.MusicVolume() );
    soundManager.audioSystem.SetRTCPValue( AK::GAME_PARAMETERS::SFXVOLUME, (AkRtpcValue)gameSetting.SfxVolume() );

}
//--------------------------------------------------------------------------------
void CGame::Update( float timeDelta, float totalTime ) {
	
	using namespace DirectX;

	//g_AudioSystem.Update();
	soundManager.Update(timeDelta);

	//if (GetAsyncKeyState(VK_ESCAPE) & 1)
	if (sceneManager.GetSize() == 0)
		PostQuitMessage(0);

    // keyboard input update
    auto kb = keyboard->GetState();
    keyboardTracker->Update( kb );


    // controller input update
    auto gp = gamePad->GetState( 0 );
    gamePadPrevConnected = gamePadCurrConnected;
    gamePadCurrConnected = gp.IsConnected();

    gamePadCurrConnected == true
        ? gamePadTracker->Update( gp )
        : gamePadTracker->Reset();

    gamePadConnectionStatus = TestGamepadConnection();

	// Play game
 //   bool same_scene = sceneManager.Input();
 //   if ( same_scene == OLD_SCENE )
	//{
	//	//sceneManager.Update(timeDelta);
	//}
    //sceneManager.Render();

}

void CGame::FixedUpdate( float timeDelta ) {



    bool same_scene = sceneManager.Input();

    //if ( TimeScale > 0.01f ) {
    //    //=============================================================
    //    // Independent physics timer/buffer
    //    //////////////////////////////////////////////////////////////
    //    m_fAccumulatedTime += timeDelta;

    //    if ( m_fAccumulatedTime < FRAME ) return;

    //    timeDelta = FRAME;

    //    while ( m_fAccumulatedTime >= FRAME ) {
    //        m_fAccumulatedTime -= FRAME;
    //    }
    //    //=============================================================
    //    // Independent physics timer/buffer
    //    //////////////////////////////////////////////////////////////
    //}

    if ( same_scene == OLD_SCENE ) {
        sceneManager.Update( timeDelta);
        sceneManager.Render();
    }

}

//--------------------------------------------------------------------------------
void CGame::StopGame(const CGeneralEventArgs<float>& args) {
	PostQuitMessage(0);
}
//--------------------------------------------------------------------------------
GameplayScene * CGame::GetGameplayScene(void)
{
	return (GameplayScene*)sceneManager.GetScene(IScene::GAMEPLAY);
}
//--------------------------------------------------------------------------------
int CGame::TestGamepadConnection(void)
{
	// Nothing happened to Gamepad connection
	if (gamePadCurrConnected == gamePadPrevConnected)
		return 0;


	int		ctrlr_state	= 0;
	IScene*	currscene	= sceneManager.GetCurrScene();


	// Gamepad was just disconnected
	if (gamePadCurrConnected == false && gamePadPrevConnected == true)
	{
		Log("Input: Gamepad was just disconnected!");
        GameSetting::GetRef().InputDevice( GameSetting::eINDEV_Keyboard );
        ctrlr_state = -1;
	}

	// Gamepad was just reconnected
	if (gamePadCurrConnected == true && gamePadPrevConnected == false)
	{
		Log("Input: Gamepad was just reconnected!");
		ctrlr_state = 1;
	}


	return ctrlr_state;

}
//--------------------------------------------------------------------------------
void CGame::SceneEnd() {
	super::SceneEnd();

	// Scene stuff
	sceneManager.Shutdown();

	// Audio stuff
	soundManager.Shutdown();

}
//--------------------------------------------------------------------------------
void CGame::Pause() { 
	m_bIsActive = false;
	Show_Cursor();
}
//--------------------------------------------------------------------------------
void CGame::UnPause() {
	m_bIsActive = true;
}
//--------------------------------------------------------------------------------
void CGame::DoFrame(float fDeltaTime, float totalTime) {
	//EventManager()->ProcessEvents();

	Update(fDeltaTime, totalTime);
    FixedUpdate( fDeltaTime );
	super::DoFrame(fDeltaTime, totalTime);
}
//--------------------------------------------------------------------------------
void CGame::DoIdleFrame( float timeDelta, float totalTime ) {
	// for debugging continue to run and update the game
#ifdef _DEBUG
	Update(timeDelta,totalTime);
#else
	//g_AudioSystem.Update();
	soundManager.Update(timeDelta);
#endif
}
//--------------------------------------------------------------------------------
CGame::~CGame()
{

}
//--------------------------------------------------------------------------------
void CGame::Show_Cursor() {
	if(!m_bShowCursor) {
		ShowCursor(true);
		m_bShowCursor = true;
		if(m_bKeepCursorPos)
			SetCursorPos(m_InitialCursorPos.x,m_InitialCursorPos.y);
	}
}
//--------------------------------------------------------------------------------
void CGame::Hide_Cursor() {
	if(m_bShowCursor) {
		ShowCursor(false);
		m_bShowCursor = false;
		if(m_bKeepCursorPos)
			GetCursorPos(&m_InitialCursorPos);
	}
}
//--------------------------------------------------------------------------------
CApplication* CreateApplication() {
	return new CGame;
}