#pragma once
#include "ISubsystem.h"

enum class RenderableType : uint
{
	Opaque,
	Transparent,
	Camera,
};

class Renderer final : public ISubsystem
{
public:
	Renderer(class Context *context);
	~Renderer();

	const bool Initialize() override;

	auto GetFrameResource() -> ID3D11ShaderResourceView*;
	
	void AcquireRenderables(class Scene* scene);

	void Render();

private:
	void PassMain();

private:
	class Graphics *graphics;

	std::shared_ptr<class Camera> camera;
	std::shared_ptr<class CommandList> command_list;
	std::shared_ptr<class Texture> render_target;

	std::unordered_map<RenderableType, std::vector<class Actor*>> renderables;

};