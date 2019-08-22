#pragma once

class Editor_Helper final
{
public:
	static Editor_Helper &Get() {
		static Editor_Helper instance;
		return instance;
	}

	void Initialize(class Context *context);

	void LoadModel(const std::string &path);

public:
	class Context *context;
	class ResourceManager *resource_manager = nullptr;
	class SceneManager *scene_manager = nullptr;
	class Thread *thread = nullptr;
	class Renderer *renderer = nullptr;
	class Actor *select_actor = nullptr;

private:
	Editor_Helper() = default;
	~Editor_Helper() = default;
};