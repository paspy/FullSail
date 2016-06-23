#include "AssetManager.h"
#include <iostream>
#include <fstream>
using namespace std;
using namespace DirectX;


const string g_MeshDirectory("../../FullSail/Resources/Model/Mesh/");
const string g_MeshTextureDirectory("../../FullSail/Resources/Model/Texture/");
const string g_GUITextureDirectory("../../FullSail/Resources/GUI/");
static bool g_Instanced = false;
CAssetManager* CAssetManager::m_pInstance = nullptr;

CAssetManager& CAssetManager::GetRef()
{
	if (!m_pInstance)
	{
		m_pInstance = new CAssetManager;
	}
	return *m_pInstance;
}

CAssetManager::CAssetManager()
{

	assert(!g_Instanced && "Only one instance of AssetManager is allowed.");


	std::vector<std::string> filenames;
	ifstream fileListIn(g_MeshDirectory + "Meshes.txt");
	if (fileListIn.is_open())
	{
		while (!fileListIn.eof())
		{
			string filename;
			fileListIn >> filename;
			if (filename.size() > 0)
			{
				filenames.push_back(filename);
			}
		}
		fileListIn.close();
	}
	for (auto& eachMashName : filenames)
	{
		LoadMesh(eachMashName);
	}


	filenames.clear();
	fileListIn.open(g_MeshTextureDirectory + "Textures.txt");
	if (fileListIn.is_open())
	{
		while (!fileListIn.eof())
		{
			string filename;
			fileListIn >> filename;
			if (filename.size() > 0)
			{
				filenames.push_back(filename);
			}
		}
		fileListIn.close();
	}
	for (auto& eachTextureName : filenames)
	{
		LoadTexture(eachTextureName);
	}


	g_Instanced = true;
}


CAssetManager::~CAssetManager()
{
	for (auto& eachMesh : m_Meshes)
	{
		delete eachMesh.second;
		eachMesh.second = nullptr;
	}

	for (auto& eachTexture : m_Textures)
	{
		if (eachTexture.second)
		{
			eachTexture.second->Release();
		}
	}
	g_Instanced = false;
}


void CAssetManager::LoadMesh(string& _fileName)
{
	ifstream bin(g_MeshDirectory + _fileName, ios_base::binary);
	m_Meshes[_fileName] = new Mesh;
	if (bin.is_open())
	{
		char header[8];
		bin.read(header, 8);
		if (strcmp(header, "FULLSAIL"))
		{
			uint8_t modelNameLength = 0;
			bin.read(reinterpret_cast<char*>(&modelNameLength), sizeof modelNameLength);


			string modelName;
			modelName.resize(modelNameLength);
			bin.read(&modelName[0], modelNameLength * sizeof(char));


			uint8_t numOfMeshes = 0;
			bin.read(reinterpret_cast<char*>(&numOfMeshes), sizeof numOfMeshes);



			for (size_t eachMesh = 0; eachMesh < numOfMeshes; eachMesh++)
			{
				uint32_t numOfVertices = 0;
				bin.read(reinterpret_cast<char*>(&numOfVertices), sizeof numOfVertices);


				m_Meshes[_fileName]->pos.resize(numOfVertices);
				m_Meshes[_fileName]->nor.resize(numOfVertices);
				m_Meshes[_fileName]->uvs.resize(numOfVertices);
				m_Meshes[_fileName]->tangents.resize(numOfVertices);

				bin.read(reinterpret_cast<char*>(m_Meshes[_fileName]->pos.data()), sizeof(XMFLOAT3) *  numOfVertices);
				bin.read(reinterpret_cast<char*>(m_Meshes[_fileName]->nor.data()), sizeof(XMFLOAT3) *  numOfVertices);
				bin.read(reinterpret_cast<char*>(m_Meshes[_fileName]->uvs.data()), sizeof(XMFLOAT3) *  numOfVertices);
				bin.read(reinterpret_cast<char*>(m_Meshes[_fileName]->tangents.data()), sizeof(XMFLOAT3) *  numOfVertices);





				uint32_t numOfIndices = 0;
				bin.read(reinterpret_cast<char*>(&numOfIndices), sizeof numOfIndices);

				m_Meshes[_fileName]->indices.resize(numOfIndices);
				bin.read(reinterpret_cast<char*>(m_Meshes[_fileName]->indices.data()), sizeof(uint32_t) *  numOfIndices);




			}
		}
	}
}

void CAssetManager::LoadTexture(string& _fileName)
{
	auto fileName = g_MeshTextureDirectory + _fileName.c_str();
	wstring wfileName(fileName.begin(), fileName.end());
	CreateDDSTextureFromFile(m_pDevice, wfileName.c_str(), nullptr, &m_Textures[_fileName]);
}