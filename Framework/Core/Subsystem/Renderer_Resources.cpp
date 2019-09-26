#include "Framework.h"
#include "Renderer.h"


void Renderer::CreateRenderTextures() //RTT. 최소 크기가 4임
{
    auto width = static_cast<uint>(resolution.x);
    auto height = static_cast<uint>(resolution.y);

    if ((width / 4) == 0 || (height / 4) == 0)//RTT. 최소 크기가 4임
    {
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
    render_textures[RenderTargetType::Light_Diffuse] = std::make_shared<Texture2D>(context, width, height, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, RTV | SRV);
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

    auto ps_debug_light = std::make_shared<Shader>(context);
    ps_debug_light->AddDefine("DEBUG_LIGHT");
    ps_debug_light->AddShader<PixelShader>(shader_directory + "PostProcess.hlsl");
    shaders[ShaderType::PS_DEBUG_LIGHT] = ps_debug_light;

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

    //vertex_pixel_shader
    auto vps_color = std::make_shared<Shader>(context);
    vps_color->AddShader<VertexShader>(shader_directory + "Color.hlsl");
    vps_color->AddShader<PixelShader>(shader_directory + "Color.hlsl");
    shaders[ShaderType::VPS_COLOR] = vps_color;
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
    depth_stencil_disabled_state->Create(false, is_reverse_z ? D3D11_COMPARISON_GREATER_EQUAL : D3D11_COMPARISON_LESS_EQUAL);
}