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
#include "..\Inc\InputLayoutManager.h"
#include "..\Inc\RendererController.h"


namespace Renderer
{
	//CInputLayoutManager* CInputLayoutManager::instancePtr = nullptr;
	ID3D11InputLayout*	 CInputLayoutManager::inputLayouts[eVetex_MAX] = {nullptr};

	CInputLayoutManager& CInputLayoutManager::GetRef()
	{
        static CInputLayoutManager instancePtr;
		return instancePtr;
	}

	void CInputLayoutManager::DeleteInstance()
	{
		for (size_t i = 0; i < eVetex_MAX; i++)
		{
			SAFE_RELEASE(inputLayouts[i]);
		}
	}

	CInputLayoutManager::CInputLayoutManager()
	{

	}

    CInputLayoutManager::~CInputLayoutManager() {
        DeleteInstance();
    }

    void CInputLayoutManager::Initilize()
	{
		for (auto& eachInputLayout : inputLayouts)
		{
			eachInputLayout = nullptr;
		}
		auto theDevicePtr = CRendererController::m_deviceResources->GetD3DDevice();


		//eVertex_PosNorUVTan
		std::ifstream fin;
		fin.open("CSO\\VERIN_POS.cso", std::ios_base::binary);
		fin.seekg(0, std::ios_base::end);
		UINT VSbyteCodeSize = (UINT)fin.tellg();
		char* VSbyteCode = new char[VSbyteCodeSize];
		fin.seekg(0, std::ios_base::beg);
		fin.read(VSbyteCode, VSbyteCodeSize);
		const UINT Vertex_POS_numofElements = 1;
		D3D11_INPUT_ELEMENT_DESC vertexPOSdesc[Vertex_POS_numofElements] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
		};
		theDevicePtr->CreateInputLayout(vertexPOSdesc, Vertex_POS_numofElements, VSbyteCode,
			VSbyteCodeSize, &inputLayouts[eVertex_POS]);
		fin.close();
		delete[] VSbyteCode;


		//eVertex_PosNorUVTan
		fin.open("CSO\\VERIN_POSNORUVTAN.cso", std::ios_base::binary);
		fin.seekg(0, std::ios_base::end);
		VSbyteCodeSize = (UINT)fin.tellg();
		VSbyteCode = new char[VSbyteCodeSize];
		fin.seekg(0, std::ios_base::beg);
		fin.read(VSbyteCode, VSbyteCodeSize);
		const UINT Vertex_POSNORDIFF_numofElements = 4;
		D3D11_INPUT_ELEMENT_DESC vertexPOSNORDIFFdesc[Vertex_POSNORDIFF_numofElements] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
		};
		theDevicePtr->CreateInputLayout(vertexPOSNORDIFFdesc, Vertex_POSNORDIFF_numofElements, VSbyteCode,
			VSbyteCodeSize, &inputLayouts[eVertex_POSNORUVTAN]);
		fin.close();
		delete[] VSbyteCode;


        //eVertex_POSUV0UV1
        //fin.open( "CSO\\VERIN_POSUVBOUNDY.cso", std::ios_base::binary );
        //fin.seekg( 0, std::ios_base::end );
        //VSbyteCodeSize = (UINT)fin.tellg();
        //VSbyteCode = new char[VSbyteCodeSize];
        //fin.seekg( 0, std::ios_base::beg );
        //fin.read( VSbyteCode, VSbyteCodeSize );
        //const UINT Vertex_POSUVBOUNDY_numofElements = 3;
        //D3D11_INPUT_ELEMENT_DESC vertexPOSUVBOUNDYdesc[Vertex_POSUVBOUNDY_numofElements] =
        //{
        //    { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,	0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        //    { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,		0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        //    { "TEXCOORD", 1, DXGI_FORMAT_R32G32_FLOAT,		0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
        //};
        //theDevicePtr->CreateInputLayout( vertexPOSUVBOUNDYdesc, Vertex_POSUVBOUNDY_numofElements, VSbyteCode,
        //    VSbyteCodeSize, &inputLayouts[eVertex_POSUV0UV1] );
        //fin.close();
        //delete[] VSbyteCode;

        //eVertex_Particle
        fin.open( "CSO\\VS_POSVELSIZAGETYP_SO.cso", std::ios_base::binary );
        fin.seekg( 0, std::ios_base::end );
        VSbyteCodeSize = (UINT)fin.tellg();
        VSbyteCode = new char[VSbyteCodeSize];
        fin.seekg( 0, std::ios_base::beg );
        fin.read( VSbyteCode, VSbyteCodeSize );
        const UINT Vertex_POSVELSIZAGETYP_numofElements = 5;
        D3D11_INPUT_ELEMENT_DESC vertexPOSVELSIZAGETYPdesc[Vertex_POSVELSIZAGETYP_numofElements] =
        {
            { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
            { "VELOCITY", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
            { "SIZE",     0, DXGI_FORMAT_R32G32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
            { "AGE",      0, DXGI_FORMAT_R32_FLOAT,       0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
            { "TYPE",     0, DXGI_FORMAT_R32_UINT,        0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        };
        theDevicePtr->CreateInputLayout( vertexPOSVELSIZAGETYPdesc, Vertex_POSVELSIZAGETYP_numofElements, VSbyteCode,
            VSbyteCodeSize, &inputLayouts[ePTVertex_POSVELSIZAGETYP] );
        fin.close();
        delete[] VSbyteCode;

	}


}