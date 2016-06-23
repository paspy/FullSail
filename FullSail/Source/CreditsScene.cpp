#include "pch.h"
#include "CreditsScene.h"
#include "SceneManager.h"
#include "Game.h"
#include "GUISlider.h"
#include "GameSetting.h"
#include "SoundManager.h"


CreditsScene::CreditsScene() : IScene() {
    // scene type
    sceneType = SceneType::CREDITS;
    m_scrollPos = 0.0f;
    m_fg_y = 0.0f;
}

void CreditsScene::Enter( void ) {
    // base enter
    IScene::Enter();
    LoadHudList();
    m_fg_y = static_cast<G2D::GUITexture*>( m_hudElems["Credit_fg"] )->Size().y;
    m_scrollPos = m_fg_y * 0.25f;

	SoundManager* sound = ((CGame*)CGame::GetApplication())->GetSoundManager();
	sound->Play3DSound(AK::EVENTS::PLAY_CREDITPLAYLIST);
}


void CreditsScene::Exit( void ) {
    ReleaseHudList();
	SoundManager* sound = ((CGame*)CGame::GetApplication())->GetSoundManager();
	sound->Play3DSound(AK::EVENTS::STOP_CREDITPLAYLIST);
    // base exit
    IScene::Exit();

}


bool CreditsScene::Input( void ) {
    return ( GameSetting::GetRef().GamePadIsConnected() ) ? Input_Gamepad() : Input_Keyboard();
}


void CreditsScene::Update( float _dt ) {
    // base update
    IScene::Update( _dt );
    m_scrollPos -= _dt * 100.0f;
    m_hudElems["Credit_fg"]->Position( { 0.0f,  m_scrollPos } );

    if ( m_scrollPos < -m_fg_y ) {
        sceneManager->PopScene();
    }
}


void CreditsScene::Render( void ) {
    // base render
    IScene::Render();

    m_GUIManager.RenderAll();

}

bool CreditsScene::Input_Keyboard( void ) {
    //CInputManager* input_manager = InputManager();
    CGame*							game = (CGame*)CGame::GetApplication();
    auto							kbs = Keyboard::Get().GetState();
    Keyboard::KeyboardStateTracker*	kbst = game->GetKeyboardTracker();

    if ( kbst->IsKeyPressed( Keyboard::Keys::Back ) == true || kbst->IsKeyPressed( Keyboard::Keys::Enter ) == true ) {
        sceneManager->PopScene();
        return NEW_SCENE;
    }

    return OLD_SCENE;
}


bool CreditsScene::Input_Gamepad( void ) {
    CGame*							game = (CGame*)CGame::GetApplication();
    auto							gps = GamePad::Get().GetState( 0 );
    GamePad::ButtonStateTracker*	gpst = game->GetGamePadTracker();

    /* Input: B, start */
    if ( gpst->b == GamePad::ButtonStateTracker::ButtonState::PRESSED || gpst->start == GamePad::ButtonStateTracker::ButtonState::PRESSED ) {
        sceneManager->PopScene();
        return NEW_SCENE;
    }

    return OLD_SCENE;
}

void CreditsScene::LoadHudList() {
    G2D::CGUIManager::LoadXMLConfig( "../../FullSail/Resources/GUI/Configs/CreditScene.xml", m_hudElems );
    for ( auto it = m_hudElems.begin(); it != m_hudElems.end(); it++ ) {
        m_GUIManager.AddEntity( it->second );
    }
}

void CreditsScene::ReleaseHudList() {
    for ( auto it = m_hudElems.begin(); it != m_hudElems.end(); it++ ) {
        m_GUIManager.RemoveEntity( it->second );
        SafeRelease( it->second );
    }
    m_hudElems.clear();
}

