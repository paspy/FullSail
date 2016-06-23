#include "pch.h"
#include "GUISlider.h"

namespace G2D {

    GUISlider::GUISlider(std::string frontTexture, std::string frontTextureFile, std::string backTexture, std::string backTextureFile) :
        m_sFrontTextureName( frontTexture ),
		m_sFrontTextureFile( frontTextureFile ),
        m_sBackTextureName( backTexture ),
		m_sBackTextureFile( backTextureFile ),
        m_fValue( 1.0f ),
        m_bVertical( false ),
        m_bAnimated( false ),
        m_tBackColor( { 1.f, 1.f, 1.f, 1.f } ),
        m_tFrontColor( { 1.f, 1.f, 1.f, 1.f } ) {

        m_type = eTYPE_GUISlider;
    }

    void GUISlider::Update( float dt ) {
        auto & backTextureInfo = CAssetManager::GetRef().GetSpriteInfo( m_sBackTextureFile );
        //static float totalDelta = 0;

        if ( m_bAnimated ) {
            //totalDelta += dt;
            //if ( totalDelta <= m_fValue ) {
            if ( m_bVertical )
                m_tRectangle.Resize( backTextureInfo.size.x, backTextureInfo.size.y*m_fValue );
            else
                m_tRectangle.Resize( backTextureInfo.size.x*m_fValue, backTextureInfo.size.y );
            //}

        } else {
            if ( m_bVertical )
                m_tRectangle.Resize( backTextureInfo.size.x, backTextureInfo.size.y*m_fValue );
            else
                m_tRectangle.Resize( backTextureInfo.size.x*m_fValue, backTextureInfo.size.y );
        }


    }

    void GUISlider::Render() {
        if ( m_eState == Renderable ) {
            // draw back
            FS_DEBUG::Assert(
                CGraphics2D::GetRef().DrawTexture( 
                    m_sBackTextureFile, 
                    m_tRectangle, 
                    Entity2D::Position(), 
                    Entity2D::Rotation(), 
                    Entity2D::Scale(), 
                    m_tBackColor, 
                    Entity2D::CenterPrivot() 
                ),
                ("GUISlider - Draw Back Texture Failed! "+ m_sBackTextureFile +" Not Found.").c_str()
            );

            // draw front
            FS_DEBUG::Assert(
                CGraphics2D::GetRef().DrawTexture( 
                    m_sFrontTextureFile, 
                    Entity2D::Position(),
                    Entity2D::Rotation(), 
                    Entity2D::Scale(), 
					m_tFrontColor, 
                    Entity2D::CenterPrivot() 
                ),
                ( "GUISlider - Draw Front Texture Failed! " + m_sFrontTextureFile + " Not Found." ).c_str()
            );

        }
    }


}