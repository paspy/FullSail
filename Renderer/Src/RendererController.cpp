/***********************************************
* Filename:
* Date:
* Mod. Date:
* Mod. Initials:
* Author:
* Purpose:
************************************************/
#include "stdafx.h"
#ifdef _DEBUG
//#include <vld.h>
#endif // _DEBUG

#include "..\stdafx.h"
#include <DDSTextureLoader.h>
#include <PrimitiveBatch.h>
#include <VertexTypes.h>
#include <Effects.h>
#include "CommonStates.h"
#include <AssetManager.h>
#include "..\Inc\RendererController.h"
#include "..\Inc\ShaderEffect.h"
#include "..\Inc\RenderSet.h"
#include "..\Inc\ShaderPass.h"
#include "..\Inc\Material.h"
#include "..\Inc\View.h"
#include "..\Inc\InputLayoutManager.h"
#include "..\Inc\ParticleCollection.h"
#include "..\Inc\CommonStateObjects.h"
#include "..\Inc\Renderable.h"
#include "..\Inc\Particle.h"
#include "..\Inc\Graphics2D.h"
#include "..\Inc\C3DHUD.h"

using namespace DirectX;
using namespace std;

namespace Renderer
{
	bool CRendererController::m_bInstantiated = false;
	std::unique_ptr<CDeviceResoureces> CRendererController::m_deviceResources = { nullptr };
	std::unique_ptr<CCommonStateObjects> CRendererController::m_CommonState = { nullptr };
	float CRendererController::m_fDeltaTime = 0.0f;
	float CRendererController::m_fTotalTime = 0.0f;
	float CRendererController::m_fWaterScrollSpeed = 0.0f;


