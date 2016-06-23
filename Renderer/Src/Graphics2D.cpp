#include "stdafx.h"
#include "../Inc/Graphics2D.h"
#include "../Inc/RendererController.h"
#include <AssetManager.h>
#include <Utilities.h>
#include <stdexcept>

const std::string g_customerPixelShaderPath = "CSO/PS_SpriteBatch.cso";

namespace G2D {
    CAssetManager& CGraphics2D::m_AssetManager = CAssetManager::GetRef();

    CGraphics2D::CGraphics2D() {
        Initialize();
    }

    CGraphics2D::~CGraphics2D() {
        //Destory();
    }

    void CGraphics2D::Initialize() {
        try {
            if ( isInit ) throw std::runtime_error( "CGraphics2D is already initialized." );

            auto d3dContext = Renderer::CRendererController::m_deviceResources->GetD3DDeviceContext();
            auto d3dDevice = Renderer::CRendererController::m_deviceResources->GetD3DDevice();
            //
            // initialize sprite batch
            //
            m_pSpriteBatch = std::make_unique<SpriteBatch>( d3dContext );

            //
            // initialize a solid color texture
            //
            DirectX::XMFLOAT4 white[1];
            white[0] = { 1.0f, 1.0f, 1.0f, 1.0f };

            D3D11_SUBRESOURCE_DATA initData;
            initData.pSysMem = white;
            initData.SysMemPitch = 1 * sizeof( DirectX::XMFLOAT4 );
            initData.SysMemSlicePitch = 0;

            D3D11_TEXTURE1D_DESC texDesc;
            texDesc.Width = 1;
            texDesc.MipLevels = 1;
            texDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
            texDesc.Usage = D3D11_USAGE_IMMUTABLE;
            texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
            texDesc.CPUAccessFlags = 0;
            texDesc.MiscFlags = 0;
            texDesc.ArraySize = 1;

            ComPtr<ID3D11Texture1D> colorTex;
            d3dDevice->CreateTexture1D( &texDesc, &initData, &colorTex );

            D3D11_SHADER_RESOURCE_VIEW_DESC viewDesc;
            viewDesc.Format = texDesc.Format;
            viewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE1D;
            viewDesc.Texture1D.MipLevels = texDesc.MipLevels;
            viewDesc.Texture1D.MostDetailedMip = 0;

            ID3D11ShaderResourceView* randomTexSRV = 0;
            d3dDevice->CreateShaderResourceView( colorTex.Get(), &viewDesc, m_pColorTexSRV.GetAddressOf() );

            // build custom pixel shader
            std::ifstream fin;
            fin.open( g_customerPixelShaderPath, std::ios_base::binary );
            if ( fin.is_open() ) {
                fin.seekg( 0, std::ios_base::end );
                UINT byteCodeSize = (UINT)fin.tellg();
                char* byteCode = new char[byteCodeSize];
                fin.seekg( 0, std::ios_base::beg );
                fin.read( byteCode, byteCodeSize );
                d3dDevice->CreatePixelShader( byteCode, byteCodeSize, nullptr, m_pixelShader.GetAddressOf() );
                fin.close();
                delete[] byteCode;
            }

            // build constant buffer
            D3D11_BUFFER_DESC cbbd;
            SecureZeroMemory( &cbbd, sizeof( D3D11_BUFFER_DESC ) );
            cbbd.Usage = D3D11_USAGE_DYNAMIC;
            cbbd.ByteWidth = sizeof( CusSpriteCB );
            cbbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
            cbbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
            d3dDevice->CreateBuffer( &cbbd, NULL, &m_cusCB );

            isInit = true;
        } catch ( std::exception &e ) {
            OutputDebugStringA( ( std::string( e.what() ) + "\n" ).c_str() );
            // Trigger a breakpoint
            __debugbreak();				// USE THE CALLSTACK TO DEBUG!
        }

    }

