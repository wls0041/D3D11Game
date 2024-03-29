#pragma once
#include "ISubsystem.h"
#include "../../Resource/IResource.h"

enum class AssetType : uint
{
	Shader,
	Texture,
	Icon,
	Model,
	Audio,
	Cubemap,
	Font,
	Script,
};

class ResourceManager final : public ISubsystem
{
public:
	ResourceManager(class Context* context);
	~ResourceManager();

	const bool Initialize() override;

	void SaveResourceToFile();
	void LoadResourceFromFile();

	auto GetTextureImporter() -> class TextureImporter* { return texture_importer.get(); }
	auto GetModelImporter() -> class ModelImporter* { return model_importer.get(); }

	template <typename T>
	auto Load(const std::string& path)->std::shared_ptr<T>;

	template <typename T>
	auto GetResourceFromName(const std::string& name)->std::shared_ptr<T>;

	template <typename T>
	auto GetResourceFromPath(const std::string& path)->std::shared_ptr<T>;

	template <typename T>
	void RegisterResource(std::shared_ptr<T>& resource);
	void RegisterDirectory(const AssetType& asset_type, const std::string& directory);

	auto HasResource(const std::string& resource_name, const ResourceType& resource_type) -> const bool;

	auto GetProjectDirectory() const -> const std::string& { return project_directory; }
	void SetProjectDirectory(const std::string& directory);

	auto GetAssetDirectory() const -> const std::string { return std::string("../../_Assets/"); }
	auto GetAssetDirectory(const AssetType& asset_type) -> const std::string&;

private:
	typedef std::vector<std::shared_ptr<IResource>> resource_group;
	std::map<ResourceType, resource_group> resource_groups;
	std::mutex resource_mutex;

	std::map<AssetType, std::string> resource_directories;
	std::string project_directory;

	std::shared_ptr<class TextureImporter> texture_importer;
	std::shared_ptr<class ModelImporter> model_importer;
};

template<typename T>
inline auto ResourceManager::Load(const std::string & path) -> std::shared_ptr<T>
{
	static_assert(std::is_base_of<IResource, T>::value, "Provided type does not implement IResource");

	if (!FileSystem::IsExistFile(path))
	{
		LOG_ERROR_F("Path \"%s\" is invalid", path.c_str());
		return nullptr;
	}

	auto relative_path = FileSystem::GetRelativeFromPath(path);
	auto resource_name = FileSystem::GetIntactFileNameFromPath(relative_path);

	if (HasResource(resource_name, IResource::DeduceResourceType<T>()))
		return GetResourceFromName<T>(resource_name);

	auto resource = std::make_shared<T>(context);
	resource->SetResourceName(resource_name);
	resource->SetResourcePath(relative_path);

	RegisterResource<T>(resource);

	if (!resource->LoadFromFile(relative_path))
	{
		LOG_ERROR_F("Failed to load \"%s\"", relative_path.c_str());
		return nullptr;
	}

	return resource;
}

template<typename T>
inline auto ResourceManager::GetResourceFromName(const std::string & name) -> std::shared_ptr<T>
{
	static_assert(std::is_base_of<IResource, T>::value, "Provided type does not implement IResource");
	for (auto& resource : resource_groups[IResource::DeduceResourceType<T>()])
	{
		if (resource->GetResourceName() == name)
			return std::static_pointer_cast<T>(resource);
	}

	return nullptr;
}

template<typename T>
inline auto ResourceManager::GetResourceFromPath(const std::string & path) -> std::shared_ptr<T>
{
	static_assert(std::is_base_of<IResource, T>::value, "Provided type does not implement IResource");
	for (auto& resource : resource_groups[IResource::DeduceResourceType<T>()])
	{
		if (resource->GetResourcePath() == path)
			return std::static_pointer_cast<T>(resource);
	}

	return nullptr;
}

template<typename T>
inline void ResourceManager::RegisterResource(std::shared_ptr<T>& resource)
{
	static_assert(std::is_base_of<IResource, T>::value, "Provided type does not implement IResource");

	if (!resource)
		return;

	std::lock_guard<std::mutex> guard(resource_mutex);
	resource_groups[resource->GetResourceType()].emplace_back(resource);
}
