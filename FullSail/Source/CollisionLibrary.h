#pragma once
#include <algorithm>
#include "Colliders.h"
#include "Game.h"

static float minFloat = -FLT_MAX;
static std::vector<Triangle*> toTest;

int ClassifySphereToPlane(const XMFLOAT3& _planeNormal, const float _planeOffset, const XMFLOAT3& _sphereCenter, const float _sphereRadius)
{
	float distance = dotProduct(_sphereCenter, _planeNormal) - _planeOffset;
	return distance > _sphereRadius ? 1 : distance < -_sphereRadius ? 2 : 3;
}

bool pointInTriangle(const XMFLOAT3& point, Triangle& pTri)
{
	XMFLOAT3 edge0 = pTri.getVerts()[1] - pTri.getVerts()[0];
	XMFLOAT3 norm0;
	crossProduct(edge0, pTri.getNorm(), norm0);
	XMFLOAT3 V = point - pTri.getVerts()[0];
	float D = dotProduct(norm0, V);
	if (D > 0)
		return false;
	XMFLOAT3 edge1 = pTri.getVerts()[2] - pTri.getVerts()[1];
	XMFLOAT3 norm1;
	crossProduct(edge1, pTri.getNorm(), norm1);
	V = point - pTri.getVerts()[1];
	D = dotProduct(norm1, V);
	if (D > 0)
		return false;
	XMFLOAT3 edge2 = pTri.getVerts()[0] - pTri.getVerts()[2];
	XMFLOAT3 norm2;
	crossProduct(edge2, pTri.getNorm(), norm2);
	V = point - pTri.getVerts()[2];
	D = dotProduct(norm2, V);
	if (D > 0)
		return false;
	return true;
}

bool separatingAxisTest(size_t _numAxes, size_t _numVertsA, size_t _numVertsB, const XMFLOAT3** _axesA, const XMFLOAT3* _vertsA, const XMFLOAT3* _vertsB, const XMFLOAT3** OUT_PenetrationAxis = nullptr, float* OUT_PenetrationDepth = &minFloat)
{
	float dot, amin, amax, bmin, bmax;
	size_t vert;
	for (size_t axis = 0; axis < _numAxes; axis++)
	{
		amax = amin = dotProduct(_vertsA[0], (*_axesA)[axis]);
		for (vert = 1; vert < _numVertsA; vert++)
		{
			dot = dotProduct(_vertsA[vert], (*_axesA)[axis]);
			if (dot < amin)
				amin = dot;
			if (dot > amax)
				amax = dot;
		}
		bmax = bmin = dotProduct(_vertsB[0], (*_axesA)[axis]);
		for (vert = 1; vert < _numVertsB; vert++)
		{
			dot = dotProduct(_vertsB[vert], (*_axesA)[axis]);
			if (dot < bmin)
				bmin = dot;
			if (dot > bmax)
				bmax = dot;
		}
		if (amin > bmax || bmin > amax)
			return false;
		else if (OUT_PenetrationAxis)
		{
			float depth = bmin - amax;
			if (fabs(depth) < fabs(*OUT_PenetrationDepth))
			{
				*OUT_PenetrationDepth = depth;
				*OUT_PenetrationAxis = &((*_axesA)[axis]);
			}
		}
	}

#ifdef _DEBUG
	for (size_t i = 0; i < _numVertsB; i++)
	{
		CGame::GetApplication()->m_pRenderer->SetDebugShapeColor(&_vertsB[i], DirectX::XMFLOAT4{ 1.0f, 0.0f, 0.0f, 0.5f });
	}
#endif //debug

	return true;
}

bool CheckCollision(XMFLOAT3& _a, PlaneCollider& _b)
{
	XMFLOAT3 AB = _b.getPosition() - _a;
	float dot = dotProduct(AB, _b.getNormal());
	return dot > 0;
}

