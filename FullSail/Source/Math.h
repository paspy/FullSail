#pragma once
#include "pch.h"

#define RotationScaleDegrees 180.0f

#define VectorZero XMFLOAT3(0, 0, 0)
#define VectorOne XMFLOAT3(1, 1, 1)
#define VectorX XMFLOAT3(1, 0, 0)
#define VectorY XMFLOAT3(0, 1, 0)
#define VectorZ XMFLOAT3(0, 0, 1)

using namespace DirectX;

namespace DirectX
{
	static XMFLOAT3& operator+ (const XMFLOAT3& l, const XMFLOAT3& r)
	{
		static XMFLOAT3 sum;
		XMStoreFloat3(&sum, XMLoadFloat3(&l) + XMLoadFloat3(&r));
		return sum;
	}

	static XMFLOAT2& operator+ (const XMFLOAT2& l, const XMFLOAT2& r)
	{
		static XMFLOAT2 sum;
		XMStoreFloat2(&sum, XMLoadFloat2(&l) + XMLoadFloat2(&r));
		return sum;
	}

	static void operator+= (XMFLOAT3& l, const XMFLOAT3& r)
	{
		XMStoreFloat3(&l, XMLoadFloat3(&l) + XMLoadFloat3(&r));
	}

	static XMFLOAT3& operator- (const XMFLOAT3& l, const XMFLOAT3& r)
	{
		static XMFLOAT3 sum;
		XMStoreFloat3(&sum, XMLoadFloat3(&l) - XMLoadFloat3(&r));
		return sum;
	}

	static XMFLOAT3& operator- (const XMFLOAT3& r)
	{
		static XMFLOAT3 sum;
		XMStoreFloat3(&sum, -XMLoadFloat3(&r));
		return sum;
	}

	static XMFLOAT2& operator- (const XMFLOAT2& l, const XMFLOAT2& r)
	{
		static XMFLOAT2 sum;
		XMStoreFloat2(&sum, XMLoadFloat2(&l) - XMLoadFloat2(&r));
		return sum;
	}

	static void operator-= (XMFLOAT3& l, const XMFLOAT3& r)
	{
		XMStoreFloat3(&l, XMLoadFloat3(&l) - XMLoadFloat3(&r));
	}

	static XMFLOAT3& operator* (const XMFLOAT3& l, const float r)
	{
		static XMFLOAT3 product;
		XMStoreFloat3(&product, XMLoadFloat3(&l) * r);
		return product;
	}

	static XMFLOAT2& operator* (const XMFLOAT2& l, const float r)
	{
		static XMFLOAT2 product;
		XMStoreFloat2(&product, XMLoadFloat2(&l) * r);
		return product;
	}

	static XMFLOAT3& operator* (const XMFLOAT3& l, const XMFLOAT3& r)
	{
		static XMFLOAT3 product;
		XMStoreFloat3(&product, XMLoadFloat3(&l) * XMLoadFloat3(&r));
		return product;
	}

	static void operator*= (XMFLOAT3& l, const float r)
	{
		XMStoreFloat3(&l, XMLoadFloat3(&l) * r);
	}

	static XMFLOAT3& operator/ (const XMFLOAT3& l, const float r)
	{
		static XMFLOAT3 product;
		XMStoreFloat3(&product, XMLoadFloat3(&l) / r);
		return product;
	}

	static void operator/= (XMFLOAT3& l, const float r)
	{
		XMStoreFloat3(&l, XMLoadFloat3(&l) / r);
	}

	static XMFLOAT4X4& operator*(const XMFLOAT4X4& l, const XMFLOAT4X4& r)
	{
		static XMFLOAT4X4 product;
		XMStoreFloat4x4(&product, XMLoadFloat4x4(&l) * XMLoadFloat4x4(&r));
		return product;
	}

	static XMFLOAT4X4& operator*(const XMFLOAT4X4& l, const XMMATRIX& r)
	{
		static XMFLOAT4X4 product;
		XMStoreFloat4x4(&product, XMLoadFloat4x4(&l) * r);
		return product;
	}

