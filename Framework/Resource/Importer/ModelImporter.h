#pragma once

class ModelImporter final
{
public:
	ModelImporter(class Context *context);
	~ModelImporter();

	auto Load(class Model *model, const std::string &path) -> const bool;

private:
	class Context*context;
	class SceneManager *scene_manager;

	std::string model_path;
	int assimp_flags;

};