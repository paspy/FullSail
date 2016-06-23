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
#include "..\Inc\Material.h"
#include "..\Inc\RenderSet.h"
#include "..\Inc\View.h"
#include "..\Inc\ShaderEffect.h"
#include "..\Inc\RendererController.h"

const std::string g_MaterialDirectory("../../FullSail/Resources/Model/Material/");

namespace Renderer
{



	void CMaterial::SaveMaterialToXML(const char* _filename)
	{
		tinyxml2::XMLDocument xmlDoc;

		tinyxml2::XMLNode * pMaterial = xmlDoc.NewElement("Material");
		xmlDoc.InsertFirstChild(pMaterial);

		//GlobalAmbient
		tinyxml2::XMLElement * pGlobalAmbient = xmlDoc.NewElement("GlobalAmbient");
		pMaterial->InsertEndChild(pGlobalAmbient);
		pGlobalAmbient->SetAttribute("x", m_MaterialData.GlobalAmbient.x);
		pGlobalAmbient->SetAttribute("y", m_MaterialData.GlobalAmbient.y);
		pGlobalAmbient->SetAttribute("z", m_MaterialData.GlobalAmbient.z);
		pGlobalAmbient->SetAttribute("w", m_MaterialData.GlobalAmbient.w);

		//AmbientColor
		tinyxml2::XMLElement * pAmbientColor = xmlDoc.NewElement("AmbientColor");
		pMaterial->InsertEndChild(pAmbientColor);
		pAmbientColor->SetAttribute("x", m_MaterialData.AmbientColor.x);
		pAmbientColor->SetAttribute("y", m_MaterialData.AmbientColor.y);
		pAmbientColor->SetAttribute("z", m_MaterialData.AmbientColor.z);
		pAmbientColor->SetAttribute("w", m_MaterialData.AmbientColor.w);


		//EmissiveColor
		tinyxml2::XMLElement * pEmissiveColor = xmlDoc.NewElement("EmissiveColor");
		pMaterial->InsertEndChild(pEmissiveColor);
		pEmissiveColor->SetAttribute("x", m_MaterialData.EmissiveColor.x);
		pEmissiveColor->SetAttribute("y", m_MaterialData.EmissiveColor.y);
		pEmissiveColor->SetAttribute("z", m_MaterialData.EmissiveColor.z);
		pEmissiveColor->SetAttribute("w", m_MaterialData.EmissiveColor.w);

		//DiffuseColor
		tinyxml2::XMLElement * pDiffuseColor = xmlDoc.NewElement("DiffuseColor");
		pMaterial->InsertEndChild(pDiffuseColor);
		pDiffuseColor->SetAttribute("x", m_MaterialData.DiffuseColor.x);
		pDiffuseColor->SetAttribute("y", m_MaterialData.DiffuseColor.y);
		pDiffuseColor->SetAttribute("z", m_MaterialData.DiffuseColor.z);
		pDiffuseColor->SetAttribute("w", m_MaterialData.DiffuseColor.w);

		//SpecularColor
		tinyxml2::XMLElement * pSpecularColor = xmlDoc.NewElement("SpecularColor");
		pMaterial->InsertEndChild(pSpecularColor);
		pSpecularColor->SetAttribute("x", m_MaterialData.SpecularColor.x);
		pSpecularColor->SetAttribute("y", m_MaterialData.SpecularColor.y);
		pSpecularColor->SetAttribute("z", m_MaterialData.SpecularColor.z);
		pSpecularColor->SetAttribute("w", m_MaterialData.SpecularColor.w);

		//SpecularColor
		tinyxml2::XMLElement * pReflectance = xmlDoc.NewElement("Reflectance");
		pMaterial->InsertEndChild(pReflectance);
		pReflectance->SetAttribute("x", m_MaterialData.Reflectance.x);
		pReflectance->SetAttribute("y", m_MaterialData.Reflectance.y);
		pReflectance->SetAttribute("z", m_MaterialData.Reflectance.z);
		pReflectance->SetAttribute("w", m_MaterialData.Reflectance.w);

		//Opacity
		tinyxml2::XMLElement * pOpacity = xmlDoc.NewElement("Opacity");
		pMaterial->InsertEndChild(pOpacity);
		pOpacity->SetAttribute("value", m_MaterialData.Opacity);

		//SpecularPower
		tinyxml2::XMLElement * pSpecularPower = xmlDoc.NewElement("SpecularPower");
		pMaterial->InsertEndChild(pSpecularPower);
		pSpecularPower->SetAttribute("value", m_MaterialData.SpecularPower);

		//IndexOfRefraction
		tinyxml2::XMLElement * pIndexOfRefraction = xmlDoc.NewElement("IndexOfRefraction");
		pMaterial->InsertEndChild(pIndexOfRefraction);
		pIndexOfRefraction->SetAttribute("value", m_MaterialData.IndexOfRefraction);

		////HasAmbientTexture
		//tinyxml2::XMLElement * pHasAmbientTexture = xmlDoc.NewElement("HasAmbientTexture");
		//pMaterial->InsertEndChild(pHasAmbientTexture);
		//pHasAmbientTexture->SetAttribute("value", m_MaterialData.HasAmbientTexture);

		////HasEmissiveTexture
		//tinyxml2::XMLElement * pHasEmissiveTexture = xmlDoc.NewElement("HasEmissiveTexture");
		//pMaterial->InsertEndChild(pHasEmissiveTexture);
		//pHasEmissiveTexture->SetAttribute("value", m_MaterialData.HasEmissiveTexture);

		////HasDiffuseTexture
		//tinyxml2::XMLElement * pHasDiffuseTexture = xmlDoc.NewElement("HasDiffuseTexture");
		//pMaterial->InsertEndChild(pHasDiffuseTexture);
		//pHasDiffuseTexture->SetAttribute("value", m_MaterialData.HasDiffuseTexture);

		////HasSpecularTexture
		//tinyxml2::XMLElement * pHasSpecularTexture = xmlDoc.NewElement("HasSpecularTexture");
		//pMaterial->InsertEndChild(pHasSpecularTexture);
		//pHasSpecularTexture->SetAttribute("value", m_MaterialData.HasSpecularTexture);

		////HasAOTexture
		//tinyxml2::XMLElement * pHasAOTexture = xmlDoc.NewElement("HasAOTexture");
		//pMaterial->InsertEndChild(pHasAOTexture);
		//pHasAOTexture->SetAttribute("value", m_MaterialData.HasAOTexture);

		////HasNormalTexture
		//tinyxml2::XMLElement * pHasNormalTexture = xmlDoc.NewElement("HasNormalTexture");
		//pMaterial->InsertEndChild(pHasNormalTexture);
		//pHasNormalTexture->SetAttribute("value", m_MaterialData.HasNormalTexture);

		////HasHeightTexture
		//tinyxml2::XMLElement * pHasHeightTexture = xmlDoc.NewElement("HasHeightTexture");
		//pMaterial->InsertEndChild(pHasHeightTexture);
		//pHasHeightTexture->SetAttribute("value", m_MaterialData.HasHeightTexture);

		////HasOpacityTexture
		//tinyxml2::XMLElement * pHasOpacityTexture = xmlDoc.NewElement("HasOpacityTexture");
		//pMaterial->InsertEndChild(pHasOpacityTexture);
		//pHasOpacityTexture->SetAttribute("value", m_MaterialData.HasOpacityTexture);

		//BumpIntensity
		tinyxml2::XMLElement * pBumpIntensity = xmlDoc.NewElement("BumpIntensity");
		pMaterial->InsertEndChild(pBumpIntensity);
		pBumpIntensity->SetAttribute("value", m_MaterialData.BumpIntensity);

		//SpecularScale
		tinyxml2::XMLElement * pSpecularScale = xmlDoc.NewElement("SpecularScale");
		pMaterial->InsertEndChild(pSpecularScale);
		pSpecularScale->SetAttribute("value", m_MaterialData.SpecularScale);

		//AlphaThreshold
		tinyxml2::XMLElement * pAlphaThreshold = xmlDoc.NewElement("AlphaThreshold");
		pMaterial->InsertEndChild(pAlphaThreshold);
		pAlphaThreshold->SetAttribute("value", m_MaterialData.AlphaThreshold);

		xmlDoc.SaveFile(_filename);
	}

