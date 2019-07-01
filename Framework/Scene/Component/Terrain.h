#pragma once

class Terrain final
{
public:
    Terrain(class Context* context, class Camera* camera);
    ~Terrain();

	auto GetHeight(const D3DXVECTOR3 &position) -> const float;

    void Update();
    void Render();

private:
    void ReadPixel(const std::string& path, std::vector<D3DXCOLOR>& pixels);

    void Raise(const D3D11_BOX& box);
	void RaiseCircle(const D3D11_BOX& box);
	void PaintColor(const D3D11_BOX& box);
	void PaintTexture(const D3D11_BOX& box);

    auto Pick(D3DXVECTOR3& position) -> const bool;

    void UpdateNormal();

private:
    class Context* context;
    class Graphics* graphics;
    class Camera* camera;

    Geometry<VertexTerrain> geometry;
    VertexBuffer* vertex_buffer;
    IndexBuffer* index_buffer;
    VertexShader* vertex_shader;
    PixelShader* pixel_shader;
    InputLayout* input_layout;
    ConstantBuffer* world_buffer;
    ConstantBuffer* brush_buffer;

    D3DXMATRIX world;

	bool bRect;
    uint width;
    uint height;

	ID3D11ShaderResourceView* srv;
	ID3D11ShaderResourceView* srv1;
	ID3D11ShaderResourceView* alpha;

    ID3D11RasterizerState* rasterizer_state;
};