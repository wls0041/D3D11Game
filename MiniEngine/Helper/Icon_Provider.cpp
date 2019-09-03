#include "stdafx.h"
#include "Icon_Provider.h"

void Icon_Provider::Initialize(Context * context)
{
	this->context = context;

	auto directory = context->GetSubsystem<ResourceManager>()->GetAssetDirectory();

	Load(directory + "Icon/option_button.png", IconType::Component_Option);
	Load(directory + "Icon/component_Camera.png", IconType::Component_Camera);
	Load(directory + "Icon/component_Transform.png", IconType::Component_Transform);
	Load(directory + "Icon/component_renderable.png", IconType::Component_Renderable);
	Load(directory + "Icon/component_Script.png", IconType::Component_Renderable);
	Load(directory + "Icon/log_info.png", IconType::Console_Info);
	Load(directory + "Icon/log_warning.png", IconType::Console_Warning);
	Load(directory + "Icon/log_error.png", IconType::Console_Error);
	Load(directory + "Icon/big_play_button.png", IconType::Button_Play);
	Load(directory + "Icon/stopButton.png", IconType::Button_Stop);
	Load(directory + "Icon/pauseButton.png", IconType::Button_Pause);
	Load(directory + "Icon/folder.png", IconType::Thumbnail_Folder);
	Load(directory + "Icon/file.png", IconType::Thumbnail_File);
}

auto Icon_Provider::GetShaderResourceFromType(const IconType & type) -> ID3D11ShaderResourceView *
{
	return Load("", type)->texture->GetShaderResourceView();
}

auto Icon_Provider::GetShaderResourceFromPath(const std::string & path) -> ID3D11ShaderResourceView *
{
	return Load(path)->texture->GetShaderResourceView();;
}

auto Icon_Provider::GetShaderResourceFromThumbnail(const Thumbnail & thumbnail) -> ID3D11ShaderResourceView *
{
	for (const auto& temp : thumbnails)
	{
		if (temp.texture->GetResourceID() == thumbnail.texture->GetResourceID())
			return temp.texture->GetShaderResourceView();
	}
	return nullptr;
}

auto Icon_Provider::Load(const std::string & path, const IconType & type) -> Thumbnail *
{
	if (type != IconType::Thumbnail_Custom)
	{
		for (auto& thumbnail : thumbnails)
		{
			if (thumbnail.type == type)
				return &thumbnail;
		}
	}
	else
	{
		for (auto& thumbnail : thumbnails)
		{
			if (thumbnail.path == path)
				return &thumbnail;
		}
	}

	if (FileSystem::IsDirectory(path))
		return GetThumbnailFromType(IconType::Thumbnail_Folder);
	if (FileSystem::IsSupportTextureFile(path))
	{
		auto texture = std::make_shared<Texture2D>(context, false);
		texture->SetWidth(100);
		texture->SetHeight(100);

		context->GetSubsystem<Thread>()->AddTask([texture, path]()
		{
			texture->LoadFromFile(path);
		});

		return &thumbnails.emplace_back(type, texture, path);
	}

	return GetThumbnailFromType(IconType::Thumbnail_File);
}

void Icon_Provider::Image(const IconType & type, const float & size)
{
	ImGui::Image(GetShaderResourceFromType(type), ImVec2(size, size));
}

void Icon_Provider::Image(const IconType & type, const ImVec2 & size)
{
	ImGui::Image(GetShaderResourceFromType(type), ImVec2(size));
}

auto Icon_Provider::ImageButton(const IconType & type, const float & size) -> const bool
{
	bool is_pressed = ImGui::ImageButton
	(
		GetShaderResourceFromType(type),
		ImVec2(size, size)
	);

	return is_pressed;
}

auto Icon_Provider::ImageButton(const IconType & type, const ImVec2 & size) -> const bool
{
	bool is_pressed = ImGui::ImageButton
	(
		GetShaderResourceFromType(type),
		size
	);

	return is_pressed;
}

auto Icon_Provider::ImageButton(const Thumbnail & thumbnail, const float & size) -> const bool
{
	bool is_pressed = ImGui::ImageButton
	(
		GetShaderResourceFromThumbnail(thumbnail),
		ImVec2(size, size)
	);

	return is_pressed;
}

auto Icon_Provider::ImageButton(const Thumbnail & thumbnail, const ImVec2 & size) -> const bool
{
	bool is_pressed = ImGui::ImageButton
	(
		GetShaderResourceFromThumbnail(thumbnail),
		size
	);

	return is_pressed;
}

auto Icon_Provider::ImageButton(const std::string & path, const float & size) -> const bool
{
	bool is_pressed = ImGui::ImageButton
	(
		GetShaderResourceFromPath(path),
		ImVec2(size, size)
	);

	return is_pressed;
}

auto Icon_Provider::ImageButton(const std::string & path, const ImVec2 & size) -> const bool
{
	bool is_pressed = ImGui::ImageButton
	(
		GetShaderResourceFromPath(path),
		size
	);

	return is_pressed;
}

auto Icon_Provider::ImageButton(const char * id, const IconType & type, const float & size) -> const bool
{
	ImGui::PushID(id);
	const auto is_pressed = ImGui::ImageButton(GetShaderResourceFromType(type), ImVec2(size, size));
	ImGui::PopID();
	
	return is_pressed;
}

auto Icon_Provider::GetThumbnailFromType(const IconType & type) -> Thumbnail *
{
	for (auto& thumbnail : thumbnails)
	{
		if (thumbnail.type == type)
			return &thumbnail;
	}
	return nullptr;
}