bool CheckCollision(BoxCollider& _a, BoxCollider& _b)
{
	size_t numAxesA;
	const XMFLOAT3* axesA = _a.getNormals(numAxesA);
	size_t numAxesB;
	const XMFLOAT3* axesB = _b.getNormals(numAxesB);
	size_t numAxes = 8;
	XMFLOAT3 axes[8] = { axesA[0], axesA[2], axesA[3], axesA[5], axesB[0], axesB[2], axesB[3], axesB[5] };
	const XMFLOAT3* pAxes = axes;
	size_t numVertsA;
	const XMFLOAT3* vertsA = _a.getVerts(numVertsA);
	size_t numVertsB;
	const XMFLOAT3* vertsB = _b.getVerts(numVertsB);
	if (!_a.isTrigger() && !_b.isTrigger())
	{
		const XMFLOAT3* axis = pAxes;
		float depth = -FLT_MAX;
		bool rtn = separatingAxisTest(numAxes, numVertsA, numVertsB, &pAxes, vertsA, vertsB, &axis, &depth);
		if (rtn)
		{
			XMFLOAT3 aNewPos = positionOf(_a.getParent()) + XMFLOAT3(axis->x, 0, axis->z) * depth * .5f;
			XMFLOAT3 bNewPos = positionOf(_b.getParent()) + XMFLOAT3(axis->x, 0, axis->z) * depth * -.5f;
			setPosition(_a.getParent(), aNewPos);
			setPosition(_b.getParent(), bNewPos);
		}
		return rtn;
	}
	return separatingAxisTest(numAxes, numVertsA, numVertsB, &pAxes, vertsA, vertsB);
}

bool CheckCollision(SphereCollider& _a, SphereCollider& _b)
{
	float aSqrRadius = _a.getRadius() * _a.getRadius();
	float bSqrRadius = _b.getRadius() * _b.getRadius();
	return sqrDistanceBetween(_a.getPosition(), _b.getPosition()) <= aSqrRadius + bSqrRadius;
}

bool CheckCollision(SphereCollider& _a, BoxCollider& _b)
{
	XMFLOAT3 dims = _b.getDimensions() * .5f;
	XMFLOAT3 closestOnBox = _b.getPosition();
	XMFLOAT3 right;
	rightAxisOf(_b.getParent(), right);
	XMFLOAT3 up;
	upAxisOf(_b.getParent(), up);
	XMFLOAT3 forward;
	forwardAxisOf(_b.getParent(), forward);
	float dot = clamp(dotProduct(_a.getPosition(), right) - dotProduct(_b.getPosition(), right), -dims.x, dims.x);
	closestOnBox += right * dot;
	dot = clamp(dotProduct(_a.getPosition(), up) - dotProduct(_b.getPosition(), up), -dims.y, dims.y);
	closestOnBox += up * dot;
	dot = clamp(dotProduct(_a.getPosition(), forward) - dotProduct(_b.getPosition(), forward), -dims.z, dims.z);
	closestOnBox += forward * dot;
	if (!_a.isTrigger() && !_b.isTrigger())
	{
		float sqrDist = sqrDistanceBetween(_a.getPosition(), closestOnBox);
		float sqrRadius = _a.getRadius() * _a.getRadius();
		if (sqrDist < sqrRadius)
		{
			XMFLOAT3 norm = positionOf(_b.getParent()) - positionOf(_a.getParent());
			normalizeFloat3(norm);
			norm.y = 0;
			setPosition(_b.getParent(), positionOf(_b.getParent()) + norm * (_a.getRadius() - sqrtf(sqrDist)));
		}
	}
	return sqrDistanceBetween(_a.getPosition(), closestOnBox) < _a.getRadius() * _a.getRadius();
}

bool CheckCollision(SphereCollider& _a, RayCollider& _b)
{
	XMFLOAT3 aToB = _b.getPosition() - _a.getPosition();
	float dot = dotProduct(aToB, _b.getVector());
	XMFLOAT3 closest = _b.getPosition() + (_b.getVector() * dot);
	return sqrDistanceBetween(_a.getPosition(), _b.getPosition()) < _a.getRadius() * _a.getRadius();
}

bool CheckCollision(SphereCollider& _a, PlaneCollider& _b)
{
	return CheckCollision(_a.getPosition() - (_b.getNormal() * _a.getRadius()), _b);
}

