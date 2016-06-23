#include "pch.h"
#include "Entity2D.h"

namespace G2D {
    using namespace DirectX;

    Entity2D::Entity2D() :
        m_bCenterPrivot( false ), m_eState( Renderable ), m_type( eTYPE_Unknown ) {
        XMFLOAT4X4 i;
        XMStoreFloat4x4( &i, XMMatrixIdentity() );
        m_transform.SetLocalMatrix( i );
    }

    Entity2D::Entity2D( XMFLOAT4X4& tranform ) :
        IEntity( tranform ), m_eState( Renderable ) { }

    void Entity2D::AddRef() {
        FS_ASSERT( m_unRefCount != 0xFFFFFFFF, "Entity2D::AddRef - max ref count has been exceeded" );
        m_unRefCount++;
    }

    void Entity2D::Release() {
        m_unRefCount--;
        if ( m_unRefCount == 0 ) {
            delete this;
        }
    }

    float Entity2D::Layer() const {
        XMVECTOR out, tmp1, tmp2;
        XMMatrixDecompose( &tmp1, &tmp2, &out, XMLoadFloat4x4( &m_transform.GetLocalMatrix() ) );
        return XMVectorGetZ( out );
    }

    void Entity2D::Layer( float val ) {
        XMVECTOR scale;
        XMVECTOR quat;
        XMVECTOR trans;
        XMVECTOR zero = XMVectorSet( 0.0f, 0.0f, 0.0f, 1.0f );
        XMFLOAT4X4 result;
        XMMatrixDecompose( &scale, &quat, &trans, XMLoadFloat4x4( &m_transform.GetLocalMatrix() ) );
        trans = XMVectorSetZ( trans, val );

        XMStoreFloat4x4( &result, XMMatrixAffineTransformation( scale, zero, quat, trans ) );
        m_transform.SetLocalMatrix( result );
    }

    Vector2 Entity2D::Position() const {
        XMVECTOR out, tmp1, tmp2;
        XMMatrixDecompose( &tmp1, &tmp2, &out, XMLoadFloat4x4( &m_transform.GetLocalMatrix() ) );
        return Vector2( XMVectorGetX( out ), XMVectorGetY( out ) );
    }

    void Entity2D::Position( Vector2 val ) {
        XMVECTOR scale;
        XMVECTOR quat;
        XMVECTOR trans;
        XMVECTOR zero = XMVectorSet( 0.0f, 0.0f, 0.0f, 1.0f );
        XMFLOAT4X4 result;
        XMMatrixDecompose( &scale, &quat, &trans, XMLoadFloat4x4( &m_transform.GetLocalMatrix() ) );
        trans = XMVectorSetX( trans, val.x );
        trans = XMVectorSetY( trans, val.y );

        XMStoreFloat4x4( &result, XMMatrixAffineTransformation( scale, zero, quat, trans ) );
        m_transform.SetLocalMatrix( result );
    }

    float Entity2D::Rotation() const {
        XMVECTOR out, tmp1, tmp2;
        XMVECTOR zAxis = XMVectorSet( 0.0f, 0.0f, 1.0f, 0.0f );
        float result;
        XMMatrixDecompose( &tmp1, &out, &tmp2, XMLoadFloat4x4( &m_transform.GetLocalMatrix() ) );
        XMQuaternionToAxisAngle( &zAxis, &result, out );
        //return result;
        return m_fTrueRot;
    }

    void Entity2D::Rotation( float val ) {
        XMVECTOR scale;
        XMVECTOR quat;
        XMVECTOR trans;
        XMVECTOR zero = XMVectorSet( 0.0f, 0.0f, 0.0f, 1.0f );
        XMVECTOR zAxis = XMVectorSet( 0.0f, 0.0f, 1.0f, 0.0f );
        XMFLOAT4X4 result;
        XMMatrixDecompose( &scale, &quat, &trans, XMLoadFloat4x4( &m_transform.GetLocalMatrix() ) );
        quat = XMQuaternionRotationNormal( zAxis, val );

        XMStoreFloat4x4( &result, XMMatrixAffineTransformation( scale, zero, quat, trans ) );
        m_transform.SetLocalMatrix( result );
        m_fTrueRot = val;
    }

    float Entity2D::Scale() const {
        XMVECTOR out, tmp1, tmp2;
        XMMatrixDecompose( &out, &tmp1, &tmp2, XMLoadFloat4x4( &m_transform.GetLocalMatrix() ) );
        return ( XMVectorGetX( out ) + XMVectorGetY( out ) ) * 0.5f;
    }

    void Entity2D::Scale( float val ) {
        XMVECTOR scale;
        XMVECTOR quat;
        XMVECTOR trans;
        XMVECTOR zero = XMVectorSet( 0.0f, 0.0f, 0.0f, 1.0f );
        XMFLOAT4X4 result;
        XMMatrixDecompose( &scale, &quat, &trans, XMLoadFloat4x4( &m_transform.GetLocalMatrix() ) );
        scale = XMVectorSetX( scale, val );
        scale = XMVectorSetY( scale, val );
        scale = XMVectorSetZ( scale, val );

        XMStoreFloat4x4( &result, XMMatrixAffineTransformation( scale, zero, quat, trans ) );
        m_transform.SetLocalMatrix( result );
    }

}

