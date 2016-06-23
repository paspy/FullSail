#include "pch.h"
#include "PauseMenuScene.h"
#include "SceneManager.h"
#include "Game.h"
#include "GameplayScene.h"
#include "GameStatistics.h"
#include "Ship.h"
#include "SideCannonHandler.h"

PauseMenuScene::PauseMenuScene() : IScene() {
    // scene type
    sceneType = SceneType::PAUSE;

    currCursor = (int)PauseItems::RESUME;
    scrollTimer = SCENE_SCROLL_TIME;

}


PauseMenuScene::~PauseMenuScene() {

}


void PauseMenuScene::Enter( void ) {
    // base enter
    IScene::Enter();

    LoadHudList();

    SoundManager* soundManager = ( (CGame*)CGame::GetApplication() )->GetSoundManager();
    soundManager->Play3DSound( AK::EVENTS::PAUSE_ALL );
    currCursor = (int)PauseItems::RESUME;

    m_FadeAmount = 0.0f;
    CGame::GetApplication()->m_pRenderer->SetPostProcessFadeAmount( m_FadeAmount );
}


void PauseMenuScene::Exit( void ) {
    CGame*			game = (CGame*)CGame::GetApplication();
    GameplayScene*	gps = (GameplayScene*)game->GetSceneManager()->GetScene( IScene::SceneType::GAMEPLAY );

    // resume gameplay bgm
    SoundManager* soundManager = ( (CGame*)CGame::GetApplication() )->GetSoundManager();
    soundManager->Play3DSound( AK::EVENTS::RESUME_ALL );
    gps->ResumeSounds();

    ReleaseHudList();

    // base exit
    IScene::Exit();

}


bool PauseMenuScene::Input( void ) {
    CGame* game = (CGame*)CGame::GetApplication();
    return ( GameSetting::GetRef().GamePadIsConnected() ) ? Input_Gamepad() : Input_Keyboard();
}


