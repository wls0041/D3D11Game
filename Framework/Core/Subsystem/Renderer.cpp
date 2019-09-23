#include "Framework.h"
#include "Renderer.h"
#include "../D3D11/CommandList.h"
#include "Gizmo/Grid.h"
#include "../../Scene/Scene.h"
#include "../../Scene/Actor.h"
#include "../../Scene/Component/Camera.h"
#include "../../Scene/Component/Transform.h"
#include "../../Scene/Component/Renderable.h"
#include "Scene/Component/Light.h"

Renderer::Renderer(Context * context)
	: ISubsystem(context)
{
	SUBSCRIBE_TO_EVENT(EventType::Render, EVENT_HANDLER(Render));
}

Renderer::~Renderer()
{
}

const bool Renderer::Initialize()
{
	graphics			= context->GetSubsystem<Graphics>();
	grid				= std::make_unique<Grid>(context);
	command_list		= std::make_shared<CommandList>(context);
	line_vertex_buffer  = std::make_shared<VertexBuffer>(context);
	
	CreateRenderTextures();
	CreateShaders();
	CreateConstantBuffers();
	CreateSamplerStates();
	CreateRasterzierStates();
	CreateBlendStates();
	CreateDepthStencilStates();

	return true;
}

auto Renderer::GetFrameResource() -> ID3D11ShaderResourceView *
{
	auto render_target = render_textures[RenderTargetType::Final];
	return render_target ? render_target->GetShaderResourceView() : nullptr;
}

void Renderer::SetResolution(const uint &width, const uint &height)
{
	if (width == 0 || height == 0) {
		LOG_WARNING_F("%Dx%d is an invalid resolution", width, height);
		return;
	}

	if (resolution.x == width && resolution.y == height) return;

	resolution.x = static_cast<float>((width % 2) != 0 ? width - 1 : width);
	resolution.y = static_cast<float>((height % 2) != 0 ? height - 1 : height);

	CreateRenderTextures();

	LOG_INFO_F("Resolution set to %dx%d", width, height);
}

void Renderer::AcquireRenderables(Scene * scene)
{
	while (is_acquire_renderables)
		LOG_WARNING_F("Waiting previous operation to finish ...");

	is_acquire_renderables = true;

	renderables.clear();
	camera = nullptr; //camera.reset();

	auto actors = scene->GetAllActors();
	for (const auto &actor : actors)
	{
		if (!actor) continue;

		auto renderable_component = actor->GetComponent<Renderable>();
		auto light_component = actor->GetComponent<Light>();
		auto camera_component = actor->GetComponent<Camera>();

		auto component_renderable = actor->GetComponent<Renderable>();
		auto component_camera = actor->GetComponent<Camera>();

		if (component_renderable)
			renderables[RenderableType::Opaque].emplace_back(actor.get());

		if (light_component)
		{
			renderables[RenderableType::Light].emplace_back(actor.get());

			switch (light_component->GetLightType())
			{
			case LightType::Directional:    renderables[RenderableType::Directional_Light].emplace_back(actor.get());   break;
			case LightType::Point:          renderables[RenderableType::Point_Light].emplace_back(actor.get());         break;
			case LightType::Spot:           renderables[RenderableType::Spot_Light].emplace_back(actor.get());          break;
			}
		}

		if (component_camera) { //scene_camera 용
			renderables[RenderableType::Camera].emplace_back(actor.get());
			camera = component_camera;
		}
	}
	is_acquire_renderables = false;
}

void Renderer::SortRenderables(std::vector<class Actor*>* actors)
{
	if (!camera || renderables.size() <= 2ULL) return;

	auto render_hash = [this](Actor *actor) {
		auto renderable = actor->GetRenderable();
		if (!renderable) return 0.0f;

		const auto material = renderable->GetMaterial();
		if (!material) return 0.0f;
		
		const float num_depth = 0.0f; //TODO : Boundbox 만들고 처리
		const auto num_material  = static_cast<float>(material->GetResourceID());

		return std::stof(std::to_string(num_depth) + "-" + std::to_string(num_material));
	};

	std::sort(actors->begin(), actors->end(), [&render_hash](Actor *lhs, Actor *rhs) {
		return render_hash(lhs) < render_hash(rhs);
	});
}

void Renderer::DrawLine(const Vector3 & from, const Vector3 & to, const Color4 & from_color, const Color4 & to_color, const bool & is_depth)
{
	if (is_depth) {
		depth_enabled_line_vertices.emplace_back(from, from_color);
		depth_enabled_line_vertices.emplace_back(to, to_color);
	}
	else{
		depth_disabled_line_vertices.emplace_back(from, from_color);
		depth_disabled_line_vertices.emplace_back(to, to_color);
	}
}

