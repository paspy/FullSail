#include "stdafx.h"
#include "..\Inc\C3DHUD.h"
#include "..\Inc\CommonStateObjects.h"
#include "..\Inc\ShaderPass.h"
#include "..\Inc\InputLayoutManager.h"
#include "..\Inc\RendererController.h"
#include "..\Inc\View.h"
#include "..\Inc\Graphics2D.h"
#include <AssetManager.h>

namespace Renderer {

    std::unique_ptr<CShaderPass>   C3DHUD::m_ShaderPass = nullptr;
    std::unique_ptr<DirectX::SpriteBatch>   C3DHUD::m_pSpriteBatch = nullptr;

    void 	C3DHUD::InitShaderPass() {
        auto device = CRendererController::m_deviceResources->GetD3DDevice();
        auto deviceContext = CRendererController::m_deviceResources->GetD3DDeviceContext();
        m_ShaderPass = std::make_unique<CShaderPass>( device,
            "CSO\\VS_InGame_HUD.cso",
            "CSO\\PS_InGame_HUD.cso",
            nullptr,
            nullptr,
            nullptr,
            0,
            0,
            0 );
        m_pSpriteBatch = std::make_unique<DirectX::SpriteBatch>( deviceContext );
    }

    void 	C3DHUD::InitShaderPass( const char* vs_file, const char* ps_file, const char* hs_file, const char* ds_file, const char* gs_file ) {
        auto device = CRendererController::m_deviceResources->GetD3DDevice();
        m_ShaderPass = std::make_unique<CShaderPass>( device,
            vs_file,
            ps_file,
            hs_file,
            ds_file,
            gs_file,
            0,
            0,
            0 );
    }

    void C3DHUD::ReleaseShaderPass() {
        m_ShaderPass.reset();
        m_pSpriteBatch.reset();
    }


    C3DHUD::C3DHUD(
        DirectX::SimpleMath::Vector4* pos,
        float width, float height,
        ID3D11ShaderResourceView* texture ) :
        m_HUDpos( pos ),
        m_HUDTexture( texture ),
        m_HUDCb( nullptr ),
        m_HUDtype( eTYPE_TEXTURE ) {
        auto device = CRendererController::m_deviceResources->GetD3DDevice();
        SecureZeroMemory( &m_HUDCbData, sizeof m_HUDCbData );
        D3D11_BUFFER_DESC bd = { 0 };
        bd.Usage = D3D11_USAGE_DYNAMIC;
        bd.ByteWidth = sizeof( m_HUDCbData );
        bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
        device->CreateBuffer( &bd, nullptr, &m_HUDCb );

        if ( texture ) {
            m_HUDCbData.gHasTexture = true;
        }
        m_HUDCbData.gWH.x = width;
        m_HUDCbData.gWH.y = height;
        m_HUDCbData.gFillColor = { 1.0f,1.0f,1.0f,1.0f };

    }

    C3DHUD::C3DHUD( std::wstring *text, std::string fontName ) :
        m_fontName( fontName ), m_text( text ), m_TextPos( DirectX::SimpleMath::Vector3::Zero ), m_HUDtype( eTYPE_TEXT ), m_scale(0.75f)
	{
        m_HUDCbData.gWH.z = 1.0f;
        m_HUDCbData.gFillColor = { 1.0f,1.0f,1.0f,0.0f };
        m_pSpriteBatch->SetRotation( DXGI_MODE_ROTATION_UNSPECIFIED );
        m_HUDCb = nullptr;
    }

    C3DHUD::~C3DHUD() {
        SAFE_RELEASE( m_HUDCb );
    }

    void C3DHUD::UpdateHUDConstantBuffer() {
        static auto deviceContext = CRendererController::m_deviceResources->GetD3DDeviceContext();

        m_HUDCbData.gPosW = *m_HUDpos;

        D3D11_MAPPED_SUBRESOURCE edit;
        SecureZeroMemory( &edit, sizeof edit );
        deviceContext->Map( m_HUDCb, 0, D3D11_MAP_WRITE_DISCARD, 0, &edit );
        memcpy_s( edit.pData, sizeof( m_HUDCbData ), &m_HUDCbData, sizeof( m_HUDCbData ) );
        deviceContext->Unmap( m_HUDCb, 0 );

    }

