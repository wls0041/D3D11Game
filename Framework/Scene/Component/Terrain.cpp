#include "Framework.h"
#include "Terrain.h"
#include "Camera.h"

Terrain::Terrain(Context * context, Camera* camera)
	: context(context)
	, camera(camera)
	, width(0)
	, height(0)
	, bRect(true)
{
	graphics = context->GetSubsystem<Graphics>();

	std::vector<D3DXCOLOR> pixels;
	ReadPixel("../../_Assets/Texture/HeightMap.bmp", pixels);

	for (uint z = 0; z < height; z++)
		for (uint x = 0; x < width; x++)
		{
			uint index = width * z + x;
			geometry.AddVertex
			(
				VertexTerrain
				(
					D3DXVECTOR3
					(
						static_cast<float>(x),
						static_cast<float>(pixels[index].r * 255.0f / 7.5f),
						static_cast<float>(z)
					),
					D3DXVECTOR2
					(
						static_cast<float>(x) / width,
						static_cast<float>(z) / height
					),
					D3DXVECTOR3(0, 0, 0),
					D3DXCOLOR(0, 0, 0, 0)
				)
			);
		}

	for (uint z = 0; z < height - 1; z++)
		for (uint x = 0; x < width - 1; x++)
		{
			geometry.AddIndex(width * z + x); //0
			geometry.AddIndex(width * (z + 1) + x); //1
			geometry.AddIndex(width * z + (x + 1)); //2
			geometry.AddIndex(width * z + (x + 1)); //2
			geometry.AddIndex(width * (z + 1) + x); //1
			geometry.AddIndex(width * (z + 1) + (x + 1)); //3
		}

	UpdateNormal();

	vertex_buffer = new VertexBuffer(context);
	vertex_buffer->Create(geometry.GetVertices());

	index_buffer = new IndexBuffer(context);
	index_buffer->Create(geometry.GetIndices());

	vertex_shader = new VertexShader(context);
	vertex_shader->Create("../../_Assets/Shader/Terrain.hlsl", "VS", "vs_5_0");

	pixel_shader = new PixelShader(context);
	pixel_shader->Create("../../_Assets/Shader/Terrain.hlsl", "PS", "ps_5_0");

	input_layout = new InputLayout(context);
	input_layout->Create(vertex_shader->GetBlob());

	world_buffer = new ConstantBuffer(context);
	world_buffer->Create<WorldData>();

	brush_buffer = new ConstantBuffer(context);
	brush_buffer->Create<BrushData>();

	D3DXMatrixIdentity(&world);

	//Create Shader Resource View
	{
		auto hr = D3DX11CreateShaderResourceViewFromFileA
		(
			graphics->GetDevice(),
			"../../_Assets/Texture/Grass.png",
			nullptr,
			nullptr,
			&srv,
			nullptr
		);
		assert(SUCCEEDED(hr));
	}

	//Create Shader Resource View
	{
		auto hr = D3DX11CreateShaderResourceViewFromFileA
		(
			graphics->GetDevice(),
			"../../_Assets/Texture/Dirt.png",
			nullptr,
			nullptr,
			&srv1,
			nullptr
		);
		assert(SUCCEEDED(hr));
	}

	//Create Shader Resource View
	{
		auto hr = D3DX11CreateShaderResourceViewFromFileA
		(
			graphics->GetDevice(),
			"../../_Assets/Texture/AlphaMap.png",
			nullptr,
			nullptr,
			&alpha,
			nullptr
		);
		assert(SUCCEEDED(hr));
	}

	//Create Rasterizer State
	{
		D3D11_RASTERIZER_DESC desc;
		ZeroMemory(&desc, sizeof(D3D11_RASTERIZER_DESC));
		desc.FillMode = D3D11_FILL_SOLID;
		desc.CullMode = D3D11_CULL_BACK;
		desc.FrontCounterClockwise = false;

		auto hr = graphics->GetDevice()->CreateRasterizerState(&desc, &rasterizer_state);
		assert(SUCCEEDED(hr));
	}
}

