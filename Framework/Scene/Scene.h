#pragma once

class Scene final
{
public:
    Scene(class Context* context);
    ~Scene();

    void Update();
    void Render();

private:
    class Context* context;
    class Graphics* graphics;
    class Camera* camera;

    Geometry<VertexColor> geometry;

    VertexBuffer* vertex_buffer;
    IndexBuffer* index_buffer;
    
    VertexShader* vertex_shader;
    PixelShader* pixel_shader;

    InputLayout* input_layout;

    ID3D11Buffer* camera_buffer;
    ID3D11Buffer* world_buffer;

    D3DXMATRIX world;
};