#pragma once
/***********************************************
* Filename:
* Date:
* Mod. Date:
* Mod. Initials:
* Author:
* Purpose:
************************************************/


#include "RenderNode.h"
#include "ConstantBuffer.h"
#include <SimpleMath.h>
#include "Light.h"

namespace Renderer
{
	class CRenderSet;


	class CView : public IRenderNode
	{
		friend class CShaderEffect;
		friend class CShaderPass;
		friend class CMaterial;
		friend class CRenderable;
		friend class CWaterRenderable;
	public:
		DirectX::SimpleMath::Vector3 m_Position;
		DirectX::SimpleMath::Vector3 m_LookAtDirection;
		DirectX::SimpleMath::Vector3 m_Up;
		DirectX::SimpleMath::Vector3 m_Right;


		float m_fFOV;
		float m_fAspect;
		float m_fNearPlane;
		float m_fFarPlane;

	private:
		eRenderState					m_CurrentState;
		//State Ids
		CShaderEffect*					m_CurrentShaderEffect;
		CShaderPass*					m_CurrentShaderPass;
		CMaterial*						m_CurrentMaterial;
		CRenderable*					m_CurrentRenderable;

		cbPerCamera						m_PerCameraData;
		ID3D11Buffer*					m_d3dPerCameraCBuffer;


		DirectX::XMFLOAT4X4				m_d3dViewMatrix;
		DirectX::XMFLOAT4X4				m_d3dProjMatrix;

		std::unique_ptr<CRenderSet>		m_opaqueShaderEffects;
		std::unique_ptr<CRenderSet>		m_transparentRenderables;

        std::unique_ptr<CRenderSet>     m_particles;

		ID3D11Buffer*					m_d3dLightBuffer;
		ID3D11ShaderResourceView*		m_d3dLightSRV;
		std::vector<Light>				m_LightList;

		

	public:
		//Accessors
		RENDERER_API inline CRenderSet* GetOpaqueShaderEffects() const { return m_opaqueShaderEffects.get(); }
        RENDERER_API inline CRenderSet* GetTransparentRenderables() const { return m_transparentRenderables.get(); }
        RENDERER_API inline CRenderSet* GetParticles() const { return m_particles.get(); }
		RENDERER_API inline DirectX::XMFLOAT4X4 ViewMatrix() const { return m_d3dViewMatrix; }
		RENDERER_API inline DirectX::XMFLOAT4X4 ProjectionMatrix() const { return m_d3dProjMatrix; }
		RENDERER_API inline DirectX::XMFLOAT4X4 GetViewProjMatrix() const;


		//Mutators
		RENDERER_API inline void SetOpaqueShaderEffects(CRenderSet* _newShaderEffects);
        RENDERER_API inline void SetTransparentRenderables( CRenderSet* _newTransparentRenderables );
        RENDERER_API inline void SetParticles( CRenderSet* _newParticles );
		RENDERER_API inline void SetViewMatrix(DirectX::XMFLOAT4X4& _viewMatrix) { m_d3dViewMatrix = _viewMatrix; }
		RENDERER_API inline void SetViewMatrix(DirectX::XMMATRIX& _viewMatrix) { DirectX::XMStoreFloat4x4(&m_d3dViewMatrix, _viewMatrix); }
		RENDERER_API inline void SetProjMatrix(DirectX::XMFLOAT4X4& _projMatrix) { m_d3dProjMatrix = _projMatrix; }
		RENDERER_API inline void SetProjMatrix(DirectX::XMMATRIX& _projMatrix) { DirectX::XMStoreFloat4x4(&m_d3dProjMatrix, _projMatrix); }

		RENDERER_API inline void SetCameraShakeAmount(float _in) {m_PerCameraData.gCameraShake.x = _in;}
		RENDERER_API inline void SetCameraShakeDuration(float _in) { m_PerCameraData.gCameraShake.y = _in; }

		RENDERER_API CView();
		RENDERER_API ~CView();
		RENDERER_API virtual void Begin(IRenderNode* pCurrentView) final;
		RENDERER_API virtual void End(IRenderNode* pCurrentView) final;

		//Lights
		RENDERER_API  bool AddLight(Light in);
		RENDERER_API  void RemoveLight(int index);
		

	private:
		void SetPerViewCBuffer();
		void SetLightStructedBuffer();
		void CreateLightStructedBuffer();
	};
}