void PauseMenuScene::Update( float _dt ) {
    // base update
    IScene::Update( _dt );


    // scroll timer
    if ( scrollTimer > 0.0f )
        scrollTimer -= _dt;
    if ( scrollTimer < 0.0f )
        scrollTimer = 0.0f;

    CGame* game = (CGame*)CGame::GetApplication();

    auto& gameStat = GameStatistics::GetRef();

    auto cusorResume = static_cast<G2D::GUITexture*>( m_hudElems["Pause_resume"] );
    auto cusorInstruction = static_cast<G2D::GUITexture*>( m_hudElems["Pause_instruction"] );
    auto cusorOption = static_cast<G2D::GUITexture*>( m_hudElems["Pause_option"] );
    auto cusorExit = static_cast<G2D::GUITexture*>( m_hudElems["Pause_exit"] );

    auto statsRegularShotCount = static_cast<G2D::GUIText*>( m_hudElems["RegularShotCount"] );
    auto statsChainShotCount = static_cast<G2D::GUIText*>( m_hudElems["ChainShotCount"] );
    auto statsFoodShipCount = static_cast<G2D::GUIText*>( m_hudElems["FoodShipCount"] );
    auto statsGoldShipCount = static_cast<G2D::GUIText*>( m_hudElems["GoldShipCount"] );
    auto statsMilitaryShipCount = static_cast<G2D::GUIText*>( m_hudElems["MilitaryShipCount"] );
    auto statsCrateCollectCount = static_cast<G2D::GUIText*>( m_hudElems["CrateCollectCount"] );
    auto statsGoldCollectCount = static_cast<G2D::GUIText*>( m_hudElems["GoldCollectCount"] );
    auto statsFoodCollectCount = static_cast<G2D::GUIText*>( m_hudElems["FoodCollectCount"] );
    auto statsRumCollectCount = static_cast<G2D::GUIText*>( m_hudElems["RumCollectCount"] );
    auto statsBodyCollectCount = static_cast<G2D::GUIText*>( m_hudElems["BodyCollectCount"] );
    auto statsDistanceCount = static_cast<G2D::GUIText*>( m_hudElems["DistanceCount"] );
    auto statsCannonBallCount = static_cast<G2D::GUIText*>( m_hudElems["CannonBallCount"] );
    auto statsBoardingCount = static_cast<G2D::GUIText*>( m_hudElems["BoardingCount"] );

    auto statsCurrCannonLv = static_cast<G2D::GUIText*>( m_hudElems["CurrCannonLv"] );
    auto statsCurrSailLv = static_cast<G2D::GUIText*>( m_hudElems["CurrSailLv"] );
    auto statsCurrHullLv = static_cast<G2D::GUIText*>( m_hudElems["CurrHullLv"] );

    cusorResume->Active( false );
    cusorInstruction->Active( false );
    cusorOption->Active( false );
    cusorExit->Active( false );

    switch ( currCursor ) {
    case RESUME:
        cusorResume->Active( true );
        break;
    case INSTRUNCTION:
        cusorInstruction->Active( true );
        break;
    case OPTIONS:
        cusorOption->Active( true );
        break;
    case EXIT:
        cusorExit->Active( true );
        break;
    default:
        break;
    }



    // update statistics
    if ( gameStat.RegularShotRate() > 0 && gameStat.RegularShotRate() < 10.0f ) {
        m_wss << std::setprecision( 2 ) << gameStat.RegularShotRate() << L"%";
    } else if ( gameStat.RegularShotRate() >= 10.0f && gameStat.RegularShotRate() < 100.0f ) {
        m_wss << std::setprecision( 3 ) << gameStat.RegularShotRate() << L"%";
    } else {
        m_wss << std::setprecision( 4 ) << gameStat.RegularShotRate() << L"%";
    }
    statsRegularShotCount->Text( m_wss.str() );
    m_wss.str( L"" );


    if ( gameStat.ChainShotRate() > 0 && gameStat.ChainShotRate() < 10.0f ) {
        m_wss << std::setprecision( 2 ) << gameStat.ChainShotRate() << L"%";
    } else if ( gameStat.ChainShotRate() >= 10.0f && gameStat.ChainShotRate() < 100.0f ) {
        m_wss << std::setprecision( 3 ) << gameStat.ChainShotRate() << L"%";
    } else {
        m_wss << std::setprecision( 4 ) << gameStat.ChainShotRate() << L"%";
    }
    statsChainShotCount->Text( m_wss.str() );
    m_wss.str( L"" );

    statsFoodShipCount->Text( std::to_wstring( gameStat.FoodShipDestroyed() ) );
    statsGoldShipCount->Text( std::to_wstring( gameStat.GoldShipDestroyed() ) );
    statsMilitaryShipCount->Text( std::to_wstring( gameStat.MilitaryShipDestroyed() ) );

    statsCrateCollectCount->Text( std::to_wstring( gameStat.CrateCollected() ) );
    statsGoldCollectCount->Text( std::to_wstring( gameStat.GoldCollected() ) );
    statsFoodCollectCount->Text( std::to_wstring( gameStat.FoodCollected() ) );
    statsRumCollectCount->Text( std::to_wstring( gameStat.RumCollected() ) );
    statsBodyCollectCount->Text( std::to_wstring( gameStat.BodyCollected() ) );


    if ( gameStat.TravelDistanceInFoot() > 0 && gameStat.TravelDistanceInFoot() < 10.0f ) {
        m_wss << std::setprecision( 2 ) << gameStat.TravelDistanceInFoot();
    } else if ( gameStat.TravelDistanceInFoot() >= 10.0f && gameStat.TravelDistanceInFoot() < 100.0f ) {
        m_wss << std::setprecision( 3 ) << gameStat.TravelDistanceInFoot();
    } else if ( gameStat.TravelDistanceInFoot() >= 100.0f && gameStat.TravelDistanceInFoot() < 1000.0f ) {
        m_wss << std::setprecision( 4 ) << gameStat.TravelDistanceInFoot();
    } else if ( gameStat.TravelDistanceInFoot() >= 1000.0f && gameStat.TravelDistanceInFoot() < 10000.0f ) {
        m_wss << std::setprecision( 5 ) << gameStat.TravelDistanceInFoot();
    } else if ( gameStat.TravelDistanceInFoot() >= 10000.0f && gameStat.TravelDistanceInFoot() < 100000.0f ) {
        m_wss << std::setprecision( 6 ) << gameStat.TravelDistanceInFoot();
    } else if ( gameStat.TravelDistanceInFoot() >= 10000.0f && gameStat.TravelDistanceInFoot() < 1000000.0f ) {
        m_wss << std::setprecision( 7 ) << gameStat.TravelDistanceInFoot();
    } else {
        m_wss << std::setprecision( 8 ) << gameStat.TravelDistanceInFoot();
    }
    statsDistanceCount->Text( m_wss.str() );
    m_wss.str( L"" );

    statsCannonBallCount->Text( std::to_wstring( gameStat.TotalCannonBallFired() ) );
    statsBoardingCount->Text( std::to_wstring( gameStat.TotalBoarding() ) );

    GameplayScene* gps = ( (GameplayScene*)sceneManager->GetScene( IScene::SceneType::GAMEPLAY ) );
    statsCurrCannonLv->Text( std::to_wstring( gps->GetPlayerShip()->GetSideCannonHandler()->GetLevel() ) + L"/5" );
    statsCurrSailLv->Text( std::to_wstring( gps->GetPlayerShip()->GetMastLevel() ) + L"/5" );
    statsCurrHullLv->Text( std::to_wstring( gps->GetPlayerShip()->GetHullLevel() ) + L"/5" );

}


void PauseMenuScene::Render( void ) {
    // base render
    IScene::Render();
    if ( !m_FadeIn ) {
        m_GUIManager.RenderAll();
    }
    // etc....
}

