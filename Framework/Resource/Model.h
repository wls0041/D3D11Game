#pragma once

class Model final
{
public:
	Model(class Context *context);
	~Model();

	void SetGeometry(const std::vector<VertexModel> &vertices, const std::vector<uint> &indices);

	void Update();
	void Render();

private:
	class Context *context;
	class Graphics *graphics;

	Geometry<VertexTexture> geometry;
	VertexBuffer* vertexBuffer;
	IndexBuffer* indexBuffer;
	InputLayout* inputLayout;
	VertexShader* vertexShader;
	PixelShader* pixelShader;
	ConstantBuffer* worldBuffer;
	D3DXMATRIX world;
};