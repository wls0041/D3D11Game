#pragma once

class Scene final
{
public:
	Scene(class Context *context);
	~Scene();

	void Update();
	void Render();

private:
	class Context *context;
	class Graphics *graphics;
	class Camera *camera;
	
	Geometry<VertexColor> geometry;
	
	VertexBuffer *vertex_buffer;
	IndexBuffer *index_buffer;

	ID3D11VertexShader *vertex_shader;
	ID3DBlob *vs_blob;

	ID3D11PixelShader *pixel_shader;
	ID3DBlob *ps_blob;

	ID3D11InputLayout *input_layout;
	ID3D11Buffer *camera_buffer;
	ID3D11Buffer *world_buffer;

	D3DXMATRIX world;
};