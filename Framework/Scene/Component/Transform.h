#pragma once
#include "IComponent.h"

class Transform final : public IComponent
{
public:
	Transform(
		class Context* context,
		class Actor* actor,
		class Transform* transform
	);
	~Transform() = default;

	void OnStart() override;
	void OnUpdate() override;
	void OnStop() override;
	
	///////////////////Local///////////////////
	auto GetLocalScale()    const -> const Vector3& { return local_scale; }
	auto GetLocalRotation() const -> const Quaternion& { return local_rotation; }
	auto GetLocalTranslation() const -> const Vector3& { return local_translation; }

	void SetLocalScale(const Vector3& scale);
	void SetLocalRotation(const Quaternion& rotation);
	void SetLocalTranslation(const Vector3& translation);

	auto GetLocalMatrix() const -> const Matrix& { return local; }

	//////////////////World///////////////////

	auto GetScale()    -> const Vector3 { return world.GetScale(); }
	auto GetRotation() -> const Quaternion { return world.GetRotation(); }
	auto GetTranslation() -> const Vector3 { return world.GetTranslation(); }

	void SetScale(const Vector3& scale);
	void SetRotation(const Quaternion& rotation);
	void SetTranslation(const Vector3& translation);

	auto GetWorldMatrix() const -> const Matrix& { return world; }
	auto GetWorldRotationMatrix() -> const Matrix;

	auto GetRight() -> const Vector3;
	auto GetUp() -> const Vector3;
	auto GetForward() -> const Vector3;

	//////////////////Hierarchy//////////////////
	auto GetRoot() -> Transform* { return HasParent() ? parent->GetRoot() : this; }
	auto GetParent() -> Transform* { return parent; }
	void SetParent(Transform *transform);

	auto GetChilds() const -> const std::vector<Transform*>& { return childs; }
	auto GetChildFromIndex(const uint &index) -> Transform*;
	auto GetChildCount() const -> const uint { return static_cast<uint>(childs.size()); }
	void AddChild(Transform *child);

	void DetachChild();
	void AcquireChild();

	auto IsRoot() const -> const bool { return !HasParent(); }
	auto IsDescendant(Transform *transform) const -> const bool;

	auto HasParent() const -> const bool { return parent ? true : false; }
	auto HasChilds() const -> const bool { return !childs.empty(); }

	void UpdateTransform();

	///////////////Constant Buffer//////////////////
	void UpdateConstantBuffer(const Matrix &view_proj);
	auto GetConstantBuffer() const -> const std::shared_ptr<class ConstantBuffer>&{ return gpu_buffer; }
private:
	Vector3 local_scale;
	Vector3 local_translation;
	Quaternion local_rotation;

	Matrix local;
	Matrix world;

	Transform *parent;
	std::vector<Transform*> childs;

	///////////////Constant Buffer//////////////////
	Matrix wvp_previous;
	TRANSFORM_DATA cpu_buffer;
	std::shared_ptr<class ConstantBuffer> gpu_buffer;
};