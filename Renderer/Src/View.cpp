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
#include "..\Inc\View.h"
#include "..\Inc\RenderSet.h"
#include "..\Inc\RendererController.h"
#include <Miscellaneous.h>

#define NUMOFMAXLIGHTS 3


using namespace std;
using namespace DirectX;
namespace Renderer
{
	CView::CView() : m_CurrentState(NULLSTATE),
		m_opaqueShaderEffects(new CRenderSet),
		m_CurrentShaderEffect(nullptr),
		m_CurrentShaderPass(nullptr),
		m_CurrentMaterial(nullptr),
		m_CurrentRenderable(nullptr),
		m_d3dLightBuffer(nullptr),
		m_d3dLightSRV(nullptr)
	{
		ZeroMemory(&m_PerCameraData, sizeof m_PerCameraData);
		D3D11_BUFFER_DESC bd = { 0 };
		bd.Usage = D3D11_USAGE_DYNAMIC;
		bd.ByteWidth = sizeof(m_PerCameraData);
		bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		CRendererController::m_deviceResources->GetD3DDevice()->CreateBuffer(&bd, nullptr, &m_d3dPerCameraCBuffer);
		CreateLightStructedBuffer();

		
	}



	RENDERER_API DirectX::XMFLOAT4X4 CView::GetViewProjMatrix() const {
		XMFLOAT4X4 r;
		XMStoreFloat4x4(&r, XMLoadFloat4x4(&m_d3dViewMatrix)*XMLoadFloat4x4(&m_d3dProjMatrix));
		return r;
	}

    RENDERER_API void CView::SetOpaqueShaderEffects(CRenderSet* _newShaderEffects)
	{
		m_opaqueShaderEffects.reset(_newShaderEffects);
	}
    RENDERER_API void CView::SetTransparentRenderables(CRenderSet* _newTransparentRenderables)
	{
		m_transparentRenderables.reset(_newTransparentRenderables);
	}

    RENDERER_API void CView::SetParticles( CRenderSet* _newParticles ) {
        m_particles.reset( _newParticles );
    }

    CView::~CView()
	{
		SAFE_RELEASE(m_d3dLightBuffer);
		SAFE_RELEASE(m_d3dLightSRV);
		SAFE_RELEASE(m_d3dPerCameraCBuffer);
	}

	void CView::SetPerViewCBuffer()
	{
        m_PerCameraData.gView = m_d3dViewMatrix;
        m_PerCameraData.gProj = m_d3dProjMatrix;
		m_PerCameraData.gCameraDir = m_LookAtDirection;
		m_PerCameraData.gCameraPos = m_Position;
		m_PerCameraData.gDeltaTime = CRendererController::GetDeltaTime();
        m_PerCameraData.gTotalTime = CRendererController::GetTotalTime();
        m_PerCameraData.gWaterScrollSpeed = CRendererController::GetWaterScrollSpeed();

        //char msg_buf[1024];
        //sprintf_s( msg_buf, 1024, "Delta time: %f\n", m_PerCameraData.gDeltaTime );
        //OutputDebugStringA( msg_buf );

		XMMATRIX  viewProj = XMMatrixMultiply(XMLoadFloat4x4(&m_d3dViewMatrix), XMLoadFloat4x4(&m_d3dProjMatrix));

        FSUtilities::Misc::ExtractFrustumPlanes( m_PerCameraData.gWorldFrustumPlanes, viewProj );

        XMStoreFloat4x4( &m_PerCameraData.gViewProj, viewProj );
		XMMATRIX  invViewProj = XMMatrixInverse(nullptr, XMLoadFloat4x4(&m_PerCameraData.gViewProj));
		XMStoreFloat4x4(&m_PerCameraData.gInvViewProj, invViewProj);

		auto deviceContextPtr = CRendererController::m_deviceResources->GetD3DDeviceContext();
		D3D11_MAPPED_SUBRESOURCE edit;
		deviceContextPtr->Map(m_d3dPerCameraCBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &edit);
        memcpy_s( edit.pData, sizeof( cbPerCamera ), &m_PerCameraData, sizeof( cbPerCamera ) );
		//*(cbPerCamera*)edit.pData = m_PerCameraData;
		deviceContextPtr->Unmap(m_d3dPerCameraCBuffer, 0);


	}


	/*virtual*/ void CView::Begin(IRenderNode* pCurrentView) /*final*/
	{
		m_CurrentState = VIEW_BEGIN;

		SetPerViewCBuffer();
		SetLightStructedBuffer();
		auto d3dDeviceContext = CRendererController::m_deviceResources->GetD3DDeviceContext();
		d3dDeviceContext->CSSetConstantBuffers(m_PerCameraData.REGISTER_SLOT, 1, &m_d3dPerCameraCBuffer);
        d3dDeviceContext->VSSetConstantBuffers(m_PerCameraData.REGISTER_SLOT, 1, &m_d3dPerCameraCBuffer);
        d3dDeviceContext->GSSetConstantBuffers(m_PerCameraData.REGISTER_SLOT, 1, &m_d3dPerCameraCBuffer);
		d3dDeviceContext->PSSetConstantBuffers(m_PerCameraData.REGISTER_SLOT, 1, &m_d3dPerCameraCBuffer);
		d3dDeviceContext->PSSetShaderResources(Light::REGISTER_SLOT, 1, &m_d3dLightSRV);

	}

