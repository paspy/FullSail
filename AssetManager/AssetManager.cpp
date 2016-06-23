#include "stdafx.h"
#include "AssetManager.h"
#include <iostream>
#include <fstream>
using namespace std;
using namespace DirectX;



const string g_MeshDirectory( "../../FullSail/Resources/Model/Mesh/" );
const string g_MeshTextureDirectory( "../../FullSail/Resources/Model/Texture/" );

// use for spritesManager
const wstring g_GUISpriteDirectoryW( L"../../FullSail/Resources/GUI/Sprites" );
const wstring g_GUIFontDirectoryW( L"../../FullSail/Resources/GUI/Fonts" );
const wstring g_spriteXmlListW( L"../../FullSail/Resources/GUI/SpriteAndFontList.xml" );

CAssetManager* CAssetManager::m_pInstance = nullptr;

CAssetManager& CAssetManager::GetRef() {
    if ( !m_pInstance ) {
        m_pInstance = new CAssetManager;
    }
    return *m_pInstance;
}

ID3D11ShaderResourceView* CAssetManager::GetSprite( __in std::string &name, __out_opt DirectX::SimpleMath::Vector2 &origin ) {
    origin = m_sprite2ds.GetSprite( name ).origin;
    return m_sprite2ds.GetSprite( name ).texture.Get();
}

ID3D11ShaderResourceView* CAssetManager::GetSprite( __in std::string &name ) {
    return m_sprite2ds.GetSprite( name ).texture.Get();
}

DirectX::SimpleMath::Vector2 CAssetManager::GetSpriteSize( __in std::string name ) {
    return m_sprite2ds.GetSprite( name ).size;
}

CAssetManager::CAssetManager() {

}


void	CAssetManager::Init() {
    std::vector<std::string> filenames;
    ifstream fileListIn( g_MeshDirectory + "Meshes.txt" );
    if ( fileListIn.is_open() ) {
        while ( !fileListIn.eof() ) {
            string filename;
            fileListIn >> filename;
            if ( filename.size() > 0 ) {
                filenames.push_back( filename );
            }
        }
        fileListIn.close();
    } else {
        assert( false && "Can't Find Meshes.txt" );
    }

    for ( auto& eachMashName : filenames ) {
        LoadMesh( eachMashName );

    }


    filenames.clear();
    fileListIn.open( g_MeshTextureDirectory + "Textures.txt" );
    if ( fileListIn.is_open() ) {
        while ( !fileListIn.eof() ) {
            string filename;
            fileListIn >> filename;
            if ( filename.size() > 0 ) {
                filenames.push_back( filename );
            }
        }
        fileListIn.close();
    } else {
        assert( false && "Can't Find Texture.txt" );
    }


    for ( auto& eachTextureName : filenames ) {
        LoadTexture( eachTextureName );
    }

    m_sprite2ds.LoadSprite2D( g_GUISpriteDirectoryW, m_pDevice );
    m_sprite2ds.LoadSpriteFont( g_GUIFontDirectoryW, m_pDevice );
    m_sprite2ds.SaveToXML( g_spriteXmlListW );
}

void	CAssetManager::Shutdown() {
    for ( auto& eachMesh : m_Meshes ) {
        delete eachMesh.second;
        eachMesh.second = nullptr;
    }

    for ( auto& eachTexture : m_Textures ) {
        SAFE_RELEASE( eachTexture.second );
    }

    delete m_pInstance;
    m_pInstance = nullptr;
}

CAssetManager::~CAssetManager() {

}

ID3D11ShaderResourceView* CAssetManager::Textures( const char* _fileName ) {
    if ( _fileName ) {
        return m_Textures[_fileName];
    }
    return nullptr;

}


