#include "Framework.h"
#include "RigidBody.h"
#include "Transform.h"
#include "Collider.h"
#include "Scene/Actor.h"
#include "Physics/BulletPhysicsHelper.h"

#pragma warning(push, 0)
#include <BulletDynamics/Dynamics/btRigidBody.h>
#include <BulletCollision/CollisionShapes/btCollisionShape.h>
#include <BulletDynamics/Dynamics/btDiscreteDynamicsWorld.h>
#pragma warning(pop)

using namespace BulletPhysicsHelper;

static const float DEFAULT_MASS = 0.0f;
static const float DEFAULT_FRICTION = 0.5f;
static const float DEFAULT_FRICTION_ROLLING = 0.0f;
static const float DEFAULT_RESTITUTION = 0.0f;
static const float DEFAULT_DEACTIVATION_TIME = 2000.0f;

class MotionState final : public btMotionState
{
public:
	MotionState(RigidBody* rigid_body)
		: rigid_body(rigid_body)
	{}

	void getWorldTransform(btTransform & worldTrans) const override
	{
		auto last_position = rigid_body->GetTransform()->GetTranslation();
		auto last_rotation = rigid_body->GetTransform()->GetRotation();

		worldTrans.setOrigin(ToBtVector3(last_position + last_rotation * rigid_body->GetCenterOfMass()));
		worldTrans.setRotation(ToBtQuaternion(last_rotation));

		rigid_body->SetHasSimulated(true);
	}

	void setWorldTransform(const btTransform & worldTrans) override
	{
		auto new_rotation = ToQuaternion(worldTrans.getRotation());
		auto new_position = ToVector3(worldTrans.getOrigin()) - new_rotation * rigid_body->GetCenterOfMass();

		rigid_body->GetTransform()->SetTranslation(new_position);
		rigid_body->GetTransform()->SetRotation(new_rotation);

		rigid_body->SetHasSimulated(true);
	}

private:
	RigidBody* rigid_body;
};

RigidBody::RigidBody(Context * context, Actor * actor, Transform * transform)
	: IComponent(context, actor, transform)
	, rigid_body(nullptr)
	, collision_shape(nullptr)
	, gravity(0.0f)
	, position_lock(0.0f)
	, rotation_lock(0.0f)
	, center_of_mass(0.0f)
	, mass(DEFAULT_MASS)
	, friction(DEFAULT_FRICTION)
	, friction_rolling(DEFAULT_FRICTION_ROLLING)
	, restitution(DEFAULT_RESTITUTION)
	, is_use_gravity(true)
	, is_kinematic(false)
	, is_in_world(false)
	, has_simulated(false)
{
	physics = context->GetSubsystem<Physics>();

	AcquireShape();
	AddToWorld();
}

RigidBody::~RigidBody()
{
	Release();
}

void RigidBody::OnStart()
{
	Activate();
}

void RigidBody::OnUpdate()
{
	if (!Engine::IsFlagsEnabled(EngineFlags_Game))
		SetPosition(transform->GetTranslation());
}

void RigidBody::OnStop()
{
}

void RigidBody::SetMass(const float & mass)
{
	auto temp_mass = Math::Max(mass, 0.0f);
	if (temp_mass != this->mass)
	{
		this->mass = temp_mass;
		AddToWorld();
	}
}

void RigidBody::SetFriction(const float & friction)
{
	if (!rigid_body || this->friction == friction)
		return;

	this->friction = friction;
	rigid_body->setFriction(this->friction);
}

void RigidBody::SetFrictionRolling(const float & friction_rolling)
{
	if (!rigid_body || this->friction_rolling == friction_rolling)
		return;

	this->friction_rolling = friction_rolling;
	rigid_body->setRollingFriction(this->friction_rolling);
}

void RigidBody::SetRestitution(const float & restitution)
{
	if (!rigid_body || this->restitution == restitution)
		return;

	this->restitution = restitution;
	rigid_body->setRestitution(this->restitution);
}

void RigidBody::SetUseGravity(const bool & is_use_gravity)
{
	if (this->is_use_gravity == is_use_gravity)
		return;

	this->is_use_gravity = is_use_gravity;
	AddToWorld();
}

void RigidBody::SetKinematic(const bool & is_kinematic)
{
	if (this->is_kinematic == is_kinematic)
		return;

	this->is_kinematic = is_kinematic;
	AddToWorld();
}

void RigidBody::SetGravity(const Vector3 & gravity)
{
	if (this->gravity == gravity)
		return;

	this->gravity = gravity;
	AddToWorld();
}

