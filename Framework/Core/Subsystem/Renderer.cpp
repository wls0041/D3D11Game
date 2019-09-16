#include "Framework.h"
#include "Renderer.h"
#include "../D3D11/CommandList.h"
#include "Gizmo/Grid.h"
#include "../../Scene/Scene.h"
#include "../../Scene/Actor.h"
#include "../../Scene/Component/Camera.h"
#include "../../Scene/Component/Transform.h"
#include "../../Scene/Component/Renderable.h"

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

	return true;
}

auto Renderer::GetFrameResource() -> ID3D11ShaderResourceView *
{
	auto render_target = render_textures[RenderTargetType::GBuffer_Normal];
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
		auto component_renderable = actor->GetComponent<Renderable>();
		auto component_camera = actor->GetComponent<Camera>();

		if (component_renderable)
			renderables[RenderableType::Opaque].emplace_back(actor.get());

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

	//GBuffer
	render_textures[RenderTargetType::GBuffer_Albedo]	= std::make_shared<Texture2D>(context, width, height, DXGI_FORMAT_R8G8B8A8_UNORM, 1, RTV | SRV);
	render_textures[RenderTargetType::GBuffer_Normal]	= std::make_shared<Texture2D>(context, width, height, DXGI_FORMAT_R16G16B16A16_FLOAT, 1, RTV | SRV);
	render_textures[RenderTargetType::GBuffer_Material]	= std::make_shared<Texture2D>(context, width, height, DXGI_FORMAT_R8G8B8A8_UNORM, 1, RTV | SRV);
	render_textures[RenderTargetType::GBuffer_Velocity]	= std::make_shared<Texture2D>(context, width, height, DXGI_FORMAT_R16G16_FLOAT, 1, RTV | SRV);
	render_textures[RenderTargetType::GBuffer_Depth]	= std::make_shared<Texture2D>(context, width, height, DXGI_FORMAT_D32_FLOAT, 1, DSV | SRV);

	//final
	auto final_render_texture = std::make_shared<Texture2D>(context, width, height, DXGI_FORMAT_R16G16B16A16_FLOAT, 1, RTV | SRV);
	render_textures[RenderTargetType::Final] = final_render_texture;
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

void Renderer::CreateDepthStencilStates()
{
	depth_stencil_enabled_state = std::make_shared<DepthStencilState>(context);
	depth_stencil_enabled_state->Create(true);

	depth_stencil_disabled_state = std::make_shared<DepthStencilState>(context);
	depth_stencil_disabled_state->Create(true);
}

void Renderer::UpdateGlobalBuffer(const uint & width, const uint & height, const Matrix & world_view_proj)
{
    auto gpu_buffer = global_buffer->Map<GLOBAL_DATA>();
    if (!gpu_buffer)
    {
        LOG_ERROR("Failed to map buffer");
        return;
    }

    gpu_buffer->world_view_proj     = world_view_proj;
    gpu_buffer->view                = camera_view;
    gpu_buffer->proj                = camera_proj;
    gpu_buffer->view_proj           = camera_view_proj;
    gpu_buffer->view_proj_inverse   = camera_view_proj_inverse;
    gpu_buffer->camera_position     = camera_position;
    gpu_buffer->camera_near         = camera_near;
    gpu_buffer->camera_far          = camera_far;
    gpu_buffer->resolution          = Vector2(static_cast<float>(width), static_cast<float>(height));
    gpu_buffer->proj_ortho          = post_process_proj;
    gpu_buffer->view_proj_ortho     = post_process_view_proj;

    global_buffer->Unmap();
}