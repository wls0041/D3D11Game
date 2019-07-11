#include "Framework.h"
#include "Scene.h"
#include "./Component/Camera.h"
#include "./Component/Skybox.h"
#include "./Component/Terrain.h"
#include "./Component/MultiCube.h"
#include "./Resource/ModelImporter/ModelImporter.h"

Scene::Scene(Context * context)
    : context(context)
{
	graphics = context->GetSubsystem<Graphics>();

	camera = new Camera(context);

	camera_buffer = new ConstantBuffer(context);
	camera_buffer->Create<CameraData>();

	skybox = new Skybox(context, camera);
	terrain = new Terrain(context, camera);

	for (int x = 0; x < 5; x++) {
		for (int y = 0; y < 5; y++) {
			for (int z = 0; z < 5; z++) {
				multiCube[x][y][z] = new MultiCube(context, camera, D3DXVECTOR3(x, y, z));
			}
		}
	}
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
	SAFE_DELETE(vertexBuffer);
	SAFE_DELETE(indexBuffer);
	SAFE_DELETE(inputLayout);
	SAFE_DELETE(vertexShader);
	SAFE_DELETE(pixelShader);
	SAFE_DELETE(worldBuffer);

	for (int x = 0; x < 5; x++) {
		for (int y = 0; y < 5; y++) {
			for (int z = 0; z < 5; z++) {
				SAFE_DELETE(multiCube[x][y][z]);
			}
		}
	}

	SAFE_DELETE(terrain);
	SAFE_DELETE(skybox);
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
		camera_data->camera_position = camera->GetPosition();
    }
    camera_buffer->Unmap();

	skybox->Update();
    terrain->Update();

	for (int x = 0; x < 5; x++) {
		for (int y = 0; y < 5; y++) {
			for (int z = 0; z < 5; z++) {
				//multiCube[x][y][z]->Update();
			}
		}
	}

	//--------------------- //�ӽ�
	static D3DXVECTOR3 position(100, 0, 100);

	if (context->GetSubsystem<Input>()->KeyPress(KeyCode::KEY_I))
		position.z += 1.0f;
	else if (context->GetSubsystem<Input>()->KeyPress(KeyCode::KEY_K))
		position.z -= 1.0f;

	if (context->GetSubsystem<Input>()->KeyPress(KeyCode::KEY_J))
		position.x -= 1.0f;
	else if (context->GetSubsystem<Input>()->KeyPress(KeyCode::KEY_L))
		position.x += 1.0f;

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
	skybox->Render();
	terrain->Render();

	for (int x = 0; x < 5; x++) {
		for (int y = 0; y < 5; y++) {
			for (int z = 0; z < 5; z++) {
				multiCube[x][y][z]->Render();
			}
		}
	}
	//------------------------------------------ //�ӽ�
	vertexBuffer->BindPipeline();
	indexBuffer->BindPipeline();
	inputLayout->BindPipeline();
	vertexShader->BindPipeline();
	pixelShader->BindPipeline();
	worldBuffer->BindPipeline(1, ShaderScope::VS);

	graphics->GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	graphics->GetDeviceContext()->DrawIndexed(geometry.GetIndexCount(), 0, 0);
}
