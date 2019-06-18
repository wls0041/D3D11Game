#pragma once
#include "ISubsystem.h"

class Graphics final : public ISubsystem
{
public:
    Graphics(class Context *context);
    ~Graphics();

    auto GetDevice() const -> ID3D11Device* { return device; }
    auto GetDeviceContext() const -> ID3D11DeviceContext* { return device_context; }

	auto Initialize() -> const bool override;
    
    void Begin();
    void End();

private:
    void CreateBackBuffer(const uint& width, const uint& height);

private:
    ID3D11Device* device;
    ID3D11DeviceContext* device_context;
    IDXGISwapChain* swap_chain;

    ID3D11RenderTargetView* rtv;
    ID3D11DepthStencilView* dsv;
    D3D11_VIEWPORT viewport;
    D3DXCOLOR clear_color;
};