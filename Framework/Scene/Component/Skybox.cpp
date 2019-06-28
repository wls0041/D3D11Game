#include "Framework.h"
#include "Skybox.h"
#include "Camera.h"

Skybox::Skybox(Context * context, Camera * camera)
	: context(context)
	, camera(camera)
{
	graphics = context->GetSubsystem<Graphics>();

	Geometry_Generator::CreateSphere(sphere);

	vertex_buffer = new VertexBuffer(context);
	vertex_buffer->Create(sphere.GetVertices());

	index_buffer = new IndexBuffer(context);
	index_buffer->Create(sphere.GetIndices());

	vertex_shader = new VertexShader(context);
	vertex_shader->Create("../../_Assets/Shader/Skybox.hlsl", "VS", "vs_5_0");

	pixel_shader = new PixelShader(context);
	pixel_shader->Create("../../_Assets/Shader/Skybox.hlsl", "PS", "ps_5_0");

	input_layout = new InputLayout(context);
	input_layout->Create(vertex_shader->GetBlob());

	constant_buffer = new ConstantBuffer(context);
	constant_buffer->Create<WorldData>();

	D3DXMatrixIdentity(&world);

	//Create ShaderResourceView
	{
		auto hr = D3DX11CreateShaderResourceViewFromFileA
		(
			graphics->GetDevice(),
			"../../_Assets/Cubemap/snowcube1024.dds",
			nullptr,
			nullptr,
			&environment_map,
			nullptr
		);
		assert(SUCCEEDED(hr));
	}

	//Create RasterizerState
	{
		D3D11_RASTERIZER_DESC desc;
		ZeroMemory(&desc, sizeof(D3D11_RASTERIZER_DESC));
		desc.FillMode = D3D11_FILL_SOLID;
		desc.CullMode = D3D11_CULL_FRONT;
		desc.FrontCounterClockwise = false;

		auto hr = graphics->GetDevice()->CreateRasterizerState
		(
			&desc,
			&rasterizer_state
		);
		assert(SUCCEEDED(hr));
	}

	//Create DepthStencilState
	{
		D3D11_DEPTH_STENCIL_DESC desc;
		ZeroMemory(&desc, sizeof(D3D11_DEPTH_STENCIL_DESC));
		desc.DepthEnable = false;
		desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		desc.DepthFunc = D3D11_COMPARISON_LESS;

		auto hr = graphics->GetDevice()->CreateDepthStencilState(&desc, &depth_state);
		assert(SUCCEEDED(hr));
	}
}

Skybox::~Skybox()
{
	SAFE_RELEASE(depth_back_state);
	SAFE_RELEASE(depth_state);

	SAFE_RELEASE(back_state);
	SAFE_RELEASE(rasterizer_state);

	SAFE_RELEASE(environment_map);

	SAFE_DELETE(constant_buffer);
	SAFE_DELETE(input_layout);
	SAFE_DELETE(pixel_shader);
	SAFE_DELETE(vertex_shader);

	SAFE_DELETE(index_buffer);
	SAFE_DELETE(vertex_buffer);
}

void Skybox::Update()
{
	D3DXMATRIX S, T;
	D3DXMatrixScaling(&S, 1, 1, 1);

	auto camera_pos = camera->GetPosition();
	D3DXMatrixTranslation(&T, camera_pos.x, camera_pos.y, camera_pos.z);

	world = S * T;

	auto world_data = constant_buffer->Map<WorldData>();
	D3DXMatrixTranspose(&world_data->world, &world);
	constant_buffer->Unmap();
}

void Skybox::Render()
{
	graphics->GetDeviceContext()->RSGetState(&back_state);
	graphics->GetDeviceContext()->OMGetDepthStencilState(&depth_back_state, nullptr);
	{
		vertex_buffer->BindPipeline();
		index_buffer->BindPipeline();
		input_layout->BindPipeline();
		vertex_shader->BindPipeline();
		pixel_shader->BindPipeline();
		constant_buffer->BindPipeline(1, ShaderScope::VS);

		graphics->GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		graphics->GetDeviceContext()->PSSetShaderResources(0, 1, &environment_map);
		graphics->GetDeviceContext()->RSSetState(rasterizer_state);
		graphics->GetDeviceContext()->OMSetDepthStencilState(depth_state, 1);
		graphics->GetDeviceContext()->DrawIndexed(sphere.GetIndexCount(), 0, 0);
	}
	graphics->GetDeviceContext()->RSSetState(back_state);
	graphics->GetDeviceContext()->OMSetDepthStencilState(depth_back_state, 1);
}
