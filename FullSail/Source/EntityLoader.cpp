#include "pch.h"
#include "EntityLoader.h"

#include <string>
#include "Log.h"
#include "Ship.h"
#include "PlayerShip.h"
#include "AIShip.h"
#include "BossShip.h"
#include "Port.h"
#include "IBoid.h"
#include "IAmmo.h"
#include "Crate.h"
#include "Island.h"
#include "Camera.h"
#include "Cannon.h"
#include "Entity3D.h"
#include "IController.h"
#include "AiController.h"
#include "PlayerController.h"
#include "SideCannonHandler.h"
#include <Utilities.h>
#define DegreeToRad(x) x * XM_PI/180.0f

using namespace tinyxml2;

EntityLoader::EntityLoader()
{
	delete XML_LoadObject("BossShip");
	delete XML_LoadObject("CargoGuard");
	delete XML_LoadObject("CargoShip");
	delete XML_LoadObject("Crate");
	delete XML_LoadObject("FleetMember");
	delete XML_LoadObject("GoldShip");
	delete XML_LoadObject("MerchantShip");
}

/*static*/ Entity3D * EntityLoader::XML_LoadObject(const char * ObjectName)
{
	//Getting the FilePath
	std::string FilePath = "../../FullSail/Resources/XML/Objects/Object_";
	FilePath += ObjectName;
	FilePath += ".xml";

	tinyxml2::XMLDocument doc;
	//loading the file in to memory
	//Quick safety check
	if (doc.LoadFile(FilePath.c_str()) != XML_NO_ERROR)
	{
		Log("Couldnt find path");
		return nullptr;
	}

	//Root Element
	XMLElement* root = doc.RootElement();
	//Safety check
	if (root == nullptr)
	{
		Log("There is no root");
		return nullptr;
	}

	//Finding the type of entity 
	int XMLtype = root->IntAttribute("Type");

	//Creating an object
	Entity3D* newObject = nullptr;

	switch (XMLtype)
	{
	case SHIP:
		newObject = ReadShip(root);
		break;

	case LAND:
		newObject = ReadIsland(root);
		break;

	case PORT_OBJ:

		newObject = ReadPort(root);
		break;

	case CRATE:
		newObject = ReadCrate(root);
		break;
	}

	doc.Clear();
	return newObject;
}

//I do not delete memory so clean it up prior please
/*static*/ void	EntityLoader::XML_LoadScene(const char* _FileName)
{
	//Creating the an IndentityMatrix and lowering the object
	XMFLOAT4X4 newTransform;
	SetToIdentity(newTransform);


	//Getting the FilePath
	std::string FilePath = "../../FullSail/Resources/XML/Scene/";
	FilePath += _FileName;
	FilePath += ".xml";

	tinyxml2::XMLDocument doc;
	//loading the file in to memory
	//Quick safety check
	if (doc.LoadFile(FilePath.c_str()) != XML_NO_ERROR)
	{
		Log("Couldnt find path");
		return;
	}

	//Root Element
	XMLElement* root = doc.RootElement();
	//Safety check
	if (root == nullptr)
	{
		Log("There is no root");
		return;
	}

	XMLElement* IslandList = root->FirstChildElement("IslandList");
	ReadInAllIslandData(IslandList);

	XMLElement* ShipList = root->FirstChildElement("ShipList");
	ReadInAllShipData(ShipList);
}

/*static*/ void EntityLoader::XML_ModifyObject(const char * ObjectName, Entity3D * object)
{

}