	/*virtual*/ void CView::End(IRenderNode* pCurrentView) /*final*/
	{
		



		m_CurrentState = VIEW_END;
		auto d3dDeviceContext = CRendererController::m_deviceResources->GetD3DDeviceContext();
		static ID3D11Buffer* unbindBuffer = nullptr;
		d3dDeviceContext->CSSetConstantBuffers(m_PerCameraData.REGISTER_SLOT, 1, &unbindBuffer);
		d3dDeviceContext->VSSetConstantBuffers(m_PerCameraData.REGISTER_SLOT, 1, &unbindBuffer);
        d3dDeviceContext->GSSetConstantBuffers(m_PerCameraData.REGISTER_SLOT, 1, &unbindBuffer);
        d3dDeviceContext->PSSetConstantBuffers(m_PerCameraData.REGISTER_SLOT, 1, &unbindBuffer);
		static ID3D11ShaderResourceView* unbindSRV = nullptr;
		d3dDeviceContext->PSSetShaderResources(Light::REGISTER_SLOT, 1, &unbindSRV);
	}


	//Lights
	void CView::CreateLightStructedBuffer()
	{
		m_LightList.resize(NUMOFMAXLIGHTS);
		//TODO:DELETE
		//DLIGHT
		ZeroMemory(&m_LightList[0], sizeof m_LightList[0]);
		m_LightList[0].Color = { 0.8f,0.6f,1.0f,1.0f };
		m_LightList[0].DirectionWS = { 0.0f, -0.707f, 0.0f,1.0f };
		m_LightList[0].Enabled = true;
		m_LightList[0].Intensity = 1.0f;
		m_LightList[0].SpotlightAngle = 90.0f;
		m_LightList[0].PositionWS = { 10.0f,1.0f,10.0f,1.0f };
		m_LightList[0].Range = 10.0f;
		m_LightList[0].Type = 0;


		ZeroMemory(&m_LightList[1], sizeof m_LightList[1]);
		m_LightList[1].Color = { 1.0f,0.0f,0.0f,1.0f };
		m_LightList[1].Enabled = false;
		m_LightList[1].Intensity = 1.0f;
		m_LightList[1].PositionWS = { 0.0f,1.0f,10.0f,1.0f };
		m_LightList[1].Range = 10.0f;
		m_LightList[1].Type = 1;


		ZeroMemory(&m_LightList[2], sizeof m_LightList[2]);
		m_LightList[2].Color = { 0.0f,0.0f,100.0f,1.0f };
		m_LightList[2].DirectionWS = { 1.0f, 0.0f, 0.0f,1.0f };
		m_LightList[2].Enabled = false;
		m_LightList[2].Intensity = 1.0f;
		m_LightList[2].SpotlightAngle = 90.0f;
		m_LightList[2].PositionWS = { -5.0f,0.0f,0.0f,1.0f };
		m_LightList[2].Range = 10.0f;
		m_LightList[2].Type = 2;

		




		auto& d3dDevice = *CRendererController::m_deviceResources->GetD3DDevice();
		D3D11_BUFFER_DESC bufferDesc = { 0 };
		bufferDesc.ByteWidth = (UINT)m_LightList.size() * sizeof(Light);
		bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		bufferDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		bufferDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
		bufferDesc.StructureByteStride = sizeof(Light);


		D3D11_SUBRESOURCE_DATA subResourceData;

		subResourceData.pSysMem = (void*)m_LightList.data();
		subResourceData.SysMemPitch = 0;
		subResourceData.SysMemSlicePitch = 0;
		d3dDevice.CreateBuffer(&bufferDesc, nullptr, &m_d3dLightBuffer);



		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Format = DXGI_FORMAT_UNKNOWN;
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
		srvDesc.Buffer.FirstElement = 0;
		srvDesc.Buffer.NumElements = NUMOFMAXLIGHTS;
		d3dDevice.CreateShaderResourceView(m_d3dLightBuffer, &srvDesc, &m_d3dLightSRV);

		SetD3DName(m_d3dLightSRV, "LightList");
	}

	void CView::SetLightStructedBuffer()
	{
		auto deviceContextPtr = CRendererController::m_deviceResources->GetD3DDeviceContext();
		D3D11_MAPPED_SUBRESOURCE edit = { 0 };
		deviceContextPtr->Map(m_d3dLightBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &edit);
		memcpy(edit.pData, m_LightList.data(), m_LightList.size() * sizeof(m_LightList[0]));
		deviceContextPtr->Unmap(m_d3dLightBuffer, 0);
	}

	bool CView::AddLight(Light in)
	{
		if (m_LightList.size() < NUMOFMAXLIGHTS)
		{
			m_LightList.push_back(in);
			return true;
		}
		else
			return false;

	}
	void CView::RemoveLight(int index)
	{
		m_LightList.erase(m_LightList.begin() + index);
	}
}