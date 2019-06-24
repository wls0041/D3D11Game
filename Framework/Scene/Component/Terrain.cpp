#include "Framework.h"
#include "Terrain.h"

Terrain::Terrain(Context * context)
    : context(context)
    , width(100)
    , height(100)
{
    graphics = context->GetSubsystem<Graphics>();

    //. . .
    //. . .
    //. . .

    for (uint z = 0; z <= height; z++)
        for (uint x = 0; x <= width; x++)
        {
            geometry.AddVertex
            (
                VertexColor
                (
                    D3DXVECTOR3
                    (
                        static_cast<float>(x), 
                        0.0f, 
                        static_cast<float>(z)), 
                    D3DXCOLOR(0, 1, 0, 1)
                )
            );
        }

    for (uint z = 0; z < height; z++)
        for (uint x = 0; x < width; x++)
        {
            geometry.AddIndex((width + 1) * z + x); //0
            geometry.AddIndex((width + 1) * (z + 1) + x); //1
            geometry.AddIndex((width + 1) * z + (x + 1)); //2
            geometry.AddIndex((width + 1) * z + (x + 1)); //2
            geometry.AddIndex((width + 1) * (z + 1) + x); //1
            geometry.AddIndex((width + 1) * (z + 1) + (x + 1)); //3
        }

    vertex_buffer = new VertexBuffer(context);
    vertex_buffer->Create(geometry.GetVertices());

    index_buffer = new IndexBuffer(context);
    index_buffer->Create(geometry.GetIndices());

    vertex_shader = new VertexShader(context);
    vertex_shader->Create("../../_Assets/Shader/Color.hlsl", "VS", "vs_5_0");

    pixel_shader = new PixelShader(context);
    pixel_shader->Create("../../_Assets/Shader/Color.hlsl", "PS", "ps_5_0");

    input_layout = new InputLayout(context);
    input_layout->Create(vertex_shader->GetBlob());

    world_buffer = new ConstantBuffer(context);
    world_buffer->Create<WorldData>();

    D3DXMatrixIdentity(&world);

    //Create Rasterizer State
    {
        D3D11_RASTERIZER_DESC desc;
        ZeroMemory(&desc, sizeof(D3D11_RASTERIZER_DESC));
        desc.FillMode = D3D11_FILL_WIREFRAME;
        desc.CullMode = D3D11_CULL_BACK;
        desc.FrontCounterClockwise = false;

        auto hr = graphics->GetDevice()->CreateRasterizerState(&desc, &rasterizer_state);
        assert(SUCCEEDED(hr));
    }
}

Terrain::~Terrain()
{
    SAFE_RELEASE(rasterizer_state);
    SAFE_DELETE(world_buffer);
    SAFE_DELETE(input_layout);
    SAFE_DELETE(pixel_shader);
    SAFE_DELETE(vertex_shader);
    SAFE_DELETE(index_buffer);
    SAFE_DELETE(vertex_buffer);
}

void Terrain::Update()
{
    auto world_data = world_buffer->Map<WorldData>();
    {
        D3DXMatrixTranspose(&world_data->world, &world);
    }
    world_buffer->Unmap();
}

void Terrain::Render()
{
    vertex_buffer->BindPipeline();
    index_buffer->BindPipeline();
    input_layout->BindPipeline();
    vertex_shader->BindPipeline();
    pixel_shader->BindPipeline();
    world_buffer->BindPipeline(1, ShaderScope::VS);

    graphics->GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    graphics->GetDeviceContext()->RSSetState(rasterizer_state);
    graphics->GetDeviceContext()->DrawIndexed(geometry.GetIndexCount(), 0, 0);
}

void Terrain::ReadPixel(const std::string & path, std::vector<D3DXCOLOR>& pixels)
{
    ID3D11Texture2D* height_map = nullptr;
    auto hr = D3DX11CreateTextureFromFileA
    (
        graphics->GetDevice(),
        path.c_str(),
        nullptr,
        nullptr,
        reinterpret_cast<ID3D11Resource**>(&height_map),
        nullptr
    );
    assert(SUCCEEDED(hr));

    D3D11_TEXTURE2D_DESC height_map_desc;
    height_map->GetDesc(&height_map_desc);

    width   = height_map_desc.Width;
    height  = height_map_desc.Height;

    //============================================================

    D3D11_TEXTURE2D_DESC dst_desc;
    ZeroMemory(&dst_desc, sizeof(D3D11_TEXTURE2D_DESC));
    dst_desc.Width = height_map_desc.Width;
    dst_desc.Height = height_map_desc.Height;
    dst_desc.MipLevels = 1;
    dst_desc.ArraySize = 1;
    dst_desc.Format = height_map_desc.Format;
    dst_desc.SampleDesc = height_map_desc.SampleDesc;
    dst_desc.Usage = D3D11_USAGE_STAGING;
    dst_desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE | D3D11_CPU_ACCESS_READ;

    ID3D11Texture2D* dst_texture = nullptr;
    hr = graphics->GetDevice()->CreateTexture2D(&dst_desc, nullptr, &dst_texture);

    hr = D3DX11LoadTextureFromTexture
    (
        graphics->GetDeviceContext(),
        height_map,
        nullptr,
        dst_texture
    );
    assert(SUCCEEDED(hr));
}
