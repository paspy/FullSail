
#ifndef GraphicsManager_h__
#define GraphicsManager_h__
#ifdef RENDERER_EXPORTS
#define RENDERER_API __declspec(dllexport)
#else
#define RENDERER_API __declspec(dllimport)
#endif
#include "stdafx.h"

#include "Geometry2D.h"
#include <SpriteBatch.h>
#include <SimpleMath.h>
#include <wrl/client.h>
#include <memory>

class CAssetManager;

namespace G2D {
    using namespace DirectX::SimpleMath;
    using namespace DirectX;
    using namespace Microsoft::WRL;

    class CGraphics2D {

    public:

        struct CusSpriteCB {
            CusSpriteCB() { SecureZeroMemory( this, sizeof( *this ) ); }
            float customFloat0;
            float customFloat1;
            float customFloat2;
            float customFloat3;
        };

        // C++11 singleton
        RENDERER_API    static  CGraphics2D& GetRef() { static CGraphics2D instance; return instance; }

        RENDERER_API    void    Initialize();
        RENDERER_API    void    Destory();
        RENDERER_API    void    BeginDraw();
        RENDERER_API    void    EndDraw();

        RENDERER_API    bool    DrawLine( DirectX::XMFLOAT2 pos1, DirectX::XMFLOAT2 pos2, unsigned int lineWidth = 1, Color color = { 1.0f, 1.0f, 1.0f, 1.0f } );
        //RENDERER_API    bool    DrawRectangle();
        RENDERER_API    bool    DrawString( std::string fontName, const wchar_t*  text, Vector2 pos, float rotation = 0.0f, float scale = 1.0f, Color color = { 1.0f, 1.0f, 1.0f, 1.0f }, bool center = false );
        RENDERER_API    bool    DrawTexture( std::string spriteName, Vector2 pos, float rotation = 0.0f, float scale = 1.0f, Color color = { 1.0f, 1.0f, 1.0f, 1.0f }, bool center = false, CusSpriteCB* constanBuffer = nullptr );
        RENDERER_API    bool    DrawTexture( std::string spriteName, Rectangle rect, Vector2 pos, float rotation = 0.0f, float scale = 1.0f, Color color = { 1.0f, 1.0f, 1.0f, 1.0f }, bool center = false, CusSpriteCB* constanBuffer = nullptr );


        // Delete stuff for singleton
        CGraphics2D( const CGraphics2D & ) = delete;
        CGraphics2D( CGraphics2D&& ) = delete;
        CGraphics2D& operator=( CGraphics2D const& ) = delete;
        CGraphics2D& operator=( CGraphics2D && ) = delete;

    private:
        bool isInit = false;
        static CAssetManager& m_AssetManager;
        ComPtr<ID3D11ShaderResourceView>        m_pColorTexSRV;
        std::unique_ptr<DirectX::SpriteBatch>   m_pSpriteBatch;

        // customer shaders
        ComPtr<ID3D11PixelShader>   m_pixelShader;
        ComPtr<ID3D11Buffer>        m_cusCB;
    protected:
        CGraphics2D();
        ~CGraphics2D();

    };

}

#endif // GraphicsManager_h__
