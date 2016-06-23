#ifndef ParticleCollection_h__
#define ParticleCollection_h__

namespace Renderer {

    template<typename Shadertype> class CShaderHandle;
    class CParticleCollection {

    public:

        struct Emitter_POSVELSIZAGETYP {
            Emitter_POSVELSIZAGETYP() { SecureZeroMemory( this, sizeof( *this ) ); }
            DirectX::XMFLOAT3 InitialPos;
            DirectX::XMFLOAT3 InitialVel;
            DirectX::XMFLOAT2 Size;
            float Age;
            uint32_t Type;
        };

        enum ParticleStruct : UINT {
            eSTRUCT_POSVELSIZAGETYP = 0,

            eSTRUCT_COUNT
        };

        enum ParticleType : UINT {
            eTYPE_FIRE = 0,
			eTYPE_WAKE,
            eTYPE_EXPLODE,
            eTYPE_SMOKE,
			eTYPE_B_EFFECTS,
            eTYPE_COUNT
        };

        static CParticleCollection & GetRef() { static CParticleCollection instance; return instance; }

        void Init( ID3D11Device* device, ID3D11DeviceContext* context );
        void Destory();

    private:
        CParticleCollection();
        ~CParticleCollection();
        bool m_isInit;

    public:
        UINT                                                    m_strides[eSTRUCT_COUNT];
        std::unique_ptr<CShaderHandle<ID3D11VertexShader>>      m_streamOutVSs[eSTRUCT_COUNT];
        
        std::unique_ptr<CShaderHandle<ID3D11GeometryShader>>    m_streamOutGSs[eTYPE_COUNT];

        std::unique_ptr<CShaderHandle<ID3D11VertexShader>>      m_drawVSs[eTYPE_COUNT];
        std::unique_ptr<CShaderHandle<ID3D11GeometryShader>>    m_drawGSs[eTYPE_COUNT];
        std::unique_ptr<CShaderHandle<ID3D11PixelShader>>       m_drawPSs[eTYPE_COUNT];

        ID3D11ShaderResourceView*                               m_pRandomSRVs[eTYPE_COUNT];
        ID3D11ShaderResourceView*                               m_pTexArraySRVs[eTYPE_COUNT];


    protected:
        CParticleCollection( const CParticleCollection & ) = delete;
        CParticleCollection &operator=( const CParticleCollection & ) = delete;

    };

}


#endif // ParticleCollection_h__
