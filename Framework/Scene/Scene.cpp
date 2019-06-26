#include "Framework.h"
#include "Scene.h"
#include "./Component/Camera.h"
#include "./Component/Terrain.h"

Scene::Scene(Context * context)
    : context(context)
{
    graphics = context->GetSubsystem<Graphics>();

    camera = new Camera(context);

    camera_buffer = new ConstantBuffer(context);
    camera_buffer->Create<CameraData>();

    terrain = new Terrain(context, camera);
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
}

void Scene::Render()
{
    camera_buffer->BindPipeline(0, ShaderScope::VS);
    terrain->Render();
}
