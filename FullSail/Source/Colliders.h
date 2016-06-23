#pragma once
#include "Math.h"


class CollisionManager;
class Collider;

class CGame;

struct Mesh;


static XMFLOAT3& nearestLineSegmentToPoint(const XMFLOAT3& point, const XMFLOAT3& _start, const XMFLOAT3& _end)
{
	static XMFLOAT3 rtn;
	XMFLOAT3 L = _end - _start;
	XMFLOAT3 N = L; normalizeFloat3(N);
	XMFLOAT3 V = point - _start;
	float D = dotProduct(V, N);
	float D2 = dotProduct(L, N);
	D = D > D2 ? D2 : D < 0 ? 0 : D;
	N *= D;
	rtn = _start + N;
	return rtn;
}
class CollisionFunction
{
public:
	virtual void call(Collider&) = 0;

	//***************Allocator Overloads****************//
public:
//#if !_DEBUG
	inline void * operator new (size_t size, const char *file, int line){
		return FSMM::MemoryManager::GetInstance()->Allocate(static_cast<unsigned int>(size), 0U, file, line);
	}

		inline void * operator new (size_t size){
			return FSMM::MemoryManager::GetInstance()->Allocate(static_cast<unsigned int>(size));
	}

		inline void * operator new[](size_t size, const char *file, int line) {
		return FSMM::MemoryManager::GetInstance()->Allocate(static_cast<unsigned int>(size), 0U, file, line);
	}

		inline void * operator new[](size_t size) {
		return FSMM::MemoryManager::GetInstance()->Allocate(static_cast<unsigned int>(size));
	}

		inline void operator delete (void * p) {
		FSMM::MemoryManager::GetInstance()->DeAllocate(p);
	}

	inline void operator delete (void * p, const char *file, int line) {
		FSMM::MemoryManager::GetInstance()->DeAllocate(p);
	}

	inline void operator delete[](void * p) {
		FSMM::MemoryManager::GetInstance()->DeAllocate(p);
	}

		inline void operator delete[](void * p, const char *file, int line) {
		FSMM::MemoryManager::GetInstance()->DeAllocate(p);
	}
//#endif
};

template<typename T>
class TemplatedCollisionCallback : public CollisionFunction
{
	void (T::*m_fkt)(Collider&);
	T* m_obj;
public:
	TemplatedCollisionCallback(T* const obj, void (T::*fkt)(Collider&)) :m_fkt(fkt), m_obj(obj) {}

	void call(Collider& _other)
	{
		if (m_obj)
			(m_obj->*m_fkt)(_other);
	}
};

class Triangle
{
	XMFLOAT3 vertices[3];
	XMFLOAT3 norm;
	XMFLOAT3 edgeNorms[4];
	XMFLOAT4X4 worldMat;
public:
	Triangle() {};
	Triangle(XMFLOAT3 _a, XMFLOAT3 _b, XMFLOAT3 _c);

	const XMFLOAT3* getVerts() const
	{
		return vertices;
	}

	const XMFLOAT3* getVerts(size_t& _numVerts) const
	{
		_numVerts = 3;
		return vertices;
	}

	const XMFLOAT3* getAxes(size_t& _numAxes)
	{
		_numAxes = 4;
		return edgeNorms;
	}

	void setVerts(XMFLOAT3 _a, XMFLOAT3 _b, XMFLOAT3 _c);

	const XMFLOAT3& getNorm() { return norm; }

	void setNorm(XMFLOAT3 _norm) { norm = _norm; }
};

class Collider
{
	friend class CollisionManager;
public:
	enum ColliderType
	{
		Box, Sphere, Plane, Ray, TriList, Segment,
		numTypes
	};

	Collider(CollisionManager& _manager, XMFLOAT4X4& _parent, void* _ID, ColliderType _type);
	Collider(CollisionManager& _manager, XMFLOAT4X4& _parent, void* _ID, ColliderType _type, std::string _tag);
	virtual ~Collider() = 0;

	void SetAsTrigger(bool _shouldBeTrigger) { trigger = _shouldBeTrigger; }

	ColliderType& GetType() { return type; }

	void SetHitEffect(CollisionFunction* _func) { callback = _func; }

	void OnCollision(Collider& _other)
	{
		if (callback)
			callback->call(_other);
	}

	bool isTrigger() { return trigger; }

	virtual XMFLOAT3& getPosition()
	{
		static XMFLOAT3 pos;
		pos = (offset * parent);
		return pos;
	}

	XMFLOAT4X4& getParent() { return parent; }

	const XMFLOAT3& getOffset() { return offset; }

	void* getID() { return ID; }

