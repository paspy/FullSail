#include "pch.h"
#include "GUIText.h"

namespace G2D {
    GUIText::GUIText( std::wstring text, SimpleMath::Color color, std::string font ) :
        m_sText( text ),
        m_tColor( color ),
        m_sFont( font ) {
        m_type = eTYPE_GUIText;

    }

    void GUIText::Update( float _dt ) {

    }

    void GUIText::Render() {
        if ( m_eState == Renderable ) {
            FS_DEBUG::Assert(
                CGraphics2D::GetRef().DrawString( m_sFont, m_sText.c_str(), Position(), Rotation(), Scale(), m_tColor, CenterPrivot() ),
                ("GUIText - Draw Text Failed! Font: "+ m_sFont +" Not Found.").c_str()
            );
            
        }
    }

}