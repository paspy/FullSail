#include "pch.h"
#include "Game.h"
#include "Entity3D.h"
#include "Colliders.h"
#include <Material.h>
#include <Renderable.h>
#include <Particle.h>
#include <Water.h>
#include "EntityLoader.h"
#define TEXTURE	1
#define MESH	0

std::unordered_map<std::string, std::unique_ptr<Renderer::CMaterial> > Entity3D::m_MaterialMasterSet;

Entity3D::Entity3D(const XMFLOAT4X4& _newTransfrom) : IEntity(_newTransfrom), m_pRenderMaterial(nullptr), m_pRenderInfo(nullptr)
{
	m_sMeshName = "";
	m_sTextureListName = "";
	m_pCollider = nullptr;
	VFXCountDown = 0.0f;

    // attach the particle list to renderer
    CGame::GetApplication()->m_pRenderer->ParticleGroup()[m_sName] = &m_vecParticleSet;
}

Entity3D::Entity3D(const XMFLOAT4X4& _newTransfrom, const char* MeshName, const char* TextureName) : IEntity(_newTransfrom), m_pRenderMaterial(nullptr), m_pRenderInfo(nullptr)
{
	m_sMeshName = MeshName;
	m_sTextureListName = TextureName;
	m_pCollider = nullptr;
	VFXCountDown = 0.0f;
    // attach the particle list to renderer
    CGame::GetApplication()->m_pRenderer->ParticleGroup()[m_sName] = &m_vecParticleSet;
}

Entity3D::Entity3D(const XMFLOAT4X4& _newTransfrom, const char* MeshName) :
	IEntity(_newTransfrom),
	m_pRenderMaterial(nullptr),
	m_pRenderInfo(nullptr),
	m_sMeshName(MeshName)
{
	m_pCollider = nullptr;
	VFXCountDown = 0.0f;
    // attach the particle list to renderer
    CGame::GetApplication()->m_pRenderer->ParticleGroup()[m_sName] = &m_vecParticleSet;
}

//The collider and renderer doesn't copy over
Entity3D & Entity3D::operator=(const Entity3D & other)
{
	if ( this == &other)
	{
		return (*this);
	}

	IEntity::operator=(other);

	 m_sMeshName = other.m_sMeshName;
	 m_sTextureListName = other.m_sTextureListName;
	
	 //Setting a pointer to this
	 Entity3D* pointer = this;

	 //MAkeing a pointer to the pointer so that I manipulate
	 Entity3D** newObject = &pointer;

	 //sets up the renderData
	 EntityLoader::ReadInRenderInfo(newObject);

	 VFXCountDown = other.VFXCountDown;
     CGame::GetApplication()->m_pRenderer->ParticleGroup()[m_sName] = &m_vecParticleSet;


	 return (*this);
}

//The collider and renderer doesn't copy over
Entity3D::Entity3D(const Entity3D & _cpy)
	: IEntity(_cpy)
{
	m_sMeshName			= _cpy.m_sMeshName;
	m_sTextureListName	= _cpy.m_sTextureListName;
	VFXCountDown		= _cpy.VFXCountDown;

	//Setting a pointer to this
	Entity3D* pointer = this;

	//MAkeing a pointer to the pointer so that I manipulate
	Entity3D** newObject = &pointer;

	//sets up the renderData
	EntityLoader::ReadInRenderInfo(newObject);
    CGame::GetApplication()->m_pRenderer->ParticleGroup()[m_sName] = &m_vecParticleSet;

}

Entity3D::~Entity3D()
{

	if (m_pCollider != nullptr)
	{
		delete m_pCollider;
		m_pCollider = nullptr;
	}

    for ( auto it = m_vecParticleSet.begin(); it != m_vecParticleSet.end(); it++ ) {
        if ( *it ) {
            SafeDelete( ( *it ) );
        }
    }

    m_vecParticleSet.clear();
    CGame::GetApplication()->m_pRenderer->ParticleGroup().erase( m_sName );
}

