#pragma once

class Skybox final
{
public:
	Skybox(class Context *context, class Camera *camera);
	~Skybox();

	void Update();
	void Render();

private:
	class Context *context;
	class Camera *camera;
	class Graphics *graphics;

	Geometry<VertexTexture> sphere;
	VertexBuffer *vertex_buffer;
	IndexBuffer *index_buffer;
	InputLayout *input_layout;
	VertexShader *vertex_shader;
	PixelShader *pixel_shader;
	ConstantBuffer *constant_buffer;

	D3DXMATRIX world;

	ID3D11ShaderResourceView *environment_map;

	ID3D11RasterizerState* rasterizer_state;
	ID3D11RasterizerState* back_state;

	ID3D11DepthStencilState *depth_state;
	ID3D11DepthStencilState *depth_back_state;
};