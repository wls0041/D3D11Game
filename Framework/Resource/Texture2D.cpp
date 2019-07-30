#include "Framework.h"
#include "Texture2D.h"
#include "Core/D3D11/D3D11_Helper.h"

auto Texture2D::CreateTexture(ID3D11Device * device, ID3D11Texture2D ** texture, const uint & width, const uint & height, const uint & channels, const uint & bpc, const uint & array_size, const DXGI_FORMAT & format, const uint & flags, std::vector<std::vector<std::byte>>& data) -> const bool
{
	D3D11_TEXTURE2D_DESC desc;
	ZeroMemory(&desc, sizeof(D3D11_TEXTURE2D_DESC));
	desc.Width = width;
	desc.Height = height;
	desc.MipLevels = data.empty() ? 1 : static_cast<uint>(data.size());
	desc.ArraySize = array_size;
	desc.Format = format;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Usage = (flags & D3D11_BIND_RENDER_TARGET) || (flags & D3D11_BIND_DEPTH_STENCIL) || (flags & D3D11_BIND_UNORDERED_ACCESS) ? D3D11_USAGE_DEFAULT : D3D11_USAGE_IMMUTABLE;
	desc.BindFlags = flags;
	desc.MiscFlags = 0;
	desc.CPUAccessFlags = 0;

	std::vector<D3D11_SUBRESOURCE_DATA> subresource_datas;
	auto mip_width = width;
	auto mip_height = height;

	for (uint i = 0; i < static_cast<uint>(data.size()); i++)
	{
		if (data[i].empty())
		{
			LOG_ERROR_F("Mipmap %d has invalid data", i);
			return false;
		}

		D3D11_SUBRESOURCE_DATA subresource_data;
		subresource_data.pSysMem = data[i].data();
		subresource_data.SysMemPitch = mip_width * channels * (bpc / 8);
		subresource_data.SysMemSlicePitch = 0;

		subresource_datas.emplace_back(subresource_data);

		mip_width = Math::Max(mip_width / 2, 1U); //1U쓰는 이유. Max함수가 기본적으로 template이기 때문에 1을 쓰면 int로 잡혀 형변환 해야함. 
		mip_height = Math::Max(mip_height / 2, 1U); // 아예 unsigned int로 나눠 형을 인식시킴
	}

	const auto result = device->CreateTexture2D(&desc, subresource_datas.data(), texture);
	if (FAILED(result))
	{
		LOG_ERROR_F("Invalid parameter, failed to create ID3D11Texture2D, %s", D3D11_Helper::DXGI_ERROR_TO_STRING(result));
		return false;
	}

	return true;
}

auto Texture2D::CreateRenderTargetView(ID3D11Device * device, ID3D11Resource * resource, ID3D11RenderTargetView ** render_target_view, const DXGI_FORMAT & format, const uint & array_size) -> const bool
{
	D3D11_RENDER_TARGET_VIEW_DESC desc;
	ZeroMemory(&desc, sizeof(D3D11_RENDER_TARGET_VIEW_DESC));
	desc.Format = format;
	desc.ViewDimension = (array_size == 1) ? D3D11_RTV_DIMENSION_TEXTURE2D : D3D11_RTV_DIMENSION_TEXTURE2DARRAY;
	desc.Texture2DArray.ArraySize = array_size; //그냥 하나로만 만들면 array를 따로 만들어야 해서, array로 만들고 하나만 쓰거나 함
	desc.Texture2DArray.FirstArraySlice = 0;
	desc.Texture2DArray.MipSlice = 0;

	const auto result = device->CreateRenderTargetView(resource, &desc, render_target_view);
	if (FAILED(result))
	{
		LOG_ERROR_F("Invalid parameter, failed to create ID3D11RenderTargetView, %s", D3D11_Helper::DXGI_ERROR_TO_STRING(result));
		return false;
	}

	return true;
}

auto Texture2D::CreateDepthStencilView(ID3D11Device * device, ID3D11Resource * resource, std::vector<ID3D11DepthStencilView*>& depth_stencil_views, const DXGI_FORMAT & format, const uint & array_size) -> const bool
{
	return false;
}

auto Texture2D::CreateShaderResourceView(ID3D11Device * device, ID3D11Resource * resource, ID3D11ShaderResourceView ** shader_resource_view, const DXGI_FORMAT & format, const uint & array_size, const std::vector<std::vector<std::byte>>& data) -> const bool
{
	return false;
}

auto Texture2D::CreateUnorderedAccessView(ID3D11Device * device, ID3D11Resource * resource, ID3D11UnorderedAccessView ** unordered_access_view, const DXGI_FORMAT & format, const uint & array_size) -> const bool
{
	return false;
}
