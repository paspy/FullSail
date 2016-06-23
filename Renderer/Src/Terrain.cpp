#include "stdafx.h"
#include "..\stdafx.h"
#include "..\Inc\Terrain.h"
#include "..\Inc\InputLayoutManager.h"
#include "..\Inc\RendererController.h"
#include "..\inc\CommonStateObjects.h"
#include "..\Inc\View.h"


namespace Renderer {
    using namespace FSUtilities;

    CTerrain::CTerrain( const wchar_t* fileName ) :
        m_d3dInputLayout( nullptr ),
        m_d3dVertexBuffer( nullptr ),
        m_d3dIndexBuffer( nullptr ),
        m_d3dPerTerrainCBuffer( nullptr ),
        m_heightMapSRV( nullptr ) {

        auto device = CRendererController::m_deviceResources->GetD3DDevice();

        bool r = Misc::LoadFromFSTerrain( fileName, device, m_heightMapScale, m_heightMapResolution, m_CellSpacing, &m_d3dVertexBuffer, &m_d3dIndexBuffer, m_uNumofIndices, &m_heightMapSRV, m_heightMapData );
        m_Stride = sizeof( TerrainLoader::TerrainVertex );
        
        m_d3dInputLayout = m_InputLayoutManager.inputLayouts[CInputLayoutManager::eVertex_POSUV0UV1];

        assert( r&&"Load Terrain Failed!" );

        std::vector<std::wstring> layerFilenames;
        layerFilenames.push_back( L"../../FullSail/Resources/Terrain/level1_bm.dds" );
        layerFilenames.push_back( L"../../FullSail/Resources/Terrain/Textures/grass.dds" );
        layerFilenames.push_back( L"../../FullSail/Resources/Terrain/Textures/darkdirt.dds" );
        layerFilenames.push_back( L"../../FullSail/Resources/Terrain/Textures/stone.dds" );
        layerFilenames.push_back( L"../../FullSail/Resources/Terrain/Textures/lightdirt.dds" );
        layerFilenames.push_back( L"../../FullSail/Resources/Terrain/Textures/snow.dds" );

        m_layerMapArraySRV = Misc::CreateTexture2DArraySRV( device, layerFilenames );

        //Create CBuffer
        D3D11_BUFFER_DESC bd;
        ZeroMemory( &bd, sizeof( bd ) );
        bd.Usage = D3D11_USAGE_DYNAMIC;
        bd.ByteWidth = sizeof( m_PerTerrainData );
        bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
        CRendererController::m_deviceResources->GetD3DDevice()->CreateBuffer( &bd, nullptr, &m_d3dPerTerrainCBuffer );


    }

    CTerrain::~CTerrain() {
        SAFE_RELEASE( m_d3dVertexBuffer );
        SAFE_RELEASE( m_d3dIndexBuffer );
        SAFE_RELEASE( m_d3dPerTerrainCBuffer );
        SAFE_RELEASE( m_heightMapSRV );
        for ( auto &srv : m_layerMapArraySRV ) {
            SAFE_RELEASE( srv );
        }
    }

    void CTerrain::Begin( IRenderNode* pCurrentView ) {
        CView& view = (CView&)( *pCurrentView );
        UINT offset = 0;
        static auto deviceContextPtr = CRendererController::m_deviceResources->GetD3DDeviceContext();
        deviceContextPtr->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_4_CONTROL_POINT_PATCHLIST );
        deviceContextPtr->IASetInputLayout( m_d3dInputLayout );
        SetPerTerrainData( view.GetViewProjMatrix() );
        
        deviceContextPtr->IASetVertexBuffers( 0, 1, &m_d3dVertexBuffer, &m_Stride, &offset );
        deviceContextPtr->IASetIndexBuffer( m_d3dIndexBuffer, DXGI_FORMAT_R16_UINT, 0 );

        deviceContextPtr->PSSetSamplers( 0, 1, &CCommonStateObjects::m_samplerStates[CCommonStateObjects::TrilinearWrap] );