	CRendererController::CRendererController(HWND mainWindow, unsigned int uwidth, unsigned int uheight, bool windowed, float gamma) :
		m_MainCamera(nullptr), m_uPostProcessEffectIndex(0)
	{
		assert(!m_bInstantiated && "Only one CRendererController instance is allowed.");
		m_deviceResources = std::make_unique<CDeviceResoureces>(mainWindow, uwidth, uheight, windowed, gamma);
		m_bInstantiated = true;

		CInputLayoutManager::GetRef().Initilize();
		CParticleCollection::GetRef().Init(m_deviceResources->GetD3DDevice(), m_deviceResources->GetD3DDeviceContext());
		m_CommonState = make_unique<CCommonStateObjects>();
		//Bind all samplers
		auto deviceContext = m_deviceResources->GetD3DDeviceContext();
		deviceContext->CSSetSamplers(0, CCommonStateObjects::COUNT_SS, m_CommonState->m_samplerStates);
		deviceContext->VSSetSamplers(0, CCommonStateObjects::COUNT_SS, m_CommonState->m_samplerStates);
		deviceContext->PSSetSamplers(0, CCommonStateObjects::COUNT_SS, m_CommonState->m_samplerStates);
		deviceContext->HSSetSamplers(0, CCommonStateObjects::COUNT_SS, m_CommonState->m_samplerStates);
		deviceContext->GSSetSamplers(0, CCommonStateObjects::COUNT_SS, m_CommonState->m_samplerStates);
		deviceContext->DSSetSamplers(0, CCommonStateObjects::COUNT_SS, m_CommonState->m_samplerStates);


		//Initialize all  ShaderEffects
		m_ShaderEffects[DEFAULT] = make_unique<CShaderEffect>();
		m_ShaderEffects[DEFAULT]->SetShaderPass(new CShaderPass(m_deviceResources->GetD3DDevice(),
			"CSO\\VS_Model.cso",
			"CSO\\PS_Model.cso",
			nullptr,
			nullptr,
			nullptr,
			CCommonStateObjects::Opaque_BS,
			CCommonStateObjects::DepthDefault_DSS,
			CCommonStateObjects::CullNone_RS));


		//SkyBox
		m_ShaderEffects[SKYBOX] = make_unique<CShaderEffect>();
		m_ShaderEffects[SKYBOX]->SetShaderPass(new CShaderPass(m_deviceResources->GetD3DDevice(),
			"CSO\\VS_Skybox.cso",
			"CSO\\PS_Skybox.cso",
			nullptr,
			nullptr,
			nullptr,
			CCommonStateObjects::Opaque_BS,
			CCommonStateObjects::DepthNone_DSS,
			CCommonStateObjects::CullNone_RS));
	

		//Water
		m_ShaderEffects[WATER] = make_unique<CShaderEffect>();
		m_ShaderEffects[WATER]->SetShaderPass(new CShaderPass(m_deviceResources->GetD3DDevice(),
			"CSO\\VS_Water.cso",
			"CSO\\PS_Water.cso",
			nullptr,
			nullptr,
			nullptr,
			CCommonStateObjects::AlphaBlend_BS,
			CCommonStateObjects::DepthDefault_DSS,
			CCommonStateObjects::CullNone_RS));

		//Terrain
	   /*m_ShaderEffects[TERRAIN] = make_unique<CShaderEffect>();
		m_ShaderEffects[TERRAIN]->SetShaderPass( new CShaderPass( m_deviceResources->GetD3DDevice(),
			"CSO\\VS_Terrain.cso",
			"CSO\\PS_Terrain.cso",
			"CSO\\HS_Terrain.cso",
			"CSO\\DS_Terrain.cso",
			nullptr,
			CCommonStateObjects::Opaque_BS,
			CCommonStateObjects::DepthDefault_DSS,
			CCommonStateObjects::CullNone_RS ) );
		*/

		//Post-InvertColor
		m_ShaderEffects[POST_INVERTCOLOR] = make_unique<CShaderEffect>();
		m_ShaderEffects[POST_INVERTCOLOR]->SetShaderPass(new CShaderPass(m_deviceResources->GetD3DDevice(),
			"CSO\\VS_POST.cso",
			"CSO\\PS_POST.cso",
			nullptr,
			nullptr,
			nullptr,
			CCommonStateObjects::Opaque_BS,
			CCommonStateObjects::DepthNone_DSS,
			CCommonStateObjects::CullNone_RS));

		//POST-Fade
		m_ShaderEffects[POST_FADEINOUT] = make_unique<CShaderEffect>();
		m_ShaderEffects[POST_FADEINOUT]->SetShaderPass(new CShaderPass(m_deviceResources->GetD3DDevice(),
			"CSO\\VS_POST.cso",
			"CSO\\PS_POSTFADE.cso",
			nullptr,
			nullptr,
			nullptr,
			CCommonStateObjects::Opaque_BS,
			CCommonStateObjects::DepthNone_DSS,
			CCommonStateObjects::CullNone_RS));

		//POST-Sepia Tone
		m_ShaderEffects[POST_SEPIALTONE] = make_unique<CShaderEffect>();
		m_ShaderEffects[POST_SEPIALTONE]->SetShaderPass(new CShaderPass(m_deviceResources->GetD3DDevice(),
			"CSO\\VS_POST.cso",
			"CSO\\PS_POSTSEPIATONE.cso",
			nullptr,
			nullptr,
			nullptr,
			CCommonStateObjects::Opaque_BS,
			CCommonStateObjects::DepthNone_DSS,
			CCommonStateObjects::CullNone_RS));


		//POST-Vignette
		m_ShaderEffects[POST_VIGNETTE] = make_unique<CShaderEffect>();
		m_ShaderEffects[POST_VIGNETTE]->SetShaderPass(new CShaderPass(m_deviceResources->GetD3DDevice(),
			"CSO\\VS_POST.cso",
			"CSO\\PS_POSTVIGNETTE.cso",
			nullptr,
			nullptr,
			nullptr,
			CCommonStateObjects::Opaque_BS,
			CCommonStateObjects::DepthNone_DSS,
			CCommonStateObjects::CullNone_RS));



		ZeroMemory(&m_PostProcessCBufferData, sizeof m_PostProcessCBufferData);
		D3D11_BUFFER_DESC bd = { 0 };
		bd.Usage = D3D11_USAGE_DYNAMIC;
		bd.ByteWidth = sizeof(m_PostProcessCBufferData);
		bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		CRendererController::m_deviceResources->GetD3DDevice()->CreateBuffer(&bd, nullptr, &m_PostProcessCBuffer);



		D3D11_TEXTURE2D_DESC backBufferDesc;
		m_deviceResources->GetBackBuffer()->GetDesc(&backBufferDesc);

		AssertIfFailed(
			m_deviceResources->GetD3DDevice()->CreateTexture2D(
				&backBufferDesc,
				nullptr,
				&m_d3dBackBufferCopy
			)
		);

		CD3D11_SHADER_RESOURCE_VIEW_DESC backBufferSRVDesc;
		ZeroMemory(&backBufferSRVDesc, sizeof(backBufferSRVDesc));
		backBufferSRVDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM_SRGB;
		backBufferSRVDesc.Texture2D.MipLevels = 1;
		backBufferSRVDesc.Texture2D.MostDetailedMip = 0;
		backBufferSRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		AssertIfFailed(
			m_deviceResources->GetD3DDevice()->CreateShaderResourceView(
				m_d3dBackBufferCopy, &backBufferSRVDesc, &m_d3dBackBufferCopySRV
			)
		);

		C3DHUD::InitShaderPass();



	}

