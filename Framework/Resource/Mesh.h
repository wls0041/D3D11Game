#pragma once
#include "IResource.h"

enum class MeshType : uint
{
	Custom,
	Cube,
	Cylinder,
	Cone,
	Sphere,
	Quad,
	ScreenQuad,
};

class Mesh final : public IResource, public Geometry<VertexModel> //다중 상속. 그다지 추천하는 방식은 아님
{
public:
	Mesh(class Context *context);
	~Mesh();

	const bool SaveToFile(const std::string &path) override;
	const bool LoadFromFile(const std::string &path) override;

	auto GetMeshType() const -> const MeshType& { return mesh_type; }
	void SetMeshType(const MeshType &type);
	
	auto GetPrimitiveTopology() const -> const D3D11_PRIMITIVE_TOPOLOGY& { return topology; }
	auto SetPrimitiveTopology(const D3D11_PRIMITIVE_TOPOLOGY &topology) { this->topology = topology; }

	auto GetVertexBuffer() -> const std::shared_ptr<VertexBuffer>& { return vertex_buffer; }
	auto GetIndexBuffer() -> const std::shared_ptr<IndexBuffer>& { return index_buffer; }

	void CreateBuffers();

private:
	std::shared_ptr<VertexBuffer> vertex_buffer;
	std::shared_ptr<IndexBuffer> index_buffer;
	D3D11_PRIMITIVE_TOPOLOGY topology;
	MeshType mesh_type;
};