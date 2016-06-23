#include "pch.h"
#include "IntroScene.h"
#include "SceneManager.h"
#include "Game.h"
#include "GameSetting.h"

IntroScene::IntroScene() : IScene() {
    // scene type
    sceneType = SceneType::INTRO;

    m_nCurrPage = 0;
    m_fPageTimer = PAGE_TIME;

}


IntroScene::~IntroScene() {

}


void IntroScene::Enter( void ) {
    // base enter
    IScene::Enter();
    LoadHudList();
    CGame::GetApplication()->m_pRenderer->SetPostProcessFadeAmount( 0.0f );
    ( *m_hudIter ).second->Active( true );
    m_fFadeInColor = 0.0f;
    m_fFadeOutColor = 1.0f;
}


void IntroScene::Exit( void ) {
    ReleaseHudList();
    // base exit
    IScene::Exit();
}


bool IntroScene::Input( void ) {
    CGame* game = (CGame*)CGame::GetApplication();

    // 1st time input exception
    if ( game->FirstTimePlaying() == true )
        return OLD_SCENE;

    return ( GameSetting::GetRef().GamePadIsConnected() ) ? Input_Gamepad() : Input_Keyboard();
}


void IntroScene::Update( float _dt ) {
    static float initBuffer = 0.0f;
 
    // base update
    IScene::Update( _dt );

    if ( _dt >= 1.0f ) {
        initBuffer += _dt*0.01f;
    } else {
        initBuffer += _dt;
    }
    if ( initBuffer <= INIT_TIME ) return;
    m_fFadeInColor += _dt*2.0f;
    
    if ( m_fFadeInColor <= 1.0f ) {
        if ( m_fFadeInColor >= 1.0f ) m_fFadeInColor = m_fFadeOutColor = 1.0f;
        static_cast<G2D::GUITexture*>( ( *m_hudIter ).second )->Color( { m_fFadeInColor,m_fFadeInColor,m_fFadeInColor,m_fFadeInColor } );
        return;
    }
    m_fPageTimer -= _dt;

    // update pages
    PostUpdate(_dt);
    
}


void IntroScene::Render( void ) {
    // base render
    IScene::Render();
    if ( !m_FadeIn ) {
        m_GUIManager.RenderAll();
    }
    // etc....
}

bool IntroScene::Input_Keyboard( void ) {
    //CInputManager* input_manager = InputManager();
    CGame*							game = (CGame*)CGame::GetApplication();
    auto							kbs = Keyboard::Get().GetState();
    Keyboard::KeyboardStateTracker*	kbst = game->GetKeyboardTracker();


    if ( kbst->IsKeyPressed( Keyboard::Keys::Back ) == true || kbst->IsKeyPressed( Keyboard::Keys::Enter ) == true ) {
        // update pages
        m_fPageTimer = 0.0f;
        return PostUpdate(0.0f);
    }

    // stay in same scene
    return OLD_SCENE;
}


bool IntroScene::Input_Gamepad( void ) {
    CGame*							game = (CGame*)CGame::GetApplication();
    auto							gps = GamePad::Get().GetState( 0 );
    GamePad::ButtonStateTracker*	gpst = game->GetGamePadTracker();


    /* Input: X, start */
    if ( gpst->a == GamePad::ButtonStateTracker::ButtonState::PRESSED ||
        gpst->b == GamePad::ButtonStateTracker::ButtonState::PRESSED ||
        gpst->start == GamePad::ButtonStateTracker::ButtonState::PRESSED ) {
        // update pages
        m_fPageTimer = 0.0f;
        return PostUpdate(0.0f);
    }

    // stay in same scene
    return OLD_SCENE;
}


bool IntroScene::PostUpdate( float _dt ) {
    if ( m_fPageTimer > 0.0f ) return OLD_SCENE;

    m_fFadeOutColor -= _dt;
    if ( m_fFadeOutColor >= 0.0f && _dt > 0.0f) {
        if ( m_fFadeOutColor <= 0.0f ) m_fFadeOutColor = 0.0f;
        static_cast<G2D::GUITexture*>( ( *m_hudIter ).second )->Color( { m_fFadeOutColor,m_fFadeOutColor,m_fFadeOutColor,m_fFadeOutColor } );
        return OLD_SCENE;
    }

    m_nCurrPage++;
    ( *m_hudIter ).second->Active( false );
    m_hudIter++;
    m_fFadeInColor = 0.0f;
    m_fFadeOutColor = 1.0f;

    if ( m_nCurrPage >= int( m_hudElems.size() ) ) {
        // remove Intro from stack
        sceneManager->PopScene();

        // add main menu to stack
        sceneManager->PushScene( IScene::SceneType::MAIN_MENU );

        // return
        return NEW_SCENE;
    }

    // reset timer
    m_fPageTimer = PAGE_TIME;

    ( *m_hudIter ).second->Active( true );
    return OLD_SCENE;
}

void IntroScene::LoadHudList() {
    G2D::CGUIManager::LoadXMLConfig( "../../FullSail/Resources/GUI/Configs/IntroScene.xml", m_hudElems );
    m_hudIter = m_hudElems.begin();
    for ( auto it = m_hudElems.begin(); it != m_hudElems.end(); it++ ) {
        m_GUIManager.AddEntity( it->second );
    }

}

void IntroScene::ReleaseHudList() {
    for ( auto it = m_hudElems.begin(); it != m_hudElems.end(); it++ ) {
        m_GUIManager.RemoveEntity( it->second );
        SafeRelease( it->second );
    }
    m_hudElems.clear();
}

