#pragma once
#include "ISubsystem.h"

enum RendererOption : uint //enum은 중복에 주의해야 함
{
	Render_Option_Bloom		 = 1U << 0,
	Render_Option_FXAA		 = 1U << 1, // Fast Approximate Anti Aliasing
	Render_Option_SSAO		 = 1U << 2, // Screen Space Ambient Occlusion
	Render_Option_TAA		 = 1U << 3, // Temporal Anti Aliasing
	Render_Option_Sharpening = 1U << 4,
	Render_Option_MotionBlur = 1U << 5,
	Render_Option_SSR		 = 1U << 6,
};

enum class ToneMappingType : uint //일일히 캐스팅해줘야 해서 안전
{
	Off,
	ACES,
	Reinhard,
	Uncharted2,
};

enum RendererBufferType : uint
{
	Render_Buffer_None,
	Render_Buffer_Albedo,
	Render_Buffer_Normal,
	Render_Buffer_Material,
	Render_Buffer_Diffuse,
	Render_Buffer_Specular,
	Render_Buffer_Velocity,
	Render_Buffer_Depth,
	Render_Buffer_SSAO, //screen space ambiance oclusion. 캐릭터가 가려지는 것에 의한 그림자
	Render_Buffer_SSR, //screen space reflection. 화면 전체 반사(ex.구)
	Render_Buffer_Bloom,
	Render_Buffer_Shadows,
};

enum ShaderType : uint
{
	Shader_GBuffer_VS,
};

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

private:
	Matrix camera_view;
	Matrix camera_proj;
	Matrix camera_view_proj;
	Matrix camera_view_proj_inverse;
	Vector3 camera_position;
	float camera_near;
	float camera_far;

	Matrix post_process_view;
	Matrix post_process_proj;
	Matrix post_process_view_proj;

	uint render_flags;
	Vector2 resolution;

	std::map<ShaderType, std::shared_ptr<class Shader>> shaders;
	std::unordered_map<RenderableType, std::vector<class Actor*>> renderables;
};