bool CheckCollision(BoxCollider& _a, RayCollider& _b)
{
	float tMin = 0.0f;
	float tMax = 100000.0f;

	XMFLOAT3 delta = _a.getPosition() - _b.getPosition();

	XMFLOAT3 xaxis = rightAxisOf(_a.getParent());
	float e = dotProduct(xaxis, delta);
	float f = dotProduct(_b.getVector(), xaxis);

	XMFLOAT3 dims = _a.getDimensions() * .5;

	// Beware, don't do the division if f is near 0 ! See full source code for details.
	float t1 = (e + dims.x) / f; // Intersection with the "left" plane
	float t2 = (e - dims.x) / f; // Intersection with the "right" plane

	if (t1 > t2) { // if wrong order
		float w = t1; t1 = t2; t2 = w; // swap t1 and t2
	}

	// tMax is the nearest "far" intersection (amongst the X,Y and Z planes pairs)
	if (t2 < tMax)
		tMax = t2;
	// tMin is the farthest "near" intersection (amongst the X,Y and Z planes pairs)
	if (t1 > tMin)
		tMin = t1;

	if (tMax < tMin)
		return false;
	return true;
}

bool CheckCollision(BoxCollider& _a, PlaneCollider& _b)
{
	size_t numVertsA;
	XMFLOAT3* vertsA = _a.getVerts(numVertsA);
	const XMFLOAT3* axis = &_b.getNormal();
	if (!_a.isTrigger() && !_b.isTrigger())
	{
		float depth = -FLT_MAX;
		bool rtn = separatingAxisTest(1, 1, numVertsA, &axis, &_b.getPosition(), vertsA, &axis, &depth);
		if (rtn)
		{
			depth *= .5f;
			setPosition(_a.getParent(), positionOf(_a.getParent()) + *axis * depth);
		}
		return rtn;
	}
	return separatingAxisTest(1, 1, numVertsA, &axis, &_b.getPosition(), vertsA);
}

bool CheckCollision(RayCollider& _a, PlaneCollider& _b)
{
	float dot = dotProduct(_b.getNormal(), _a.getVector());
	if (dot != 0)
	{
		float t = dotProduct(_b.getOffset() - _a.getVector(), _b.getNormal()) / dot;
		if (t >= 0)
			return true;
	}
	return false;
}

bool CheckCollision(BoxCollider& _a, Triangle* _b, bool _bTrigger = false)
{
	size_t numAxes = 1;
	size_t numVertsA = 1;
	size_t numVertsB = 1;
	const XMFLOAT3* axesA = _b->getAxes(numAxes);
	const XMFLOAT3* vertsA = _b->getVerts(numVertsA);
	const XMFLOAT3* vertsB = _a.getVerts(numVertsB);
	XMFLOAT3 axes[10];
	const XMFLOAT3* pAxes = axes;
	for (size_t i = 0; i < numAxes; i++)
	{
		axes[i] = axesA[i];
	}
	const XMFLOAT3* axesB = _a.getNormals(numAxes);
	numAxes = 4;
	for (size_t i = numAxes; i < 10; i++)
	{
		axes[i] = axesB[i - numAxes];
	}
	numAxes = 10;
	//numVertsA = 1;
#ifdef _DEBUG
	for (size_t i = 0; i < numVertsA; i++)
	{
		CGame::GetApplication()->m_pRenderer->SetDebugShapeColor(&vertsA[i], DirectX::XMFLOAT4{ 0.0f, 1.0f, 0.0f, 0.5f });
	}
#endif //debug
	if (!_a.isTrigger() && !_bTrigger)
	{
		bool rtn = separatingAxisTest(numAxes, numVertsA, numVertsB, &pAxes, vertsA, vertsB);
		if (rtn)
		{
			XMFLOAT3 axis = _b->getNorm();
			float amax, amin, bmax, bmin, dot, depth;
			size_t vert;
			amax = amin = dotProduct(vertsA[0], axis);
			for (vert = 1; vert < numVertsA; vert++)
			{
				dot = dotProduct(vertsA[vert], axis);
				if (dot < amin)
					amin = dot;
				if (dot > amax)
					amax = dot;
			}
			bmax = bmin = dotProduct(vertsB[0], axis);
			for (vert = 1; vert < numVertsB; vert++)
			{
				dot = dotProduct(vertsB[vert], axis);
				if (dot < bmin)
					bmin = dot;
				if (dot > bmax)
					bmax = dot;
			}
			depth = max(amin - bmax, bmin - amax);
			XMFLOAT3 axis2 = axis;
			axis2.y = 0;
			normalizeFloat3(axis2);
			dot = dotProduct(axis, axis2);
			setPosition(_a.getParent(), positionOf(_a.getParent()) + axis2 * (-depth * dot));
		}
		return rtn;
	}
	return separatingAxisTest(numAxes, numVertsA, numVertsB, &pAxes, vertsA, vertsB);
}

