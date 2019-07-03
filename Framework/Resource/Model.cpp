#include "Framework.h"
#include "Model.h"
#include "./ModelImporter/FbxType.h"

Model::Model(Context * context)
    : context(context)
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
}

Model::~Model()
{
    for (auto& mesh : meshes)
    {
        SAFE_DELETE(mesh.indexBuffer);
        SAFE_DELETE(mesh.vertexBuffer);
    }

    SAFE_DELETE(inputLayout);
    SAFE_DELETE(vertexShader);
    SAFE_DELETE(pixelShader);
    SAFE_DELETE(worldBuffer);
}

void Model::AddGeometry(const FbxMeshData & mesh_data)
{
    Mesh mesh;
    mesh.vertexBuffer = new VertexBuffer(context);
    mesh.vertexBuffer->Clear();
    mesh.vertexBuffer->Create(mesh_data.vertices);

    mesh.indexBuffer = new IndexBuffer(context);
    mesh.indexBuffer->Clear();
    mesh.indexBuffer->Create(mesh_data.indices);

    meshes.emplace_back(mesh);
}

void Model::AddMaterial(const FbxMaterialData & material_data)
{
    Material material;
    auto hr = D3DX11CreateShaderResourceViewFromFileA
    (
        graphics->GetDevice(),
        material_data.albedo_texture_path.c_str(),
        nullptr,
        nullptr,
        &material.diffuse,
        nullptr
    );
    assert(SUCCEEDED(hr));

    materials.emplace_back(material);
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
    inputLayout->BindPipeline();
    vertexShader->BindPipeline();
    pixelShader->BindPipeline();
    worldBuffer->BindPipeline(1, ShaderScope::VS);
    graphics->GetDeviceContext()->PSSetShaderResources(0, 1, &materials[0].diffuse);
    graphics->GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    for (auto& mesh : meshes)
    {
        mesh.vertexBuffer->BindPipeline();
        mesh.indexBuffer->BindPipeline();
        graphics->GetDeviceContext()->DrawIndexed(mesh.indexBuffer->GetCount(), 0, 0);
    }
}
