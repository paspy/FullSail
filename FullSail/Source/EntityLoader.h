#pragma once

class Ship;
class PlayerShip;
class AIShip;
class BossShip;
class Boid;
class Cannon;
class Entity3D;
class IController;
class AiController;
class SideCannonHandler;
class Crate;
class Port;
class Camera;

namespace DirectX
{
	struct XMFLOAT4X4;
}

namespace tinyxml2
{
	class XMLElement;
}

//Cannot use zero since xml returns 0 as an error
enum XMLobjects
{
	SHIP = 1, LAND = 2, CRATE = 4, PORT_OBJ = 5,
};

class EntityLoader
{
	//Helper functions
	static Port*		ReadPort(tinyxml2::XMLElement* RootElement);
	static Entity3D*	ReadShip(tinyxml2::XMLElement* RootElement);
	static Crate*		ReadCrate(tinyxml2::XMLElement* RootElement);
	static Entity3D*	ReadIsland(tinyxml2::XMLElement* RootElement);
	static void			ReadInHull(tinyxml2::XMLElement* RootElement, Ship** newShip);
	static void			ReadInMast(tinyxml2::XMLElement* RootElement, Ship** newShip);
	static void			ReadInShipResource(tinyxml2::XMLElement* RootElement, Ship** newShip);

	static void			ReadInTransform(tinyxml2::XMLElement* RootElement, DirectX::XMFLOAT4X4& newTransform);
	static void			ReadInTexture(tinyxml2::XMLElement* RootElement, Entity3D** newObject, unsigned int numTextures);
	static void			ReadInController(tinyxml2::XMLElement * RootElement, Ship** newShip, IController** newController);
	static void			ReadInCannon(tinyxml2::XMLElement* RootElement, DirectX::XMFLOAT4X4& newTransform, Ship** newShip, Cannon** newCannon);
	static void			ReadInCannonHandler(tinyxml2::XMLElement* RootElement, DirectX::XMFLOAT4X4& newTransform, Ship** newShip, SideCannonHandler** newMast);
	static void			ReadInAllIslandData(tinyxml2::XMLElement* RootElement);
	static void			ReadInAllShipData(tinyxml2::XMLElement* RootElement);
public:
	EntityLoader();

	//For seperation of what is in the 

	/*******************************Utility*********************************/
	static void			ReadCameraOffset(Camera*);
	static void			XML_LoadScene(const char* _FileName);
	static Entity3D*	XML_LoadObject(const char* _FileName);
	static Boid*		ReadInBoidInfo(std::string BoidName, Entity3D ** newObject);
	static void			ReadInRenderInfo(Entity3D** newObject);
	static void			XML_ModifyObject(const char* _FileName, Entity3D* object);
	static void			ReadInControllerInfo(const char * _FileName, Ship ** newShip, AiController ** newController);


	//Destructor
	~EntityLoader();
};

