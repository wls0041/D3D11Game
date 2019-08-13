#pragma once

class Scene final
{
public:
	Scene(class Context* context);
	~Scene();

	Scene(const Scene&) = delete;
	Scene& operator=(const Scene&) = delete;

	void SaveToFile(const std::string& path);
	void LoadFromFile(const std::string& path);

	auto GetName() const -> const std::string& { return name; }
	void SetName(const std::string& name) { this->name = name; }

	auto GetAllActors() const -> const std::vector<std::shared_ptr<class Actor>>& { return actors; }
	auto GetActorCount() const -> const uint { return static_cast<uint>(actors.size()); }
	auto GetRoots() const -> const std::vector<std::shared_ptr<class Actor>>;
	auto GetActorFromID(const uint& id) const -> const std::shared_ptr<class Actor>;
	 
	auto CreateActor()->std::shared_ptr<class Actor>&;
	void AddActor(const std::shared_ptr<class Actor> &actor);
	void RemoveActor(const std::shared_ptr<class Actor> &actor);

	void Update();

private:
	class Context* context;
	class Renderer* renderer;

	std::string name;
	bool is_editor_mode;
	bool is_added;

	std::vector<std::shared_ptr<class Actor>> actors;
};