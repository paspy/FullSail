#include "pch.h"
#include "Colliders.h"
#include "CollisionManager.h"
#include "Game.h"


//TODO: DAMIEN
Collider::Collider(CollisionManager& _manager, XMFLOAT4X4& _parent, void* _ID, ColliderType _type) : parent(_parent), manager(_manager), type(_type), tag("")
{
	ID = _ID;
	manager.trackThis(this);
}

Collider::Collider(CollisionManager & _manager, XMFLOAT4X4 & _parent, void * _ID, ColliderType _type, std::string _tag) : parent(_parent), manager(_manager), type(_type), tag(_tag)
{
	ID = _ID;
	manager.trackThis(this);
}


Collider::~Collider()
{
	manager.stopTracking(this);
	if (callback)
		delete callback;
#ifdef _DEBUG
	CGame::GetApplication()->m_pRenderer->RemoveDebugShape(this);
#endif // _DEBUG
}

//template<typename T>
//BoxCollider::BoxCollider(CollisionManager& _manager, Mesh& _mesh, const XMFLOAT4X4& _parent, void* _ID, CollisionFunction _func, bool _shouldBeTrigger) : Collider(_manager, _parent, _ID)
//{
//	trigger = _shouldBeTrigger;
//	collisionFunc = _func;
//	type = Box;
//}

BoxCollider::BoxCollider(CollisionManager& _manager, XMFLOAT4X4 & _parent, void* _ID, CollisionFunction* _func, XMFLOAT3 & _dimensions, XMFLOAT3 & _offset, bool _shouldBeTrigger) : Collider(_manager, _parent, _ID, Box)
{
	dimensions = _dimensions;
	offset = _offset;
	trigger = _shouldBeTrigger;
	callback = _func;
	XMStoreFloat4x4(&localMat, XMMatrixTransformation(
		XMLoadFloat3(&VectorZero),
		XMQuaternionIdentity(),
		XMLoadFloat3(&dimensions),
		XMLoadFloat3(&VectorZero),
		XMQuaternionIdentity(),
		XMLoadFloat3(&_offset)));

	verts[0] = ((XMFLOAT3(dimensions.x, dimensions.y, dimensions.z) * .5f) + offset);
	verts[1] = ((XMFLOAT3(dimensions.x, dimensions.y, -dimensions.z) * .5f) + offset);
	verts[2] = ((XMFLOAT3(dimensions.x, -dimensions.y, dimensions.z) * .5f) + offset);
	verts[3] = ((XMFLOAT3(dimensions.x, -dimensions.y, -dimensions.z) * .5f) + offset);
	verts[4] = ((XMFLOAT3(-dimensions.x, dimensions.y, dimensions.z) * .5f) + offset);
	verts[5] = ((XMFLOAT3(-dimensions.x, dimensions.y, -dimensions.z) * .5f) + offset);
	verts[6] = ((XMFLOAT3(-dimensions.x, -dimensions.y, dimensions.z) * .5f) + offset);
	verts[7] = ((XMFLOAT3(-dimensions.x, -dimensions.y, -dimensions.z) * .5f) + offset);
	rebuildTransform();

#ifdef _DEBUG
	XMFLOAT4 color = XMFLOAT4(1, 0, 0, 1);
	bool wire = false;
	CGame::GetApplication()->m_pRenderer->AddDebugCube(this, worldMat, color, wire);
#endif // _DEBUG
}

BoxCollider::~BoxCollider()
{
}

//SphereCollider::SphereCollider(CollisionManager& _manager, Mesh& _mesh, XMFLOAT4X4& _parent, void* _ID, CollisionFunction* _func, bool _shouldBeTrigger) : Collider(_manager, _parent, _ID)
//{
//	trigger = _shouldBeTrigger;
//	callback = _func;
//	type = Sphere;
//}

SphereCollider::SphereCollider(CollisionManager& _manager, XMFLOAT4X4 & _parent, void* _ID, CollisionFunction* _func, float _radius, XMFLOAT3 & _offset, bool _shouldBeTrigger, std::string _tag) : Collider(_manager, _parent, _ID, Sphere, _tag)
{
	radius = _radius;
	offset = _offset;
	trigger = _shouldBeTrigger;
	callback = _func;
	XMStoreFloat4x4(&localMat, XMMatrixTransformation(
		XMLoadFloat3(&VectorZero),
		XMQuaternionIdentity(),
		XMLoadFloat3(&(VectorOne*_radius)),
		XMLoadFloat3(&VectorZero),
		XMQuaternionIdentity(),
		XMLoadFloat3(&offset)));
	oldPosition = getPosition();

#ifdef _DEBUG
	XMFLOAT4 color = XMFLOAT4(1, 0, 0, 1);
	bool wire = false;
	CGame::GetApplication()->m_pRenderer->AddDebugSphere(this, worldMat, color, wire);
#endif // _DEBUG	
}

SphereCollider::~SphereCollider()
{
}

