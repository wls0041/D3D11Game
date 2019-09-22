#pragma once

enum class IconType : uint
{
	Component_Camera,
	Component_Transform,
	Component_Renderable,
	Component_RigidBody,
	Component_Collider,
	Component_Material,
	Component_Script,
	Console_Info,
	Console_Warning,
	Console_Error,
	Button_Option,
	Button_Play,
	Button_Stop,
	Button_Pause,
	Button_Remove,
	Thumbnail_Folder,
	Thumbnail_File,
	Thumbnail_Custom,
};

struct Thumbnail
{
    Thumbnail() = default;
    Thumbnail(const IconType& type, std::shared_ptr<Texture> texture, const std::string& path)
        : type(type), texture(texture), path(path)
    {}
    
    IconType type;
    std::shared_ptr<Texture> texture;
    std::string path;
};

class Icon_Provider final
{
public:
    static Icon_Provider& Get()
    {
        static Icon_Provider instance;
        return instance;
    }

    void Initialize(class Context* context);

    auto GetShaderResourceFromType(const IconType& type) -> ID3D11ShaderResourceView*;
    auto GetShaderResourceFromPath(const std::string& path)->ID3D11ShaderResourceView*;
    auto GetShaderResourceFromThumbnail(const Thumbnail& thumbnail)->ID3D11ShaderResourceView*;

    auto Load(const std::string& path, const IconType& type = IconType::Thumbnail_Custom) -> Thumbnail*;

	void Image(const IconType& type, const float& size);
	void Image(const IconType& type, const ImVec2& size);

    auto ImageButton(const IconType& type, const float& size) -> const bool;
    auto ImageButton(const IconType& type, const ImVec2& size) -> const bool;
    auto ImageButton(const Thumbnail& thumbnail, const float& size) -> const bool;
    auto ImageButton(const Thumbnail& thumbnail, const ImVec2& size) -> const bool;
    auto ImageButton(const std::string& path, const float& size) -> const bool;
    auto ImageButton(const std::string& path, const ImVec2& size) -> const bool;

	auto ImageButton(const char *id, const IconType &type, const float &size) -> const bool;

private:
    Icon_Provider() = default;
    ~Icon_Provider() = default;

    auto GetThumbnailFromType(const IconType& type) -> Thumbnail*;

private:
    class Context* context;
    std::vector<Thumbnail> thumbnails;
};