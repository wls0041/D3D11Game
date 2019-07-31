#pragma once
#include "IResource.h"

enum TEXTURE_BIND_FLAGS : uint
{
	RTV = 1U << 0,
	DSV = 1U << 1,
	SRV = 1U << 2,
	UAV = 1U << 3,
};

class Texture : public IResource
{
public:
    typedef std::vector<std::byte> mip_level;

public:
    Texture(class Context* context);
    virtual ~Texture();

	const bool SaveToFile(const std::string &path) override;
	const bool LoadFromFile(const std::string &path) override;

    //Resource Views
    auto GetShaderResourceView() const -> ID3D11ShaderResourceView* { return shader_resource_view; }
    auto GetUnorderedAccessView() const -> ID3D11UnorderedAccessView* { return unordered_access_view; }
    auto GetRenderTargetView() const -> ID3D11RenderTargetView* { return render_target_view; }
    auto GetDepthStencilView(const uint& index = 0) const -> ID3D11DepthStencilView* { return index < static_cast<uint>(depth_stencil_views.size()) ? depth_stencil_views[index] : nullptr; }

    auto GetViewport() const -> const D3D11_VIEWPORT& { return viewport; }
    void SetViewport(const float& x, const float& y, const float& width, const float& height);

    auto GetFormat() const -> const DXGI_FORMAT& { return format; }
    void SetFormat(const DXGI_FORMAT& format) { this->format = format; }

    auto GetBPP() const -> const uint& { return bpp; }
    void SetBPP(const uint& bpp) { this->bpp = bpp; }

    auto GetBPC() const -> const uint& { return bpc; }
    void SetBPC(const uint& bpc) { this->bpc = bpc; }

    auto GetWidth() const -> const uint& { return width; }
    void SetWidth(const uint& width) { this->width = width; }

    auto GetHeight() const -> const uint& { return height; }
    void SetHeight(const uint& height) { this->height = height; }

    auto GetChannels() const -> const uint& { return channels; }
    void SetChannels(const uint& channels) { this->channels = channels; }

    auto IsGrayscale() const -> const bool { return is_gray_scale; }
    void SetGrayscale(const bool& is_gray_scale) { this->is_gray_scale = is_gray_scale; }

    auto IsTransparent() const -> const bool { return is_transparent; }
    void SetTransparent(const bool& is_transparent) { this->is_transparent = is_transparent; }

    auto IsGenerateMipChain() const -> const bool { return is_generate_mip_chain; }
    void SetGenerateMipChain(const bool& is_generate_mip_chain) { this->is_generate_mip_chain = is_generate_mip_chain; }

    void GetMipChain(std::vector<mip_level>* mip_chain);
    void SetMipChain(const std::vector<mip_level>& mip_chain) { this->mip_chain = mip_chain; }
    void ClearMipChain();

    auto GetMipLevel(const uint& level) -> mip_level*;
    auto AddMipLevel() -> mip_level* { return &mip_chain.emplace_back(mip_level()); }

    auto HasMipChain() -> const bool { return !mip_chain.empty(); }

    auto Load_Foreign(const std::string& path, const bool &is_generate_mip_chain) -> const bool;
	auto Load_Native(const std::string& path) -> const bool;

	auto GetByteCount() -> const uint;

	virtual const bool CreateGpuResource() = 0;

protected:
	auto GetChannelsFromFormat(const DXGI_FORMAT &format) -> const uint;

protected:
    class Graphics* graphics;

    uint bpp;
    uint bpc;
    uint width;
    uint height;
    uint channels;
    uint array_size;
    bool is_gray_scale;
    bool is_transparent;
    bool is_generate_mip_chain;
    DXGI_FORMAT format;
    D3D11_VIEWPORT viewport;
    std::vector<mip_level> mip_chain;

	TEXTURE_BIND_FLAGS bind_flags;

    ID3D11ShaderResourceView* shader_resource_view;
    ID3D11UnorderedAccessView* unordered_access_view;
    ID3D11RenderTargetView* render_target_view;
    std::vector<ID3D11DepthStencilView*> depth_stencil_views;
};