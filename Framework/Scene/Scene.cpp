#include "Framework.h"
#include "Scene.h"
#include "./Component/Camera.h"

Scene::Scene(Context * context)
    : context(context)
{
    graphics = context->GetSubsystem<Graphics>();

    camera = new Camera(context);

    geometry.AddVertex(VertexColor(D3DXVECTOR3(-0.5f, -0.5f, 0.0f), D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f)));
    geometry.AddVertex(VertexColor(D3DXVECTOR3(-0.5f, +0.5f, 0.0f), D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f)));
    geometry.AddVertex(VertexColor(D3DXVECTOR3(+0.5f, -0.5f, 0.0f), D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f)));
    geometry.AddVertex(VertexColor(D3DXVECTOR3(+0.5f, +0.5f, 0.0f), D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f)));

    geometry.AddIndex(0); geometry.AddIndex(1); geometry.AddIndex(2);
    geometry.AddIndex(2); geometry.AddIndex(1); geometry.AddIndex(3);

    vertex_buffer = new VertexBuffer(context);
    vertex_buffer->Create(geometry.GetVertices(), D3D11_USAGE_IMMUTABLE);

    index_buffer = new IndexBuffer(context);
    index_buffer->Create(geometry.GetIndices(), D3D11_USAGE_IMMUTABLE);

    vertex_shader = new VertexShader(context);
    vertex_shader->Create("../../_Assets/Shader/Color.hlsl", "VS", "vs_5_0");

    pixel_shader = new PixelShader(context);
    pixel_shader->Create("../../_Assets/Shader/Color.hlsl", "PS", "ps_5_0");

    input_layout = new InputLayout(context);
    input_layout->Create(vertex_shader->GetBlob());

    //Create Constant Buffer
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

    //Create Constant Buffer
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
    SAFE_DELETE(input_layout);
    SAFE_DELETE(pixel_shader);
    SAFE_DELETE(vertex_shader);
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
        graphics->GetDeviceContext()->Map
        (
            camera_buffer,
            0,
            D3D11_MAP_WRITE_DISCARD,
            0,
            &sub_resource
        );

        auto data = static_cast<CameraData*>(sub_resource.pData);

        D3DXMatrixTranspose(&data->view, &camera->GetViewMatrix());
        D3DXMatrixTranspose(&data->projection, &camera->GetProjectionMatrix());

        graphics->GetDeviceContext()->Unmap(camera_buffer, 0);
    }

    //world Mapped
    {
        D3D11_MAPPED_SUBRESOURCE sub_resource;
        ZeroMemory(&sub_resource, sizeof(D3D11_MAPPED_SUBRESOURCE));
        graphics->GetDeviceContext()->Map
        (
            world_buffer,
            0,
            D3D11_MAP_WRITE_DISCARD,
            0,
            &sub_resource
        );

        auto data = static_cast<WorldData*>(sub_resource.pData);

        D3DXMatrixTranspose(&data->world, &world);

        graphics->GetDeviceContext()->Unmap(world_buffer, 0);
    }
}

void Scene::Render()
{
    vertex_buffer->BindPipeline();
    index_buffer->BindPipeline();

    vertex_shader->BindPipeline();
    pixel_shader->BindPipeline();

    input_layout->BindPipeline();
    
    graphics->GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    graphics->GetDeviceContext()->VSSetConstantBuffers(0, 1, &camera_buffer);
    graphics->GetDeviceContext()->VSSetConstantBuffers(1, 1, &world_buffer);

    graphics->GetDeviceContext()->DrawIndexed(geometry.GetIndexCount(), 0, 0);
}
