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