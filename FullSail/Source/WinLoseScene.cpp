#include "pch.h"
#include "WinLoseScene.h"
#include "SceneManager.h"
#include "Game.h"
#include "MainWindow.h"
#include "Camera.h"
#include "GameplayScene.h"
#include "GameStatistics.h"
#include "GameSetting.h"
#include "Ship.h"
#include "SideCannonHandler.h"

const std::string g_ScreenShotPathA( "../../Screenshots/" );
const std::wstring g_ScreenShotPathW( L"../../Screenshots/" );


WinLoseScene::WinLoseScene() : IScene(), m_isVictory( false ) {
    // scene type
    sceneType = SceneType::WIN_LOSE;
    scrollTimer = SCENE_SCROLL_TIME;

}

void WinLoseScene::Enter( void ) {
    // base enter
    IScene::Enter();

    LoadHudList();

    CGame*			game = (CGame*)CGame::GetApplication();
    SoundManager*	sound = game->GetSoundManager();
    if ( m_isVictory ) {
        sound->Play3DSound( AK::EVENTS::PLAYWINNINGSONG );
    } else {
        sound->Play3DSound( AK::EVENTS::PLAYGAMEOVER );
    }

    sound->PostEvent( AK::EVENTS::STOPMAINMENU );

    m_currStage = SHOW_STATS;
    currCursor = SCREEN_SHOT;
    m_wsFinalScore = L"00000000";
    m_screenShotSaved = false;
    m_FadeIn = true;
    m_disableInput = true;
    m_Timer = 5.0f;
    m_FadeAmount = 0.0f;
    CGame::GetApplication()->m_pRenderer->SetPostProcessFadeAmount( m_FadeAmount );
    CGame::GetApplication()->m_pRenderer->SetPostProcessIndex( 0 );
}

void WinLoseScene::Exit( void ) {

    CGame*			game = (CGame*)CGame::GetApplication();
    SoundManager*	sound = game->GetSoundManager();
    sound->Play3DSound( AK::EVENTS::STOPWINNINGSONG );

    ReleaseHudList();

    // base exit
    IScene::Exit();
}

bool WinLoseScene::Input( void ) {
    if ( /*m_FadeIn ||*/ m_disableInput )
        return OLD_SCENE;
    return ( GameSetting::GetRef().GamePadIsConnected() ) ? Input_Gamepad() : Input_Keyboard();
}

void WinLoseScene::Update( float _dt ) {
    static float fadingTimer = 0.0f;
    // base update
    IScene::Update( _dt );

    if ( scrollTimer > 0.0f )
        scrollTimer -= _dt;
    if ( scrollTimer < 0.0f )
        scrollTimer = 0.0f;

    if ( m_FadeIn ) {
        auto winLose_bg = static_cast<G2D::GUITexture*>( m_hudElems["WinLose_bg"] );
        fadingTimer += _dt*0.4f;
        if ( fadingTimer >= 1.0f ) {
            fadingTimer = 1.0f;
            m_FadeIn = false;
        }
        winLose_bg->Color( { fadingTimer,fadingTimer,fadingTimer,fadingTimer } );
        return;
    }

    AnimationUpdate( _dt );

    if ( !m_disableInput ) {
        auto cusorSave = static_cast<G2D::GUITexture*>( m_hudElems["WinLose_options_save"] );
        auto cusorContinue = static_cast<G2D::GUITexture*>( m_hudElems["WinLose_options_continue"] );
        auto options = static_cast<G2D::GUITexture*>( m_hudElems["WinLose_options"] );
        cusorSave->Active( false );
        cusorContinue->Active( false );

        switch ( currCursor ) {
        case SCREEN_SHOT:
            cusorSave->Active( true );
            break;

        case CONTINUE:
            cusorContinue->Active( true );
            break;
        }
    }

}

