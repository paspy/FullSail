#include "pch.h"
#include "GameSetting.h"
#include "Game.h"
#include <ScreenGrab.h>
#include <wincodec.h>
#include <Shlobj.h>

const std::string g_SettingPathA( "../../Setting.xml" );
const std::wstring g_SettingPathW( L"../../Setting.xml" );

GameSetting::GameSetting() :
    m_firstTimePlayer( false ),
    m_windowed( false ),
    m_musicVolume( 0 ),
    m_sfxVolume( 0 ),
    m_gammaValue( 0.0f ),
    m_resolution( eRES_1920x1080 ),
    m_inputDevice( eINDEV_Keyboard ) {

}

bool GameSetting::GenerateDefaultSetting() {
    // default settings
    tinyxml2::XMLDocument xmlDoc;
    m_firstTimePlayer = true;
    m_windowed = false;
    m_musicVolume = 80;
    m_sfxVolume = 70;
    m_gammaValue = 1.0f;
    m_resolution = eRES_1920x1080;
    m_inputDevice = eINDEV_Keyboard;
    return true;
}

bool GameSetting::LoadExistingSetting() {
    std::tr2::sys::path settingFile = std::tr2::sys::path( g_SettingPathA.c_str() );
    if ( !std::tr2::sys::exists( settingFile ) ) {
        return false;
    }
    tinyxml2::XMLDocument xmlDoc;
    if ( xmlDoc.LoadFile( g_SettingPathA.c_str() ) != tinyxml2::XML_SUCCESS ) {
        return false;
    }
    //auto game = (CGame*)CGame::GetApplication();
    tinyxml2::XMLElement * root = xmlDoc.RootElement();
    FS_DEBUG::Assert( ( std::string( root->Name() ) == std::string( "FS_Setting" ) ), L"Bad Setting File." );

    root->ToElement()->QueryBoolAttribute( "First_Time_Player", &m_firstTimePlayer );
    root->ToElement()->QueryBoolAttribute( "Window_Mode", &m_windowed );
    root->ToElement()->QueryUnsignedAttribute( "Music_Volume", &m_musicVolume );
    root->ToElement()->QueryUnsignedAttribute( "SFX_Volume", &m_sfxVolume );
    if ( m_musicVolume > 100 ) m_musicVolume = 100;
    if ( m_sfxVolume > 100 ) m_sfxVolume = 100;
    root->ToElement()->QueryFloatAttribute( "Gamma_Value", &m_gammaValue );
    unsigned int tmpResolution, tmpInputDevice;
    root->ToElement()->QueryUnsignedAttribute( "Resolution", &tmpResolution );
    root->ToElement()->QueryUnsignedAttribute( "Input_Device", &tmpInputDevice );
    m_resolution = eRESOLUTION( tmpResolution );
    m_inputDevice = eINPUT_DEVICE( tmpInputDevice );
    //if ( game->GamePadIsConnected() )
    //else
    //    m_inputDevice = eINDEV_Keyboard;

    xmlDoc.Clear();

    return true;
}

bool GameSetting::SaveCurrentSetting() {
    std::tr2::sys::path settingFile = std::tr2::sys::path( g_SettingPathA.c_str() );
    if ( std::tr2::sys::exists( settingFile ) ) {
        std::tr2::sys::remove( settingFile );
    }

    tinyxml2::XMLDocument xmlDoc;
    tinyxml2::XMLDeclaration *decl = xmlDoc.NewDeclaration();
    xmlDoc.InsertFirstChild( decl );

    tinyxml2::XMLNode *settingNode = xmlDoc.NewElement( "FS_Setting" );

    settingNode->ToElement()->SetAttribute( "First_Time_Player", m_firstTimePlayer );
    settingNode->ToElement()->SetAttribute( "Window_Mode", m_windowed );
    settingNode->ToElement()->SetAttribute( "Music_Volume", m_musicVolume );
    settingNode->ToElement()->SetAttribute( "SFX_Volume", m_sfxVolume );
    settingNode->ToElement()->SetAttribute( "Gamma_Value", m_gammaValue );
    settingNode->ToElement()->SetAttribute( "Resolution", int( m_resolution ) );
    settingNode->ToElement()->SetAttribute( "Input_Device", int( m_inputDevice ) );
    settingNode->ToElement()->SetText( ( "Last Saved Date: " + GetCurrentDate() ).c_str() );

    xmlDoc.InsertEndChild( settingNode );

    if ( xmlDoc.SaveFile( g_SettingPathA.c_str() ) != tinyxml2::XML_SUCCESS ) {
        return false;
    }

    xmlDoc.Clear();
    return true;
}

bool GameSetting::SaveScreeshotToFile( const std::wstring & path ) {
    using namespace DirectX;
    using namespace Microsoft::WRL;
    ComPtr<ID3D11Texture2D> backBuffer;

    LPWSTR wszPath = nullptr;
    HRESULT hr = SHGetKnownFolderPath( FOLDERID_Pictures, 0, 0, &wszPath );

    std::wstring pathDoc = wszPath;
    auto game = static_cast<CGame*>( CGame::GetApplication() );
    std::wstring fileName = L"\\Full Sail Screenshots\\FullSail_" + GetCurrentDateW() + L".png" ;
    std::replace( fileName.begin(), fileName.end(), ':', '-' );
    pathDoc += fileName;
    hr = game->m_pRenderer->m_deviceResources->GetSwapChain()->GetBuffer( 0, __uuidof( ID3D11Texture2D ),
        reinterpret_cast<LPVOID*>( backBuffer.GetAddressOf() ) );
    FS_DEBUG::Assert( SUCCEEDED( hr ), "SaveScreeshotToFile - Get Back Buffer Failed." );
    if ( SUCCEEDED( hr ) ) {

        std::tr2::sys::path screenShotPath = std::tr2::sys::path( pathDoc.c_str() );
        if ( screenShotPath.has_parent_path() && !std::tr2::sys::exists( screenShotPath.parent_path() ) ) {
            std::tr2::sys::create_directories( screenShotPath.parent_path() );
        }

        hr = SaveWICTextureToFile( game->m_pRenderer->m_deviceResources->GetD3DDeviceContext(), backBuffer.Get(),
            GUID_ContainerFormatPng, pathDoc.c_str());
        FS_DEBUG::Assert( SUCCEEDED( hr ), "SaveScreeshotToFile - Save ScreenShot Failed." );

    }
    return true;
}

std::string GameSetting::GetCurrentDate() const {
    time_t     now = time( 0 );
    struct tm  tstruct;
    char       buf[80];
    localtime_s( &tstruct, &now );
    strftime( buf, sizeof( buf ), "%Y-%m-%d_%X", &tstruct );
    return buf;
}

std::wstring GameSetting::GetCurrentDateW() const {
    time_t     now = time( 0 );
    struct tm  tstruct;
    wchar_t       buf[80];
    localtime_s( &tstruct, &now );
    wcsftime( buf, sizeof( buf ), L"%Y-%m-%d_%X", &tstruct );
    return buf;
}