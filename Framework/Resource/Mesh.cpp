#include "Framework.h"
#include "Mesh.h"

Mesh::Mesh(Context * context)
	: IResource(context, ResourceType::Mesh)
	, primitive_topology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST)
	, mesh_type(MeshType::Custom)
{
}

Mesh::~Mesh()
{
}

const bool Mesh::SaveToFile(const std::string & path)
{
	return false;
}

const bool Mesh::LoadFromFile(const std::string & path)
{
	return false;
}

void Mesh::SetMeshType(const MeshType & type)
{
	if (mesh_type == type) return;

	mesh_type = type;
	
	Clear();
	if (vertex_buffer) vertex_buffer->Clear();
	if (index_buffer) index_buffer->Clear();
}

void Mesh::CreateBuffers()
{
	if (!vertex_buffer) vertex_buffer = std::make_shared<VertexBuffer>(context);
	if (!index_buffer) index_buffer = std::make_shared<IndexBuffer>(context);
	
	switch (mesh_type)
	{
	case MeshType::Custom:
		break;
	case MeshType::Cube:
		SetResourceName("Cube");
		Geometry_Generator::CreateCube(*this);
		break;
	case MeshType::Cylinder:
		SetResourceName("Cylinder");
		Geometry_Generator::CreateCylinder(*this);
		break;
	case MeshType::Cone:
		SetResourceName("Cone");
		Geometry_Generator::CreateCone(*this);
		break;
	case MeshType::Sphere:
		SetResourceName("Sphere");
		Geometry_Generator::CreateSphere(*this);
		break;
	case MeshType::Quad:
		SetResourceName("Quad");
		Geometry_Generator::CreateQuad(*this);
		break;
	case MeshType::ScreenQuad:
		SetResourceName("ScreenQuad");
		Geometry_Generator::CreateScreenQuad(*this);
		break;
	default:
		break;
	}

	vertex_buffer->Create(vertices);
	index_buffer->Create(indices);
}
