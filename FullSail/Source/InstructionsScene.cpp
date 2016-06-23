#include "pch.h"
#include "InstructionsScene.h"
#include "SceneManager.h"
#include "Game.h"
//#include "InputManager.h"

#define NUM_SLIDES		((int)5)


InstructionsScene::InstructionsScene() : IScene() {
    // scene type
    sceneType = SceneType::INSTRUCTIONS;

    currSlide = 0;

}


InstructionsScene::~InstructionsScene() {

}


void InstructionsScene::Enter( void ) {
    // base enter
    IScene::Enter();
    LoadHudList();
    currCursor = int( INTRODUCTION );

}


void InstructionsScene::Exit( void ) {
    ReleaseHudList();
    // base exit
    IScene::Exit();
}


bool InstructionsScene::Input( void ) {
    CGame* game = (CGame*)CGame::GetApplication();
    return ( GameSetting::GetRef().GamePadIsConnected() ) ? Input_Gamepad() : Input_Keyboard();
}


void InstructionsScene::Update( float _dt ) {
    // base update
    IScene::Update( _dt );

    // scroll timer
    if ( scrollTimer > 0.0f )
        scrollTimer -= _dt;
    if ( scrollTimer < 0.0f )
        scrollTimer = 0.0f;

    auto layerIntro = static_cast<G2D::GUITexture*>( m_hudElems["Instruction_layer_intro"] );
    auto layerCommands = static_cast<G2D::GUITexture*>( m_hudElems["Instruction_layer_commands"] );
    auto layerBattles = static_cast<G2D::GUITexture*>( m_hudElems["Instruction_layer_battles"] );
    auto layerEnemies = static_cast<G2D::GUITexture*>( m_hudElems["Instruction_layer_enemies"] );
    auto layerResources = static_cast<G2D::GUITexture*>( m_hudElems["Instruction_layer_resources"] );
    auto layerBack = static_cast<G2D::GUITexture*>( m_hudElems["Instruction_layer_back"] );

    layerIntro->Active( false );
    layerCommands->Active( false );
    layerBattles->Active( false );
    layerEnemies->Active( false );
    layerResources->Active( false );
    layerBack->Active( false );

    switch ( currCursor ) {
    case INTRODUCTION:
        layerIntro->Active( true );
        break;
    case COMMANDS:
        layerCommands->Active( true );
        break;
    case BATTLES:
        layerBattles->Active( true );
        break;
    case ENEMEIES:
        layerEnemies->Active( true );
        break;
    case RESOURCES:
        layerResources->Active( true );
        break;
    case BACK:
        layerBack->Active( true );
        break;
    default:
        break;
    }
}


void InstructionsScene::Render( void ) {
    // base render
    IScene::Render();
	if (!m_FadeIn)
	{
		m_GUIManager.RenderAll();
	}
    
}


bool InstructionsScene::Input_Keyboard( void ) {
    CGame*							game = (CGame*)CGame::GetApplication();
    auto							kbs = Keyboard::Get().GetState();
    Keyboard::KeyboardStateTracker*	kbst = game->GetKeyboardTracker();
    SoundManager*					sound = game->GetSoundManager();

    if ( kbst->IsKeyPressed( Keyboard::Keys::Back ) == true ) {
        currCursor = InstruItems::BACK;
        sound->Play3DSound( AK::EVENTS::PLAYCURSOR );
    }

    if ( kbst->IsKeyPressed( Keyboard::Keys::Down ) == true || kbst->IsKeyPressed( Keyboard::Keys::NumPad2 ) == true || kbst->IsKeyPressed( Keyboard::Keys::S ) == true ) {
        currCursor = ( currCursor + 1 <= InstruItems::BACK ) ? ( currCursor + 1 ) : InstruItems::INTRODUCTION;
        sound->Play3DSound( AK::EVENTS::PLAYCURSOR );
    }

    if ( kbst->IsKeyPressed( Keyboard::Keys::Up ) == true || kbst->IsKeyPressed( Keyboard::Keys::NumPad8 ) == true || kbst->IsKeyPressed( Keyboard::Keys::W ) == true ) {
        currCursor = ( currCursor - 1 >= InstruItems::INTRODUCTION ) ? ( currCursor - 1 ) : InstruItems::BACK;
        sound->Play3DSound( AK::EVENTS::PLAYCURSOR );
    }


    if ( kbst->IsKeyPressed( Keyboard::Keys::Enter ) == true ) {
        sound->Play3DSound( AK::EVENTS::PLAYCURSOR );

        switch ( currCursor ) {
        case InstruItems::BACK:
            sceneManager->PopScene();
            return NEW_SCENE;
            break;
        case InstruItems::INTRODUCTION:
        case InstruItems::COMMANDS:
        case InstruItems::BATTLES:
        case InstruItems::ENEMEIES:
        case InstruItems::RESOURCES:
        default:
            break;
        }
    }

    // stay in same scene
    return OLD_SCENE;
}


bool InstructionsScene::Input_Gamepad( void ) {
    CGame*							game = (CGame*)CGame::GetApplication();
    auto							gps = GamePad::Get().GetState( 0 );
    GamePad::ButtonStateTracker*	gpst = game->GetGamePadTracker();
    SoundManager*					sound = game->GetSoundManager();

    if ( gpst->b == GamePad::ButtonStateTracker::ButtonState::PRESSED ) {
        currCursor = InstruItems::BACK;
        sound->Play3DSound( AK::EVENTS::PLAYCURSOR );
    }

    if ( gps.IsLeftThumbStickDown() == true || gps.IsDPadDownPressed() == true ) {
        if ( scrollTimer <= 0.0f ) {
            scrollTimer = SCENE_SCROLL_TIME;
            currCursor = ( currCursor + 1 <= InstruItems::BACK ) ? ( currCursor + 1 ) : InstruItems::INTRODUCTION;
            sound->Play3DSound( AK::EVENTS::PLAYCURSOR );
        }
    }

    if ( gps.IsLeftThumbStickUp() == true || gps.IsDPadUpPressed() == true ) {
        if ( scrollTimer <= 0.0f ) {
            scrollTimer = SCENE_SCROLL_TIME;
            currCursor = ( currCursor - 1 >= InstruItems::INTRODUCTION ) ? ( currCursor - 1 ) : InstruItems::BACK;
            sound->Play3DSound( AK::EVENTS::PLAYCURSOR );
        }
    }

    if ( gpst->a == GamePad::ButtonStateTracker::ButtonState::PRESSED ) {

        sound->Play3DSound( AK::EVENTS::PLAYCURSOR );

        switch ( currCursor ) {
        case InstruItems::BACK:
            sceneManager->PopScene();
            return NEW_SCENE;
            break;
        case InstruItems::INTRODUCTION:
        case InstruItems::COMMANDS:
        case InstruItems::BATTLES:
        case InstruItems::ENEMEIES:
        case InstruItems::RESOURCES:
        default:
            break;
        }
    }


    return OLD_SCENE;
}

void InstructionsScene::LoadHudList() {
    G2D::CGUIManager::LoadXMLConfig( "../../FullSail/Resources/GUI/Configs/InstructionsScene.xml", m_hudElems );

    for ( auto it = m_hudElems.begin(); it != m_hudElems.end(); it++ ) {
        m_GUIManager.AddEntity( it->second );
    }
}

void InstructionsScene::ReleaseHudList() {
    for ( auto it = m_hudElems.begin(); it != m_hudElems.end(); it++ ) {
        m_GUIManager.RemoveEntity( it->second );
        SafeRelease( it->second );
    }
    m_hudElems.clear();
}