void Renderer::Render()
{
	if (!camera)
	{
		command_list->ClearRenderTarget(render_textures[RenderTargetType::Final], Color4::Black);
		return;
	}

	if (renderables.empty())
	{
		command_list->ClearRenderTarget(render_textures[RenderTargetType::Final], Color4::Black);
		return;
	}

	camera_near = camera->GetNearPlane();
	camera_far = camera->GetFarPlane();
	camera_view = camera->GetViewMatrix();
	camera_proj = camera->GetProjectionMatrix();
	camera_view_proj = camera_view * camera_proj;
	camera_view_proj_inverse = camera_view_proj.Inverse();
	camera_position = camera->GetTransform()->GetTranslation();

	post_process_view = Matrix::LookAtLH(Vector3(0.0f, 0.0f, -camera_near), Vector3::Forward, Vector3::Up);
	post_process_proj = Matrix::OrthoLH(resolution.x, resolution.y, camera_near, camera_far);
	post_process_view_proj = post_process_view * post_process_proj;

	PassMain();
}

void Renderer::CreateRenderTextures() //RTT. 최소 크기가 4임.
{
	auto width = static_cast<uint>(resolution.x);
	auto height = static_cast<uint>(resolution.y);

	if ((width / 4) == 0 || (height / 4) == 0) { //4보다 작으면 int형이기 때문에 0이 됨.
		LOG_WARNING_F("%dx%d is an invalid resolution", width, height);
		return;
	}

	//Screen Quad
	Geometry<VertexTexture> screen_quad;
	Geometry_Generator::CreateScreenQuad(screen_quad, width, height);

	screen_vertex_buffer = std::make_shared<VertexBuffer>(context);
	screen_vertex_buffer->Create(screen_quad.GetVertices());

	screen_index_buffer = std::make_shared<IndexBuffer>(context);
	screen_index_buffer->Create(screen_quad.GetIndices());

	//GBuffer
	render_textures[RenderTargetType::GBuffer_Albedo] = std::make_shared<Texture2D>(context, width, height, DXGI_FORMAT_R8G8B8A8_UNORM, 1, RTV | SRV);
	render_textures[RenderTargetType::GBuffer_Normal] = std::make_shared<Texture2D>(context, width, height, DXGI_FORMAT_R16G16B16A16_FLOAT, 1, RTV | SRV);
	render_textures[RenderTargetType::GBuffer_Material] = std::make_shared<Texture2D>(context, width, height, DXGI_FORMAT_R8G8B8A8_UNORM, 1, RTV | SRV);
	render_textures[RenderTargetType::GBuffer_Velocity] = std::make_shared<Texture2D>(context, width, height, DXGI_FORMAT_R16G16_FLOAT, 1, RTV | SRV);
	render_textures[RenderTargetType::GBuffer_Depth] = std::make_shared<Texture2D>(context, width, height, DXGI_FORMAT_D32_FLOAT, 1, DSV | SRV);

	//Light
	render_textures[RenderTargetType::Light_Diffuse] = std::make_shared<Texture2D>(context, width, height, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, RTV | SRV); //diffuse rgba32 -- 좀 더 깔끔한 출력을 위해 변경
	render_textures[RenderTargetType::Light_Specular] = std::make_shared<Texture2D>(context, width, height, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, RTV | SRV);

	//Composition
	render_textures[RenderTargetType::Composition] = std::make_shared<Texture2D>(context, width, height, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, RTV | SRV);
	render_textures[RenderTargetType::Composition_Previous] = std::make_shared<Texture2D>(context, width, height, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, RTV | SRV);

	//Final
	render_textures[RenderTargetType::Final] = std::make_shared<Texture2D>(context, width, height, DXGI_FORMAT_R16G16B16A16_FLOAT, 1, RTV | SRV);
}

