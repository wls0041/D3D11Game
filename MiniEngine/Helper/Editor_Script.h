#pragma once

enum class ScriptLanguage : uint
{
	C,
	CPlusPlus,
	AngelScript,
	HLSL
};

class Editor_Script final
{
public:
	static const std::string Base_Script_Code;

public:
	static Editor_Script& Get()
	{
		static Editor_Script instance;
		return instance;
	}

	void Initialize(class Context* context);
	void Render();

	void SetScript(const std::string& path);
	void SetScriptLanguage(const ScriptLanguage& language);

	auto IsVisible() const -> const bool& { return is_visible; }
	void SetVisible(const bool& is_visible) { this->is_visible = is_visible; }

private:
	Editor_Script() = default;
	~Editor_Script() = default;

	void CreateNewScript();
	void OpenScript(const std::string& path);
	void SaveToScript();
	void SaveAsToScript();

private:
	class Context* context;
	std::string path;
	std::string new_name;
	bool is_visible;
};