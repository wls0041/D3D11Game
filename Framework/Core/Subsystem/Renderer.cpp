#include "Framework.h"
#include "Renderer.h"
#include "../D3D11/CommandList.h"
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
	graphics = context->GetSubsystem<Graphics>();
	command_list = std::make_shared<CommandList>(context);
	render_target = std::make_shared<Texture2D>(context, 1280, 720, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, RTV | SRV);

	return true;
}

auto Renderer::GetFrameResource() -> ID3D11ShaderResourceView *
{
	return render_target ? render_target->GetShaderResourceView() : nullptr;
}

void Renderer::AcquireRenderables(Scene * scene)
{
	while (is_acquire_renderables)
		LOG_WARNING_F("Waiting previous operation to finish ...");

	is_acquire_renderables = true;

	renderables.clear();
	camera.reset(); //camera = nullptr;

	auto actors = scene->GetAllActors();
	for (const auto &actor : actors)
	{
		if (!actor) continue;
		auto component_renderable = actor->GetComponent<Renderable>();
		auto component_camera = actor->GetComponent<Camera>();

		if (component_renderable)
			renderables[RenderableType::Opaque].emplace_back(actor);

		if (component_camera) { //scene_camera 용
			renderables[RenderableType::Camera].emplace_back(actor);
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

void Renderer::Render()
{
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
}

void Renderer::CreateConstantBuffers()
{
    global_buffer = std::make_shared<ConstantBuffer>(context);
    global_buffer->Create<CPU_GLOBAL_DATA>();
}

void Renderer::UpdateGlobalBuffer(const uint & width, const uint & height, const Matrix & world_view_proj)
{
    auto gpu_buffer = global_buffer->Map<CPU_GLOBAL_DATA>();
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