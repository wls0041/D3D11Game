#pragma once

struct Material {
	ID3D11ShaderResourceView *diffuse;
};

struct Mesh {
	VertexBuffer* vertexBuffer;
	IndexBuffer* indexBuffer;
};

class Model final
{
public:
	Model(class Context *context);
	~Model();

	void AddGeometry(const struct FbxMeshData &mesh_data);
	void AddMaterial(const struct FbxMaterialData &material_data);
	void AddMaterial();

	void Update();
	void Render();

private:
	class Context *context;
	class Graphics *graphics;

	InputLayout* inputLayout;
	VertexShader* vertexShader;
	PixelShader* pixelShader;
	ConstantBuffer* worldBuffer;
	D3DXMATRIX world;

	std::vector<Mesh> meshes;
	std::vector<Material> materials;
};