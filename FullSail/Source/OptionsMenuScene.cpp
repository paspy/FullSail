#include "pch.h"
#include "OptionsMenuScene.h"
#include "SceneManager.h"
#include "SoundManager.h"
#include "Game.h"
#include "GameSetting.h"
#include "MainWindow.h"

OptionsMenuScene::OptionsMenuScene() : IScene() {
    // scene type
    sceneType = SceneType::OPTIONS;
    m_bOptionFlag = 0;
    currCursor = (int)OptionItems::VOLUME_MUSIC;
    scrollTimer = SCENE_SCROLL_TIME;
}

OptionsMenuScene::~OptionsMenuScene() {

}

void OptionsMenuScene::Enter( void ) {
    // base enter
    IScene::Enter();
    LoadHudList();
    m_bOptionFlag = 0;
    currCursor = (int)OptionItems::VOLUME_MUSIC;
    m_gammaValue = GameSetting::GetRef().GammaValue();

}

void OptionsMenuScene::Exit( void ) {
    ReleaseHudList();
    m_bOptionFlag = 0;

    // base exit
    IScene::Exit();
}

bool OptionsMenuScene::Input( void ) {
    return ( Input_Gamepad() || Input_Keyboard() );
}

void OptionsMenuScene::Update( float _dt ) {
    // base update
    IScene::Update( _dt );

    // scroll timer
    if ( scrollTimer > 0.0f )
        scrollTimer -= _dt;
    if ( scrollTimer < 0.0f )
        scrollTimer = 0.0f;

    auto& gameSetting = GameSetting::GetRef();

    auto musicVal = static_cast<G2D::GUIText*>( m_hudElems["MusicText"] );
    auto sfxVal = static_cast<G2D::GUIText*>( m_hudElems["SFXText"] );
    auto gammaVal = static_cast<G2D::GUIText*>( m_hudElems["GammaText"] );
    auto resVal = static_cast<G2D::GUIText*>( m_hudElems["ResolutionText"] );
    auto inputVal = static_cast<G2D::GUIText*>( m_hudElems["InputDeviceText"] );

    musicVal->Text( std::to_wstring( gameSetting.MusicVolume() ) );
    sfxVal->Text( std::to_wstring( gameSetting.SfxVolume() ) );

    std::wostringstream woss;
    woss << std::setprecision( 2 ) << m_gammaValue;

    gammaVal->Text( woss.str() );

    switch ( gameSetting.Resolution() ) {
    case GameSetting::eRES_1920x1080:
        resVal->Text( L"1920x1080" );
        break;
    case GameSetting::eRES_1280x720:
        resVal->Text( L"1280x720" );
        break;
    }
    switch ( gameSetting.InputDevice() ) {
    case GameSetting::eINDEV_Keyboard:
        inputVal->Text( L"Keyboard" );
        break;
    case GameSetting::eINDEV_GamePad:
        inputVal->Text( L"GamePad" );
        break;
    }

    auto music = static_cast<G2D::GUITexture*>( m_hudElems["Option_music_selected"] );
    auto res = static_cast<G2D::GUITexture*>( m_hudElems["Option_resolution_selected"] );
    auto sfx = static_cast<G2D::GUITexture*>( m_hudElems["Option_sfx_selected"] );
    auto gamma = static_cast<G2D::GUITexture*>( m_hudElems["Option_gamma_selected"] );
    auto inputDev = static_cast<G2D::GUITexture*>( m_hudElems["Option_inputdevice_selected"] );
    auto apply = static_cast<G2D::GUITexture*>( m_hudElems["Option_apply_selected"] );
    auto back = static_cast<G2D::GUITexture*>( m_hudElems["Option_back_selected"] );
    auto def = static_cast<G2D::GUITexture*>( m_hudElems["Option_default_selected"] );

    auto kbHovered = static_cast<G2D::GUITexture*>( m_hudElems["Option_keyboard_hovered"] );
    auto xboxHovered = static_cast<G2D::GUITexture*>( m_hudElems["Option_xbox_hovered"] );

    music->Active( false );
    res->Active( false );
    sfx->Active( false );
    gamma->Active( false );
    inputDev->Active( false );
    apply->Active( false );
    back->Active( false );
    def->Active( false );

    kbHovered->Active( false );
    xboxHovered->Active( false );

    switch ( currCursor ) {
    case VOLUME_MUSIC:
        music->Active( true );
        break;
    case VOLUME_SFX:
        sfx->Active( true );
        break;
    case GAMMA:
        gamma->Active( true );
        break;
    case RESOLUTION:
        res->Active( true );
        break;
    case CONTROLLER:
        inputDev->Active( true );
        break;
    case ACCEPT:
        apply->Active( true );
        break;
    case BACK:
        back->Active( true );
        break;
    case DEFAULT:
        def->Active( true );
        break;
    default:
        break;
    }

    if ( m_bOptionFlag & 1 << VOLUME_MUSIC ) {
        if ( gameSetting.GamePadIsConnected() ) {
            xboxHovered->Active( true );
            xboxHovered->Position( xboxHovered->StaticPostions()["Music"] );
        } else {
            kbHovered->Active( true );
            kbHovered->Position( kbHovered->StaticPostions()["Music"] );
        }
    }
    if ( m_bOptionFlag & 1 << VOLUME_SFX ) {
        if ( gameSetting.GamePadIsConnected() ) {
            xboxHovered->Active( true );
            xboxHovered->Position( xboxHovered->StaticPostions()["SFX"] );
        } else {
            kbHovered->Active( true );
            kbHovered->Position( kbHovered->StaticPostions()["SFX"] );
        }
    }
    if ( m_bOptionFlag & 1 << GAMMA ) {
        if ( gameSetting.GamePadIsConnected() ) {
            xboxHovered->Active( true );
            xboxHovered->Position( xboxHovered->StaticPostions()["Gamma"] );
        } else {
            kbHovered->Active( true );
            kbHovered->Position( kbHovered->StaticPostions()["Gamma"] );
        }
    }
    if ( m_bOptionFlag & 1 << RESOLUTION ) {
        if ( gameSetting.GamePadIsConnected() ) {
            xboxHovered->Active( true );
            xboxHovered->Position( xboxHovered->StaticPostions()["Res"] );
        } else {
            kbHovered->Active( true );
            kbHovered->Position( kbHovered->StaticPostions()["Res"] );
        }
    }
    if ( m_bOptionFlag & 1 << CONTROLLER ) {
        if ( gameSetting.GamePadIsConnected() ) {
            xboxHovered->Active( true );
            xboxHovered->Position( xboxHovered->StaticPostions()["Input"] );
        } else {
            kbHovered->Active( true );
            kbHovered->Position( kbHovered->StaticPostions()["Input"] );
        }
    }
}