/*
Entity3D::Entity3D(XMFLOAT4X4& _newTransfrom, const char* MeshName, const char* TextureName) : IEntity(_newTransfrom), m_pRenderMaterial(nullptr), m_pRenderInfo(nullptr)
{
	m_sMeshName = MeshName;
	m_sTextureName = TextureName;


	//TODO: Refactor this Ash.
	static auto& assetManager = CAssetManager::GetRef();
	m_pRenderMaterial.reset(new Renderer::CMaterial());
	m_pRenderMaterial->AddMap(0, assetManager.Textures(ValidateRenderExtension(TextureName,TEXTURE)));

	//m_transform.SetLocalMatrix(_newTransfrom);
	m_pRenderInfo.reset(new Renderer::CRenderable(*m_pRenderMaterial,ValidateRenderExtension(MeshName, MESH)));
	//m_pRenderInfo->SetWorldMatrix(m_transform.GetWorldMatrix());
}
*/

void Entity3D::OnCollision(Collider& _other)
{

}

void Entity3D::Update(float dt)
{
	//m_transform.DirtyTransform();
	memcpy(&m_renderMatrix, &m_transform.GetWorldMatrix(), sizeof(m_renderMatrix));

    for ( auto it = m_vecParticleSet.begin(); it != m_vecParticleSet.end(); it++ ) {
        if ( *it ) {
            ( *it )->Update( dt );
        }
    }
}

void Entity3D::PlayParticleSet() {
    for ( auto it = m_vecParticleSet.begin(); it != m_vecParticleSet.end(); it++ ) {
        if ( *it ) {
            ( *it )->Play();
        }
    }
}

void Entity3D::StopParticleSet() {
    for ( auto it = m_vecParticleSet.begin(); it != m_vecParticleSet.end(); it++ ) {
        if ( *it ) {
            ( *it )->Stop();
        }
    }
}

void Entity3D::ResetParticleSet() {
    for ( auto it = m_vecParticleSet.begin(); it != m_vecParticleSet.end(); it++ ) {
        if ( *it ) {
            ( *it )->Reset();
        }
    }
}

void Entity3D::SetMeshData( const char* MeshName, const char* TextureListName )
{
	//safety check
	if (MeshName == nullptr || MeshName[0] == '\0')
	{
		return;
	}

	//assigning
	m_sMeshName = MeshName;

	if (TextureListName == nullptr || TextureListName[0] == '\0')
	{
		return;
	}

	m_sTextureListName = TextureListName;
}

void Entity3D::SetCollider(Collider * newCollider)
{
	if (newCollider == nullptr)
	{
		return;
	}

	//Event needs to call instead of deleting so that the manager that contains the object doesnt crash from a invalid
	//Aka change this 
	//delete m_pCollider;

	m_pCollider = newCollider;
}

//Validates the string if you input it wrong
//false means adds a mesh extension
//true means adds a texture extension
const char* Entity3D::ValidateRenderExtension(const char* _name, bool TextureOrMesh)
{
	//getting the null terminator index
	unsigned int endingIndex = FindStringEnding(_name);
	std::string newString = _name;

	if (TextureOrMesh == TEXTURE)
	{
		//early out check
		//moving index to the spot that the "." should be in the 
		if (_name[endingIndex - 4] == '.')
		{
			return _name;
		}
		//Adding the .dds to the char
		newString += ".dds";
	}

	if (TextureOrMesh == MESH)
	{
		//early out check
		//moving index to the spot that the "." should be in the 
		if (_name[endingIndex - 5] == '.')
		{
			return _name;
		}
		//Adding the .mesh to the char
		newString += ".mesh";
	}

	return newString.c_str();
}

//finding the null terminator
unsigned int Entity3D::FindStringEnding(const char* _name)
{
	unsigned int index = 0;

	//looping to find the ternimator
	for (; _name[index] != '\0'; index++)
	{
		//nothing needs to go here
	}

	//returning the index of the null terminator
	return index;
}

