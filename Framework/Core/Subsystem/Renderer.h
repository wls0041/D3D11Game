#pragma once
#include "ISubsystem.h"

enum RenderOption : uint //enum�� �ߺ��� �����ؾ� ��
{
	Render_Option_Bloom		 = 1U << 0,
	Render_Option_FXAA		 = 1U << 1, // Fast Approximate Anti Aliasing
	Render_Option_SSAO		 = 1U << 2, // Screen Space Ambient Occlusion
	Render_Option_TAA		 = 1U << 3, // Temporal Anti Aliasing
	Render_Option_Sharpening = 1U << 4,
	Render_Option_MotionBlur = 1U << 5,
	Render_Option_SSR		 = 1U << 6,
};

enum class ToneMappingType : uint //������ ĳ��������� �ؼ� ����
{
	Off,
	ACES,
	Reinhard,
	Uncharted2,
};

enum RenderBufferType : uint
{
	Render_Buffer_None,
	Render_Buffer_Albedo,
	Render_Buffer_Normal,
	Render_Buffer_Material,
	Render_Buffer_Diffuse,
	Render_Buffer_Specular,
	Render_Buffer_Velocity,
	Render_Buffer_Depth,
	Render_Buffer_SSAO, //screen space ambiance oclusion. ĳ���Ͱ� �������� �Ϳ� ���� �׸���
	Render_Buffer_SSR, //screen space reflection. ȭ�� ��ü �ݻ�(ex.��)
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
	void SortRenderables(std::vector<class Actor*> *actors);

	void Render();

private:
	void CreateRenderTextures();
	void CreateShaders();

private:
	void PassMain();

private:
	class Graphics *graphics;

	std::shared_ptr<class Camera> camera;
	std::shared_ptr<class CommandList> command_list;
	std::shared_ptr<class Texture> render_target;

private:
	//Core
	Matrix post_process_view;
	Matrix post_process_proj;
	Matrix post_process_view_proj;

	Matrix camera_view;
	Matrix camera_proj;
	Matrix camera_view_proj;
	Matrix camera_view_proj_inverse;
	Vector3 camera_position;
	float camera_near = 0.0f;
	float camera_far = 0.0f;
	
	RenderBufferType debug_buffer_type = RenderBufferType::Render_Buffer_None;
	uint render_flags = 0;
	bool is_initialized = false;
	bool is_reverse_z = true;
	std::atomic<bool> is_acquire_renderables = false;
	
	Vector2 resolution;

	//Render Textures
	//RTT. �ӵ��� ���������� �̸� ������ ������ ���� ȿ���� �� �� �ִ�.
	//��� ������. �������� ȿ�� ������ ����. (Ư�� ����)
	//���� ������. ������ ����� ������ ȿ�� ����� �ڷ� �̷�. Ư�� ���� ���.
	//���� ����� ������ ��� ���� �ѹ��� ����� ������ ��� �� ��� ����� ���ľ� ��.
	//���� ������ ���� ���� ����� �ڷ� ���� ������ ������ ������ �̹����� ��� ���߿� ������ ����.
	//�ϴ� ȭ���� ��� ȿ���� �� �ʿ䰡 ���� �κ��� ����� ������ �� ����.
	//��. �׷��� �� �̱� ������ ����ü�� �׷����� ����. ��Ƽ���ϸ���� �Ұ�

	std::shared_ptr<class Texture> final_render_texture;
	
	//Shaders
	std::map<std::string, std::shared_ptr<class Shader>> shaders;
	
	//Actors
	std::unordered_map<RenderableType, std::vector<class Actor*>> renderables;
};