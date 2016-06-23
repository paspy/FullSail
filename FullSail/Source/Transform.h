#pragma once
#include "Math.h"

using namespace DirectX;

class Transform {
public:
	Transform(void);
	Transform(const Transform& rhs);

	~Transform(){}

	DirectX::XMFLOAT4X4& GetWorldMatrix(void);

	const DirectX::XMFLOAT4X4& GetLocalMatrix(void)const;
	DirectX::XMFLOAT4X4& GetLocalMatrix(void);

	void SetLocalMatrix(const DirectX::XMFLOAT4X4& matrix);

	Transform* GetParent(void);
	const Transform* GetParent(void)const;

	void SetParent(Transform* _parent);

	void DirtyTransform(void);

	bool wasModified(void);

	template<typename Func>
	static void ForEachTransformInHierarchy(Transform* transform, Func func) {
		func(transform);

		for (transform = transform->firstChild; transform != 0; transform = transform->nextSibling)
			ForEachTransformInHierarchy(transform, func);
	}

	Transform* clone(void)const {
		return new Transform(*this);
	}
	void SetChild(Transform* _child);

	void Translate(XMFLOAT3& offset, bool local = false);

	void Rotate(XMFLOAT3& offset, bool local = false);

	void Scale(const XMFLOAT3& offset, bool local = false);

private:
	void SetSibling(Transform* _sibling);
	void RemoveSibling(Transform* _sibling);
	Transform* parent;

	Transform* child;
	Transform* sibling;

	DirectX::XMFLOAT4X4 worldMatrix;
	DirectX::XMFLOAT4X4 localMatrix;

	void RemoveChild(Transform* child);
	void OnDestroy(void);

	int dirtied : 1;
	int modified : 1;
};