bool CheckCollision(BoxCollider& _a, MeshCollider& _b)
{
	toTest.reserve(2048);
	toTest.clear();
	_b.getQuadTree().ColliderToQT(_a, &toTest);

	for (size_t i = 0; i < toTest.size(); i++)
	{
		if (CheckCollision(_a, toTest[i], _b.isTrigger()))
			return true;
	}
	return false;
}

bool CheckCollision(BoxCollider& _a, SegmentCollider& _b)
{
	size_t numAxes = 1;
	size_t numVertsA = 1;
	size_t numVertsB = 1;
	const XMFLOAT3* axes = _a.getAxes(numAxes);
	const XMFLOAT3* vertsA = _a.getVerts(numVertsA);
	const XMFLOAT3* vertsB = _b.getVerts(numVertsB);
	return separatingAxisTest(numAxes, numVertsA, numVertsB, &axes, vertsA, vertsB);
}
bool CheckCollision(SegmentCollider& _a, BoxCollider& _b)
{
	return CheckCollision(_b, _a);
}

bool CheckCollision(Triangle* _a, SegmentCollider& _b, bool _isTrigger = false)
{
	size_t numVertsA;
	XMFLOAT3 norm = _a->getNorm();
	XMFLOAT3 start = _b.getStart();
	XMFLOAT3 end = _b.getEnd();
	float eDot = dotProduct(end, norm);
	float sDot = dotProduct(start, norm);
	float tDot = dotProduct(_a->getVerts(numVertsA)[0], norm);
	XMFLOAT3 segVector = end - start;


	if (sDot - tDot < 0)
		return false;
	if (eDot - tDot > 0)
		return false;

	if ((sDot - tDot < 0) && (eDot - tDot < 0))
		return false;

	if ((sDot - tDot > 0) && (eDot - tDot > 0))
		return false;

	float d2 = sDot - tDot;
	float d3 = dotProduct(segVector, norm);
	if (sDot < tDot || d3 > FLT_EPSILON)
	{
		return false;
	}
	float df = -(d2 / d3);
	XMFLOAT3 pointOnPlane = start + segVector * df;

	if (pointInTriangle(pointOnPlane, *_a))
	{
#ifdef DEBUG
		CGame::GetApplication()->m_pRenderer->SetDebugShapeColor(&_b, DirectX::XMFLOAT4{ 1.0f, 0.0f, 0.0f, 0.5f });
		_b.colliderMat._41 = pointOnPlane.x;
		_b.colliderMat._42 = pointOnPlane.y;
		_b.colliderMat._43 = pointOnPlane.z;
		_b.colliderMat *= XMMatrixInverse(nullptr, XMLoadFloat4x4(&_b.getParent()));
#endif // DEBUG
		return true;
	}
#ifdef DEBUG
	CGame::GetApplication()->m_pRenderer->SetDebugShapeColor(&_b, DirectX::XMFLOAT4{ 0.0f, 1.0f, 0.0f, 0.5f });
	_b.colliderMat = _b.localMat;
#endif // DEBUG
	return false;

}

