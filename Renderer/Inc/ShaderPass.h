/***********************************************
* Filename:
* Date:
* Mod. Date:
* Mod. Initials:
* Author:
* Purpose:
************************************************/
#pragma once


#include "RenderNode.h"

namespace Renderer
{
	template<typename Shadertype> class CShaderHandle;
	class CShaderPass : public IRenderNode
	{
		friend class CView;

	private:
		unsigned int m_blendState;
		unsigned int m_depthStencilState;
		unsigned int m_rasterizerStates;

		float*       m_blendFactor = nullptr;

		//Shaders
		CShaderHandle<ID3D11VertexShader>*		m_pVertexShader = nullptr;
		CShaderHandle<ID3D11PixelShader>*		m_pPixelShader = nullptr;
		CShaderHandle<ID3D11HullShader>*		m_pHullShader = nullptr;
		CShaderHandle<ID3D11DomainShader>*		m_pDomainShader = nullptr;
		CShaderHandle<ID3D11GeometryShader>*	m_pGeometryShader = nullptr;


	public:

		//Opaque_BS = 0, AlphaBlend_BS, Additive_BS, NonPremultiplied_BS
		//DepthNone_DSS = 0, DepthDefault_DSS, DepthRead_DSS
		//CullNone_RS = 0, CullClockwise_RS, CullCounterClockwise_RS, Wireframe_RS
		CShaderPass(ID3D11Device* d3dDevice, unsigned int blendState = 0, unsigned int depthStencilState = 1, unsigned int rasterizerStates = 0, float *blendFactor = nullptr);

		//Opaque_BS = 0, AlphaBlend_BS, Additive_BS, NonPremultiplied_BS
		//DepthNone_DSS = 0, DepthDefault_DSS, DepthRead_DSS
		//CullNone_RS = 0, CullClockwise_RS, CullCounterClockwise_RS, Wireframe_RS
		CShaderPass(ID3D11Device* d3dDevice,
			const char* vs_file, const char* ps_file, const char* hs_file, const char* ds_file, const char* gs_file,
			unsigned int blendState = 0, unsigned int depthStencilState = 1, unsigned int rasterizerStates = 0, float *blendFactor = nullptr);

		~CShaderPass();

		virtual void Begin(IRenderNode* pCurrentView) final;
		virtual void End(IRenderNode* pCurrentView) final;

		virtual void Begin();
		virtual void End();
	};
}