void WinLoseScene::AnimationUpdate( float _dt ) {
    static float animationTimer = 0.0f;
    static float color = 0.0f;
    static float scale = 2.0f;
    static float rotation = 0.0f;
    static float positonX = 0.0f;
    static float positonY = 0.0f;

    static G2D::GUITexture* winLoseWord = nullptr;
    if ( m_isVictory ) {
        winLoseWord = static_cast<G2D::GUITexture*>( m_hudElems["WinLose_victory"] );
    } else {
        winLoseWord = static_cast<G2D::GUITexture*>( m_hudElems["WinLose_defeat"] );
    }
    auto statsLayout = static_cast<G2D::GUITexture*>( m_hudElems["WinLose_statLayout"] );
    auto skullHead = static_cast<G2D::GUITexture*>( m_hudElems["WinLose_skullHead"] );
    auto skullSword = static_cast<G2D::GUITexture*>( m_hudElems["WinLose_skullSword"] );
    auto finalScore = static_cast<G2D::GUIText*>( m_hudElems["FinalScore"] );


    switch ( m_currStage ) {

    case WinLoseScene::SHOW_STATS:
    {
        statsLayout->Active( true );
        color += _dt;
        if ( color >= 1.0f ) color = 1.0f;
        statsLayout->Color( { color, color, color, color } );
        StatisticsUpdate( color >= 1.0f );
        // go to next stage
        animationTimer += _dt;
        if ( animationTimer >= ANIM_TIME ) {
            animationTimer = 0.0f;
            color = 0.0f;
            scale = 2.0f;
            rotation = 0.0f;
            positonX = 0.0f;
            positonY = 0.0f;
            statsLayout->Active( false );
            StatisticsUpdate( statsLayout->Active() );
            m_currStage = SHOW_SKULL;
        }
        break;
    }

    case WinLoseScene::SHOW_SKULL:
    {
        skullHead->Active( true );
        skullSword->Active( true );
        color += _dt;
        scale -= _dt;
        if ( scale <= 1.0f ) scale = 1.0f;
        skullHead->Scale( scale );
        skullSword->Scale( scale );
        skullHead->Color( { color, color, color, color } );
        skullSword->Color( { color, color, color, color } );
        if ( !m_isVictory && scale <= 1.0f ) {
            if ( skullHead->Position().x >= CGame::GetApplication()->m_window->GetWindowWidth() ) {
                rotation += _dt * 2;
                if ( rotation <= 1.0f ) {
                    skullSword->Rotation( rotation );
                }
                if ( rotation >= 1.5f ) {
                    positonY += _dt * 1000;
                    skullSword->Position( { skullSword->StaticPostions()["EdgePos"].x, skullSword->StaticPostions()["EdgePos"].y + positonY } );
                }
            } else {
                rotation += _dt * 10;
                positonX += _dt * 1000;
                positonY += _dt * 800;
                skullHead->Rotation( rotation );
                skullHead->Position( { positonX + skullHead->StaticPostions()["Original"].x, skullHead->StaticPostions()["Original"].y - positonY } );
                if ( skullHead->Position().x >= CGame::GetApplication()->m_window->GetWindowWidth() ) {
                    rotation = 0.0f;
                    positonX = 0.0f;
                    positonY = 0.0f;
                    skullSword->CenterPrivot( false );
                    skullSword->Position( skullSword->StaticPostions()["EdgePos"] );
                }
            }
        }

        // go to next stage
        if ( m_isVictory ) {
            animationTimer += _dt*2.0f;
        } else {
            animationTimer += _dt;
        }
        if ( animationTimer >= ANIM_TIME ) {

            if ( m_isVictory ) {
                color = 0.4f;
                skullHead->Color( { color, color, color, color } );
                skullSword->Color( { color, color, color, color } );
            } else {
                skullHead->Active( false );
                skullSword->Active( false );
            }
            animationTimer = 0.0f;
            color = 0.0f;
            scale = 2.0f;
            rotation = 0.0f;
            positonX = 0.0f;
            positonY = 0.0f;
            m_currStage = SHOW_WIN_LOSE_TEXT;
            winLoseWord->Active( true );
        }
        break;
    }
    case WinLoseScene::SHOW_WIN_LOSE_TEXT:
    {
        color += _dt;
        scale -= _dt;
        if ( scale <= 1.0f ) scale = 1.0f;
        if ( color >= 1.0f ) color = 1.0f;
        winLoseWord->Scale( scale );
        winLoseWord->Color( { color, color, color, color } );

        // go to next stage
        animationTimer += _dt;
        if ( animationTimer >= ANIM_TIME*0.5f ) {
            animationTimer = 0.0f;
            color = 0.0f;
            scale = 0.0f;
            rotation = 0.0f;
            positonX = 0.0f;
            positonY = 0.0f;
            m_currStage = SHOW_SCORE;
            static_cast<G2D::GUITexture*>( m_hudElems["WinLose_score"] )->Active( true );
            finalScore->Active( true );
            finalScore->Text( m_wsFinalScore );
        }
        break;
    }

    case WinLoseScene::SHOW_SCORE:
    {
        m_wsFinalScore.replace( m_wsFinalScore.begin(), m_wsFinalScore.begin() + 1, std::to_wstring( FSUtilities::Misc::RandInt( 0, 9 ) ).c_str() );
        m_wsFinalScore.replace( m_wsFinalScore.begin() + 1, m_wsFinalScore.begin() + 2, std::to_wstring( FSUtilities::Misc::RandInt( 0, 9 ) ).c_str() );
        m_wsFinalScore.replace( m_wsFinalScore.begin() + 2, m_wsFinalScore.begin() + 3, std::to_wstring( FSUtilities::Misc::RandInt( 0, 9 ) ).c_str() );
        m_wsFinalScore.replace( m_wsFinalScore.begin() + 3, m_wsFinalScore.begin() + 4, std::to_wstring( FSUtilities::Misc::RandInt( 0, 9 ) ).c_str() );
        m_wsFinalScore.replace( m_wsFinalScore.begin() + 4, m_wsFinalScore.begin() + 5, std::to_wstring( FSUtilities::Misc::RandInt( 0, 9 ) ).c_str() );
        m_wsFinalScore.replace( m_wsFinalScore.begin() + 5, m_wsFinalScore.begin() + 6, std::to_wstring( FSUtilities::Misc::RandInt( 0, 9 ) ).c_str() );
        m_wsFinalScore.replace( m_wsFinalScore.begin() + 6, m_wsFinalScore.begin() + 7, std::to_wstring( FSUtilities::Misc::RandInt( 0, 9 ) ).c_str() );
        m_wsFinalScore.replace( m_wsFinalScore.begin() + 7, m_wsFinalScore.begin() + 8, std::to_wstring( FSUtilities::Misc::RandInt( 0, 9 ) ).c_str() );
        finalScore->Text( m_wsFinalScore );
        // go to next stage
        animationTimer += _dt;
        if ( animationTimer >= ANIM_TIME*0.75f ) {
            animationTimer = 0.0f;
            color = 0.0f;
            scale = 0.0f;
            rotation = 0.0f;
            positonX = 0.0f;
            positonY = 0.0f;
            m_currStage = ANIM_END;
            m_disableInput = false;
            if ( m_isVictory ) {
                finalScore->Text( std::to_wstring( ( GameStatistics::GetRef().GetFinalScore() + 500000 ) ) );
            } else {
                finalScore->Text( std::to_wstring( GameStatistics::GetRef().GetFinalScore() ) );
            }
            static_cast<G2D::GUITexture*>( m_hudElems["WinLose_options"] )->Active( true );

        }
        break;
    }
    case WinLoseScene::ANIM_END:
    default:
        break;
    }
}

