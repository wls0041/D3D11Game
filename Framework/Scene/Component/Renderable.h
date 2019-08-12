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
	~Renderable();

	Renderable(const Renderable&) = delete;
	Renderable& operator=(const Renderable&) = delete;

	void OnStart() override;
	void OnUpdate() override;
	void OnStop() override;

	auto GetMaterial()->std::shared_ptr<class Material> const { return material; }
	void SetMaterial(std::shared_ptr<class Material> material) { this->material = material; }
	void SetMaterial(const std::string& path);
	void SetStandardMaterial();

	auto GetMesh()->std::shared_ptr<class Mesh> const { return mesh; }
	void SetMesh(std::shared_ptr<class Mesh> mesh) { this->mesh = mesh; }
	void SetMesh(const std::string& path);
	void SetStandardMesh(const MeshType& type);
	
private:
	std::shared_ptr<class Material> material;
	std::shared_ptr<class Mesh> mesh;
};