#pragma once
#include "./Component/IComponent.h"

class Actor final
{
public:
	Actor(class Context *context);
	~Actor();

	void Initialize(class Transform* transform);

	void Start();
	void Update();
	void Stop();

	auto GetID() const -> const uint& { return actor_id; }

	auto GetName() const -> const std::string& { return name; }
	void SetName(const std::string& name) { this->name = name; }

	auto IsActive() const -> const bool& { return is_active; }
	void SetIsActive(const bool& is_active) { this->is_active = is_active; }

	auto GetTransform() const -> class Transform* { return transform; }
	auto GetRenderable() const -> class Renderable* { return renderable; }

	auto AddComponent(const ComponentType& type) -> std::shared_ptr<IComponent>;

	template <typename T> auto AddComponent() -> std::shared_ptr<T>;
	template <typename T> auto GetComponent() -> std::shared_ptr<T>;
	template <typename T> auto GetComponents() -> std::vector<std::shared_ptr<T>>;

	auto GetAllComponents() const -> const std::vector<std::shared_ptr<IComponent>>& { return components; }

	auto HasComponent(const ComponentType& type) -> const bool;
	template <typename T> auto HasComponent() -> const bool
	{
		return HasComponent(IComponent::DeduceComponentType<T>());
	}

	template <typename T> void RemoveComponent();

private:
	class Context* context;
	class Transform* transform; //가장 자주 불리는 데이터기 때문에 icomponent에 포함되어 있어도 거기서 계속 찾아오는 것이 느리기 때문에 따로 만들어 빼줌
	class Renderable *renderable;

	std::string name;
	uint actor_id;
	bool is_active;

	std::vector<std::shared_ptr<IComponent>> components;
};