#include "Framework.h"
#include "MultiCube.h"
#include "Camera.h"

MultiCube::MultiCube(Context * context, class Camera* camera, const D3DXVECTOR3 &position) 
	: context(context)
	, camera(camera)
	, position(position)
{
	graphics = context->GetSubsystem<Graphics>();

	Geometry_Generator::CreateCube(geometry);

	vertexBuffer = new VertexBuffer(context);
	vertexBuffer->Create(geometry.GetVertices());

	indexBuffer = new IndexBuffer(context);
	indexBuffer->Create(geometry.GetIndices());

	vertexShader = new VertexShader(context);
	vertexShader->Create("../../_Assets/Shader/MultiCube.hlsl", "VS", "vs_5_0");

	pixelShader = new PixelShader(context);
	pixelShader->Create("../../_Assets/Shader/MultiCube.hlsl", "PS", "ps_5_0");

	inputLayout = new InputLayout(context);
	inputLayout->Create(vertexShader->GetBlob());

	worldBuffer = new ConstantBuffer(context);
	worldBuffer->Create<WorldData>();

	D3DXMatrixIdentity(&world);


	D3DXMATRIX S, T;
	D3DXMatrixScaling(&S, 10, 10, 10);
	D3DXMatrixTranslation(&T, 75.0f + this->position.x * 15, 75.0f + this->position.y * 15, 75.0f + this->position.z * 15);

	world = S * T;

	auto world_data = worldBuffer->Map<WorldData>();
	{
		D3DXMatrixTranspose(&world_data->world, &world);
	}
	worldBuffer->Unmap();
}

MultiCube::~MultiCube()
{
	SAFE_DELETE(vertexBuffer);
	SAFE_DELETE(indexBuffer);
	SAFE_DELETE(inputLayout);
	SAFE_DELETE(vertexShader);
	SAFE_DELETE(pixelShader);
	SAFE_DELETE(worldBuffer);
	SAFE_DELETE(multi_buffer);
}

void MultiCube::Update()
{
	static D3DXVECTOR3 temp;
	auto picked = Pick(temp);

	if (picked)
	{
		D3DXVECTOR3 may = temp;

		auto multi_data = multi_buffer->Map<MultiData>();
		{
			multi_data->paint_type = 0;
			multi_data->index = temp;
		}
		multi_buffer->Unmap();
	}
}

void MultiCube::Render()
{
	vertexBuffer->BindPipeline();
	indexBuffer->BindPipeline();
	inputLayout->BindPipeline();
	vertexShader->BindPipeline();
	pixelShader->BindPipeline();
	worldBuffer->BindPipeline(1, ShaderScope::VS);

	graphics->GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	graphics->GetDeviceContext()->DrawIndexed(geometry.GetIndexCount(), 0, 0);
}

auto MultiCube::Pick(D3DXVECTOR3 & position) -> const bool
{
	D3DXVECTOR3 org, dir;
	camera->GetWorldRay(org, dir);

	auto vertices = geometry.GetVertexData();
	auto indices = geometry.GetIndexData();

	for (uint i = 0; i < geometry.GetIndexCount(); i += 3)
	{
		auto index0 = indices[i + 0];
		auto index1 = indices[i + 1];
		auto index2 = indices[i + 2];

		auto v0 = vertices[index0].position;
		auto v1 = vertices[index1].position;
		auto v2 = vertices[index2].position;

		float u, v, distance;

		if (D3DXIntersectTri(&v0, &v1, &v2, &org, &dir, &u, &v, &distance))
		{
			//v0 += u * (v2 - v0) + v * (v1 - v0);
			//position = v0;

			position = org + distance * dir;

			return true;
		}
	}

	return false;
}