	CMaterial::CMaterial(const char* _filename) : IRenderNode(),m_Renderables(new CRenderSet), m_PerMaterialCBuffer(nullptr), m_Name(_filename)
	{
		for (unsigned int i = 0; i < CMaterialTexture::MAP_COUNT; i++)
		{
			m_MaterialTextures.m_TextureMap[i] = nullptr;
		}
		LoadMaterialData(_filename);
	}


	CMaterial::~CMaterial()
	{
		SAFE_RELEASE(m_PerMaterialCBuffer);
	}

	void 	CMaterial::LoadMaterialData(const char* _filename)
	{
		ZeroMemory(&m_MaterialData,sizeof(m_MaterialData));



		if (_filename)
		{
			tinyxml2::XMLDocument xmlDoc;
			tinyxml2::XMLError eResult = xmlDoc.LoadFile(std::string(g_MaterialDirectory + _filename).c_str());
			if (tinyxml2::XMLError::XML_SUCCESS == eResult)
			{
			
				tinyxml2::XMLNode * pRoot = xmlDoc.FirstChild();
				tinyxml2::XMLElement * pNext = pRoot->FirstChildElement();
				pNext->QueryFloatAttribute("x", &m_MaterialData.GlobalAmbient.x);
				pNext->QueryFloatAttribute("y", &m_MaterialData.GlobalAmbient.y);
				pNext->QueryFloatAttribute("z", &m_MaterialData.GlobalAmbient.z);
				pNext->QueryFloatAttribute("w", &m_MaterialData.GlobalAmbient.w);

				pNext = pNext->NextSiblingElement();
				pNext->QueryFloatAttribute("x", &m_MaterialData.AmbientColor.x);
				pNext->QueryFloatAttribute("y", &m_MaterialData.AmbientColor.y);
				pNext->QueryFloatAttribute("z", &m_MaterialData.AmbientColor.z);
				pNext->QueryFloatAttribute("w", &m_MaterialData.AmbientColor.w);

				pNext = pNext->NextSiblingElement();
				pNext->QueryFloatAttribute("x", &m_MaterialData.EmissiveColor.x);
				pNext->QueryFloatAttribute("y", &m_MaterialData.EmissiveColor.y);
				pNext->QueryFloatAttribute("z", &m_MaterialData.EmissiveColor.z);
				pNext->QueryFloatAttribute("w", &m_MaterialData.EmissiveColor.w);

				pNext = pNext->NextSiblingElement();
				pNext->QueryFloatAttribute("x", &m_MaterialData.DiffuseColor.x);
				pNext->QueryFloatAttribute("y", &m_MaterialData.DiffuseColor.y);
				pNext->QueryFloatAttribute("z", &m_MaterialData.DiffuseColor.z);
				pNext->QueryFloatAttribute("w", &m_MaterialData.DiffuseColor.w);

				pNext = pNext->NextSiblingElement();
				pNext->QueryFloatAttribute("x", &m_MaterialData.SpecularColor.x);
				pNext->QueryFloatAttribute("y", &m_MaterialData.SpecularColor.y);
				pNext->QueryFloatAttribute("z", &m_MaterialData.SpecularColor.z);
				pNext->QueryFloatAttribute("w", &m_MaterialData.SpecularColor.w);

				pNext = pNext->NextSiblingElement();
				pNext->QueryFloatAttribute("x", &m_MaterialData.Reflectance.x);
				pNext->QueryFloatAttribute("y", &m_MaterialData.Reflectance.y);
				pNext->QueryFloatAttribute("z", &m_MaterialData.Reflectance.z);
				pNext->QueryFloatAttribute("w", &m_MaterialData.Reflectance.w);

				pNext = pNext->NextSiblingElement();
				pNext->QueryFloatAttribute("value", &m_MaterialData.Opacity);

				pNext = pNext->NextSiblingElement();
				pNext->QueryFloatAttribute("value", &m_MaterialData.SpecularPower);

				pNext = pNext->NextSiblingElement();
				pNext->QueryFloatAttribute("value", &m_MaterialData.IndexOfRefraction);

				pNext = pNext->NextSiblingElement();
				pNext->QueryFloatAttribute("value", &m_MaterialData.BumpIntensity);

				pNext = pNext->NextSiblingElement();
				pNext->QueryFloatAttribute("value", &m_MaterialData.SpecularScale);

				pNext = pNext->NextSiblingElement();
				pNext->QueryFloatAttribute("value", &m_MaterialData.AlphaThreshold);
			}
			

		}

		D3D11_BUFFER_DESC desc = { 0 };

		desc.ByteWidth = sizeof(m_MaterialData);
		desc.Usage = D3D11_USAGE_DYNAMIC;
		desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

		CRendererController::m_deviceResources->GetD3DDevice()->CreateBuffer(&desc, nullptr, &m_PerMaterialCBuffer);

		std::string debugName = { "PerMaterial: " };
	/*	if (nullptr != m_Name)
		{
			debugName.append(m_Name);
		}*/
		SetD3DName(m_PerMaterialCBuffer, debugName.c_str());

	}