        deviceContextPtr->VSSetSamplers( 1, 1, &CCommonStateObjects::m_samplerStates[CCommonStateObjects::LinearClamp] );
        deviceContextPtr->DSSetSamplers( 1, 1, &CCommonStateObjects::m_samplerStates[CCommonStateObjects::LinearClamp] );
        deviceContextPtr->PSSetSamplers( 1, 1, &CCommonStateObjects::m_samplerStates[CCommonStateObjects::LinearClamp] );

        deviceContextPtr->VSSetShaderResources( 0, 1, &m_layerMapArraySRV[0] );
        deviceContextPtr->HSSetShaderResources( 0, 1, &m_layerMapArraySRV[0] );
        deviceContextPtr->DSSetShaderResources( 0, 1, &m_layerMapArraySRV[0] );
        deviceContextPtr->PSSetShaderResources( 0, 1, &m_layerMapArraySRV[0] );

        deviceContextPtr->VSSetShaderResources( 1, 1, &m_heightMapSRV );
        deviceContextPtr->HSSetShaderResources( 1, 1, &m_heightMapSRV );
        deviceContextPtr->DSSetShaderResources( 1, 1, &m_heightMapSRV );
        deviceContextPtr->PSSetShaderResources( 1, 1, &m_heightMapSRV );

        for ( UINT i = 1; i < (UINT)m_layerMapArraySRV.size(); i++ ) {
            deviceContextPtr->PSSetShaderResources( i + 1, 1, &m_layerMapArraySRV[i] );
        }

        deviceContextPtr->DrawIndexed( m_uNumofIndices, 0, 0 );

    }

    void CTerrain::End( IRenderNode* pCurrentView ) {
        static auto deviceContextPtr = CRendererController::m_deviceResources->GetD3DDeviceContext();
        deviceContextPtr->IASetInputLayout( nullptr );
        unsigned int strid = 0;
        unsigned int offset = 0;
        ID3D11Buffer * vs[] = { nullptr };
        deviceContextPtr->IASetVertexBuffers( 0, 1, vs, &strid, &offset );
        deviceContextPtr->IASetIndexBuffer( nullptr, DXGI_FORMAT_R32_UINT, 0 );
        // set back to default 
        deviceContextPtr->RSSetState( 0 );
        deviceContextPtr->HSSetShader( 0, 0, 0 );
        deviceContextPtr->DSSetShader( 0, 0, 0 );
        deviceContextPtr->IASetPrimitiveTopology( D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST );
        CView& view = (CView&)( *pCurrentView );
    }

    void CTerrain::SetPerTerrainData( DirectX::XMFLOAT4X4& viewProj ) {
        auto deviceContextPtr = CRendererController::m_deviceResources->GetD3DDeviceContext();

        m_PerTerrainData.gMinDist = 200.0f;
        m_PerTerrainData.gMaxDist = 1000.0f;
        m_PerTerrainData.gMinTess = 0.0f;
        m_PerTerrainData.gMaxTess = 6.0f;
        m_PerTerrainData.gTexelCellSpaceU = 1.0f / m_heightMapResolution;
        m_PerTerrainData.gTexelCellSpaceV = 1.0f / m_heightMapResolution;
        m_PerTerrainData.gWorldCellSpace = m_CellSpacing;

        Misc::ExtractFrustumPlanes( m_PerTerrainData.gWorldFrustumPlanes, XMLoadFloat4x4( &viewProj ) );

        D3D11_MAPPED_SUBRESOURCE edit;
        deviceContextPtr->Map( m_d3dPerTerrainCBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &edit );
        *(cbPerTerrain*)edit.pData = m_PerTerrainData;
        deviceContextPtr->Unmap( m_d3dPerTerrainCBuffer, 0 );
        deviceContextPtr->VSSetConstantBuffers( m_PerTerrainData.REGISTER_SLOT, 1, &m_d3dPerTerrainCBuffer );
        deviceContextPtr->DSSetConstantBuffers( m_PerTerrainData.REGISTER_SLOT, 1, &m_d3dPerTerrainCBuffer );
        deviceContextPtr->HSSetConstantBuffers( m_PerTerrainData.REGISTER_SLOT, 1, &m_d3dPerTerrainCBuffer );
        deviceContextPtr->PSSetConstantBuffers( m_PerTerrainData.REGISTER_SLOT, 1, &m_d3dPerTerrainCBuffer );

    }

}
