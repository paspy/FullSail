#include "stdafx.h"
#include "../stdafx.h"
#include "../Inc/ParticleCollection.h"
#include "../Inc/ShaderHandle.h"

namespace Renderer {

    void CParticleCollection::Init( ID3D11Device* device, ID3D11DeviceContext* context ) {
        assert( !m_isInit && "Already Initialized." );

        // Particle Vertex Structures
        // eSTRUCT_POSVELSIZAGETYP
        m_strides[eSTRUCT_POSVELSIZAGETYP] = sizeof( Emitter_POSVELSIZAGETYP );
        m_streamOutVSs[eSTRUCT_POSVELSIZAGETYP] = std::make_unique<CShaderHandle<ID3D11VertexShader>>( device, "CSO/VS_POSVELSIZAGETYP_SO.cso" );
        UINT NUM_POSVELSIZAGETYP = 5;
        D3D11_SO_DECLARATION_ENTRY SODECL_POSVELSIZAGETYP[5] = {
            // stream #,    semantic name,  semantic index, start component,    component count,    output slot
            { 0,           "POSITION",     0,              0,                  3,                  0 },
            { 0,           "VELOCITY",     0,              0,                  3,                  0 },
            { 0,           "SIZE",         0,              0,                  2,                  0 },
            { 0,           "AGE",          0,              0,                  1,                  0 },
            { 0,           "TYPE",         0,              0,                  1,                  0 },
        };
        
        // eTYPE_FIRE
        std::vector< std::wstring> fires = { L"../../FullSail/Resources/Particle/flare.dds" };
        m_streamOutGSs[eTYPE_FIRE] = std::make_unique<CShaderHandle<ID3D11GeometryShader>>( device, "CSO/GS_ParticleFireSO.cso", SODECL_POSVELSIZAGETYP, NUM_POSVELSIZAGETYP );
        m_drawVSs[eTYPE_FIRE] = std::make_unique<CShaderHandle<ID3D11VertexShader>>( device, "CSO/VS_ParticleFire.cso" );
        m_drawGSs[eTYPE_FIRE] = std::make_unique<CShaderHandle<ID3D11GeometryShader>>( device, "CSO/GS_BillboardQuad.cso" );
        m_drawPSs[eTYPE_FIRE] = std::make_unique<CShaderHandle<ID3D11PixelShader>>( device, "CSO/PS_ParticleFire.cso" );
        m_pRandomSRVs[eTYPE_FIRE] = FSUtilities::Misc::CreateRandomTexture1DSRV( device );
        m_pTexArraySRVs[eTYPE_FIRE] = FSUtilities::Misc::CreateTexture2DArraySRV( device, context, fires );


		//WAKE
        std::vector< std::wstring> wakes = 
        { 
            L"../../FullSail/Resources/Particle/Stream_01.dds"
        };
		m_streamOutGSs[eTYPE_WAKE] = std::make_unique<CShaderHandle<ID3D11GeometryShader>>(device, "CSO/GS_ParticleWakeSO.cso", SODECL_POSVELSIZAGETYP, NUM_POSVELSIZAGETYP);
		m_drawVSs[eTYPE_WAKE] = std::make_unique<CShaderHandle<ID3D11VertexShader>>(device, "CSO/VS_ParticleWake.cso");
		m_drawGSs[eTYPE_WAKE] = std::make_unique<CShaderHandle<ID3D11GeometryShader>>(device, "CSO/GS_BillboardQuad.cso");
		m_drawPSs[eTYPE_WAKE] = std::make_unique<CShaderHandle<ID3D11PixelShader>>(device, "CSO/PS_ParticleWake.cso");
		m_pRandomSRVs[eTYPE_WAKE] = FSUtilities::Misc::CreateRandomTexture1DSRV(device);
		m_pTexArraySRVs[eTYPE_WAKE] = FSUtilities::Misc::CreateTexture2DArraySRV(device, context, wakes);

        // Explode
        std::vector< std::wstring> explode =
        {
            L"../../FullSail/Resources/Particle/explode_01.dds",
            L"../../FullSail/Resources/Particle/explode_02.dds",
            L"../../FullSail/Resources/Particle/explode_03.dds",
            L"../../FullSail/Resources/Particle/explode_04.dds"
        };
        m_streamOutGSs[eTYPE_EXPLODE] = std::make_unique<CShaderHandle<ID3D11GeometryShader>>( device, "CSO/GS_ParticleExplodeSO.cso", SODECL_POSVELSIZAGETYP, NUM_POSVELSIZAGETYP );
        m_drawVSs[eTYPE_EXPLODE] = std::make_unique<CShaderHandle<ID3D11VertexShader>>( device, "CSO/VS_ParticleExplode.cso" );
        m_drawGSs[eTYPE_EXPLODE] = std::make_unique<CShaderHandle<ID3D11GeometryShader>>( device, "CSO/GS_BillboardQuad.cso" );
        m_drawPSs[eTYPE_EXPLODE] = std::make_unique<CShaderHandle<ID3D11PixelShader>>( device, "CSO/PS_ParticleExplode.cso" );
        m_pRandomSRVs[eTYPE_EXPLODE] = FSUtilities::Misc::CreateRandomTexture1DSRV( device );
        m_pTexArraySRVs[eTYPE_EXPLODE] = FSUtilities::Misc::CreateTexture2DArraySRV( device, context, explode );

        // Smoke
        std::vector< std::wstring> smoke =
        {
            L"../../FullSail/Resources/Particle/Stream_01.dds"
        };
        m_streamOutGSs[eTYPE_SMOKE] = std::make_unique<CShaderHandle<ID3D11GeometryShader>>( device, "CSO/GS_ParticleFireSO.cso", SODECL_POSVELSIZAGETYP, NUM_POSVELSIZAGETYP );
        m_drawVSs[eTYPE_SMOKE] = std::make_unique<CShaderHandle<ID3D11VertexShader>>( device, "CSO/VS_ParticleFire.cso" );
        m_drawGSs[eTYPE_SMOKE] = std::make_unique<CShaderHandle<ID3D11GeometryShader>>( device, "CSO/GS_BillboardQuad.cso" );
        m_drawPSs[eTYPE_SMOKE] = std::make_unique<CShaderHandle<ID3D11PixelShader>>( device, "CSO/PS_ParticleFire.cso" );
        m_pRandomSRVs[eTYPE_SMOKE] = FSUtilities::Misc::CreateRandomTexture1DSRV( device );
        m_pTexArraySRVs[eTYPE_SMOKE] = FSUtilities::Misc::CreateTexture2DArraySRV( device, context, smoke );

		// BoardingEffects
		std::vector< std::wstring> BoardingEffects =
		{
			L"../../FullSail/Resources/Particle/BoardingCannonball.dds",
			L"../../FullSail/Resources/Particle/BoardingSword.dds",
		};
		m_streamOutGSs[eTYPE_B_EFFECTS] = std::make_unique<CShaderHandle<ID3D11GeometryShader>>(device, "CSO/GS_ParticleBoardingEffect.cso", SODECL_POSVELSIZAGETYP, NUM_POSVELSIZAGETYP);
		m_drawVSs[eTYPE_B_EFFECTS] = std::make_unique<CShaderHandle<ID3D11VertexShader>>(device, "CSO/VS_ParticleBoardingEffect.cso");
		m_drawGSs[eTYPE_B_EFFECTS] = std::make_unique<CShaderHandle<ID3D11GeometryShader>>(device, "CSO/GS_RotatingBillBoard.cso");
		m_drawPSs[eTYPE_B_EFFECTS] = std::make_unique<CShaderHandle<ID3D11PixelShader>>(device, "CSO/PS_ParticleBoardingEffect.cso");
		m_pRandomSRVs[eTYPE_B_EFFECTS] = FSUtilities::Misc::CreateRandomTexture1DSRV(device);
		m_pTexArraySRVs[eTYPE_B_EFFECTS] = FSUtilities::Misc::CreateTexture2DArraySRV(device, context, BoardingEffects);

        // eTYPE_xxxx

        m_isInit = true;
    }

    void CParticleCollection::Destory() {
        assert( m_isInit && "Not Yet Initialize." );

        for ( auto i = 0; i < eSTRUCT_COUNT; i++ ) {
            m_streamOutVSs[i].reset();
        }

        for ( auto i = 0; i < eTYPE_COUNT; i++ ) {
            m_streamOutGSs[i].reset();
            m_drawVSs[i].reset();
            m_drawGSs[i].reset();
            m_drawPSs[i].reset();
            SAFE_RELEASE( m_pRandomSRVs[i] );
            SAFE_RELEASE( m_pTexArraySRVs[i] );
        }
        m_isInit = false;
    }

    CParticleCollection::CParticleCollection() :m_isInit(false) {

    }

    CParticleCollection::~CParticleCollection() {

    }

}