Terrain::~Terrain()
{
	SAFE_RELEASE(rasterizer_state);
	SAFE_RELEASE(alpha);
	SAFE_RELEASE(srv1);
	SAFE_RELEASE(srv);

	SAFE_DELETE(brush_buffer);
	SAFE_DELETE(world_buffer);
	SAFE_DELETE(input_layout);
	SAFE_DELETE(pixel_shader);
	SAFE_DELETE(vertex_shader);
	SAFE_DELETE(index_buffer);
	SAFE_DELETE(vertex_buffer);
}

auto Terrain::GetHeight(const D3DXVECTOR3 & position) -> const float
{
	auto x = static_cast<uint>(position.x);
	auto z = static_cast<uint>(position.z);

	if (x < 0 || z < 0 || x > width || z > height)
		return 0.0f;

	auto vertices = geometry.GetVertexData();

	auto index0 = width * z + x;
	auto index1 = width * (z + 1) + x;
	auto index2 = width * z + (x + 1);
	auto index3 = width * (z + 1) + (x + 1);

	auto v0 = vertices[index0].position;
	auto v1 = vertices[index1].position;
	auto v2 = vertices[index2].position;
	auto v3 = vertices[index3].position;

	auto dx = position.x - v0.x;
	auto dz = position.z - v0.z;

	D3DXVECTOR3 temp;
	if (dx + dz <= 1) // 아랫 삼각형
		temp = v0 + (v2 - v0) * dx + (v1 - v0) * dz;
	else
	{
		dx = 1 - dx;
		dz = 1 - dz;

		temp = v3 + (v1 - v3) * dx + (v2 - v3) * dz;
	}

	return temp.y;
}

void Terrain::Update()
{
	auto world_data = world_buffer->Map<WorldData>();
	{
		D3DXMatrixTranspose(&world_data->world, &world);
	}
	world_buffer->Unmap();

	static D3DXVECTOR3 position;
	auto picked = Pick(position);

	if (picked)
	{
		auto brush_data = brush_buffer->Map<BrushData>();
		{
			brush_data->brush_type = 1;
			brush_data->brush_position = position;
			brush_data->brush_range = 10;
			brush_data->brush_color = D3DXVECTOR3(0, 0, 1);
		}
		brush_buffer->Unmap();

		//종속성 오류
		//if (context->GetSubsystem<Input>()->KeyPress(KeyCode::KEY_R)) bRect != bRect; 

		if (context->GetSubsystem<Input>()->BtnPress(KeyCode::CLICK_LEFT))
		{
			auto left = static_cast<uint>(position.x) - static_cast<uint>(10);
			auto right = static_cast<uint>(position.x) + static_cast<uint>(10);
			auto bottom = static_cast<uint>(position.z) - static_cast<uint>(10);
			auto top = static_cast<uint>(position.z) + static_cast<uint>(10);

			D3D11_BOX box;
			box.left = left < 0 ? 0 : left;
			box.top = top >= height ? height : top;
			box.right = right >= width ? width : right;
			box.bottom = bottom < 0 ? 0 : bottom;

			//if(bRect) RaiseCircle(box);

			//Raise(box);
			//PaintColor(box);
			PaintTexture(box);

			//Flatting(box); 오류
			//Down(box);
		}
	}
	
}

void Terrain::Render()
{
	vertex_buffer->BindPipeline();
	index_buffer->BindPipeline();
	input_layout->BindPipeline();
	vertex_shader->BindPipeline();
	pixel_shader->BindPipeline();
	world_buffer->BindPipeline(1, ShaderScope::VS);
	brush_buffer->BindPipeline(2, ShaderScope::VS);

	graphics->GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	graphics->GetDeviceContext()->PSSetShaderResources(0, 1, &srv);
	graphics->GetDeviceContext()->PSSetShaderResources(1, 1, &srv1);
	graphics->GetDeviceContext()->PSSetShaderResources(2, 1, &alpha);
	graphics->GetDeviceContext()->RSSetState(rasterizer_state);
	graphics->GetDeviceContext()->DrawIndexed(geometry.GetIndexCount(), 0, 0);
}

