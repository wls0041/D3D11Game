#pragma once

class MultiCube final
{
public:
	MultiCube(class Context *context, class Camera* camera, const D3DXVECTOR3 &position);
	~MultiCube();

	void Update();
	void Render();

	auto Pick(D3DXVECTOR3& position) -> const bool;

private:
	class Context* context;
	class Graphics* graphics;
	class Camera* camera;

	D3DXVECTOR3 position;

	Geometry<VertexTexture> geometry;
	VertexBuffer* vertexBuffer;
	IndexBuffer* indexBuffer;
	InputLayout* inputLayout;
	VertexShader* vertexShader;
	PixelShader* pixelShader;
	ConstantBuffer* worldBuffer;
	ConstantBuffer* multi_buffer;

	D3DXMATRIX world;
};