PlaneCollider::PlaneCollider(CollisionManager& _manager, XMFLOAT4X4& _parent, void* _ID, CollisionFunction* _func, XMFLOAT3& _normal, XMFLOAT3& _offset, bool _shouldBeTrigger) : Collider(_manager, _parent, _ID, Plane)
{
	normal = _normal;
	offset = _offset;
	trigger = _shouldBeTrigger;
	callback = _func;
}

PlaneCollider::~PlaneCollider()
{
}

RayCollider::RayCollider(CollisionManager& _manager, XMFLOAT4X4& _parent, void* _ID, CollisionFunction* _func, XMFLOAT3& _vector, XMFLOAT3& _offset, bool _shouldBeTrigger) : Collider(_manager, _parent, _ID, Ray)
{
	vector = _vector;
	offset = _offset;
	trigger = _shouldBeTrigger;
	callback = _func;
}

RayCollider::~RayCollider()
{
}



//QUADTREE
template <typename T>
Quadtree<T>::Quadtree()
{
	CAPACITY = 4;
	nw = nullptr;
	ne = nullptr;
	sw = nullptr;
	se = nullptr;
	boundary = AABB();
	objects = std::vector< T >();
}

template <typename T>
Quadtree<T>::Quadtree(AABB boundary)
{
	objects = std::vector< T >();
	CAPACITY = 4;
	nw = nullptr;
	ne = nullptr;
	sw = nullptr;
	se = nullptr;
	this->boundary = boundary;
}

template <typename T>
Quadtree<T>::~Quadtree()
{
	delete nw;
	delete sw;
	delete ne;
	delete se;
}

template <typename T>
void Quadtree<T>::subdivide()
{
	XMFLOAT2 qSize = XMFLOAT2(boundary.halfSize.x *.5f, boundary.halfSize.y *.5f);
	XMFLOAT2 qCenter = XMFLOAT2(boundary.center.x - qSize.x*.5f, boundary.center.y - qSize.y*.5f);
	nw = new Quadtree(AABB(qCenter, qSize));

	qCenter = XMFLOAT2(boundary.center.x + qSize.x*.5f, boundary.center.y - qSize.y*.5f);
	ne = new Quadtree(AABB(qCenter, qSize));

	qCenter = XMFLOAT2(boundary.center.x - qSize.x*.5f, boundary.center.y + qSize.y*.5f);
	sw = new Quadtree(AABB(qCenter, qSize));

	qCenter = XMFLOAT2(boundary.center.x + qSize.x*.5f, boundary.center.y + qSize.y*.5f);
	se = new Quadtree(AABB(qCenter, qSize));
}

template <typename T>
bool Quadtree<T>::insert(T d)
{
	if (!boundary.contains(d))
	{
		return false;
	}

	if (nw == nullptr)
		subdivide();

	if (nw->insert(d))
		return true;
	if (ne->insert(d))
		return true;
	if (sw->insert(d))
		return true;
	if (se->insert(d))
		return true;

	objects.push_back(d);
	return true;
}

MeshCollider::MeshCollider(CollisionManager & _manager, XMFLOAT4X4 & _parent, Mesh & _mesh, void * _ID, CollisionFunction * _func, bool _shouldBeTrigger, std::string _tag) : Collider(_manager, _parent, _ID, TriList, _tag)
{
	size_t numTris = _mesh.RawData.indices.size() / 3;
	tris = new Triangle[numTris];
	std::vector<Triangle*> culledTris;
	culledTris.reserve(numTris);
	size_t n = 0;
	XMFLOAT3 verts[3];
	float minx = FLT_MAX;
	float maxx = -FLT_MAX;
	float minz = FLT_MAX;
	float maxz = -FLT_MAX;
	for (size_t i = 0; i < numTris; i++)
	{
		verts[0] = _mesh.RawData.pos[_mesh.RawData.indices[3 * i]] * parent;
		verts[1] = _mesh.RawData.pos[_mesh.RawData.indices[3 * i + 1]] * parent;
		verts[2] = _mesh.RawData.pos[_mesh.RawData.indices[3 * i + 2]] * parent;

		for (size_t i = 0; i < 3; ++i)
		{
			if (minx > verts[i].x)
				minx = verts[i].x;
			if (maxx < verts[i].x)
				maxx = verts[i].x;
			if (minz > verts[i].z)
				minz = verts[i].z;
			if (maxz < verts[i].z)
				maxz = verts[i].z;
		}

		tris[i].setVerts(verts[0], verts[1], verts[2]);
		culledTris.push_back(&tris[i]);
	}
	minx -= 1;
	maxx += 1;
	minz -= 1;
	maxz += 1;

	float xdiff = maxx - minx;
	float zdiff = maxz - minz;
	offset = XMFLOAT3(minx + xdiff*0.5f, 0, minz + zdiff*0.5f);
	AABB bounds = AABB(XMFLOAT2(offset.x, offset.z), XMFLOAT2(xdiff, zdiff));
	triQT = Quadtree<Triangle*>(bounds);

	trigger = _shouldBeTrigger;
	callback = _func;

	XMStoreFloat4x4(&localMat, XMMatrixTransformation(
		XMLoadFloat3(&VectorZero),
		XMQuaternionIdentity(),
		XMLoadFloat3(&XMFLOAT3(parent._11, parent._22, parent._33)),
		XMLoadFloat3(&VectorZero),
		XMQuaternionIdentity(),
		XMLoadFloat3(&offset)));
	for (auto&& tri : culledTris)
		triQT.insert(tri);
}

