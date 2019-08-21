#include "Framework.h"
#include "ResourceManager.h"
#include "Resource/Importer/TextureImporter.h"
#include "Resource/Importer/ModelImporter.h"

ResourceManager::ResourceManager(Context * context)
	: ISubsystem(context)
{
	std::string asset_directory = GetAssetDirectory();

	RegisterDirectory(AssetType::Shader,	asset_directory + "Shader/");
	RegisterDirectory(AssetType::Texture,	asset_directory + "Texture/");
	RegisterDirectory(AssetType::Icon,		asset_directory + "Icon/");
	RegisterDirectory(AssetType::Model,		asset_directory + "Model/");
	RegisterDirectory(AssetType::Audio,		asset_directory + "Audio/");
	RegisterDirectory(AssetType::Cubemap,	asset_directory + "Cubemap/");
	RegisterDirectory(AssetType::Font,		asset_directory + "Font/");
	RegisterDirectory(AssetType::Script,	asset_directory + "Script/");

	SetProjectDirectory("../../_Project/");
}

ResourceManager::~ResourceManager()
{
}

const bool ResourceManager::Initialize()
{
	texture_importer = std::make_shared<TextureImporter>(context);
	model_importer = std::make_shared<ModelImporter>(context);
	
	return true;
}

void ResourceManager::SaveResourceToFile()
{
}

void ResourceManager::LoadResourceFromFile()
{
}

void ResourceManager::RegisterDirectory(const AssetType & asset_type, const std::string & directory)
{
	resource_directories[asset_type] = directory;
}

auto ResourceManager::HasResource(const std::string & resource_name, const ResourceType & resource_type) -> const bool
{
	if (resource_name == NOT_ASSIGNED_STR) {
		LOG_ERROR("Invalid parameter");
		return false;
	}

	for (const auto &resource : resource_groups[resource_type]) {
		if (resource->GetResourceName() == resource_name) return true;
	}
	return false;
}

void ResourceManager::SetProjectDirectory(const std::string & directory)
{
	if (!FileSystem::IsExistDirectory(directory))
		FileSystem::Create_Directory(directory);

	project_directory = directory;
}

auto ResourceManager::GetAssetDirectory(const AssetType & asset_type) -> const std::string &
{
	for (auto &directory : resource_directories) {
		if (directory.first == asset_type) return directory.second;
	}

	return NOT_ASSIGNED_STR;
}
