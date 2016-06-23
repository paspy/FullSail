#include "stdafx.h"
#include "..\stdafx.h"
#include "..\Inc\Particle.h"
#include "..\Inc\InputLayoutManager.h"
#include "..\Inc\ShaderHandle.h"
#include "..\inc\CommonStateObjects.h"
#include "..\Inc\RendererController.h"
#include "..\Inc\View.h"

namespace Renderer {
    CInputLayoutManager& CParticle::m_refInputLayoutManager = CInputLayoutManager::GetRef();
    CParticleCollection& CParticle::m_refParticleCollection = CParticleCollection::GetRef();

    RENDERER_API CParticle::CParticle(
        uint32_t _id,
        uint32_t _maxParticle,
        CParticleCollection::ParticleType _particleType,
        bool _playAwake,
        Vector3 _pos,
        Vector3 _dir,
        CParticleCollection::ParticleStruct _structType ) :
        m_uId( _id ),
        m_pDevice( CRendererController::m_deviceResources->GetD3DDevice() ),
        m_pContext( CRendererController::m_deviceResources->GetD3DDeviceContext() ),
        m_offset( 0 ),
        m_bFirstRun( true ),
        m_bPlay( _playAwake ),
        m_fAge( 0.0f ),
        m_uiMaxParticles( _maxParticle ),
        m_uiParticleType( (UINT)_particleType ),
        m_uiStructType( (UINT)_structType ),
        m_vEmitPositionW( _pos ),
        m_vEmitDirectionW( _dir ),
        m_vEmitSizeW( Vector2( 1.0f, 1.0f ) ),
        m_vEmitColor( Vector4::Zero ),
        m_worldMat( Matrix::Identity ),
        m_fParticleLife( 1.0f ),
        m_fMaxTexCount( 1.0f ),
        m_eBlendMode( eBM_Additive ) {

        CoCreateGuid( &m_tGUID );
        OLECHAR* bstrGuid;
        StringFromCLSID( m_tGUID, &bstrGuid );

        m_sName = std::wstring( bstrGuid );

        CoTaskMemFree( bstrGuid );

        D3D11_BUFFER_DESC cbbd;
        SecureZeroMemory( &cbbd, sizeof( D3D11_BUFFER_DESC ) );
        cbbd.Usage = D3D11_USAGE_DYNAMIC;
        cbbd.ByteWidth = sizeof( cbPerParticle );
        cbbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        cbbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
        m_pDevice->CreateBuffer( &cbbd, NULL, &m_cbPerPaticleBuffer );

        // Create the buffer to kick-off the particle system.
        D3D11_BUFFER_DESC vbd;
        D3D11_SUBRESOURCE_DATA vinitData;
        SecureZeroMemory( &vbd, sizeof( D3D11_BUFFER_DESC ) );
        SecureZeroMemory( &vinitData, sizeof( D3D11_SUBRESOURCE_DATA ) );
        vbd.Usage = D3D11_USAGE_DEFAULT;
        vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;

        switch ( m_uiStructType ) {
        case CParticleCollection::eSTRUCT_POSVELSIZAGETYP:
        {
            m_pInputLayout = m_refInputLayoutManager.inputLayouts[CInputLayoutManager::ePTVertex_POSVELSIZAGETYP];
            // The initial particle emitter has type 0 and age 0.
            vbd.ByteWidth = sizeof( CParticleCollection::Emitter_POSVELSIZAGETYP ) * 1;
            CParticleCollection::Emitter_POSVELSIZAGETYP p;
            p.Age = 0.0f;
            p.Type = 0;
            vinitData.pSysMem = &p;
            m_pDevice->CreateBuffer( &vbd, &vinitData, &m_pEmiiterVB );
            vbd.ByteWidth = sizeof( CParticleCollection::Emitter_POSVELSIZAGETYP ) * m_uiMaxParticles;
            break;
        }
        default:
            assert( false && "Unknown Particle Structure Provided." );
            break;
        }

        // Create the ping-pong buffers for stream-out and drawing.
        vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER | D3D11_BIND_STREAM_OUTPUT;
        m_pDevice->CreateBuffer( &vbd, 0, &m_pStreamOutVB );
        m_pDevice->CreateBuffer( &vbd, 0, &m_pDrawingVB );
    }

    RENDERER_API CParticle::~CParticle() {
        SAFE_RELEASE( m_cbPerPaticleBuffer );
        SAFE_RELEASE( m_pEmiiterVB );
        SAFE_RELEASE( m_pDrawingVB );
        SAFE_RELEASE( m_pStreamOutVB );
    }


    RENDERER_API void CParticle::Play() {
        if ( !m_bPlay )
            m_bPlay = true;
    }

    RENDERER_API void CParticle::Stop() {
        if ( m_bPlay )
            m_bPlay = false;
    }

    RENDERER_API void CParticle::Reset() {
        m_fAge = 0.0f;
        m_bFirstRun = true;
    }

    RENDERER_API void CParticle::Update( float _dt ) {
        if ( !m_bPlay ) return;
        if ( m_funcUpdate ) {
            m_funcUpdate( _dt );
        }
        m_fAge += _dt;
    }

    void CParticle::Begin( IRenderNode* pCurrentView ) {

        if ( !m_bPlay ) return;
        m_pContext->IASetInputLayout( m_pInputLayout );
        m_pContext->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_POINTLIST );

        m_pContext->PSSetShaderResources( 10, 1, &m_refParticleCollection.m_pTexArraySRVs[m_uiParticleType] );
        m_pContext->GSSetShaderResources( 11, 1, &m_refParticleCollection.m_pRandomSRVs[m_uiParticleType] );

        SetPerParticleData();

