/***********************************************
* Filename:
* Date:
* Mod. Date:
* Mod. Initials:
* Author:
* Purpose:
************************************************/
#pragma once



#include "..\stdafx.h"
#include "RenderNode.h"
#include "ConstantBuffer.h"

namespace Renderer
{
	class CRenderSet;
	class CShaderEffect;
	class CView;

	class CMaterial : public IRenderNode
	{
		struct CMaterialTexture
		{
			enum eTextureType : unsigned int
			{
				DIFFUSE = 0, SPECULAR, EMISSIVE, NORMAL, HEIGHT, SKYBOX, AO, AMBIENT, OPACITY, MAP_COUNT
			};
			std::unordered_map<unsigned int, ID3D11ShaderResourceView*> m_TextureMap;
		};

		friend class CView;
	private:
		CMaterialTexture												m_MaterialTextures;
		cbPerMaterial													m_MaterialData;
		ID3D11Buffer*													m_PerMaterialCBuffer;
	public:
		std::unique_ptr<CRenderSet>										m_Renderables;

		 std::string														m_Name;
		//Accessors
		// DIFFUSE = 0, SPECULAR = 1, EMISSIVE = 2, NORMAL = 3, HEIGHT =4,SKYBOX = 5, AO = 6,
		RENDERER_API inline ID3D11ShaderResourceView* GetMapSRV(unsigned int _mapType) { return m_MaterialTextures.m_TextureMap[_mapType]; }
		//Mutators
		// DIFFUSE = 0, SPECULAR = 1, EMISSIVE = 2, NORMAL = 3, HEIGHT =4,SKYBOX = 5, AO = 6,
		RENDERER_API void AddMap(unsigned int _mapType, ID3D11ShaderResourceView* _mapView);

		//TODO: Remake to load from XML file
		RENDERER_API void LoadMaterialData(const char* _filename);
		RENDERER_API void SaveMaterialToXML(const char* _filename);
		RENDERER_API void SetPerMaterialCBuffer();

		RENDERER_API CMaterial(const char* _filename);
		RENDERER_API ~CMaterial();
		virtual void Begin(IRenderNode* pCurrentView) final;
		virtual void End(IRenderNode* pCurrentView) final;

	/*	bool operator==(const CMaterial &other) const;*/
	};
}