	void CMaterial::SetPerMaterialCBuffer()
	{

		auto d3dDeviceContext = CRendererController::m_deviceResources->GetD3DDeviceContext();
		D3D11_MAPPED_SUBRESOURCE mappedResource;
		AssertIfFailed(
			d3dDeviceContext->Map(m_PerMaterialCBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource)
		);
		memcpy_s(mappedResource.pData, sizeof(m_MaterialData), &m_MaterialData, sizeof(m_MaterialData));
		//*(cbPerMaterial*)mappedResource.pData = m_MaterialData;
		d3dDeviceContext->Unmap(m_PerMaterialCBuffer, 0);

		d3dDeviceContext->PSSetConstantBuffers(m_MaterialData.REGISTER_SLOT, 1, &m_PerMaterialCBuffer);
	}

	/*virtual*/ void CMaterial::Begin(IRenderNode* pCurrentView) /*final*/
	{
		CView& view = (CView&)(*pCurrentView);
		view.m_CurrentState = MATERIAL_BEGIN;
		view.m_CurrentMaterial = this;
		auto d3dDeviceContext = CRendererController::m_deviceResources->GetD3DDeviceContext();
		ID3D11ShaderResourceView* SRVs[CMaterialTexture::MAP_COUNT];
		for (unsigned int i = 0; i < CMaterialTexture::MAP_COUNT; i++)
		{
			SRVs[i] = m_MaterialTextures.m_TextureMap[i];
		}
		d3dDeviceContext->VSSetShaderResources(CMaterialTexture::HEIGHT, 1, &SRVs[CMaterialTexture::HEIGHT]);
		d3dDeviceContext->PSSetShaderResources(0, CMaterialTexture::MAP_COUNT, SRVs);

		SetPerMaterialCBuffer();
		//TODO: Remove
		d3dDeviceContext->CSSetShaderResources(CMaterialTexture::HEIGHT, 1, &SRVs[CMaterialTexture::HEIGHT]);
	}