/*static*/ Entity3D * EntityLoader::ReadShip(XMLElement * RootElement)
{
	//Going to the next element in the xml file

	Ship* newShip = nullptr;

	//Creating the an IndentityMatrix and lowering the object
	XMFLOAT4X4 newTransform;
	SetToIdentity(newTransform);
	setPosition(newTransform, XMFLOAT3(0, -0.1f, 0));

	//Reading in RenderInfo
	XMLElement* newElement = RootElement->FirstChildElement();
	const char* textureList = newElement->GetText();


	//Alignment
	bool alignement = false;
	newElement = newElement->NextSiblingElement();
	newElement->QueryBoolAttribute("Alignment", &alignement);


	//Ship type
	int shiptype;
	newElement->QueryIntAttribute("Type", &shiptype);


	//Alignment
	bool boardable = false;
	newElement->QueryBoolAttribute("Boardable", &boardable);


	switch (shiptype)
	{
	case Ship::eShipType::TYPE_PLAYER:
		newShip = newx PlayerShip(newTransform, "", textureList);
		break;

	case Ship::eShipType::TYPE_AI:
		newShip = newx AIShip(newTransform, "", textureList);
		break;

	case Ship::eShipType::TYPE_BOSS:
		newShip = newx BossShip(newTransform, "", textureList);
		break;

	case Ship::eShipType::TYPE_UNKNOWN:
	default:
		newShip = newx Ship(newTransform, "", textureList);
		break;
	}
	newShip->SetAlignment(alignement);
	newShip->SetBoardingStage((boardable == true) ? Ship::eBoardingState::NONE : Ship::eBoardingState::UNBOARDABLE);


	//loading in Textures
	ReadInRenderInfo((Entity3D**)&newShip);


	//Constructing the ship

	//Hull
	XMLElement* ShipParts = newElement->FirstChildElement("Hull");
	EntityLoader::ReadInHull(ShipParts, &newShip);


	//Mast
	ShipParts = ShipParts->NextSiblingElement("Mast");
	EntityLoader::ReadInMast(ShipParts, &newShip);


	//Cannons
	SideCannonHandler* newHandler = nullptr;
	ShipParts->NextSiblingElement("CannonHandler");
	EntityLoader::ReadInCannonHandler(ShipParts->NextSiblingElement("CannonHandler"), newTransform, &newShip, &newHandler);

	newShip->SetWeapon(newHandler);

	//Setting the controller
	IController* control = nullptr;
	ShipParts = ShipParts->NextSiblingElement("Controller");
	EntityLoader::ReadInController(ShipParts, &newShip, &control);
	newShip->SetController(control);

	//Resource
	ShipParts->NextSiblingElement("Resource");
	EntityLoader::ReadInShipResource(ShipParts->NextSiblingElement("Resource"), &newShip);


	return newShip;
}

/*static*/ Entity3D * EntityLoader::ReadIsland(tinyxml2::XMLElement * RootElement)
{
	Island* newLand = nullptr;

	//Creating the an IndentityMatrix and lowering the object
	XMFLOAT4X4 newTransform;
	SetToIdentity(newTransform);
	setPosition(newTransform, XMFLOAT3(0, -0.1f, 0));

	//Reading in RenderInfo
	XMLElement* newElement = RootElement->FirstChildElement();
	const char* textureList = newElement->GetText();

	//Creating creating a base ship to add parts to
	newLand = newx Island(newTransform, textureList);

	//loading in Textures
	ReadInRenderInfo((Entity3D**)&newLand);

	//Setting the Collision mesh
	newElement = newElement->NextSiblingElement();
	newLand->SetCollisionMesh(newElement->GetText());

	//Setting the name of the Island
	newElement = newElement->NextSiblingElement();

	//Creating the port
	newElement = newElement->NextSiblingElement();

	//If there is no port return what you have
	if (newElement == nullptr)
	{
		return newLand;
	}

	//Port* newPort = nullptr;
	//Casting for porting name convention
	//std::wstring PortName = (const wchar_t*)newElement->Attribute("Name");
	std::string ObjectFile = newElement->Attribute("ObjectFile");

	//Getting the transform
	XMFLOAT4X4 PortTransform;
	ReadInTransform(newElement->FirstChildElement(), PortTransform);

	newLand->PortPosition = PortTransform;
	newLand->PortName = ObjectFile.c_str();

	//Firing an event
	//fsEventManager::GetInstance()->FireEvent(fsEvents::LoadObject, &EventArgs2<const char*, XMFLOAT4X4>(ObjectFile.c_str(), PortTransform));


	return newLand;
}

