#pragma once

enum class ComponentType : uint
{
	Unknown,
	Camera,
	Skybox,
	Transform,
	Renderable,
	Script,
	Terrain,
	Light,
	Collider,
	RigidBody,
	Constraint,
};

struct Attribute
{
	std::function<std::any()> getter; //괄호 붙어있지만 함수 x
	std::function<void(std::any)> setter;
};

class IComponent 
{
public:
	template <typename T>
	static constexpr ComponentType DeduceComponentType();

public:
	IComponent
	(
		class Context* context,
		class Actor* actor,
		class Transform* transform
	);
	virtual ~IComponent() = default;

	virtual void OnStart() = 0;
	virtual void OnUpdate() = 0;
	virtual void OnStop() = 0;

	auto GetID() const -> const uint& { return id; }
	auto GetActor() const -> class Actor* { return actor; }
	auto GetActorName() const -> const std::string;
	auto GetTransform() const -> class Transform* { return transform; }

	auto GetComponentType() const -> const ComponentType& { return component_type; }
	void SetComponentType(const ComponentType& type) { this->component_type = type; }

	auto IsEnabled() const -> const bool& { return is_enabled; }
	void SetEnabled(const bool &is_enabled) { this->is_enabled = is_enabled; }

	auto GetAttributes() const -> const std::vector<Attribute>& { return attributes; }
	void SetAttributes(const std::vector<Attribute> &attributes);

protected:
	void RegisterAttribute(std::function<std::any()> &&getter, std::function<void(std::any)> &&setter);

protected:
	class Context* context;
	class Actor* actor;
	class Transform* transform;

	ComponentType component_type;
	bool is_enabled;

private:
	uint id;
	std::vector<Attribute> attributes;
};