	virtual void rebuildTransform() {}

protected:
	bool trigger;
	ColliderType type;
	XMFLOAT4X4& parent;
	XMFLOAT3 offset;
	CollisionFunction* callback;
	std::string tag = "";
private:
	CollisionManager& manager;
	void* ID;

	//***************Allocator Overloads****************//
public:
//#if !_DEBUG
	inline void * operator new (size_t size, const char *file, int line){
		return FSMM::MemoryManager::GetInstance()->Allocate(static_cast<unsigned int>(size), 0U, file, line);
	}

		inline void * operator new (size_t size){
			return FSMM::MemoryManager::GetInstance()->Allocate(static_cast<unsigned int>(size));
	}

		inline void * operator new[](size_t size, const char *file, int line) {
		return FSMM::MemoryManager::GetInstance()->Allocate(static_cast<unsigned int>(size), 0U, file, line);
	}

		inline void * operator new[](size_t size) {
		return FSMM::MemoryManager::GetInstance()->Allocate(static_cast<unsigned int>(size));
	}

		inline void operator delete (void * p) {
		FSMM::MemoryManager::GetInstance()->DeAllocate(p);
	}

	inline void operator delete (void * p, const char *file, int line) {
		FSMM::MemoryManager::GetInstance()->DeAllocate(p);
	}

	inline void operator delete[](void * p) {
		FSMM::MemoryManager::GetInstance()->DeAllocate(p);
	}

		inline void operator delete[](void * p, const char *file, int line) {
		FSMM::MemoryManager::GetInstance()->DeAllocate(p);
	}
//#endif
};

class SegmentCollider : public Collider
{
private:
	XMFLOAT3 verts[2] = { VectorZero, VectorZero };
	XMFLOAT3 wVerts[2];
	XMFLOAT3 vector;
	XMFLOAT4X4 worldMat;
public:
	XMFLOAT4X4 localMat;
	XMFLOAT4X4 colliderMat;
	SegmentCollider(
		CollisionManager& _manager,
		XMFLOAT4X4& _parent,
		void* _ID = nullptr,
		CollisionFunction* _func = nullptr,
		XMFLOAT3 _verts[2] = nullptr,
		XMFLOAT3& _offset = VectorZero,
		bool _shouldBeTrigger = true);
	~SegmentCollider() {}
	void SetStartPoint(const XMFLOAT3& startPoint) { verts[0] = startPoint; }
	void SetEndPoint(const XMFLOAT3& endPoint) { verts[1] = endPoint; }
	const XMFLOAT3& getStart() { wVerts[0] = (verts[0] + offset) * parent; return wVerts[0]; }
	const XMFLOAT3& getEnd() { wVerts[1] = (verts[1] + offset) * parent; return wVerts[1]; }
	const XMFLOAT3* getVerts(size_t& _numVerts)
	{
		_numVerts = 2;
		wVerts[0] = (verts[0] + offset) * parent;
		wVerts[1] = (verts[1] + offset) * parent;
		return wVerts;
	}
	XMFLOAT3& getVector()
	{
		vector = (verts[1] - verts[0]) * parent;
		return vector;
	}
	XMFLOAT3& getNormVector()
	{
		vector = (verts[1] - verts[0]) * parent;
		normalizeFloat3(vector);
		return vector;
	}

	void rebuildTransform()
	{
		worldMat = colliderMat * parent;
	}
};

class BoxCollider : public Collider
{
private:
	XMFLOAT3 dimensions;
	XMFLOAT3 norms[6];
	XMFLOAT3 axes[3];
	XMFLOAT3 verts[8];
	XMFLOAT3 wVerts[8];
	XMFLOAT4X4 worldMat;
	XMFLOAT4X4 localMat;
public:

	/*BoxCollider(
		CollisionManager& _manager,
		Mesh& _mesh,
		const XMFLOAT4X4& _parent,
		void* _ID = nullptr,
		void(*_func)(Collider&) = nullptr,
		bool _shouldBeTrigger = true);*/
	BoxCollider(
		CollisionManager& _manager,
		XMFLOAT4X4& _parent,
		void* _ID = nullptr,
		CollisionFunction* _func = nullptr,
		XMFLOAT3& _dimensions = VectorOne,
		XMFLOAT3& _offset = VectorZero,
		bool _shouldBeTrigger = true);
	~BoxCollider();

	const XMFLOAT3& getDimensions()
	{
		return dimensions;
	}

	void setDimensions(XMFLOAT3& _dims)
	{
		dimensions = _dims;
		localMat._11 = _dims.x;
		localMat._22 = _dims.y;
		localMat._33 = _dims.z;
	}

	void setOffset(XMFLOAT3& _offset) {
		offset = _offset;
		memcpy(&localMat._41, &_offset, sizeof(_offset));
	}