/*static*/ void EntityLoader::ReadInRenderInfo(Entity3D ** newObject)
{
	//Mesh information
	std::string material;
	std::string textureList[7];
	unsigned int TextureSize = 0;

	//Setting the Texture list name for safekeeping
	textureList[0] = (*newObject)->GetTextureListName();

	//Getting the FilePath
	std::string FilePath = "../../FullSail/Resources/XML/Textures/TextureList_";
	FilePath += (*newObject)->GetTextureListName();
	FilePath += ".xml";
	tinyxml2::XMLDocument doc;
	//loading the file in to memory
	//Quick safety check

	if (doc.LoadFile(FilePath.c_str()) != XML_NO_ERROR)
	{
		Log("Couldnt find path");
		return;
	}

	//Root Element
	XMLElement* root = doc.RootElement();

	//Safety check
	if (root == nullptr)
	{
		Log("There is no root");
		return;
	}

	//Getting the next element in the XML
	XMLElement* RootElement = root->FirstChildElement();

	//***********************Mesh**********************************//

	//Setting the mesh data
	std::string meshName = RootElement->Attribute("model");

	//Setting the mesh data
	(*newObject)->SetMeshData(meshName.c_str(), textureList[0].c_str());
	textureList[0] = "";

	//getting the texture size so that I know how long to loop for
	RootElement->QueryUnsignedAttribute("textureSize", &TextureSize);

	//**********************TextureList*****************************//
	RootElement = RootElement->NextSiblingElement();

	//getting the first child
	material = RootElement->Attribute("material");

	//Getting the element inside of the Element
	XMLElement * newElement = RootElement->FirstChildElement("diffuse");


	//Getting the info for the textures
	for (unsigned int i = 0, counter = 0; i < TextureSize; newElement = newElement->NextSiblingElement())
	{
		//If the Text is null
		if (newElement->GetText() == nullptr)
		{
			//continue do not pass go do not update the index
			continue;
		}

		//Getting the string for the texture
		textureList[i] = newElement->GetText();

		//updating the array index
		i++;
	}

	//Setting up the RenderData
	(*newObject)->SetupRenderData(material.c_str(), 0, 1, textureList[0].c_str(), textureList[1].c_str(),
		textureList[2].c_str(), textureList[3].c_str(), textureList[4].c_str(), textureList[5].c_str(), textureList[6].c_str());

	//Closing the file
	doc.Clear();
}

/*static*/ Crate * EntityLoader::ReadCrate(tinyxml2::XMLElement * RootElement)
{
	Crate* newCrate = nullptr;

	//Creating the an IndentityMatrix and lowering the object
	XMFLOAT4X4 newTransform;
	SetToIdentity(newTransform);
	setPosition(newTransform, XMFLOAT3(0, -0.1f, 0));

	//Reading in RenderInfo
	XMLElement* newElement = RootElement->FirstChildElement();
	const char* textureList = newElement->GetText();

	//Creating creating a base ship to add parts to
	newCrate = newx Crate(newTransform, "", textureList);

	//loading in Textures
	ReadInRenderInfo((Entity3D**)&newCrate);

	return newCrate;
}

/*static*/ Port * EntityLoader::ReadPort(tinyxml2::XMLElement * RootElement)
{
	Port* newPort = nullptr;

	//Creating the an IndentityMatrix and lowering the object
	XMFLOAT4X4 newTransform;
	SetToIdentity(newTransform);
	setPosition(newTransform, XMFLOAT3(0, -0.1f, 0));

	//Reading in RenderInfo
	XMLElement* newElement = RootElement->FirstChildElement();
	const char* textureList = newElement->GetText();

	//Creating creating a base ship to add parts to
	newPort = newx Port(newTransform, "", textureList);


	//loading in Textures
	ReadInRenderInfo((Entity3D**)&newPort);


	//reading in ports
	newElement = newElement->NextSiblingElement();

	std::string Name = newElement->GetText();
	std::wstring portName = AssetUtilities::s2ws(Name);

	//checking to see if the port came with a name
	if (portName.c_str() != nullptr)
		newPort->SetPortName(portName);

	//reading in port info
	XMLElement* portinfoElement = newElement->NextSiblingElement("PortInfo");

	//port pages
	int pages;
	XMLElement* portpages = portinfoElement->FirstChildElement("pages");
	portpages->QueryIntText(&pages);
	newPort->SetNumPages(pages);

	//port upgrades abailable
	int p1, p2, p3;
	XMLElement* portupgrades = portinfoElement->FirstChildElement("Upgrades");
	portupgrades = portupgrades->FirstChildElement("W");
	portupgrades->QueryIntText(&p1);
	portupgrades = portupgrades->NextSiblingElement("S");
	portupgrades->QueryIntText(&p2);
	portupgrades = portupgrades->NextSiblingElement("H");
	portupgrades->QueryIntText(&p3);
	newPort->SetMaxUpgrade(0, p1);
	newPort->SetMaxUpgrade(1, p2);
	newPort->SetMaxUpgrade(2, p3);




	return newPort;
}

