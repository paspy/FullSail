#ifndef Particle_h__
#define Particle_h__
#include "RenderNode.h"
#include "ParticleCollection.h"
#include "ConstantBuffer.h"

namespace Renderer {
    using namespace DirectX::SimpleMath;
    class CInputLayoutManager;

    class CParticle : public IRenderNode {
        friend class CView;
        friend class CRendererController;
    public:
        enum eBlendMode : UINT {
            eBM_Additive = 0,
            eBM_AlphaBlend
        };

        
    public:
        RENDERER_API CParticle(
            uint32_t _id,
            uint32_t _maxParticle,
            CParticleCollection::ParticleType _particleType,
            bool _playAwake = false,
            Vector3 _pos = Vector3::Zero,
            Vector3 _dir = Vector3::Zero,
            CParticleCollection::ParticleStruct _structType = CParticleCollection::eSTRUCT_POSVELSIZAGETYP );

        RENDERER_API ~CParticle();

        RENDERER_API void Play();
        RENDERER_API void Stop();
        RENDERER_API void Reset();

        RENDERER_API float GetAge() const { return m_fAge; }
        RENDERER_API uint32_t Id() const { return m_uId; }
        RENDERER_API GUID GetGUID() const { return m_tGUID; }
        RENDERER_API std::wstring Name() const { return m_sName; }
        RENDERER_API Vector3 EmitPositionW() const { return m_vEmitPositionW; }
        RENDERER_API Vector3 EmitDirectionW() const { return m_vEmitDirectionW; }
        RENDERER_API Vector3 EmitVelocity() const { return m_vEmitVelocity; }
        RENDERER_API Vector2 EmitSizeW() const { return m_vEmitSizeW; }
        RENDERER_API Vector4 EmitColor() const { return m_vEmitColor; }
        RENDERER_API float EmitSpeed() const { return m_fEmitSpeed; }
        RENDERER_API float ParticleLife() const { return m_fParticleLife; }
        RENDERER_API float MaxTexCount() const { return m_fMaxTexCount; }
        RENDERER_API eBlendMode BlendMode() const { return m_eBlendMode; }
		RENDERER_API Matrix WorldMat() const { return m_worldMat; }
		RENDERER_API Matrix RotationMat() const { return m_rotationMat; }
        RENDERER_API std::function<void( float )> FuncUpdate() const { return m_funcUpdate; }

        RENDERER_API void FuncUpdate( std::function<void( float )> val ) { m_funcUpdate = val; }
		RENDERER_API void WorldMat(Matrix val) { m_worldMat = val; }
		RENDERER_API void RotationMat( Matrix val ) { m_rotationMat = val; }
        RENDERER_API void Id( uint32_t val ) { m_uId = val; }
        RENDERER_API void MaxTexCount( float val ) { m_fMaxTexCount = val; }
        RENDERER_API void EmitSizeW( Vector2 val ) { m_vEmitSizeW = val; }
        RENDERER_API void EmitSpeed( float val ) { m_fEmitSpeed = val; }
        RENDERER_API void EmitPositionW( Vector3 val ) { m_vEmitPositionW = val; }
        RENDERER_API void EmitDirectionW( Vector3 val ) { m_vEmitDirectionW = val; }
        RENDERER_API void EmitVelocity( Vector3 val ) { m_vEmitVelocity = val; }
        RENDERER_API void EmitColor( Vector4 val ) { m_vEmitColor = val; }
        RENDERER_API void ParticleLife( float val ) { m_fParticleLife = val; }
        RENDERER_API void BlendMode( eBlendMode val ) { m_eBlendMode = val; }
        RENDERER_API void Name( std::wstring val ) { m_sName = val; }

        RENDERER_API void Update( float _dt );
        virtual void Begin( IRenderNode* pCurrentView = nullptr ) override final;
        virtual void End( IRenderNode* pCurrentView = nullptr )   override final;

        void SetPerParticleData();

    private:
        std::wstring                    m_sName;
        GUID                            m_tGUID;
        uint32_t                        m_uId;

        ID3D11Device*                   m_pDevice;
        ID3D11DeviceContext*            m_pContext;
        ID3D11InputLayout*	            m_pInputLayout;

		Matrix                          m_worldMat;
		Matrix                          m_rotationMat;
        Vector3                         m_vEmitPositionW;
        Vector3                         m_vEmitDirectionW;
        Vector3							m_vEmitVelocity;
        Vector2                         m_vEmitSizeW;
        Vector4                         m_vEmitColor;
        
        std::function<void( float )>    m_funcUpdate;

        UINT                            m_offset;
        UINT                            m_uiParticleType;
        UINT                            m_uiStructType;
        uint32_t                        m_uiMaxParticles;

        cbPerParticle                   m_cbPerPaticleData;
        ID3D11Buffer*		            m_cbPerPaticleBuffer;
        ID3D11Buffer*		            m_pEmiiterVB;
        ID3D11Buffer*		            m_pDrawingVB;
        ID3D11Buffer*		            m_pStreamOutVB;

        bool                            m_bFirstRun;
        bool                            m_bPlay;
        float                           m_fAge;
        float                           m_fParticleLife;
        float                           m_fEmitSpeed;
        float                           m_fMaxTexCount;
        eBlendMode                      m_eBlendMode;


        static CInputLayoutManager&     m_refInputLayoutManager;
        static CParticleCollection&     m_refParticleCollection;

        //***************Allocator Overloads****************//
    public:
        RENDERER_API  void * operator new ( size_t size, const char *file, int line ) {
            return FSMM::MemoryManager::GetInstance()->Allocate( static_cast<unsigned int>( size ), 0U, file, line );
        }

        RENDERER_API inline void * operator new ( size_t size ) {
            return FSMM::MemoryManager::GetInstance()->Allocate( static_cast<unsigned int>( size ) );
        }

        RENDERER_API inline void * operator new[] ( size_t size, const char *file, int line ) {
            return FSMM::MemoryManager::GetInstance()->Allocate( static_cast<unsigned int>( size ), 0U, file, line );
        }

            RENDERER_API inline void * operator new[] ( size_t size ) {
            return FSMM::MemoryManager::GetInstance()->Allocate( static_cast<unsigned int>( size ) );
        }

            RENDERER_API inline void operator delete ( void * p ) {
            FSMM::MemoryManager::GetInstance()->DeAllocate( p );
        }

        RENDERER_API inline void operator delete ( void * p, const char *file, int line ) {
            FSMM::MemoryManager::GetInstance()->DeAllocate( p );
        }

        RENDERER_API inline void operator delete[] ( void * p ) {
            FSMM::MemoryManager::GetInstance()->DeAllocate( p );
        }

            RENDERER_API inline void operator delete[] ( void * p, const char *file, int line ) {
            FSMM::MemoryManager::GetInstance()->DeAllocate( p );
        }

    };

}
#endif // Particle_h__
