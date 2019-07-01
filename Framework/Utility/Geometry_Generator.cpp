#include "Framework.h"
#include "Geometry_Generator.h"

void Geometry_Generator::CreateQuad(Geometry<VertexTexture>& geometry)
{
	geometry.AddVertex(VertexTexture({ -0.5f, -0.5f, 0.0f }, { 0.0f, 1.0f }));
	geometry.AddVertex(VertexTexture({ -0.5f, +0.5f, 0.0f }, { 0.0f, 0.0f }));
	geometry.AddVertex(VertexTexture({ +0.5f, -0.5f, 0.0f }, { 1.0f, 1.0f }));
	geometry.AddVertex(VertexTexture({ +0.5f, +0.5f, 0.0f }, { 1.0f, 0.0f }));

	geometry.AddIndex(0); geometry.AddIndex(1); geometry.AddIndex(2);
	geometry.AddIndex(2); geometry.AddIndex(1); geometry.AddIndex(3);
}

void Geometry_Generator::CreateCube(Geometry<VertexTexture>& geometry)
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

void Geometry_Generator::CreateSphere(Geometry<VertexTexture>& geometry, const float & radius, const int & slices, const int & stacks)
{
	geometry.AddVertex(VertexTexture({ 0, radius, 0 }, { 0, 0 }));

	float phiStep = static_cast<float>(D3DX_PI / stacks);
	float thetaStep = static_cast<float>(D3DX_PI * 2.0 / slices);

	for (int i = 1; i <= stacks - 1; i++)
	{
		float phi = i * phiStep;

		for (int j = 0; j <= slices; j++)
		{
			float theta = j * thetaStep;

			D3DXVECTOR3 position = D3DXVECTOR3
			(
				radius * sin(phi) * cos(theta),
				radius * cos(phi),
				radius * sin(phi) * sin(theta)
			);

			D3DXVECTOR2 uv = D3DXVECTOR2
			(
				static_cast<float>(theta / D3DX_PI * 2.0),
				static_cast<float>(phi / D3DX_PI)
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