/***********************************************
* Filename:
* Date:
* Mod. Date:
* Mod. Initials:
* Author:
* Purpose:
************************************************/
#include "stdafx.h"
#include "..\stdafx.h"
#include "..\Inc\Renderable.h"
#include "..\Inc\InputLayoutManager.h"
#include "..\Inc\RendererController.h"
#include "..\Inc\View.h"
#include <AssetManager.h>


using namespace std;
using namespace DirectX;

namespace Renderer
{
	CInputLayoutManager& CRenderable::m_InputLayoutManager = CInputLayoutManager::GetRef();
	CAssetManager& CRenderable::m_AssetManager = CAssetManager::GetRef();


	CRenderable::CRenderable(CMaterial& material, const char* fileName, unsigned int _RenderBucketIndex, unsigned int _inputLayoutIndex)
		:
		m_Material(material),
		m_d3dWorldMatrix(nullptr),
		m_Visible(true),
		m_BeingHit(false),
		m_Depth(0.0f),
		m_uNumofIndices(0),
		m_uNumofVertices(0),
		m_InputLayoutIndex(_inputLayoutIndex),
		m_RenderBucketIndex(_RenderBucketIndex),
		m_d3dVertexBuffer(nullptr),
		m_d3dIndexBuffer(nullptr),
		m_d3dInputLayout(nullptr),
		m_d3dPerObjectCBuffer(nullptr)
	{
		switch (m_InputLayoutIndex)
		{
		case CInputLayoutManager::eVertex_POS:
			m_d3dVertexBuffer = m_AssetManager.Meshes(fileName)->D3DData.pPOS_VBuffer;
			break;
		case CInputLayoutManager::eVertex_POSNORUVTAN:
			m_d3dVertexBuffer = m_AssetManager.Meshes(fileName)->D3DData.pPOSNORUVTAN_VBuffer;
			break;
		default:
			assert(false && "Invalid InputLayout index.");
			break;
		}


		m_Stride = m_AssetManager.Meshes(fileName)->D3DData.m_Stride[m_InputLayoutIndex];
		m_d3dIndexBuffer = m_AssetManager.Meshes(fileName)->D3DData.pIBuffer;
		m_uNumofIndices = (unsigned int)m_AssetManager.Meshes(fileName)->RawData.indices.size();
		m_uNumofVertices = (unsigned int)m_AssetManager.Meshes(fileName)->RawData.pos.size();



		//Create CBuffer
		D3D11_BUFFER_DESC bd;
		ZeroMemory(&bd, sizeof(bd));
		bd.Usage = D3D11_USAGE_DYNAMIC;
		bd.ByteWidth = sizeof(m_PerObjectData);
		bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		CRendererController::m_deviceResources->GetD3DDevice()->CreateBuffer(&bd, nullptr, &m_d3dPerObjectCBuffer);


		m_d3dInputLayout = m_InputLayoutManager.inputLayouts[m_InputLayoutIndex];


	}



	CRenderable::~CRenderable()
	{
		SAFE_RELEASE(m_d3dPerObjectCBuffer);
	}




	/*virtual*/ void CRenderable::Begin(IRenderNode* pCurrentView) /*final*/
	{
		if (!m_Visible)
		{
			return;
		}
		CView& view = (CView&)(*pCurrentView);
		view.m_CurrentRenderable = this;
		static auto deviceContextPtr = CRendererController::m_deviceResources->GetD3DDeviceContext();
		deviceContextPtr->IASetInputLayout(m_d3dInputLayout);
		SetPerObjectData(view.ViewMatrix(), view.ProjectionMatrix());

		unsigned int offset = 0;
		deviceContextPtr->IASetVertexBuffers(0, 1, &m_d3dVertexBuffer, &m_Stride, &offset);
		deviceContextPtr->IASetIndexBuffer(m_d3dIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
		deviceContextPtr->DrawIndexed(m_uNumofIndices, 0, 0);

	}
	/*virtual*/ void CRenderable::End(IRenderNode* pCurrentView) /*final*/
	{
		static auto deviceContextPtr = CRendererController::m_deviceResources->GetD3DDeviceContext();
		deviceContextPtr->IASetInputLayout(nullptr);
		unsigned int strid = 0;
		unsigned int offset = 0;
		ID3D11Buffer * vs[] = { nullptr };
		deviceContextPtr->IASetVertexBuffers(0, 1, vs, &strid, &offset);
		deviceContextPtr->IASetIndexBuffer(nullptr, DXGI_FORMAT_R32_UINT, 0);

		CView& view = (CView&)(*pCurrentView);
		view.m_CurrentRenderable = nullptr;
	}





	void CRenderable::SetPerObjectData(DirectX::XMFLOAT4X4& view, DirectX::XMFLOAT4X4& proj)
	{
		m_PerObjectData.gBeingHit = m_BeingHit;
		auto vp = XMMatrixMultiply(XMLoadFloat4x4(&view), XMLoadFloat4x4(&proj));
		auto mvp = XMMatrixMultiply(XMLoadFloat4x4(m_d3dWorldMatrix), vp);
		XMStoreFloat4x4(&m_PerObjectData.gMVP, mvp);
		m_PerObjectData.gWorld = *m_d3dWorldMatrix;

		auto deviceContextPtr = CRendererController::m_deviceResources->GetD3DDeviceContext();

		D3D11_MAPPED_SUBRESOURCE edit;
		deviceContextPtr->Map(m_d3dPerObjectCBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &edit);
		*(cbPerObject*)edit.pData = m_PerObjectData;
		deviceContextPtr->Unmap(m_d3dPerObjectCBuffer, 0);
		deviceContextPtr->VSSetConstantBuffers(m_PerObjectData.REGISTER_SLOT, 1, &m_d3dPerObjectCBuffer);
		deviceContextPtr->PSSetConstantBuffers(m_PerObjectData.REGISTER_SLOT, 1, &m_d3dPerObjectCBuffer);
		deviceContextPtr->CSSetConstantBuffers(m_PerObjectData.REGISTER_SLOT, 1, &m_d3dPerObjectCBuffer);
	}
}