MeshCollider::~MeshCollider()
{
}

Triangle::Triangle(XMFLOAT3 _a, XMFLOAT3 _b, XMFLOAT3 _c)
{
	vertices[0] = _a;
	vertices[1] = _b;
	vertices[2] = _c;
}

void Triangle::setVerts(XMFLOAT3 _a, XMFLOAT3 _b, XMFLOAT3 _c)
{
	vertices[0] = _a;
	vertices[1] = _b;
	vertices[2] = _c;

	XMFLOAT3 norm;
	XMFLOAT3 edge01 = _a - _b;
	XMFLOAT3 edge12 = _b - _c;
	XMFLOAT3 edge20 = _c - _a;
	XMStoreFloat3(&norm, XMVector3Cross(XMLoadFloat3(&edge01), XMLoadFloat3(&edge12)));
	normalizeFloat3(norm);
	setNorm(norm);

	XMStoreFloat3(&edgeNorms[0], XMVector3Cross(XMLoadFloat3(&edge01), XMLoadFloat3(&norm)));
	XMStoreFloat3(&edgeNorms[1], XMVector3Cross(XMLoadFloat3(&edge12), XMLoadFloat3(&norm)));
	XMStoreFloat3(&edgeNorms[2], XMVector3Cross(XMLoadFloat3(&edge20), XMLoadFloat3(&norm)));
	edgeNorms[3] = norm;

	//#ifdef _DEBUG
	//	XMFLOAT4 color = XMFLOAT4(1, 0, 0, 1);
	//	bool wire = false;
	//	for (size_t i = 0; i < 3; i++)
	//	{
	//		XMStoreFloat4x4(&worldMat, XMMatrixTransformation(
	//			XMLoadFloat3(&VectorZero),
	//			XMQuaternionIdentity(),
	//			XMLoadFloat3(&(VectorOne * .1f)),
	//			XMLoadFloat3(&VectorZero),
	//			XMQuaternionIdentity(),
	//			XMLoadFloat3(&vertices[i])));
	//		CGame::GetApplication()->m_pRenderer->AddDebugSphere(&vertices[i], worldMat, color, wire);
	//	}
	//#endif // _DEBUG
}

AABB::AABB(XMFLOAT2 center, XMFLOAT2 halfSize) : center(center), halfSize(halfSize)
{
	XMFLOAT2 vert2[4];
	vert2[0] = center + XMFLOAT2(halfSize.x, halfSize.y);
	vert2[1] = center + XMFLOAT2(halfSize.x, -halfSize.y);
	vert2[2] = center + XMFLOAT2(-halfSize.x, halfSize.y);
	vert2[3] = center + XMFLOAT2(-halfSize.x, -halfSize.y);
	verts[0] = XMFLOAT3(vert2[0].x, 0, vert2[0].y);
	verts[1] = XMFLOAT3(vert2[1].x, 0, vert2[1].y);
	verts[2] = XMFLOAT3(vert2[2].x, 0, vert2[2].y);
	verts[3] = XMFLOAT3(vert2[3].x, 0, vert2[3].y);
	//#ifdef _DEBUG
	//	XMStoreFloat4x4(&worldMat, XMMatrixTransformation(
	//		XMLoadFloat3(&VectorZero),
	//		XMQuaternionIdentity(),
	//		XMLoadFloat3(&XMFLOAT3(halfSize.x, 0.1f, halfSize.y)),
	//		XMLoadFloat3(&VectorZero),
	//		XMQuaternionIdentity(),
	//		XMLoadFloat3(&(XMFLOAT3(center.x, .1f, center.y)))));
	//	XMFLOAT4 color = XMFLOAT4(0.0f, 1.0f, 0, 1.0f);
	//	bool wire = true;
	//	CGame::GetApplication()->m_pRenderer->AddDebugCube(this, worldMat, color, wire);
	//#endif // _DEBUG
}

SegmentCollider::SegmentCollider(CollisionManager & _manager, XMFLOAT4X4 & _parent, void * _ID, CollisionFunction * _func, XMFLOAT3 _verts[2], XMFLOAT3 & _offset, bool _shouldBeTrigger) : Collider(_manager, _parent, _ID, Segment)
{
	if (_verts != nullptr)
	{
		verts[0] = _verts[0];
		verts[1] = _verts[1];
	}
	offset = _offset;
	trigger = _shouldBeTrigger;
	callback = _func;

	XMStoreFloat4x4(&colliderMat, XMMatrixTranslation(verts[1].x + offset.x, verts[1].y + offset.y, verts[1].z + offset.z));
	worldMat = colliderMat * parent;
	localMat = colliderMat;
	CGame::GetApplication()->m_pRenderer->AddDebugSphere(this, worldMat);
}
