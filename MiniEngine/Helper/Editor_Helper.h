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
    class Context* context                  = nullptr;
    class ResourceManager* resource_manager = nullptr;
    class SceneManager* scene_manager       = nullptr;
    class Thread* thread                    = nullptr;
    class Renderer* renderer                = nullptr;

    std::weak_ptr<class Actor> select_actor;

private:
    Editor_Helper() = default;
    ~Editor_Helper() = default;
};

static inline ImVec2 operator+(const ImVec2& lhs, const ImVec2& rhs)
{
	return ImVec2(lhs.x + rhs.x, lhs.y + rhs.y);
}

static inline ImVec2 operator-(const ImVec2& lhs, const ImVec2& rhs)
{
	return ImVec2(lhs.x - rhs.x, lhs.y - rhs.y);
}