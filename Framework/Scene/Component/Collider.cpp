#include "Framework.h"
#include "Collider.h"
#include "Transform.h"
#include "RigidBody.h"
#include "Renderable.h"
#include "Scene/Actor.h"
#include "Physics/BulletPhysicsHelper.h"

#pragma warning(push, 0)
#include <BulletCollision/CollisionShapes/btSphereShape.h>
#include <BulletCollision/CollisionShapes/btCylinderShape.h>
#include <BulletCollision/CollisionShapes/btCapsuleShape.h>
#include <BulletCollision/CollisionShapes/btStaticPlaneShape.h>
#include <BulletCollision/CollisionShapes/btConeShape.h>
#include <BulletCollision/CollisionShapes/btConvexHullShape.h>
#pragma warning(pop)

using namespace BulletPhysicsHelper;

Collider::Collider(Context * context, Actor * actor, Transform * transform)
	: IComponent(context, actor, transform)
	, collision_shape(nullptr)
	, shape_type(ColliderShapeType::Box)
	, size(Vector3::One)
	, center(Vector3::Zero)
	, vertex_limit(100'000)
	, optimize(true)
{
	if (auto renderable = actor->GetComponent<Renderable>())
	{
		center = Vector3::Zero;
		size = renderable->GetBoundBox().GetSize();
	}

	CollisionShape_Update();
}

Collider::~Collider()
{
	CollisionShape_Release();
}

void Collider::OnStart()
{
}

void Collider::OnUpdate()
{
}

void Collider::OnStop()
{
}

void Collider::SetSize(const Vector3 & size)
{
	if (this->size == size)
		return;

	this->size = size;
	this->size.x = Math::Clamp(this->size.x, std::numeric_limits<float>::epsilon(), std::numeric_limits<float>::infinity());
	this->size.y = Math::Clamp(this->size.y, std::numeric_limits<float>::epsilon(), std::numeric_limits<float>::infinity());
	this->size.z = Math::Clamp(this->size.z, std::numeric_limits<float>::epsilon(), std::numeric_limits<float>::infinity());

	CollisionShape_Update();
}

void Collider::SetCenter(const Vector3 & center)
{
	if (this->center == center)
		return;

	this->center = center;
	SetRigidBody_CenterOfMass(this->center);
}

void Collider::SetShapeType(const ColliderShapeType & type)
{
	if (this->shape_type == type)
		return;

	this->shape_type = type;
	CollisionShape_Update();
}

void Collider::SetIsOptimize(const bool & optimize)
{
	if (this->optimize == optimize)
		return;

	this->optimize = optimize;
	CollisionShape_Update();
}

void Collider::CollisionShape_Update()
{
	CollisionShape_Release();

	auto world_scale = transform->GetScale();

	switch (shape_type)
	{
	case ColliderShapeType::Box:
		collision_shape = new btBoxShape(ToBtVector3(size * 0.5f));
		collision_shape->setLocalScaling(ToBtVector3(world_scale));
		break;

	case ColliderShapeType::Sphere:
		collision_shape = new btSphereShape(size.x * 0.5f);
		collision_shape->setLocalScaling(ToBtVector3(world_scale));
		break;

	case ColliderShapeType::StaticPlane:
		collision_shape = new btStaticPlaneShape(btVector3(0.0f, 1.0f, 0.0f), 0.0f);
		break;

	case ColliderShapeType::Cylinder:
		collision_shape = new btCylinderShape(btVector3(size.x * 0.5f, size.y * 0.5f, size.z * 0.5f));
		collision_shape->setLocalScaling(ToBtVector3(world_scale));
		break;

	case ColliderShapeType::Capsule:
		collision_shape = new btCapsuleShape(size.x * 0.5f, Math::Max(size.y - size.x, 0.0f));
		collision_shape->setLocalScaling(ToBtVector3(world_scale));
		break;

	case ColliderShapeType::Cone:
		collision_shape = new btConeShape(size.x * 0.5f, size.y);
		collision_shape->setLocalScaling(ToBtVector3(world_scale));
		break;

	case ColliderShapeType::Mesh:
	{
		auto renderable = actor->GetComponent<Renderable>();
		auto mesh = renderable ? renderable->GetMesh() : nullptr;

		if (!renderable || !mesh)
		{
			LOG_ERROR("Can't construct mesh shape, there is no renderable component attached");
			return;
		}

		if (mesh->GetVertexCount() >= vertex_limit)
		{
			LOG_ERROR("No user defined collider with more than" + std::to_string(vertex_limit) + "vertices is allowed");
			return;
		}

		if (mesh->GetVertices().empty())
		{
			LOG_ERROR("No vertices");
			return;
		}

		collision_shape = new btConvexHullShape
		(
			reinterpret_cast<btScalar*>(mesh->GetVertexData()),
			mesh->GetVertexCount(),
			sizeof(VertexModel)
		);

		collision_shape->setLocalScaling(ToBtVector3(world_scale));

		if (optimize)
		{
			auto hull = reinterpret_cast<btConvexHullShape*>(collision_shape);
			hull->optimizeConvexHull();
			hull->initializePolyhedralFeatures();
		}
		break;
	}
	}

	collision_shape->setUserPointer(this);

	SetRigidBody_CollisionShape(collision_shape);
	SetRigidBody_CenterOfMass(center);
}

void Collider::CollisionShape_Release()
{
	SetRigidBody_CollisionShape(nullptr);
	SAFE_DELETE(collision_shape);
}

void Collider::SetRigidBody_CollisionShape(btCollisionShape * shape)
{
	if (const auto& rigid_body = actor->GetComponent<RigidBody>())
		rigid_body->SetShape(shape);
}

void Collider::SetRigidBody_CenterOfMass(const Vector3 & center)
{
	if (const auto& rigid_body = actor->GetComponent<RigidBody>())
		rigid_body->SetCenterOfMass(center);
}