void Terrain::ReadPixel(const std::string & path, std::vector<D3DXCOLOR>& pixels)
{
	ID3D11Texture2D* height_map = nullptr;
	auto hr = D3DX11CreateTextureFromFileA
	(
		graphics->GetDevice(),
		path.c_str(),
		nullptr,
		nullptr,
		reinterpret_cast<ID3D11Resource**>(&height_map),
		nullptr
	);
	assert(SUCCEEDED(hr));

	D3D11_TEXTURE2D_DESC height_map_desc;
	height_map->GetDesc(&height_map_desc);

	width = height_map_desc.Width;
	height = height_map_desc.Height;

	//============================================================

	D3D11_TEXTURE2D_DESC dst_desc;
	ZeroMemory(&dst_desc, sizeof(D3D11_TEXTURE2D_DESC));
	dst_desc.Width = height_map_desc.Width;
	dst_desc.Height = height_map_desc.Height;
	dst_desc.MipLevels = 1;
	dst_desc.ArraySize = 1;
	dst_desc.Format = height_map_desc.Format;
	dst_desc.SampleDesc = height_map_desc.SampleDesc;
	dst_desc.Usage = D3D11_USAGE_STAGING;
	dst_desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE | D3D11_CPU_ACCESS_READ;

	ID3D11Texture2D* dst_texture = nullptr;
	hr = graphics->GetDevice()->CreateTexture2D(&dst_desc, nullptr, &dst_texture);

	hr = D3DX11LoadTextureFromTexture
	(
		graphics->GetDeviceContext(),
		height_map,
		nullptr,
		dst_texture
	);
	assert(SUCCEEDED(hr));

	std::vector<uint> colors(height_map_desc.Width * height_map_desc.Height, uint());

	D3D11_MAPPED_SUBRESOURCE mapped_resource;
	graphics->GetDeviceContext()->Map
	(
		dst_texture,
		0,
		D3D11_MAP_READ,
		0,
		&mapped_resource
	);

	memcpy(colors.data(), mapped_resource.pData, sizeof(uint) * colors.size());

	graphics->GetDeviceContext()->Unmap(dst_texture, 0);

	for (uint z = 0; z < height_map_desc.Height; z++)
		for (uint x = 0; x < height_map_desc.Width; x++)
		{
			uint index = height_map_desc.Width * z + x;

			const float f = 1.0f / 255.0f;

			float a = f * static_cast<float>(static_cast<unsigned char>(colors[index] >> 24));
			float r = f * static_cast<float>(static_cast<unsigned char>(colors[index] >> 16));
			float g = f * static_cast<float>(static_cast<unsigned char>(colors[index] >> 8));
			float b = f * static_cast<float>(static_cast<unsigned char>(colors[index] >> 0));

			pixels.emplace_back(r, g, b, a);
		}

	colors.clear();
	colors.shrink_to_fit();

	SAFE_RELEASE(dst_texture);
	SAFE_RELEASE(height_map);
}

void Terrain::Flatting(const D3D11_BOX & box)
{
	auto vertices = geometry.GetVertexData();

	for (uint z = box.bottom; z < box.top; z++)
		for (uint x = box.left; x < box.right; x++)
		{
			uint index = width * z + x;
			if(vertices[index].position.y != 0.0f)
				vertices[index].position.y = 0.0f;
		}

	UpdateNormal();

	graphics->GetDeviceContext()->UpdateSubresource
	(
		vertex_buffer->GetResource(),
		0,
		nullptr,
		vertices,
		sizeof(VertexTerrain) * geometry.GetVertexCount(),
		0
	);
}

void Terrain::Down(const D3D11_BOX & box)
{
	auto vertices = geometry.GetVertexData();

	for (uint z = box.bottom; z < box.top; z++)
		for (uint x = box.left; x < box.right; x++)
		{
			uint index = width * z + x;
			vertices[index].position.y -= 5.0f;
		}

	UpdateNormal();

	graphics->GetDeviceContext()->UpdateSubresource
	(
		vertex_buffer->GetResource(),
		0,
		nullptr,
		vertices,
		sizeof(VertexTerrain) * geometry.GetVertexCount(),
		0
	);
}