void WinLoseScene::StatisticsUpdate( bool toggle ) {
    auto& gameStat = GameStatistics::GetRef();
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

    statsRegularShotCount->Active( toggle );
    statsChainShotCount->Active( toggle );
    statsFoodShipCount->Active( toggle );
    statsGoldShipCount->Active( toggle );
    statsMilitaryShipCount->Active( toggle );
    statsCrateCollectCount->Active( toggle );
    statsGoldCollectCount->Active( toggle );
    statsFoodCollectCount->Active( toggle );
    statsRumCollectCount->Active( toggle );
    statsBodyCollectCount->Active( toggle );
    statsDistanceCount->Active( toggle );
    statsCannonBallCount->Active( toggle );
    statsBoardingCount->Active( toggle );
    statsCurrCannonLv->Active( toggle );
    statsCurrSailLv->Active( toggle );
    statsCurrHullLv->Active( toggle );
    if ( !toggle ) {
        return;
    }

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
    gameStat.CannonUpgraded( gps->GetPlayerShip()->GetSideCannonHandler()->GetLevel() );
    gameStat.SailUpgraded( gps->GetPlayerShip()->GetMastLevel() );
    gameStat.HullUpgraded( gps->GetPlayerShip()->GetHullLevel() );
    statsCurrCannonLv->Text( std::to_wstring( gps->GetPlayerShip()->GetSideCannonHandler()->GetLevel() ) + L"/5" );
    statsCurrSailLv->Text( std::to_wstring( gps->GetPlayerShip()->GetMastLevel() ) + L"/5" );
    statsCurrHullLv->Text( std::to_wstring( gps->GetPlayerShip()->GetHullLevel() ) + L"/5" );
}

void WinLoseScene::Render( void ) {
    // base render
    IScene::Render();
    m_GUIManager.RenderAll();
}

