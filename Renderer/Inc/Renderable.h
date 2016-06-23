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
#include "Light.h"
#include "ConstantBuffer.h"


class CAssetManager;

namespace Renderer
{
	class CMaterial;
	class CInputLayoutManager;

	class CRenderable : public IRenderNode
	{
		friend class CView;
		friend class CRendererController;


		protected:

		bool m_Visible;
		bool m_BeingHit;
		unsigned int		m_RenderBucketIndex;
		CMaterial&			m_Material;


		const DirectX::XMFLOAT4X4*  m_d3dWorldMatrix;
		float				m_Depth;


		unsigned int		m_uNumofVertices;
		unsigned int		m_uNumofIndices;
		ID3D11Buffer*		m_d3dVertexBuffer;
		unsigned int        m_Stride;
		ID3D11Buffer*		m_d3dIndexBuffer;
		unsigned int		m_InputLayoutIndex;
		ID3D11InputLayout*	m_d3dInputLayout;


		cbPerObject         m_PerObjectData;
		ID3D11Buffer*		m_d3dPerObjectCBuffer;





		static CInputLayoutManager& m_InputLayoutManager;
		static CAssetManager& m_AssetManager;
	public:
		RENDERER_API CRenderable(CMaterial& material, const char*  fileName,
			unsigned int _RenderBucketIndex = 0,
			unsigned int _inputLayoutIndex = 1);
		RENDERER_API inline void SetWorldMatrix(const DirectX::XMFLOAT4X4& _worldMatix) { m_d3dWorldMatrix = &_worldMatix; }
		RENDERER_API virtual ~CRenderable();
		RENDERER_API bool IsVisible() { return m_Visible; }
		RENDERER_API void SetVisible(bool _visible) { m_Visible = _visible; }
		RENDERER_API void SetRenderBucketIndex(unsigned int _index) { m_RenderBucketIndex = _index; }
		RENDERER_API void HitEffect(bool _hit) { m_BeingHit = _hit; }

		virtual void Begin(IRenderNode* pCurrentView) override;
		virtual void End(IRenderNode* pCurrentView) override;
		void SetPerObjectData(DirectX::XMFLOAT4X4& view, DirectX::XMFLOAT4X4& proj);
	};
}
