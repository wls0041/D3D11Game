#include "Framework.h"
#include "Model.h"
#include "../Scene/Actor.h"
#include "../Scene/Component/Renderable.h"
#include "../Scene/Component/Transform.h"
#include "./Importer/ModelImporter.h"

Model::Model(Context * context)
	: IResource(context, ResourceType::Model)
	, normalize_scale(1.0f)
	, is_animated(false)
{
}

Model::~Model()
{
}

const bool Model::SaveToFile(const std::string & path)
{
	return false;
}

const bool Model::LoadFromFile(const std::string & path)
{
	auto resource_manager = context->GetSubsystem<ResourceManager>();

	SetWorkingDirectories(resource_manager->GetProjectDirectory() + FileSystem::GetIntactFileNameFromPath(path));
																										  //색같이 변경이 필요한 데이터는 xml로 할 수 있다.
	SetResourcePath(model_directory + FileSystem::GetIntactFileNameFromPath(path) + MODEL_BIN_EXTENSION); //fbx는 모든 데이터를 가지고 있고, 한 번 실행 후에는 필요한 데이터만 뽑아서 바이너리로 따로 저장하고 그걸 불러와서 속도 향상
	SetResourceName(FileSystem::GetIntactFileNameFromPath(path));

	if (context->GetSubsystem<ResourceManager>()->GetModelImporter()->Load(this, path))
	{
		//root_actor->GetTransform()->SetScale(Vector3(0.0075f, 0.0075f, 0.0075f));
		//root_actor->GetTransform()->UpdateTransform();

		SaveToFile(GetResourcePath());
		return true;
	}
	return false;
}

auto Model::GetMaterial(const uint & index) -> std::shared_ptr<Material>
{
	if (index >= static_cast<uint>(materials.size())) {
		LOG_ERROR("Invlaid Parameter, Out of Range");
		return nullptr;
	}
	return materials[index];
}

void Model::AddMaterial(const std::shared_ptr<Material>& material, const std::shared_ptr<class Renderable>& renderable)
{
	if (!material || !renderable)
	{
		LOG_ERROR("Invalid parameter");
		return;
	}

	material->SetResourcePath(material_directory + material->GetResourceName() + MATERIAL_BIN_EXTENSION);
	material->SaveToFile(material->GetResourcePath());
	materials.emplace_back(material);

	renderable->SetMaterial(material);
}

void Model::AddTexture(const std::shared_ptr<Material>& material, const TextureType & type, const std::string & path)
{
	if (!material || path == NOT_ASSIGNED_STR)
	{
		LOG_ERROR("Invalid parameter");
		return;
	}

	const auto texture_name = FileSystem::GetIntactFileNameFromPath(path);
	auto texture = context->GetSubsystem<ResourceManager>()->GetResourceFromName<Texture2D>(texture_name);

	if (texture)
		material->SetTexture(type, texture);
	else
	{
		texture = std::make_shared<Texture2D>(context);
		texture->LoadFromFile(path);

		texture->SetResourcePath(texture_directory + texture_name + TEXTURE_BIN_EXTENSION);
		texture->SetResourceName(texture_name);
		texture->SaveToFile(texture->GetResourcePath());
		texture->ClearMipChain();

		material->SetTexture(type, texture);
	}
}

auto Model::GetMesh(const uint & index) -> std::shared_ptr<Mesh>
{
	if (index >= static_cast<uint>(meshes.size())) {
		LOG_ERROR("Invlaid Parameter, Out of Range");
		return nullptr;
	}
	return meshes[index];
}

void Model::AddMesh(const std::shared_ptr<Mesh>& mesh, const std::shared_ptr<class Renderable>& renderable)
{
	if (!mesh || !renderable) {
		LOG_ERROR("Invalid Parameter");
		return;
	}

	mesh->SetResourcePath(mesh_directory + mesh->GetResourceName() + MESH_BIN_EXTENSION);
	mesh->SaveToFile(mesh->GetResourcePath());
	meshes.emplace_back(mesh);

	renderable->SetMesh(mesh);
}

void Model::AddMesh(const std::vector<VertexModel>& vertices, const std::vector<uint>& indices, const std::shared_ptr<class Renderable>& renderable)
{
	if (vertices.empty() || indices.empty() || !renderable)
	{
		LOG_ERROR("Invalid parameter");
		return;
	}

	auto mesh = std::make_shared<Mesh>(context);
	mesh->SetVertices(vertices);
	mesh->SetIndices(indices);
	mesh->CreateBuffers();
	//TODO : set mesh name

	AddMesh(mesh, renderable);
}

void Model::SetWorkingDirectories(const std::string & directory)
{
	model_directory = directory + "/";
	material_directory = model_directory + "Material/";
	mesh_directory = model_directory + "Mesh/";
	texture_directory = model_directory + "Texture/";

	FileSystem::Create_Directory(model_directory);
	FileSystem::Create_Directory(material_directory);
	FileSystem::Create_Directory(mesh_directory);
	FileSystem::Create_Directory(texture_directory);
}