void OptionsMenuScene::Render( void ) {
    // base render
    IScene::Render();
    if ( !m_FadeIn ) {
        m_GUIManager.RenderAll();
    }
    // etc....
}

bool OptionsMenuScene::Input_Keyboard( void ) {
    auto    game = (CGame*)CGame::GetApplication();
    auto    kbs = Keyboard::Get().GetState();
    auto    kbst = game->GetKeyboardTracker();
    auto    sound = game->GetSoundManager();
    auto&   gameSetting = GameSetting::GetRef();

    if ( !m_bOptionFlag ) {

        if ( kbst->IsKeyPressed( Keyboard::Keys::Back ) ) {
            currCursor = OptionItems::BACK;
            return NEW_SCENE;
        }

        if ( kbst->IsKeyPressed( Keyboard::Keys::Down ) || kbst->IsKeyPressed( Keyboard::Keys::S ) ) {
            sound->Play3DSound( AK::EVENTS::PLAYCURSOR );
            switch ( currCursor ) {
            case VOLUME_MUSIC: currCursor = VOLUME_SFX; break;
            case VOLUME_SFX: currCursor = CONTROLLER; break;
            case GAMMA: currCursor = CONTROLLER; break;
            case RESOLUTION: currCursor = GAMMA; break;
            case CONTROLLER: currCursor = ACCEPT; break;
            case ACCEPT: currCursor = VOLUME_MUSIC; break;
            case BACK: currCursor = RESOLUTION; break;
            case DEFAULT: currCursor = RESOLUTION; break;
            }
        }

        if ( kbst->IsKeyPressed( Keyboard::Keys::Up ) || kbst->IsKeyPressed( Keyboard::Keys::W ) ) {
            sound->Play3DSound( AK::EVENTS::PLAYCURSOR );
            switch ( currCursor ) {
            case VOLUME_MUSIC: currCursor = ACCEPT; break;
            case VOLUME_SFX: currCursor = VOLUME_MUSIC; break;
            case GAMMA: currCursor = RESOLUTION; break;
            case RESOLUTION: currCursor = BACK; break;
            case CONTROLLER: currCursor = VOLUME_SFX; break;
            case ACCEPT: currCursor = CONTROLLER; break;
            case BACK: currCursor = CONTROLLER; break;
            case DEFAULT: currCursor = CONTROLLER; break;
            }
        }

        if ( kbst->IsKeyPressed( Keyboard::Keys::Left ) || kbst->IsKeyPressed( Keyboard::Keys::A ) ) {
            sound->Play3DSound( AK::EVENTS::PLAYCURSOR );
            switch ( currCursor ) {
            case VOLUME_MUSIC: currCursor = RESOLUTION; break;
            case VOLUME_SFX: currCursor = GAMMA; break;
            case GAMMA: currCursor = VOLUME_SFX; break;
            case RESOLUTION: currCursor = VOLUME_MUSIC; break;
            case CONTROLLER: break;
            case ACCEPT: currCursor = DEFAULT; break;
            case BACK: currCursor = ACCEPT; break;
            case DEFAULT: currCursor = BACK; break;
            }
        }

        if ( kbst->IsKeyPressed( Keyboard::Keys::Right ) || kbst->IsKeyPressed( Keyboard::Keys::D ) ) {
            sound->Play3DSound( AK::EVENTS::PLAYCURSOR );
            switch ( currCursor ) {
            case VOLUME_MUSIC: currCursor = RESOLUTION; break;
            case VOLUME_SFX: currCursor = GAMMA; break;
            case GAMMA: currCursor = VOLUME_SFX; break;
            case RESOLUTION: currCursor = VOLUME_MUSIC; break;
            case CONTROLLER: break;
            case ACCEPT: currCursor = BACK; break;
            case BACK: currCursor = DEFAULT; break;
            case DEFAULT: currCursor = ACCEPT; break;
            }
        }

        /* Input: select curr menu option */
        if ( kbst->IsKeyPressed( Keyboard::Keys::Enter ) ) {
            sound->Play3DSound( AK::EVENTS::PLAYCURSOR );
            switch ( currCursor ) {
            case ACCEPT:
                ApplyOption();
                sceneManager->PopScene();
                //game->GetSceneManager()->ReleaseSceneHudLists();
                //game->GetSceneManager()->LoadSceneHudLists();
                //game->GetSceneManager()->GetScene( game->GetSceneManager()->GetCurrSceneType() )->ReleaseHudList();
                //game->GetSceneManager()->GetScene( game->GetSceneManager()->GetCurrSceneType() )->LoadHudList();
				//fsEventManager::GetInstance()->FireEvent(fsEvents::PortSetColor);
				return NEW_SCENE;
                break;

            case BACK:
                GameSetting::GetRef().LoadExistingSetting();
                if ( !game->GamePadIsConnected() ) {
                    GameSetting::GetRef().InputDevice( GameSetting::eINDEV_Keyboard );
                }
                game->m_pRenderer->m_deviceResources->SetGamma( GameSetting::GetRef().GammaValue() );
                sound->audioSystem.SetRTCPValue( AK::GAME_PARAMETERS::SFXVOLUME, (AkRtpcValue)gameSetting.SfxVolume() );
                sound->audioSystem.SetRTCPValue( AK::GAME_PARAMETERS::MUSICVOLUME, (AkRtpcValue)gameSetting.MusicVolume() );
                sceneManager->PopScene();
                return NEW_SCENE;
                break;

                // revert edits to factory default & DONT save them out
            case DEFAULT:
                GameSetting::GetRef().GenerateDefaultSetting();
                m_gammaValue = GameSetting::GetRef().GammaValue();
                game->m_pRenderer->m_deviceResources->SetGamma( GameSetting::GetRef().GammaValue() );
                break;

            case VOLUME_MUSIC:
                m_bOptionFlag |= 1 << VOLUME_MUSIC;
                sound->Play3DSound( AK::EVENTS::PLAYMAINMENU );
                break;
            case VOLUME_SFX:
                m_bOptionFlag |= 1 << VOLUME_SFX;
                break;
            case GAMMA:
                m_bOptionFlag |= 1 << GAMMA;
                break;
            case RESOLUTION:
                m_bOptionFlag |= 1 << RESOLUTION;
                break;
            case CONTROLLER:
                m_bOptionFlag |= 1 << CONTROLLER;
                break;
            case UNKNOWN:
            default:
                break;
            }
        }
    } else {
        if ( kbst->IsKeyPressed( Keyboard::Keys::Back ) || kbst->IsKeyPressed( Keyboard::Keys::Enter ) ) {
            sound->Play3DSound( AK::EVENTS::PLAYCURSOR );
            sound->PostEvent( AK::EVENTS::STOPMAINMENU );
            m_bOptionFlag = 0;
            return OLD_SCENE;
        }

        if ( kbst->IsKeyPressed( Keyboard::Keys::Up ) || kbst->IsKeyPressed( Keyboard::Keys::W ) ) {
            if ( m_bOptionFlag & 1 << VOLUME_MUSIC ) {
                gameSetting.MusicVolume( gameSetting.MusicVolume() + 10 );
                if ( gameSetting.MusicVolume() >= 100 ) {
                    gameSetting.MusicVolume( 100 );
                }
                sound->audioSystem.SetRTCPValue( AK::GAME_PARAMETERS::MUSICVOLUME, (AkRtpcValue)gameSetting.MusicVolume() );

            }

            if ( m_bOptionFlag & 1 << VOLUME_SFX ) {
                gameSetting.SfxVolume( gameSetting.SfxVolume() + 10 );
                if ( gameSetting.SfxVolume() >= 100 ) {
                    gameSetting.SfxVolume( 100 );
                }
                sound->audioSystem.SetRTCPValue( AK::GAME_PARAMETERS::SFXVOLUME, (AkRtpcValue)gameSetting.SfxVolume() );
            }

            if ( m_bOptionFlag & 1 << GAMMA ) {
                m_gammaValue += 0.1f;
                if ( m_gammaValue >= 2.0f ) {
                    m_gammaValue = 2.0f;
                }
                auto game = (CGame*)CApplication::GetApplication();
                game->m_pRenderer->m_deviceResources->SetGamma( m_gammaValue );
            }

            if ( m_bOptionFlag & 1 << RESOLUTION ) {
                gameSetting.Resolution( GameSetting::eRESOLUTION( ( int( gameSetting.Resolution() ) + 1 ) ) );
                if ( gameSetting.Resolution() >= int( GameSetting::eRESOLUTION::eRES_MAX ) ) {
                    gameSetting.Resolution( GameSetting::eRESOLUTION( 0 ) );
                }
            }

            if ( m_bOptionFlag & 1 << CONTROLLER ) {
                auto game = (CGame*)CApplication::GetApplication();
                if ( game->GamePadIsConnected() ) {
                    gameSetting.InputDevice( GameSetting::eINPUT_DEVICE( ( int( gameSetting.InputDevice() ) + 1 ) ) );
                    if ( gameSetting.InputDevice() >= int( GameSetting::eINPUT_DEVICE::eINDEV_MAX ) ) {
                        gameSetting.InputDevice( GameSetting::eINPUT_DEVICE( 0 ) );
                    }
                }
            }
            sound->Play3DSound( AK::EVENTS::PLAYCURSOR );
        }

        if ( kbst->IsKeyPressed( Keyboard::Keys::Down ) || kbst->IsKeyPressed( Keyboard::Keys::S ) ) {

            if ( m_bOptionFlag & 1 << VOLUME_MUSIC ) {
                gameSetting.MusicVolume( gameSetting.MusicVolume() - 10 );
                if ( gameSetting.MusicVolume() <= 0 || gameSetting.MusicVolume() > 10000 ) {
                    gameSetting.MusicVolume( 0 );
                }
                AkRtpcValue musicVol = static_cast<AkRtpcValue>( gameSetting.MusicVolume() );
                if ( musicVol <= 0 )musicVol = 1;
                sound->audioSystem.SetRTCPValue( AK::GAME_PARAMETERS::MUSICVOLUME, musicVol );
            }

            if ( m_bOptionFlag & 1 << VOLUME_SFX ) {
                gameSetting.SfxVolume( gameSetting.SfxVolume() - 10 );
                if ( gameSetting.SfxVolume() <= 0 || gameSetting.SfxVolume() > 10000 ) {
                    gameSetting.SfxVolume( 0 );
                }
                sound->audioSystem.SetRTCPValue( AK::GAME_PARAMETERS::SFXVOLUME, (AkRtpcValue)gameSetting.SfxVolume() );
            }

            if ( m_bOptionFlag & 1 << GAMMA ) {
                m_gammaValue -= 0.1f;
                if ( m_gammaValue <= 1.0f ) {
                    m_gammaValue = 1.0f;
                }
                auto game = (CGame*)CApplication::GetApplication();
                game->m_pRenderer->m_deviceResources->SetGamma( m_gammaValue );
            }

            if ( m_bOptionFlag & 1 << RESOLUTION ) {
                gameSetting.Resolution( GameSetting::eRESOLUTION( ( int( gameSetting.Resolution() ) + 1 ) ) );
                if ( gameSetting.Resolution() >= int( GameSetting::eRESOLUTION::eRES_MAX ) ) {
                    gameSetting.Resolution( GameSetting::eRESOLUTION( 0 ) );
                }
            }

            if ( m_bOptionFlag & 1 << CONTROLLER ) {
                auto game = (CGame*)CApplication::GetApplication();
                if ( game->GamePadIsConnected() ) {
                    gameSetting.InputDevice( GameSetting::eINPUT_DEVICE( ( int( gameSetting.InputDevice() ) + 1 ) ) );
                    if ( gameSetting.InputDevice() >= int( GameSetting::eINPUT_DEVICE::eINDEV_MAX ) ) {
                        gameSetting.InputDevice( GameSetting::eINPUT_DEVICE( 0 ) );
                    }
                }
            }
            sound->Play3DSound( AK::EVENTS::PLAYCURSOR );

        }

    }



    // stay in same scene
    return OLD_SCENE;
}

