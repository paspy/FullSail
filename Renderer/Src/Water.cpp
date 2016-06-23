#include "stdafx.h"
#include "..\stdafx.h"
#include "..\Inc\Water.h"
#include "..\Inc\InputLayoutManager.h"
#include "..\Inc\RendererController.h"
#include "..\Inc\View.h"
#include <AssetManager.h>

namespace Renderer
{
	CWaterRenderable::CWaterRenderable(CMaterial& material) :
		CRenderable(material, "testWaterPlane.mesh", 2),
		m_CSWater(nullptr),
		m_WaterUAV(nullptr)
	{
		static auto devicePtr = CRendererController::m_deviceResources->GetD3DDevice();
		std::ifstream fin;
		fin.open("CSO\\CS_Water.cso", std::ios_base::binary);
		if (fin.is_open())
		{
			fin.seekg(0, std::ios_base::end);
			UINT byteCodeSize = (UINT)fin.tellg();
			char* byteCode = new char[byteCodeSize];
			fin.seekg(0, std::ios_base::beg);
			fin.read(byteCode, byteCodeSize);
			devicePtr->CreateComputeShader(byteCode, byteCodeSize, nullptr, &m_CSWater);
			fin.close();
			delete[] byteCode;
		}

		D3D11_BUFFER_DESC bufferDesc;
		m_d3dVertexBuffer->GetDesc(&bufferDesc);
		D3D11_UNORDERED_ACCESS_VIEW_DESC desc = {};
		desc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
		desc.Buffer.FirstElement = 0;
		desc.Format = DXGI_FORMAT_R32_TYPELESS; // Format must be DXGI_FORMAT_R32_TYPELESS, when creating Raw Unordered Access View
		desc.Buffer.Flags = D3D11_BUFFER_UAV_FLAG_RAW;
		desc.Buffer.NumElements = bufferDesc.ByteWidth / 4;
		devicePtr->CreateUnorderedAccessView(m_d3dVertexBuffer, &desc, &m_WaterUAV);

		m_PerObjectData.gDarkWaterSize.x = m_PerObjectData.gDarkWaterSize.y = 300.0f;
	}


	CWaterRenderable::~CWaterRenderable()
	{
		SAFE_RELEASE(m_WaterUAV);
		SAFE_RELEASE(m_CSWater);
	}

	/*virtual*/ void CWaterRenderable::Begin(IRenderNode* pCurrentView) /*final*/
	{
		CView& view = (CView&)(*pCurrentView);
		view.m_CurrentRenderable = this;
		static auto deviceContextPtr = CRendererController::m_deviceResources->GetD3DDeviceContext();
		
		deviceContextPtr->IASetIndexBuffer(m_d3dIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
		deviceContextPtr->IASetInputLayout(m_d3dInputLayout);
		SetPerObjectData(view.ViewMatrix(), view.ProjectionMatrix());

		deviceContextPtr->CSSetShader(m_CSWater, nullptr, 0);
		deviceContextPtr->CSSetUnorderedAccessViews(3, 1, &m_WaterUAV, 0);
		deviceContextPtr->Dispatch(101, 101, 1);
		ID3D11UnorderedAccessView* uavs[]{ nullptr };
		deviceContextPtr->CSSetUnorderedAccessViews(3, 1, uavs, 0);


		unsigned int offset = 0;
		deviceContextPtr->IASetVertexBuffers(0, 1, &m_d3dVertexBuffer, &m_Stride, &offset);
		deviceContextPtr->DrawIndexed(m_uNumofIndices, 0, 0);
	}

	/*virtual*/ void CWaterRenderable::End(IRenderNode* pCurrentView) /*final*/
	{
		static auto deviceContextPtr = CRendererController::m_deviceResources->GetD3DDeviceContext();
		deviceContextPtr->CSSetShader(nullptr, nullptr, 0);
		deviceContextPtr->IASetInputLayout(nullptr);
		unsigned int strid = 0;
		unsigned int offset = 0;
		ID3D11Buffer * vs[] = { nullptr };
		deviceContextPtr->IASetVertexBuffers(0, 1, vs, &strid, &offset);
		deviceContextPtr->IASetIndexBuffer(nullptr, DXGI_FORMAT_R32_UINT, 0);

		CView& view = (CView&)(*pCurrentView);
		view.m_CurrentRenderable = nullptr;
	}
}