#include "Framework.h"
#include "Texture2D.h"
#include "Core/D3D11/D3D11_Helper.h"

auto Texture2D::SaveTextureToFile(const std::string & path, const std::string & format, const uint & width, const uint & height, void * data) -> const std::string
{
	const auto texture_path = FileSystem::GetPathWithoutExtension(path) + "." + format;

	auto stream = std::make_shared<FileStream>(texture_path, StreamMode::Stream_Write);
	if (!stream->IsOpen()) return NOT_ASSIGNED_STR;
	
	if (height == 0) stream->Write(data, width);
	else stream->Write(data, width * height);

	return texture_path;
}

auto Texture2D::CreateTexture(ID3D11Device * device, ID3D11Texture2D ** texture, const uint & width, const uint & height, const uint & channels, const uint & bpc, const uint & array_size, const DXGI_FORMAT & format, const uint & flags, std::vector<std::vector<std::byte>>& data) -> const bool
{
    D3D11_TEXTURE2D_DESC desc;
    ZeroMemory(&desc, sizeof(D3D11_TEXTURE2D_DESC));
    desc.Width              = width;
    desc.Height             = height;
    desc.MipLevels          = data.empty() ? 1 : static_cast<uint>(data.size());
    desc.ArraySize          = array_size;
    desc.Format             = format;
    desc.SampleDesc.Count   = 1;
    desc.SampleDesc.Quality = 0;
    desc.Usage              = (flags & D3D11_BIND_RENDER_TARGET) || (flags & D3D11_BIND_DEPTH_STENCIL) || (flags & D3D11_BIND_UNORDERED_ACCESS) ? D3D11_USAGE_DEFAULT : D3D11_USAGE_IMMUTABLE;
    desc.BindFlags          = flags;
    desc.MiscFlags          = 0;
    desc.CPUAccessFlags     = 0;

    std::vector<D3D11_SUBRESOURCE_DATA> subresource_datas;
    auto mip_width  = width;
    auto mip_height = height;

    for (uint i = 0; i < static_cast<uint>(data.size()); i++)
    {
        if (data[i].empty())
        {
            LOG_ERROR_F("Mipmap %d has invalid data", i);
            return false;
        }

        D3D11_SUBRESOURCE_DATA subresource_data;
        subresource_data.pSysMem            = data[i].data();
        subresource_data.SysMemPitch        = mip_width * channels * (bpc / 8);
        subresource_data.SysMemSlicePitch   = 0;

        subresource_datas.emplace_back(subresource_data);

        mip_width   = Math::Max(mip_width / 2, 1U);//1U쓰는 이유. Max함수가 기본적으로 template이기 때문에 1을 쓰면 int로 잡혀 형변환 해야함. 
        mip_height  = Math::Max(mip_height / 2, 1U);// 아예 unsigned int로 나눠 형을 인식시킴
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
    desc.Format                         = format;
    desc.ViewDimension                  = (array_size == 1) ? D3D11_RTV_DIMENSION_TEXTURE2D : D3D11_RTV_DIMENSION_TEXTURE2DARRAY;
    desc.Texture2DArray.ArraySize       = array_size;//그냥 하나로만 만들면 array를 따로 만들어야 해서, array로 만들고 하나만 쓰거나 함
    desc.Texture2DArray.FirstArraySlice = 0;
    desc.Texture2DArray.MipSlice        = 0;
    
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
    D3D11_DEPTH_STENCIL_VIEW_DESC desc;
    ZeroMemory(&desc, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));
    desc.Format = format;
    desc.ViewDimension = (array_size == 1) ? D3D11_DSV_DIMENSION_TEXTURE2D : D3D11_DSV_DIMENSION_TEXTURE2DARRAY;
    desc.Texture2DArray.ArraySize = 1;
    desc.Texture2DArray.FirstArraySlice = 0;
    desc.Texture2DArray.MipSlice = 0;

    for (uint i = 0; i < array_size; i++)
    {
        desc.Texture2DArray.FirstArraySlice = i;
        const auto depth_stencil_view = &depth_stencil_views.emplace_back(nullptr);
        const auto result = device->CreateDepthStencilView(resource, &desc, depth_stencil_view);
        if (FAILED(result))
        {
            LOG_ERROR_F("Invalid parameter, failed to create ID3D11DepthStencilView, %s", D3D11_Helper::DXGI_ERROR_TO_STRING(result));
            return false;
        }
    }

    return true;
}

