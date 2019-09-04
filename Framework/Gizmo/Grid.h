#pragma once

class Grid final
{
public:
	Grid();
	~Grid() = default;

	auto GetComputeWorldMatrix(class Transform *camera_transform) -> const Matrix;
	auto GetVertexBuffer() const -> const std::shared_ptr<class VertexBuffer>&;
	auto GetIndexBuffer() const -> const std::shared_ptr<class IndexBuffer>&;
	auto GetIndexCount() const -> const uint& { return index_count; }
private:
	void UpdateGrid(std::vector<struct VertexColor> &vertices, const std::vector<uint> &indices);

	void CreateBuffers(std::vector<struct VertexColor> &vertices, const std::vector<uint> &indices);

private:
	class Context *context;
	Matrix world;
	uint index_count;
	uint width;
	uint height;

	std::shared_ptr<class VertexBuffer> vertex_buffer;
	std::shared_ptr<class IndexBuffer> index_buffer;
};