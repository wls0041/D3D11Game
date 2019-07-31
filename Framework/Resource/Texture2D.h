#pragma once
#include "Texture.h"

class Texture2D final : public Texture
{
public:
	static auto CreateTexture
	(
		ID3D11Device* device,
		ID3D11Texture2D** texture,
		const uint& width,
		const uint& height,
		const uint& channels,
		const uint& bpc,
		const uint& array_size,
		const DXGI_FORMAT& format,
		const uint& flags,
		std::vector<std::vector<std::byte>>& data
	) -> const bool;

	static auto CreateRenderTargetView
	(
		ID3D11Device* device,
		ID3D11Resource* resource,
		ID3D11RenderTargetView** render_target_view,
		const DXGI_FORMAT& format,
		const uint& array_size
	) -> const bool; //하나만 있어도 되서 **

	static auto CreateDepthStencilView
	(
		ID3D11Device* device,
		ID3D11Resource* resource,
		std::vector<ID3D11DepthStencilView*>& depth_stencil_views,
		const DXGI_FORMAT& format,
		const uint& array_size
	) -> const bool; //그림자 만들 때 cube를 만들고 면마다 뎁스를 만들어줘야 해서 vector 만듦

	static auto CreateShaderResourceView
	(
		ID3D11Device* device,
		ID3D11Resource* resource,
		ID3D11ShaderResourceView** shader_resource_view,
		const DXGI_FORMAT& format,
		const uint& array_size,
		const std::vector<std::vector<std::byte>>& data
	) -> const bool;

	static auto CreateUnorderedAccessView
	(
		ID3D11Device* device,
		ID3D11Resource* resource,
		ID3D11UnorderedAccessView** unordered_access_view,
		const DXGI_FORMAT& format,
		const uint& array_size
	) -> const bool;


public:
	Texture2D
	(
		class Context* context,
		const uint& width,
		const uint& height,
		const DXGI_FORMAT& format,
		const std::vector<std::vector<std::byte>>& mip_chain,
		const uint& flags = SRV
	);

	Texture2D
	(
		class Context* context,
		const uint& width,
		const uint& height,
		const DXGI_FORMAT& format,
		const std::vector<std::byte>& mip_level,
		const uint& flags = SRV
	);

	Texture2D
	(
		class Context* context,
		const uint& width,
		const uint& height,
		const DXGI_FORMAT& format,
		const uint& array_size,
		const uint& flags = SRV
	);

	Texture2D
	(
		class Context* context,
		const bool& is_generate_mipmap = true,
		const uint& flags = SRV
	);

	~Texture2D() = default;

	const bool CreateGpuResource() override;
};