void RigidBody::SetCenterOfMass(const Vector3 & center_of_mass)
{
	this->center_of_mass = center_of_mass;
	SetPosition(GetPosition());
}

auto RigidBody::GetPosition() const -> const Vector3
{
	if (rigid_body)
	{
		const btTransform& transform = rigid_body->getWorldTransform();
		return ToVector3(transform.getOrigin()) - ToQuaternion(transform.getRotation()) * center_of_mass;
	}

	return Vector3::Zero;
}

void RigidBody::SetPosition(const Vector3 & position)
{
	if (!rigid_body)
		return;

	btTransform& worldTrans = rigid_body->getWorldTransform();
	worldTrans.setOrigin(ToBtVector3(position + ToQuaternion(worldTrans.getRotation()) * center_of_mass));

	//게임 실행중에 위치 업데이트 불가
	if (!has_simulated && physics->IsSimulating())
	{
		btTransform interp_transform = rigid_body->getInterpolationWorldTransform();
		interp_transform.setOrigin(worldTrans.getOrigin());
		rigid_body->setInterpolationWorldTransform(interp_transform);
	}

	Activate();
}

auto RigidBody::GetRotation() const -> const Quaternion
{
	return rigid_body ? ToQuaternion(rigid_body->getWorldTransform().getRotation()) : Quaternion::Identity;
}

void RigidBody::SetRotation(const Quaternion & rotation)
{
	if (!rigid_body)
		return;

	Vector3 old_position = GetPosition();
	btTransform& worldTrans = rigid_body->getWorldTransform();
	worldTrans.setRotation(ToBtQuaternion(rotation));

	if (center_of_mass != Vector3::Zero)
		worldTrans.setOrigin(ToBtVector3(old_position + rotation * center_of_mass));

	if (!has_simulated && physics->IsSimulating())
	{
		btTransform interp_transform = rigid_body->getInterpolationWorldTransform();
		interp_transform.setRotation(worldTrans.getRotation());

		if (center_of_mass != Vector3::Zero)
			interp_transform.setOrigin(worldTrans.getOrigin());

		rigid_body->setInterpolationWorldTransform(interp_transform);
	}

	rigid_body->updateInertiaTensor();
	Activate();
}

void RigidBody::SetPositionLock(const Vector3 & lock)
{
	if (!rigid_body || position_lock == lock)
		return;

	this->position_lock = lock;

	auto linear_factor = Vector3(!lock.x, !lock.y, !lock.z);
	rigid_body->setLinearFactor(ToBtVector3(linear_factor));
}

void RigidBody::SetPositionLock(const bool & lock)
{
	if (lock)
		SetPositionLock(Vector3::One);
	else
		SetPositionLock(Vector3::Zero);
}

void RigidBody::SetRotationLock(const Vector3 & lock)
{
	if (!rigid_body || rotation_lock == lock)
		return;

	this->rotation_lock = lock;

	auto angle_factor = Vector3(!lock.x, !lock.y, !lock.z);
	rigid_body->setAngularFactor(ToBtVector3(angle_factor));
}

void RigidBody::SetRotationLock(const bool & lock)
{
	if (lock)
		SetRotationLock(Vector3::One);
	else
		SetRotationLock(Vector3::Zero);
}

void RigidBody::SetLinearVelocity(const Vector3 & velocity) const
{
	if (!rigid_body)
		return;

	rigid_body->setLinearVelocity(ToBtVector3(velocity));
	if (velocity != Vector3::Zero)
		Activate();
}

void RigidBody::SetAngularVelocity(const Vector3 & velocity) const
{
	if (!rigid_body)
		return;

	rigid_body->setAngularVelocity(ToBtVector3(velocity));
	if (velocity != Vector3::Zero)
		Activate();
}

void RigidBody::ApplyForce(const Vector3 & force, const ForceType & force_type)
{
	if (!rigid_body)
		return;

	Activate();

	switch (force_type)
	{
	case ForceType::Force:      rigid_body->applyCentralForce(ToBtVector3(force));      break;
	case ForceType::Impulse:    rigid_body->applyCentralImpulse(ToBtVector3(force));    break;
	}
}

void RigidBody::ApplyForceAtPosition(const Vector3 & force, const Vector3 & position, const ForceType & force_type)
{
	if (!rigid_body)
		return;

	Activate();

	switch (force_type)
	{
	case ForceType::Force:      rigid_body->applyForce(ToBtVector3(force), ToBtVector3(position));      break;
	case ForceType::Impulse:    rigid_body->applyImpulse(ToBtVector3(force), ToBtVector3(position));    break;
	}
}

