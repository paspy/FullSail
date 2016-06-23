#include "stdafx.h"
#include "SpritesManager.h"
#include "Utilities.h"

#include <tinyxml2.h>

namespace G2D {

    CSpritesManager::CSpritesManager() {

    }

    CSpritesManager::~CSpritesManager() {

    }

    void CSpritesManager::LoadSprite2D( const std::wstring & path, ID3D11Device * device ) {
        std::vector<std::wstring> fileList;
        GetFileInDirectory( fileList, path );

        for ( auto i = 0; i < fileList.size(); i++ ) {
            ComPtr<ID3D11Resource> resource;
            Sprite2DInfo spriteInfo;
            // store complete path
            spriteInfo.path = fileList[i];

            // delete file extension save as a sprite name
            wchar_t fname[MAX_PATH];
            _wsplitpath_s( fileList[i].c_str(), NULL, 0, NULL, 0, fname, MAX_PATH, NULL, 0 );
            spriteInfo.name = AssetUtilities::ws2s( fname );

            DX::ThrowIfFailed(
                DirectX::CreateDDSTextureFromFile( device, fileList[i].c_str(),
                    resource.GetAddressOf(),
                    spriteInfo.texture.ReleaseAndGetAddressOf() )
            );
            // to get the texture origin
            ComPtr<ID3D11Texture2D> tex;
            DX::ThrowIfFailed( resource.As( &tex ) );
            CD3D11_TEXTURE2D_DESC texDesc;
            tex->GetDesc( &texDesc );

            spriteInfo.size.x = float( texDesc.Width );
            spriteInfo.size.y = float( texDesc.Height );

            spriteInfo.origin.x = float( texDesc.Width * 0.5f );
            spriteInfo.origin.y = float( texDesc.Height * 0.5f );

            m_storedSprites[spriteInfo.name] = spriteInfo;
        }

    }

    void CSpritesManager::LoadSpriteFont( const std::wstring & path, ID3D11Device * device ) {
        std::vector<std::wstring> fileList;
        GetFileInDirectory( fileList, path );

        for ( auto i = 0; i < fileList.size(); i++ ) {
            SpriteFontInfo fontInfo;

            fontInfo.font = std::shared_ptr<DirectX::SpriteFont>( new DirectX::SpriteFont( device, fileList[i].c_str() ) );
            // delete file extension save as a sprite name
            wchar_t fname[MAX_PATH];
            _wsplitpath_s( fileList[i].c_str(), NULL, 0, NULL, 0, fname, MAX_PATH, NULL, 0 );
            fontInfo.name = AssetUtilities::ws2s( fname );

            fontInfo.path = fileList[i];

            m_storedFonts[fontInfo.name] = fontInfo;
        }

    }

    Sprite2DInfo CSpritesManager::GetSprite( std::string name ) {
        return m_storedSprites[name];
    }

    SpriteFontInfo CSpritesManager::GetFont( std::string name ) {
        return m_storedFonts[name];
    }

    bool CSpritesManager::IsFontExist( std::string name ) {
        return m_storedFonts.count( name ) > 0;

    }

    bool CSpritesManager::IsSpriteExist( std::string name ) {
        return m_storedSprites.count( name ) > 0;
    }

    void CSpritesManager::SaveToXML( const std::wstring &path ) {
        if ( path.empty() ) return; // path not provide than return
        tinyxml2::XMLDocument xmlDoc;


        tinyxml2::XMLNode *spriteListNode = xmlDoc.NewElement( "Asset2D" );
        xmlDoc.InsertFirstChild( spriteListNode );

        // save sprite list
        if ( m_storedSprites.size() > 0 ) {
            tinyxml2::XMLElement *spriteElem = xmlDoc.NewElement( "Sprite" );
            spriteElem->SetAttribute( "Count", (unsigned int)m_storedSprites.size() );
            for ( const auto & elem : m_storedSprites ) {
                tinyxml2::XMLElement *item = xmlDoc.NewElement( "Sprite" );
                item->SetAttribute( "Name", elem.second.name.c_str() );
                item->SetAttribute( "Width", elem.second.size.x );
                item->SetAttribute( "Height", elem.second.size.y );
                item->SetAttribute( "Origin_X", elem.second.origin.x );
                item->SetAttribute( "Origin_Y", elem.second.origin.y );
                item->SetText( AssetUtilities::ws2s( elem.second.path ).c_str() );
                spriteElem->InsertEndChild( item );
            }
            spriteListNode->InsertEndChild( spriteElem );
        }

        // save font list
        if ( m_storedFonts.size() > 0 ) {
            tinyxml2::XMLElement *fontElem = xmlDoc.NewElement( "Font" );
            fontElem->SetAttribute( "Count", (unsigned int)m_storedFonts.size() );

            for ( const auto & elem : m_storedFonts ) {
                tinyxml2::XMLElement *item = xmlDoc.NewElement( "Font" );
                item->SetAttribute( "name", elem.second.name.c_str() );
                item->SetText( AssetUtilities::ws2s( elem.second.path ).c_str() );
                fontElem->InsertEndChild( item );
            }
            spriteListNode->InsertEndChild( fontElem );
        }

        tinyxml2::XMLDeclaration *decl = xmlDoc.NewDeclaration();
        xmlDoc.InsertFirstChild( decl );

        if ( xmlDoc.SaveFile( AssetUtilities::ws2s( path ).c_str() ) != tinyxml2::XML_SUCCESS ) {
            // to-do error check
        }
        xmlDoc.Clear();
    }

    bool CSpritesManager::GetFileInDirectory( std::vector<std::wstring> &out, const std::wstring &path ) {
        HANDLE dir;
        WIN32_FIND_DATA file_data;

        if ( ( dir = FindFirstFile( ( path + L"/*" ).c_str(), &file_data ) ) == INVALID_HANDLE_VALUE ) {
            return false; /* No files found */
        }

        do {
            const std::wstring file_name = file_data.cFileName;
            const std::wstring full_file_name = path + L"/" + file_name;
            const bool is_directory = ( file_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ) != 0;

            if ( file_name[0] == '.' )
                continue;

            if ( is_directory )
                continue;

            out.push_back( full_file_name );
        } while ( FindNextFile( dir, &file_data ) );

        FindClose( dir );
        return true;
    }

    bool CSpritesManager::GetFileInDirectory( std::vector<std::string> &out, const std::wstring &path ) {
        std::vector<std::wstring> wout;
        if ( !GetFileInDirectory( wout, path ) ) {
            return false;
        }
        out.clear();
        for ( auto i = 0; i < wout.size(); i++ ) {
            out.push_back( AssetUtilities::ws2s( wout[i] ) );
        }
        return true;
    }

}