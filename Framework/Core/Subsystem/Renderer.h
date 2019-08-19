#pragma once
#include "ISubsystem.h"

enum RenderOption : uint //enum은 중복에 주의해야 함
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
	//RTT. 속도가 느려지지만 이를 감수할 정도로 좋은 효과를 줄 수 있다.
	//즉시 렌더링. 빠르지만 효과 적용이 힘듦. (특히 조명)
	//지연 렌더링. 렌더링 방식은 같지만 효과 계산을 뒤로 미룸. 특히 조명 계산.
	//원래 방식은 찍으면 모든 색을 한번에 찍었기 때문에 찍기 전 모든 계산을 마쳐야 함.
	//지연 문맥을 쓰면 조명 계산을 뒤로 빼기 때문에 조명을 제외한 이미지를 찍고 나중에 조명을 넣음.
	//일단 화면을 찍고 효과를 줄 필요가 없는 부분은 계산을 생략할 수 있음.
	//단. 그려진 걸 뽑기 때문에 투명객체가 그려지지 않음. 안티에일리어싱 불가

	std::shared_ptr<class Texture> final_render_texture;
	
	//Shaders
	std::map<std::string, std::shared_ptr<class Shader>> shaders;
	
	//Actors
	std::unordered_map<RenderableType, std::vector<class Actor*>> renderables;
};