	static XMFLOAT4X4& operator*(const XMMATRIX& l, const XMFLOAT4X4& r)
	{
		static XMFLOAT4X4 product;
		XMStoreFloat4x4(&product, l * XMLoadFloat4x4(&r));
		return product;
	}

	static void operator*=(XMFLOAT4X4& l, const XMMATRIX& r)
	{
		XMStoreFloat4x4(&l, XMLoadFloat4x4(&l) * r);
	}

	static XMFLOAT3& operator*(const XMFLOAT3& l, const XMMATRIX& r)
	{
		static XMFLOAT3 product;
		XMStoreFloat3(&product, XMVector3Transform(XMLoadFloat3(&l), r));
		return product;
	}

	static XMFLOAT3& operator*(const XMFLOAT3& l, const XMFLOAT4X4& r)
	{
		static XMFLOAT3 product;
		XMStoreFloat3(&product, XMVector3Transform(XMLoadFloat3(&l), XMLoadFloat4x4(&r)));
		return product;
	}

	static XMFLOAT3& operator*(const XMFLOAT3& l, const XMFLOAT3X3& r)
	{
		static XMFLOAT3 product;
		XMStoreFloat3(&product, XMVector3Transform(XMLoadFloat3(&l), XMLoadFloat3x3(&r)));
		return product;
	}

	//Checking to see if the Vectors are equal to one another
	static bool operator==(const XMFLOAT3& l, const XMFLOAT3& r)
	{
		return XMVector3Equal(XMLoadFloat3(&l), XMLoadFloat3(&r));
	}

	//Checking to see if the vectors are not equal to one another
	static bool operator!=(const XMFLOAT3& l, const XMFLOAT3& r)
	{
		return !XMVector3Equal(XMLoadFloat3(&l), XMLoadFloat3(&r));
	}

	//Checking to see if the Vectors are equal to one another
	static bool operator==(const XMFLOAT4X4& l, const XMFLOAT4X4& r)
	{
		XMFLOAT3 LeftHand, RightHand;

		memcpy(&LeftHand, &l._11, sizeof(LeftHand));
		memcpy(&LeftHand, &r._11, sizeof(RightHand));
		if (LeftHand != RightHand)
		{
			return false;
		}

		memcpy(&LeftHand, &l._21, sizeof(LeftHand));
		memcpy(&LeftHand, &r._21, sizeof(RightHand));
		if (LeftHand != RightHand)
		{
			return false;
		}
		memcpy(&LeftHand, &l._31, sizeof(LeftHand));
		memcpy(&LeftHand, &r._31, sizeof(RightHand));
		if (LeftHand != RightHand)
		{
			return false;
		}
		memcpy(&LeftHand, &l._41, sizeof(LeftHand));
		memcpy(&LeftHand, &r._41, sizeof(RightHand));
		if (LeftHand != RightHand)
		{
			return false;
		}
		return true;
	}



	static XMFLOAT3 positionOf(const XMFLOAT4X4& _m)
	{
		static XMFLOAT3 pos;
		XMStoreFloat3(&pos, XMLoadFloat3((XMFLOAT3*)&_m._41));
		return pos;
	}

	// this should be slightly faster than the other method
	static void positionOf(const XMFLOAT4X4& _m, XMFLOAT3& _positionOut)
	{
		XMStoreFloat3(&_positionOut, XMLoadFloat3((XMFLOAT3*)&_m._41));
	}

	static void lookTo(XMFLOAT4X4& _m, XMFLOAT3& _look)
	{
		static XMFLOAT3 pos;
		positionOf(_m, pos);
		XMStoreFloat4x4(&_m, XMMatrixLookToLH(XMLoadFloat3(&pos), XMLoadFloat3(&_look), XMLoadFloat3(&VectorY)));
	}

	static void lookAt(XMFLOAT4X4& _m, XMFLOAT3& _target)
	{
		static XMFLOAT3 pos;
		positionOf(_m, pos);
		XMStoreFloat4x4(&_m, XMMatrixLookAtLH(XMLoadFloat3(&pos), XMLoadFloat3(&_target), XMLoadFloat3(&VectorY)));
	}

	static void normalizeFloat3(XMFLOAT3& _f3)
	{
		XMStoreFloat3(&_f3, XMVector3Normalize(XMLoadFloat3(&_f3)));
	}