/*static*/ void EntityLoader::ReadInTexture(tinyxml2::XMLElement * RootElement, Entity3D ** newObject, unsigned int numTextures)
{

	std::string material;
	//Mesh information
	std::string textureList[7];




	//getting the first child
	material = RootElement->Attribute("material");

	//Getting the element inside of the Element
	XMLElement * newElement = RootElement->FirstChildElement("diffuse");


	//Getting the info for the textures
	for (unsigned int i = 0, counter = 0; i < 7 && counter < numTextures; i++, newElement = newElement->NextSiblingElement())
	{
		if (newElement->GetText() == nullptr)
		{
			continue;
		}

		textureList[i] = newElement->GetText();

		if (textureList[i].size() > 1)
		{
			counter++;
		}
	}

	//Setting up the RenderData
	(*newObject)->SetupRenderData(material.c_str(), 0, 1, textureList[0].c_str(), textureList[1].c_str(),
		textureList[2].c_str(), textureList[3].c_str(), textureList[4].c_str(), textureList[5].c_str(), textureList[6].c_str());
}

/*static*/ void EntityLoader::ReadInShipResource(tinyxml2::XMLElement * RootElement, Ship ** newShip)
{
	XMLElement * newElement = RootElement->FirstChildElement("Gold");
	int ReadInValue = 0;
	//Gold
	newElement->QueryIntText(&ReadInValue);
	(*newShip)->SetCurrGold(ReadInValue);

	//Food
	newElement = newElement->NextSiblingElement();
	newElement->QueryIntText(&ReadInValue);
	(*newShip)->SetCurrWaterFood(ReadInValue);

	//Rum
	newElement = newElement->NextSiblingElement();
	newElement->QueryIntText(&ReadInValue);
	(*newShip)->SetCurrRum(ReadInValue);

	//Crew
	newElement = newElement->NextSiblingElement();
	newElement->QueryIntText(&ReadInValue);
	(*newShip)->SetCurrCrew(ReadInValue);

}

/*static*/ void EntityLoader::ReadInHull(tinyxml2::XMLElement* RootElement, Ship** newShip)
{
	float turnSpeed;
	int Health;

	//Grabbing the info from the XML
	RootElement->QueryIntAttribute("Health", &Health);
	RootElement->FirstChildElement()->QueryFloatText(&turnSpeed);

	//Setting the appropriate values
	(*newShip)->SetMaxHealth(Health);
	(*newShip)->SetMaxTurnRate(turnSpeed);


}

/*static*/ void EntityLoader::ReadInMast(tinyxml2::XMLElement* RootElement, Ship** newShip)
{
	float MaxSpeed;
	float SlowDown;
	float ReadInValue;

	//Grabbing the info from the XML
	RootElement->FirstChildElement("MaxSpeed")->QueryFloatText(&MaxSpeed);
	RootElement->FirstChildElement("Acceleration")->QueryFloatText(&ReadInValue);
	(*newShip)->SetAcceleration(ReadInValue);
	RootElement->FirstChildElement("SlowdownSpeed")->QueryFloatText(&SlowDown);
	//Setting the appropriate values
	(*newShip)->SetMaxSpeed(MaxSpeed);
	(*newShip)->SetSlowDownMultipler(SlowDown);
}