bool OptionsMenuScene::Input_Gamepad( void ) {
    auto    game = (CGame*)CGame::GetApplication();
    auto    gps = GamePad::Get().GetState( 0 );
    auto    gpst = game->GetGamePadTracker();
    auto&   gameSetting = GameSetting::GetRef();
    auto    sound = game->GetSoundManager();

    if ( !m_bOptionFlag ) {


        if ( gpst->b == GamePad::ButtonStateTracker::ButtonState::PRESSED ) {
            sound->Play3DSound( AK::EVENTS::PLAYCURSOR );
            currCursor = OptionItems::BACK;
            return NEW_SCENE;
        }

        if ( ( gps.IsLeftThumbStickDown() || gps.IsDPadDownPressed() ) && scrollTimer <= 0.0f ) {
            sound->Play3DSound( AK::EVENTS::PLAYCURSOR );
            scrollTimer = SCENE_SCROLL_TIME;
            switch ( currCursor ) {
            case VOLUME_MUSIC: currCursor = VOLUME_SFX; break;
            case VOLUME_SFX: currCursor = CONTROLLER; break;
            case GAMMA: currCursor = CONTROLLER; break;
            case RESOLUTION: currCursor = GAMMA; break;
            case CONTROLLER: currCursor = ACCEPT; break;
            case ACCEPT: currCursor = VOLUME_MUSIC; break;
            case BACK: currCursor = RESOLUTION; break;
            case DEFAULT: currCursor = RESOLUTION; break;
            }
        }

        if ( ( gps.IsLeftThumbStickUp() || gps.IsDPadUpPressed() ) && scrollTimer <= 0.0f ) {
            sound->Play3DSound( AK::EVENTS::PLAYCURSOR );
            scrollTimer = SCENE_SCROLL_TIME;
            switch ( currCursor ) {
            case VOLUME_MUSIC: currCursor = ACCEPT; break;
            case VOLUME_SFX: currCursor = VOLUME_MUSIC; break;
            case GAMMA: currCursor = RESOLUTION; break;
            case RESOLUTION: currCursor = BACK; break;
            case CONTROLLER: currCursor = VOLUME_SFX; break;
            case ACCEPT: currCursor = CONTROLLER; break;
            case BACK: currCursor = CONTROLLER; break;
            case DEFAULT: currCursor = CONTROLLER; break;
            }
        }

        if ( ( gps.IsLeftThumbStickLeft() || gps.IsDPadLeftPressed() ) && scrollTimer <= 0.0f ) {
            sound->Play3DSound( AK::EVENTS::PLAYCURSOR );
            scrollTimer = SCENE_SCROLL_TIME;
            switch ( currCursor ) {
            case VOLUME_MUSIC: currCursor = RESOLUTION; break;
            case VOLUME_SFX: currCursor = GAMMA; break;
            case GAMMA: currCursor = VOLUME_SFX; break;
            case RESOLUTION: currCursor = VOLUME_MUSIC; break;
            case CONTROLLER: break;
            case ACCEPT: currCursor = DEFAULT; break;
            case BACK: currCursor = ACCEPT; break;
            case DEFAULT: currCursor = BACK; break;
            }
        }

        if ( ( gps.IsLeftThumbStickRight() || gps.IsDPadRightPressed() ) && scrollTimer <= 0.0f ) {
            sound->Play3DSound( AK::EVENTS::PLAYCURSOR );
            scrollTimer = SCENE_SCROLL_TIME;
            switch ( currCursor ) {
            case VOLUME_MUSIC: currCursor = RESOLUTION; break;
            case VOLUME_SFX: currCursor = GAMMA; break;
            case GAMMA: currCursor = VOLUME_SFX; break;
            case RESOLUTION: currCursor = VOLUME_MUSIC; break;
            case CONTROLLER: break;
            case ACCEPT: currCursor = BACK; break;
            case BACK: currCursor = DEFAULT; break;
            case DEFAULT: currCursor = ACCEPT; break;
            }
        }


        if ( gpst->a == GamePad::ButtonStateTracker::ButtonState::PRESSED ) {
            sound->Play3DSound( AK::EVENTS::PLAYCURSOR );
            switch ( currCursor ) {
            case ACCEPT:
                ApplyOption();
                sceneManager->PopScene();
                //game->GetSceneManager()->ReleaseSceneHudLists();
                //game->GetSceneManager()->LoadSceneHudLists();
                //game->GetSceneManager()->GetScene( game->GetSceneManager()->GetCurrSceneType() )->ReleaseHudList();
                //game->GetSceneManager()->GetScene( game->GetSceneManager()->GetCurrSceneType())->LoadHudList();
				//fsEventManager::GetInstance()->FireEvent(fsEvents::PortSetColor);
				return NEW_SCENE;
                break;

            case BACK:
                GameSetting::GetRef().LoadExistingSetting();
                if ( !game->GamePadIsConnected() ) {
                    GameSetting::GetRef().InputDevice( GameSetting::eINDEV_Keyboard );
                }
                game->m_pRenderer->m_deviceResources->SetGamma( GameSetting::GetRef().GammaValue() );
                sound->audioSystem.SetRTCPValue( AK::GAME_PARAMETERS::SFXVOLUME, (AkRtpcValue)gameSetting.SfxVolume() );
                sound->audioSystem.SetRTCPValue( AK::GAME_PARAMETERS::MUSICVOLUME, (AkRtpcValue)gameSetting.MusicVolume() );
                sceneManager->PopScene();
                return NEW_SCENE;
                break;

                // revert edits to factory default & DONT save them out
            case DEFAULT:
                GameSetting::GetRef().GenerateDefaultSetting();
                m_gammaValue = GameSetting::GetRef().GammaValue();
                game->m_pRenderer->m_deviceResources->SetGamma( GameSetting::GetRef().GammaValue() );
                break;

            case VOLUME_MUSIC:
                m_bOptionFlag |= 1 << VOLUME_MUSIC;
                sound->Play3DSound( AK::EVENTS::PLAYMAINMENU );
                break;
            case VOLUME_SFX:
                m_bOptionFlag |= 1 << VOLUME_SFX;
                break;
            case GAMMA:
                m_bOptionFlag |= 1 << GAMMA;
                break;
            case RESOLUTION:
                m_bOptionFlag |= 1 << RESOLUTION;
                break;
            case CONTROLLER:
                m_bOptionFlag |= 1 << CONTROLLER;
                break;
            case UNKNOWN:
            default:
                break;
            }
        }

    } else {

        if ( gpst->b == GamePad::ButtonStateTracker::ButtonState::PRESSED || gpst->a == GamePad::ButtonStateTracker::ButtonState::PRESSED ) {
            sound->Play3DSound( AK::EVENTS::PLAYCURSOR );
            sound->PostEvent( AK::EVENTS::STOPMAINMENU );
            m_bOptionFlag = 0;
            return OLD_SCENE;
        }


        if ( ( gps.IsLeftThumbStickUp() || gps.IsDPadUpPressed() ) && scrollTimer <= 0.0f ) {
            sound->Play3DSound( AK::EVENTS::PLAYCURSOR );
            scrollTimer = SCENE_SCROLL_TIME;
            if ( m_bOptionFlag & 1 << VOLUME_MUSIC ) {
                gameSetting.MusicVolume( gameSetting.MusicVolume() + 10 );
                if ( gameSetting.MusicVolume() >= 100 ) {
                    gameSetting.MusicVolume( 100 );
                }
                sound->audioSystem.SetRTCPValue( AK::GAME_PARAMETERS::MUSICVOLUME, (AkRtpcValue)gameSetting.MusicVolume() );
            }

            if ( m_bOptionFlag & 1 << VOLUME_SFX ) {
                gameSetting.SfxVolume( gameSetting.SfxVolume() + 10 );
                if ( gameSetting.SfxVolume() >= 100 ) {
                    gameSetting.SfxVolume( 100 );
                }
                sound->audioSystem.SetRTCPValue( AK::GAME_PARAMETERS::SFXVOLUME, (AkRtpcValue)gameSetting.SfxVolume() );
            }

            if ( m_bOptionFlag & 1 << GAMMA ) {
                m_gammaValue += 0.1f;
                if ( m_gammaValue >= 2.0f ) {
                    m_gammaValue = 2.0f;
                }
                auto game = (CGame*)CApplication::GetApplication();
                game->m_pRenderer->m_deviceResources->SetGamma( m_gammaValue );
            }

            if ( m_bOptionFlag & 1 << RESOLUTION ) {
                gameSetting.Resolution( GameSetting::eRESOLUTION( ( int( gameSetting.Resolution() ) + 1 ) ) );
                if ( gameSetting.Resolution() >= int( GameSetting::eRESOLUTION::eRES_MAX ) ) {
                    gameSetting.Resolution( GameSetting::eRESOLUTION( 0 ) );
                }
            }

            if ( m_bOptionFlag & 1 << CONTROLLER ) {
                gameSetting.InputDevice( GameSetting::eINPUT_DEVICE( ( int( gameSetting.InputDevice() ) + 1 ) ) );
                if ( gameSetting.InputDevice() >= int( GameSetting::eINPUT_DEVICE::eINDEV_MAX ) ) {
                    gameSetting.InputDevice( GameSetting::eINPUT_DEVICE( 0 ) );
                }
            }
        }

        if ( ( gps.IsLeftThumbStickDown() || gps.IsDPadDownPressed() ) && scrollTimer <= 0.0f ) {
            sound->Play3DSound( AK::EVENTS::PLAYCURSOR );
            scrollTimer = SCENE_SCROLL_TIME;
            if ( m_bOptionFlag & 1 << VOLUME_MUSIC ) {
                gameSetting.MusicVolume( gameSetting.MusicVolume() - 10 );
                if ( gameSetting.MusicVolume() <= 0 || gameSetting.MusicVolume() > 10000 ) {
                    gameSetting.MusicVolume( 0 );
                }
                AkRtpcValue musicVol = static_cast<AkRtpcValue>( gameSetting.MusicVolume() );
                if ( musicVol <= 0 )musicVol = 1;
                sound->audioSystem.SetRTCPValue( AK::GAME_PARAMETERS::MUSICVOLUME, musicVol );
            }

            if ( m_bOptionFlag & 1 << VOLUME_SFX ) {
                gameSetting.SfxVolume( gameSetting.SfxVolume() - 10 );
                if ( gameSetting.SfxVolume() <= 0 || gameSetting.SfxVolume() > 10000 ) {
                    gameSetting.SfxVolume( 0 );
                }
                sound->audioSystem.SetRTCPValue( AK::GAME_PARAMETERS::SFXVOLUME, (AkRtpcValue)gameSetting.SfxVolume() );
            }

            if ( m_bOptionFlag & 1 << GAMMA ) {
                m_gammaValue -= 0.1f;
                if ( m_gammaValue <= 1.0f ) {
                    m_gammaValue = 1.0f;
                }
                auto game = (CGame*)CApplication::GetApplication();
                game->m_pRenderer->m_deviceResources->SetGamma( m_gammaValue );
            }

            if ( m_bOptionFlag & 1 << RESOLUTION ) {
                gameSetting.Resolution( GameSetting::eRESOLUTION( ( int( gameSetting.Resolution() ) + 1 ) ) );
                if ( gameSetting.Resolution() >= int( GameSetting::eRESOLUTION::eRES_MAX ) ) {
                    gameSetting.Resolution( GameSetting::eRESOLUTION( 0 ) );
                }
            }

            if ( m_bOptionFlag & 1 << CONTROLLER ) {
                gameSetting.InputDevice( GameSetting::eINPUT_DEVICE( ( int( gameSetting.InputDevice() ) + 1 ) ) );
                if ( gameSetting.InputDevice() >= int( GameSetting::eINPUT_DEVICE::eINDEV_MAX ) ) {
                    gameSetting.InputDevice( GameSetting::eINPUT_DEVICE( 0 ) );
                }
            }
        }

    }

    // stay in same scene
    return OLD_SCENE;
}

