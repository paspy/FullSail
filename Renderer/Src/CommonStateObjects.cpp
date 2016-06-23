/***********************************************
* Filename:
* Date:
* Mod. Date:
* Mod. Initials:
* Author:
* Purpose:
************************************************/
#include "stdafx.h"
#include "..\Inc\CommonStateObjects.h"
#include "..\Inc\RendererController.h"
namespace Renderer
{


	// Helper for creating blend state objects.
	void CreateBlendState(D3D11_BLEND srcBlend, D3D11_BLEND destBlend, ID3D11BlendState** pResult)
	{
		ID3D11Device* device = CRendererController::m_deviceResources->GetD3DDevice();
		D3D11_BLEND_DESC desc;
		ZeroMemory(&desc, sizeof(desc));

		desc.RenderTarget[0].BlendEnable = (srcBlend != D3D11_BLEND_ONE) ||
			(destBlend != D3D11_BLEND_ZERO);

		desc.RenderTarget[0].SrcBlend = desc.RenderTarget[0].SrcBlendAlpha = srcBlend;
		desc.RenderTarget[0].DestBlend = desc.RenderTarget[0].DestBlendAlpha = destBlend;
		desc.RenderTarget[0].BlendOp = desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;

		desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
		AssertIfFailed(device->CreateBlendState(&desc, pResult));

	}


	// Helper for creating depth stencil state objects.
	void CreateDepthStencilState(bool enable, bool writeEnable, ID3D11DepthStencilState** pResult)
	{
		ID3D11Device* device = CRendererController::m_deviceResources->GetD3DDevice();
		D3D11_DEPTH_STENCIL_DESC desc;
		ZeroMemory(&desc, sizeof(desc));

		desc.DepthEnable = enable;
		desc.DepthWriteMask = writeEnable ? D3D11_DEPTH_WRITE_MASK_ALL : D3D11_DEPTH_WRITE_MASK_ZERO;
		desc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;

		desc.StencilEnable = false;
		desc.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
		desc.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;

		desc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
		desc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		desc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		desc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;

		desc.BackFace = desc.FrontFace;

		AssertIfFailed(device->CreateDepthStencilState(&desc, pResult));
	}


	// Helper for creating rasterizer state objects.
	void CreateRasterizerState(D3D11_CULL_MODE cullMode, D3D11_FILL_MODE fillMode, ID3D11RasterizerState** pResult)
	{
		ID3D11Device* device = CRendererController::m_deviceResources->GetD3DDevice();
		D3D11_RASTERIZER_DESC desc;
		ZeroMemory(&desc, sizeof(desc));

		desc.CullMode = cullMode;
		desc.FillMode = fillMode;
		desc.DepthClipEnable = true;
		desc.MultisampleEnable = true;

		AssertIfFailed(device->CreateRasterizerState(&desc, pResult));
	}


	// Helper for creating sampler state objects.
	void CreateSamplerState(D3D11_FILTER filter, D3D11_TEXTURE_ADDRESS_MODE addressMode, ID3D11SamplerState** pResult)
	{
		ID3D11Device* device = CRendererController::m_deviceResources->GetD3DDevice();
		D3D11_SAMPLER_DESC desc;
		ZeroMemory(&desc, sizeof(desc));

		desc.Filter = filter;

		desc.AddressU = addressMode;
		desc.AddressV = addressMode;
		desc.AddressW = addressMode;

		desc.MaxAnisotropy = (device->GetFeatureLevel() > D3D_FEATURE_LEVEL_9_1) ? 16 : 2;

		desc.MaxLOD = FLT_MAX;
		desc.ComparisonFunc = D3D11_COMPARISON_NEVER;

		AssertIfFailed(device->CreateSamplerState(&desc, pResult));
	}



	// Blend states.
	ID3D11BlendState* CCommonStateObjects::m_blendStates[COUNT_BS] = {nullptr};
	// Depth stencil states.
	ID3D11DepthStencilState* CCommonStateObjects::m_depthStencilStates[COUNT_DSS] = { nullptr };
	// Rasterizer states.
	ID3D11RasterizerState* CCommonStateObjects::m_rasterizerStates[COUNT_RS] = { nullptr };
	// Sampler states.
	ID3D11SamplerState*  CCommonStateObjects::m_samplerStates[COUNT_SS] = { nullptr };


