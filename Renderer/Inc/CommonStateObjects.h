/***********************************************
* Filename:
* Date:
* Mod. Date:
* Mod. Initials:
* Author:
* Purpose:
************************************************/
#pragma once
#ifdef RENDERER_EXPORTS
#define RENDERER_API __declspec(dllexport)
#else
#define RENDERER_API __declspec(dllimport)
#endif
#include <d3d11.h>
namespace Renderer
{
	class CCommonStateObjects
	{

		static bool m_bInstantiated;
		//Public state objects
	public:
		enum eBS_States : unsigned int
		{
			Opaque_BS = 0, AlphaBlend_BS, Additive_BS, NonPremultiplied_BS, PaticleAlphaBlend_BS, COUNT_BS
		};
		

		enum eDSS_States : unsigned int
		{
			DepthNone_DSS = 0, DepthDefault_DSS, DepthRead_DSS, COUNT_DSS
		};
		

		enum eRS_States : unsigned int
		{
			CullNone_RS = 0, CullClockwise_RS, CullCounterClockwise_RS, Wireframe_RS, COUNT_RS
		};

		enum eSS_States : unsigned int
		{
			PointWrap = 0, PointClamp, LinearWrap, LinearClamp, TrilinearWrap, TrilinearClamp, AnisotropicWrap, AnisotropicClamp ,COUNT_SS
		};

		


		// Blend states.
		static ID3D11BlendState* m_blendStates[COUNT_BS];
		// Depth stencil states.
		static ID3D11DepthStencilState* m_depthStencilStates[COUNT_DSS];
		// Rasterizer states.
		static ID3D11RasterizerState* m_rasterizerStates[COUNT_RS];
		// Sampler states.
		static ID3D11SamplerState*  m_samplerStates[COUNT_SS];

	public:
		CCommonStateObjects();
		~CCommonStateObjects();
	};

}