    void CGraphics2D::Destory() {
        try {
            if ( !isInit ) throw std::runtime_error( "CGraphics2D did not initialize." );
            m_pSpriteBatch.reset();
            m_pColorTexSRV.Reset();
            m_pixelShader.Reset();
            m_cusCB.Reset();
            isInit = false;
        } catch ( const std::exception& e ) {
            OutputDebugStringA( ( std::string( e.what() ) + "\n" ).c_str() );
            // Trigger a breakpoint
            __debugbreak();				// USE THE CALLSTACK TO DEBUG!
        }

    }

    void CGraphics2D::BeginDraw() {
        try {
            if ( !isInit ) throw std::runtime_error( "CGraphics2D did not initialize." );

            auto d3dContext = Renderer::CRendererController::m_deviceResources->GetD3DDeviceContext();
            auto rtv = Renderer::CRendererController::m_deviceResources->GetBackBufferRenderTargetView();
            auto dsv = Renderer::CRendererController::m_deviceResources->GetDepthStencilView();
            d3dContext->OMSetRenderTargets( 1, &rtv, dsv );

            //m_pSpriteBatch->Begin();
            m_pSpriteBatch->Begin( SpriteSortMode_Immediate,
                nullptr, nullptr, nullptr, nullptr, [=] {
                d3dContext->PSSetShader( m_pixelShader.Get(), nullptr, 0 );
                d3dContext->PSSetConstantBuffers( 1, 1, m_cusCB.GetAddressOf() );
            } );

        } catch ( const std::exception& e ) {
            OutputDebugStringA( ( std::string( e.what() ) + "\n" ).c_str() );
            // Trigger a breakpoint
            __debugbreak();				// USE THE CALLSTACK TO DEBUG!
        }
    }

    void CGraphics2D::EndDraw() {
        try {
            if ( !isInit ) throw std::runtime_error( "CGraphics2D did not initialize." );
            auto d3dContext = Renderer::CRendererController::m_deviceResources->GetD3DDeviceContext();
            m_pSpriteBatch->End();
            //d3dContext->OMSetRenderTargets( 0, nullptr, nullptr );

        } catch ( const std::exception& e ) {
            OutputDebugStringA( ( std::string( e.what() ) + "\n" ).c_str() );
            // Trigger a breakpoint
            __debugbreak();				// USE THE CALLSTACK TO DEBUG!
        }
    }

    bool CGraphics2D::DrawLine( DirectX::XMFLOAT2 start, DirectX::XMFLOAT2 end, unsigned int lineWidth, Color color /*= { 1.0f, 1.0f, 1.0f, 1.0f}*/ ) {
        using namespace DirectX;

        try {
            if ( !isInit ) throw std::runtime_error( "CGraphics2D did not initialize." );
            // Calculate the difference in positions
            float dX = end.x - start.x;
            float dY = end.y - start.y;

            if ( dX == 0 && dY == 0 || lineWidth <= 0 ) {
                return false;
            }

            XMFLOAT2 edge = end - start;
            float edgeLength = std::sqrtf( ( end.x*end.x ) + ( end.y*end.y ) );
            float angle = std::atan2( edge.y, edge.x );
            RECT lineRect = { (LONG)start.x, (LONG)start.y, (LONG)edgeLength, (LONG)lineWidth };
            m_pSpriteBatch->Draw( m_pColorTexSRV.Get(), lineRect, NULL, color, angle );

        } catch ( const std::exception& e ) {
            OutputDebugStringA( ( std::string( e.what() ) + "\n" ).c_str() );
            // Trigger a breakpoint
            __debugbreak();				// USE THE CALLSTACK TO DEBUG!
        }
        return true;
    }

    //bool CGraphics2D::DrawRectangle() {
    //    try {
    //        if ( !isInit ) throw std::runtime_error( "CGraphics2D did not initialize." );
    //        // to-do

    //    } catch ( const std::exception& e ) {
    //        OutputDebugStringA( ( std::string( e.what() ) + "\n" ).c_str() );
    //        // Trigger a breakpoint
    //        __debugbreak();				// USE THE CALLSTACK TO DEBUG!
    //    }
    //    return true;

    //}

