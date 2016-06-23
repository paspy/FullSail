#pragma once
#include "Renderable.h"
namespace Renderer
{
	class CWaterRenderable :
		public CRenderable
	{
		///Compute shader 
		ID3D11ComputeShader * m_CSWater = nullptr;
		ID3D11UnorderedAccessView* m_WaterUAV = nullptr;
	public:
		RENDERER_API CWaterRenderable(CMaterial& material);
		RENDERER_API virtual ~CWaterRenderable();

		RENDERER_API void SetDarkWaterHalfWidth(float _width) { m_PerObjectData.gDarkWaterSize.x = _width; }
		RENDERER_API void SetDarkWaterHalfHeight(float _height) { m_PerObjectData.gDarkWaterSize.y = _height; }
		RENDERER_API void SetDarkWaterCenter(float _x, float _y) { m_PerObjectData.gDarkWaterCenter.x = _x; 
		m_PerObjectData.gDarkWaterCenter.y = _y;
		}
		virtual void Begin(IRenderNode* pCurrentView) final;
		virtual void End(IRenderNode* pCurrentView) final;
	};

}