#include "Framework.h"
#include "Actor.h"
#include "Scene.h"
#include "Component/Camera.h"
#include "Component/Renderable.h"
#include "Component/Transform.h"

Actor::Actor(Context * context)
	: context(context)
	, name(NOT_ASSIGNED_STR)
	, is_active(true)
{
	actor_id = GUID_Generator::Generate();
}

Actor::~Actor()
{
	components.clear();
	components.shrink_to_fit();
}

void Actor::Initialize(const std::shared_ptr<class Transform>& transform)
{
	this->transform = transform;
}

void Actor::Start()
{
	if (!is_active)
		return;

	for (const auto& component : components)
		component->OnStart();
}

void Actor::Update()
{
	if (!is_active)
		return;

	for (const auto& component : components)
		component->OnUpdate();
}

void Actor::Stop()
{
	if (!is_active)
		return;

	for (const auto& component : components)
		component->OnStop();
}

auto Actor::AddComponent(const ComponentType & type) -> std::shared_ptr<IComponent>
{
	std::shared_ptr<IComponent> component;

	switch (type)
	{
	case ComponentType::Camera:     component = AddComponent<Camera>();     break;
	case ComponentType::Skybox:     break;
	case ComponentType::Transform:  component = AddComponent<Transform>();  break;
	case ComponentType::Renderable: component = AddComponent<Renderable>(); break;
	case ComponentType::Script:     break;
	case ComponentType::Terrain:    break;
	case ComponentType::Light:      break;
	case ComponentType::Collider:   break;
	case ComponentType::RigidBody:  break;
	case ComponentType::Constraint: break;
	}

	return component;
}

auto Actor::HasComponent(const ComponentType & type) -> const bool
{
	for (const auto& component : components)
	{
		if (component->GetComponentType() == type)
			return true;
	}
	return false;
}
