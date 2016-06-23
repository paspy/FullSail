#ifndef GUITexture_h__
#define GUITexture_h__
#include "Entity2D.h"

namespace G2D {
    class GUITexture : public Entity2D {
    public:
        GUITexture( std::string textureName, std::string textureFile, SimpleMath::Color color = { 1.f, 1.f, 1.f, 1.f } );
        ~GUITexture() = default;
		GUITexture(GUITexture const& copy);

        // interfaces
        void Render()           final override;
        void Update( float dt ) final override;

        // Mutators
        std::string TextureName() const { return m_sTextureName; }
        SimpleMath::Vector2 Size() const { return CAssetManager::GetRef().GetSpriteSize( m_sTextureFile ); }
        SimpleMath::Color Color() const { return m_tColor; }
        std::unordered_map<std::string, Vector2> Offsets() const { return m_mOffsets; }
        std::unordered_map<std::string, DirectX::SimpleMath::Vector2> StaticPostions() const { return m_mStaticPostions; }

        // Accessors
        void TextureName( std::string val ) { m_sTextureName = val; }
        void Color( SimpleMath::Color val ) { m_tColor = val; }
        void Offsets( std::unordered_map<std::string, Vector2> val ) { m_mOffsets = val; }
        void StaticPostions( std::unordered_map<std::string, DirectX::SimpleMath::Vector2> val ) { m_mStaticPostions = val; }

    private:
        std::string                                 m_sTextureName;
		std::string                                 m_sTextureFile;
        SimpleMath::Color                           m_tColor;
        std::unordered_map<std::string, Vector2>    m_mStaticPostions;
        std::unordered_map<std::string, Vector2>    m_mOffsets;

    protected:
        
        GUITexture( GUITexture&& ) = delete;
        GUITexture& operator=( GUITexture const& ) = delete;
        GUITexture& operator=( GUITexture && ) = delete;

    };
}

#endif // GUITexture_h__