auto Texture2D::CreateShaderResourceView(ID3D11Device * device, ID3D11Resource * resource, ID3D11ShaderResourceView ** shader_resource_view, const DXGI_FORMAT & format, const uint & array_size, const std::vector<std::vector<std::byte>>& data) -> const bool
{
    D3D11_SHADER_RESOURCE_VIEW_DESC desc;
    ZeroMemory(&desc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
    desc.Format                         = format;
    desc.ViewDimension                  = (array_size == 1) ? D3D11_SRV_DIMENSION_TEXTURE2D : D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
    desc.Texture2DArray.ArraySize       = array_size;
    desc.Texture2DArray.FirstArraySlice = 0;
    desc.Texture2DArray.MipLevels       = data.empty() ? 1 : static_cast<uint>(data.size());
    desc.Texture2DArray.MostDetailedMip = 0;//가장 자세한 맵 = 원본.

    const auto result = device->CreateShaderResourceView(resource, &desc, shader_resource_view);
    if (FAILED(result))
    {
        LOG_ERROR_F("Invalid parameter, failed to create ID3D11ShaderResourceView, %s", D3D11_Helper::DXGI_ERROR_TO_STRING(result));
        return false;
    }

    return true;
}

auto Texture2D::CreateUnorderedAccessView(ID3D11Device * device, ID3D11Resource * resource, ID3D11UnorderedAccessView ** unordered_access_view, const DXGI_FORMAT & format, const uint & array_size) -> const bool
{
    D3D11_UNORDERED_ACCESS_VIEW_DESC desc;
    ZeroMemory(&desc, sizeof(D3D11_UNORDERED_ACCESS_VIEW_DESC));
    desc.Format                         = format;
    desc.ViewDimension                  = (array_size == 1) ? D3D11_UAV_DIMENSION_TEXTURE2D : D3D11_UAV_DIMENSION_TEXTURE2DARRAY;
    desc.Texture2DArray.ArraySize       = array_size;
    desc.Texture2DArray.FirstArraySlice = 0;
    desc.Texture2DArray.MipSlice        = 0;

    const auto result = device->CreateUnorderedAccessView(resource, &desc, unordered_access_view);
    if (FAILED(result))
    {
        LOG_ERROR_F("Invalid parameter, failed to create ID3D11UnorderedAccessView, %s", D3D11_Helper::DXGI_ERROR_TO_STRING(result));
        return false;
    }

    return true;
}

Texture2D::Texture2D(Context * context, const uint & width, const uint & height, const DXGI_FORMAT & format, const std::vector<std::vector<std::byte>>& mip_chain, const uint & flags)
    : Texture(context)
{
    this->width         = width;
    this->height        = height;
    this->format        = format;
    this->channels      = GetChannelsFromFormat(format);
    this->mip_chain     = mip_chain;
    this->bind_flags    = static_cast<TEXTURE_BIND_FLAGS>(flags);

    SetViewport(0.0f, 0.0f, static_cast<float>(width), static_cast<float>(height));

    CreateGpuResource();
}

Texture2D::Texture2D(Context * context, const uint & width, const uint & height, const DXGI_FORMAT & format, const std::vector<std::byte>& mip_level, const uint & flags)
    : Texture(context)
{
    this->width         = width;
    this->height        = height;
    this->format        = format;
    this->channels      = GetChannelsFromFormat(format);
    this->bind_flags    = static_cast<TEXTURE_BIND_FLAGS>(flags);
    this->mip_chain.emplace_back(mip_level);

    SetViewport(0.0f, 0.0f, static_cast<float>(width), static_cast<float>(height));

    CreateGpuResource();
}

Texture2D::Texture2D(Context * context, const uint & width, const uint & height, const DXGI_FORMAT & format, const uint & array_size, const uint & flags)
    : Texture(context)
{
    this->width         = width;
    this->height        = height;
    this->format        = format;
    this->channels      = GetChannelsFromFormat(format);
    this->array_size    = array_size;
    this->bind_flags    = static_cast<TEXTURE_BIND_FLAGS>(flags);

    SetViewport(0.0f, 0.0f, static_cast<float>(width), static_cast<float>(height));

    CreateGpuResource();
}

Texture2D::Texture2D(Context * context, const bool & is_generate_mipmap, const uint & flags)
    : Texture(context)
{
    this->is_generate_mip_chain = is_generate_mipmap;
    this->bind_flags            = static_cast<TEXTURE_BIND_FLAGS>(flags);
}

const bool Texture2D::CreateGpuResource()
{
    uint final_flags = 0U;

    final_flags |= (bind_flags & SRV) ? D3D11_BIND_SHADER_RESOURCE  : 0U;
    final_flags |= (bind_flags & RTV) ? D3D11_BIND_RENDER_TARGET    : 0U;
    final_flags |= (bind_flags & DSV) ? D3D11_BIND_DEPTH_STENCIL    : 0U;
    final_flags |= (bind_flags & UAV) ? D3D11_BIND_UNORDERED_ACCESS : 0U;

    auto buffer_format  = format;
    auto dsv_format     = format;
    auto srv_format     = format;

    if (format == DXGI_FORMAT_D32_FLOAT)
    {
        buffer_format   = DXGI_FORMAT_R32_TYPELESS; //srv, rtv는 어떤 형식이 들어올지 모르기 때문에 typeless
        dsv_format      = DXGI_FORMAT_D32_FLOAT;	//깊이는 d32. 이것만으로는 화면에 출력 못함
        srv_format      = DXGI_FORMAT_R32_FLOAT;	//색이 표현되야 하기 때문에 r32
    }

    ID3D11Texture2D* texture = nullptr;
    auto result = CreateTexture
    (
        graphics->GetDevice(),
        &texture,
        width,
        height,
        channels,
        bpc,
        array_size,
        buffer_format,
        final_flags,
        mip_chain
    );

    if (!result)
        return false;

    if (bind_flags & RTV)
    {
        result = CreateRenderTargetView
        (
            graphics->GetDevice(),
            texture,
            &render_target_view,
            buffer_format,
            array_size
        );

        if (!result)
        {
            SAFE_RELEASE(texture);
            return false;
        }
    }

    if (bind_flags & DSV)
    {
        result = CreateDepthStencilView
        (
            graphics->GetDevice(),
            texture,
            depth_stencil_views,
            dsv_format,
            array_size
        );

        if (!result)
        {
            SAFE_RELEASE(texture);
            return false;
        }
    }

    if (bind_flags & SRV)
    {
        result = CreateShaderResourceView
        (
            graphics->GetDevice(),
            texture,
            &shader_resource_view,
            srv_format,
            array_size,
            mip_chain
        );

        if (!result)
        {
            SAFE_RELEASE(texture);
            return false;
        }
    }

    if (bind_flags & UAV)
    {
        result = CreateUnorderedAccessView
        (
            graphics->GetDevice(),
            texture,
            &unordered_access_view,
            buffer_format,
            array_size
        );

        if (!result)
        {
            SAFE_RELEASE(texture);
            return false;
        }
    }

    SAFE_RELEASE(texture);
    return true;
}