void RigidBody::ApplyTorque(const Vector3 & torque, const ForceType & force_type)
{
	if (!rigid_body)
		return;

	Activate();

	switch (force_type)
	{
	case ForceType::Force:      rigid_body->applyTorque(ToBtVector3(torque));           break;
	case ForceType::Impulse:    rigid_body->applyTorqueImpulse(ToBtVector3(torque));    break;
	}
}

void RigidBody::Activate() const
{
	if (!rigid_body)
		return;

	if (mass > 0.0f)
		rigid_body->activate(true);
}

void RigidBody::Deactivate() const
{
	if (!rigid_body)
		return;

	rigid_body->setActivationState(WANTS_DEACTIVATION);
}

void RigidBody::ClearForces() const
{
	if (!rigid_body)
		return;

	rigid_body->clearForces();
}

void RigidBody::SetShape(btCollisionShape * shape)
{
	collision_shape = shape;
	if (collision_shape)
		AddToWorld();
	else
		RemoveFromWorld();
}

void RigidBody::AddToWorld()
{
	if (mass < 0.0f)
		mass = 0.0f;

	btVector3 local_inertia = btVector3(0, 0, 0);
	if (collision_shape && rigid_body)
	{
		local_inertia = rigid_body->getLocalInertia();
		collision_shape->calculateLocalInertia(mass, local_inertia);
	}

	Release();

	auto motion_state = new MotionState(this);

	btRigidBody::btRigidBodyConstructionInfo construction_info(mass, motion_state, collision_shape, local_inertia);
	construction_info.m_mass = mass;
	construction_info.m_friction = friction;
	construction_info.m_rollingFriction = friction_rolling;
	construction_info.m_restitution = restitution;
	construction_info.m_collisionShape = collision_shape;
	construction_info.m_localInertia = local_inertia;
	construction_info.m_motionState = motion_state;

	rigid_body = new btRigidBody(construction_info);
	rigid_body->setUserPointer(this);

	//TODO : Constraint

	UpdateKinematic();
	UpdateGravity();

	SetPosition(transform->GetTranslation());
	SetRotation(transform->GetRotation());

	SetPositionLock(position_lock);
	SetRotationLock(rotation_lock);

	physics->GetWorld()->addRigidBody(rigid_body);
	if (mass > 0.0f)
		Activate();
	else
	{
		SetLinearVelocity(Vector3::Zero);
		SetAngularVelocity(Vector3::Zero);
	}

	has_simulated = false;
	is_in_world = true;
}

void RigidBody::Release()
{
	if (!rigid_body)
		return;

	//TODO : Remove Constraints

	RemoveFromWorld();

	rigid_body = nullptr;
}

void RigidBody::RemoveFromWorld()
{
	if (!rigid_body)
		return;

	if (is_in_world)
	{
		physics->GetWorld()->removeRigidBody(rigid_body);
		delete rigid_body->getMotionState();
		delete rigid_body;

		rigid_body = nullptr;
		is_in_world = false;
	}
}

void RigidBody::AcquireShape()
{
	if (const auto& collider = actor->GetComponent<Collider>())
	{
		collision_shape = collider->GetShape();
		center_of_mass = collider->GetCenter();
	}
}

void RigidBody::UpdateKinematic()
{
	int flags = rigid_body->getCollisionFlags();

	if (is_kinematic)
		flags |= btCollisionObject::CF_KINEMATIC_OBJECT;
	else
		flags &= ~btCollisionObject::CF_KINEMATIC_OBJECT;

	rigid_body->setCollisionFlags(flags);
	rigid_body->forceActivationState(is_kinematic ? DISABLE_DEACTIVATION : ISLAND_SLEEPING);
	rigid_body->setDeactivationTime(DEFAULT_DEACTIVATION_TIME);
}

void RigidBody::UpdateGravity()
{
	int flags = rigid_body->getFlags();

	if (is_use_gravity)
		flags &= ~BT_DISABLE_WORLD_GRAVITY;
	else
		flags |= BT_DISABLE_WORLD_GRAVITY;

	rigid_body->setFlags(flags);

	if (is_use_gravity)
	{
		btVector3 gravity = ToBtVector3(physics->GetGravity());
		rigid_body->setGravity(gravity);
	}
	else
		rigid_body->setGravity(btVector3(0, 0, 0));
}

auto RigidBody::IsActivated() const -> const bool
{
	if (!rigid_body)
		return false;

	return rigid_body->isActive();
}