        //////////////////////////////////////////////////////////////////////////
        // Stream-Out Stage
        m_pContext->VSSetShader( m_refParticleCollection.m_streamOutVSs[m_uiStructType].get()->m_shader, nullptr, 0 );
        m_pContext->GSSetShader( m_refParticleCollection.m_streamOutGSs[m_uiParticleType].get()->m_shader, nullptr, 0 );
        m_pContext->PSSetShader( nullptr, nullptr, 0 );

        m_pContext->OMSetDepthStencilState( CCommonStateObjects::m_depthStencilStates[CCommonStateObjects::DepthNone_DSS], 0 );

        // Draw the current particle list using stream-out only to update them.  
        // The updated vertices are streamed-out to the target VB. 
        m_pContext->SOSetTargets( 1, &m_pStreamOutVB, &m_offset );
        if ( m_bFirstRun ) {
            m_pContext->IASetVertexBuffers( 0, 1, &m_pEmiiterVB, &m_refParticleCollection.m_strides[m_uiStructType], &m_offset );
            m_pContext->Draw( 1, 0 );
            m_bFirstRun = false;
        } else {
            m_pContext->IASetVertexBuffers( 0, 1, &m_pDrawingVB, &m_refParticleCollection.m_strides[m_uiStructType], &m_offset );
            m_pContext->DrawAuto();
        }
        // done streaming-out--unbind the vertex buffer
        ID3D11Buffer* emptyBufferArr[1] = { nullptr };
        m_pContext->SOSetTargets( 1, emptyBufferArr, &m_offset );

        //////////////////////////////////////////////////////////////////////////
        // Drawing Stage
        m_pContext->VSSetShader( m_refParticleCollection.m_drawVSs[m_uiParticleType].get()->m_shader, nullptr, 0 );
        m_pContext->GSSetShader( m_refParticleCollection.m_drawGSs[m_uiParticleType].get()->m_shader, nullptr, 0 );
        m_pContext->PSSetShader( m_refParticleCollection.m_drawPSs[m_uiParticleType].get()->m_shader, nullptr, 0 );
        m_pContext->OMSetDepthStencilState( CCommonStateObjects::m_depthStencilStates[CCommonStateObjects::DepthRead_DSS], 0 );

        switch ( m_eBlendMode ) {
        case eBM_Additive:
            m_pContext->OMSetBlendState( CCommonStateObjects::m_blendStates[CCommonStateObjects::Additive_BS], nullptr, 0xffffffff );
            break;
        case eBM_AlphaBlend:
            m_pContext->OMSetBlendState( CCommonStateObjects::m_blendStates[CCommonStateObjects::PaticleAlphaBlend_BS], nullptr, 0xffffffff );
            break;
        default:
            break;
        }

        // ping-pong the vertex buffers
        std::swap( m_pDrawingVB, m_pStreamOutVB );
        // Draw the updated particle system we just streamed-out.
        m_pContext->IASetVertexBuffers( 0, 1, &m_pDrawingVB, &m_refParticleCollection.m_strides[m_uiStructType], &m_offset );
        m_pContext->DrawAuto();

    }

    void CParticle::End( IRenderNode* pCurrentView ) {
        float blendFactor[] = { 0.0f, 0.0f, 0.0f, 0.0f };
        ID3D11ShaderResourceView* nullSRV[] = { nullptr };
        // restore default
        m_pContext->GSSetShader( nullptr, nullptr, 0 );
        m_pContext->OMSetBlendState( nullptr, blendFactor, 0xffffffff );
        m_pContext->PSSetShaderResources( 10, 1, nullSRV );
        m_pContext->GSSetShaderResources( 11, 1, nullSRV );
        m_pContext->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );
    }

    void CParticle::SetPerParticleData() {
        m_cbPerPaticleData.gEmitPosW = float4( m_vEmitPositionW.x, m_vEmitPositionW.y, m_vEmitPositionW.z, 1.0f );
        m_cbPerPaticleData.gEmitDirW = float4( m_vEmitDirectionW.x, m_vEmitDirectionW.y, m_vEmitDirectionW.z, 1.0f );
        m_cbPerPaticleData.gEmitVelocity = m_vEmitVelocity;
        m_cbPerPaticleData.gParticleLife = m_fParticleLife;
        m_cbPerPaticleData.gEmitSpeed = m_fEmitSpeed;
        m_cbPerPaticleData.gParticleSize = m_vEmitSizeW;
        m_cbPerPaticleData.gEmitColor = m_vEmitColor;
        m_cbPerPaticleData.gTexArrIndex = float( FSUtilities::Misc::RandInt( 0, int( m_fMaxTexCount ) ) );
        m_cbPerPaticleData.gParticleWorld = m_worldMat;
        m_cbPerPaticleData.gRotationMat = m_rotationMat;

        D3D11_MAPPED_SUBRESOURCE mappedRes;
        m_pContext->Map( m_cbPerPaticleBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedRes );
        memcpy_s( mappedRes.pData, sizeof( cbPerParticle ), &m_cbPerPaticleData, sizeof( cbPerParticle ) );
        m_pContext->Unmap( m_cbPerPaticleBuffer, 0 );

        m_pContext->VSSetConstantBuffers( m_cbPerPaticleData.REGISTER_SLOT, 1, &m_cbPerPaticleBuffer );
        m_pContext->GSSetConstantBuffers( m_cbPerPaticleData.REGISTER_SLOT, 1, &m_cbPerPaticleBuffer );
        m_pContext->PSSetConstantBuffers( m_cbPerPaticleData.REGISTER_SLOT, 1, &m_cbPerPaticleBuffer );
        m_pContext->RSSetState( NULL );

    }
}