bool WinLoseScene::Input_Keyboard( void ) {
    auto game = (CGame*)CGame::GetApplication();
    auto kbs = Keyboard::Get().GetState();
    auto kbst = game->GetKeyboardTracker();
    auto sound = game->GetSoundManager();

    if ( kbst->IsKeyPressed( Keyboard::Keys::Escape ) == true ) {
        currCursor = CONTINUE;
        return OLD_SCENE;
    }

    if ( kbst->IsKeyPressed( Keyboard::Keys::Right ) == true || kbst->IsKeyPressed( Keyboard::Keys::D ) == true ) {
        currCursor = ( currCursor + 1 <= CONTINUE ) ? ( currCursor + 1 ) : SCREEN_SHOT;
        sound->Play3DSound( AK::EVENTS::PLAYCURSOR );
    }

    if ( kbst->IsKeyPressed( Keyboard::Keys::Left ) == true || kbst->IsKeyPressed( Keyboard::Keys::A ) == true ) {
        currCursor = ( currCursor - 1 >= CONTINUE ) ? ( currCursor - 1 ) : SCREEN_SHOT;
        sound->Play3DSound( AK::EVENTS::PLAYCURSOR );
    }

    if ( kbst->IsKeyPressed( Keyboard::Keys::Enter ) == true ) {
        switch ( currCursor ) {
        case WinLoseItems::SCREEN_SHOT:
            if ( !m_screenShotSaved ) {
                sound->Play3DSound( AK::EVENTS::PLAYUPGRADE );
                m_screenShotSaved = GameSetting::GetRef().SaveScreeshotToFile( g_ScreenShotPathW );
            } else {
                sound->Play3DSound( AK::EVENTS::PLAYREJECT );
            }
            return OLD_SCENE;
            break;

        case WinLoseItems::CONTINUE:
            sceneManager->PopScene();
            if ( GameSetting::GetRef().FirstTimePlayer() )
                sceneManager->PushScene( SceneType::CREDITS );
            GameSetting::GetRef().FirstTimePlayer( false );
            GameSetting::GetRef().SaveCurrentSetting();

            return NEW_SCENE;
            break;

        default:
            break;
        }
    }
    return OLD_SCENE;
}

bool WinLoseScene::Input_Gamepad( void ) {
    auto game = (CGame*)CGame::GetApplication();
    auto gps = GamePad::Get().GetState( 0 );
    auto gpst = game->GetGamePadTracker();
    auto sound = game->GetSoundManager();

    if ( gpst->b == GamePad::ButtonStateTracker::ButtonState::PRESSED ) {
        currCursor = CONTINUE;
        return OLD_SCENE;
    }

    if ( gps.IsLeftThumbStickRight() == true || gps.IsDPadRightPressed() == true ) {
        if ( scrollTimer <= 0.0f ) {
            scrollTimer = SCENE_SCROLL_TIME;
            currCursor = ( currCursor + 1 <= CONTINUE ) ? ( currCursor + 1 ) : SCREEN_SHOT;
            sound->Play3DSound( AK::EVENTS::PLAYCURSOR );
        }
    }

    if ( gps.IsLeftThumbStickLeft() == true || gps.IsDPadLeftPressed() == true ) {
        if ( scrollTimer <= 0.0f ) {
            scrollTimer = SCENE_SCROLL_TIME;
            currCursor = ( currCursor + 1 <= CONTINUE ) ? ( currCursor + 1 ) : SCREEN_SHOT;
            sound->Play3DSound( AK::EVENTS::PLAYCURSOR );
        }
    }

    if ( gpst->a == GamePad::ButtonStateTracker::ButtonState::PRESSED ) {
        switch ( currCursor ) {
        case WinLoseItems::SCREEN_SHOT:
            if ( !m_screenShotSaved ) {
                sound->Play3DSound( AK::EVENTS::PLAYUPGRADE );
                m_screenShotSaved = GameSetting::GetRef().SaveScreeshotToFile( g_ScreenShotPathW );
            } else {
                sound->Play3DSound( AK::EVENTS::PLAYREJECT );
            }
            return OLD_SCENE;
            break;

        case WinLoseItems::CONTINUE:
            sceneManager->PopScene();
            if ( GameSetting::GetRef().FirstTimePlayer() )
                sceneManager->PushScene( SceneType::CREDITS );
            GameSetting::GetRef().FirstTimePlayer( false );
            GameSetting::GetRef().SaveCurrentSetting();
            return NEW_SCENE;
            break;

        default:
            break;
        }

    }
    return OLD_SCENE;
}

void WinLoseScene::LoadHudList() {
    G2D::CGUIManager::LoadXMLConfig( "../../FullSail/Resources/GUI/Configs/WinLoseScene.xml", m_hudElems );

    for ( auto it = m_hudElems.begin(); it != m_hudElems.end(); it++ ) {
        m_GUIManager.AddEntity( it->second );
    }
}

void WinLoseScene::ReleaseHudList() {
    for ( auto it = m_hudElems.begin(); it != m_hudElems.end(); it++ ) {
        m_GUIManager.RemoveEntity( it->second );
        SafeRelease( it->second );
    }
    m_hudElems.clear();

}

