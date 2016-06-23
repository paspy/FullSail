#include "pch.h"
#include "GUITexture.h"

namespace G2D {

    GUITexture::GUITexture( std::string textureName, std::string textureFile, SimpleMath::Color color ) :
        m_sTextureName(textureName),
		m_sTextureFile(textureFile),
        m_tColor( color ) {
        m_type = eTYPE_GUITexture;

    }

    void GUITexture::Update( float dt ) {

    }

    void GUITexture::Render() {
        if ( m_eState == Renderable ) {
            FS_DEBUG::Assert(
                CGraphics2D::GetRef().DrawTexture(m_sTextureFile, Entity2D::Position(), Entity2D::Rotation(), Entity2D::Scale(), GUITexture::Color(), Entity2D::CenterPrivot(), &m_cbSpriteData ),
                ("GUITexture - Draw Texture Failed! Sprite: "+ m_sTextureFile +" Not Found.").c_str()
            );
        }
    }

	GUITexture::GUITexture(GUITexture const& copy)
	{
		m_sTextureName = copy.m_sTextureName;
		m_sTextureFile = copy.m_sTextureFile;
		m_tColor = copy.m_tColor;
		m_transform = copy.m_transform;
		m_mStaticPostions = copy.m_mStaticPostions;
		m_mOffsets = copy.m_mOffsets;

		m_bCenterPrivot = copy.m_bCenterPrivot;
		m_eState = copy.m_eState;
		m_fTrueRot = copy.m_fTrueRot;
		m_sName = copy.m_sName;
		m_type = copy.m_type;
		
	}

}

