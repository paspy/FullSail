#pragma once
#include <stdint.h>
#include <unordered_map>
#include <vector>
#include <string>
#include <DirectXMath.h>
#include <DDSTextureLoader.h>

struct Mesh
{
	std::vector<DirectX::XMFLOAT3> pos;
	std::vector<DirectX::XMFLOAT3> nor;
	std::vector<DirectX::XMFLOAT3> uvs;
	std::vector<DirectX::XMFLOAT3> tangents;
	std::vector<uint32_t> indices;
};
class CAssetManager
{
private:

	static CAssetManager* m_pInstance;
	ID3D11Device* m_pDevice;
	void LoadMesh(std::string& _fileName);
	void LoadTexture(std::string& _fileName);
	CAssetManager();
	CAssetManager(const CAssetManager&) = delete;
	CAssetManager& operator=(const CAssetManager&) = delete;
public:
	~CAssetManager();
	void	Init();
	void	Shutdown();
	static CAssetManager& GetRef();
	void SetDevicePtr(ID3D11Device* devicePtr) { m_pDevice = devicePtr; }
	std::unordered_map<std::string, Mesh*> m_Meshes;
	std::unordered_map<std::string, ID3D11ShaderResourceView*> m_Textures;
};