bool PauseMenuScene::Input_Keyboard( void ) {
    auto game = (CGame*)CGame::GetApplication();
    auto kbs = Keyboard::Get().GetState();
    auto kbst = game->GetKeyboardTracker();
    auto sound = game->GetSoundManager();

    /* Input: Esc */
    if ( kbst->IsKeyPressed( Keyboard::Keys::Escape ) == true ) {
        sceneManager->PopScene();
        //currCursor = PauseItems::EXIT;
        return OLD_SCENE;
    }

    /* Input: move cursor down (w/ wrap around) */
    if ( kbst->IsKeyPressed( Keyboard::Keys::Right ) == true || kbst->IsKeyPressed( Keyboard::Keys::D ) == true ) {
        currCursor = ( currCursor + 1 <= PauseItems::EXIT ) ? ( currCursor + 1 ) : PauseItems::RESUME;
        sound->Play3DSound( AK::EVENTS::PLAYCURSOR );
    }

    /* Input: move cursor up  (w/ wrap around) */
    if ( kbst->IsKeyPressed( Keyboard::Keys::Left ) == true || kbst->IsKeyPressed( Keyboard::Keys::A ) == true ) {
        currCursor = ( currCursor - 1 >= PauseItems::RESUME ) ? ( currCursor - 1 ) : PauseItems::EXIT;
        sound->Play3DSound( AK::EVENTS::PLAYCURSOR );
    }


    /* Input: select curr menu option */
    if ( kbst->IsKeyPressed( Keyboard::Keys::Enter ) == true ) {
        sound->PostEvent( AK::EVENTS::PLAYCURSOR );
        // change curr scene
        switch ( currCursor ) {
        case PauseItems::RESUME:
            sceneManager->PopScene();
            return NEW_SCENE;
            break;

        case PauseItems::INSTRUNCTION:
            sceneManager->PushScene( SceneType::INSTRUCTIONS );
            return NEW_SCENE;
            break;

        case PauseItems::OPTIONS:
            sceneManager->PushScene( SceneType::OPTIONS );
            return NEW_SCENE;
            break;

        case PauseItems::EXIT:
            sceneManager->PopScene();
            sceneManager->PopScene();
            return NEW_SCENE;
            break;

        case PauseItems::UNKNOWN:
        case PauseItems::NUM_CHOICES:
        default:
            break;
        }
    }



    // stay in same scene
    return OLD_SCENE;
}


bool PauseMenuScene::Input_Gamepad( void ) {
    auto game = (CGame*)CGame::GetApplication();
    auto gps = GamePad::Get().GetState( 0 );
    auto gpst = game->GetGamePadTracker();
    auto sound = game->GetSoundManager();


    /* Input: B */
    if ( gpst->b == GamePad::ButtonStateTracker::ButtonState::PRESSED ) {
        sceneManager->PopScene();
        return OLD_SCENE;
    }

    /* Input: move cursor down (w/ wrap around) */
    if ( gps.IsLeftThumbStickRight() == true || gps.IsDPadRightPressed() == true ) {
        if ( scrollTimer <= 0.0f ) {
            scrollTimer = SCENE_SCROLL_TIME;
            currCursor = ( currCursor + 1 <= PauseItems::EXIT ) ? ( currCursor + 1 ) : PauseItems::RESUME;
            sound->Play3DSound( AK::EVENTS::PLAYCURSOR );
        }
    }


    /* Input: move cursor up  (w/ wrap around) */
    if ( gps.IsLeftThumbStickLeft() == true || gps.IsDPadLeftPressed() == true ) {
        if ( scrollTimer <= 0.0f ) {
            scrollTimer = SCENE_SCROLL_TIME;
            currCursor = ( currCursor - 1 >= PauseItems::RESUME ) ? ( currCursor - 1 ) : PauseItems::EXIT;
            sound->Play3DSound( AK::EVENTS::PLAYCURSOR );
        }
    }

    /* Input: A */
    if ( gpst->a == GamePad::ButtonStateTracker::ButtonState::PRESSED ) {
        // change curr scene
        switch ( currCursor ) {
        case PauseItems::RESUME:
            sceneManager->PopScene();
            return NEW_SCENE;
            break;

        case PauseItems::INSTRUNCTION:
            sceneManager->PushScene( SceneType::INSTRUCTIONS );
            return NEW_SCENE;
            break;

        case PauseItems::OPTIONS:
            sceneManager->PushScene( SceneType::OPTIONS );
            return NEW_SCENE;
            break;

        case PauseItems::EXIT:
            sceneManager->PopScene();
            sceneManager->PopScene();
            return NEW_SCENE;
            break;

        case PauseItems::UNKNOWN:
        case PauseItems::NUM_CHOICES:
        default:
            break;
        }
    }


    // stay in same scene
    return OLD_SCENE;
}

void PauseMenuScene::LoadHudList() {
    G2D::CGUIManager::LoadXMLConfig( "../../FullSail/Resources/GUI/Configs/PauseScene.xml", m_hudElems );

    for ( auto it = m_hudElems.begin(); it != m_hudElems.end(); it++ ) {
        m_GUIManager.AddEntity( it->second );
    }
}

void PauseMenuScene::ReleaseHudList() {
    for ( auto it = m_hudElems.begin(); it != m_hudElems.end(); it++ ) {
        m_GUIManager.RemoveEntity( it->second );
        SafeRelease( it->second );
    }
    m_hudElems.clear();
}

