#ifndef SpritesManager_h__
#define SpritesManager_h__

#include <SimpleMath.h>
#include <SpriteFont.h>
#include <wrl/client.h>
#include <string>
#include <unordered_map>

namespace G2D {

    using namespace Microsoft::WRL;

    struct Sprite2DInfo {
        std::string name;
        DirectX::SimpleMath::Vector2 size;
        DirectX::SimpleMath::Vector2 origin;
        ComPtr<ID3D11ShaderResourceView> texture;
        std::wstring path;
    };

    struct SpriteFontInfo {
        std::string name;
        std::shared_ptr<DirectX::SpriteFont> font;
        std::wstring path;
    };

    class CSpritesManager {

    public:
        typedef std::unordered_map<std::string, Sprite2DInfo> PairedSprite2D;
        typedef std::unordered_map<std::string, SpriteFontInfo> PairedSpriteFont;

        CSpritesManager();
        ~CSpritesManager();

        void LoadSprite2D( const std::wstring & path, ID3D11Device * device );
        void LoadSpriteFont( const std::wstring & path, ID3D11Device * device );

        Sprite2DInfo GetSprite( std::string name );
        SpriteFontInfo GetFont( std::string name );

        bool IsSpriteExist( std::string name );
        bool IsFontExist( std::string name );


        // Save to xml file for easy use -- default: off
        void SaveToXML( const std::wstring &path );

    private:

        PairedSprite2D m_storedSprites;
        PairedSpriteFont m_storedFonts;

        bool GetFileInDirectory( std::vector<std::wstring> &out, const std::wstring &path );
        bool GetFileInDirectory( std::vector<std::string> &out, const std::wstring &path );

    protected:
        CSpritesManager( CSpritesManager const& ) = delete;
        CSpritesManager( CSpritesManager&& ) = delete;
        CSpritesManager& operator=( CSpritesManager const& ) = delete;
        CSpritesManager& operator=( CSpritesManager && ) = delete;
    };
}
#endif // SpritesManager_h__