void Terrain::Raise(const D3D11_BOX & box)
{
	auto vertices = geometry.GetVertexData();

	for (uint z = box.bottom; z < box.top; z++)
		for (uint x = box.left; x < box.right; x++)
		{
			uint index = width * z + x;
			vertices[index].position.y += 5.0f;
		}

	UpdateNormal();

	graphics->GetDeviceContext()->UpdateSubresource
	(
		vertex_buffer->GetResource(),
		0,
		nullptr,
		vertices,
		sizeof(VertexTerrain) * geometry.GetVertexCount(),
		0
	);
}

void Terrain::RaiseCircle(const D3D11_BOX & box)
{
	auto vertices = geometry.GetVertexData();
	float distance = (box.right - box.left) * 0.5f;
	D3DXVECTOR2 mid = { (box.right + box.left) * 0.5f, (box.top + box.bottom) * 0.5f };
	
	for (uint z = box.bottom; z < box.top; z++)
		for (uint x = box.left; x < box.right; x++)
		{
			if (distance >= sqrt(pow(mid.x - x, 2) + pow(mid.y - z, 2))) {
				uint index = width * z + x;
				vertices[index].position.y += 5.0f;
			}
		}

	UpdateNormal();

	graphics->GetDeviceContext()->UpdateSubresource
	(
		vertex_buffer->GetResource(),
		0,
		nullptr,
		vertices,
		sizeof(VertexTerrain) * geometry.GetVertexCount(),
		0
	);
}

void Terrain::PaintColor(const D3D11_BOX & box)
{
	auto vertices = geometry.GetVertexData();

	for (uint z = box.bottom; z < box.top; z++)
		for (uint x = box.left; x < box.right; x++)
		{
			uint index = width * z + x;
			vertices[index].color = D3DXCOLOR(1, 0, 0, 1);
		}

	graphics->GetDeviceContext()->UpdateSubresource
	(
		vertex_buffer->GetResource(),
		0,
		nullptr,
		vertices,
		sizeof(VertexTerrain) * geometry.GetVertexCount(),
		0
	);
}

void Terrain::PaintTexture(const D3D11_BOX & box)
{
	auto vertices = geometry.GetVertexData();

	for (uint z = box.bottom; z < box.top; z++)
		for (uint x = box.left; x < box.right; x++)
		{
			uint index = width * z + x;
			vertices[index].alpha.a = 1.0f;
		}

	graphics->GetDeviceContext()->UpdateSubresource
	(
		vertex_buffer->GetResource(),
		0,
		nullptr,
		vertices,
		sizeof(VertexTerrain) * geometry.GetVertexCount(),
		0
	);
}

auto Terrain::Pick(D3DXVECTOR3 & position) -> const bool
{
	D3DXVECTOR3 org, dir;
	camera->GetWorldRay(org, dir);

	auto vertices = geometry.GetVertexData();
	auto indices = geometry.GetIndexData();

	for (uint i = 0; i < geometry.GetIndexCount(); i += 3)
	{
		auto index0 = indices[i + 0];
		auto index1 = indices[i + 1];
		auto index2 = indices[i + 2];

		auto v0 = vertices[index0].position;
		auto v1 = vertices[index1].position;
		auto v2 = vertices[index2].position;

		float u, v, distance;

		if (D3DXIntersectTri(&v0, &v1, &v2, &org, &dir, &u, &v, &distance))
		{
			//v0 += u * (v2 - v0) + v * (v1 - v0);
			//position = v0;

			position = org + distance * dir;

			return true;
		}
	}

	return false;
}

void Terrain::UpdateNormal()
{
	auto vertices = geometry.GetVertexData();
	auto indices = geometry.GetIndexData();

	for (uint i = 0; i < geometry.GetIndexCount() / 3; i++)
	{
		auto index0 = indices[i * 3 + 0];
		auto index1 = indices[i * 3 + 1];
		auto index2 = indices[i * 3 + 2];

		auto v0 = vertices[index0];
		auto v1 = vertices[index1];
		auto v2 = vertices[index2];

		auto d1 = v1.position - v0.position;
		auto d2 = v2.position - v0.position;

		D3DXVECTOR3 normal;
		D3DXVec3Cross(&normal, &d1, &d2);

		vertices[index0].normal += normal;
		vertices[index1].normal += normal;
		vertices[index2].normal += normal;
	}
}
