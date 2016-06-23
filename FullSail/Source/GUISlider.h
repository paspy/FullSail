#ifndef GUISlider_h__
#define GUISlider_h__
#include "Entity2D.h"

namespace G2D {
    class GUISlider : public Entity2D {
    public:
        GUISlider( std::string frontTexture, std::string frontTextureFile, std::string backTexture, std::string backTextureFile );
        ~GUISlider() = default;

        // interfaces
        void Update( float dt )                     final override;
        void Render()                               final override;

        // Mutators
        std::string FrontTextureName() const        { return m_sFrontTextureName; }
        std::string BackTextureName() const         { return m_sBackTextureName; }
        SimpleMath::Color FrontColor() const        { return m_tFrontColor; }
        SimpleMath::Color BackColor() const         { return m_tBackColor; }
        bool Animated() const                       { return m_bAnimated; }
        bool Vertical() const                       { return m_bVertical; }
        float Value() const                         { return m_fValue; }

        // Accessors
        void FrontTextureName( std::string val )    { m_sFrontTextureName = val; }
        void BackTextureName( std::string val )     { m_sBackTextureName = val; }
        void FrontColor( SimpleMath::Color val )    { m_tFrontColor = val; }
        void BackColor( SimpleMath::Color val )     { m_tBackColor = val; }
        void Animated( bool val )                   { m_bAnimated = val; }
        void Vertical( bool val )                   { m_bVertical = val; }
        void Value( float val )                     { m_fValue = val; }

    private:
        std::string                                 m_sFrontTextureName;
        std::string                                 m_sFrontTextureFile;
		std::string                                 m_sBackTextureName;
		std::string                                 m_sBackTextureFile;
        SimpleMath::Color                           m_tFrontColor;
        SimpleMath::Color                           m_tBackColor;
        Rectangle                                   m_tRectangle;
        bool                                        m_bAnimated;
        bool                                        m_bVertical;
        float                                       m_fValue;

    protected:
        GUISlider( GUISlider const& ) = delete;
        GUISlider( GUISlider&& ) = delete;
        GUISlider& operator=( GUISlider const& ) = delete;
        GUISlider& operator=( GUISlider && ) = delete;
    };
}

#endif // GUISlider_h__
