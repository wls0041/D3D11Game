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

void Renderer::CreateRenderTextures() //RTT. 최소 크기가 4임.
{
	auto width = static_cast<uint>(resolution.x);
	auto height = static_cast<uint>(resolution.y);

	if ((width / 4) == 0 || (height / 4) == 0) { //4보다 작으면 int형이기 때문에 0이 됨.
		LOG_WARNING_F("%dx%d is an invalid resolution", width, height);
		return;
	}

	//final
	final_render_texture = std::make_shared<Texture2D>(context, width, height, DXGI_FORMAT_R16G16B16A16_FLOAT, 1, RTV | SRV);
}

void Renderer::CreateShaders()
{
	const auto shader_directory = context->GetSubsystem<ResourceManager>()->GetAssetDirectory(AssetType::Shader);

	//vertex shader
	auto vs_gbuffer = std::make_shared<Shader>(context);
	vs_gbuffer->AddShader<VertexShader>(shader_directory + "GBuffer.hlsl");
	shaders["vs_gbuffer"] = vs_gbuffer;

	auto vs_post_process = std::make_shared<Shader>(context);
	vs_post_process->AddShader<VertexShader>(shader_directory + "PostProcess.hlsl");
	shaders["PostProcess"] = vs_post_process;


}
