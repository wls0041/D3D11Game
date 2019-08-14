#include "Framework.h"
#include "Actor.h"
#include "Scene.h"
#include "./Component/Camera.h"
#include "./Component/Renderable.h"
#include "./Component/Transform.h"

Actor::Actor(Context * context)
	: context(context)
	, transform(nullptr)
	, name("")
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

	for (auto component : components)
		component->OnStart();
}

void Actor::Update()
{
	if (!is_active)
		return;

	for (auto component : components)
		component->OnUpdate();
}

void Actor::Stop()
{
	if (!is_active)
		return;

	for (auto component : components)
		component->OnStop();
}

auto Actor::AddComponent(const ComponentType & type) -> std::shared_ptr<IComponent>
{
	std::shared_ptr<IComponent> component;

	switch (type)
	{
	case ComponentType::Camera:         component = AddComponent<Camera>();         break;
	case ComponentType::Skybox:         break;//component = AddComponent<Skybox>();         break;
	case ComponentType::Transform:      component = AddComponent<Transform>();      break;
	case ComponentType::Renderable:     component = AddComponent<Renderable>();     break;
	case ComponentType::Script:         break;//component = AddComponent<Script>();         break;
	case ComponentType::Terrain:        break;//component = AddComponent<Terrain>();         break;
	case ComponentType::Light:          break;//component = AddComponent<Light>();          break;
	case ComponentType::Collider:       break;//component = AddComponent<Collider>();       break;
	case ComponentType::RigiBody:		break;//component = AddComponent<RigiBody>();       break;
	case ComponentType::Constrain:      break;//component = AddComponent<Constrain>();       break;
	}

	return component;
}

auto Actor::HasComponent(const ComponentType & type) -> const bool
{
	for (auto component : components)
	{
		if (component->GetComponentType() == type)
			return true;
	}
	return false;
}
