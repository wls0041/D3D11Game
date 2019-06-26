#pragma once

class Terrain final
{
public:
    Terrain(class Context* context, class Camera* camera);
    ~Terrain();

    void Update();
    void Render();

private:
    void ReadPixel(const std::string& path, std::vector<D3DXCOLOR>& pixels);

    void Raise(const D3D11_BOX& box);

    auto Pick(D3DXVECTOR3& position) -> const bool;

    void UpdateNormal();

private:
    class Context* context;
    class Graphics* graphics;
    class Camera* camera;

    Geometry<VertexTextureNormal> geometry;
    VertexBuffer* vertex_buffer;
    IndexBuffer* index_buffer;
    VertexShader* vertex_shader;
    PixelShader* pixel_shader;
    InputLayout* input_layout;
    ConstantBuffer* world_buffer;
    ConstantBuffer* brush_buffer;

    D3DXMATRIX world;

    uint width;
    uint height;

    ID3D11ShaderResourceView* srv;

    ID3D11RasterizerState* rasterizer_state;
};