void Renderer::CreateShaders()
{
	const auto shader_directory = context->GetSubsystem<ResourceManager>()->GetAssetDirectory(AssetType::Shader);

	//vertex shader
	auto vs_gbuffer = std::make_shared<Shader>(context);
	vs_gbuffer->AddShader<VertexShader>(shader_directory + "GBuffer.hlsl");
	shaders[ShaderType::VS_GBUFFER] = vs_gbuffer;

	auto vs_post_process = std::make_shared<Shader>(context);
	vs_post_process->AddShader<VertexShader>(shader_directory + "PostProcess.hlsl");
	shaders[ShaderType::VS_POST_PROCESS] = vs_post_process;

	//pixel shader
	auto ps_texture = std::make_shared<Shader>(context);
	ps_texture->AddDefine("PASS_TEXTURE");
	ps_texture->AddShader<PixelShader>(shader_directory + "PostProcess.hlsl");
	shaders[ShaderType::PS_TEXTURE] = ps_texture;

	auto ps_debug_normal = std::make_shared<Shader>(context);
	ps_debug_normal->AddDefine("DEBUG_NORMAL");
	ps_debug_normal->AddShader<PixelShader>(shader_directory + "PostProcess.hlsl");
	shaders[ShaderType::PS_DEBUG_NORMAL] = ps_debug_normal;

	auto ps_debug_depth = std::make_shared<Shader>(context);
	ps_debug_depth->AddDefine("DEBUG_DEPTH");
	ps_debug_depth->AddShader<PixelShader>(shader_directory + "PostProcess.hlsl");
	shaders[ShaderType::PS_DEBUG_DEPTH] = ps_debug_depth;

	auto ps_directional_light = std::make_shared<Shader>(context);
	ps_directional_light->AddDefine("DIRECTIONAL");
	ps_directional_light->AddShader<PixelShader>(shader_directory + "Light.hlsl");
	shaders[ShaderType::PS_DIRECTIONAL_LIGHT] = ps_directional_light;

	auto ps_spot_light = std::make_shared<Shader>(context);
	ps_spot_light->AddDefine("SPOT");
	ps_spot_light->AddShader<PixelShader>(shader_directory + "Light.hlsl");
	shaders[ShaderType::PS_SPOT_LIGHT] = ps_spot_light;

	auto ps_point_light = std::make_shared<Shader>(context);
	ps_point_light->AddDefine("POINT");
	ps_point_light->AddShader<PixelShader>(shader_directory + "Light.hlsl");
	shaders[ShaderType::PS_POINT_LIGHT] = ps_point_light;

	auto ps_composition = std::make_shared<Shader>(context);
	ps_composition->AddShader<PixelShader>(shader_directory + "Composition.hlsl");
	shaders[ShaderType::PS_COMPOSITION] = ps_composition;

	auto ps_gamma_correction = std::make_shared<Shader>(context);
	ps_gamma_correction->AddDefine("GAMMA_CORRECTION");
	ps_gamma_correction->AddShader<PixelShader>(shader_directory + "PostProcess.hlsl");
	shaders[ShaderType::PS_GAMMA_CORRECTION] = ps_gamma_correction;

	//vertex_pixel shader
	auto vps_color = std::make_shared<Shader>(context);
	vps_color->AddShader<VertexShader>(shader_directory + "Color.hlsl");
	vps_color->AddShader<PixelShader>(shader_directory + "Color.hlsl");
}

void Renderer::CreateConstantBuffers()
{
    global_buffer = std::make_shared<ConstantBuffer>(context);
    global_buffer->Create<GLOBAL_DATA>();
}

void Renderer::CreateSamplerStates()
{
	compare_depth = std::make_shared<SamplerState>(context);
	compare_depth->Create
	(
		D3D11_FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT,
		D3D11_TEXTURE_ADDRESS_CLAMP,
		D3D11_COMPARISON_GREATER
	);

	point_clamp = std::make_shared<SamplerState>(context);
	point_clamp->Create
	(
		D3D11_FILTER_MIN_MAG_MIP_POINT,
		D3D11_TEXTURE_ADDRESS_CLAMP,
		D3D11_COMPARISON_ALWAYS
	);

	bilinear_clamp = std::make_shared<SamplerState>(context);
	bilinear_clamp->Create
	(
		D3D11_FILTER_MIN_MAG_LINEAR_MIP_POINT,
		D3D11_TEXTURE_ADDRESS_CLAMP,
		D3D11_COMPARISON_ALWAYS
	);

	bilinear_wrap = std::make_shared<SamplerState>(context);
	bilinear_wrap->Create
	(
		D3D11_FILTER_MIN_MAG_LINEAR_MIP_POINT,
		D3D11_TEXTURE_ADDRESS_WRAP,
		D3D11_COMPARISON_ALWAYS
	);

	trilinear_clamp = std::make_shared<SamplerState>(context);
	trilinear_clamp->Create
	(
		D3D11_FILTER_MIN_MAG_MIP_LINEAR,
		D3D11_TEXTURE_ADDRESS_CLAMP,
		D3D11_COMPARISON_ALWAYS
	);

	anisotropic_wrap = std::make_shared<SamplerState>(context);
	anisotropic_wrap->Create
	(
		D3D11_FILTER_ANISOTROPIC,
		D3D11_TEXTURE_ADDRESS_CLAMP,
		D3D11_COMPARISON_ALWAYS
	);
}

