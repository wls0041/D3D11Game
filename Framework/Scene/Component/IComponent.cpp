#include "Framework.h"
#include "IComponent.h"
#include "Camera.h"
#include "Transform.h"
#include "Renderable.h"

IComponent::IComponent(Context * context, Actor * actor, Transform * transform)
	: context(context)
	, actor(actor)
	, transform(transform)
	, component_type(ComponentType::Unknown)
	, is_enabled(true)
{
	id = GUID_Generator::Generate();
}

void IComponent::SetAttributes(const std::vector<Attribute>& attributes)
{
	for (uint i = 0; i < static_cast<uint>(this->attributes.size()); i++)
		this->attributes[i].setter(attributes[i].getter());
}

void IComponent::RegisterAttribute(std::function<std::any()>&& getter, std::function<void(std::any)>&& setter)
{
	Attribute attribute;
	attribute.getter = std::move(getter); //move - rvalue reference를 lvalue reference로 캐스팅 해줌
	attribute.setter = std::move(setter);

	attributes.emplace_back(attribute);
}

template<typename T>
constexpr ComponentType IComponent::DeduceComponentType()
{
	return ComponentType::Unknown;
}

#define REGISTER_COMPONENT_TYPE(T, enum_type) template<> ComponentType IComponent::DeduceComponentType<T>() { return enum_type; }

REGISTER_COMPONENT_TYPE(Camera, ComponentType::Camera)
REGISTER_COMPONENT_TYPE(Transform, ComponentType::Transform)
REGISTER_COMPONENT_TYPE(Renderable, ComponentType::Renderable)