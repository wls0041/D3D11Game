#pragma once

template <typename T> class Geometry;

class Geometry_Generator final
{
public:
	static void CreateQuad(Geometry<struct VertexTexture>& geometry);
	static void CreateCube(Geometry<struct VertexTexture>& geometry);
	static void CreateSphere
	(
		Geometry<struct VertexTexture>& geometry,
		const float& radius = 1.0f,
		const int& slices = 15,
		const int& stacks = 15
	);
};