	void CRendererController::SetupSkybox()
	{
		skyboxMat = make_unique<CMaterial>("testDefaultMaterial.xml");
		skyboxMat->AddMap(5, CAssetManager::GetRef().Textures("SkyboxOcean.dds"));
		skyboxSphere = make_unique<CRenderable>(*skyboxMat.get(), "SkyboxCube.mesh");
		skyboxWorld = XMFLOAT4X4{
		   1.0f,0.0f,0.0f,0.0f,
		   0.0f,1.0f,0.0f,0.0f,
		   0.0f,0.0f,1.0f,0.0f,
		   0.0f,0.0f,0.0f,1.0f };
		skyboxSphere->SetWorldMatrix(skyboxWorld);
		skyboxSphere->m_d3dVertexBuffer = CAssetManager::GetRef().Meshes("SkyboxCube.mesh")->D3DData.pPOS_VBuffer;
		skyboxSphere->m_d3dInputLayout = CInputLayoutManager::GetRef().InputLayout(CInputLayoutManager::eVertex_POS);
		skyboxSphere->m_Stride = CAssetManager::GetRef().Meshes("SkyboxCube.mesh")->D3DData.m_Stride[0];
	}

	void CRendererController::DrawSkybox()
	{
		m_MainCamera->GetOpaqueShaderEffects()->AddtoHead(m_ShaderEffects[SKYBOX].get());
		m_ShaderEffects[SKYBOX]->Materials()->AddtoHead(skyboxMat.get());
		skyboxMat->m_Renderables->AddtoHead(skyboxSphere.get());

	}

	void CRendererController::SetupTerrain() {
		//m_terrain = make_unique<CTerrain>( L"../../FullSail/Resources/Terrain/level1.fsterrain" );
		//m_ShaderEffects[TERRAIN]->Terrains()->AddtoHead( m_terrain.get() );

	}

	void CRendererController::SetMainCamera(CView* _mainCamera)
	{
		if (m_MainCamera)
		{
			m_MainCamera->GetOpaqueShaderEffects()->m_Set.clear();
		}
		m_MainCamera = _mainCamera;
		if (m_MainCamera)
		{
			m_MainCamera->GetOpaqueShaderEffects()->m_Set.clear();
			m_MainCamera->GetOpaqueShaderEffects()->AddtoHead(m_ShaderEffects[DEFAULT].get());
			//m_MainCamera->GetOpaqueShaderEffects()->AddtoHead(m_ShaderEffects[SKYBOX].get());
			m_MainCamera->GetOpaqueShaderEffects()->AddtoTail(m_ShaderEffects[WATER].get());
			// m_MainCamera->GetOpaqueShaderEffects()->AddtoTail( m_ShaderEffects[TERRAIN].get() );
			//SetupSkybox();

			//SetupTerrain();
		}
	}