	static XMFLOAT3& normalizeFloat3(const XMFLOAT3& _f3)
	{
		static XMFLOAT3 product;
		XMStoreFloat3(&product, XMVector3Normalize(XMLoadFloat3(&_f3)));
		return product;
	}

	static void normalizeFloat2(XMFLOAT2& _f2)
	{
		XMStoreFloat2(&_f2, XMVector2Normalize(XMLoadFloat2(&_f2)));
	}

	static float sqrDistanceBetween(const XMFLOAT3& _a, const XMFLOAT3& _b)
	{
		XMFLOAT3 diffs = _a - _b;
		return diffs.x * diffs.x + diffs.y * diffs.y + diffs.z * diffs.z;
	}

	static float sqrDistanceBetween(const XMFLOAT2& _a, const XMFLOAT2& _b)
	{
		XMFLOAT2 diffs = _a - _b;
		return diffs.x * diffs.x + diffs.y * diffs.y;
	}

	static float distanceBetween(const XMFLOAT3& _a, const XMFLOAT3& _b)
	{
		return sqrtf(sqrDistanceBetween(_a, _b));
	}

	static float dotProduct(const XMFLOAT3& _a, const XMFLOAT3& _b)
	{
		float dot;
		XMStoreFloat(&dot, XMVector3Dot(XMLoadFloat3(&_a), XMLoadFloat3(&_b)));
		return dot;
	}

	static void crossProduct(const XMFLOAT3& _a, const XMFLOAT3& _b, XMFLOAT3& _OUT)
	{
		XMStoreFloat3(&_OUT, XMVector3Cross(XMLoadFloat3(&_a), XMLoadFloat3(&_b)));
	}

	static XMFLOAT3& crossProduct(const XMFLOAT3& _a, const XMFLOAT3& _b)
	{
		static XMFLOAT3 cross;
		XMStoreFloat3(&cross, XMVector3Cross(XMLoadFloat3(&_a), XMLoadFloat3(&_b)));
		return cross;
	}

	static XMFLOAT3& rightAxisOf(const XMFLOAT4X4& _m)
	{
		static XMFLOAT3 right;
		XMStoreFloat3(&right, XMLoadFloat3((XMFLOAT3*)&_m._11));
		return right;
	}

	static XMFLOAT3& upAxisOf(const XMFLOAT4X4& _m)
	{
		static XMFLOAT3 up;
		XMStoreFloat3(&up, XMLoadFloat3((XMFLOAT3*)&_m._21));
		return up;
	}

	static XMFLOAT3& forwardAxisOf(const XMFLOAT4X4& _m)
	{
		static XMFLOAT3 forward;
		XMStoreFloat3(&forward, XMLoadFloat3((XMFLOAT3*)&_m._31));
		return forward;
	}

	// this should be slightly faster than the other method
	static void rightAxisOf(const XMFLOAT4X4& _m, XMFLOAT3& _rightOut)
	{
		XMStoreFloat3(&_rightOut, XMLoadFloat3((XMFLOAT3*)&_m._11));
	}

	// this should be slightly faster than the other method
	static void upAxisOf(const XMFLOAT4X4& _m, XMFLOAT3& _upOut)
	{
		XMStoreFloat3(&_upOut, XMLoadFloat3((XMFLOAT3*)&_m._21));
	}

	// this should be slightly faster than the other method
	static void forwardAxisOf(const XMFLOAT4X4& _m, XMFLOAT3& _forwardOut)
	{
		XMStoreFloat3(&_forwardOut, XMLoadFloat3((XMFLOAT3*)&_m._31));
	}

	static float sqrMagnitudeOf(const XMFLOAT3& _f3)
	{
		return _f3.x * _f3.x + _f3.y * _f3.y + _f3.z * _f3.z;
	}

	static float sqrMagnitudeOf(const XMFLOAT2& _f2)
	{
		return _f2.x * _f2.x + _f2.y * _f2.y;
	}

	static float magnitudeOf(const XMFLOAT3& _f3)
	{
		return sqrtf(sqrMagnitudeOf(_f3));
	}