    void C3DHUD::Begin( IRenderNode* pCurrentView ) {
        static auto deviceContext = CRendererController::m_deviceResources->GetD3DDeviceContext();
        static auto& assetManger = CAssetManager::GetRef();
        CView& view = (CView&)( *pCurrentView );

        if ( m_HUDtype == eTYPE_TEXT ) {
            using namespace DirectX;
            //SimpleMath::Matrix matrix = SimpleMath::Matrix::CreateTranslation( *m_TextPos );

            DirectX::XMVECTOR textPos = XMVectorSet( m_TextPos.x, m_TextPos.y, m_TextPos.z, 0.0f );
            SimpleMath::Matrix viewMat = view.ViewMatrix();
            SimpleMath::Matrix projMat = view.ProjectionMatrix();

            textPos = DirectX::XMVector3Transform( textPos, viewMat );
            textPos = DirectX::XMVector3Transform( textPos, projMat );
            SimpleMath::Vector3 finalPos = textPos;
            finalPos.x /= finalPos.z;
            finalPos.y /= finalPos.z;

            finalPos.x = ( finalPos.x + 1.0f ) *  CRendererController::m_deviceResources->GetScreenViewport().Width / 2.0f;
            finalPos.y = ( -finalPos.y + 1.0f ) * CRendererController::m_deviceResources->GetScreenViewport().Height / 2.0f;

            //m_pSpriteBatch->Begin( /*DirectX::SpriteSortMode_Deferred, nullptr, nullptr, nullptr, nullptr, nullptr, matrix*/ );
            //if ( !assetManger.IsFontExist( m_fontName ) ) {
            //    __debugbreak();
            //}
            //auto spriteFont = assetManger.GetFont( m_fontName ).get();
            //SimpleMath::Vector2 origin = spriteFont->MeasureString( m_text->c_str() ) / 2.0f;
            SimpleMath::Color color = m_HUDCbData.gFillColor;
            //spriteFont->DrawString( m_pSpriteBatch.get(), m_text->c_str(), { m_TextPos.x, m_TextPos.y}, color * color.w, 0.0f, origin, 1.0f);
            G2D::CGraphics2D::GetRef().BeginDraw();

			if (m_scale >= 0.75f)
			{
				m_scale = 0.75f;
			}
            G2D::CGraphics2D::GetRef().DrawString( m_fontName, m_text->c_str(), { finalPos.x, finalPos.y }, 0.0f, m_scale, color, true );

        } else {
            UpdateHUDConstantBuffer();
            deviceContext->VSSetConstantBuffers( 0, 1, &m_HUDCb );
            deviceContext->PSSetConstantBuffers( 0, 1, &m_HUDCb );
            m_ShaderPass->Begin();
            deviceContext->PSSetShaderResources( 0, 1, &m_HUDTexture );
            deviceContext->IASetPrimitiveTopology( D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP );
            deviceContext->Draw( 4, 0 );
        }


    }

    void C3DHUD::End( IRenderNode* pCurrentView ) {
        static auto deviceContext = CRendererController::m_deviceResources->GetD3DDeviceContext();

        if ( m_HUDtype == eTYPE_TEXT ) {
            //m_pSpriteBatch->End();
            G2D::CGraphics2D::GetRef().EndDraw();

        } else {
            ID3D11Buffer* nullBuffer = nullptr;
            deviceContext->VSSetConstantBuffers( 0, 1, &nullBuffer );
            deviceContext->PSSetConstantBuffers( 0, 1, &nullBuffer );
            ID3D11ShaderResourceView* nullView = nullptr;
            deviceContext->PSSetShaderResources( 0, 1, &nullView );
            m_ShaderPass->End();
        }


    }
}