	bool Unique_Material(IRenderNode *lhs, IRenderNode *rhs)
	{
		return lhs == rhs;
	}

	void CRendererController::SetUpRenderables(std::vector<CRenderable*>& _renderSet)
	{
		
		//push materials
		for (auto& eachRenderable : _renderSet)
		{
			m_ShaderEffects[eachRenderable->m_RenderBucketIndex]->Materials()->AddtoHead(&eachRenderable->m_Material);
			//m_ShaderEffects[eachRenderable->m_RenderBucketIndex]->Materials()->m_Set.unique(Unique_Material);
			eachRenderable->m_Material.m_Renderables->AddtoHead(eachRenderable);
		}
#if 0 //Noise Map Gen code


		CPerlinNoise noise;
		noise.GenerateWhiteNoise();
		noise.GeneratePerlinNoise();

		DirectX::Image image;
		image.format = DXGI_FORMAT_R32_FLOAT;
		image.height = image.width = 2048;
		image.rowPitch = 2048 * sizeof(float);
		image.slicePitch = 2048 * 2048 * sizeof(float);
		image.pixels = (uint8_t*)noise.m_PerlinNoise;
		DirectX::SaveToDDSFile(image, DDS_FLAGS_NONE, L"NOISE2.DDS");

#endif
	}

	void CRendererController::ClearRenderables()
	{
		for (auto& eachShaderEffect : m_ShaderEffects)
		{
			for (auto& eachMaterial : eachShaderEffect->Materials()->m_Set)
			{
				auto mat = (CMaterial*)eachMaterial;
				mat->m_Renderables->m_Set.clear();
			}
			eachShaderEffect->Materials()->m_Set.clear();
		}

	}

	CRendererController::~CRendererController()
	{
		m_deviceResources->GetD3DDeviceContext()->Flush();
		m_deviceResources->GetD3DDeviceContext()->ClearState();

		C3DHUD::ReleaseShaderPass();
		CInputLayoutManager::DeleteInstance();
		CParticleCollection::GetRef().Destory();
		m_CommonState.reset();

		SAFE_RELEASE(m_PostProcessCBuffer);
		SAFE_RELEASE(m_d3dBackBufferCopySRV);
		SAFE_RELEASE(m_d3dBackBufferCopy);

		for (auto& eachShaderEffect : m_ShaderEffects)
		{
			eachShaderEffect.reset();
		}
		skyboxMat.reset();
		skyboxSphere.reset();
		//m_terrain.reset();
		m_DebugShapes.clear();


		m_deviceResources.reset();
		m_bInstantiated = false;
	}
	void CRendererController::DrawLine()
	{
		static_assert(true, "The function is not implemented!");
		/*static auto d3dDeviceContext = m_deviceResources->GetD3DDeviceContext();
		static std::unique_ptr<PrimitiveBatch<VertexPositionColor>> primitiveBatch(new PrimitiveBatch<VertexPositionColor>(d3dDeviceContext));
		std::unique_ptr<BasicEffect> basicEffect(new BasicEffect(m_deviceResources->GetD3DDevice()));

		basicEffect->SetProjection(XMMatrixOrthographicOffCenterLH(0, 1024, 768, 0, 0, 1));
		basicEffect->SetVertexColorEnabled(true);

		void const* shaderByteCode;
		size_t byteCodeLength;

		basicEffect->GetVertexShaderBytecode(&shaderByteCode, &byteCodeLength);

		Microsoft::WRL::ComPtr<ID3D11InputLayout> inputLayout;

		m_deviceResources->GetD3DDevice()->CreateInputLayout(VertexPositionColor::InputElements,
			VertexPositionColor::InputElementCount,
			shaderByteCode, byteCodeLength,
			inputLayout.GetAddressOf());



		d3dDeviceContext->OMSetBlendState(CCommonStateObjects::m_blendStates[CCommonStateObjects::Opaque_BS], nullptr, 0xFFFFFFFF);
		d3dDeviceContext->OMSetDepthStencilState(CCommonStateObjects::m_depthStencilStates[CCommonStateObjects::DepthNone_DSS], 0);
		d3dDeviceContext->RSSetState(CCommonStateObjects::m_rasterizerStates[CCommonStateObjects::CullNone_RS]);

		basicEffect->Apply(d3dDeviceContext);
		d3dDeviceContext->IASetInputLayout(inputLayout.Get());

		primitiveBatch->Begin();
		const XMVECTORF32 s1 = { 10.f, 50.f, 1.f };
		const XMVECTORF32 s2 = { 1024.f, 200.f, 1.f };

		VertexPositionColor v1(s1, Colors::Green);
		VertexPositionColor v2(s2, Colors::Green);
		primitiveBatch->DrawLine(v1, v2);
		primitiveBatch->End();*/


	}