bool CheckCollision(SphereCollider& _a, Triangle* _b, bool _isTrigger = false)
{
	size_t numVerts = 0;

	XMFLOAT3 normal = _b->getNorm();
	XMFLOAT3& sphereCenter = _a.getPosition();
	float offset = dotProduct(_b->getVerts(numVerts)[0], normal);
	int stpClass = ClassifySphereToPlane(normal, offset, sphereCenter, _a.getRadius());
	if (stpClass != 3)
		return false;
	XMFLOAT3 closestPoint;
	XMFLOAT3 pPt = sphereCenter - (normal * dotProduct((sphereCenter - _b->getVerts()[0]), normal));
	if (pointInTriangle(pPt, *_b))
	{
		closestPoint = pPt;

		if (sqrDistanceBetween(closestPoint, sphereCenter) <= _a.getRadius() * _a.getRadius())
			return true;
	}

	float d0 = sqrDistanceBetween(_b->getVerts()[0], sphereCenter);
	float d1 = sqrDistanceBetween(_b->getVerts()[1], sphereCenter);
	float d2 = sqrDistanceBetween(_b->getVerts()[2], sphereCenter);
	size_t furthest = d0 > d1 ? d0 > d2 ? 0 : 2 : d1 > d2 ? 1 : 2;
	XMFLOAT3 closestEdgeStart = _b->getVerts()[furthest != 2 ? 2 : furthest != 1 ? 1 : 0];
	XMFLOAT3 closestEdgeEnd = _b->getVerts()[furthest != 0 ? 0 : furthest != 1 ? 1 : 2];

	closestPoint = nearestLineSegmentToPoint(sphereCenter, closestEdgeStart, closestEdgeEnd);
	if (sqrDistanceBetween(closestPoint, sphereCenter) > _a.getRadius() * _a.getRadius())
		return false;
	return true;
}

bool CheckCollision(SphereCollider& _a, MeshCollider& _b)
{
	toTest.reserve(2048);
	toTest.clear();
	_b.getQuadTree().ColliderToQT(_a, &toTest);

	for (size_t i = 0; i < toTest.size(); i++)
	{
		if (CheckCollision(_a, toTest[i], _b.isTrigger()))
			return true;
	}
	return false;
}

bool CheckCollision(MeshCollider& _a, SphereCollider& _b)
{
	return CheckCollision(_b, _a);
}

bool CheckCollision(SegmentCollider& _a, Triangle* _b, bool _isTrigger = false)
{
	return CheckCollision(_b, _a, _isTrigger);
}

bool CheckCollision(SegmentCollider& _a, MeshCollider& _b)
{
	toTest.reserve(2048);
	toTest.clear();
	_b.getQuadTree().ColliderToQT(_a, &toTest);

	for (size_t i = 0; i < toTest.size(); i++)
	{
		if (CheckCollision(_a, toTest[i], _b.isTrigger()))
			return true;
	}
	return false;
}

bool CheckCollision(MeshCollider& _a, SegmentCollider& _b)
{
	return CheckCollision(_b, _a);
}

bool CheckCollision(MeshCollider& _a, BoxCollider& _b)
{
	return CheckCollision(_b, _a);
}

bool CheckCollision(BoxCollider& _a, SphereCollider& _b)
{
	return CheckCollision(_b, _a);
}

bool CheckCollision(RayCollider& _a, SphereCollider& _b)
{
	return CheckCollision(_b, _a);
}

bool CheckCollision(PlaneCollider& _a, SphereCollider& _b)
{
	return CheckCollision(_b, _a);
}

bool CheckCollision(RayCollider& _a, BoxCollider& _b)
{
	return CheckCollision(_b, _a);
}

bool CheckCollision(PlaneCollider& _a, BoxCollider& _b)
{
	return CheckCollision(_b, _a);
}

bool CheckCollision(PlaneCollider& _a, RayCollider& _b)
{
	return CheckCollision(_b, _a);
}

