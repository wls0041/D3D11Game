#pragma once
#include "./Component/IComponent.h"

class Actor final : public std::enable_shared_from_this<Actor> //이렇게 하면 this*사용가능. 그냥 포인터(raw pointer)가 smart pointer를 인식 가능
{
public:
	Actor(class Context* context);
	~Actor();

	void Initialize(const std::shared_ptr<class Transform>& transform);

	void Start();
	void Update();
	void Stop();

	auto GetID() const -> const uint& { return actor_id; }

	auto GetName() const -> const std::string& { return name; }
	void SetName(const std::string& name) { this->name = name; }

	auto IsActive() const -> const bool& { return is_active; }
	void SetIsActive(const bool& is_active) { this->is_active = is_active; }

	auto GetTransform() const -> const std::shared_ptr<class Transform>& { return transform; }
	auto GetRenderable() const -> const std::shared_ptr<class Renderable>& { return renderable; }

	auto AddComponent(const ComponentType& type)->std::shared_ptr<IComponent>;

	template<typename T> auto AddComponent()->std::shared_ptr<T>;
	template<typename T> auto GetComponent()->std::shared_ptr<T>;
	template<typename T> auto GetComponents()->std::vector<std::shared_ptr<T>>;

	auto GetAllComponents() const -> const std::vector< std::shared_ptr<IComponent>>& { return components; }

	auto HasComponent(const ComponentType& type) -> const bool;

	template <typename T> auto HasComponent() -> const bool;
	template <typename T> void RemoveComponent();

private:
	class Context* context;
	std::shared_ptr<class Transform> transform; //가장 자주 불리는 데이터기 때문에 icomponent에 포함되어 있어도 거기서 계속 찾아오는 것이 느리기 때문에 따로 만들어 빼줌
	std::shared_ptr<class Renderable> renderable;

	std::string name;
	uint actor_id;
	bool is_active;

	std::vector<std::shared_ptr<IComponent>> components;
};

template<typename T>
inline auto Actor::AddComponent() -> std::shared_ptr<T>
{
	static_assert(std::is_base_of<IComponent, T>::value, "Provided type does not implement IComponent");

	auto type = IComponent::DeduceComponentType<T>();

	if (HasComponent(type) && type != ComponentType::Script)
		return GetComponent<T>();

	components.emplace_back
	(
		std::make_shared<T>
		(
			context,
			this,
			transform.get()
			)
	);

	auto new_component = std::static_pointer_cast<T>(components.back());
	new_component->SetComponentType(type);
	
	if constexpr (std::is_same<T, Renderable>::value) {
		renderable = new_component;
	}

	return new_component;
}

template<typename T>
inline auto Actor::GetComponent() -> std::shared_ptr<T>
{
	static_assert(std::is_base_of<IComponent, T>::value, "Provided type does not implement IComponent");

	auto type = IComponent::DeduceComponentType<T>();
	for (const auto& component : components)
	{
		if (component->GetComponentType() == type)
			return std::static_pointer_cast<T>(component);
	}
	return nullptr;
}

template<typename T>
inline auto Actor::GetComponents() -> std::vector<std::shared_ptr<T>>
{
	static_assert(std::is_base_of<IComponent, T>::value, "Provided type does not implement IComponent");

	auto type = IComponent::DeduceComponentType<T>();

	std::vector<std::shared_ptr<T>> temp_components;
	for (const auto& component : components)
	{
		if (component->GetComponentType() != type)
			continue;

		temp_components.emplace_back(std::static_pointer_cast<T>(component));
	}

	return temp_components;
}

template<typename T>
inline auto Actor::HasComponent() -> const bool
{
	static_assert(std::is_base_of<IComponent, T>::value, "Provided type does not implement IComponent");

	return HasComponent(IComponent::DeduceComponentType<T>());
}

template<typename T>
inline void Actor::RemoveComponent()
{
	static_assert(std::is_base_of<IComponent, T>::value, "Provided type does not implement IComponent");

	auto type = IComponent::DeduceComponentType<T>();

	for (auto iter = components.begin(); iter != components.end(); )
	{
		auto component = *iter;
		if (component->GetComponentType() == type)
		{
			component.reset();
			iter = components.erase(iter);
		}
		else
			iter++;
	}
}
