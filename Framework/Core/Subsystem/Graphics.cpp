#include "Framework.h"
#include "Graphics.h"

Graphics::Graphics()
    : device(nullptr)
    , device_context(nullptr)
    , swap_chain(nullptr)
    , rtv(nullptr)
    , dsv(nullptr)
    , clear_color(0xFF555566)
{
    ZeroMemory(&viewport, sizeof(D3D11_VIEWPORT));
}

Graphics::~Graphics()
{
    SAFE_RELEASE(dsv);
    SAFE_RELEASE(rtv);
    SAFE_RELEASE(device_context);
    SAFE_RELEASE(device);
    SAFE_RELEASE(swap_chain);
}

void Graphics::Initialize()
{
    DXGI_SWAP_CHAIN_DESC desc;
    ZeroMemory(&desc, sizeof(DXGI_SWAP_CHAIN_DESC));
    desc.BufferDesc.Width                   = 0;
    desc.BufferDesc.Height                  = 0;
    desc.BufferDesc.RefreshRate.Numerator   = 60;
    desc.BufferDesc.RefreshRate.Denominator = 1;
    desc.BufferDesc.Format                  = DXGI_FORMAT_R8G8B8A8_UNORM;
    desc.BufferDesc.ScanlineOrdering        = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
    desc.BufferDesc.Scaling                 = DXGI_MODE_SCALING_UNSPECIFIED;
    desc.BufferCount                        = 1;
    desc.SampleDesc.Count                   = 1;
    desc.SampleDesc.Quality                 = 0;
    desc.OutputWindow                       = Settings::Get().GetWindowHandle();
    desc.Windowed                           = TRUE;
    desc.SwapEffect                         = DXGI_SWAP_EFFECT_DISCARD;
    desc.BufferUsage                        = DXGI_USAGE_RENDER_TARGET_OUTPUT;

    std::vector<D3D_FEATURE_LEVEL> feature_levels
    {
        D3D_FEATURE_LEVEL_11_1,
        D3D_FEATURE_LEVEL_11_0,
        D3D_FEATURE_LEVEL_10_1,
        D3D_FEATURE_LEVEL_10_0,
        D3D_FEATURE_LEVEL_9_3,
        D3D_FEATURE_LEVEL_9_2,
        D3D_FEATURE_LEVEL_9_1,
    };

    auto hr = D3D11CreateDeviceAndSwapChain
    (
        nullptr,
        D3D_DRIVER_TYPE_HARDWARE,
        nullptr,
        D3D11_CREATE_DEVICE_BGRA_SUPPORT,
        feature_levels.data(),
        feature_levels.size(),
        D3D11_SDK_VERSION,
        &desc,
        &swap_chain,
        &device,
        nullptr,
        &device_context
    );
    assert(SUCCEEDED(hr));

    CreateBackBuffer
    (
        static_cast<uint>(Settings::Get().GetWidth()),
        static_cast<uint>(Settings::Get().GetHeight())
    );
}

void Graphics::Begin()
{
    device_context->RSSetViewports(1, &viewport);
    device_context->OMSetRenderTargets(1, &rtv, dsv);
    device_context->ClearRenderTargetView(rtv, clear_color);
}

void Graphics::End()
{
    auto hr = swap_chain->Present(1, 0);
    assert(SUCCEEDED(hr));
}

void Graphics::CreateBackBuffer(const uint & width, const uint & height)
{
    auto hr = swap_chain->ResizeBuffers
    (
        0,
        width,
        height,
        DXGI_FORMAT_UNKNOWN,
        0
    );
    assert(SUCCEEDED(hr));

    ID3D11Texture2D* back_buffer = nullptr;
    hr = swap_chain->GetBuffer
    (
        0,
        __uuidof(ID3D11Texture2D),//IID_ID3D11Texture2D,
        reinterpret_cast<void**>(&back_buffer)
    );
    assert(SUCCEEDED(hr));

    hr = device->CreateRenderTargetView(back_buffer, nullptr, &rtv);
    assert(SUCCEEDED(hr));

    SAFE_RELEASE(back_buffer);

    viewport.TopLeftX   = 0.0f;
    viewport.TopLeftY   = 0.0f;
    viewport.Width      = static_cast<float>(width);
    viewport.Height     = static_cast<float>(height);
    viewport.MaxDepth   = 1.0f;
    viewport.MinDepth   = 0.0f;
}
