#include "Framework.h"
#include "Renderer.h"
#include "../D3D11/CommandList.h"
#include "../../Scene/Scene.h"
#include "../../Scene/Actor.h"
#include "../../Scene/Component/Camera.h"
#include "../../Scene/Component/Transform.h"

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
}

void Renderer::Render()
{
	PassMain();
}