bool ParseColliders(Collider& _a, Collider& _b)
{
	switch (_a.GetType())
	{
	case Collider::ColliderType::Box:
		switch (_b.GetType())
		{
		case Collider::ColliderType::Box:
			return CheckCollision((BoxCollider&)_a, (BoxCollider&)_b);
			break;
		case Collider::ColliderType::Sphere:
			return CheckCollision((BoxCollider&)_a, (SphereCollider&)_b);
			break;
		case Collider::ColliderType::Ray:
			return CheckCollision((BoxCollider&)_a, (RayCollider&)_b);
			break;
		case Collider::ColliderType::Plane:
			return CheckCollision((BoxCollider&)_a, (PlaneCollider&)_b);
			break;
		case Collider::ColliderType::TriList:
			return CheckCollision((BoxCollider&)_a, (MeshCollider&)_b);
			break;
		case Collider::ColliderType::Segment:
			return CheckCollision((BoxCollider&)_a, (SegmentCollider&)_b);
			break;
		default:
			return false;
		}
		break;
	case Collider::ColliderType::Sphere:
		switch (_b.GetType())
		{
		case Collider::ColliderType::Box:
			return CheckCollision((SphereCollider&)_a, (BoxCollider&)_b);
			break;
		case Collider::ColliderType::Sphere:
			return false;// CheckCollision((SphereCollider&)_a, (SphereCollider&)_b);
			break;
		case Collider::ColliderType::Ray:
			return CheckCollision((SphereCollider&)_a, (RayCollider&)_b);
			break;
		case Collider::ColliderType::Plane:
			return CheckCollision((SphereCollider&)_a, (PlaneCollider&)_b);
			break;
		case Collider::ColliderType::TriList:
			return CheckCollision((SphereCollider&)_a, (MeshCollider&)_b);
		case Collider::ColliderType::Segment:
			return false;
		default:
			return false;
		}
		break;
	case Collider::ColliderType::Ray:
		switch (_b.GetType())
		{
		case Collider::ColliderType::Box:
			return CheckCollision((RayCollider&)_a, (BoxCollider&)_b);
			break;
		case Collider::ColliderType::Sphere:
			return CheckCollision((RayCollider&)_a, (SphereCollider&)_b);
			break;
		case Collider::ColliderType::Ray:
			return false;
			break;
		case Collider::ColliderType::Plane:
			return CheckCollision((RayCollider&)_a, (PlaneCollider&)_b);
			break;
		case Collider::ColliderType::TriList:
			return false;
		case Collider::ColliderType::Segment:
			return false;
		default:
			return false;
		}
		break;
	case Collider::ColliderType::Plane:
		switch (_b.GetType())
		{
		case Collider::ColliderType::Box:
			return CheckCollision((PlaneCollider&)_a, (BoxCollider&)_b);
			break;
		case Collider::ColliderType::Sphere:
			return CheckCollision((PlaneCollider&)_a, (SphereCollider&)_b);
			break;
		case Collider::ColliderType::Ray:
			return CheckCollision((PlaneCollider&)_a, (RayCollider&)_b);
			break;
		case Collider::ColliderType::Plane:
			return false;
			break;
		case Collider::ColliderType::TriList:
			return false;
		case Collider::ColliderType::Segment:
			return false;
		default:
			return false;
		}
		break;
	case Collider::ColliderType::TriList:
		switch (_b.GetType())
		{
		case Collider::ColliderType::Box:
			return CheckCollision((MeshCollider&)_a, (BoxCollider&)_b);
			break;
		case Collider::ColliderType::Sphere:
			return CheckCollision((MeshCollider&)_a, (SphereCollider&)_b);
			break;
		case Collider::ColliderType::Ray:
			return false;
			break;
		case Collider::ColliderType::Plane:
			return false;
			break;
		case Collider::ColliderType::TriList:
			return false;
			break;
		case Collider::ColliderType::Segment:
			return CheckCollision((MeshCollider&)_a, (SegmentCollider&)_b);
		default:
			return false;
		}
		break;
	case Collider::ColliderType::Segment:
		switch (_b.GetType())
		{
		case Collider::ColliderType::Box:
			return false;
			break;
		case Collider::ColliderType::Sphere:
			return false;
			break;
		case Collider::ColliderType::Ray:
			return false;
			break;
		case Collider::ColliderType::Plane:
			return false;
			break;
		case Collider::ColliderType::TriList:
			return CheckCollision((SegmentCollider&)_a, (MeshCollider&)_b);
			break;
		case Collider::ColliderType::Segment:
			return false;
		default:
			return false;
		}
		break;
	default:
		return false;
	}
}