#include "Framework.h"
#include "Scene.h"
#include "./Component/Camera.h"

Scene::Scene(Context * context) : context(context)
{
	graphics = context->GetSubsystem<Graphics>();
	camera = new Camera(context);

	//create vertex data



	geometry.AddVertex(VertexColor(D3DXVECTOR3(-0.5f, -0.5f, 0.0f), D3DXCOLOR(0.5f, 0.5f, 0.0f, 1.0f)));
	geometry.AddVertex(VertexColor(D3DXVECTOR3(-0.5f, +0.5f, 0.0f), D3DXCOLOR(0.5f, 0.5f, 0.0f, 1.0f)));
	geometry.AddVertex(VertexColor(D3DXVECTOR3(+0.5f, -0.5f, 0.0f), D3DXCOLOR(0.5f, 0.5f, 0.0f, 1.0f)));
	geometry.AddVertex(VertexColor(D3DXVECTOR3(+0.5f, +0.5f, 0.0f), D3DXCOLOR(0.5f, 0.5f, 0.0f, 1.0f)));


	vertex_buffer = new VertexBuffer(context);
	vertex_buffer->Create(geometry.GetVertices(), D3D11_USAGE_IMMUTABLE);

	index_buffer = new IndexBuffer(context);
	index_buffer->Create(geometry.GetIndices(), D3D11_USAGE_DEFAULT);

	//Create Index buffer
	{
		D3D11_BUFFER_DESC desc;
		ZeroMemory(&desc, sizeof(D3D11_BUFFER_DESC));
		desc.Usage = D3D11_USAGE_IMMUTABLE;
		desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		desc.ByteWidth = sizeof(uint) * geometry.GetIndexByteWidth();

		D3D11_SUBRESOURCE_DATA sub_data;
		ZeroMemory(&sub_data, sizeof(D3D11_SUBRESOURCE_DATA));
		sub_data.pSysMem = geometry.GetIndexData();

		auto hr = graphics->GetDevice()->CreateBuffer(&desc, &sub_data, &index_buffer);
		assert(SUCCEEDED(hr));
	}

	//Vertex Shader
	{
		auto hr = D3DX11CompileFromFileA("../../_Assets/Shader/Color.hlsl", nullptr, nullptr, "VS", "vs_5_0", 0, 0, nullptr, &vs_blob, nullptr, nullptr); //아스키, vs_blob - 컴파일한 정보
		assert(SUCCEEDED(hr));

		hr = graphics->GetDevice()->CreateVertexShader(vs_blob->GetBufferPointer(), vs_blob->GetBufferSize(), nullptr, &vertex_shader);
		assert(SUCCEEDED(hr));
	}

	//Pixel Shader
	{
		auto hr = D3DX11CompileFromFileA("../../_Assets/Shader/Color.hlsl", nullptr, nullptr, "PS", "ps_5_0", 0, 0, nullptr, &ps_blob, nullptr, nullptr);
		assert(SUCCEEDED(hr));

		hr = graphics->GetDevice()->CreatePixelShader(ps_blob->GetBufferPointer(), ps_blob->GetBufferSize(), nullptr, &pixel_shader);
		assert(SUCCEEDED(hr));
	}

	//create InputLayout
	{
		D3D11_INPUT_ELEMENT_DESC descs[]
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 }
		};

		auto hr = graphics->GetDevice()->CreateInputLayout(descs, 2, vs_blob->GetBufferPointer(), vs_blob->GetBufferSize(), &input_layout);
		assert(SUCCEEDED(hr));
	}

	//create constant buffer
	{
		D3D11_BUFFER_DESC desc;
		ZeroMemory(&desc, sizeof(D3D11_BUFFER_DESC));
		desc.Usage = D3D11_USAGE_DYNAMIC;
		desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		desc.ByteWidth = sizeof(CameraData);
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

		auto hr = graphics->GetDevice()->CreateBuffer(&desc, nullptr, &camera_buffer);
		assert(SUCCEEDED(hr));

	}

	//create world buffer
	{
		D3D11_BUFFER_DESC desc;
		ZeroMemory(&desc, sizeof(D3D11_BUFFER_DESC));
		desc.Usage = D3D11_USAGE_DYNAMIC;
		desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		desc.ByteWidth = sizeof(WorldData);
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

		auto hr = graphics->GetDevice()->CreateBuffer(&desc, nullptr, &world_buffer);
		assert(SUCCEEDED(hr));
	}

	D3DXMatrixIdentity(&world);
}

Scene::~Scene()
{
	SAFE_DELETE(index_buffer);
	SAFE_DELETE(vertex_buffer);
	SAFE_DELETE(camera);
}

void Scene::Update()
{
	camera->Update();

	//Camera Mapped
	{
		D3D11_MAPPED_SUBRESOURCE sub_resource;
		ZeroMemory(&sub_resource, sizeof(D3D11_MAPPED_SUBRESOURCE));
		graphics->GetDeviceContext()->Map(camera_buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &sub_resource);
	
		auto data = static_cast<CameraData*>(sub_resource.pData);

		D3DXMatrixTranspose(&data->view, &camera->GetViewMatrix());
		D3DXMatrixTranspose(&data->projection, &camera->GetProjectionMatrix());
		
		graphics->GetDeviceContext()->Unmap(camera_buffer, 0);
	}

	//world Mapped
	{
		D3D11_MAPPED_SUBRESOURCE sub_resource;
		ZeroMemory(&sub_resource, sizeof(D3D11_MAPPED_SUBRESOURCE));
		graphics->GetDeviceContext()->Map(world_buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &sub_resource);

		auto data = static_cast<WorldData*>(sub_resource.pData);

		D3DXMatrixTranspose(&data->world, &world);

		graphics->GetDeviceContext()->Unmap(world_buffer, 0);
	}

}

void Scene::Render()
{
	vertex_buffer->BindPipeline();
	index_buffer->BindPipeline();

	graphics->GetDeviceContext()->IASetInputLayout(input_layout);
	graphics->GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	graphics->GetDeviceContext()->VSSetShader(vertex_shader, nullptr, 0);
	graphics->GetDeviceContext()->VSSetConstantBuffers(0, 1, &camera_buffer);
	graphics->GetDeviceContext()->VSSetConstantBuffers(1, 1, &world_buffer);

	graphics->GetDeviceContext()->PSSetShader(pixel_shader, nullptr, 0);

	graphics->GetDeviceContext()->DrawIndexed(geometry.GetIndexCount(), 0, 0);
}