	/*virtual*/ void CMaterial::End(IRenderNode* pCurrentView) /*final*/
	{
		auto d3dDeviceContext = CRendererController::m_deviceResources->GetD3DDeviceContext();
		CView& view = (CView&)(*pCurrentView);
		view.m_CurrentState = MATERIAL_END;

		ID3D11Buffer* unbindCB = { nullptr };
		d3dDeviceContext->PSSetConstantBuffers(m_MaterialData.REGISTER_SLOT, 1, &unbindCB);

		ID3D11ShaderResourceView* SRVs[CMaterialTexture::MAP_COUNT] = { nullptr };
		d3dDeviceContext->VSSetShaderResources(0, CMaterialTexture::MAP_COUNT, SRVs);
		d3dDeviceContext->PSSetShaderResources(0, CMaterialTexture::MAP_COUNT, SRVs);
		d3dDeviceContext->CSSetShaderResources(0, CMaterialTexture::MAP_COUNT, SRVs);
		view.m_CurrentMaterial = nullptr;
	}

	void CMaterial::AddMap(unsigned int _mapType, ID3D11ShaderResourceView* _mapView)
	{
		if (!_mapView)
		{
			return;
		}
		m_MaterialTextures.m_TextureMap[_mapType] = _mapView;
		switch (_mapType)
		{
		case CMaterialTexture::DIFFUSE:
			m_MaterialData.HasDiffuseTexture = true;
			break;
		case CMaterialTexture::SPECULAR:
			m_MaterialData.HasSpecularTexture = true;
			break;
		case CMaterialTexture::EMISSIVE:
			m_MaterialData.HasEmissiveTexture = true;
			break;
		case CMaterialTexture::NORMAL:
			m_MaterialData.HasNormalTexture = true;
			break;
		case CMaterialTexture::HEIGHT:
			m_MaterialData.HasHeightTexture = true;
			break;
		case CMaterialTexture::AO:
			m_MaterialData.HasAOTexture = true;
			break;
		case CMaterialTexture::OPACITY:
			m_MaterialData.HasOpacityTexture = true;
			break;
		default:
			break;
		}
	}
}