	//MAKE SURE THAT THE MATRIX DOESNT FALL OUT OF SCOPE WHEN YOU USE THIS
	void CRendererController::AddDebugSphere(const void* const _id, const XMFLOAT4X4& _worldMatrix, XMFLOAT4& _color, bool _wireframe)
	{
		/*DebugShape sphere(_id);
		static auto d3dDeviceContext = m_deviceResources->GetD3DDeviceContext();
		sphere.shape = GeometricPrimitive::CreateSphere(d3dDeviceContext,1.0f,16,false);
		sphere.color = _color;
		sphere.wireframe = _wireframe;
		sphere.worldMatrix = &_worldMatrix;
		m_DebugShapes.push_back(std::move(sphere));*/
	}

	void CRendererController::AddDebugCube(const void* const _id, XMFLOAT4X4& _worldMatrix, XMFLOAT4& _color, bool _wireframe)
	{
		/*DebugShape cube(_id);
		static auto d3dDeviceContext = m_deviceResources->GetD3DDeviceContext();
		cube.shape = GeometricPrimitive::CreateCube(d3dDeviceContext, 1.0f, false);
		cube.color = _color;
		cube.wireframe = _wireframe;
		cube.worldMatrix = &_worldMatrix;
		m_DebugShapes.push_back(std::move(cube));*/
	}

	void CRendererController::RemoveDebugShape(const void* const _id)
	{
		m_DebugShapes.remove(_id);
	}

	void CRendererController::SetDebugShapeColor(const void* const _id, DirectX::XMFLOAT4& _color)
	{
		//auto it = find(m_DebugShapes.begin(), m_DebugShapes.end(), _id);
		//if (it != m_DebugShapes.end()) {
		//	it->color = _color;
		//}
	}

	void CRendererController::DrawDebugShapes()
	{
		for (auto& eachShape : m_DebugShapes)
		{
			eachShape.shape->Draw(XMLoadFloat4x4(eachShape.worldMatrix),
				XMLoadFloat4x4(&m_MainCamera->ViewMatrix()),
				XMLoadFloat4x4(&m_MainCamera->ProjectionMatrix()),
				XMLoadFloat4(&eachShape.color),
				nullptr,
				eachShape.wireframe);
		}
	}

	void CRendererController::PostProcessDraw(unsigned effect)
	{
		if (effect == 0)
		{
			return;
		}
		static auto d3dDeviceContext = m_deviceResources->GetD3DDeviceContext();

		m_deviceResources->GetD3DDeviceContext()->CopyResource(m_d3dBackBufferCopy, m_deviceResources->GetBackBuffer());
		d3dDeviceContext->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
		d3dDeviceContext->PSSetShaderResources(9, 1, &m_d3dBackBufferCopySRV);

		m_ShaderEffects[effect]->Begin(m_MainCamera);
		d3dDeviceContext->Draw(4, 0);
		m_ShaderEffects[effect]->End(m_MainCamera);

		ID3D11ShaderResourceView* nullSrv = nullptr;
		d3dDeviceContext->PSSetShaderResources(9, 1, &nullSrv);
	}

