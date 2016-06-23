#pragma once

#include "RenderNode.h"
#include "Light.h"
#include "ConstantBuffer.h"
#include "../../FSUtilities/Miscellaneous.h"

class CAssetManager;

namespace Renderer {
    using namespace FSUtilities;
    class CMaterial;
    class CInputLayoutManager;

    class CTerrain : public IRenderNode {
        friend class CView;
        friend class CRendererController;

    public:
        RENDERER_API CTerrain( const wchar_t*  fileName );
        RENDERER_API ~CTerrain();

        virtual void Begin( IRenderNode* pCurrentView ) override;
        virtual void End( IRenderNode* pCurrentView ) override;
        void SetPerTerrainData( DirectX::XMFLOAT4X4& viewProj );

    private:
        static CInputLayoutManager&                 m_InputLayoutManager;

        unsigned int                                m_Stride;
        unsigned int		                        m_uNumofIndices;

        float                                       m_heightMapScale;
        float                                       m_heightMapResolution;
        float                                       m_CellSpacing;

        //cbPerTerrain                                m_PerTerrainData;

        std::vector<DirectX::PackedVector::HALF>    m_heightMapData;

        ID3D11InputLayout*	                        m_d3dInputLayout;
        ID3D11Buffer*		                        m_d3dVertexBuffer;
        ID3D11Buffer*		                        m_d3dIndexBuffer;
        ID3D11Buffer*				                m_d3dPerTerrainCBuffer;

        ID3D11ShaderResourceView*	                m_heightMapSRV;
        std::vector<ID3D11ShaderResourceView*>      m_layerMapArraySRV; // sub-0 is blendMap

    };
}