/*static*/ void EntityLoader::ReadInCannonHandler(tinyxml2::XMLElement * RootElement, DirectX::XMFLOAT4X4 & newTransform, Ship ** newShip, SideCannonHandler ** newSideCannonHandler)
{

	int NumCannon;
	int CurrentAmmo;

	//Attribute info collection
	RootElement->QueryIntAttribute("NumCannon", &NumCannon);
	RootElement->QueryIntAttribute("CurrentAmmo", &CurrentAmmo);

	//Children Elements
	Cannon * newFrontCannon = nullptr;
	ReadInCannon(RootElement->FirstChildElement(), newTransform, newShip, &newFrontCannon);
	Cannon * newSideCannon = nullptr;
	ReadInCannon(RootElement->FirstChildElement()->NextSiblingElement(), newTransform, newShip, &newSideCannon);


	(*newSideCannonHandler) = newx SideCannonHandler(*newShip, newTransform);
	(*newSideCannonHandler)->SetNumAcquiredCannons(NumCannon);
	(*newSideCannonHandler)->SetCurrentAmmoIndex(CurrentAmmo);
	(*newSideCannonHandler)->SetSideCannon(newSideCannon);
	(*newSideCannonHandler)->SetLongNine(newFrontCannon);
}

/*static*/ void EntityLoader::ReadInAllIslandData(tinyxml2::XMLElement * RootElement)
{
	if (RootElement == nullptr)
	{
		return;
	}

	XMFLOAT4X4 newTransform;
	SetToIdentity(newTransform);

	XMLElement* IslandData = RootElement->FirstChildElement();


	for (; IslandData != nullptr; IslandData = IslandData->NextSiblingElement())
	{

		//Getting the object file name 
		const char* filename = IslandData->Attribute("ObjectFile");

		ReadInTransform(IslandData->FirstChildElement(), newTransform);


		fsEventManager::GetInstance()->FireEvent(fsEvents::LoadObject, &EventArgs2<const char*, XMFLOAT4X4>(filename, newTransform));
	}
}

/*static*/ void EntityLoader::ReadInAllShipData(tinyxml2::XMLElement * RootElement)
{
	if (RootElement == nullptr)
	{
		return;
	}

	XMLElement* ShipData = RootElement->FirstChildElement();

	XMFLOAT4X4 newTransform;
	SetToIdentity(newTransform);

	for (; ShipData != nullptr; ShipData = ShipData->NextSiblingElement())
	{
		const char* filename = ShipData->Attribute("ObjectFile");

		ReadInTransform(ShipData->FirstChildElement(), newTransform);

		fsEventManager::GetInstance()->FireEvent(fsEvents::LoadObject, &EventArgs2<const char*, XMFLOAT4X4>(filename, newTransform));
	}
}

/*static*/ void EntityLoader::ReadCameraOffset(Camera * _cam)
{
	//Getting the FilePath
	std::string FilePath = "../../FullSail/Resources/XML/Data/CameraOffset.xml";

	tinyxml2::XMLDocument doc;
	//loading the file in to memory
	//Quick safety check
	if (doc.LoadFile(FilePath.c_str()) != XML_NO_ERROR)
	{
		Log("Couldnt find path");
		return;
	}

	//Root Element
	XMLElement* root = doc.RootElement();
	//Safety check
	if (root == nullptr)
	{
		Log("There is no root");
		return;
	}
	XMLElement* newElement = root->FirstChildElement();
	newElement->QueryFloatText(&_cam->m_Offset);
	newElement = newElement->NextSiblingElement();
	newElement->QueryFloatText(&_cam->m_maxDiff);
}

/*static*/ void EntityLoader::ReadInCannon(tinyxml2::XMLElement * RootElement, DirectX::XMFLOAT4X4 & newTransform, Ship ** newShip, Cannon ** newCannon)
{
	if (RootElement == nullptr)
	{
		newCannon = nullptr;
		return;
	}

	int weaponType;
	RootElement->QueryIntAttribute("weaponType", &weaponType);

	int minDamage, maxDamage;
	tinyxml2::XMLElement * Damage = RootElement->FirstChildElement();
	Damage->QueryIntAttribute("MinDamage", &minDamage);
	Damage->QueryIntAttribute("MaxDamage", &maxDamage);

	(*newCannon) = newx Cannon(*newShip, (Cannon::WeaponType)weaponType, newTransform);
	(*newCannon)->SetDamage(minDamage, maxDamage);
}

