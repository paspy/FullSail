/**************************************************************************************/
/*
This is an abstract class for all 3D Entities
to inherit off of

Author : David Ashbourne(Full Sail)
Contributor(s) : Jason Bloomfield (Add name here if you like)
*/
/**************************************************************************************/
#pragma once
#include "IEntity.h"
#include <string>
#include <memory>


class Collider;


namespace Renderer
{
	class CRenderable;
	class CMaterial;
    class CParticle;
};



class Entity3D : public IEntity
{
public:
	enum EntityType { UNKNOWN = -1, SHIP = 0, SHIP_PART, CRATE, AMMO, WEATHER, LAND, PORT };

	Entity3D(const XMFLOAT4X4& _newTransfrom);
	Entity3D(const XMFLOAT4X4& _newTransfrom, const char* MeshName, const char* TextureName);
	Entity3D(const XMFLOAT4X4& _newTransfrom, const char* MeshName);

	//****************Trilogy*****************//
	Entity3D& operator=(const Entity3D& other);
	Entity3D(const Entity3D& _cpy);

	virtual ~Entity3D();


	//**************Functions****************//
	// This Function will assigned to the collider object
	// state what should be done once there is a collision detected
	virtual void OnCollision(Collider& _other);
	virtual void Update(float dt);

	void SetupRenderData(const char* _materialFile, unsigned int _RenderBucketIndex, unsigned int _inputLayoutIndex,
		const char* _diffuse, const char* _normal = nullptr, const char* _height = nullptr, const char* _specular = nullptr,
		const char* _skybox = nullptr, const char* _emissive = nullptr, const char* _ao = nullptr);

	void CreateWaterRenderData();

	//**************Accessors****************//
	virtual const EntityType GetType()                  { return LAND; }
	const char* GetMeshName()                           { return m_sMeshName.c_str(); }
	const char* GetTextureListName()                    { return m_sTextureListName.c_str(); }
	//const Collider* GetCollider()                       { return m_pCollider; }
	Collider* GetCollider() const						{ return m_pCollider; }
	Renderer::CRenderable* GetRenderInfo()              { return m_pRenderInfo.get(); }

    std::vector<Renderer::CParticle*>& GetParticleSet()   { return m_vecParticleSet; }
    void PlayParticleSet();
    void StopParticleSet();
    void ResetParticleSet();

	//**************Mutators****************//
	void SetMeshData(const char* MeshName, const char* TextureName = nullptr);
	void ChangeRenderMesh(const char* _MeshName, unsigned int _RenderBucketIndex, unsigned int _InputLayoutIndex);
	void ChangeRenderTexture(const char* _diffuse, const char* _normal = nullptr, const char* _height = nullptr, const char* _specular = nullptr, const char* _emissive = nullptr, const char* _ao = nullptr);
	void SetCollider(Collider* newCollider);

protected:
	//***************Data Members****************//
    Collider*                               m_pCollider;
	XMFLOAT4X4								m_renderMatrix;

    //MainData
    std::string                             m_sMeshName;
    std::string                             m_sTextureListName;

	//rendering Info
    std::unique_ptr<Renderer::CRenderable>  m_pRenderInfo;
    Renderer::CMaterial*	                m_pRenderMaterial;

    // Particle Set
    std::vector<Renderer::CParticle*>       m_vecParticleSet;

    //  function pointer
    std::function<void( float )>            m_funcUpdate;

    float				                    VFXCountDown;
	static std::unordered_map<std::string, std::unique_ptr<Renderer::CMaterial> > m_MaterialMasterSet;
private:
	//*******************Utility********************//
	const char* ValidateRenderExtension(const char* _name, bool TextureOrMesh);
	unsigned int FindStringEnding(const char* _name);

};

