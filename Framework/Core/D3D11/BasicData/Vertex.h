#pragma once

//Stack
//Heap
//---------------
//Data
//Code

struct VertexColor final
{
	VertexColor()
		: position(0, 0, 0)
		, color(0, 0, 0, 1)
	{}

	VertexColor(const D3DXVECTOR3& position, const D3DXCOLOR& color)
		: position(position)
		, color(color)
	{}

	D3DXVECTOR3 position;
	D3DXCOLOR color;

	static D3D11_INPUT_ELEMENT_DESC descs[];
	static const uint count = 2;
};

struct VertexTexture final
{
	VertexTexture()
		: position(0, 0, 0)
		, uv(0, 0)
	{}

	VertexTexture(const D3DXVECTOR3& position, const D3DXVECTOR2& uv)
		: position(position)
		, uv(uv)
	{}

	D3DXVECTOR3 position;
	D3DXVECTOR2 uv;

	static D3D11_INPUT_ELEMENT_DESC descs[];
	static const uint count = 2;
};

struct VertexTextureNormal final
{
	VertexTextureNormal()
		: position(0, 0, 0)
		, uv(0, 0)
		, normal(0, 0, 0)
	{}

	VertexTextureNormal(const D3DXVECTOR3& position, const D3DXVECTOR2& uv, const D3DXVECTOR3& normal)
		: position(position)
		, uv(uv)
		, normal(normal)
	{}

	D3DXVECTOR3 position;
	D3DXVECTOR2 uv;
	D3DXVECTOR3 normal;

	static D3D11_INPUT_ELEMENT_DESC descs[];
	static const uint count = 3;
};

struct VertexTerrain final
{
	VertexTerrain()
		: position(0, 0, 0)
		, uv(0, 0)
		, normal(0, 0, 0)
		, color(0, 0, 0, 0)
		, alpha(0, 0, 0, 0)
	{}

	VertexTerrain
	(
		const D3DXVECTOR3& position,
		const D3DXVECTOR2& uv,
		const D3DXVECTOR3& normal,
		const D3DXCOLOR& color
	)
		: position(position)
		, uv(uv)
		, normal(normal)
		, color(color)
		, alpha(0, 0, 0, 0)
	{}

	D3DXVECTOR3 position;
	D3DXVECTOR2 uv;
	D3DXVECTOR3 normal;
	D3DXCOLOR color;
	D3DXCOLOR alpha;

	static D3D11_INPUT_ELEMENT_DESC descs[];
	static const uint count = 4;
};