	void CRendererController::PostProcessFade()
	{

		static auto d3dDeviceContext = m_deviceResources->GetD3DDeviceContext();

		D3D11_MAPPED_SUBRESOURCE edit;
		d3dDeviceContext->Map(m_PostProcessCBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &edit);
		*(cbPost*)edit.pData = m_PostProcessCBufferData;
		d3dDeviceContext->Unmap(m_PostProcessCBuffer, 0);

		m_deviceResources->GetD3DDeviceContext()->CopyResource(m_d3dBackBufferCopy, m_deviceResources->GetBackBuffer());
		d3dDeviceContext->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
		d3dDeviceContext->PSSetShaderResources(9, 1, &m_d3dBackBufferCopySRV);
		d3dDeviceContext->PSSetConstantBuffers(m_PostProcessCBufferData.REGISTER_SLOT, 1, &m_PostProcessCBuffer);

		m_ShaderEffects[POST_FADEINOUT]->Begin(m_MainCamera);
		d3dDeviceContext->Draw(4, 0);
		m_ShaderEffects[POST_FADEINOUT]->End(m_MainCamera);


		ID3D11ShaderResourceView* nullSrv = nullptr;
		d3dDeviceContext->PSSetShaderResources(9, 1, &nullSrv);
		static ID3D11Buffer* unbindBuffer = nullptr;
		d3dDeviceContext->PSSetConstantBuffers(m_PostProcessCBufferData.REGISTER_SLOT, 1, &unbindBuffer);
	}

	void CRendererController::Draw()
	{

		static auto d3dDeviceContext = m_deviceResources->GetD3DDeviceContext();
		ID3D11RenderTargetView* RTVs[]{ m_deviceResources->GetBackBufferRenderTargetView() };
		d3dDeviceContext->OMSetRenderTargets(1, RTVs, m_deviceResources->GetDepthStencilView());
		static FLOAT clearClor[4] = { 0.0f,0.0f,0.0f,1.0f };
		d3dDeviceContext->ClearRenderTargetView(m_deviceResources->GetBackBufferRenderTargetView(), clearClor);
		d3dDeviceContext->ClearDepthStencilView(m_deviceResources->GetDepthStencilView(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
		d3dDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		if (m_MainCamera)
		{
			m_MainCamera->Begin(m_MainCamera);


			for (auto& eachShaderEffect : m_MainCamera->GetOpaqueShaderEffects()->m_Set)
			{
				CShaderEffect& SEptr = (CShaderEffect&)(*eachShaderEffect);
				SEptr.Begin(m_MainCamera);

				for (auto& eachMateial : SEptr.Materials()->m_Set)
				{
					CMaterial& MATptr = (CMaterial&)(*eachMateial);
					MATptr.Begin(m_MainCamera);

					for (auto& eachRenderable : MATptr.m_Renderables->m_Set)
					{
						CRenderable& renderable = (CRenderable&)(*eachRenderable);
						if (renderable.IsVisible())
						{
							renderable.Begin(m_MainCamera);
							renderable.End(m_MainCamera);
						}

					}
					MATptr.End(m_MainCamera);
				}
				SEptr.End(m_MainCamera);

			}

			// Particle Pass
			for (auto it = m_umParticleGroup.begin(); it != m_umParticleGroup.end(); it++) {
				if ((*it).second && !(*it).second->empty()) {
					for (auto lst = ((*it).second)->begin(); lst != ((*it).second)->end(); lst++) {
						if (*lst) {
							(*lst)->Begin(m_MainCamera);
							(*lst)->End(m_MainCamera);
						}
					}
				}
			}

#if _DEBUG
			//DrawDebugShapes();
#endif

			//Draw in-game HUD
			for (auto& hud : m_InGameHUD)
			{
				hud->Begin( m_MainCamera );
				hud->End( m_MainCamera );
			}


			m_MainCamera->End(m_MainCamera);



			PostProcessDraw(m_uPostProcessEffectIndex);

			if (m_PostProcessCBufferData.gFadeAmount > 0.0f)
			{
				PostProcessFade();
			}

		}




	}


}