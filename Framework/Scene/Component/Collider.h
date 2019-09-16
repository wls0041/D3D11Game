#pragma once
#include "IComponent.h"

enum class ColliderShapeType : uint
{
	Box,
	Sphere,
	StaticPlane,
	Cylinder,
	Capsule,
	Cone,
	Mesh
};

class Collider final : public IComponent
{
public:
	Collider
	(
		class Context* context,
		class Actor* actor,
		class Transform* transform
	);
	~Collider();

	void OnStart() override;
	void OnUpdate() override;
	void OnStop() override;

	auto GetSize() const -> const Vector3& { return size; }
	void SetSize(const Vector3& size);

	auto GetCenter() const -> const Vector3& { return center; }
	void SetCenter(const Vector3& center);

	auto GetShapeType() const -> const ColliderShapeType& { return shape_type; }
	void SetShapeType(const ColliderShapeType& type);

	auto GetShape() const -> class btCollisionShape* { return collision_shape; }

	auto IsOptimize() const -> const bool& { return optimize; }
	void SetIsOptimize(const bool& optimize);

private:
	void CollisionShape_Update();
	void CollisionShape_Release();

	void SetRigidBody_CollisionShape(btCollisionShape* shape);
	void SetRigidBody_CenterOfMass(const Vector3& center);

private:
	class btCollisionShape* collision_shape;
	ColliderShapeType shape_type;
	Vector3 size;
	Vector3 center;
	uint vertex_limit;
	bool optimize;
};