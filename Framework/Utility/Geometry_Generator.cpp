#include "Framework.h"
#include "Geometry_Generator.h"

void Geometry_Generator::CreateQuad(Geometry<struct VertexTexture>& geometry)
{
	geometry.AddVertex(VertexTexture({ -0.5f, -0.5f, 0.0f }, { 0.0f, 1.0f }));
	geometry.AddVertex(VertexTexture({ -0.5f, +0.5f, 0.0f }, { 0.0f, 0.0f }));
	geometry.AddVertex(VertexTexture({ +0.5f, -0.5f, 0.0f }, { 1.0f, 1.0f }));
	geometry.AddVertex(VertexTexture({ +0.5f, +0.5f, 0.0f }, { 1.0f, 0.0f }));

	geometry.AddIndex(0); geometry.AddIndex(1); geometry.AddIndex(2);
	geometry.AddIndex(2); geometry.AddIndex(1); geometry.AddIndex(3);
}

void Geometry_Generator::CreateScreenQuad(Geometry<struct VertexTextureNormalTangentBlend>& geometry)
{
}

void Geometry_Generator::CreateScreenQuad(Geometry<struct VertexTexture>& geometry, const uint &width, const uint &height) //화면 전체크기가 아닌 resolution기준이어야 해서 따로 크기 받음
{
	const auto half_width = width * 0.5f;
	const auto half_height = height * 0.5f;

	geometry.AddVertex(VertexTexture({ half_width, -half_height, 0.0f }, { 0.0f, 1.0f }));
	geometry.AddVertex(VertexTexture({ half_width, +half_height, 0.0f }, { 0.0f, 0.0f }));
	geometry.AddVertex(VertexTexture({ half_width, -half_height, 0.0f }, { 1.0f, 1.0f }));
	geometry.AddVertex(VertexTexture({ half_width, +half_height, 0.0f }, { 1.0f, 0.0f }));

	geometry.AddIndex(0); geometry.AddIndex(1); geometry.AddIndex(2);
	geometry.AddIndex(2); geometry.AddIndex(1); geometry.AddIndex(3);
}

void Geometry_Generator::CreateCube(Geometry<struct VertexTexture>& geometry)
{
	//front
	geometry.AddVertex(VertexTexture({ -0.5f, -0.5f, -0.5f }, { 0, 1 }));
	geometry.AddVertex(VertexTexture({ -0.5f, +0.5f, -0.5f }, { 0, 0 }));
	geometry.AddVertex(VertexTexture({ +0.5f, -0.5f, -0.5f }, { 1, 1 }));
	geometry.AddVertex(VertexTexture({ +0.5f, +0.5f, -0.5f }, { 1, 0 }));

	//bottom
	geometry.AddVertex(VertexTexture({ -0.5f, -0.5f, +0.5f }, { 0, 1 }));
	geometry.AddVertex(VertexTexture({ -0.5f, -0.5f, -0.5f }, { 0, 0 }));
	geometry.AddVertex(VertexTexture({ +0.5f, -0.5f, +0.5f }, { 1, 1 }));
	geometry.AddVertex(VertexTexture({ +0.5f, -0.5f, -0.5f }, { 1, 0 }));

	//back
	geometry.AddVertex(VertexTexture({ -0.5f, -0.5f, +0.5f }, { 1, 1 }));
	geometry.AddVertex(VertexTexture({ -0.5f, +0.5f, +0.5f }, { 1, 0 }));
	geometry.AddVertex(VertexTexture({ +0.5f, -0.5f, +0.5f }, { 0, 1 }));
	geometry.AddVertex(VertexTexture({ +0.5f, +0.5f, +0.5f }, { 0, 0 }));

	//top
	geometry.AddVertex(VertexTexture({ -0.5f, +0.5f, +0.5f }, { 0, 0 }));
	geometry.AddVertex(VertexTexture({ -0.5f, +0.5f, -0.5f }, { 0, 1 }));
	geometry.AddVertex(VertexTexture({ +0.5f, +0.5f, +0.5f }, { 1, 0 }));
	geometry.AddVertex(VertexTexture({ +0.5f, +0.5f, -0.5f }, { 1, 1 }));

	//left
	geometry.AddVertex(VertexTexture({ -0.5f, -0.5f, +0.5f }, { 0, 1 }));
	geometry.AddVertex(VertexTexture({ -0.5f, +0.5f, +0.5f }, { 0, 0 }));
	geometry.AddVertex(VertexTexture({ -0.5f, -0.5f, -0.5f }, { 1, 1 }));
	geometry.AddVertex(VertexTexture({ -0.5f, +0.5f, -0.5f }, { 1, 0 }));

	//right
	geometry.AddVertex(VertexTexture({ +0.5f, -0.5f, +0.5f }, { 1, 1 }));
	geometry.AddVertex(VertexTexture({ +0.5f, +0.5f, +0.5f }, { 1, 0 }));
	geometry.AddVertex(VertexTexture({ +0.5f, -0.5f, -0.5f }, { 0, 1 }));
	geometry.AddVertex(VertexTexture({ +0.5f, +0.5f, -0.5f }, { 0, 0 }));

	//front
	geometry.AddIndex(0); geometry.AddIndex(1); geometry.AddIndex(2);
	geometry.AddIndex(2); geometry.AddIndex(1); geometry.AddIndex(3);

	//bottom
	geometry.AddIndex(4); geometry.AddIndex(5); geometry.AddIndex(6);
	geometry.AddIndex(6); geometry.AddIndex(5); geometry.AddIndex(7);

	//back
	geometry.AddIndex(10); geometry.AddIndex(9); geometry.AddIndex(8);
	geometry.AddIndex(11); geometry.AddIndex(9); geometry.AddIndex(10);

	//top
	geometry.AddIndex(14); geometry.AddIndex(13); geometry.AddIndex(12);
	geometry.AddIndex(15); geometry.AddIndex(13); geometry.AddIndex(14);

	//left
	geometry.AddIndex(16); geometry.AddIndex(17); geometry.AddIndex(18);
	geometry.AddIndex(18); geometry.AddIndex(17); geometry.AddIndex(19);

	//right
	geometry.AddIndex(22); geometry.AddIndex(21); geometry.AddIndex(20);
	geometry.AddIndex(23); geometry.AddIndex(21); geometry.AddIndex(22);
}

