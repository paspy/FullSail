#ifndef GUIText_h__
#define GUIText_h__
#include "Entity2D.h"

namespace G2D {
    //////////////////////////////////////////////////////////////////////////
    //  The default size of a font is set to 28
    //////////////////////////////////////////////////////////////////////////
    class GUIText : public Entity2D {
    public:
        GUIText( std::wstring text, SimpleMath::Color color = { 0.f, 0.f, 0.f, 1.f }, std::string font = "Forte" );
        ~GUIText() = default;

        // interfaces
        void Update( float _dt ) final override;
        void Render() final override;

        // Mutators
        std::wstring Text() const { return m_sText; }
        std::string Font() const { return m_sFont; }
        SimpleMath::Color Color() const { return m_tColor; }

        // Accessors
        void Text( std::wstring val ) { m_sText = val; }
        void Font( std::string val ) { m_sFont = val; }
        void Color( SimpleMath::Color val ) { m_tColor = val; }

    private:
        std::wstring m_sText;
        std::string m_sFont;
        SimpleMath::Color   m_tColor;


    protected:
        GUIText( GUIText const& ) = delete;
        GUIText( GUIText&& ) = delete;
        GUIText& operator=( GUIText const& ) = delete;
        GUIText& operator=( GUIText && ) = delete;
    };

}

#endif // GUIText_h__
