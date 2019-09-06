#pragma once
#include "IComponent.h"

enum class ForceType : uint
{
	Force,
	Impulse,
};

class RigidBody final : public IComponent
{
public:
	RigidBody
	(
		class Context* context,
		class Actor* actor,
		class Transform* transform
	);
	~RigidBody() = default;

	void OnStart() override;
	void OnUpdate() override;
	void OnStop() override;

	auto GetMass() const -> const float& { return mass; }
	void SetMass(const float& mass);

	auto GetFriction() const -> const float& { return friction; }
	void SetFriction(const float& friction);

	auto GetFrictionRolling() const -> const float& { return friction_rolling; }
	void SetFrictionRolling(const float& friction_rolling);

	auto GetRestitution() const -> const float& { return restitution; }
	void SetRestitution(const float& restitution);

	auto IsUseGravity() const -> const bool& { return is_use_gravity; }
	void SetUseGravity(const bool& is_use_gravity);

	auto IsKinematic() const -> const bool& { return is_kinematic; }
	void SetKinematic(const bool& is_kinematic);

	auto IsHasSimulated() const -> const bool& { return has_simulated; }
	void SetHasSimulated(const bool& has_simulated) { this->has_simulated = has_simulated; }

	auto GetGravity() const -> const Vector3& { return gravity; }
	void SetGravity(const Vector3& gravity);

	auto GetCenterOfMass() const -> const Vector3& { return center_of_mass; }
	void SetCenterOfMass(const Vector3& center_of_mass);

	auto GetPosition() const -> const Vector3;
	void SetPosition(const Vector3& position);

	auto GetRotation() const -> const Quaternion;
	void SetRotation(const Quaternion& rotation);

	auto GetPositionLock() const -> const Vector3& { return position_lock; }
	void SetPositionLock(const Vector3& lock);
	void SetPositionLock(const bool& lock);

	auto GetRotationLock() const -> const Vector3& { return rotation_lock; }
	void SetRotationLock(const Vector3& lock);
	void SetRotationLock(const bool& lock);

	void Activate() const;

private:
	void AddToWorld();

private:
	class Physics* physics;
	class btRigidBody* rigid_body;
	class btCollisionShape* collision_shape;

	Vector3 gravity;
	Vector3 position_lock;
	Vector3 rotation_lock;
	Vector3 center_of_mass;

	float mass;
	float friction;
	float friction_rolling;
	float restitution;
	bool is_use_gravity;
	bool is_kinematic; //가해지는 힘을 제거한 정적에너지
	bool is_in_world;
	bool has_simulated;

	std::vector<class Constraint*> constraints;
};