/*static*/ void EntityLoader::ReadInTransform(tinyxml2::XMLElement * RootElement, XMFLOAT4X4& newTransform)
{
	//Getting the position
	XMFLOAT3 position = VectorZero;
	XMLElement* Position = RootElement->FirstChildElement("Position");
	Position->FirstChildElement("X")->QueryFloatText(&position.x);
	Position->FirstChildElement("Y")->QueryFloatText(&position.y);
	Position->FirstChildElement("Z")->QueryFloatText(&position.z);

	//Getting the rotation
	float rotationY = 0;
	XMLElement* Rotation = RootElement->FirstChildElement("Rotation");
	Rotation->FirstChildElement("Y")->QueryFloatText(&rotationY);
	rotationY = DegreeToRad(rotationY);

	//Getting the Scaling
	XMFLOAT3 Scaling = VectorOne;
	XMLElement* Scale = RootElement->FirstChildElement("Scale");
	Scale->FirstChildElement("X")->QueryFloatText(&Scaling.x);
	Scale->FirstChildElement("Y")->QueryFloatText(&Scaling.y);
	Scale->FirstChildElement("Z")->QueryFloatText(&Scaling.z);
	XMStoreFloat4x4(&newTransform, XMMatrixIdentity());

	//setting up the matrix
	//newTransform = newTransform * XMMatrixTransformation(
	//	XMLoadFloat3(&VectorZero),
	//	XMQuaternionRotationMatrix(XMLoadFloat4x4(&newTransform)),
	//	XMLoadFloat3(&Scaling),
	//	XMLoadFloat3(&VectorZero),
	//	XMQuaternionRotationAxis(XMLoadFloat3(&VectorY), rotationY),
	//	XMLoadFloat3(&position));

	XMStoreFloat4x4(&newTransform, XMMatrixRotationY(rotationY)*XMMatrixScaling(Scaling.x, Scaling.y, Scaling.z)*XMMatrixTranslation(position.x, position.y, position.z));

}

/*static*/ void EntityLoader::ReadInController(tinyxml2::XMLElement * RootElement, Ship** newShip, IController** newController)
{
	bool controller = false;
	float acceleration = 0;
	float RotationAcceleration = 0;

	RootElement->QueryBoolAttribute("controller", &controller);
	RootElement->QueryFloatAttribute("acceleration", &acceleration);
	RootElement->QueryFloatAttribute("rotationAcceleration", &RotationAcceleration);

	//Checking wheither its a boy or a girl heheheheheheheahahahahahahahh..mwuah
	//Checking to see if its Ai Controlled or player controlled

	if (controller)
	{
		(*newController) = new AiController(*newShip);

		//Grabbing the name from the Text 
		std::string aiName;
		aiName = RootElement->GetText();
		(*newShip)->SetControllerName(aiName.c_str());
		ReadInControllerInfo(aiName.c_str(), newShip, (AiController**)newController);
		//Setting the boid of the Ai controller
	}
	else
	{
		(*newController) = new PlayerController(*newShip);
		(*newShip)->SetControllerName("PlayerController");

	}



}

/*static*/ void EntityLoader::ReadInControllerInfo(const char* ObjectName, Ship ** newShip, AiController ** newController)
{
	//Getting the FilePath
	std::string FilePath = "../../FullSail/Resources/XML/Data/Controller";
	FilePath += ObjectName;
	FilePath += ".xml";

	tinyxml2::XMLDocument doc;
	//loading the file in to memory
	//Quick safety check
	if (doc.LoadFile(FilePath.c_str()) != XML_NO_ERROR)
	{
		return;
	}

	//Root Element
	XMLElement* root = doc.RootElement();
	//Safety check
	if (root == nullptr)
	{
		return;
	}

	//grabbing the type of AI
	int AI_TYPE = 0;
	root->QueryIntAttribute("aiType", &AI_TYPE);
	//Setting the Type
	AiController* newbie = (*newController);
	(*newController)->SetType(AI_TYPE);


	float ReadInValue;
	//Movement
	XMLElement* newElement = root->FirstChildElement();//moving the pointer down the tree

	newElement->FirstChildElement("SlowDownRadius")->QueryFloatText(&ReadInValue);
	newbie->SetSlowDownRadius(ReadInValue);

	//Attack 
	newElement = newElement->NextSiblingElement();//moving the pointer down the tree

	newElement->FirstChildElement("AttackDistance")->QueryFloatText(&ReadInValue);
	newbie->SetAttackDistance(ReadInValue);

	newElement->FirstChildElement("SideAttackDistance")->QueryFloatText(&ReadInValue);
	newbie->SetSideAttackDistance(ReadInValue);

	newElement->FirstChildElement("SideAttackPointRange")->QueryFloatText(&ReadInValue);
	newbie->SetAttackPointRange(ReadInValue);

	newElement->FirstChildElement("StationaryDistance")->QueryFloatText(&ReadInValue);
	newbie->SetDistanceStationary(ReadInValue);

	//CannonFire
	newElement = newElement->NextSiblingElement();//moving the pointer down the tree

	newElement->FirstChildElement("SideCannonAttackDistance")->QueryFloatText(&ReadInValue);
	newbie->SetSideCannonFireDistance(ReadInValue);

	newElement->FirstChildElement("SideCannonAngleLimit")->QueryFloatText(&ReadInValue);
	newbie->SetSideCannonAngleLimit(ReadInValue);

	newElement->FirstChildElement("FrontCannonRange")->QueryFloatText(&ReadInValue);
	newbie->SetFrontCannonRange(ReadInValue);

	//Flocking

	newElement = newElement->NextSiblingElement(); //moving the pointer down the tree
	newElement->FirstChildElement("FleeHealth")->QueryFloatText(&ReadInValue);
	newbie->SetFleeHealth((int)ReadInValue);
	doc.Clear();
}

