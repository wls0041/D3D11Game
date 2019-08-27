#pragma once

enum class FileDialogType : uint
{
	Browser,
	FileSelection,
};

enum class FileDialogOpertation : uint
{
	Open,
	Load,
	Save,
};

enum class FileDialogFilter : uint
{
	All,
	Scene,
	Model,
	Texture,
};
class FileDialogItem final
{
public:
	FileDialogItem(const std::string& path, const Thumbnail& thumbnail)
		: path(path)
		, thumbnail(thumbnail)
	{
		id = GUID_Generator::Generate();
		label = FileSystem::GetFileNameFromPath(path);
		is_directory = FileSystem::IsDirectory(path);
	}
	~FileDialogItem() = default;

	auto GetPath() const -> const std::string& { return path; }
	auto GetLabel() const -> const std::string& { return label; }
	auto GetID() const -> const uint& { return id; }
	auto GetTexture() const -> const std::shared_ptr<Texture>& { return thumbnail.texture; }
	auto IsDirectory() const -> const bool& { return is_directory; }
	auto GetTimeSinceLastClickMs() const -> const float { return static_cast<float>(time_since_last_click.count()); }
	
    void Clicked()
    {
        const auto now          = std::chrono::high_resolution_clock::now();
        time_since_last_click   = now - last_click_time;
        last_click_time         = now;
    }

private:
	Thumbnail thumbnail;
	uint id;
	std::string path;
	std::string label;
	bool is_directory;
	std::chrono::duration<double, std::milli> time_since_last_click;
	std::chrono::time_point<std::chrono::high_resolution_clock> last_click_time;
};
class FileDialog final
{
public:
	FileDialog();
	~FileDialog() = default;

};