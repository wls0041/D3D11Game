#pragma once

enum class FileDialogType : uint
{
	Browser,
	FileSelection,
};

enum class FileDialogOperation : uint
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
	auto GetTimeSinceLastClickedMs() const -> const float { return static_cast<float>(time_since_last_click.count()); }
	
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
	FileDialog
	(
		class Context* context,
		const FileDialogType& type,
		const FileDialogOperation& operation,
		const FileDialogFilter& filter,
		const bool& is_windowed
	);
	~FileDialog() = default;

	auto GetType() const -> const FileDialogType& { return type; }
	auto GetFilter() const -> const FileDialogFilter& { return filter; }

	auto GetOperation() const -> const FileDialogOperation& { return operation; }
	void SetOperation(const FileDialogOperation& operation);

	auto Show(bool* is_visible, std::string* directory = nullptr, std::string* path = nullptr) -> const bool;

	void SetCallBackOnItemClick(const std::function<void(const std::string&)>& callback);
	void SetCallBackOnItemDoubleClick(const std::function<void(const std::string&)>& callback);

private:
	void ShowTop(bool* is_visible);
	void ShowMiddle();
	void ShowBottom(bool* is_visible);

	void ItemDrag(FileDialogItem* item) const;
	void ItemClick(FileDialogItem* item) const;
	void ItemMenu(FileDialogItem* item);

	auto SetDialogCurrentPath(const std::string& path) -> const bool;
	auto UpdateDialogFromDirectory(const std::string& path) -> const bool;
	void MenuPopup();

private:
	class Context* context;

	FileDialogType type;
	FileDialogOperation operation;
	FileDialogFilter filter;

	ImVec2 item_size;
	bool is_window;
	bool is_selection;
	bool is_update;

	std::string title;
	std::string current_directory;
	std::string input_box;

	std::vector<FileDialogItem> items;
	std::function<void(const std::string&)> on_item_clicked;
	std::function<void(const std::string&)> on_item_double_clicked;
};