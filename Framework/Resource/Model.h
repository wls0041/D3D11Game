#pragma once
#include "IResource.h"

class Model final : public IResource
{
public:
	Model(class Context *context);
	~Model();

	const bool SaveToFile(const std::string &path) override;
	const bool LoadFromFile(const std::string &path) override;

	auto GetRootActor() const -> const std::shared_ptr<class Actor>& { return root_actor; }
	void SetRootActor(std::shared_ptr<class Actor> &root_actor) { this->root_actor = root_actor; }


	/////////////////////////////////Material///////////////////////////////////////////////
	auto GetMaterials() const -> const std::vector<std::shared_ptr<Material>>& { return materials; }
	auto GetMaterial(const uint &index = 0) -> std::shared_ptr<Material>; //범위가 벗어나거나 하면 null이 나갈 수 도 있기 때문에 const 참조 쓰지 않음
	void AddMaterial(const std::shared_ptr<Material> &material, const std::shared_ptr<class Renderable> &renderable);
	void AddTexture(const std::shared_ptr<Material> &material, const TextureType &type, const std::string &path);

	///////////////////////////////////Mesh///////////////////////////////////////////////
	auto GetMeshes() const -> const std::vector<std::shared_ptr<Mesh>>& { return meshes; }
	auto GetMesh(const uint &index = 0)->std::shared_ptr<Mesh>;
	void AddMesh(const std::shared_ptr<Mesh> &mesh, const std::shared_ptr<class Renderable> &renderable);
	void AddMesh(
		const std::vector<VertexModel> &vertices,
		const std::vector<uint> &indices,
		const std::shared_ptr<class Renderable> &renderable
	);

	///////////////////////////////////MICS///////////////////////////////////////////////
	auto GetModelDirectory() const -> const std::string& { return model_directory; }
	auto GetMaterialDirectory() const -> const std::string& { return material_directory; }
	auto GetTextureDirectory() const -> const std::string& { return texture_directory; }
	auto GetMeshDirectory()  const -> const std::string& { return mesh_directory; }

private:
	void SetWorkingDirectories(const std::string& directory);

private:
	std::shared_ptr<class Actor> root_actor;
	
	std::vector<std::shared_ptr<Mesh>> meshes;
	std::vector<std::shared_ptr<Material>> materials;
	
	std::string model_directory;
	std::string material_directory;
	std::string texture_directory;
	std::string mesh_directory;

	float normalize_scale;
	bool is_animated;
};