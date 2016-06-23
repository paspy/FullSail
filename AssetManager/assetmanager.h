#ifndef AssetManager_h__
#define AssetManager_h__

#ifdef ASSETMANAGER_EXPORTS
#define ASSETMANAGER_API __declspec(dllexport)
#else
#define ASSETMANAGER_API __declspec(dllimport)
#endif

#include <stdint.h>
#include <unordered_map>
#include <vector>
#include <string>
#include <DirectXMath.h>
#include <DDSTextureLoader.h>

#include "SpritesManager.h"

#define SAFE_RELEASE(com) { int keeper = 0; if(com) { if (0 == (keeper = com->Release() ) ) com = nullptr; } }
struct Mesh
{
	struct Raw
	{
		std::vector<DirectX::XMFLOAT3> pos;
		std::vector<DirectX::XMFLOAT3> nor;
		std::vector<DirectX::XMFLOAT3> uvs;
		std::vector<DirectX::XMFLOAT3> tangents;
		std::vector<uint32_t> indices;
	}RawData;
	
	struct D3D
	{
		ID3D11Buffer*  pPOS_VBuffer;
		ID3D11Buffer*  pPOSNORUVTAN_VBuffer;
		ID3D11Buffer*  pIBuffer;
		unsigned int m_Stride[2];

		~D3D()
		{
			SAFE_RELEASE(pPOS_VBuffer);
			SAFE_RELEASE(pPOSNORUVTAN_VBuffer);
			SAFE_RELEASE(pIBuffer);
		}
	}D3DData;
	
};

struct __declspec(align(16)) Vertex
{
	DirectX::XMFLOAT3 pos;
	DirectX::XMFLOAT3 nor;
	DirectX::XMFLOAT3 uvw;
	DirectX::XMFLOAT3 tangent;
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
	std::unordered_map<std::string, Mesh*> m_Meshes;
	std::unordered_map<std::string, ID3D11ShaderResourceView*> m_Textures;
    // Sprite2D Manager
    G2D::CSpritesManager m_sprite2ds;

public:
	ASSETMANAGER_API ~CAssetManager();
	ASSETMANAGER_API void	Init();
	ASSETMANAGER_API void	Shutdown();
	ASSETMANAGER_API static CAssetManager& GetRef();
	ASSETMANAGER_API void SetDevicePtr(ID3D11Device* devicePtr) { m_pDevice = devicePtr; }
	ASSETMANAGER_API Mesh* Meshes(const char* _fileName) { return m_Meshes[_fileName]; }
	ASSETMANAGER_API ID3D11ShaderResourceView* Textures(const char* _fileName);
    // sprite2d
    ASSETMANAGER_API bool IsFontExist( std::string &name ) { return m_sprite2ds.IsFontExist( name ); }
    ASSETMANAGER_API bool IsSpriteExist( std::string &name ) { return m_sprite2ds.IsSpriteExist( name ); }
    ASSETMANAGER_API G2D::Sprite2DInfo GetSpriteInfo( std::string &name ) { return m_sprite2ds.GetSprite( name ); }
    ASSETMANAGER_API G2D::SpriteFontInfo GetFontInfo( std::string &name ) { return m_sprite2ds.GetFont( name ); }
    ASSETMANAGER_API std::shared_ptr<DirectX::SpriteFont> GetFont( std::string &name ) { return m_sprite2ds.GetFont( name ).font; }
    ASSETMANAGER_API ID3D11ShaderResourceView* GetSprite( __in std::string &name, __out_opt DirectX::SimpleMath::Vector2 &origin );
    ASSETMANAGER_API ID3D11ShaderResourceView* GetSprite( __in std::string &name );
    ASSETMANAGER_API DirectX::SimpleMath::Vector2 GetSpriteSize( __in std::string name );

};
#endif // AssetManager_h__

