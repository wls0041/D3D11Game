#include "Framework.h"
#include "Model.h"

Model::Model(Context * context)
	: context(context)
	, vertexBuffer(nullptr)
	, indexBuffer(nullptr)
{
	graphics = context->GetSubsystem<Graphics>();

	vertexShader = new VertexShader(context);
	vertexShader->Create("../../_Assets/Shader/Model.hlsl", "VS", "vs_5_0");

	pixelShader = new PixelShader(context);
	pixelShader->Create("../../_Assets/Shader/Model.hlsl", "PS", "ps_5_0");

	inputLayout = new InputLayout(context);
	inputLayout->Create(vertexShader->GetBlob());

	worldBuffer = new ConstantBuffer(context);
	worldBuffer->Create<WorldData>();

	D3DXMatrixIdentity(&world);

	vertexBuffer = new VertexBuffer(context);
	indexBuffer = new IndexBuffer(context);

}

Model::~Model()
{
	SAFE_DELETE(vertexBuffer);
	SAFE_DELETE(indexBuffer);
	SAFE_DELETE(inputLayout);
	SAFE_DELETE(vertexShader);
	SAFE_DELETE(pixelShader);
	SAFE_DELETE(worldBuffer);
}

void Model::SetGeometry(const std::vector<VertexModel>& vertices, const std::vector<uint>& indices)
{
	vertexBuffer->Create(vertices);
	indexBuffer->Create(indices);
}

void Model::Update()
{
	auto world_data = worldBuffer->Map<WorldData>();
	{
		D3DXMatrixTranspose(&world_data->world, &world);
	}
	worldBuffer->Unmap();
}

void Model::Render()
{
	vertexBuffer->BindPipeline();
	indexBuffer->BindPipeline();
	inputLayout->BindPipeline();
	vertexShader->BindPipeline();
	pixelShader->BindPipeline();
	worldBuffer->BindPipeline(1, ShaderScope::VS);

	graphics->GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	graphics->GetDeviceContext()->DrawIndexed(indexBuffer->GetCount(), 0, 0);
}
