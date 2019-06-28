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
	class ConstantBuffer* camera_buffer;

	class Skybox *skybox;
	class Terrain* terrain;

	//====================
	Geometry<VertexTexture> geometry;
	VertexBuffer* vertexBuffer;
	IndexBuffer* indexBuffer;
	InputLayout* inputLayout;
	VertexShader* vertexShader;
	PixelShader* pixelShader;
	ConstantBuffer* worldBuffer;
	D3DXMATRIX world;
};