	bool CCommonStateObjects::m_bInstantiated = false;
	CCommonStateObjects::CCommonStateObjects()
	{
		assert(!m_bInstantiated && "Only one CCommonStateObjects instance is allowed.");
		CreateBlendState(D3D11_BLEND_ONE, D3D11_BLEND_ZERO, &m_blendStates[Opaque_BS]);
		CreateBlendState(D3D11_BLEND_ONE, D3D11_BLEND_INV_SRC_ALPHA, &m_blendStates[AlphaBlend_BS]);
		CreateBlendState(D3D11_BLEND_SRC_ALPHA, D3D11_BLEND_ONE, &m_blendStates[Additive_BS]);
		CreateBlendState(D3D11_BLEND_SRC_ALPHA, D3D11_BLEND_INV_SRC_ALPHA, &m_blendStates[NonPremultiplied_BS]);

		CreateDepthStencilState(false, false, &m_depthStencilStates[DepthNone_DSS]);
		CreateDepthStencilState(true, true, &m_depthStencilStates[DepthDefault_DSS]);
		CreateDepthStencilState(true, false, &m_depthStencilStates[DepthRead_DSS]);

		CreateRasterizerState(D3D11_CULL_NONE, D3D11_FILL_SOLID, &m_rasterizerStates[CullNone_RS]);
		CreateRasterizerState(D3D11_CULL_FRONT, D3D11_FILL_SOLID, &m_rasterizerStates[CullClockwise_RS]);
		CreateRasterizerState(D3D11_CULL_BACK, D3D11_FILL_SOLID, &m_rasterizerStates[CullCounterClockwise_RS]);
		CreateRasterizerState(D3D11_CULL_NONE, D3D11_FILL_WIREFRAME, &m_rasterizerStates[Wireframe_RS]);

		CreateSamplerState(D3D11_FILTER_MIN_MAG_MIP_POINT, D3D11_TEXTURE_ADDRESS_WRAP, &m_samplerStates[PointWrap]);
		CreateSamplerState(D3D11_FILTER_MIN_MAG_MIP_POINT, D3D11_TEXTURE_ADDRESS_CLAMP, &m_samplerStates[PointClamp]);
		CreateSamplerState(D3D11_FILTER_MIN_MAG_LINEAR_MIP_POINT, D3D11_TEXTURE_ADDRESS_WRAP, &m_samplerStates[LinearWrap]);
		CreateSamplerState(D3D11_FILTER_MIN_MAG_LINEAR_MIP_POINT, D3D11_TEXTURE_ADDRESS_CLAMP, &m_samplerStates[LinearClamp]);
        CreateSamplerState( D3D11_FILTER_MIN_MAG_MIP_LINEAR, D3D11_TEXTURE_ADDRESS_WRAP, &m_samplerStates[TrilinearWrap] );
        CreateSamplerState( D3D11_FILTER_MIN_MAG_MIP_LINEAR, D3D11_TEXTURE_ADDRESS_CLAMP, &m_samplerStates[TrilinearClamp] );
		CreateSamplerState(D3D11_FILTER_ANISOTROPIC, D3D11_TEXTURE_ADDRESS_WRAP, &m_samplerStates[AnisotropicWrap]);
        CreateSamplerState( D3D11_FILTER_ANISOTROPIC, D3D11_TEXTURE_ADDRESS_CLAMP, &m_samplerStates[AnisotropicClamp] );


        D3D11_BLEND_DESC blendDesc;
        SecureZeroMemory( &blendDesc, sizeof( blendDesc ) );
        blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

        blendDesc.RenderTarget[0].BlendEnable = true;
        blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
        blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
        blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
        blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
        blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
        blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
        blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
        AssertIfFailed( CRendererController::m_deviceResources->GetD3DDevice()->CreateBlendState( &blendDesc, &m_blendStates[PaticleAlphaBlend_BS] ) );

		m_bInstantiated = true;
	}


	CCommonStateObjects::~CCommonStateObjects()
	{
		for (auto& bs : m_blendStates)
		{
			SAFE_RELEASE(bs);
		}
		for (auto& dss : m_depthStencilStates)
		{
			SAFE_RELEASE(dss);
		}
		for (auto& rs : m_rasterizerStates)
		{

			SAFE_RELEASE(rs);
		}
		for (auto& ss : m_samplerStates)
		{
			SAFE_RELEASE(ss);
		}
	}
}