/*static*/ Boid* EntityLoader::ReadInBoidInfo(std::string BoidName, Entity3D ** newObject)
{
	std::string FilePath = "../../FullSail/Resources/XML/Data/Boid";
	FilePath += BoidName;
	FilePath += ".xml";

	tinyxml2::XMLDocument doc;
	//loading the file in to memory
	//Quick safety check
	if (doc.LoadFile(FilePath.c_str()) != XML_NO_ERROR)
	{
		std::string FilePath = "../../FullSail/Resources/XML/Data/Controller";
		FilePath += "Default";
		FilePath += ".xml";

		if (doc.LoadFile(FilePath.c_str()) != XML_NO_ERROR)
		{
			return nullptr;
		}
	}

	//Root Element
	XMLElement* root = doc.RootElement();
	//Safety check
	if (root == nullptr)
	{
		return nullptr;
	}

	Boid* newBoid = new Boid();
	newBoid->SetOwner(*newObject);

	float ReadInValue = 0;

	//moving the pointer down the tree
	XMLElement* BoidInfo = root;

	BoidInfo->FirstChildElement("LeaderCohesionWeight")->QueryFloatText(&ReadInValue);
	newBoid->SetFollowingCohesion(ReadInValue);

	BoidInfo->FirstChildElement("LeaderAlignmentWeight")->QueryFloatText(&ReadInValue);
	newBoid->SetFollowingAlignment(ReadInValue);

	BoidInfo->FirstChildElement("LeaderSeperationWeight")->QueryFloatText(&ReadInValue);
	newBoid->SetFollowingSeperation(ReadInValue);

	BoidInfo->FirstChildElement("SquadCohesionWeight")->QueryFloatText(&ReadInValue);
	newBoid->SetSquadCohesion(ReadInValue);

	BoidInfo->FirstChildElement("SquadAlignmentWeight")->QueryFloatText(&ReadInValue);
	newBoid->SetSquadAlignment(ReadInValue);

	BoidInfo->FirstChildElement("SquadSeperationWeight")->QueryFloatText(&ReadInValue);
	newBoid->SetSquadSeperation(ReadInValue);

	BoidInfo->FirstChildElement("PursuitWeight")->QueryFloatText(&ReadInValue);
	newBoid->SetPursuitWeight(ReadInValue);

	BoidInfo->FirstChildElement("EvadeWeight")->QueryFloatText(&ReadInValue);
	newBoid->SetEvadeWeight(ReadInValue);

	BoidInfo->FirstChildElement("FleeWeight")->QueryFloatText(&ReadInValue);
	newBoid->SetFleeWeight(ReadInValue);

	BoidInfo->FirstChildElement("SeekWeight")->QueryFloatText(&ReadInValue);
	newBoid->SetSeekWeight(ReadInValue);

	BoidInfo->FirstChildElement("FlockRadius")->QueryFloatText(&ReadInValue);
	newBoid->SetRadius(ReadInValue);
	
	return newBoid;
}

EntityLoader::~EntityLoader()
{
}