	const XMFLOAT3* getNormals(size_t& _numNormsOut)
	{
		_numNormsOut = 6;
		XMFLOAT3 right;
		rightAxisOf(parent, right);
		XMFLOAT3 up;
		upAxisOf(parent, up);
		XMFLOAT3 forward;
		forwardAxisOf(parent, forward);
		norms[0] = right;
		norms[1] = up;
		norms[2] = forward;
		norms[3] = -right;
		norms[4] = -up;
		norms[5] = -forward;
		return norms;
	}

	const XMFLOAT3* getAxes(size_t& _numAxes)
	{
		_numAxes = 3;
		rightAxisOf(parent, axes[0]);
		upAxisOf(parent, axes[1]);
		forwardAxisOf(parent, axes[2]);
		return axes;
	}

	XMFLOAT3* getVerts(size_t& _numVertsOut)
	{
		_numVertsOut = 8;
		wVerts[0] = (verts[0] * parent);
		wVerts[1] = (verts[1] * parent);
		wVerts[2] = (verts[2] * parent);
		wVerts[3] = (verts[3] * parent);
		wVerts[4] = (verts[4] * parent);
		wVerts[5] = (verts[5] * parent);
		wVerts[6] = (verts[6] * parent);
		wVerts[7] = (verts[7] * parent);
		return wVerts;
	}

	XMFLOAT3* getVerts2D(size_t& _numVertsOut)
	{
		_numVertsOut = 4;
		wVerts[0] = (verts[0] * parent);
		wVerts[1] = (verts[1] * parent);
		wVerts[2] = (verts[2] * parent);
		wVerts[3] = (verts[3] * parent);
		return wVerts;
	}

	void rebuildTransform()
	{
		XMFLOAT3 origin;
		positionOf(parent, origin);

		worldMat = localMat * parent;
	}
};

class SphereCollider : public Collider
{
	float radius;
	XMFLOAT4X4 worldMat;
	XMFLOAT4X4 localMat;
public:
	XMFLOAT3 oldPosition;
	/*SphereCollider(
		CollisionManager& _manager,
		Mesh& _mesh,
		XMFLOAT4X4& _parent,
		void* _ID = nullptr,
		CollisionFunction* _func = nullptr,
		bool _shouldBeTrigger = true);*/
	SphereCollider(
		CollisionManager& _manager,
		XMFLOAT4X4& _parent,
		void* _ID = nullptr,
		CollisionFunction* _func = nullptr,
		float _radius = .5f,
		XMFLOAT3& _offset = VectorZero,
		bool _shouldBeTrigger = true,
		std::string _tag = "");
	~SphereCollider();

	const float& getRadius()
	{
		return radius;
	}

	void getMovementExtents(XMFLOAT3 OUT_Extents[2])
	{
		OUT_Extents[0] = getPosition();
		OUT_Extents[1] = oldPosition;
	}

	void rebuildTransform()
	{
		XMFLOAT3 origin;
		positionOf(parent, origin);

		worldMat = localMat * parent;
	}
};

class PlaneCollider : public Collider
{
	XMFLOAT3 normal;
public:
	PlaneCollider(
		CollisionManager& _manager,
		XMFLOAT4X4& _parent,
		void* _ID = nullptr,
		CollisionFunction* _func = nullptr,
		XMFLOAT3& _normal = VectorY,
		XMFLOAT3& _offset = VectorZero,
		bool _shouldBeTrigger = true);
	~PlaneCollider();

	const XMFLOAT3& getNormal()
	{
		return normal;
	}
};

class RayCollider : public Collider
{
	XMFLOAT3 vector;
public:
	RayCollider(
		CollisionManager& _manager,
		XMFLOAT4X4& _parent,
		void* _ID = nullptr,
		CollisionFunction* _func = nullptr,
		XMFLOAT3& _vector = VectorZ,
		XMFLOAT3& _offset = VectorZero,
		bool _shouldBeTrigger = true);
	~RayCollider();

	const XMFLOAT3& getVector()
	{
		return vector;
	}
};

class AABB
{
	XMFLOAT4X4 worldMat;
	XMFLOAT3 axes[2] = { VectorX, VectorZ };
	XMFLOAT3 verts[4];
public:
	XMFLOAT2 center;
	XMFLOAT2 halfSize;

	AABB(XMFLOAT2 center = XMFLOAT2(0, 0), XMFLOAT2 halfSize = XMFLOAT2(100, 100));

	const XMFLOAT3* getAxes(size_t& _numAxes)
	{
		_numAxes = 2;
		return axes;
	}

	const XMFLOAT3* getVerts(size_t& _numVerts)
	{
		_numVerts = 2;
		return verts;
	}

	bool contains(const XMFLOAT3& _point)
	{
		if (_point.x <= center.x + (halfSize.x * .5f) && _point.x >= center.x - (halfSize.x * .5f))
		{
			return (_point.z <= center.y + (halfSize.y * .5f) && _point.z >= center.y - (halfSize.y * .5f));
		}
		return false;
	}

