#ifndef Entity2D_h__
#define Entity2D_h__

#include <SimpleMath.h>
#include <string>
#include <objbase.h>
#include "IEntity.h"

namespace G2D {

    using namespace DirectX::SimpleMath;
    using namespace DirectX;

    class Entity2D : public IEntity {
    public:
        enum RenderState : unsigned int {
            NonRenderable = 0, Renderable = 1,
        };
        enum Entity2DType {
            eTYPE_Unknown = -1, eTYPE_GUITexture, eTYPE_GUIText, eTYPE_GUISlider
        };
        Entity2D( );
        Entity2D( XMFLOAT4X4& tranform );

        virtual ~Entity2D() = default;

        virtual void Render() = 0;
        virtual void Update( float dt ) = 0;

        // Reference Counting:
        virtual void AddRef();
        virtual void Release();

        float Layer() const;
        void Layer( float val );

        Vector2 Position() const;
        void Position( Vector2 val );

        float Rotation() const;
        void Rotation( float val );

        float Scale() const;
        void Scale( float val );

        bool CenterPrivot() const { return m_bCenterPrivot; }
        void CenterPrivot( bool val ) { m_bCenterPrivot = val; }

        bool Active() const { return (m_eState == Renderable); }
        void Active( bool val ) { m_eState = val ? Renderable : NonRenderable; }

        CGraphics2D::CusSpriteCB &SpriteData() { return m_cbSpriteData; }

        Entity2DType Type() const { return m_type; }
    private:
        // reference count
        unsigned int	m_unRefCount = 1;	// calling new gives the 'prime' reference

    protected:
        RenderState                 m_eState;
        Entity2DType                m_type;
        bool                        m_bCenterPrivot;
        float                       m_fTrueRot;
        // customer constant buffer
        CGraphics2D::CusSpriteCB    m_cbSpriteData;
    };
}

#endif // Entity2D_h__