void CAssetManager::LoadMesh( string& _fileName ) {
    ifstream bin( g_MeshDirectory + _fileName, ios_base::binary );
    if ( m_Meshes.count( _fileName ) ) {
        return;
    }
    m_Meshes[_fileName] = new Mesh;
    if ( bin.is_open() ) {
        char header[8];
        bin.read( header, 8 );
        if ( strcmp( header, "FULLSAIL" ) ) {
            uint8_t modelNameLength = 0;
            bin.read( reinterpret_cast<char*>( &modelNameLength ), sizeof modelNameLength );


            string modelName;
            modelName.resize( modelNameLength );
            bin.read( &modelName[0], modelNameLength * sizeof( char ) );


            uint8_t numOfMeshes = 0;
            bin.read( reinterpret_cast<char*>( &numOfMeshes ), sizeof numOfMeshes );



            for ( size_t eachMesh = 0; eachMesh < numOfMeshes; eachMesh++ ) {
                uint32_t numOfVertices = 0;
                bin.read( reinterpret_cast<char*>( &numOfVertices ), sizeof numOfVertices );

                m_Meshes[_fileName]->RawData.pos.resize( numOfVertices );
                m_Meshes[_fileName]->RawData.nor.resize( numOfVertices );
                m_Meshes[_fileName]->RawData.uvs.resize( numOfVertices );
                m_Meshes[_fileName]->RawData.tangents.resize( numOfVertices );

                bin.read( reinterpret_cast<char*>( m_Meshes[_fileName]->RawData.pos.data() ), sizeof( XMFLOAT3 ) *  numOfVertices );
                bin.read( reinterpret_cast<char*>( m_Meshes[_fileName]->RawData.nor.data() ), sizeof( XMFLOAT3 ) *  numOfVertices );
                bin.read( reinterpret_cast<char*>( m_Meshes[_fileName]->RawData.uvs.data() ), sizeof( XMFLOAT3 ) *  numOfVertices );
                bin.read( reinterpret_cast<char*>( m_Meshes[_fileName]->RawData.tangents.data() ), sizeof( XMFLOAT3 ) *  numOfVertices );




                D3D11_BUFFER_DESC vertexBuffer_DESC;
                ZeroMemory( &vertexBuffer_DESC, sizeof( D3D11_BUFFER_DESC ) );
                D3D11_SUBRESOURCE_DATA vertexBufferData;
                ZeroMemory( &vertexBufferData, sizeof( vertexBufferData ) );
                vertexBuffer_DESC.Usage = D3D11_USAGE_DEFAULT;
                vertexBuffer_DESC.BindFlags = D3D11_BIND_VERTEX_BUFFER | D3D11_BIND_UNORDERED_ACCESS;
                vertexBuffer_DESC.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS;
                m_Meshes[_fileName]->D3DData.m_Stride[0] = sizeof( XMFLOAT3 );
                vertexBuffer_DESC.ByteWidth = numOfVertices* m_Meshes[_fileName]->D3DData.m_Stride[0];
                vertexBufferData.pSysMem = m_Meshes[_fileName]->RawData.pos.data();
                m_pDevice->CreateBuffer( &vertexBuffer_DESC, &vertexBufferData, &m_Meshes[_fileName]->D3DData.pPOS_VBuffer );

                vector<Vertex> verts;
                verts.resize( numOfVertices );
                for ( size_t eachVert = 0; eachVert < numOfVertices; eachVert++ ) {
                    verts[eachVert].pos = m_Meshes[_fileName]->RawData.pos[eachVert];
                    verts[eachVert].nor = m_Meshes[_fileName]->RawData.nor[eachVert];
                    verts[eachVert].uvw = m_Meshes[_fileName]->RawData.uvs[eachVert];
                    verts[eachVert].tangent = m_Meshes[_fileName]->RawData.tangents[eachVert];
                }
                m_Meshes[_fileName]->D3DData.m_Stride[1] = sizeof( Vertex );
                vertexBuffer_DESC.ByteWidth = (unsigned int)verts.size() * sizeof( Vertex );
                vertexBufferData.pSysMem = verts.data();
                m_pDevice->CreateBuffer( &vertexBuffer_DESC, &vertexBufferData, &m_Meshes[_fileName]->D3DData.pPOSNORUVTAN_VBuffer );



                uint32_t numOfIndices = 0;
                bin.read( reinterpret_cast<char*>( &numOfIndices ), sizeof numOfIndices );

                m_Meshes[_fileName]->RawData.indices.resize( numOfIndices );
                bin.read( reinterpret_cast<char*>( m_Meshes[_fileName]->RawData.indices.data() ), sizeof( uint32_t ) *  numOfIndices );

                //Create Index Buffer
                D3D11_BUFFER_DESC inputBuffer_DESC;
                ZeroMemory( &inputBuffer_DESC, sizeof( D3D11_BUFFER_DESC ) );
                D3D11_SUBRESOURCE_DATA indexBufferData;
                ZeroMemory( &indexBufferData, sizeof( indexBufferData ) );
                inputBuffer_DESC.Usage = D3D11_USAGE_IMMUTABLE;
                inputBuffer_DESC.BindFlags = D3D11_BIND_INDEX_BUFFER;
                inputBuffer_DESC.ByteWidth = (unsigned int)numOfIndices * sizeof( unsigned int );
                indexBufferData.pSysMem = m_Meshes[_fileName]->RawData.indices.data();
                m_pDevice->CreateBuffer( &inputBuffer_DESC, &indexBufferData, &m_Meshes[_fileName]->D3DData.pIBuffer );


            }
        }
    }
}

void CAssetManager::LoadTexture( string& _fileName ) {
    if ( m_Textures.count( _fileName ) ) {
        return;
    }
    auto fileName = g_MeshTextureDirectory + _fileName.c_str();
    wstring wfileName( fileName.begin(), fileName.end() );
    CreateDDSTextureFromFile( m_pDevice, wfileName.c_str(), nullptr, &m_Textures[_fileName] );
}