void OptionsMenuScene::ApplyOption() {
    auto game = (CGame*)CGame::GetApplication();

    //switch ( GameSetting::GetRef().Resolution() ) {
    //case GameSetting::eRES_1280x720:
    //    game->m_window->SetSize( 1280, 720 );
    //    break;
    //case GameSetting::eRES_1920x1080:
    //    game->m_window->SetSize( 1920, 1080 );
    //    break;
    //}
    //game->m_window->SetWindowed( game->m_pRenderer->m_deviceResources->GetWindowed() );
    //game->m_pRenderer->m_deviceResources->ResizeWindow( game->m_window->GetWindowWidth(), game->m_window->GetWindowHeight() );

    game->m_pRenderer->m_deviceResources->SetGamma( m_gammaValue );
    GameSetting::GetRef().GammaValue( m_gammaValue );
    GameSetting::GetRef().SaveCurrentSetting();

}

void OptionsMenuScene::LoadHudList() {
    G2D::CGUIManager::LoadXMLConfig( "../../FullSail/Resources/GUI/Configs/OptionScene.xml", m_hudElems );
    for ( auto it = m_hudElems.begin(); it != m_hudElems.end(); it++ ) {
        m_GUIManager.AddEntity( it->second );
    }
}

void OptionsMenuScene::ReleaseHudList() {
    for ( auto it = m_hudElems.begin(); it != m_hudElems.end(); it++ ) {
        m_GUIManager.RemoveEntity( it->second );
        SafeRelease( it->second );
    }
    m_hudElems.clear();
}

