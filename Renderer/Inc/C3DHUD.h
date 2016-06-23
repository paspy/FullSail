#pragma once
#include <vector>
#include <SpriteBatch.h>
#include "ConstantBuffer.h"

#ifdef RENDERER_EXPORTS
#define RENDERER_API __declspec(dllexport)
#else
#define RENDERER_API __declspec(dllimport)
#endif

namespace Renderer
{
	class CShaderPass;
    class IRenderNode;

	class C3DHUD
	{
    public:
        enum HUD_TYPE {
            eTYPE_TEXTURE = 0, eTYPE_SLIDER, eTYPE_TEXT, eTYPE_MAX
        };

    private:
        HUD_TYPE                        m_HUDtype;
        DirectX::SimpleMath::Vector4*   m_HUDpos;
        ID3D11ShaderResourceView*       m_HUDTexture;
        ID3D11Buffer*			        m_HUDCb;
        cbPerHUD				        m_HUDCbData;

        // 3D text
        std::string                     m_fontName;
        std::wstring*                   m_text;
        DirectX::SimpleMath::Vector3    m_TextPos;
		float							m_scale;

		static std::unique_ptr<CShaderPass>    m_ShaderPass;

        static std::unique_ptr<DirectX::SpriteBatch>   m_pSpriteBatch;

	public:
        RENDERER_API C3DHUD( 
            DirectX::SimpleMath::Vector4* pos, 
            float width, float height, 
            ID3D11ShaderResourceView* texture = nullptr);

        RENDERER_API C3DHUD( std::wstring *text, std::string fontName = "Forte" );

		static void InitShaderPass();
		static void InitShaderPass(const char* vs_file, const char* ps_file, const char* hs_file, const char* ds_file, const char* gs_file);
		static void ReleaseShaderPass();
		virtual ~C3DHUD();
		void UpdateHUDConstantBuffer();
		void Begin( IRenderNode* pCurrentView );
		void End( IRenderNode* pCurrentView );

		RENDERER_API float GetScale() { return m_scale; }
		RENDERER_API void SetScale(float s) { m_scale = s; }
		RENDERER_API void SetProgression(float p) { m_HUDCbData.gWH.z = p; }
        RENDERER_API void SetFillColor( float4 color ) { m_HUDCbData.gFillColor = color; }
        RENDERER_API void SetTextPosition( DirectX::SimpleMath::Vector3 position ) { m_TextPos = position; }

	};

}
