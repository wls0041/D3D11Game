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
	Render_Option_Debug_Physics = 1U << 7,
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

enum class ShaderType : uint
{
	VS_GBUFFER,
	VS_POST_PROCESS,
	PS_TEXTURE,
	PS_DEBUG_NORMAL,
	PS_DEBUG_DEPTH,
	PS_DEBUG_LIGHT,
	PS_DIRECTIONAL_LIGHT,
	PS_SPOT_LIGHT,
	PS_POINT_LIGHT,
	PS_COMPOSITION,
	PS_GAMMA_CORRECTION,
	VPS_COLOR,
};

enum class RenderTargetType : uint
{
	GBuffer_Albedo,
	GBuffer_Normal,
	GBuffer_Material,
	GBuffer_Velocity,
	GBuffer_Depth,
	Light_Diffuse,
	Light_Specular,
	Composition,
	Composition_Previous,
	Final,
};

enum class RenderableType : uint
{
	Opaque,
	Transparent,
	Camera,
	Light,
	Directional_Light,
	Spot_Light,
	Point_Light,
};

class Renderer final : public ISubsystem
{
public:
	Renderer(class Context *context);
	~Renderer();

	const bool Initialize() override;

	auto GetCamera() -> class Camera* { return camera.get(); }

	auto GetFrameResource()->ID3D11ShaderResourceView*;

	auto GetEditorOffset() const -> const Vector2& { return editor_offset; }
	void SetEditorOffset(const Vector2 &offset) { this->editor_offset = offset; }

	auto GetResolution() const -> const Vector2& { return resolution; }
	void SetResolution(const uint &width, const uint &height);

	auto IsReverseZ() const -> const bool& { return is_reverse_z; }
	void SetReverseZ(const bool& is_reverse_z) { this->is_reverse_z = is_reverse_z; }

	auto GetDebugBufferType() const -> const RenderBufferType& { return debug_buffer_type; }
	void SetDebugBufferType(const RenderBufferType& type) { this->debug_buffer_type = type; }

	void AcquireRenderables(class Scene* scene);
	void SortRenderables(std::vector<class Actor*> *actors);

	void DrawLine(
		const Vector3 &from,
		const Vector3 &to,
		const Color4 &from_color = Color4(0.41f, 0.86f, 1.0f, 1.0f),
		const Color4 &to_color = Color4(0.41f, 0.86f, 1.0f, 1.0f),
		const bool &is_depth = true
	);

	void Render();

private:
	void CreateRenderTextures();
	void CreateShaders();
	void CreateConstantBuffers();
	void CreateSamplerStates();
	void CreateRasterzierStates();
	void CreateBlendStates();
	void CreateDepthStencilStates();

	void UpdateGlobalBuffer(const uint& width, const uint& height, const Matrix& world_view_proj = Matrix::Identity);

	auto GetClearDepth() const -> const float { return is_reverse_z ? 0.0f : 1.0f; }
	auto GetRasterizerState(const D3D11_CULL_MODE& cull_mode, const D3D11_FILL_MODE& fill_mode = D3D11_FILL_SOLID) -> const std::shared_ptr<class RasterizerState>&;


private:
	void PassMain();

	void PassGBuffer();
	void PassLight();
	void PassComposition(std::shared_ptr<class Texture>& out);
	void PassPostProcess(std::shared_ptr<class Texture>& in, std::shared_ptr<class Texture>& out);

	void PassLine(std::shared_ptr<class Texture> &out);
	void PassDebug(std::shared_ptr<class Texture> &out);

	void PassGammaCorrection(std::shared_ptr<class Texture>& in, std::shared_ptr<class Texture>& out);

private:
	class Graphics *graphics;

	std::shared_ptr<class Grid> grid;
	std::shared_ptr<class VertexBuffer> screen_vertex_buffer;
	std::shared_ptr<class IndexBuffer> screen_index_buffer;

	std::shared_ptr<class Camera> camera;
	std::shared_ptr<class CommandList> command_list;

	std::vector<struct VertexColor> depth_enabled_line_vertices;
	std::vector<struct VertexColor> depth_disabled_line_vertices;
	std::shared_ptr<class VertexBuffer> line_vertex_buffer;

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
	
	Vector2 resolution = Vector2(1280, 720);
	Vector2 editor_offset = Vector2::Zero;

	//Constant Buffer
	std::shared_ptr<class ConstantBuffer> global_buffer;

	//Sampler State
	std::shared_ptr<class SamplerState> compare_depth;
	std::shared_ptr<class SamplerState> point_clamp;
	std::shared_ptr<class SamplerState> bilinear_clamp;
	std::shared_ptr<class SamplerState> bilinear_wrap;
	std::shared_ptr<class SamplerState> trilinear_clamp;
	std::shared_ptr<class SamplerState> anisotropic_wrap;

	//Rasterizer State
	std::shared_ptr<class RasterizerState> cull_back_solid;
	std::shared_ptr<class RasterizerState> cull_front_solid;
	std::shared_ptr<class RasterizerState> cull_none_solid;
	std::shared_ptr<class RasterizerState> cull_back_wireframe;
	std::shared_ptr<class RasterizerState> cull_front_wireframe;
	std::shared_ptr<class RasterizerState> cull_none_wireframe;

	//Blend State
	std::shared_ptr<class BlendState> blend_enabled;
	std::shared_ptr<class BlendState> blend_disabled;
	std::shared_ptr<class BlendState> blend_color_add;
	std::shared_ptr<class BlendState> blend_bloom;

	//Depth Stencil State
	std::shared_ptr<class DepthStencilState> depth_stencil_enabled_state;
	std::shared_ptr<class DepthStencilState> depth_stencil_disabled_state;

	//Render Textures
	std::map<RenderTargetType, std::shared_ptr<class Texture>> render_textures;
	//RTT. �ӵ��� ���������� �̸� ������ ������ ���� ȿ���� �� �� �ִ�.
	//��� ������. �������� ȿ�� ������ ����. (Ư�� ����)
	//���� ������. ������ ����� ������ ȿ�� ����� �ڷ� �̷�. Ư�� ���� ���.
	//���� ����� ������ ��� ���� �ѹ��� ����� ������ ��� �� ��� ����� ���ľ� ��.
	//���� ������ ���� ���� ����� �ڷ� ���� ������ ������ ������ �̹����� ��� ���߿� ������ ����.
	//�ϴ� ȭ���� ��� ȿ���� �� �ʿ䰡 ���� �κ��� ����� ������ �� ����.
	//��. �׷��� �� �̱� ������ ����ü�� �׷����� ����. ��Ƽ���ϸ���� �Ұ�

	//Shaders
	std::map<ShaderType, std::shared_ptr<class Shader>> shaders;

	//Actors
	std::unordered_map<RenderableType, std::vector<class Actor*>> renderables;
};