	static float magnitudeOf(const XMFLOAT2& _f2)
	{
		return sqrtf(sqrMagnitudeOf(_f2));
	}

	static float clamp(float _val, float _min, float _max)
	{
		float clampedVal;
		XMStoreFloat(&clampedVal, XMVectorClamp(XMLoadFloat(&_val), XMLoadFloat(&_min), XMLoadFloat(&_max)));
		return clampedVal;
	}

	static void clampValue(float& _val, const float& _min, const float& _max)
	{
		XMStoreFloat(&_val, XMVectorClamp(XMLoadFloat(&_val), XMLoadFloat(&_min), XMLoadFloat(&_max)));
	}

	static void setPosition(XMFLOAT4X4& _m, XMFLOAT3& _pos)
	{
		XMStoreFloat4((XMFLOAT4*)&_m._41, XMLoadFloat4(&XMFLOAT4(_pos.x, _pos.y, _pos.z, _m._44)));
	}

	static void setScale(XMFLOAT4X4& _m, XMFLOAT3& _sc)
	{
		_m._11 = _sc.x;
		_m._22 = _sc.y;
		_m._33 = _sc.z;
	}

	static void vectorRotationY(const XMFLOAT3& heading, const float rad_angle, XMFLOAT3& output)
	{
		XMFLOAT3X3 rotationMatrix;

		ZeroMemory(&rotationMatrix, sizeof(XMFLOAT3X3));

		rotationMatrix._11 = cosf(rad_angle);

		rotationMatrix._13 = sinf(rad_angle);

		rotationMatrix._22 = 1;

		rotationMatrix._31 = -sinf(rad_angle);

		rotationMatrix._33 = cosf(rad_angle);

		XMFLOAT3 product;
		XMStoreFloat3(&product, XMVector3Transform(XMLoadFloat3(&heading), XMLoadFloat3x3(&rotationMatrix)));
		output = product;
	}

	static XMFLOAT3 vectorRotationY(const XMFLOAT3& heading, const float rad_angle)
	{
		XMFLOAT3X3 rotationMatrix;

		ZeroMemory(&rotationMatrix, sizeof(XMFLOAT3X3));

		rotationMatrix._11 = cosf(rad_angle);

		rotationMatrix._13 = sinf(rad_angle);

		rotationMatrix._22 = 1;

		rotationMatrix._31 = -sinf(rad_angle);

		rotationMatrix._33 = cosf(rad_angle);

		XMFLOAT3 product;
		XMStoreFloat3(&product, XMVector3Transform(XMLoadFloat3(&heading), XMLoadFloat3x3(&rotationMatrix)));
		return product;
	}

	static XMFLOAT3X3 GetRotationMatrix(const XMFLOAT4X4& matrix)
	{
		XMFLOAT3X3 _m;

		XMStoreFloat3((XMFLOAT3*)&_m._11, XMLoadFloat3((XMFLOAT3*)&matrix._11));
		XMStoreFloat3((XMFLOAT3*)&_m._21, XMLoadFloat3((XMFLOAT3*)&matrix._21));
		XMStoreFloat3((XMFLOAT3*)&_m._31, XMLoadFloat3((XMFLOAT3*)&matrix._31));

		return _m;
	}

	static void SetToIdentity(XMFLOAT4X4& matrix)
	{
		XMStoreFloat4x4(&matrix, XMMatrixIdentity());
	}

	static XMFLOAT3 InToLocalSpace(const XMFLOAT3& float3, const XMFLOAT4X4& Local)
	{
		XMFLOAT3 result;

		//Putting the vector into the space of the matrix
		XMStoreFloat3(&result, XMVector3Transform(XMLoadFloat3(&float3), XMMatrixInverse(nullptr, XMLoadFloat4x4(&Local))));

		return result;
	}

	static std::ostream& operator<<(std::ostream& os, const XMFLOAT3& data)
	{
		os << " x: " << data.x << " y: " << data.y << " z: " << data.z << " ";
		return os;
	}

	static  std::wostream& operator<<(std::wostream& os, const XMFLOAT3& data)
	{
		os << " x: " << data.x << " y: " << data.y << " z: " << data.z << " ";
		return os;
	}
}
