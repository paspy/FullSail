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

#include "DeviceResoureces.h"
#include "ConstantBuffer.h"
#include <GeometricPrimitive.h>

namespace G2D {
	class CGraphics2D;
}
namespace DirectX
{
	class GeometricPrimitive;
}
namespace Renderer
{

	class CView;
	class CShaderEffect;
	class CMaterial;
	class CCommonStateObjects;
	class CRenderable;
	class CParticle;
	class C3DHUD;

	class CRendererController
	{
	public:
		enum eRenderBucket : unsigned int
		{
			DEFAULT = 0, SKYBOX, WATER, POST_INVERTCOLOR, POST_FADEINOUT, POST_SEPIALTONE, POST_VIGNETTE, SHADEREFFECT_COUNT
		};


	private:
		struct DebugShape
		{
			const void* const id;
			std::unique_ptr<DirectX::GeometricPrimitive> shape;
			DirectX::XMFLOAT4 color;
			bool wireframe;
			const DirectX::XMFLOAT4X4* worldMatrix;

			DebugShape(const void* const _id) : id(_id)
			{

			}

			bool operator==(const DebugShape& _rhs)
			{
				return this->id == _rhs.id;
			}
		};
	private:
		//Post-Processing
		ID3D11Texture2D*					m_d3dBackBufferCopy;
		ID3D11ShaderResourceView*			m_d3dBackBufferCopySRV;

		//In-Game Hud
		std::list<C3DHUD*>					m_InGameHUD;

		static bool m_bInstantiated;


		std::unique_ptr<CShaderEffect> m_ShaderEffects[SHADEREFFECT_COUNT];

		CView* m_MainCamera;

		std::list<DebugShape> m_DebugShapes;

		std::unordered_map<std::wstring, std::vector<CParticle*>* > m_umParticleGroup;

		std::unique_ptr<CMaterial> skyboxMat;
		std::unique_ptr<CRenderable> skyboxSphere;
		DirectX::XMFLOAT4X4 skyboxWorld;

		//  std::unique_ptr<CTerrain> m_terrain;
		unsigned		m_uPostProcessEffectIndex;
		cbPost			m_PostProcessCBufferData;
		ID3D11Buffer*	m_PostProcessCBuffer;
		static float    m_fDeltaTime;
		static float    m_fTotalTime;
		static float    m_fWaterScrollSpeed;

	public:

		RENDERER_API std::unordered_map<std::wstring, std::vector<CParticle*>* >& ParticleGroup() { return m_umParticleGroup; }

		RENDERER_API static std::unique_ptr<CDeviceResoureces> m_deviceResources;
		static std::unique_ptr<CCommonStateObjects> m_CommonState;

		RENDERER_API CRendererController(HWND mainWindow, unsigned int uwidth = 1920u, unsigned int uheight = 1080u, bool windowed = false, float gamma = 1.0f);
		RENDERER_API ~CRendererController();
		RENDERER_API void SetMainCamera(CView* _mainCamera);
		RENDERER_API CView* GetMainCamera() const { return m_MainCamera; }
		RENDERER_API void SetUpRenderables(std::vector<CRenderable*>& _renderSet);
		RENDERER_API void ClearRenderables();
		RENDERER_API void DrawSkybox();
		RENDERER_API void SetupSkybox();
		RENDERER_API void SetupTerrain();
		RENDERER_API void Draw();
		//POST_INVERTCOLOR = 3, POST_FADEINOUT, POST_SEPIALTONE
		RENDERER_API void PostProcessDraw(unsigned effect);
		RENDERER_API void PostProcessFade();
		RENDERER_API void Resize()
		{
			SAFE_RELEASE(m_d3dBackBufferCopySRV);
			SAFE_RELEASE(m_d3dBackBufferCopy);
		}

		//RENDERER_API  void SetTotalTime(float totaltime) { m_PostProcessCBufferData.gTotaltime = totaltime; }
		RENDERER_API static void SetTotalTime(float tt) { m_fTotalTime = tt; }
		RENDERER_API static void SetDeltaTime(float dt) { m_fDeltaTime = dt; }
		RENDERER_API static void SetWaterScrollSpeed(float speed) { m_fWaterScrollSpeed = speed; }

		RENDERER_API static float GetTotalTime(void) { return m_fTotalTime; }
		RENDERER_API static float GetDeltaTime(void) { return m_fDeltaTime; }
		RENDERER_API static float GetWaterScrollSpeed(void) { return m_fWaterScrollSpeed; }

		RENDERER_API  void        SetPostProcessIndex(unsigned index) { m_uPostProcessEffectIndex = index; }
		RENDERER_API  unsigned    GetPostProcessIndex(void) { return m_uPostProcessEffectIndex; }

		RENDERER_API  void  SetPostProcessFadeAmount(float amount)
		{
			m_PostProcessCBufferData.gFadeAmount = amount;
		}
		RENDERER_API void Add3DHUD(C3DHUD* in)
		{
			m_InGameHUD.push_back(in);
		}

		RENDERER_API void Remove3DHUD(C3DHUD* in)
		{
			m_InGameHUD.remove(in);
		}


		//Debug Function
		RENDERER_API void AddDebugSphere(const void* const _id, const DirectX::XMFLOAT4X4& _worldMatrix, DirectX::XMFLOAT4& _color = DirectX::XMFLOAT4{ 1.0f,0.0f,0.0f,0.5f }, bool _wireframe = false);
		RENDERER_API void AddDebugCube(const void* const _id, DirectX::XMFLOAT4X4& _worldMatrix, DirectX::XMFLOAT4& _color = DirectX::XMFLOAT4{ 1.0f,0.0f,0.0f,0.5f }, bool _wireframe = false);
		RENDERER_API void RemoveDebugShape(const void* const _id);
		RENDERER_API void SetDebugShapeColor(const void* const _id, DirectX::XMFLOAT4& _color);
		RENDERER_API void DrawLine();



	private:
		void DrawDebugShapes();




	private:
		CRendererController(const CRendererController&) = delete;
		CRendererController& operator=(const CRendererController&) = delete;
	};

}