    bool CGraphics2D::DrawString( std::string fontName, const wchar_t* text, Vector2 pos, float rotation, float scale, Color color, bool center ) {
        try {
            if ( !isInit ) throw std::runtime_error( "CGraphics2D did not initialize." );
            if ( !text || !m_AssetManager.IsFontExist( fontName ) )
                return false;
            auto spriteFont = m_AssetManager.GetFont( fontName ).get();
            Vector2 origin = spriteFont->MeasureString( text ) / 2.0f;

            if ( center ) {
                spriteFont->DrawString( m_pSpriteBatch.get(), text, pos, color * color.w, rotation, origin, scale );
            } else {
                spriteFont->DrawString( m_pSpriteBatch.get(), text, pos, color * color.w, rotation, Vector2( .0f, .0f ), scale );
            }

        } catch ( const std::exception& e ) {
            OutputDebugStringA( ( std::string( e.what() ) + "\n" ).c_str() );
            // Trigger a breakpoint
            __debugbreak();				// USE THE CALLSTACK TO DEBUG!
        }
        return true;
    }

    bool CGraphics2D::DrawTexture( std::string spriteName, Vector2 pos, float rotation, float scale, Color color, bool center, CusSpriteCB* constanBuffer ) {
        try {
            if ( !isInit ) throw std::runtime_error( "CGraphics2D did not initialize." );
            if ( !m_AssetManager.IsSpriteExist( spriteName ) )
                return false;

            Vector2 spriteOrigin;
            auto sprite = m_AssetManager.GetSprite( spriteName, spriteOrigin );
            auto d3dContext = Renderer::CRendererController::m_deviceResources->GetD3DDeviceContext();

            if ( constanBuffer ) {
                D3D11_MAPPED_SUBRESOURCE mappedRes;
                d3dContext->Map( m_cusCB.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedRes );
                memcpy_s( mappedRes.pData, sizeof( CusSpriteCB ), constanBuffer, sizeof( CusSpriteCB ) );
                d3dContext->Unmap( m_cusCB.Get(), 0 );
            }

            if ( center ) {
                m_pSpriteBatch->Draw( sprite, pos, NULL, color, rotation, spriteOrigin, scale );
            } else {
                m_pSpriteBatch->Draw( sprite, pos, NULL, color, rotation, Vector2( .0f, .0f ), scale );
            }

        } catch ( const std::exception& e ) {
            OutputDebugStringA( ( std::string( e.what() ) + "\n" ).c_str() );
            // Trigger a breakpoint
            __debugbreak();				// USE THE CALLSTACK TO DEBUG!
        }
        return true;

    }

    bool CGraphics2D::DrawTexture( std::string spriteName, Rectangle rect, Vector2 pos, float rotation, float scale, Color color, bool center, CusSpriteCB* constanBuffer ) {
        try {
            if ( !isInit ) throw std::runtime_error( "CGraphics2D did not initialize." );
            if ( !m_AssetManager.IsSpriteExist( spriteName ) /*|| rect.IsEmpty()*/ )
                return false;

            Vector2 spriteOrigin;
            RECT stdRect = { (LONG)rect.left, (LONG)rect.top, (LONG)rect.right, (LONG)rect.bottom };
            auto sprite = m_AssetManager.GetSprite( spriteName, spriteOrigin );
            auto d3dContext = Renderer::CRendererController::m_deviceResources->GetD3DDeviceContext();

            if ( constanBuffer ) {
                D3D11_MAPPED_SUBRESOURCE mappedRes;
                d3dContext->Map( m_cusCB.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedRes );
                memcpy_s( mappedRes.pData, sizeof( CusSpriteCB ), constanBuffer, sizeof( CusSpriteCB ) );
                d3dContext->Unmap( m_cusCB.Get(), 0 );
            }

            if ( center ) {
                m_pSpriteBatch->Draw( sprite, pos, &stdRect, color * color.w, rotation, spriteOrigin, scale );
            } else {
                m_pSpriteBatch->Draw( sprite, pos, &stdRect, color * color.w, rotation, Vector2( .0f, .0f ), scale );
            }

        } catch ( const std::exception& e ) {
            OutputDebugStringA( ( std::string( e.what() ) + "\n" ).c_str() );
            // Trigger a breakpoint
            __debugbreak();				// USE THE CALLSTACK TO DEBUG!
        }

        return true;
    }

}
