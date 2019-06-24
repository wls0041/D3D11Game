#pragma once

class Terrain final
{
public:
    Terrain(class Context* context);
    ~Terrain();

    void Update();
    void Render();

private:
    void ReadPixel(const std::string& path, std::vector<D3DXCOLOR>& pixels);

private:
    class Context* context;
    class Graphics* graphics;

    Geometry<VertexColor> geometry;
    VertexBuffer* vertex_buffer;
    IndexBuffer* index_buffer;
    VertexShader* vertex_shader;
    PixelShader* pixel_shader;
    InputLayout* input_layout;
    ConstantBuffer* world_buffer;

    D3DXMATRIX world;

    uint width;
    uint height;

    ID3D11RasterizerState* rasterizer_state;
};