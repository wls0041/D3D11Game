#include "Framework.h"
#include "Scene.h"
#include "./Component/Camera.h"
#include "./Component/Terrain.h"

Scene::Scene(Context * context)
    : context(context)
{
	///////////////////////////////////////////

	Geometry_Generator::CreateCube(geometry);

	vertexBuffer = new VertexBuffer(context);
	vertexBuffer->Create(geometry.GetVertices());

	indexBuffer = new IndexBuffer(context);
	indexBuffer->Create(geometry.GetIndices());

	vertexShader = new VertexShader(context);
	vertexShader->Create("../../_Assets/Shader/Texture.hlsl", "VS", "vs_5_0");

	pixelShader = new PixelShader(context);
	pixelShader->Create("../../_Assets/Shader/Texture.hlsl", "PS", "ps_5_0");

	inputLayout = new InputLayout(context);
	inputLayout->Create(vertexShader->GetBlob());

	worldBuffer = new ConstantBuffer(context);
	worldBuffer->Create<WorldData>();

	D3DXMatrixIdentity(&world);
}

Scene::~Scene()
{
    SAFE_DELETE(terrain);
    SAFE_DELETE(camera_buffer);
    SAFE_DELETE(camera);
}

void Scene::Update()
{
    camera->Update();

    auto camera_data = camera_buffer->Map<CameraData>();
    {
        D3DXMatrixTranspose(&camera_data->view, &camera->GetViewMatrix());
        D3DXMatrixTranspose(&camera_data->projection, &camera->GetProjectionMatrix());
    }
    camera_buffer->Unmap();

    terrain->Update();

	//--------------------- //임시
	static D3DXVECTOR3 position(100, 0, 100);

	if (context->GetSubsystem<Input>()->KeyPress(KeyCode::KEY_W))
		position.z += 1.0;
	else if (context->GetSubsystem<Input>()->KeyPress(KeyCode::KEY_S))
		position.z -= 1.0;
	
	if (context->GetSubsystem<Input>()->KeyPress(KeyCode::KEY_A))
		position.x -= 1.0;
	else if (context->GetSubsystem<Input>()->KeyPress(KeyCode::KEY_D))
		position.x += 1.0;
	
	position.y = terrain->GetHeight(position);

	D3DXMATRIX S, T;
	D3DXMatrixScaling(&S, 10, 10, 10);
	D3DXMatrixTranslation(&T, position.x, position.y, position.z);

	world = S * T;

	auto world_data = worldBuffer->Map<WorldData>();
	{
		D3DXMatrixTranspose(&world_data->world, &world);
	}
	worldBuffer->Unmap();
}

void Scene::Render()
{
    camera_buffer->BindPipeline(0, ShaderScope::VS);
    terrain->Render();

	//------------------------------------------ //임시
	vertexBuffer->BindPipeline();
	indexBuffer->BindPipeline();
	inputLayout->BindPipeline();
	vertexShader->BindPipeline();
	pixelShader->BindPipeline();
	worldBuffer->BindPipeline(1, ShaderScope::VS);

	graphics->GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	graphics->GetDeviceContext()->DrawIndexed(geometry.GetIndexCount(), 0, 0);
}