void Renderer::CreateRasterzierStates()
{
	cull_back_solid = std::make_shared<RasterizerState>(context);
	cull_back_solid->Create(D3D11_CULL_BACK, D3D11_FILL_SOLID);

	cull_front_solid = std::make_shared<RasterizerState>(context);
	cull_front_solid->Create(D3D11_CULL_FRONT, D3D11_FILL_SOLID);

	cull_none_solid = std::make_shared<RasterizerState>(context);
	cull_none_solid->Create(D3D11_CULL_NONE, D3D11_FILL_SOLID);

	cull_back_wireframe = std::make_shared<RasterizerState>(context);
	cull_back_wireframe->Create(D3D11_CULL_BACK, D3D11_FILL_WIREFRAME);

	cull_front_wireframe = std::make_shared<RasterizerState>(context);
	cull_front_wireframe->Create(D3D11_CULL_FRONT, D3D11_FILL_WIREFRAME);

	cull_none_wireframe = std::make_shared<RasterizerState>(context);
	cull_none_wireframe->Create(D3D11_CULL_NONE, D3D11_FILL_WIREFRAME);
}

void Renderer::CreateBlendStates()
{
	blend_enabled = std::make_shared<BlendState>(context);
	blend_enabled->Create(true);

	blend_disabled = std::make_shared<BlendState>(context);
	blend_disabled->Create(false);

	blend_color_add = std::make_shared<BlendState>(context);
	blend_color_add->Create(true, D3D11_BLEND_ONE, D3D11_BLEND_ONE);

	blend_bloom = std::make_shared<BlendState>(context);
	blend_bloom->Create(true, D3D11_BLEND_ONE, D3D11_BLEND_ONE, D3D11_BLEND_OP_ADD, D3D11_BLEND_ONE, D3D11_BLEND_ONE, D3D11_BLEND_OP_ADD, 0.5f);
}

void Renderer::CreateDepthStencilStates()
{
	/*
	is_reverse_z->true이면 비선형 값이라는 뜻
	가까운 곳에서 대부분의 데이터량을 사용하고 멀어질 수록 정밀도가 낮아짐.
	그게 어느 기준을 기점으로 급격한 차이를 보이는 비선형적 구조인데, 이를 z를 역수로 취함으로서 균일하게 낮아지도록 함.(추후 설명)
	*/
	depth_stencil_enabled_state = std::make_shared<DepthStencilState>(context);
	depth_stencil_enabled_state->Create(true, is_reverse_z ? D3D11_COMPARISON_GREATER_EQUAL : D3D11_COMPARISON_LESS_EQUAL);

	depth_stencil_disabled_state = std::make_shared<DepthStencilState>(context);
	depth_stencil_disabled_state->Create(true, is_reverse_z ? D3D11_COMPARISON_GREATER_EQUAL : D3D11_COMPARISON_LESS_EQUAL);
}

void Renderer::UpdateGlobalBuffer(const uint & width, const uint & height, const Matrix & world_view_proj)
{
    auto gpu_buffer = global_buffer->Map<GLOBAL_DATA>();
    if (!gpu_buffer)
    {
        LOG_ERROR("Failed to map buffer");
        return;
    }

	float directional_light_intensity = 0.0f;
	if (auto& actor = renderables[RenderableType::Directional_Light].front())
	{
		if (auto& light = actor->GetComponent<Light>())
			directional_light_intensity = light->GetIntensity();
	}

	gpu_buffer->world_view_proj = world_view_proj;
	gpu_buffer->view = camera_view;
	gpu_buffer->proj = camera_proj;
	gpu_buffer->view_proj = camera_view_proj;
	gpu_buffer->view_proj_inverse = camera_view_proj_inverse;
	gpu_buffer->camera_position = camera_position;
	gpu_buffer->camera_near = camera_near;
	gpu_buffer->camera_far = camera_far;
	gpu_buffer->resolution = Vector2(static_cast<float>(width), static_cast<float>(height));
	gpu_buffer->proj_ortho = post_process_proj;
	gpu_buffer->view_proj_ortho = post_process_view_proj;
	gpu_buffer->directional_light_intensity = directional_light_intensity;

    global_buffer->Unmap();
}

auto Renderer::GetRasterizerState(const D3D11_CULL_MODE & cull_mode, const D3D11_FILL_MODE & fill_mode) -> const std::shared_ptr<class RasterizerState>&
{
	switch (cull_mode)
	{
	case D3D11_CULL_NONE:
	{
		switch (fill_mode)
		{
		case D3D11_FILL_WIREFRAME:  return cull_none_wireframe;
		case D3D11_FILL_SOLID:      return cull_none_solid;
		}
		break;
	}
	case D3D11_CULL_FRONT:
	{
		switch (fill_mode)
		{
		case D3D11_FILL_WIREFRAME:  return cull_front_wireframe;
		case D3D11_FILL_SOLID:      return cull_front_solid;
		}
		break;
	}
	case D3D11_CULL_BACK:
	{
		switch (fill_mode)
		{
		case D3D11_FILL_WIREFRAME:  return cull_back_wireframe;
		case D3D11_FILL_SOLID:      return cull_back_solid;
		}
		break;
	}
	}

	return cull_back_solid;
}