void Geometry_Generator::CreateCube(Geometry<struct VertexTextureNormalTangentBlend> &geometry)
{
}

void Geometry_Generator::CreateCylinder(Geometry<struct VertexTextureNormalTangentBlend>& geometry)
{
}

void Geometry_Generator::CreateCone(Geometry<struct VertexTextureNormalTangentBlend>& geometry)
{
}

void Geometry_Generator::CreateQuad(Geometry<struct VertexTextureNormalTangentBlend> &geometry)
{
}

void Geometry_Generator::CreateSphere(Geometry<struct VertexTextureNormalTangentBlend>& geometry)
{
}

void Geometry_Generator::CreateSphere(Geometry<struct VertexTexture>& geometry, const float & radius, const int & slices, const int & stacks)
{
	geometry.AddVertex(VertexTexture({ 0, radius, 0 }, { 0, 0 }));

	float phiStep = static_cast<float>(Math::PI / stacks);
	float thetaStep = static_cast<float>(Math::PI * 2.0 / slices);

	for (int i = 1; i <= stacks - 1; i++)
	{
		float phi = i * phiStep;

		for (int j = 0; j <= slices; j++)
		{
			float theta = j * thetaStep;

			Vector3 position = Vector3
			(
				radius * sin(phi) * cos(theta),
				radius * cos(phi),
				radius * sin(phi) * sin(theta)
			);

			Vector2 uv = Vector2
			(
				static_cast<float>(theta / Math::PI * 2.0),
				static_cast<float>(phi / Math::PI)
			);

			geometry.AddVertex(VertexTexture(position, uv));
		}
	}

	geometry.AddVertex(VertexTexture({ 0, -radius, 0 }, { 0, 1 }));

	for (int i = 1; i <= slices; i++)
	{
		geometry.AddIndex(0);
		geometry.AddIndex(i + 1);
		geometry.AddIndex(i);
	}

	int baseIndex = 1;
	int ringVertexCount = slices + 1;

	for (int i = 0; i < stacks - 2; i++)
		for (int j = 0; j < slices; j++)
		{
			geometry.AddIndex(baseIndex + i * ringVertexCount + j);
			geometry.AddIndex(baseIndex + i * ringVertexCount + j + 1);
			geometry.AddIndex(baseIndex + (i + 1) * ringVertexCount + j);

			geometry.AddIndex(baseIndex + (i + 1) * ringVertexCount + j);
			geometry.AddIndex(baseIndex + i * ringVertexCount + j + 1);
			geometry.AddIndex(baseIndex + (i + 1) * ringVertexCount + j + 1);
		}

	int southPoleIndex = static_cast<int>(geometry.GetVertexCount() - 1);
	baseIndex = southPoleIndex - ringVertexCount;

	for (int i = 0; i < slices; i++)
	{
		geometry.AddIndex(southPoleIndex);
		geometry.AddIndex(baseIndex + i);
		geometry.AddIndex(baseIndex + i + 1);
	}
}