	bool contains(const Triangle* _tri)
	{
		size_t numVerts = 0;
		const XMFLOAT3* verts = _tri->getVerts(numVerts);
		return contains(verts[0])
			&& contains(verts[1])
			&& contains(verts[2]);
	}

	bool contains(SegmentCollider& _seg)
	{
		auto start = _seg.getStart();

		auto end = _seg.getEnd();
		XMFLOAT3 closestOnSeg = nearestLineSegmentToPoint(XMFLOAT3(center.x, 0, center.y), start, end); // _seg.getStart() + _seg.getVector() * interp;
		return contains(closestOnSeg);
	}

	bool contains(Collider& _collider)
	{
		size_t numVerts = 0;
		const XMFLOAT3* cverts;
		switch (_collider.GetType())
		{
		case Collider::ColliderType::Box:
			cverts = reinterpret_cast<BoxCollider&>(_collider).getVerts2D(numVerts);
			break;
		case Collider::ColliderType::Sphere:
		{
			SphereCollider& sphere = reinterpret_cast<SphereCollider&>(_collider);
			XMFLOAT3& spherePos = sphere.getPosition();
			XMFLOAT2 spherePos2d = XMFLOAT2(spherePos.x, spherePos.z);
			XMFLOAT2 closestPointOnAABB = center;
			closestPointOnAABB.x += clamp(spherePos2d.x - center.x, -halfSize.x*.5f, halfSize.x*.5f);
			closestPointOnAABB.y += clamp(spherePos2d.y - center.y, -halfSize.y*.5f, halfSize.y*.5f);
			return sqrDistanceBetween(closestPointOnAABB, spherePos2d) < sphere.getRadius() * sphere.getRadius();
		}
		case Collider::ColliderType::Plane:
			return false;
		case Collider::ColliderType::Ray:
			return false;
		case Collider::ColliderType::TriList:
			return false;
		case Collider::ColliderType::Segment:
			return contains(reinterpret_cast<SegmentCollider&>(_collider));
		default:
			return false;
		}
		for (size_t i = 0; i < numVerts; i++)
		{
			if (contains(cverts[i]))
				return true;
		}
		return false;
	}
};

template <typename T>
class Quadtree
{
private:
	//4 children
	Quadtree* nw = nullptr;
	Quadtree* ne = nullptr;
	Quadtree* sw = nullptr;
	Quadtree* se = nullptr;

	AABB boundary;

	std::vector< T > objects;

	int CAPACITY;
public:
	Quadtree<T>();
	Quadtree<T>(AABB boundary);

	~Quadtree();

	bool insert(T d);
	void subdivide();

	void ColliderToQT(Collider& _other, std::vector<T>* _objectsOUT)
	{
		if (!boundary.contains(_other))
		{
#ifdef _DEBUG
			CGame::GetApplication()->m_pRenderer->SetDebugShapeColor(&boundary, DirectX::XMFLOAT4{ 0.0f, 1.0f, 0.0f, 0.5f });
#endif //DEBUG
			return;
		}

#ifdef _DEBUG
		CGame::GetApplication()->m_pRenderer->SetDebugShapeColor(&boundary, DirectX::XMFLOAT4{ 1.0f, 0.0f, 0.0f, 0.5f });
#endif //DEBUG

		if (objects.size() > 0)
		{
			_objectsOUT->insert(_objectsOUT->end(), objects.begin(), objects.end());
		}

		if (ne)
			ne->ColliderToQT(_other, _objectsOUT);
		if (nw)
			nw->ColliderToQT(_other, _objectsOUT);
		if (se)
			se->ColliderToQT(_other, _objectsOUT);
		if (sw)
			sw->ColliderToQT(_other, _objectsOUT);
	}
	AABB& GetMyAABB() { return boundary; }

};

class MeshCollider : public Collider
{
	Triangle* tris;
	XMFLOAT4X4 worldMat;
	XMFLOAT4X4 localMat;
	Quadtree<Triangle*> triQT;
public:
	MeshCollider(
		CollisionManager& _manager,
		XMFLOAT4X4& _parent,
		Mesh& _mesh,
		void* _ID = nullptr,
		CollisionFunction* _func = nullptr,
		bool _shouldBeTrigger = true,
		std::string _tag = "");
	~MeshCollider();

	Quadtree<Triangle*>& getQuadTree() { return triQT; }

	void rebuildTransform()
	{
		XMFLOAT3 origin;
		positionOf(parent, origin);

		worldMat = localMat * parent;
	}

	/*XMFLOAT3 getPosition()
	{
		return positionOf(worldMat);
	}*/
};