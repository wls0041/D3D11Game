#pragma once
#include "IComponent.h"

class Renderable final : public IComponent
{
public:
	Renderable
	(
		class Context* context,
		class Actor* actor,
		class Transform* transform
	);
	~Renderable() = default;

	void OnStart() override;
	void OnUpdate() override;
	void OnStop() override;

	auto GetMaterial() const -> const std::shared_ptr<Material>& { return material; }
	void SetMaterial(const std::shared_ptr<Material>& material) { this->material = material; }
	void SetMaterial(const std::string& path);
	void SetStandardMaterial();

	auto GetMesh() const -> const std::shared_ptr<Mesh>& { return mesh; }
	void SetMesh(const std::shared_ptr<Mesh>& mesh) { this->mesh = mesh; }
	void SetMesh(const std::string& path);
	void SetStandardMesh(const MeshType& type);

private:
	std::shared_ptr<Material> material;
	std::shared_ptr<Mesh> mesh;
};