void Entity3D::SetupRenderData(const char* _materialFile, unsigned int _RenderBucketIndex, unsigned int _inputLayoutIndex,
	const char* _diffuse, const char* _normal, const char* _height, const char* _specular,
	const char* _skybox, const char* _emissive, const char* _ao)
{

	static auto& assetManager = CAssetManager::GetRef();
	
	if (m_MaterialMasterSet.count(_materialFile) == 0)
	{
		m_MaterialMasterSet[_materialFile] = std::make_unique<Renderer::CMaterial>(_materialFile);
	}

	m_pRenderMaterial = m_MaterialMasterSet[_materialFile].get();
	m_pRenderMaterial->AddMap(0, assetManager.Textures(_diffuse));
	m_pRenderMaterial->AddMap(1, assetManager.Textures(_specular));
	m_pRenderMaterial->AddMap(2, assetManager.Textures(_emissive));
	m_pRenderMaterial->AddMap(3, assetManager.Textures(_normal));
	m_pRenderMaterial->AddMap(4, assetManager.Textures(_height));
	m_pRenderMaterial->AddMap(5, assetManager.Textures(_skybox));
	m_pRenderMaterial->AddMap(6, assetManager.Textures(_ao));

	m_pRenderInfo.reset();
	m_pRenderInfo = std::make_unique<Renderer::CRenderable>(*m_pRenderMaterial, ValidateRenderExtension(m_sMeshName.c_str(), MESH), _RenderBucketIndex, _inputLayoutIndex);

	m_renderMatrix = m_transform.GetWorldMatrix();
	m_pRenderInfo->SetWorldMatrix(m_renderMatrix);

}

void Entity3D::ChangeRenderTexture(const char* _diffuse, const char* _normal, const char* _height, const char* _specular, const char* _emissive, const char* _ao)
{
	static auto& assetManager = CAssetManager::GetRef();
	m_pRenderMaterial->AddMap(0, assetManager.Textures(_diffuse));
	m_pRenderMaterial->AddMap(1, assetManager.Textures(_specular));
	m_pRenderMaterial->AddMap(2, assetManager.Textures(_emissive));
	m_pRenderMaterial->AddMap(3, assetManager.Textures(_normal));
	m_pRenderMaterial->AddMap(4, assetManager.Textures(_height));
	m_pRenderMaterial->AddMap(6, assetManager.Textures(_ao));
}

void Entity3D::ChangeRenderMesh(const char* _MeshName,unsigned int _RenderBucketIndex, unsigned int _InputLayoutIndex)
{
	m_sMeshName = _MeshName;
	m_pRenderInfo.reset();
	m_pRenderInfo = std::make_unique<Renderer::CRenderable>(*m_pRenderMaterial, ValidateRenderExtension(m_sMeshName.c_str(), MESH), _RenderBucketIndex, _InputLayoutIndex);
	m_pRenderInfo->SetWorldMatrix(m_renderMatrix);
}

void Entity3D::CreateWaterRenderData()
{
	static auto& assetManager = CAssetManager::GetRef();
	if (m_MaterialMasterSet.count("waterMaterial.xml") == 0)
	{
		m_MaterialMasterSet["waterMaterial.xml"] = std::make_unique<Renderer::CMaterial>("waterMaterial.xml");
	}
	m_pRenderMaterial = m_MaterialMasterSet["waterMaterial.xml"].get();
	m_pRenderMaterial->AddMap(4, assetManager.Textures("WaterHeight.dds"));
	m_pRenderMaterial->AddMap(5, assetManager.Textures("SkyboxOcean.dds"));

	m_pRenderInfo = std::make_unique<Renderer::CWaterRenderable>(*m_pRenderMaterial);
	m_pRenderInfo->SetWorldMatrix(m_transform.GetWorldMatrix());
}