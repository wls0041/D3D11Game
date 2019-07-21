#include "Framework.h"
#include "Texture.h"

Texture::Texture(Context * context)
    : IResource(context)
    , bpp(0)
    , bpc(8)
    , width(0)
    , height(0)
    , channels(0)
    , array_size(0)
    , is_gray_scale(false)
    , is_transparent(false)
    , is_generate_mip_chain(true)
    , format(DXGI_FORMAT_R8G8B8A8_UNORM)
    , shader_resource_view(nullptr)
    , unordered_access_view(nullptr)
    , render_target_view(nullptr)
{
    graphics = context->GetSubsystem<Graphics>();

    ZeroMemory(&viewport, sizeof(D3D11_VIEWPORT));
}

Texture::~Texture()
{
    SAFE_RELEASE(shader_resource_view);
    SAFE_RELEASE(unordered_access_view);
    SAFE_RELEASE(render_target_view);

    for (auto depth_stencil_view : depth_stencil_views)
        SAFE_RELEASE(depth_stencil_view);
}

void Texture::SetViewport(const float & x, const float & y, const float & width, const float & height)
{
    viewport.TopLeftX   = x;
    viewport.TopLeftY   = y;
    viewport.Width      = width;
    viewport.Height     = height;
    viewport.MinDepth   = 0.0f;
    viewport.MaxDepth   = 1.0f;
}

void Texture::GetMipChain(std::vector<mip_level>* mip_chain)
{
}

void Texture::ClearMipChain()
{
}

auto Texture::GetMipLevel(const uint & level) -> mip_level *
{
    return nullptr;
}

auto Texture::Load(const std::string & path) -> const bool
{
    return false;
}

auto Texture::Serialize(const std::string & path) -> const bool
{
    return false;
}

auto Texture::Deserialize(const std::string & path) -> const bool
{
    return false;
}
